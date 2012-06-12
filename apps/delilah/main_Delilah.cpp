/* ****************************************************************************
*
* FILE            main_Delilah.cpp
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Main file for the "delilah" console app
*
*/
#include "parseArgs/paBuiltin.h"        // paLsHost, paLsPort
#include "parseArgs/paConfig.h"         // paConfig()
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/string.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "au/ThreadManager.h"
#include "au/log/LogToServer.h"
#include "au/log/log_server_common.h"


#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/common/ports.h"
#include "samson/common/status.h"
#include "samson/common/samsonVersion.h"
#include "samson/common/SamsonSetup.h"

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Packet.h"
#include "samson/network/HostMgr.h"
#include "samson/network/Host.h"

#include "samson/delilah/DelilahConsole.h"

#include "samson/common/status.h"

#include "samson_system/Process.h"

samson::system::Process p;


/* ****************************************************************************
*
* Option variables
*/
char             target_host[80];
char             user[1024];
char             password[1024];
int              target_port;

int              memory_gb;
int              load_buffer_size_mb;
char             commandFileName[1024];
bool             monitorization;
char             command[1024]; 

char             log_file[1024]; 
char             log_host[1024];
unsigned short   log_port; 


#define LOC "localhost"
#define LOG_PORT AU_LOG_SERVER_PORT
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    { "-log_host",          log_host,             "", PaString, PaOpt, _i "localhost", PaNL, PaNL,  "log server host"  },
    { "-log_port",          &log_port,            "", PaInt,    PaOpt,  LOG_PORT,      0, 10000,  "log server port"    },
    { "-log_file",          log_file,             "", PaString, PaOpt, _i "",          PaNL, PaNL,  "Local log file"   },
    
    { "-user",             user,                  "",                       PaString, PaOpt, _i "anonymous",      PaNL, PaNL,  "User to connect to SAMSON cluster"      },
    { "-password",         password,              "",                       PaString, PaOpt, _i "anonymous",      PaNL, PaNL,  "Password to connect to SAMSON cluster"  },
    { "-memory",           &memory_gb,            "MEMORY",                 PaInt,    PaOpt, 1,                   1,    100,   "memory in GBytes"                       },
    { "-load_buffer_size", &load_buffer_size_mb,  "LOAD_BUFFER_SIZE",       PaInt,    PaOpt, 64,                  64,   2048,  "load buffer size in MBytes"             },
    { "-f",                commandFileName,       "FILE_NAME",              PaString, PaOpt, _i "",               PaNL, PaNL,  "File with commands to run"              },
    { "-command",          command,               "", PaString, PaOpt, _i "",               PaNL, PaNL,  "Single command to be executed"          },
    { "",                  target_host,           "",                       PaString, PaOpt, _i LOC,              PaNL, PaNL,  "SAMSON server hostname"                 },
    { "",                 &target_port,           "",                       PaInt,    PaOpt, SAMSON_WORKER_PORT,  1,    99999, "SAMSON server port"                     },

    PA_END_OF_ARGS
};


/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription =  "delilah is the command-line client for SAMSON system\n";
static const char* manDescription      =
    "\n"
    "delilah is the command-line client to upload & download data, run processing commands and monitor a SAMSON system.\n"
    "See pdf document about samson system to get more information about how to use delilah client"
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;



samson::DelilahConsole* delilahConsole = NULL;



void cleanup(void)
{
    // Stop all threads to clean up
    engine::Engine::stop();
    engine::DiskManager::stop();
    engine::ProcessManager::stop();

    if( delilahConsole )
        delilahConsole->stop();
    
    // Wait all threads to finsih
    au::ThreadManager::shared()->wait("Delilah");

    // Clear google protocol buffers library
    google::protobuf::ShutdownProtobufLibrary();
    
    LM_T(LmtCleanup, ("Shutting down delilah components (delilahConsole at %p)", delilahConsole));
    if (delilahConsole != NULL)
    {
        LM_T(LmtCleanup, ("deleting delilahConsole"));
        delete delilahConsole;
        delilahConsole = NULL;
    }
    
    LM_T(LmtCleanup, ("Shutting down LockDebugger"));
    au::LockDebugger::destroy();

    LM_T(LmtCleanup, ("destroying ModulesManager"));
    samson::ModulesManager::destroy("delilah");

    LM_T(LmtCleanup, ("destroying ProcessManager"));
    engine::ProcessManager::destroy();

    LM_T(LmtCleanup, ("destroying DiskManager"));
    engine::DiskManager::destroy();

    LM_T(LmtCleanup, ("destroying MemoryManager"));
    engine::MemoryManager::destroy();

    LM_T(LmtCleanup, ("destroying Engine"));
    engine::Engine::destroy();

    LM_T(LmtCleanup, ("Shutting down SamsonSetup"));
    samson::SamsonSetup::destroy();
    
    LM_T(LmtCleanup, ("Calling paConfigCleanup"));
    paConfigCleanup();
    LM_T(LmtCleanup, ("Calling lmCleanProgName"));
    lmCleanProgName();
    LM_T(LmtCleanup, ("Cleanup DONE"));
    
    // Stop logging to server
    au::stop_log_to_server();
}



// Custom name for the log file
extern char * paProgName;
extern size_t delilah_random_code;

/* ****************************************************************************
*
* main - 
*/

int main(int argC, const char *argV[])
{

    paConfig("prefix",                        (void*) "DELILAH_");
    paConfig("builtin prefix",                (void*) "SS_DELILAH_");
    paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) true);
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE: TEXT");
    paConfig("log to stderr",                 (void*) true);

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

    paConfig("default value", "-logDir", (void*) "/var/log/samson");
    paConfig("if hook active, no traces to file", (void*) true);

    // Random initialization
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int rand_seq = tp.tv_sec*1000000 + tp.tv_usec;
    srand( rand_seq );

    // Random code for delilah
    delilah_random_code = au::code64_rand();
    paProgName = strdup( au::str("delilah_%s" , au::code64_str( delilah_random_code ).c_str() ).c_str() );

    paParse(paArgs, argC, (char**) argV, 1, true);
    lmAux((char*) "father");
    logFd = lmFirstDiskFileDescriptor();

    // Clean up function
    atexit(cleanup);

    // Start connection with log server....
    std::string local_log_file;
    if( strlen(log_file) > 0 )
    {
        local_log_file = log_file;
    }
    else
        local_log_file = au::str("%s/delilahLog_%s_%d" , paLogDir 
                                                       , au::code64_str( delilah_random_code ).c_str() 
                                                       , (int) getpid() );
    
    au::start_log_to_server( log_host , log_port , local_log_file );

    if ((strcmp(target_host, "localhost") == 0) || (strcmp(target_host, "127.0.0.1") == 0))
    {
        if (gethostname(target_host, sizeof(target_host)) != 0)
            LM_X(1, ("gethostname: %s", strerror(errno)));
        //LM_M(("Translated 'localhost' to %s", target_host));
    }


    // Make sure this singleton is created just once
    au::LockDebugger::shared();

    // Init samson setup with default values
    samson::SamsonSetup::init("","");

    // Setup parameters from command line 
    size_t _memory           = (size_t) memory_gb * (size_t) (1024*1024*1024);
    size_t _load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);

    samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",_memory));
    samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",_load_buffer_size) );

    // Engine and its associated elements
    engine::Engine::init();
    engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
    engine::DiskManager::init(1);
    engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));

    samson::ModulesManager::init("delilah");         // Init the modules manager
    
    
    // Create a DelilahControler once network is ready
    delilahConsole = new samson::DelilahConsole();
    delilahConsole->connect( target_host , target_port , user , password );
    
    // ----------------------------------------------------------------
    // Special mode with one command line command
    // ----------------------------------------------------------------
    
    if ( strcmp( command , "" ) != 0 )
    {
        {
            au::Cronometer cronometer;
            while ( !delilahConsole->isConnectionReady() ) 
            {
                usleep(100000);
                if( cronometer.diffTime() > 1 )
                {
                    LM_V(("Waiting delilah to connect to all workers"));
                    cronometer.reset();
                }
            }
        }
        // Set output to screen
        delilahConsole->setNoOutput();
        size_t id = delilahConsole->runAsyncCommand( command );

        if( id != 0)
        {
            // Wait until this operation is finished
            while (delilahConsole->isActive( id ) )
            {
                // Wait until command is finished
                usleep(1000);
            }
            
            if( delilahConsole->hasError( id ) )
            {
                LM_E(("Error running '%s' \n", command ));
                LM_E(("Error: %s",  delilahConsole->errorMessage( id ).c_str()));
            }
            else
            {
                printf("%s" , delilahConsole->getOutputForComponent(id).c_str());
                fflush(stdout);
            }
            
        }
        
        exit(0);
    }
    
    LM_M(("Delilah random code %s" , au::code64_str( delilah_random_code ).c_str() ));
    LM_M(("Running delilah console..."));
    lmFdUnregister(2); // no more traces to stdout
    
    // ----------------------------------------------------------------
    // Special mode for file-based commands
    // ----------------------------------------------------------------

    if ( strcmp( commandFileName,"") != 0 )
    {
        
        // Set timple output
        delilahConsole->setSimpleOutput();
        
        {
            au::Cronometer cronometer;
            while ( !delilahConsole->isConnectionReady() ) 
            {
                usleep(100000);
                if( cronometer.diffTime() > 1 )
                {
                    LM_M(("Waiting delilah to connect to all workers"));
                    cronometer.reset();
                }
            }
        }
        
        FILE *f = fopen( commandFileName , "r" );
        if( !f )
        {
            LM_E(("Error opening commands file %s", commandFileName));
            exit(0);
        }
        
        int num_line = 0;
        char line[1024];
        
        std::cerr << au::str("Processing commands file %s\n", commandFileName );

        while( fgets(line, sizeof(line), f) )
        {
            // Remove the last return of a string
            while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
                line[ strlen(line)-1]= '\0';
            
            //LM_M(("Processing line: %s", line ));
            num_line++;
            
            if( ( line[0] != '#' ) && ( strlen(line) > 0) )
            {
                
                std::cerr << au::str("Processing: '%s'\n", line);
                size_t id = delilahConsole->runAsyncCommand( line );
                std::cerr << au::str("Delilah id generated %lu\n" , id);
                
                if( id != 0)
                {
                    //LM_M(("Waiting until delilah-component %ul finish", id ));
                    // Wait until this operation is finished
                    while (delilahConsole->isActive( id ) )
                        usleep(1000);
                    
                    if( delilahConsole->hasError( id ) )
                    {
                        LM_E(("Error running '%s' at line %d", line, num_line));
                        LM_E(("Error: %s",  delilahConsole->errorMessage( id ).c_str()));
                    }
                    
                }
            }
        }
        
        fclose(f);

        // Flush content of console
        //delilahConsole->flush();
        LM_M(("delilah exit correctly"));
        exit(0);
    }

    // Run console
    delilahConsole->run();

    return 0;
}
