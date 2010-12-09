#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet
#include "Status.h"			// ss::getStatusFromArray(.)
#include "samson/Environment.h"			// ss::Environment
#include "Job.h"						// ss::Job

namespace ss
{

	size_t ControllerTaskManager::addTask( ControllerTaskInfo *info , size_t job_id  )
	{
		lock.lock();
		
		ControllerTask * t = new ControllerTask( current_task_id++ , job_id,  info , controller->network->getNumWorkers() );

		// Stak into internal map
		size_t id = t->getId();
		task.insertInMap( id , t ); 
		
		// Send this task to all the workers
		sendWorkerTasks( t );										
		
		lock.unlock();

		return t->getId();
		
	}
	
	void ControllerTaskManager::notifyWorkerConfirmation( int worker_id, network::WorkerTaskConfirmation* confirmationMessage )
	{
		// get the task id that is confirmed
		size_t task_id = confirmationMessage->task_id();

		bool task_finished =  false;
		bool error;
		std::string error_message;
		
		
		lock.lock();
		
		ControllerTask * t = task.findInMap( task_id );

		size_t job_id;
		
		if( t )
		{
			// Get the job id
			job_id = t->getJobId();
			
			// Notify that this worker has answered
			t->notifyWorkerConfirmation( worker_id , confirmationMessage , &controller->data );
			
			if( t->isFinish() )
			{
				task_finished =  true;
				
				error = t->error;
				error_message= t->error_message;
				
				// Delete this task from this manager
				t = task.extractFromMap( task_id );
				
			}
		}
		lock.unlock();

		
		// This has to be outputside the lock to avoid dead-lock
		if( task_finished )
		{
			// Notify the JobManager that this task is finish
			controller->jobManager.notifyFinishTask( job_id ,task_id , error , error_message );
			delete t;

		}
		
		

		
	}
	
	
	
	std::string ControllerTaskManager::getStatus()
	{
		/*
		std::stringstream o;
		o << getStatusFromArray( task );
		return o.str();
		*/
		return "Error";
	}
	
	
	/* ****************************************************************************
	 *
	 * sendWorkerTasks - 
	 */
	void ControllerTaskManager::sendWorkerTasks( ControllerTask *task )
	{
		// Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
		
		for (int i = 0 ; i < controller->network->getNumWorkers() ; i++)
			sendWorkerTask(i, task->getId(), task);
	}	
	
	
	
	/* ****************************************************************************
	 *
	 * sendWorkerTask - 
	 */
	void ControllerTaskManager::sendWorkerTask(int workerIdentifier, size_t task_id, ControllerTask *task  )
	{
		// Get status of controller
		Packet *p2 = new Packet();
		
		network::WorkerTask *t = p2->message.mutable_worker_task();
		t->set_task_id(task_id);
		
		// Fill information for this packet ( input / outputs )
		task->fillInfo( t , workerIdentifier );
		
		// special flag used in generators
		t->set_generator( task->generator == workerIdentifier );	
		
		
		
		controller->network->send(controller,  controller->network->workerGetIdentifier(workerIdentifier) , Message::WorkerTask,  p2);
	}
	
	
	
}
