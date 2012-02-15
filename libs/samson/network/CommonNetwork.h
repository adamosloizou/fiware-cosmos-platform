


#ifndef _H_SAMSON_COMMON_NETWORK
#define _H_SAMSON_COMMON_NETWORK

#include "engine/Object.h"

#include "samson/common/status.h"
#include "samson/common/ports.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/NetworkManager.h"
#include "samson/network/NetworkListener.h"

namespace samson {
    
    
    class CommonNetwork : public NetworkManager , public NetworkInterface , public engine::Object
    {
        
        size_t tmp_canceled_counter;
        
    protected:
        
        // My identifier in the cluster
        NodeIdentifier node_identifier;
        
        // Cluster information for this node ( worker or delilah )
        ClusterInformation cluster_information;
        
    public:
        
        CommonNetwork()
        {
            
            // Init 
            tmp_canceled_counter = 0;
            
            // Listen notifications for network manager
            listen("network_manager_review");
            
            // Create a periodic notification to review connections
            engine::Engine::shared()->notify( new engine::Notification( "network_manager_review" ) , 1  );
        }
        
        // Engine notification interface
        // ----------------------------------------------------------------
        void notify( engine::Notification* notification );

        //NetworkManager
        // ----------------------------------------------------------------
        void receive( NetworkConnection* connection, Packet* packet );
        virtual void processHello(NetworkConnection* connection , Packet* packet)
        {
            LM_W(("processHello not implemented"));
        }


        
        // NetworkInterface common methods
        // ----------------------------------------------------------------
        Status send( Packet* packet );
        std::vector<size_t> getWorkerIds();
        std::vector<size_t> getDelilahIds();
        void getInfo( ::std::ostringstream& output , std::string command );
        
        // Add output worker connections
        // ----------------------------------------------------------------
        Status addWorkerConnection( size_t worker_id , std::string host , int port );
        
        // Monitorization
        std::string str();
        
    protected:
        
        // Hello message for this connection
        Packet* helloMessage( NetworkConnection * target ); 

        // Remove all current connections
        void resetNetworkManager();
      
        void report_worker_connected( size_t id );
        void report_worker_disconnected( size_t id );
        
        void report_delilah_connected( size_t id );
        void report_delilah_disconnected( size_t id );
        
    };
    

}

#endif