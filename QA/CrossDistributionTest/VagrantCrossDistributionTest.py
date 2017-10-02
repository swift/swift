#!/usr/bin/env python

# Required Python libraries:
#
# pip install python-vagrant tqdm
#
# Purpose of this script:
#
# This script tests
# a) InstallSwiftDependencies.sh installs all required packages for Swift and Swiften on system and
# b) all Swift projects build successfully on the system and their tests pass.

from tqdm import tqdm
from time import sleep
import sys
import vagrant

testSystems = [
    "bento/ubuntu-16.04",
    "bento/ubuntu-15.04",
    "bento/debian-8.5",
    "bento/opensuse-leap-42.1",
    "bento/fedora-24",
    "Sabayon/spinbase-amd64"
]

progressBar = tqdm(testSystems)

successfulSystems = []
failedSystems = []

for testSystem in progressBar :
    v = vagrant.Vagrant(quiet_stdout=False)
    try :
        progressBar.set_description("create Vagrantfile for %s" % testSystem)
        with open('Vagrantfile', 'w') as vagrantfile :
            vagrantfile.write("""
Vagrant.configure("2") do |config|
    config.vm.box = "%s"

    config.ssh.insert_key = false

    config.vm.provider "virtualbox" do |v|
        v.memory = 4096
        v.cpus = 2
    end

    # This is needed because Fedora only comes with Python 3 and Ansible needs Python 2
    # on the target system for some features.
    if config.vm.box.include? "fedora"
        config.vm.provision "shell", inline: "sudo dnf install -y python2 python2-dnf libselinux-python"
    end

    if config.vm.box.include? "Sabayon"
        config.vm.provision "shell", inline: "sudo equo update && sudo env ACCEPT_LICENSE=* equo install dev-vcs/git sys-apps/lsb-release"
    end

    config.vm.synced_folder "../..", "/home/vagrant/swift-host", type: "rsync"
    config.vm.synced_folder ".", "/vagrant", type: "rsync"

    config.vm.provision "ansible" do |ansible|
        #ansible.verbose = "vvv"
        ansible.playbook = "playbook.yml"
    end
end""" % testSystem)

        progressBar.set_description("vagrant up")
        v.up(testSystem, provision=True )

        progressBar.set_description("start building swift on %s" % testSystem)
        # unset QTDIR is needed, because Fedora 24 sets QTDIR to Qt 3, even though Qt 5 is installed.
        # SCons will pick up the Qt installation from QTDIR if QTDIR is set.
        v._call_vagrant_command(["ssh", "-c", "cd /home/vagrant/swift && unset QTDIR && ./scons test=unit -j 2"])

        progressBar.set_description("vagrant destory %s" % testSystem)
        v.destroy()
        successfulSystems.append(testSystem)
    except :
        e = sys.exc_info()[0]
        print("Exception: %s" % e)
        progressBar.set_description("vagrant destory %s" % testSystem)
        v.destroy()
        failedSystems.append(testSystem)

for system in successfulSystems:
    print("SUCCESS: %s" % system)
for system in failedSystems:
    print("FAILED: %s" % system)

exit(len(failedSystems))