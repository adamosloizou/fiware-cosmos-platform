
#include "samson/network/NetworkConnection.h"
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"
#include "NetworkManager.h" // Own interface

namespace samson {
    


    Status NetworkManager::move_connection( std::string connection_from , std::string connection_to )
    {
        if( connections.findInMap(connection_to) != NULL )
            return Error;
        
        NetworkConnection * network_connection = connections.extractFromMap( connection_from );
        
        if( !network_connection )
            return Error;
        
        network_connection->name = connection_to;
        connections.insertInMap(connection_to, network_connection);

        // recover pending packets if any...
        multi_packet_queue.pop_pending_packet( connection_to,  &network_connection->packet_queue );
        
        return OK;
    }
 
    Status NetworkManager::add( NetworkConnection * network_connection )
    {
        std::string name = network_connection->getName();

        if( connections.findInMap( name ) != NULL )
            return Error;
        
        // Add to the map of connections
        connections.insertInMap( name , network_connection );

        // recover pending packets if any...
        multi_packet_queue.pop_pending_packet( name,  &network_connection->packet_queue );
        
        // Init threads once included in the map
        network_connection->initReadWriteThreads();
        
        return OK;
    }
    


    
    bool NetworkManager::isConnected( std::string connection_name )
    {
        return (connections.findInMap(connection_name) != NULL);
    }
    
    
    au::tables::Table * NetworkManager::getConnectionsTable()
    {
        
        au::tables::Table* table = new au::tables::Table( au::StringVector( "Name" , "Host" , "In" , "Out" ) );
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            au::StringVector values;
            
            values.push_back( it_connections->first ); // Name of the connection

            NetworkConnection* connection = it_connections->second;
            au::SocketConnection* socket_connection = connection->socket_connection;
            values.push_back( socket_connection->getHostAndPort() );
            values.push_back( au::str( connection->get_rate_in() , "B/s" ) );
            values.push_back( au::str( connection->get_rate_out() , "B/s" ) );
            
            table->addRow( values );
            
        }
        
        table->setTitle("Connections");
        
        return table;
    }
    
    NetworkConnection* NetworkManager::extractNextDisconnectedConnection( )
    {
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            NetworkConnection * network_connection = it_connections->second;
            
            if( network_connection->isDisconnected() )
                if( network_connection->noThreadsRunning() )
                {
                    connections.erase( it_connections );
                    return network_connection;
                }
        }
        return NULL; // No next unconnected
    }
    
    std::vector<size_t> NetworkManager::getDelilahIds()
    {
        // Return all connections with pattern delilah_X
        std::vector<size_t> ids;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            NodeIdentifier _node_identifier = it_connections->second->getNodeIdentifier();
            
            if( _node_identifier.node_type  == DelilahNode )
            {
                size_t id = _node_identifier.id;
                
                if( it_connections->first == _node_identifier.getCodeName() )
                {
                    // Add this id to the list
                    ids.push_back(id);
                }
                else
                    LM_W(("Delilah %lu (%s) connected using wrong connection name %s",
                          _node_identifier.id,
                          _node_identifier.getCodeName().c_str(),
                          it_connections->first.c_str()
                          ));
            }
        }
        
        return ids;
    }
    
    std::string NetworkManager::str()
    {
        std::ostringstream output;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++)
            output << it_connections->first << " : " << it_connections->second->str() << "\n";
        
        return output.str();
    }
    
    Status NetworkManager::send( Packet* packet )
    {
        // Recover connection name
        std::string name = packet->to.getCodeName();
        
        NetworkConnection* connection = connections.findInMap( name );
        
        if( !connection )
        {
            // Only messages to workers are saved to be sended when reconnecting
            if(  !packet->disposable && (packet->to.node_type == WorkerNode) )
            {
                // Save this messages appart
                multi_packet_queue.push_pending_packet( name , packet );
                return OK;
            }
            else
            {
                // Delilah messages, just discard them
                LM_W(("Packet %s destroyed since connection %s is not available" , packet->str().c_str(), name.c_str() ));
                return Error;
            }
        }
        
        connection->push( packet );   
        return OK;
    }

    network::Collection* NetworkManager::getConnectionsCollection( Visualization* visualization )
    {
        network::Collection* collection = new network::Collection();
        collection->set_name("connections");
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        
        for ( it_connections =connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            network::CollectionRecord* record = collection->add_record();            
            it_connections->second->fill( record , visualization );
            
        }
        
        return collection;
    }
    
    void NetworkManager::reset()
    {
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            std::string name = it_connections->first;
            
            NetworkConnection * connection = it_connections->second;
            connection->close();
            connection->setNodeIdentifier( NodeIdentifier(UnknownNode,-1) );
            
            // Wait until the thread goes out
            au::Cronometer cronometer;
            while( !connection->noThreadsRunning() )
            {
                usleep(100000);
                if( cronometer.diffTime() > 1 )
                {
                    cronometer.reset();
                    LM_W(("Waiting thread of network connection %s to finish..." , connection->str().c_str() ));
                }
            }
            delete connection;
        }
        
        // Remove map ( not elements inside, already deleted here )
        connections.clear();
        
        
    }
    
    size_t NetworkManager::get_rate_in()
    {
        size_t total = 0;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_in();
        
        return total;
    }
    
    size_t NetworkManager::get_rate_out()
    {
        size_t total = 0;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_out();
        
        return total;
    }

    std::string NetworkManager::getStatusForConnection( std::string connection_name )
    {
        // Find this connection...
        NetworkConnection* connection = connections.findInMap( connection_name );
        
        if (!connection )
            return "Non connected";
        else if ( connection->isDisconnected() )
            return "Disconnecting";
        else
            return au::str( "Connected In: %s Out: %s " , au::str( connection->get_rate_in() , "B/s" ).c_str() , au::str( connection->get_rate_out() , "B/s" ).c_str() );
    }
    
    au::tables::Table * NetworkManager::getPendingPacketsTable()
    {
        return multi_packet_queue.getPendingPacketsTable();
    }

    void NetworkManager::check()
    {
        multi_packet_queue.check();
    }
    
    void NetworkManager::push_pending_packet( std::string name , PacketQueue * packet_queue )
    {
        multi_packet_queue.push_pending_packet(name, packet_queue );
    }

}