



#ifndef _H_SAMSON_DELILAH_NETWORK
#define _H_SAMSON_DELILAH_NETWORK

#include "engine/EngineElement.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/CommonNetwork.h"

namespace samson {
    
    
    class DelilahNetwork : public CommonNetwork
    {
        
        std::string host;
        int port;
        
    public:
        
        DelilahNetwork( std::string connection_type , size_t delilah_random_code );
        
        // NetworkManager interface
        // ------------------------------------------------------------
        void processHello( NetworkConnection* connection, Packet* packet );
        
        // NetworkInterface
        // ------------------------------------------------------------
        bool ready()
        {
            if ( cluster_information.getId() == 0 )
                return false;
            
            // Check all workers connected...
            std::vector<size_t> workers = cluster_information.getWorkerIds();
            for ( size_t i = 0 ; i < workers.size() ; i++ )
            {            
                std::string name = NodeIdentifier( WorkerNode , workers[i] ).getCodeName();
                if( ! NetworkManager::isConnected( name ) )
                    return false;
            }
            return true;
        }
        
        void stop()
        {
            // Close all connections
            NetworkManager::reset();
        }
        
        std::string getLoginInfo()
        {
            return au::str("%s@%s:%d" , user.c_str() , host.c_str() , port );
        }
        
        // Suspend the network elements implemented behind NetworkInterface
        // Close everything and return the "run" call
        virtual void quit(void)
        {
        }
        
        virtual std::vector<size_t> getDelilahIds()
        {
            LM_X(1, ("Delilah should never vall this method"));
            return std::vector<size_t>();
        }
        
        virtual std::string cluster_command( std::string command );
        
        // Auxiliar tools
        void message( std::string txt );
        
        
        // Add initial connections to "connect to a cluster" or "add a node to the connected cluster"
        // -----------------------------------------------------------------------------------------------
        Status addMainDelilahConnection( std::string host 
                                        , int port 
                                        , std::string user 
                                        , std::string password 
                                        );
        
    private:    
        
        std::string addSecondaryDelilahConnection( std::string host , int port );
        
        
        
    };
    
     
    class DelilahNetworkRemove : public engine::EngineElement
    {

        DelilahNetwork* delilah_network;
        
    public:
        
        DelilahNetworkRemove( DelilahNetwork* _delilah_network )
        {
            delilah_network = _delilah_network;
        }
        
        void run()
        {
            delilah_network->reset();
            delete delilah_network;
        }
        
    };
    
}

#endif
