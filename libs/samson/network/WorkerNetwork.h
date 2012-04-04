


#ifndef _H_SAMSON_WORKER_NETWORK
#define _H_SAMSON_WORKER_NETWORK

#include "samson/common/status.h"
#include "samson/common/ports.h"


#include "samson/network/NetworkInterface.h"
#include "samson/network/CommonNetwork.h"
#include "au/network/NetworkListener.h"

namespace samson {
    
    
    class WorkerNetwork : public CommonNetwork , public au::NetworkListenerInterface
    {
        
        // Counter to asign new names
        size_t tmp_counter;
        
        // Worker listtener for new worker / delilah connections
        au::NetworkListener * worker_listener;

        // Web RESET interface
        au::NetworkListener * web_listener;
        
    public:
        
        // Constructor
        // ------------------------------------------------------------
        WorkerNetwork( int port , int web_port );        
        ~WorkerNetwork();
        
        // NetworkManager interface
        // ------------------------------------------------------------
        void processHello( NetworkConnection* connection, Packet* packet );
        
        // NetworkInterface
        // ------------------------------------------------------------
        std::string cluster_command( std::string command );
        
        // Stop background threads
        void stop();
        
        // NetworkManager interface
        void newSocketConnection( au::NetworkListener* listener , au::SocketConnection * socket_connetion );
    
        
    private:
        
        void update_cluster_information( ClusterInformation * new_cluster_information );
        void update_cluster_information( ClusterInformation * new_cluster_information , size_t assigned_id );
        
    };
    
}

#endif
