/* ****************************************************************************
 *
 * FILE            ObjectsManager
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Managert of all the objets in engine library
 *   It keep tracks of all the objects with a map for fast notification delivery
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_OBJECTS_MANAGER
#define _H_ENGINE_OBJECTS_MANAGER


#include <string>       // size_t

#include "au/map.h"     // au::map
#include "au/Token.h"     // au::Token

namespace engine {
    
    class Object;
    class Notification;
    
    // IdsCollection: collection of ids to send notifications
    
    class IdsCollection
    {
        
    public:
        
        std::set<size_t> ids;
        
        void add( size_t id )
        {
            ids.insert( id );
        }
        
        void remove( size_t id )
        {
            ids.erase( id );
        }
        
    };
    
    /**
     
     Global class to manage all the objects controlled by "Engine"
     
     **/
    
    class ObjectsManager
    {
        
        // Internal variable to create new objects
        size_t engine_id;
        
        // Map of all the objects by id
        au::map < size_t , Object > objects;
        
        // Map of ids object per notification channel
        au::map< const char* , IdsCollection , au::strCompare > channels;

        // Map of objects by name
        au::map< const char* , Object , au::strCompare > objects_by_name;
        
    public:
        
        ObjectsManager();
        ~ObjectsManager();

        // Add and remove Objects
        void add( Object* o );
        void remove( Object* o );

        // Add and remove objects to channels
        void add( Object* o , const char* name );
        void remove(Object* o , const char* name );

        // Send a notification
        void send( Notification* notification ); 

        // Get the object registered with this name
        Object* getObjectByName( const char *name );
        
    private:
        
        // Get the collections of ids for a particular channel
        IdsCollection* get( const char* name );

        // Internal funciton to send a notification to a particular target-objetc ( if exist )
        void send( Notification *notification, size_t target );
        
        
    };

    
}


#endif