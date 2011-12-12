
/*
 
 samsonPush
 
 Example app using samsonClient lib
 It listen std input and push content to the samson cluster indicated by parameters
 
 AUTHOR: Andreu Urruela
 
 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/common/coding.h"               // KVHeader


size_t buffer_size;
size_t timeOut;
char breaker_sequence[1024];
char controller[1024];
char queue_name[1024];
bool lines;                         // Flag to indicate that input is read line by line
int push_memory;                    // Global memory used as a bffer
int max_rate;                       // Max rate

static const char* manShortDescription = 
"samsonPush is a easy-to-use client to send data to a particular queue in a SAMSON system. Just push data into the standard input\n";

static const char* manSynopsis =
"[-help] [-controller str_controller] [-timeout int_t] [-buffer_size int_size] [-breaker_sequence str_pattern] [-lines bool] queue\n";

int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{
	{ "-controller",  controller,            "",  PaString,  PaOpt, _i "localhost",                PaNL,        PaNL,  "controller IP:port"                         },
	{ "-timeout",     &timeOut,              "",  PaInt,     PaOpt,              0,                    0,       10000,  "Timeout to deliver a block to the platform" },
	{ "-buffer_size", &buffer_size,          "",  PaInt,     PaOpt,       default_buffer_size,         1,   128000000,  "Buffer size in bytes"                       },
	{ "-mr",          &max_rate,             "",  PaInt,     PaOpt,       10000000,      100,  100000000,  "Max rate in bytes/s"                  },
	{ "-breaker_sequence", breaker_sequence, "",  PaString,  PaOpt,        _i "\n",   PaNL,         PaNL,  "Breaker sequence ( by default \\n )"        },
	{ "-lines",       &lines,                "",  PaBool,    PaOpt,          false,  false,         true,  "Read std-in line by line"                   },
	{ "-memory",      &push_memory,          "",  PaInt,     PaOpt,           1000,      1,      1000000,  "Memory in Mb used to push data ( default 1000)" },
	{ " ",            queue_name,            "",  PaString,  PaReq,      _i "null",   PaNL,         PaNL,  "name of the queue to push data"             },
	PA_END_OF_ARGS
};

int logFd = -1;

void find_and_replace( std::string &source, const std::string find, std::string replace ) {
   size_t j;
   for ( ; (j = source.find( find )) != std::string::npos ; ) {
	  source.replace( j, find.length(), replace );
   }
}


void literal_string( std::string& txt )
{
   std::string slash = "\n"; 
   std::string replace_slash = "\\n";
   find_and_replace( txt , slash , replace_slash );   
}


size_t full_read( int fd , char* data , size_t size)
{
    size_t read_size = 0;
    
    while( read_size < size )
    {
        ssize_t t = read( fd , data+read_size , size - read_size );
        
        if( t==-1)
		   LM_X(1,("Error reading input data"));
        
        if( t == 0)
            break;
        else
            read_size+=t;
    }
    
    return read_size;    
}

int main( int argC , const char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) false);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",          (void*) manSynopsis);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();


	// Check queue is specified
	if( strcmp( queue_name , "null") == 0 )
	   LM_X(1,("Please, specify a queue to push data to"));
    
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;
    
    // Set 1G RAM for uploading content
    size_t total_memory = push_memory*1024*1024;
    LM_V(("Setting memory for samson client %s" , au::str(total_memory,"B").c_str() ));
    client.setMemory( total_memory );
    
    LM_V(("Connecting to %s ..." , controller));
    
    // Init connection
    if( !client.init( controller ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , client.getErrorMessage().c_str() );
        exit(0);
    }

    // Create the push buffer to send data to a queue in buffer-mode
    samson::SamsonPushBuffer *pushBuffer = new samson::SamsonPushBuffer( &client , queue_name , timeOut );

    LM_V(("Conection to %s OK" , controller));
    
    // Read data in blocks, lock the separator backward
    // --------------------------------------------------------------------------------
    
    if( buffer_size == 0)
        LM_X(1,("Wrong buffer size %lu", buffer_size ));
    
    char *data = (char*) malloc ( buffer_size );
    if( !data )
        LM_X(1,("Error allocating %lu bytes" , buffer_size ));
    

    size_t size = 0;                // Bytes currently contained in the buffer
    

    std::string tmp_separator = breaker_sequence;
    literal_string( tmp_separator );

    LM_V(("Input parameter buffer_size %s" , au::str(buffer_size).c_str() ));
    LM_V(("Input parameter timeout %s " , au::time_string( timeOut ).c_str() ));
    LM_V(("Input parameter break_sequence '%s' ( length %d ) " , tmp_separator.c_str() , strlen(breaker_sequence) ));

    // Statistics about stdin rate ( it also controls max rate )
    au::rate::Rate rate_stdin;
    
    while( true )
    {
        
        // --------------------------------------------------------------------
        // Read content from stdin
        // --------------------------------------------------------------------
        
        size_t read_bytes = 0;
        if( lines ) 
        {
            char * string = fgets(data , buffer_size, stdin );

            if( string ) 
                read_bytes = strlen( string );
            else
                read_bytes = 0;
            
        }
        else
        {
            read_bytes = full_read( 0 , data + size , buffer_size - size );
        }
        
        // Statistics about input size
        rate_stdin.push( read_bytes );
        
        // Information about current status....
        size_t memory = engine::MemoryManager::shared()->getMemory();
        size_t used_memory = engine::MemoryManager::shared()->getUsedMemory();
        double memory_usage = engine::MemoryManager::shared()->getMemoryUsage();

        LM_V(("Stdin info: %s", rate_stdin.str().c_str())); 
        
        if( used_memory > 0 )
        {
            LM_V(("Memory used %s / %s ( %s )", 
                  au::str( used_memory , "B" ).c_str() , 
                  au::str( memory , "B" ).c_str() , 
                  au::percentage_string( memory_usage ).c_str() 
                  ));
        }
       
        // Print verbose the list of push components ( if any )
        client.getInfoAboutPushConnections(true);
        
        //LM_M(("Read command for %lu bytes. Read %lu" , buffer_size - size , read_bytes ));
        
        if( read_bytes == 0 )
            break;

        // Increase the size of data contained in the buffer
        size += read_bytes;

        
        // Processing data contained in "data" with size "size"
        // -----------------------------------------------------------------
        
        // Find backward a particular string...
        const char * last_pos = au::laststrstr( data , size , breaker_sequence );
        
        if( !last_pos )
            LM_X(1,("Not found breaker sequecny '%s' in a buffer of %lu bytes" , breaker_sequence , size ));
        
        size_t output_size = last_pos - data + strlen( breaker_sequence );
        
        //LM_M(("Processing buffer with %s --> %s block push to queue" , au::str(size).c_str() , au::str(output_size).c_str() ));
        
        // Emit this block of data
        pushBuffer->push(data, output_size );

        // -----------------------------------------------------------------


        // Sleep if necessary
        if ( rate_stdin.getGlobalRate() > (double) max_rate )
        {
            LM_V(("Stdin info: %s", rate_stdin.str().c_str())); 
            LM_V(("Sleeping respect max rate %s" , au::str( max_rate ).c_str() ));
            sleep ( 1 );
        }
        

        // Move rest of data to the begining of the buffer
        if( output_size < size )
        {
            memmove( data , data + output_size , size - output_size );
            size = size-output_size;
        }
        else
            size = 0;
        
    }
    
    // Last push
    pushBuffer->flush();

    LM_V(("--------------------------------------------------------------------------------"));
    LM_V(("Stdin info:        %s", rate_stdin.str().c_str())); 
    LM_V(("PushBuffer info:   %s", pushBuffer->rate.str().c_str())); 
    LM_V(("SamsonClient info: %s", client.rate.str().c_str())); 
    LM_V(("--------------------------------------------------------------------------------"));
    
    
    // Wait until all operations are complete
    LM_V(("Waiting for all the push operations to complete..."));
    client.waitUntilFinish();

    
    
	
}
