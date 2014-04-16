#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

class cosmos::api inherits cosmos::params {

  include cosmos::setup

  exec { 'install-cosmos-api':
    command   => "yum localinstall -d 0 -e 0 -y ${cosmos_stack_repo_path}/cosmos-api-*.rpm",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
  }

  exec { 'install-cosmos-admin':
    command   => "yum localinstall -d 0 -e 0 -y ${cosmos_stack_repo_path}/cosmos-admin-*.rpm",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
  }

  service { 'cosmos-api':
    ensure     => 'running',
    enable     => true,
    hasstatus  => true,
    hasrestart => true,
  }

  Exec['install-cosmos-api'] -> Service['cosmos-api']

  Class['cosmos::setup']                      ~> Service['cosmos-api']
  File['cosmos-api.conf', 'logback.conf']     ~> Service['cosmos-api']

  anchor { 'cosmos::api::begin': }
    -> Class['cosmos::setup']
    -> anchor { 'cosmos::api::end': }
}
