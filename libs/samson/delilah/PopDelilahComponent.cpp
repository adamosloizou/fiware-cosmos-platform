

#include <sys/stat.h>		// mkdir

#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/MemoryRequest.h"
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"							// engine::Buffer
#include "engine/Notification.h"                    // engine::Notificaiton
#include "engine/DiskOperation.h"

#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...

#include "PopDelilahComponent.h"                      // Own interface


namespace samson
{
    
    
#pragma mark pop
    
    PopDelilahComponent::PopDelilahComponent( std::string _queue , std::string _fileName , bool _force_flag , bool _show_flag ) : DelilahComponent( DelilahComponent::pop )
    {
        
        queue = _queue;
        
        fileName = _fileName;
        
        num_write_operations = 0;
        

        force_flag = _force_flag;
        show_flag = _show_flag;
        
        setConcept( au::str("Pop queue %s to local directory %s" , queue.c_str() , fileName.c_str() ) );
    }
    
    PopDelilahComponent::~PopDelilahComponent()
    {
    }
    
    // Function to get the status
    std::string PopDelilahComponent::getStatus()
    {
        std::ostringstream message;
        
        if ( error.isActivated() )
        {
            // Nothing since error is shown by the delilah
        }
        else
            message << "Completed " << num_finish_worker << " / " << workers.size() << " workers";
        
        return message.str();
    }
    
    
    void PopDelilahComponent::run()
    {

        if( force_flag )
        {
            au::ErrorManager error;
            au::removeDirectory( fileName , error );
            if( error.isActivated() )
                delilah->showWarningMessage( error.getMessage() );
        }
        
        if( mkdir( fileName.c_str() , 0755 ) )
        {
            
            setComponentFinishedWithError( au::str( "Not possible to create directory %s (%s)." , fileName.c_str() , strerror( errno ) ) );
            return;
        }
                
        // Send to all the workers a message to pop a queue
        workers = delilah->network->getWorkerIds();
        
        num_finish_worker = 0;
        
        for ( size_t w = 0 ; w < workers.size() ; w++ )
        {
            Packet *p = new Packet( Message::PopQueue );
            
            network::PopQueue *pq = p->message->mutable_pop_queue();
            
            pq->set_queue( queue );                     // Set the name of the queue
            
            p->message->set_delilah_component_id(id);             // Identifier of the component at this delilah
            
            // Information about direction
            p->to.node_type = WorkerNode;
            p->to.id = workers[w];

            // Send message
            delilah->network->send( p );
        }
        
    }
    
    void PopDelilahComponent::receive( Packet* packet )
    {
        if( packet->msgCode != Message::PopQueueResponse )
        {
            delete packet;
            return;
        }
        
        if( packet->buffer )
        {
            
            num_write_operations++;
         
            size_t worker_id = packet->from.id;
            int num_output = counter_per_worker.getCounterFor( worker_id );
            
            std::string _fileName = au::str("%s/worker_%06d_file_%06d" , fileName.c_str() , worker_id, num_output );

            engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation( packet->buffer , _fileName , getEngineId() );
            engine::DiskManager::shared()->add( operation );                
            
        }
        
        // If finished,
        if( packet->message->pop_queue_response().finish() )
            num_finish_worker++;
        
        if( workers.size() > 0 )
            setProgress( (double) num_finish_worker /  (double) workers.size() );
        
        // Check errors
        if( packet->message->pop_queue_response().has_error() )
            error.set( packet->message->pop_queue_response().error().message() );
        

        check();
    }
    
    
    void PopDelilahComponent::notify( engine::Notification* notification )
    {
        if( notification->isName(notification_disk_operation_request_response) )
        {
            // Delete operation
            engine::DiskOperation *operation = (engine::DiskOperation*) notification->extractObject();
            delete operation;

            num_write_operations--;
            check();
        }
        else
            LM_W(("Unexpected notification %s" , notification->getName() ));
            
    }
    
    void PopDelilahComponent::check()
    {
        if ( error.isActivated() )
            setComponentFinished();

        if( ( num_finish_worker == workers.size() ) && (num_write_operations==0) )
        {
            if( !isComponentFinished() )
            {
                // Finish operation
                setComponentFinished();
                

                
            }
        }
        
    }

    
	
	
}
