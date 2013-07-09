class openvz  (
    $vz_utils_repo,   
    $vz_kernel_repo,
) inherits openvz::params {
  include openvz::sysctl

  package { $openvz::params::packages : 
    ensure    => installed,
  }

  service { $openvz::params::servicename :
    ensure    => running,
    enable    => true,
  }

  file { 'vz.conf' :
    path      => "$openvz::params::basedir/vz.conf",
    ensure    => present,
    source    => "puppet:///modules/openvz/vz.conf",
    notify    => Service[$openvz::params::servicename],
  }
  
  file { 'cosmos-openvz-repo' : 
    ensure    => present,
    owner     => 'root',
    group     => 'root',
    mode      => '0644',
    path      => "/etc/yum.repos.d/cosmos-openvz.repo",
    content   => template('openvz/cosmos-openvz.repo.erb'),
  }

  File['cosmos-openvz-repo']
    -> Package[$openvz::params::packages]
    -> File['vz.conf']
    -> Class['openvz::sysctl']
    -> Service[$openvz::params::servicename]
}
