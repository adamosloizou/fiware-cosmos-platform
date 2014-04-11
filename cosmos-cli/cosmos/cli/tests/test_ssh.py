# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
import unittest


from mock import MagicMock, patch
from os import path

from cosmos.cli.ssh import ssh_cluster
from cosmos.cli.util import ExitWithError
from cosmos.cli.tests.mock_home_dir import mock_home
from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import collect_outputs, mock_response


PROVISIONING = {
    'state': 'provisioning'
}
RUNNING = {
    'state': 'running',
    'master': {
        'ipAddress': '192.168.20.18'
    },
    'blockedPorts': [1, 2, 3]
}
PROFILE = {
    'handle': 'user1'
}


class SshCommandTest(unittest.TestCase):

    def setUp(self):
        self.config = MagicMock()
        self.config.ssh_command = 'ssh'
        self.config.api_url = 'http://host:port/some/api/v1'
        self.config.ssh_key = ''

    def test_refuse_ssh_clusters_in_post_running_states(self):
        for state in ['terminating', 'terminated', 'failed']:
            response = mock_response(json=dict(state=state))
            with patch('requests.get', MagicMock(return_value=response)), \
                    mock_home():
                self.assertRaisesRegexp(
                    ExitWithError, 'cluster in %s state' % state,
                    ssh_cluster, 'cluster1', self.config)

    def test_refuse_ssh_nonexistent_cluster(self):
        response = mock_response(status_code=404)
        with patch('requests.get', MagicMock(return_value=response)), \
                mock_home():
            self.assertRaisesRegexp(
                ExitWithError, 'cluster1 does not exist',
                ssh_cluster, 'cluster1', self.config)

    def test_refuse_ssh_cluster_for_error_statuses(self):
        response = mock_response(status_code=500)
        with patch('requests.get', MagicMock(return_value=response)), \
                mock_home():
            self.assertRaisesRegexp(
                ResponseError, 'Cannot get cluster details',
                ssh_cluster, 'cluster1', self.config)

    def test_ssh_clusters_in_running_state(self):
        response = mock_response(json=MagicMock(side_effect=[RUNNING, PROFILE]))
        call_mock = MagicMock(return_value=0)
        with patch('requests.get', MagicMock(return_value=response)), \
                patch('subprocess.call', call_mock), mock_home():
            self.assertEquals(0, ssh_cluster('cluster1', self.config))
        call_mock.assert_called_with(['ssh', '192.168.20.18',
                                      '-l', 'user1',
                                      '-o', 'UserKnownHostsFile=/dev/null',
                                      '-o', 'StrictHostKeyChecking=no',
                                      '-L1:192.168.20.18:1',
                                      '-L2:192.168.20.18:2',
                                      '-L3:192.168.20.18:3'])
        self.assertEmptyIterator(response.json.side_effect)

    def test_ssh_cluster_with_custom_key(self):
        self.config.ssh_key = '~/.ssh/aws_key'
        response = mock_response(json=MagicMock(side_effect=[RUNNING, PROFILE]))
        call_mock = MagicMock(return_value=0)
        with patch('requests.get', MagicMock(return_value=response)), \
                patch('subprocess.call', call_mock), mock_home():
            self.assertEquals(0, ssh_cluster('cluster1', self.config))
        expected_key_path = path.expanduser(self.config.ssh_key)
        call_mock.assert_called_with(['ssh', '192.168.20.18',
                                      '-l', 'user1',
                                      '-o', 'UserKnownHostsFile=/dev/null',
                                      '-o', 'StrictHostKeyChecking=no',
                                      '-L1:192.168.20.18:1',
                                      '-L2:192.168.20.18:2',
                                      '-L3:192.168.20.18:3',
                                      '-i', expected_key_path])

    def test_exit_with_error_when_ssh_command_is_not_executable(self):
        response = mock_response(json=MagicMock(side_effect=[RUNNING, PROFILE]))
        call_mock = MagicMock(side_effect=OSError())
        with patch('requests.get', MagicMock(return_value=response)), \
                patch('subprocess.call', call_mock), mock_home():
            self.assertRaisesRegexp(ExitWithError, 'Cannot execute',
                                    ssh_cluster, 'cluster1', self.config)
        self.assertEmptyIterator(response.json.side_effect)

    def test_poll_service_until_provisioned(self):
        response = mock_response(json=MagicMock(side_effect=[
            PROVISIONING, PROVISIONING, PROVISIONING, RUNNING, PROFILE]))
        call_mock = MagicMock(return_value=0)
        with mock_home() as home, collect_outputs(), \
                patch('requests.get', MagicMock(return_value=response)), \
                patch('subprocess.call', call_mock), patch('time.sleep'):
            self.assertEquals(0, ssh_cluster('cluster1', self.config))
            self.assertEquals(home.read_last_cluster(), "cluster1")
        self.assertEmptyIterator(response.json.side_effect)

    def assertEmptyIterator(self, iterator):
        try:
            value = iterator.next()
            self.fail('Expected empty iterator but "%s" was not consumed' %
                      value)
        except StopIteration:
            return

