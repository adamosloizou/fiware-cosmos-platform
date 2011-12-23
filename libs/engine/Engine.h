
/* ****************************************************************************
 *
 * FILE            Engine.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Engine: Singlenton object with all the main funcitonalities of the engine library
 *
 * ****************************************************************************/

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                         // std::cout
#include <set>                              // std::set

#include "au/list.h"                        // au::list
#include "au/Token.h"                       // au::Token
#include "au/Cronometer.h"                  // au::Cronometer
#include "au/namespace.h"                // NAMESPACE_BEGIN & NAMESPACE_END

#include "engine/EngineService.h"
#include "engine/Object.h"                  // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager

NAMESPACE_BEGIN(engine)
class Error;
class Token;
NAMESPACE_END

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

/**
 Main engine platform
 */

class Engine : public EngineService
{
    au::list<EngineElement> elements;               // Elements of the samson engine
    
    EngineElement *running_element;                 // Element that is currently running
    
    au::Token token;                                // General mutex to protect global variable engine and block the main thread if necessary
    
    pthread_t t;                                    // Thread to run the engine in background ( if necessary )
    bool flag_finish_threads;                       // Flag used to indicate to threads that engine will finish
    
    size_t counter;                                 // Counter of EngineElement processed
    
    ObjectsManager objectsManager;                  // Management of objects ( notification )
    
    Engine();
    
public:
    
    au::Cronometer uptime;                          // Total up time
    
    ~Engine();
    
    static void init();
    static void reset();
    static Engine* shared();
    
public:
    
    // Methods only executed from the thread-creation-functions ( never use directly )
    void run();
    
private:
    
    // Find the position in the list to inser a new element
    std::list<EngineElement*>::iterator _find_pos( EngineElement *e);
    
    
public:
    
    // get xml information
    void getInfo( std::ostringstream& output);
    
private:
    
    friend class Object;
    
    // Functions to register objects ( general and for a particular notification )
    void register_object( Object* object );
    void register_object_for_channel( Object* object, const char* channel );
    
    // Generic method to unregister an object
    void unregister_object( Object* object );
    
public:
    
    // Add a notification
    void notify( Notification*  notification );
    void notify( Notification*  notification , int seconds ); // Repeated notification
    
    // Function to add a simple foreground tasks 
    void add( EngineElement *element );	
    
    // Get an object by its registry names
    Object* getObjectByName( const char *name );
    
public:
    
    void quitEngineService();

private:
    
    friend class NotificationElement;
    
    // Run a particular notification
    // Only executed from friend class "NotificationElement"
    void send( Notification * notification );
    
    
};

NAMESPACE_END

#endif
