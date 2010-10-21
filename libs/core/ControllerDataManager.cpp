

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat
#include "ControllerQueue.h"				// ss::ControllerQueue
#include "au_map.h"							// au::insertInMap

namespace ss {

	void ControllerDataManager::updateWithFinishedTask( ControllerTask *task )
	{

		lock.lock();
		

		// Confirm that task has finished
		
		
		lock.unlock();
	}
	
	bool ControllerDataManager::_run( size_t task_id , std::string command )
	{
		au::CommandLine  commandLine;
		commandLine.parse( command );
		
		if( commandLine.get_num_arguments() == 0)
			return true;
		
		if( commandLine.get_argument(0) == "add_queue" )
		{
			std::string name = commandLine.get_argument(1);
			KVFormat format = KVFormat::format( commandLine.get_argument(2) , commandLine.get_argument(3) );
			
			ControllerQueue *tmp = new ControllerQueue(name , format);
			au::insertInMap( queues , name , tmp );
		}
		
		return true;
	}
	
	
	
	std::string ControllerDataManager::getLogFileName(   )
	{
		std::ostringstream fileName;
		fileName << "/var/samson/log/log_controller";
		return fileName.str();
	}
	
	std::string ControllerDataManager::status()
	{
		std::ostringstream o;
		o << "Data Manager:" << std::endl;
		lock.lock();
		for ( std::map< std::string , ControllerQueue*>::iterator q = queues.begin() ; q != queues.end() ; q++)
			o << q->first << " " << q->second->str() << std::endl;
		lock.unlock();
		
		return o.str();
		
	}		
	
	

}
