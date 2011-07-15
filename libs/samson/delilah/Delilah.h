#ifndef DELILAH_H
#define DELILAH_H

/* ****************************************************************************
*
* FILE                     Delilah.h
*
* DESCRIPTION			   Client application for Samson
*
*/
#include <iostream>				// std::cout
#include <set>					// std::set

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "au/Token.h"				// au::Lock
#include "au/map.h"				// au::map
#include "au/Cronometer.h"      // au::Cronometer
#include "au/Info.h"                // au::Info

#include "samson/module/Environment.h"	// samson::Environment
#include "samson/common/samson.pb.h"			// samson::network::..

#include "engine/Object.h" // engine::Object
#include "engine/Object.h"          // engine::Object

#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/traces.h"				// TRACE_DALILAH

#include "samson/network/Network.h"			// NetworkInterface
#include "samson/network/Message.h"            // Message::MessageCode
#include "samson/network/Endpoint.h"			// Endpoint



namespace  engine {
    class Buffer;
}

namespace samson {
	
    
	// Thread method
	class DelilahClient;
	class DelilahComponent;
	class DelilahUploadDataProcess;
	class DelilahDownloadDataProcess;
    class PushComponent;
    class PopComponent;
    
	extern au::Lock info_lock;						// Lock to protect the information provided here
    
	extern network::OperationList *ol;              // List of available opertions ( updated periodically for autocompletion )
	extern network::QueueList *ql;                  // List of queues in the system ( updated periodically for autocompletion )

    extern network::SamsonStatus *samsonStatus;     // Global status information of the platform            
    extern au::Cronometer cronometer_samsonStatus;  // Cronometer for this updated message
	
    
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface , public engine::Object
	{
		// Id counter of the command - messages sent to controller ( commands / upload/ download )
		size_t id;												
		
		// Private token to protect the local list of components
		au::Token token;
        
		// Map of components that intercept messages
		au::map<size_t , DelilahComponent> components;			
		
        protected:
        
        bool trace_on;

        au::Info* info;  // Global info from SAMSON SYSTEM
        
	public:
		
		Environment environment;								// Environment properties to be sent in the next job

		NetworkInterface* network;								// Network interface
		bool              finish;								// Global flag used by all threads to detect to stop

		
		// Full information everything in samson controller ( updated with updateInfo() )
		
	public:
		
		Delilah( NetworkInterface *_network ,  bool automatic_update );
		~Delilah();
        
		void initController(std::string controller);
		void quit();

        // Notification system
        void notify( engine::Notification* notification );
        
		// PacketReceiverInterface
		void receive( Packet* packet );

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
		size_t addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion, int _max_num_threads);
		size_t addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen );
        size_t addPushData( std::vector<std::string> fileNames , std::string queue );
        size_t addPopData( std::string queue , int channel, std::string parserOut , std::string fileName );

		size_t sendCommand( std::string command , engine::Buffer *buffer );
		
		
		// Check a particular if
		bool isActive( size_t id );
		
	public:
				
		/** 
		 Methonds implemented by subclasses
		 */
		
		// Function to be implemented by sub-classes to process packets ( not handled by this class )
		virtual int _receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// A load data process has finished
		virtual void pushConfirmation( PushComponent *process ){};

		// A load data process has finished
		virtual void popConfirmation( PopComponent *process ){};
        
		// A load data process has finished
		virtual void uploadDataConfirmation( DelilahUploadDataProcess *process ){};

		// A download process has finished
		virtual void downloadDataConfirmation( DelilahDownloadDataProcess *process ){};
		
		// Write something on screen
		virtual void showMessage( std::string message)
        {
        }

		// Show traces  ( by default it does nothing )
		virtual void showTrace( std::string message){};
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t )
        {
        }
		
		// Get info about the list of loads
		std::string getListOfLoads();
        std::string getListOfComponents();
        
	protected:
		
		void clearComponents();
        void clearAllComponents();  // Force all of them to be removed
		
	private:		
		
		size_t addComponent( DelilahComponent* component );
        
        void _receive_global_update( Packet *packet );
        
		
	};
}

#endif
