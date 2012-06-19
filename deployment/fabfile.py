"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import os
import json
from StringIO import StringIO

from fabric.api import run, execute, sudo, put, cd, env
from fabric.contrib import files
import fabric.context_managers as ctx
from fabric.decorators import roles, task
from mako.template import Template

import common
import hadoop_install
import hue_deployment

CONFIG = json.loads(open(env.config, 'r').read())
env.roledefs = CONFIG['hosts']
BASEPATH = os.path.dirname(os.path.realpath(__file__))

@task
def deploy(dependenciespath, thrift_tar, jdk_rpm):
    """
    Deploys all the necessary components to get a running Cosmos cluster
    """
    execute(deploy_jdk, os.path.join(dependenciespath, jdk_rpm))
    deploy_cdh()
    execute(deploy_mongo)
    execute(deploy_hue, os.path.join(dependenciespath, thrift_tar))
    execute(deploy_sftp)
    
@task
@roles('frontend')
def add_test_setup():
    """
    Installs any test-specific setup components
    """
    files_to_delete = put(os.path.join(BASEPATH, '../cosmos/tests/testUser.json'), 'testUser.json')
    with cd('/usr/share/hue'):
        run('build/env/bin/hue loaddata ~/testUser.json')
    for f in files_to_delete:
        run('rm %s' % f)
@task
@roles('namenode', 'jobtracker', 'frontend', 'datanodes', 'tasktrackers')
def deploy_jdk(jdkpath):
    if not common.has_package('jdk'):
        put(jdkpath, 'jdk.rpm')
        run('rpm -ihv jdk.rpm')

@task
@roles('frontend')
def deploy_hue(thrift_tarpath):
    """
    Deploys the HUE frontend from Cloudera, plus our fixes and our app
    """
    common.install_dependencies(['mysql', 'git'])
    hue_deployment.install_and_patch_hue(CONFIG)
    execute(hue_deployment.install_hue_plugins)
    hue_deployment.install_thrift(thrift_tarpath)
    hue_deployment.install_cosmos_app()
    hue_deployment.start_daemons()
    hue_deployment.cleanup()

@task
@roles('frontend')
def deploy_sftp():
    """
    Deploys the SFTP server as a Java JAR and starts it
    """
    injection_exec = 'injection-server-{0}.jar'.format(CONFIG['version'])
    injection_jar = os.path.join(BASEPATH, CONFIG['injection_path'], 'target',
                                 injection_exec)
    exec_path = os.path.join('~', 'injection', injection_exec)

    put(injection_jar, exec_path)

    injection_conf = StringIO()
    template = Template(filename = os.path.join(BASEPATH,'templates/injection.conf.mako'))
    injection_conf.write(template.render(
            namenode=CONFIG['hosts']['namenode'][0]))
    put(injection_conf, "/etc/injection.cfg")
    symlink = "/usr/local/injection-server"
    if not files.exists(symlink):
        run("ln -s {0} {1}".format(exec_path, symlink))
    logfile = "/var/log/injection/server.log"
    if not files.exists(logfile):
        run("mkdir -p /var/log/injection")
        run("echo '' >> {0}".format(logfile))
    pidfile = "/var/run/injection/server.pid"
    if not files.exists(pidfile):
        run("mkdir -p /var/run/injection")
        run("echo '' >> {0}".format(pidfile))
    put(os.path.join(BASEPATH, "templates/injection.init.d"), "/etc/init.d/injection")
    with ctx.settings(warn_only=True):
        start = run("/etc/init.d/injection start", pty=False)
        if start.failed:
            run("/etc/init.d/injection restart", pty=False)
      
@task
def deploy_cdh():
    """Deploys the Cloudera Distribution for Hadoop"""
    execute(hadoop_install.install_cdh, CONFIG)
    execute(hadoop_install.create_hadoop_dirs, CONFIG)
    execute(hadoop_install.configure_hadoop, CONFIG)
    execute(hadoop_install.deploy_namenode_daemon)
    execute(hadoop_install.deploy_jobtracker_daemon)
    execute(hadoop_install.deploy_datanode_daemon)
    execute(hadoop_install.deploy_tasktracker_daemon)
  
@task
@roles('mongo')  
def deploy_mongo():
    """Install the latest MongoDB distribution"""
    with cd('/etc/yum.repos.d'):
        if not files.exists('10gen.repo'):
            put(os.path.join(BASEPATH, 'templates/10gen.repo'), '10gen.repo')
    run('yum -y install mongo-10gen mongo-10gen-server')
    run('service mongod start')
    run('chkconfig mongod on')
    run("mongo --eval \"db.adminCommand('listDatabases').databases.forEach("
        "function (d) { "
        "  if (d.name != 'local' && d.name != 'admin' && d.name != 'config')"
        "    db.getSiblingDB(d.name).dropDatabase();"
        "})\"")

@task
def deploy_models():
    """
    Deploys the preconfigured statistical models

    This function is not ready for production. It is only here as a general
    idea on what is needed to deploy a model, but we currently have no models
    to deploy
    """
    model_paths = [] # Dummy variable, this should be part of
                     # the configuration or similar
    for model in model_paths:
        put(model)
        sudo('hadoop dfs -put {0} /models/{0}'.format(model))
        run('rm %s' % model)
