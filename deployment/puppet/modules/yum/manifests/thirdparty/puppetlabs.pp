#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

class yum::thirdparty::puppetlabs($repo_server = 'servilinux.hi.inet') {

  yumrepo { 'puppetlabs-products':
    descr    => "Puppet Labs Products - $operatingsystemmajrelease",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/puppet/yum/el/$operatingsystemmajrelease/products/\$basearch/",
  }

  yumrepo { 'puppetlabs-deps':
    descr    => "Puppet Labs Dependencies - $operatingsystemmajrelease",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/puppet/yum/el/$operatingsystemmajrelease/dependencies/\$basearch/",
  }
}
