#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
* Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
*/
#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/TokenTaker.h"                  // au::TokenTake
#include "au/Console.h"                     // au::Console
#include "au/ConsoleAutoComplete.h"

#include "tables/Select.h"
#include "tables/Table.h"

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "samson/delilah/DelilahCommandCatalogue.h"
#include "samson/delilah/AlertCollection.h"

namespace samson {
    
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahConsole : public au::Console, public Delilah
	{
		
        std::string commandFileName;
        bool database_mode; // Flag to indicate that we are running commands over table collection
        
        DelilahCommandCatalogue delilah_command_catalogue;
        
        AlertCollection trace_colleciton;    // Collection of traces for monitoring
        std::string trace_file_name;         // Name of the traces file ( if any )
        FILE *trace_file;                    // FILE to store traces if defined with save_traces
        
        // Counter for the received stream buffers
        au::CounterCollection<std::string> stream_out_queue_counters;

        // Flag to indicate if we are shoing traces
        bool show_alerts;
        bool verbose;
        
        // Flag to just visualize content on screen ( delilah -command  or -f XX )
        bool simple_output;
        
	public:
		
        
		DelilahConsole( NetworkInterface *network );		
		~DelilahConsole();
		
        // Main run command
        void run();
        
        // Set the command-file
        void setCommandfileName( std::string _commandFileName)
        {
            commandFileName = _commandFileName;
        }
        
        void setSimpleOutput()
        {
            simple_output = true;
        }
                
		// Eval a command from the command line
        virtual std::string getPrompt();
		virtual void evalCommand( std::string command );
        virtual void autoComplete( au::ConsoleAutoComplete* info );

        void autoCompleteOperations( au::ConsoleAutoComplete* info );
        void autoCompleteOperations( au::ConsoleAutoComplete* info , std::string type );

        void autoCompleteQueueForOperation( au::ConsoleAutoComplete* info , std::string operation_name , int argument_pos );
        void autoCompleteQueueWithFormat(au::ConsoleAutoComplete* info  ,  std::string key_format , std::string value_format);
        void autoCompleteQueues(au::ConsoleAutoComplete* info );
        
        virtual void process_escape_sequence( std::string sequence )
        {
            if( sequence == "samson" )
            {
                writeWarningOnConsole("SAMSON's cool ;)");
            }

            if( sequence == "q" )
            {
                appendToCommand("ls_queues");
            }
            
            if( sequence == "d" )
            {
                database_mode = !database_mode;
                refresh(); // refresh console
            }
                
        }
        
        
		// Run asynch command and returns the internal operation in delilah
		size_t runAsyncCommand( std::string command );

		// Functions overloaded from Delilah
		// --------------------------------------------------------
		
        void delilahComponentFinishNotification( DelilahComponent *component);
        void delilahComponentStartNotification( DelilahComponent *component);
        
		// Function to process messages from network elements not handled by Delila class
		int _receive( Packet* packet );		

        // Process buffers of data received in streaming from SAMSON
        void receive_buffer_from_queue( std::string queue , engine::Buffer* buffer );
        
        
		// Notify that an operation hash finish
		virtual void notifyFinishOperation( size_t id )
		{
			std::ostringstream output;
			output << "Finished local delilah process with : " << id ;
			writeWarningOnConsole( output.str() );
		}
		
		// Show a message on screen
		void showMessage( std::string message)
		{
            if( simple_output )
            {
                std::cout << message;
                return;
            }
            
			writeOnConsole( au::strToConsole( message ) );
		}
		void showWarningMessage( std::string message)
        {
            if( simple_output )
            {
                std::cout << message;
                return;
            }
            
			writeWarningOnConsole( au::strToConsole( message ) );
        }
        
		void showErrorMessage( std::string message)
        {
            if( simple_output )
            {
                std::cout << message;
                return;
            }
            
			writeErrorOnConsole( au::strToConsole( message ) );
        };

		
		virtual void showTrace( std::string message)
		{
			if( show_alerts )
				writeWarningOnConsole( message );
		}
        
        
        void run_repeat_command( std::string command )
        {
            LM_TODO(("Check what type of messages...."));
            runAsyncCommand(command);
        }

	};


}


#endif
