#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include <cstdlib>				// atexit

#include "logMsg.h"				

#include "Console.h"			// au::Console
#include "DelilahClient.h"      // ss:DelilahClient
#include "Delilah.h"			// ss::Delilah

#include <time.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

namespace ss {
	
	
	typedef struct {
		const char *name;				/* User printable name of the function. */
		const char *doc;				/* Documentation for this function.  */
	} COMMAND;
	
	
	char * dupstr (const char *s);
	char * command_generator (const char * text, int state);
	char ** readline_completion ( const char* text, int start,int end );	
	

	class AutoCompletionOptions
	{
		std::vector<std::string> options;
		size_t index;	// Current index of search
		
	public:
		
		void clearOptions()
		{
			options.clear();
			index = 0;
		}
		
		void addOption( std::string v )
		{
			options.push_back( v );
		}
		
		void clearSearch()
		{
			//std::cout << "clear earch\n";
			index = 0;
		}
		
		char *next( const char *text )
		{
			//std::cout << "next: " << text << " \n";
			
			size_t len = strlen(text);
			
			while ( index < options.size() )
			{
				const char *name = options[index].c_str();
				
				index++;

				//std::cout << "consideting: " << name << " .... ";
				
				if (strncmp (name, text, len) == 0)
				{
					//std::cout << "added\n";
					return (dupstr((char*)name));
				}
				else
				{
					//std::cout << "discarted\n";
				}
			}
			
			//std::cout << "No more options\n";
			return NULL;
		}
	};

	class DelilahConsole : public au::Console, public DelilahClient
	{
		Delilah* delilah;	// Internal delilah object to interact with SAMSON
		
		
		
	public:
		
		DelilahConsole( Delilah *_delilah )
		{
			// double link between delilah and its client
			delilah = _delilah;
			delilah->client =  this;	
			
			/* completion function for readline library */
			rl_attempted_completion_function = readline_completion;

		}
		
		~DelilahConsole()
		{
			delete delilah;
		}
				
		virtual std::string getPrompt()
		{
			return  "Delilah> ";
		}
		
		virtual std::string getHeader()
		{
			return  "Delilah";
		}
		
		// Eval a command from the command line
		virtual void evalCommand( std::string command );

		// PacketReceiverInterface
		int receive(int fromId, Message::MessageCode msgCode, Packet* packet);		

		virtual void quit()
		{
			au::Console::quit();
		}
	
		virtual void loadDataConfirmation( DelilahUploadDataProcess *process);		
	
		
		virtual void showMessage( std::string message)
		{
			writeWarningOnConsole( message );
		}
		
		virtual void notifyFinishOperation( size_t id )
		{
			std::ostringstream output;
			output << "Finished local delilah process with : " << id ;
			writeWarningOnConsole( output.str() );
		}
		
		
		void showQueues( const network::QueueList ql);
		void showAutomaticOperations( const network::AutomaticOperationList aol);
		void showDatas( const network::DataList ql);
		void showOperations( const network::OperationList ql);
		void showJobs( const network::JobList ql);
		void showWorkers( const network::ControllerStatus &cs, const network::WorkerStatusList ql);


		
	};

}


#endif
