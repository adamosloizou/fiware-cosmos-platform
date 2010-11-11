#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map
#include "DataManager.h"		// ss::DataManager
#include "au_map.h"				// au::map
#include "samson.pb.h"			// ss::network::...

namespace ss {
	
	class ControllerQueue;
	class ControllerTask;
	class DataManagerCommandResponse;
	class SamsonController;
	
	class DataQueue
	{
		std::string name;	// Name of the queue
		size_t size;		// Size of this queue
		
	public:
		DataQueue( std::string _name)
		{
			name = _name;
			size = 0 ;
		}
		
		size_t getSize()
		{
			return size;
		}
		
		std::string getName()
		{
			return name;
		}
							
		
	};
	
	/**
	 Data manager at the controller
	 */
	
	class ControllerDataManager : public DataManager
	{
		au::Lock lock;
		
		au::map< std::string , ControllerQueue> queues;		// List of KeyValue queues
		au::map< std::string , DataQueue> data_queues;		// List of data values ( upload normal files )
		
		
		SamsonController *controller;	// Pointer to controller for module access
		
	public:
		
		ControllerDataManager( SamsonController *_controller ) : DataManager( getLogFileName()  )
		{
			controller = _controller;
		}
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string status();

		/**
		 Get the fileName of the log file
		 */
		
		static std::string getLogFileName( );

		
		// Get help about data stuff
		void helpQueues( network::HelpResponse *response );
			
		
	private:
		
		virtual DataManagerCommandResponse _run( std::string command );
		virtual void _un_run( std::string command );
		
		
		
	};

}

#endif
