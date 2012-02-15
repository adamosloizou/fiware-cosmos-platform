



#ifndef _H_SAMSON_DELILAH_NETWORK
#define _H_SAMSON_DELILAH_NETWORK

#include "samson/network/NetworkInterface.h"
#include "samson/network/CommonNetwork.h"

namespace samson {

    
    
class DelilahNetwork : public CommonNetwork
{

    std::string host;
    int port;
    
public:
    
    DelilahNetwork( );
    
    // NetworkManager interface
    // ------------------------------------------------------------
    void processHello( NetworkConnection* connection, Packet* packet );
    
    // NetworkInterface
    // ------------------------------------------------------------
    bool ready()
    {
        return true;
    }
    
    // Suspend the network elements implemented behind NetworkInterface
    // Close everything and return the "run" call
    virtual void quit(void)
    {
    }
    
    // Basic information about the cluster ( list of workers and delilahs )
    virtual std::vector<size_t> getWorkerIds()
    {
        return cluster_information.getWorkerIds();
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
    // ------------------------------------------------------------
    Status addMainDelilahConnection( std::string host , int port );
private:    
    std::string addSecondaryDelilahConnection( std::string host , int port );

    
    
};

}

#endif