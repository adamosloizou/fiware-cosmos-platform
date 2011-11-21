

#include "engine/ProcessManager.h"



#include "logMsg/logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "au/Descriptors.h"                         // au::Descriptors
#include "au/TokenTaker.h"                          // au::TokenTake
#include "au/xml.h"         // au::xml...

#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"                  // engine::Process
#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement

namespace engine
{
    
    static ProcessManager *processManager=NULL;
    
    void destroy_process_manager()
    {
        //LM_M(("ProcessManager terminating"));
        
        if( processManager )
        {
            delete processManager;
            processManager = NULL;
        }
    }
    
    void ProcessManager::init( int _num_processes)
    {
        if( processManager )
            LM_X(1,("Please, init processManager only once"));
        processManager = new ProcessManager( _num_processes );
    }

    void ProcessManager::destroy( )
    {
        if( !processManager )
            LM_X(1,("Please, init processManager before destroying"));
        delete processManager;
    }
    
    
    ProcessManager* ProcessManager::shared()
    {
        if( !processManager )
            LM_X(1, ("ProcessManager not initialiazed"));
        
        return processManager;
    }

    ProcessManager::ProcessManager( int _num_processes ) : token("engine::ProcessManager")
    {
        // By default only one process at a time
        num_processes = _num_processes;  
        
		// Add the processManager as a listener for process request
        listen( notification_process_manager_check_background_process );
     
        // Check every second if we should start a new process item
        engine::Engine::shared()->notify( new Notification( notification_process_manager_check_background_process ) , 1 );
    }
    
    ProcessManager::~ProcessManager()
    {
    }
    
    void ProcessManager::notify( Notification* notification )
    {
        
        if( notification->isName( notification_process_manager_check_background_process ) )                 
            checkBackgroundProcesses();
        else        
	  LM_X(1,("Wrong notification at ProcessManager [Listener %lu] %s" , getEngineId() , notification->getDescription().c_str()));
        
    }
    
    void ProcessManager::finishProcessItem( ProcessItem *item )
    {
        // Protect multi-thread access
        au::TokenTaker tt( &token );
        
        
        // Statistics
        if ( ( item->operation_name != "unknown" ) && ( item->working_size > 0 ) )
        {
            std::string name = item->operation_name;
            size_t working_size = item->working_size;
            int time_in_seconds = item->cronometer.getSeconds();
            
            LM_T(LmtProcessManager, ("Reported finish to task '%s' in %d seconds with size %lu", name.c_str() , time_in_seconds , working_size ));
            au::SimpleRateCollection * simple_rate = rates.findOrCreate( name , name );
            simple_rate->push( working_size , time_in_seconds );
        }
        
        running_items.erase(item);

        // Send to me a notification to review background processes
        Engine::shared()->notify( new Notification( notification_process_manager_check_background_process ) );
        
        // Notify this using the notification Mechanism
        {
            Notification * notification = new Notification( notification_process_request_response , item , item->listeners );
            notification->environment.copyFrom( &item->environment );
            Engine::shared()->notify( notification );
        }
        
    }
    
    void ProcessManager::haltProcessItem( ProcessItem *item )
    {
        // Protect multi-thread access
        au::TokenTaker tt( &token );
        
        running_items.erase(item);
        halted_items.insert(item);
        
        {
            Notification * notification = new Notification( notification_process_manager_check_background_process );
            Engine::shared()->notify( notification );
        }
        
    }   
    
    void ProcessManager::add( ProcessItem *item , size_t listenerId  )
    {
        // We mae sure, items always come with a listenerId
        item->addListenerId( listenerId );
        
        // Protect multi-thread access
        au::TokenTaker tt( &token );
        
        LM_T( LmtProcessManager , ("Adding ProcessItem") );
        
        // set the pointer to myself
        item->processManager = this;
        
        items.insert( item );
        
        LM_T( LmtProcessManager , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
        
        LM_T( LmtProcessManager , ("Finish Adding ProcessItem") );

        // A notification will check if it is necessary to run a new process item
        engine::Engine::shared()->notify( new Notification( notification_process_manager_check_background_process ) );
        
    }
    
    void ProcessManager::cancel( ProcessItem *item )
    {
        // Protect multi-thread access
        au::TokenTaker tt( &token );

        //LM_M(("Removing process item %p ",item));
        
        if ( items.extractFromSet( item ) )
        {
            //LM_M(("Removing item %p since it was still in the queue",item));

            // Set this process as error
            item->error.set( "ProcessItem canceled" );
            
            // Notify this using the notification Mechanism
            {
                Notification * notification = new Notification( notification_process_request_response , item , item->listeners );
                notification->environment.copyFrom( &item->environment );
                notification->environment.set("error", "Canceled" );
                Engine::shared()->notify( notification );
            }
            
        }
        else
        {
            //LM_M(("Flag item %p as cancel since it was NOT still in the queue",item));
            item->setCanceled();    // Activate the canceled flag to make the process kill itself when possible
        }
        
    }
    
    void ProcessManager::checkBackgroundProcesses()
    {
        // Protect multi-thread access
        au::TokenTaker tt( &token );
        
        // Get the next process item to process ( if CPU slots available )
        ProcessItem * item = NULL;
        
        do
        {
            
            item = NULL;
            if( (int)running_items.size() < num_processes )
            {
                // Space for another process running... get the item to be executed or continued
                item = _getNextItemToRun();
            }
            
            if( item )
            {
                running_items.insert( item );	// Insert in the set of running processes
                
                switch (item->state) {
                    case ProcessItem::queued:
                        item->state = ProcessItem::running;
                        item->runInBackground();
                        break;
                    case ProcessItem::halted:
                        item->state = ProcessItem::running;
                        item->unHalt();
                        break;
                    default:
                        LM_X(1,("Unexpected state running item at Engine"));
                        break;
                }
            }
            
        } while ( item != NULL);
        
        LM_T( LmtEngine , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
        
    }
    
    // Function ONLY used inside checkBackgroundProcesses, so it is protected by the its token taker
    
    ProcessItem* ProcessManager::_getNextItemToRun()
    {
        
        ProcessItem* item = NULL;
        
        // Halted process comes first 
        
        for (std::set<ProcessItem*>::iterator i = halted_items.begin() ; i != halted_items.end() ; i++ )
        {
            if( (*i)->isReady() )
            {
                item = *i;
                halted_items.erase( item );
                return item;
            }
        }
        
        // If not process in the halt list is ready,
        // we get the highest priority element in the queue of pending processes
        
        for ( std::set<ProcessItem*>::iterator i =  items.begin() ; i!= items.end() ; i++)
        {
            if( (*i)->isReady() )   // Let's check if the process is ready to be executed
            {
                if( !item  )
                    item = *i;
                else
                {
                    if( (*i)->priority > item->priority )
                        item = *i;
                }
            }
        }
        
        if( item )
            items.erase( item );	// Remove form the pending list
        
        // It is null if no process is required to be executed
        return item;
        
    }	

    
    int ProcessManager::getNumCores()
    {
        if( processManager )
            return processManager->num_processes;
        else
            return 0;
        
    }
    
    int ProcessManager::getNumUsedCores()
    {
        if( processManager )
            return processManager->running_items.size();
        else
            return 0;
    }
    

    void ProcessManager::getInfo( std::ostringstream& output)
    {
        au::xml_open(output, "process_manager");

        au::xml_iterate_list(output, "queued", items);

        au::xml_iterate_list(output, "running", running_items);

        au::xml_iterate_list(output, "halted", halted_items);

        // List of rates
        
        au::xml_iterate_map( output , "rates", rates );
        
        // General information
        
        au::xml_simple( output , "num_processes" ,  num_processes );
        au::xml_simple( output , "num_running_processes" ,  running_items.size() );
        
        au::xml_close(output, "process_manager");
        
    }
    
    bool ProcessManager::hasFreeCores()
    {
        au::TokenTaker tt( &token );

        size_t total_process = running_items.size() + items.size() + halted_items.size();
        
        return( total_process < (size_t) num_processes );
    }
    
    void ProcessManager::quit()
    {
        
		items.clearSet();				// List of items to be executed ( all priorities  )
		halted_items.clearSet();		// Set of items currently being executed but halted
        
        while( running_items.size() > 0 )
        {
            LM_M(("Waiting to finish running background processes"));
            sleep(1);
        }
        
    }
    
}
