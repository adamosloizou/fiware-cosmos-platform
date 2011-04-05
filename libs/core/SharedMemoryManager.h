/* ****************************************************************************
 *
 * FILE                     SharedMemoryManager.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_SHARED_MEMORY_MANAGER
#define _H_SHARED_MEMORY_MANAGER

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <set>						// std::set
#include <list>						// std::list
#include <iostream>					// std::cout
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE


#include "Buffer.h"					// ss::Buffer
#include "Token.h"					// au::Token
#include "Stopper.h"				// au::Stopper
#include "au_map.h"					// au::map
#include "Format.h"					// au::Format

#include "samson.pb.h"				// network::..


// #define SS_SHARED_MEMORY_KEY_ID	872934	// Not used any more since now, IPC_PRIVATE shared memory can be created

namespace ss {
       
    class SharedMemoryItem;
    
	/**
	 
	 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
	 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
	 It is also responsible to manage shared-memory areas to share between differnt processes. 
	 Shared memory is used to isolate operations from main process
	 
	 */
    
    class SharedMemoryManager;
    
    
	class SharedMemoryManager 
	{
            
        
		au::Token token;                                    // Token to protect this instance and memoryRequests
	
		// Shared memory management
		size_t shared_memory_size_per_buffer;               // Shared memory used in eery buffer
		int shared_memory_num_buffers;                      // Number of shared memory buffers to create
		bool* shared_memory_used_buffers;                   // Bool vector showing if a particular shared memory buffer is used
        int * shm_ids;                                      // Vector containing all the shared memory identifiers
		
		
        std::string sharedMemoryIdsFileName;
        
	public:
		

		SharedMemoryManager();
		~SharedMemoryManager();
		
		/**
		 Function to retain and release a free shared-memory area
		 */

		int retainSharedMemoryArea();
		void releaseSharedMemoryArea( int id );

		/**
		 Functions to get and release a shared memory area
		 */
		
		SharedMemoryItem* getSharedMemory( int i );
		void freeSharedMemory(SharedMemoryItem* item);

        /*
         Function to create shared memory segments.
         */
        
        void createSharedMemorySegments( );
        
        static void removeSharedMemorySegments( int *ids , int length );
        
        // Function to work with files
        void removeSharedMemorySegmentsOnDisk( );
        
        
		// Init and destroy functions
        static void init();
        static SharedMemoryManager* shared();
        
        
        // Status report
		void fill( network::WorkerStatus* ws);

	};
	
};

#endif
