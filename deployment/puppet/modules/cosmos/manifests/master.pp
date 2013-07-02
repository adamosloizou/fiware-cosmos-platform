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

class cosmos::master inherits cosmos::base {
  include ssh_keys, ambari, mysql, cosmos::api

  # class { 'mysql': }

  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' }
  }

  mysql::server::config { 'basic_config':
    settings => {
      'mysqld' => {
        'bind-address' => '0.0.0.0',
        #'read-only'    => true,
      }#,
      #'client' => {
      #  'port' => '3306'
      #}
    },
  }

  # database_user{ 'cosmos@%':
  #   ensure        => present,
  #   password_hash => mysql_password('cosmos'),
  #   require       => Class['mysql::server'],
  # }

  # database_grant { 'cosmos@%/cosmosdb':
  #   privileges => ['all'] ,
  # }

  #host { 'slave1':
  #  ip => '192.168.10.21',
  #  host_aliases => 'cosmos.slave1',
  #}
  #
  #host { 'slave2':
  #  ip => '192.168.10.22',
  #  host_aliases => 'cosmos.slave2',
  #}
}
