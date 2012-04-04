
#include <signal.h>
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()
#include "au/ThreadManager.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"               // KVHeader

#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"

#include "SamsonConnector.h"

size_t buffer_size;
char input[1024];
char output[1024];
char input_splitter_name[1024];
bool interactive;

static const char* manShortDescription = 
"samsonConnector is a easy-to-use network tool used for small processing and data transportation in a SAMSON system.\n"
"Several inputs and outputs can be especified as follows:\n"
"\n"
"------------------------------------------------------------------------------------\n"
" Inputs\n"
"------------------------------------------------------------------------------------\n"
"      stdin                        Data is read from stdin\n"
"      port:X                       Opens a port waiting for incomming connections that push data to us\n"
"      connection[:host]:port       We connect to this host and port and read data\n" 
"      samson[:host][:port]:queue   We connect to this SAMSON cluster and pop data from queue\n" 
"\n"
"------------------------------------------------------------------------------------\n"
" Outputs\n"
"------------------------------------------------------------------------------------\n"
"      stdout                       Data is write to stdout\n"
"      port:X                       Opens a port waiting for output connections. Data is pushed to all connections\n"
"      connection:host:port         We connect to this host and port and write data\n" 
"      samson[:host][:port]:queue   We connect to this SAMSON cluster and push data from queue\n" 
"\n"
"------------------------------------------------------------------------------------\n"
" Examples:\n"
"------------------------------------------------------------------------------------\n"
"\n"
" samsonConnector -input stdin -output samson:samson01:input\n"
"\n"
"        Data is read from stdin and pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01'\n"
"\n"
" samsonConnector -input port:10000 -output \"port:20000 samson:samson01:input\"\n"
"\n"
"        Data is read from incomming connections to port 10000.\n"
"        Data is then pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01' and to any connection to port 20000\n"

;

static const char* manSynopsis = "[-input input_description]  [-output output_description]  [-buffer_size size] [-splitter splitter_name]\n";


int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{   
	{ "-input",            input,               "",  PaString,  PaOpt, _i "stdin"  , PaNL, PaNL,        "Input sources "          },
	{ "-output",           output,              "",  PaString,  PaOpt, _i "stdout"  , PaNL, PaNL,       "Output sources "         },
	{ "-buffer_size",      &buffer_size,        "",  PaInt,     PaOpt,       default_buffer_size, 1,   default_buffer_size,  "Buffer size in bytes"    },
	{ "-splitter",         input_splitter_name, "",  PaString,  PaOpt, _i "",   PaNL, PaNL,  "Splitter"  },
	{ "-i",                &interactive,        "",  PaBool,    PaOpt,    false,  false,   true,        "Interactive console"          },
	PA_END_OF_ARGS
};

// Log fg for traces
int logFd = -1;


// Network connections ( input and output )
samson::SamsonConnector samson_connector;



// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;


void* review_samson_connector(void*p)
{
    // Endless loop waiting for data....
    while( true )
    {
        samson_connector.review();
        sleep(5);
    }

	return NULL;
}

void captureSIGPIPE(int s )
{
    // Nothing
}

int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) true);  // In production it will be false
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("log to stderr",                 (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
    // Random initialization
    srand( time(NULL) );
    
    // Capturing SIGPIPE
    if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR)
        LM_W(("SIGPIPE cannot be handled"));
    
    
    if( buffer_size == 0)
        LM_X(1,("Wrong buffer size %lu", buffer_size ));

    // Init samson setup with default values
	samson::SamsonSetup::init("","");			
    
    // Engine and its associated elements
	engine::Engine::init();
	engine::MemoryManager::init( 1000000000 );
	//engine::DiskManager::init(1);
	//engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
    
	samson::ModulesManager::init();         // Init the modules manager

    // Ignore verbose mode if interactive is activated
    if( interactive )
        lmVerbose = false;
    
    // Add outputs
    au::ErrorManager error;
    samson_connector.add_outputs(output ,&error);
    if( error.isActivated() )
        samson_connector.show_error( error.getMessage().c_str()  );
    
    // Add inputs
    samson_connector.add_inputs(input , &error);
    if( error.isActivated() )
        samson_connector.show_error( error.getMessage().c_str() );
    
    // Review to create dedicated connections
    samson_connector.set_setup_complete();
    samson_connector.review();
    
    // Background thread to review connections in samson connector
    pthread_t t;
    au::ThreadManager::shared()->addThread( "SamsonConector:review" , &t, NULL, review_samson_connector, NULL);
    
    // Run console if interactive mode is activated
    if( interactive )
        samson_connector.runConsole();
    else
        while( true )
            sleep(10000);
    
    
    
	return 0;
}
