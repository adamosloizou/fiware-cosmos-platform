# -*- mode: ruby -*-
# vi: set ft=ruby :
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

Vagrant.configure("2") do |config|

  config.vm.box = "CentOS-6.4-x86_64-vzkernel-v20131211"
  config.vm.box_url = "http://cosmos10.hi.inet:8080/vagrant-boxes/CentOS-6.4-x86_64-vzkernel-v20131211.box"

  config.vm.provision :puppet, :module_path =>  "modules" do |puppet|
    puppet.manifests_path = "target/dist/puppet/modules/cosmos/manifests"
    puppet.manifest_file  = "site.pp"
    puppet.module_path = ["deployment/puppet/modules", "deployment/puppet/modules_third_party", "target/dist/rpms"]
    puppet.working_directory = "/tmp/vagrant-puppet"
    puppet.options = "--environment vagrant --hiera_config /tmp/vagrant-puppet/manifests/hiera.yaml --manifestdir /tmp/vagrant-puppet/manifests"
  end

  config.vm.define :store1 do |config|
    config.vm.network :private_network, ip: "192.168.11.21"
    config.vm.hostname = "store1.vagrant"
  end

  config.vm.define :store2 do |config|
    config.vm.network :private_network, ip: "192.168.11.22"
    config.vm.hostname = "store2.vagrant"
  end

  config.vm.define :compute1 do |config|
    config.vm.network :private_network, ip: "192.168.11.23"
    config.vm.hostname = "compute1.vagrant"
  end

  config.vm.define :compute2 do |config|
    config.vm.network :private_network, ip: "192.168.11.24"
    config.vm.hostname = "compute2.vagrant"
  end

  config.vm.define :master do |config|
    config.vm.box = "CentOS-6.4-x86_64-v20131211"
    config.vm.box_url = "http://cosmos10.hi.inet:8080/vagrant-boxes/CentOS-6.4-x86_64-v20131211.box"
    config.vm.network :private_network, ip: "192.168.11.10"
    config.vm.hostname = "master.vagrant"
    config.vm.network :forwarded_port, host: 9080, guest: 80
    config.vm.network :forwarded_port, host: 9443, guest: 443
    config.vm.network :forwarded_port, host: 8080, guest: 8080
    config.vm.network :forwarded_port, host: 8000, guest: 8000
    config.vm.provider :virtualbox do |vb|
      vb.customize ["modifyvm", :id, "--nicpromisc2", "allow-all"]
      vb.customize ["modifyvm", :id, "--memory", "480"]
      vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
    end
  end

  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--nicpromisc2", "allow-all"]
    vb.customize ["modifyvm", :id, "--memory", "1024"]
  end
end
