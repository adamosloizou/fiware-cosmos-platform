#ifndef _H_AU_LOG_CENTRAL
#define _H_AU_LOG_CENTRAL

#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paBuiltin.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/network/SocketConnection.h"
#include "au/string.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/list.h"
#include "au/containers/set.h"

#include "au/log/Log.h"
#include "au/log/LogPlugin.h"
#include "log_server_common.h"

namespace au
{
        
    class LogCentral
    {
        // Connection information
        std::string host;
        int port;
        std::string local_file; // Name of the local file ( if not possible to connect with server )
                
        SocketConnection * socket_connection;    // Socket connection with the logServer
        au::Cronometer time_since_last_try;      // Cronometer with the time since last connection
        int time_reconnect;                      // time for the next reconnection
        
        // Local file descriptor to write the log if not possible to connect
        FileDescriptor* local_file_descriptor; 
        
        // Mutex to protect socket connection
        au::Token token;
        
        // List of plugins
        au::Token token_plugins;
        au::set<LogPlugin> plugins;
        
        
        // Current thread loging
        au::Token token_current_thread;
        pthread_t current_thread;
        bool current_thread_activated;
        
    public:
        
        LogCentral( std::string _host , int _port , std::string _local_file );
        ~LogCentral();
        
        // Change the host and port
        void set_host_and_port( std::string log_host , int log_port = AU_LOG_SERVER_PORT );

        // Get fd ( toa void closing it when working )
        int getFd();
        
        // Write log
        void write( Log *log );
        void internal_write( Log *log );
        
        // Plugins
        void addPlugin( LogPlugin* p );
        void removePlugin( LogPlugin* p );
        
        // Get host
        std::string getHost()
        {
            return host;
        }
        
        int getPort()
        {
            return port;
        }
        
    private:
        
        void write_to_server_or_file( Log *log );
        void write_to_plugins( Log *log );
        
        void try_connect();     // Try connection with server
        void open_local_file();  // Open local file to save logs
        
    };
}

#endif
    
    