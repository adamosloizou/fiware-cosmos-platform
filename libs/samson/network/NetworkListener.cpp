

#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <sys/socket.h>
#include <netdb.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/network/SocketConnection.h"
#include "samson/network/misc.h"
#include "samson/network/NetworkManager.h"

#include "NetworkListener.h" // Own interface

namespace samson {

    NetworkListener::NetworkListener( NetworkManager * _network_manager  )
    {
        network_manager = _network_manager;
        
        rFd = -1; // Init value
        port = -1;
        
        quit_flag = false;
    }
    
    void NetworkListener::quit()
    {
        quit_flag = true;
    }
    
    Status NetworkListener::initNetworkListener( int _port )
    {
        // Keep port information
        port = _port;
        
        int                 reuse = 1;
        struct sockaddr_in  sock;
        struct sockaddr_in  peer;
        
        if (rFd != -1)
        {
            LM_W(("This listener already seems to be prepared, so not init again"));
            return Error;
        }
        
        if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            LM_RP(SocketError, ("socket"));
        
        fcntl(rFd, F_SETFD, 1);
        
        memset((char*) &sock, 0, sizeof(sock));
        memset((char*) &peer, 0, sizeof(peer));
        
        sock.sin_family      = AF_INET;
        sock.sin_addr.s_addr = INADDR_ANY;
        sock.sin_port        = htons(port);
        
        setsockopt(rFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
        
        if (bind(rFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
        {
            ::close(rFd);
            rFd = -1;
            LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
        }
        
        if (listen(rFd, 10) == -1)
        {
            ::close(rFd);
            rFd = -1;
            LM_RP(ListenError, ("listen to port %d", port));
        }
        
        return OK;
    }
    
    void* NetworkListener_run(void*p)
    {
        NetworkListener * network_listener = (NetworkListener*) p;
        network_listener->runNetworkListener();
        
        return NULL;
    }
    
    void NetworkListener::runNetworkListenerInBackground()
    {
        // Create thread
        pthread_create(&t, NULL, NetworkListener_run, this);
    }
    
    void NetworkListener::runNetworkListener()
    {
        if( rFd == -1 )
            LM_X(1, ("NetworkListener not initialized"));
        
        
        fd_set           rFds;
        int              max;
        struct timeval   tv;
        
        int              fds;
        int              eps;
        
        while ( true )
        {
            if ( quit_flag )
            {
                close( rFd );
                return;
            }
            
            //periodic();
            
            do
            {
                // One fd to read connections
                FD_ZERO(&rFds);
                eps = 1;
                max = rFd;
                FD_SET(rFd, &rFds);
                
                // Timeout
                tv.tv_sec  = 2;
                tv.tv_usec = 0;
                
                // Main select to wait new connections
                fds = select(max + 1,  &rFds, NULL, NULL, &tv);
                
            } while ((fds == -1) && (errno == EINTR));
            
            if (fds == -1)
                LM_X(1, ("select: %s", strerror(errno)));
            else if (fds == 0)
            {
                //timeout();
            }
            else
            {
                if (!FD_ISSET(rFd, &rFds))
                    LM_X(1, ("Error in main loop to accept connections"));
                
                // Accept a new connection
                SocketConnection* socket_connection = acceptNewNetworkConnection();
                
                // Notify this new connection
                network_manager->newSocketConnection(this, socket_connection );
            }
            
        }
    }
    
    SocketConnection* NetworkListener::acceptNewNetworkConnection(void)
    {
        int                 fd;
        struct sockaddr_in  sin;
        char                hostName[64];
        unsigned int        len         = sizeof(sin);
        int                 hostNameLen = sizeof(hostName);
        
        memset((char*) &sin, 0, len);
        
        LM_T(LmtNetworkListener, ("Accepting incoming connection"));
        if ((fd = ::accept(rFd, (struct sockaddr*) &sin, &len)) == -1)
            LM_RP(NULL, ("accept"));
        
        ip2string(sin.sin_addr.s_addr, hostName, hostNameLen);
        
        // Create the connection
        SocketConnection* socket_connection = new SocketConnection( fd , hostName , -1 );
        return socket_connection;
    }

}