#ifndef _H_DATA_MANAGER
#define _H_DATA_MANAGER

#include <iostream>
#include <fstream>							// ifstream , ofstream
#include <sstream>							// std::ostringstream
#include <set>								// std::set

#include "logMsg.h"							// LM_*
#include "traceLevels.h"				    // Trace Levels

#include "data.pb.h"
#include "traces.h"
#include "Lock.h"							// au::Lock
#include "au_map.h"							// au::map
#include "LogFile.h"						// ss::LogFile
#include "DataManagerCommandResponse.h"		// ss::DataManagerCommandResponse

namespace ss
{
	class LogFile;
	class DataManagerItem;
	
	class DataManager
	{
		LogFile file;							// File to write
		size_t task_counter;					// Internal counter to give new task ids

		
	protected:
		
		au::Lock lock;							// Lock to protect multi-thread
		std::set<size_t> active_tasks;			// Set of active tasks ( to ignore non active tasks commands )
		
	public:
		
		DataManager( std::string  fileName );
		
		virtual ~DataManager() {}

		// Init session
		void initSession();
		
		/**
		 Function to interact with "data"
		 */
		
		void beginTask( size_t task_id , std::string command  );
		DataManagerCommandResponse runOperation( size_t task_id , std::string command );
		void cancelTask( size_t task_id , std::string error );
		void addComment( size_t task_id , std::string comment);
		void finishTask( size_t task_id );
		
	protected:
		
		DataManagerCommandResponse _runOperation( size_t task_id , std::string command );

		
	protected:
		
		/**
		 Get a new task id for this data manager ( not repeated with any previous task )
		 */
		
		size_t _getNewTaskId()
		{
			return task_counter++;
		}
		
	public:
		
		/**
		 Get a new task id for this data manager ( not repeated with any previous task )
		 */
		
		size_t getNewTaskId()
		{
			lock.lock();
			size_t id = _getNewTaskId();
			lock.unlock();
			return id;
		}
		
		
	private:

		friend class DataManagerItem;
		
		/**
		 Unique interface to update the status of this DataManager
		 This methods have to be implemented by any subclass
		 */

		virtual void _clear()=0;
		virtual void _beginTask( size_t task )=0;
		virtual void _cancelTask( size_t task )=0;
		virtual void _finishTask( size_t task )=0;
		virtual DataManagerCommandResponse _run( size_t task , std::string command )=0;

		
	private:
		
		// Static function to get "today" string
		static std::string today();

		// Reload data form file
		void _reloadData();
		
	};

}

#endif
