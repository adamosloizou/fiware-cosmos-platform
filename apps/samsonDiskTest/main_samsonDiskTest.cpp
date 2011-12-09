/* ****************************************************************************
*
* FILE                     main_samsonWorker.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include <signal.h>


#include "au/LockDebugger.h"            // au::LockDebugger
#include "au/Cronometer.h"
#include "au/time.h"
#include "au/string.h"


#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/Notification.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/samsonVars.h"
#include "samson/common/platformProcesses.h"
#include "samson/common/SamsonSetup.h"
#include "samson/network/Network2.h"
#include "samson/network/Endpoint2.h"
#include "samson/network/EndpointManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/module/ModulesManager.h"



/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

int param_num_files;
int param_file_size;
int param_max_threads;
int param_trials;

/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	SAMSON_ARGS,
    { "-size",        &param_file_size,    "",  PaInt,     PaOpt,     64,   1, 1024,  "Size in megabytes of each file" },
    { "-files",       &param_num_files,    "",  PaInt,     PaOpt,     100,  1, 1024,  "Number of files" },
    { "-max_threads", &param_max_threads,  "",  PaInt,     PaOpt,     3,   1, 100,    "Max number of threads to test" },
    { "-trials",      &param_trials,       "",  PaInt,     PaOpt,     3,   1, 100,    "Number of trials for each test" },
	PA_END_OF_ARGS
};

/* ****************************************************************************
*
* global variables
*/
int                   logFd             = -1;


/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsonDiskTest is a simple tool to analyse disk performance in a SAMSON deployment.\n\n";
static const char* manDescription      =
    "\n"
    "samsonDiskTest is a simple tool to analyse disk performance in a SAMSON deployment."
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;


/* ****************************************************************************
 *
 * Test - 
 */

class Test : public engine::Object
{
    
    // Setup of the test
    int num_threads;
    
    // Cronometer
    au::Cronometer cronometer;
    au::Cronometer cronometer_show_status;
    
    // Write operations
    int files_num;
    int files_size;
    
    // Progress 
    int files_num_write;
    int files_num_read;
    int files_num_remove;
    
    // Flag to indicate finish of test
    bool finish;
    size_t accumulated_size;
    
    // Temporal buffer for reads...
    char * read_data;

    // Final time when finishing test
    size_t final_time;
    
    public:
    
    Test( int _num_threads )
    {
        
        // Setup for this test
        num_threads = _num_threads;
        
        // Common setup
        
        files_num = param_num_files;
        files_size = param_file_size * 1024 * 1024;
        
        files_num_write=0;
        files_num_read=0;
        files_num_remove=0;
        
        accumulated_size = 0;

        read_data = (char*) malloc( files_size );
        finish = false;
    }
    
    ~Test()
    {
        free( read_data );
    }
    
    
    std::string str_results()
    {
        return au::str("Test with %02d threads Time %s / Accumulated size %s / Rate %s " , 
                       num_threads,
                       au::time_string( final_time ).c_str() ,
                       au::str( accumulated_size ).c_str() ,
                       au::str_rate( accumulated_size , final_time ).c_str()
                       );
    }
    
    void notify( engine::Notification* notification )
    {
        
        if ( notification->isName( notification_disk_operation_request_response ) )
        {
            
            int file = notification->environment.getInt("file", -1);
            std::string type = notification->environment.get("type", "-");
            
            engine::DiskOperation *operation = (engine::DiskOperation*) notification->extractObject();
            
            if( !operation )
                LM_X(1, ("Operation not present in a disk operation notification"));
            
            if( operation->error.isActivated() )
                LM_X(1, ("Disk operation [%s] failed: %s" , operation->getDescription().c_str() , operation->error.getMessage().c_str() ));
            
            if( file == -1 )
                LM_X(1,("Received file %d", file));
            
            if(  type == "write" )
            {
                files_num_write++;    
                accumulated_size += files_size;
                schedule_reader( file );
            }
            if( type == "read" )
            {
                files_num_read++;    
                accumulated_size += files_size;
                schedule_remove( file );
            }
            if( type == "remove" )
            {
                files_num_remove++;    
            }        
            
        }
        
        if( files_num_write == files_num )
            if( files_num_read == files_num )
                if( files_num_remove == files_num )
                    finish = true;
        
    }

    void init()
    {
        engine::DiskManager::shared()->setNumOperations( num_threads );
    }
    
    void schedule_remove( int i )
    {
        // Get the file name
        std::string fileName = getFileName( i );
        
        // Create the operation
        engine::DiskOperation *operation = engine::DiskOperation::newRemoveOperation(fileName, getEngineId() );
        operation->environment.setInt("file", i);
        
        // Schedule operation
        engine::DiskManager::shared()->add( operation );
    }
    
    
    void schedule_reader( int i )
    {
        // Get the file name
        std::string fileName = getFileName( i );
        
        // Create the operation
        engine::DiskOperation *operation = engine::DiskOperation::newReadOperation( read_data , fileName , 0 , files_size, getEngineId() );
        operation->environment.setInt("file", i);
        
        // Schedule operation
        engine::DiskManager::shared()->add( operation );
    }
    
    
    void schedule_writer( int i )
    {
        // Get a buffer
        engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer("disk_test", files_size, 0 );
        buffer->setSize(files_size);

        // Get the file name
        std::string fileName = getFileName( i );

        // Create the operation
        engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation( buffer , fileName , getEngineId() );
        operation->environment.setInt("file", i);

        
        // Schedule operation
        engine::DiskManager::shared()->add( operation );
    }
    
    void run()
    {
        // Init setup for this test
        init();
        
        LM_M(("Running test with %d threads ( %d files of %s = %s )" 
              , num_threads 
              , files_num 
              , au::str( files_size , "B").c_str() 
              , au::str( ((size_t)files_num * (size_t)files_size) , "B").c_str() 
              ));


        
        // Schedule writters
        for ( int i = 0 ; i < files_num ; i ++ )
            schedule_writer(i);
 
        // Init the test
        cronometer.reset();
        
        while(!finish)
        {
            usleep(100000);
            
            if( cronometer_show_status.diffTimeInSeconds() > 10 )
            {
                cronometer_show_status.reset();
                
                LM_V(("Progress  W:%d R:%d D:%d / %d " ,  files_num_write, files_num_read, files_num_remove, files_num ));
                LM_V(("Partical results: Time %s Accumulated size %s ( Rate %s )" , 
                      au::time_string( cronometer.diffTimeInSeconds() ).c_str(),
                      au::str( accumulated_size , "B" ).c_str() ,
                      au::str_rate( accumulated_size ,  cronometer.diffTimeInSeconds() , "Bs" ).c_str()
                      ));
            }
        }
        
        // Write results...
        final_time = cronometer.diffTimeInSeconds();
        
        // Print results
        LM_M(("Finish test: %s", str_results().c_str() ));
        
    }    
    
    int getNumThreads()
    {
        return num_threads;
    }
    
private:
    
    std::string getFileName( int i )
    {
        return au::str("disk_test_file_%d.txt",i );
    }

};




/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{

	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paConfig("man synopsis",                  (void*) manSynopsis);
	paConfig("man shortdescription",          (void*) manShortDescription);
	paConfig("man description",               (void*) manDescription);
	paConfig("man exitstatus",                (void*) manExitStatus);
	paConfig("man author",                    (void*) manAuthor);
	paConfig("man reportingbugs",             (void*) manReportingBugs);
	paConfig("man copyright",                 (void*) manCopyright);
	paConfig("man version",                   (void*) manVersion);

    
	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

    /*
	samson::SamsonSetup::init(samsonHome , samsonWorking);          // Load setup and create default directories
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories
    
	engine::SharedMemoryManager::init(
                    samson::SamsonSetup::shared()->getInt("general.num_processess") , 
                    samson::SamsonSetup::shared()->getUInt64("general.shared_memory_size_per_buffer")
                                      );
    */
	engine::Engine::init();
	engine::DiskManager::init(1);
	//engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
	engine::MemoryManager::init( 1000000000 );

    LM_V(("---------------------------------------------------------------------------"));
    LM_V(("Setup"));
    LM_V(("---------------------------------------------------------------------------"));
    LM_V(("%15s %d", "trials" , param_trials));
    LM_V(("%15s %d", "max_threads" , param_max_threads));
    LM_V(("%15s %d", "#files" , param_num_files));
    LM_V(("%15s %d", "file size (MB)" , param_file_size));
    LM_V(("---------------------------------------------------------------------------"));
    
    std::vector<Test*> tests;

    // Schedule all the tests
    for ( int i=1;i<=param_max_threads;i++)
        for (int t=0;t<param_trials;t++)
            tests.push_back( new Test(i) );
    
    // Run all tests
    for ( size_t i=0 ; i<tests.size() ; i++ )        
        tests[i]->run();

    // Show results
    printf("---------------------------------------------------------------------------\n");
    printf("Results\n");
    printf("---------------------------------------------------------------------------\n");
    for ( size_t i=0 ; i<tests.size() ; i++ )        
    {
        if( (i>0) && (tests[i]->getNumThreads() != tests[i-1]->getNumThreads() ) )
            printf("---------------------------------------------------------------------------\n");
        printf("%s\n" , tests[i]->str_results().c_str() );

    }
    printf("---------------------------------------------------------------------------\n");
    
}