#ifndef _H_DELILAH_BASE
#define _H_DELILAH_BASE

/* ****************************************************************************
 *
 * FILE                     DelilahBase.h
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */


#include <string>                       // std::string
#include <vector>                       // std::vector

#include "au/containers/map.h"                   
#include "au/mutex/Token.h"                   // au::Token
#include "au/Cronometer.h"              // au::Cronometer
#include "au/console/ConsoleAutoComplete.h"

#include "au/tables/DataBase.h"

#include "samson/network/NetworkInterface.h"


#include "au/tables/pugi.h"                  // pugi::... node_to_string_function

namespace samson {
    
    /*
     Class to hold monitorization data for delilah components ( delilah / monitorization )
     */
    
    
    class XMLStringHolder
    {
        au::Cronometer cronometer;     // Cronometer used to monitorize the updated time
        std::string xml_data;          // xml content
        size_t counter;                   // Number of times this has been updated
        
    public:
        
        XMLStringHolder()
        {
            counter = 0;
        }
        
        au::tables::TreeItem* update( std::string _txt )
        {
            cronometer.reset();
            counter++;
            
            xml_data = _txt;    // Keep the xml document just for debuggin
            
            return au::tables::TreeItem::getTreeFromXML( _txt );            
        }
        
        size_t getTime()
        {
            return cronometer.diffTimeInSeconds();
        }
        
        size_t getNumUpdated()
        {
            return counter;
        }
        
        
    };
    
    class DelilahBase
    {

        // XML data holders for workers and this delilah
        au::map<size_t, XMLStringHolder > workers;
        
        // XML data holders for this delilah
        XMLStringHolder* delilah;
    
    public:
        
        // Data base used to keep all monitorization information
        au::tables::DataBase database;
        
    public:

        // Network interface
		NetworkInterface* network;								
        
        DelilahBase( );
        ~DelilahBase();
        
        // Update internal monitorization from all elements
        void updateDelilahXMLString( std::string txt );
        void updateWorkerXMLString( size_t worker_id , std::string txt );

        // Own funciton to get xml content
        virtual void getInfo( std::ostringstream& output )=0; 
        
        // Get the max time since the last monitorization update
        int getUpdateSeconds( );

        // Simple queries to get a list of operations, queues, etc...
        std::vector<std::string> getOperationNames( );        
        std::vector<std::string> getOperationNames( std::string type );        
        
        std::vector<std::string> getQueueNames();
        std::vector<std::string> getQueueNames( KVFormat );

        // String with information about updates
        std::string updateTimeString();

        // Run a command over database
        std::string runDatabaseCommand( std::string command);

        // Autocomplete for database mode
        void autoCompleteForDatabaseCommand( au::ConsoleAutoComplete* info );
        
    };
}


#endif
    
