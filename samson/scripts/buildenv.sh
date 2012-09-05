#
# Script to setup the build environment
#

if [ -z $SAMSON_VERSION ]; then
  SAMSON_VERSION=HEAD
fi
if [ -z $SAMSON_TEST_HOST ]; then
  SAMSON_TEST_HOST=localhost
fi
if [ -z $SAMSON_WORKER_PORT_ENV ]; then
  SAMSON_WORKER_PORT_ENV=1324
fi
if [ -z $SAMSON_WORKER_WEB_PORT_ENV ]; then
  SAMSON_WORKER_WEB_PORT_ENV=1202
fi
if [ -z $SAMSON_LOGQUERY_PORT_ENV ]; then
  SAMSON_LOGQUERY_PORT_ENV=6000
fi
if [ -z $SAMSON_LOG_PORT_ENV ]; then
  SAMSON_LOG_PORT_ENV=6001
fi
if [ -z $ZOOKEEPER_SERVER ]; then
  ZOOKEEPER_SERVER=localhost
fi
if [ -z $ZOOKEEPER_PORT ]; then
  ZOOKEEPER_PORT=2181
fi

#SAMSON_LOG_LEVELS="-t 15,20,22,31,32,33,34,35,210"

# Base directory for installation
if [ -z $1 ];
then
    SAMSON_ROOT=$HOME/.samson-$SAMSON_VERSION
else
    SAMSON_ROOT=$1
fi

echo "Using $SAMSON_ROOT as the base directory"

if [ ! -d $SAMSON_ROOT ];
then
    mkdir -p $SAMSON_ROOT
fi

# Setup variables based on SAMSON_ROOT
SAMSON_HOME=$SAMSON_ROOT/home
SAMSON_WORKING=$SAMSON_ROOT/working
#samsonPush/Pop... log directory
LOG_DIR=$SAMSON_ROOT/logs
#samsonWorker log directory
SS_WORKER_LOG_DIR=$LOG_DIR
#delilah log directory
SS_DELILAH_LOG_DIR=$LOG_DIR
#logServer log directory
LOGSERVER_LOG_DIR=$SAMSON_ROOT/logserver

PATH=${SAMSON_HOME}/bin:${PWD}/scripts:${PATH}

OS=$(uname -s)

# Setup the Shared library path
if [ ${OS} = "Linux" ]; then
    LD_LIBRARY_PATH=${SAMSON_HOME}/lib:${LD_LIBRARY_PATH}
elif [ ${OS} = "Darwin" ]; then
    DYLD_LIBRARY_PATH=${SAMSON_HOME}/lib:${DYLD_LIBRARY_PATH}
fi

# Whoami?
SAMSON_OWNER=$(whoami)

mkdir -p $SAMSON_HOME $SAMSON_WORKING $SS_WORKER_LOG_DIR $LOG_DIR

export SAMSON_HOME SAMSON_WORKING LOG_DIR SS_DELILAH_LOG_DIR SS_WORKER_LOG_DIR PATH SAMSON_OWNER SAMSON_VERSION SAMSON_WORKER_PORT_ENV SAMSON_WORKER_WEB_PORT_ENV SAMSON_LOG_PORT_ENV LOGSERVER_LOG_DIR SAMSON_LOGQUERY_PORT_ENV SAMSON_LOG_LEVELS SAMSON_TEST_HOST ZOOKEEPER_SERVER ZOOKEEPER_PORT

echo SAMSON_HOME is $SAMSON_HOME
echo SAMSON_WORKING is $SAMSON_WORKING
echo SS_WORKER_LOG_DIR is $SS_WORKER_LOG_DIR
echo LOG_DIR is $LOG_DIR
echo ""
echo SAMSON_WORKER_PORT_ENV is $SAMSON_WORKER_PORT_ENV
echo SAMSON_WORKER_WEB_PORT_ENV is $SAMSON_WORKER_WEB_PORT_ENV
echo SAMSON_LOGQUERY_PORT_ENV is $SAMSON_LOGQUERY_PORT_ENV
echo SAMSON_LOG_PORT_ENV is $SAMSON_LOG_PORT_ENV
echo ""
echo ZOOKEEPER_SERVER is $ZOOKEEPER_SERVER
echo ZOOKEEPER_PORT is $ZOOKEEPER_PORT

alias sgrep="egrep --exclude=*.svn*"

