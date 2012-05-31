"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import json
from StringIO import StringIO

from fabric.api import run, execute, sudo, put, cd, env
from fabric.contrib import files
from fabric.decorators import roles, task
from mako.template import Template

import common
import hadoop_install
import hue_deployment

DEFAULT_CONFIG = 'staging.json'

CONFIG = json.loads(open(DEFAULT_CONFIG, 'r').read())
env.user = CONFIG['user'] 
env.password = CONFIG['password']
env.roledefs = CONFIG['hosts']

@task
def deploy():
    """
    Deploys all the necessary components to get a running Cosmos cluster
    """
    deploy_cdh()
    deploy_hue()
    deploy_models()
    deploy_sftp()

@task
@roles('frontend')
def deploy_hue(thrift_tarpath='~/install-dependencies/thrift-0.8.0.tar.gz'):
    """
    Deploys the HUE frontend from Cloudera, plus our fixes and our app
    """
    execute(common.check_dependencies, ['mysql', 'git'])
    execute(hue_deployment.patch_hue, CONFIG)
    execute(hue_deployment.install_thrift, thrift_tarpath)
    execute(hue_deployment.install_cosmos_app)
    execute(hue_deployment.start_daemons)
    execute(hue_deployment.cleanup)

@task
@roles('frontend')
def deploy_sftp():
    """
    Deploys the SFTP server as a Java JAR and starts it
    """
    with cd("/root/injection"):
        put("target/injection*.jar")
        injection_conf = StringIO()
        template = Template(filename='injection.conf.mako')
        injection_conf.write(template.render(
                namenode=CONFIG['hosts']['namenode'][0]))
        put(injection_conf, "/etc/injection.cfg")
        put("templates/injection.init.d", "/etc/init.d/injection")
        #run("update_config ?")
    run("/etc/init.d/injection start")
      
@task
def deploy_cdh():
    """Deploys the Cloudera Distribution for Hadoop"""
    execute(hadoop_install.install_cdh)
    execute(hadoop_install.create_hadoop_dirs)
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
            put('templates/10gen.repo', '10gen.repo')
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
    modelpaths = [] # Dummy variable, this should be part of
                    # the configuration or similar
    for model in modelpaths:
        put(model)
        sudo('hadoop dfs -put {0} /models/{0}'.format(model))
        run('rm %s' % model)
