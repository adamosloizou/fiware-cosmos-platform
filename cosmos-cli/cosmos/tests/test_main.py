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

from testfixtures import TempDirectory

import cosmos.main as main

class MainTest(unittest.TestCase):

    def setUp(self):
        self.parser = main.build_argument_parser()

    def test_exit_on_wrong_arguments(self):
        self.assertRaises(SystemExit, self.parser.parse_args, 
                          "wrong arguments".split())

    def test_parse_correct_arguments(self):
        self.assertValidArguments("configure")
        self.assertValidArguments("list")
        self.assertValidArguments("show 12345678901234567890123456789012")
        self.assertValidArguments("terminate 12345678901234567890123456789012")
        self.assertValidArguments("create --name foo --size 3")
        self.assertValidArguments("ssh 12345678901234567890123456789012")
        self.assertValidArguments("ls /tmp")
        self.assertValidArguments("rm /tmp")
        self.assertValidArguments("get /tmp/file.csv ../Downloads/")
        with TempDirectory() as tmp:
            path = tmp.write('local.txt', 'content')
            self.assertValidArguments("put %s /remote/path/" % path)

    def assertValidArguments(self, arguments):
        self.assertIsNotNone(self.parser.parse_args(arguments.split()))

