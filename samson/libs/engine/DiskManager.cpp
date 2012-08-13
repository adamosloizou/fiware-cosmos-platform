

#include <time.h>
#include <sys/time.h>

#include "logMsg/logMsg.h"				// LM_X

#include "au/xml.h"         // au::xml...
#include "au/mutex/TokenTaker.h"         // au::TokenTaker...
#include "au/ThreadManager.h"

#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"      // engine::EngineNotificationElement
#include "au/Descriptors.h"                         // au::Descriptors

#include "engine/DiskManager.h"                    // Own interface


NAMESPACE_BEGIN(engine)


//iniitalise singleton instance pointer to NULL
DiskManager* DiskManager::diskManager = NULL;

void DiskManager::init( int _num_disk_operations )
{
    LM_V(("DiskManager init with %d max background operations" , _num_disk_operations ));
    
    if( diskManager )
	{
        LM_W(("Please, init diskManager only once. Ignoring..."));
		return;
	}
    
    diskManager = new DiskManager ( _num_disk_operations );
}

void DiskManager::stop( )
{
    if( diskManager )
        diskManager->quitting = true;
}


void DiskManager::destroy( )
{
    LM_V(("DiskManager destroy"));

    if( !diskManager )
    {
        LM_W(("Please init diskManager before destroying it"));
        return;
    }
    delete diskManager;
    diskManager = NULL;
}


DiskManager* DiskManager::shared()
{
    if (!diskManager)
        LM_E(("Please init DiskManager before using it, calling DiskManager::init()"));
    return diskManager;
}

#pragma mark DiskManager

void* run_disk_manager_worker( void* p )
{
    DiskManager* dm = (DiskManager*) p;
    dm->run_worker();
    return NULL;
}

DiskManager::DiskManager( int _num_disk_operations ) : token("engine::DiskManager")
{
    // Number of parallel disk operations
    num_disk_operations = _num_disk_operations;
    
    // Init counter for number of workers
    num_disk_manager_workers = 0;
    
    quitting = false;

    // Create as many threads as required
    createThreads();

}


void DiskManager::createThreads()
{
    // Create as many workers as necessary
    while( get_num_disk_manager_workers() < num_disk_operations )
    {
        // Increse the counter of worker
        add_worker();
        
        pthread_t t;
        au::ThreadManager::shared()->addThread("DiskManager_worker",&t, NULL, run_disk_manager_worker, this);
    }

}

DiskManager::~DiskManager()
{
    // Remove pending elements
    pending_operations.clearList();
}

void DiskManager::add( DiskOperation *operation )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    // Set the pointer to myself
    operation->diskManager = this;

    // Retain operation
    operation->retain();
    
    // Insert the operation in the queue of pending operations
    pending_operations.insert( _find_pos(operation), operation );
    
}

void DiskManager::cancel( DiskOperation *operation )
{
    au::TokenTaker tt(&token);
    if( pending_operations.extractFromList( operation ) )
    {
        // Add a notification for this operation ( removed when delegate is notified )
        Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
        notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
        Engine::shared()->notify(notification);            
        
        // Release object
        operation->release();
    }
}

void DiskManager::finishDiskOperation( DiskOperation *operation )
{
    // Callback received from background process
    
    // Mutex protection
    au::TokenTaker tt(&token);
    
    running_operations.erase( operation );
    
    
    if( operation->getType() == DiskOperation::read )
        rate_in.push( operation->getSize() );
    if( operation->getType() == DiskOperation::write )
        rate_out.push( operation->getSize() );
    if( operation->getType() == DiskOperation::append )
        rate_out.push( operation->getSize() );
    
    LM_T( LmtDisk , ("DiskManager::finishDiskOperation erased and ready to send notification on file:%s", 
                     operation->fileName.c_str() 
                     ));
    
    // Add a notification for this operation to the required target listener
    Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
    notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
    
    LM_T( LmtDisk , ("DiskManager::finishDiskOperation notification sent on file:%s and ready to share and checkDiskOperations", operation->fileName.c_str() ));
    Engine::shared()->notify(notification);    
    
    // Release object
    operation->release();
}


DiskOperation * DiskManager::getNextDiskOperation(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    if( pending_operations.size() == 0 )
    {
        on_off_monitor.set_on( !(running_operations.size() == 0) );
        return NULL;
    }
    
    // Extract the next operation
    DiskOperation *operation = pending_operations.extractFront();
    
    // Insert in the running list
    running_operations.insert( operation );

    // We are runnin one, so it is on
    on_off_monitor.set_on(true);
    
    return operation;
}


int DiskManager::get_num_disk_manager_workers(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return num_disk_manager_workers;
}


void DiskManager::add_worker(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    num_disk_manager_workers ++;
}


// Check if we can run more disk operations
void DiskManager::run_worker()
{
    
    while( true )
    {
        
        
        // Check if I should quit
        if( check_quit_worker() )
            return;
        
        DiskOperation* operation = getNextDiskOperation();
        if (operation)
		{
            operation->run();
		}
        else
            usleep(100000);
    }
    
    
}

bool DiskManager::check_quit_worker(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);

    // If quitting the disk manager.. just quit.
    if (quitting)
    {
        num_disk_manager_workers--;
        return true;
    }

    if( num_disk_manager_workers > num_disk_operations )
    {
        num_disk_manager_workers--;
        return true;
    }
    
    return false;
    
}

int DiskManager::getNumOperations()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    return pending_operations.size() + diskManager->running_operations.size();
}

void DiskManager::setNumOperations( int _num_disk_operations )
{
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        num_disk_operations = _num_disk_operations;
    }
    
    // Create as many threads as required
    createThreads();
    
}

size_t DiskManager::get_rate_in()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return rate_in.getRate();
}

size_t DiskManager::get_rate_out()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return rate_out.getRate();
}

double DiskManager::get_rate_operations_in()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return rate_in.getHitRate();
}

double DiskManager::get_rate_operations_out()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return rate_out.getHitRate();
}

double DiskManager::get_on_off_activity()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return on_off_monitor.getActivity();
}


void DiskManager::getInfo( std::ostringstream& output)
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    output << "<disk_manager>\n";
    
    au::xml_simple( output , "num_pending_operations" , pending_operations.size() );
    au::xml_simple( output , "num_running_operations" , pending_operations.size() );
    
    output << "<running>\n";
    for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
        (*i)->getInfo(output);
    output << "</running>\n";
    
    output << "<queued>\n";
    for ( std::list<DiskOperation*>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
        (*i)->getInfo(output);
    output << "</queued>\n";
    
    output << "</disk_manager>\n";
    
}

au::list<DiskOperation>::iterator DiskManager::_find_pos( DiskOperation *diskOperation )
{
    for (au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
    {
        if( diskOperation->compare(*i) )
            return i;
    }
    return pending_operations.end();
}
 
NAMESPACE_END