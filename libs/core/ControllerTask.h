#ifndef _H_CONTROLLER_TASK
#define _H_CONTROLLER_TASK

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// std::ostringstream
#include <iostream>							// std::cout
#include "samson.pb.h"						// network::...
#include <set>								// std::set

namespace ss {
	
	class SamsonController;
	
	/**
	 Task at the controller
	 Managed by ControllerTaskManager
	 */
	
	class ControllerTask
	{
		friend class ControllerDataManager;
		
		size_t id;			// Id of the task ( shared by all the workers )
		size_t job_id;		// Id of the job it belongs
		
		// Main command line
		std::string command;
		
		int total_workers;	// Total workers that have to confirm the task
		std::vector<network::WorkerTaskConfirmation> confirmationMessages;		// All confirmation messages received for this task
		
		friend class ControllerTaskManager;
		
	public:
		
		// Children controller tasks
		ControllerTask( size_t _id , size_t _job_id,  std::string _command , int _total_workers )
		{
			// Keep the command and the id
			id = _id;
			job_id =_job_id;
			command = _command;

			// total number of workers to wait for this number of confirmation ( in case we sent to workers )
			total_workers = _total_workers;
			
		}
		
		size_t getId()
		{
			return id;
		}
		
		size_t getJobId()
		{
			return job_id;
		}
		
		void notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation confirmationMessage );
		
		std::string getCommand()
		{
			return command;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "Task " << id << " : " << command;
			return o.str();
		}
		
		
		bool isFinish()
		{
			return ( (int)(confirmationMessages.size() )  == total_workers );
		}
		
	};
	

	
}

#endif

	
