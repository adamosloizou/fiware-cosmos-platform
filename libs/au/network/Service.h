
#ifndef _H_AU_NETWORK_SERVICE
#define _H_AU_NETWORK_SERVICE

#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/set.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"

#include "au/console/Console.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"

namespace au
{
    namespace network
    {
        
        class Service;
        
        class ServiceItem
        {
            Service * service;
            SocketConnection * socket_connection;

            bool quit; // Falg to inform that it is necessary to quit
            
            friend void* run_service_item( void*p );
            friend class Service;
            
        public:
            
            ServiceItem( Service * _service , SocketConnection * _socket_connection )
            {
                // Keep a pointer to the service to nofity when finished
                service = _service;
                
                // Keep a pointer to the socket connection
                socket_connection = _socket_connection;
                
                // By default, do not quit
                quit = false;
            }
            
            virtual ~ServiceItem()
            {
                delete socket_connection;
            }
            
            void stop()
            {
                quit = true;
            }
            
            void runInBackground();
            
        };
        
        // Service opening a paricular port and accepting connections
        
        class Service : public NetworkListenerInterface
        {
            
            int port;                      // Port to offer this service
            bool init;                     // Flag to avoid multiple inits 
            NetworkListener listener;      // Listener to receive connections
            au::set< ServiceItem > items;  // Connected items 

            au::Token token;               // Mutex protection
            
            friend class ServiceItem;
            friend void* run_service_item( void*p );
            
        public:
            
            Service( int _port ) : listener( this ) , token("Service")
            {
                port = _port;
                init = false;
            }
            
            Status initService( )
            {
                Status s = listener.initNetworkListener( port );
                
                if( s == OK )
                    listener.runNetworkListenerInBackground();
                
                return s;
            }
            
            void stop()
            {
                au::TokenTaker tt(&token);
                au::set< ServiceItem >::iterator it_items;
                for (it_items = items.begin() ; it_items != items.end() ; it_items ++ )
                    (*it_items)->stop();
            }
            
            void newSocketConnection( NetworkListener* _listener , SocketConnection * socket_connetion )
            {
                au::TokenTaker tt(&token);
                
                if( _listener != &listener )
                {
                    LM_E(("Unexpected listner in au::Service"));
                    return ;
                }
                
                // Create the item
                ServiceItem *item = new ServiceItem( this , socket_connetion );
                
                // Insert the item
                items.insert( item );
                
                // Run in background
                item->runInBackground();
                
            }
            
            au::tables::Table* getConnectionsTable()
            {
                
                std::string format = "Host|time,f=time|In (B),f=uint64|Out (B),f=uint64|In (B/s),f=uint64|Out (B/s),f=uint64";
                au::tables::Table* table = new au::tables::Table( format );
                
                au::set< ServiceItem >::iterator it_items;
                for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
                {
                    ServiceItem * item = *it_items;
                    
                    au::StringVector values;
                    
                    values.push_back( item->socket_connection->getHostAndPort() );
                    values.push_back( au::str("%lu", item->socket_connection->getTime() ) );

                    values.push_back( au::str("%lu", item->socket_connection->rate_in.getTotalSize() ) );
                    values.push_back( au::str("%lu", item->socket_connection->rate_out.getTotalSize() ) );
                    values.push_back( au::str("%lu", item->socket_connection->rate_in.getRate() ) );
                    values.push_back( au::str("%lu", item->socket_connection->rate_out.getRate() ) );
                    
                    table->addRow( values );
                }
                
                
                return table;
            }
            
            int getPort()
            {
                return  port;
            }
            
            // Overload this method to define service action
            virtual void run( SocketConnection * soket_connection , bool *quit )=0;
            
        private:
            
            void finish( ServiceItem * item )
            {
                // Notify this has finished
                items.erase( item );
            }
            
        };
        
    }
}

#endif
