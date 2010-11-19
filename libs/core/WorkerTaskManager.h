#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>						// std::list

#include "au_map.h"					// au::map
#include "samson.pb.h"				// WorkerTask
#include "ObjectWithStatus.h"
#include "Lock.h"					// au::Lock
#include "samson.pb.h"				// ss::network::...

namespace ss {

	class SamsonWorker;
	class DataBufferItem;
	
	/**
	 Class describing a part of a task.
	 This item can be executed by a particular Process
	 */
	
	class WorkerTaskItem
	{
	public:
		
		size_t task_id;	// Global task id
		int item_id;	// Item id in this worker
			
		std::string operation;					// Name of the operation

		std::vector<std::string> outputs_names;	// Queues to emit key-values ( names from command line )
		std::vector<network::Queue> outputs;	// Queues to emit key-values ( inserted at data manager )
		
		enum State
		{
			definition,
			ready,
			running
		};
		
		State state;
		
		
		WorkerTaskItem( size_t _task_id , size_t _item_id ,  const network::WorkerTask &task )
		{
			operation = task.operation();
			
			for (int i = 0 ; i < task.output_size() ; i++)
				outputs.push_back( task.output(i) );
			
			task_id = _task_id;
			item_id = _item_id;
			
			state = definition;
			state = ready;	// To be automatically accepted now
			
		}
		
		State getState()
		{
			return state;
		}
		
		void startProcess()
		{
			assert( state == ready );
			state = running;
		}
		
		bool isReady()
		{
			return (state == ready);
		}
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "\t\tTask Item (Task: " << task_id << ") Item: " << item_id << " ";
			
			switch (state) {
				case definition:
					output << " in definition ";
					break;
				case ready:
					output << " ready ";
					break;
				case running:
					output << " running ";
					break;
				default:
					break;
			}
			
			return output.str();
		}
		
	};
	
	class WorkerTask
	{
		
	public:
		
		size_t task_id;
		
		bool error;
		std::string error_message;
		
		std::string operation;					// Operation to be executed
		au::map<size_t,WorkerTaskItem> item;	// List of items to be executed by processes

		int num_finish_items;
		
		WorkerTask( const network::WorkerTask &task )
		{
			operation = task.operation();	// Save the operation to perform		
			task_id = task.task_id();		// Save the task id
			
			// By default no error
			error = false;
			
			// rigth not for demo, we create 10 items to be executed
			for (size_t i = 0 ; i < 1 ; i++)
				item.insertInMap( i , new WorkerTaskItem( task_id , i ,  task ) );
		}

		// Get the next item ( if any )
		
		WorkerTaskItem *getNextItemToProcess()
		{
			std::map<size_t,WorkerTaskItem*>::iterator iterator;
			for (iterator = item.begin() ; iterator != item.end() ; iterator++)
			{
				WorkerTaskItem *item = iterator->second;
				if( item->isReady() )
					return item;
			}
				
			return NULL;
		}
		
		void finishItem( size_t id , bool _error , std::string _error_message );
		
		bool isFinish()
		{
			return ( item.size() == 0);	// No more items to process
		}
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "\tTask " << task_id << " Operation: " << operation << std::endl;
			output << getStatusFromArray( item );
			return output.str();
		}
		
		
		
	};
	
	class WorkerTaskManager
	{
		SamsonWorker *worker;

		au::map<size_t,WorkerTask> task;	// List of tasks
		
		au::Lock lock;			// General lock to protect multiple access ( network thread & Process threads )
		au::StopLock stopLock;	// Stop lock to block the Process when there is no process to run
		
	public:
		
		WorkerTaskManager(SamsonWorker *_worker) : stopLock( &lock )
		{
			worker = _worker;
		}

		// Add a task to the local task manager ( map / reduce / generator )
		void addTask( const network::WorkerTask &task );

		// Method called from the ProcessAssitant to get the next element to process
		WorkerTaskItem *getNextItemToProcess();
		
		// Method called by ProcessAssitant when a particula process is finished
		
		void finishItem( WorkerTaskItem *item , bool error, std::string error_message );

		// Noitification received from the DataBuffer when everything is saved to disk
		void completeItem( size_t task_id , DataBufferItem * item );

		
		std::string getStatus()
		{
			std::ostringstream output;
			output << getStatusFromArray( task );
			return output.str();
		}
		
	private:
		
		// Function used to send the confirmation of a task to the controller
		void sendWorkTaskConfirmation( WorkerTask *t );
		
		
		
	};
}

#endif
