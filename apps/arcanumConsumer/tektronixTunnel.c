/* ****************************************************************************
*
* FILE                      tektronixTunnel.c - interconnect two machines 
*
* AUTHOR                    Ken Zangelin, Telefonica I+D
*
* CREATION DATE             January 9, 2012
*
* COPYRIGHT                 Nah, not really necessary ... :-)
*
*
*/
#include <stdio.h>              // printf
#include <string.h>             // strstr, etc ...
#include <strings.h>            // strstr, etc ...
#include <sys/types.h>          // types needed by socket include files
#include <errno.h>              // errno
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <inttypes.h>           // int32_t, ...
#include <signal.h>             // SIGPIPE, sigignore, ...

extern int sigignore(int sig);



/* ****************************************************************************
*
* MAX - 
*/
#ifndef MAX
#define MAX(a, b) (((a) > (b))? (a) : (b))
#endif



/* ****************************************************************************
*
* Debug macros
*/
#define M(s)                                                      \
do {                                                              \
    printf("%s[%d]: %s: ", __FILE__, __LINE__, __FUNCTION__);     \
    printf s;                                                     \
    printf("\n");                                                 \
} while (0)

#define V0(level, s)                                              \
do {                                                              \
    if (verbose >= level)                                         \
        M(s);                                                     \
} while (0)

#define V1(s) V0(1, s)
#define V2(s) V0(2, s)
#define V3(s) V0(3, s)
#define V4(s) V0(4, s)
#define V5(s) V0(5, s)
#define E(s)  M(s)

#define X(code, s)                                                \
do {                                                              \
    M(s);                                                         \
    exit(code);                                                   \
} while (0)

#define R(r, s)                                                   \
do {                                                              \
    M(s);                                                         \
    return r;                                                     \
} while (0)



/* ****************************************************************************
*
* Node - 
*/
typedef struct Node
{
    char*           name;
	char            host[128];
	unsigned short  port;
	int             listenFd;
	int             fd;         // only allowing ONE connection ...
} Node;



#define BUFSIZE (16 * 1024 * 1024)
/* ****************************************************************************
*
* global variables - 
*/ 
static int      verbose     = 0;
static int      filter      = 0;
unsigned short  snifferPort = 0;
Node            tektronix;
Node            samson;
Node            sniffer;
char            buffer[BUFSIZE];
char            savedBuffer[128 * 1024];
int             bufSize           = BUFSIZE;
int             savedMissing      = 0;
int             savedLen          = 0;
int             packetsSent       = 0;
int             packetsForSniffer = 0;



/* ****************************************************************************
*
* bufPresent - 
*/
void bufPresent(char* title, char* buf, int bufLen)
{
	int ix = 0;

	if (verbose < 2)
	   return;

	printf("----- %s -----\n", title);

	while (ix < bufLen)
	{
		if (ix % 16 == 0)
			printf("%08x:  ", ix);
		printf("%02x ", buf[ix] & 0xFF);
		++ix;
		if (ix % 16 == 0)
			printf("\n");
	}

	printf("\n");
	printf("\n");	
}



/* ****************************************************************************
*
* serverInit - 
*/
void serverInit(Node* nodeP)
{
	int                 reuse = 1;
	int                 fd;
	struct sockaddr_in  sa;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		X(13, ("socket: %s\n", strerror(errno)));

	fcntl(fd, F_SETFD, 1);

	memset((char*) &sa, 0, sizeof(sa));

	if (nodeP->host[0] == 0)
	{
        sa.sin_addr.s_addr = INADDR_ANY;
        V3(("Listen socket for '%s' bound to 'ANY' host, port %d", nodeP->name, nodeP->port));
    }
	else
	{
        struct hostent* heP;
		
        heP = gethostbyname(nodeP->host);
        bcopy(heP->h_addr, &sa.sin_addr, heP->h_length);
        V3(("Listen socket for '%s' bound to host '%s', port %d", nodeP->name, nodeP->host, nodeP->port));
	}

	sa.sin_family      = AF_INET;
	sa.sin_port        = htons(nodeP->port);
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
    V1(("Binding '%s' to port %d", nodeP->name, nodeP->port));
	if (bind(fd, (struct sockaddr*) &sa, sizeof(struct sockaddr_in)) == -1)
	{
		close(fd);
		X(12, ("bind to port %d: %s\n", nodeP->port, strerror(errno)));
	}

	if (listen(fd, 10) == -1)
	{
		close(fd);
		X(11, ("listen to port %d\n", nodeP->port));
	}

	nodeP->listenFd = fd;
	nodeP->fd       = -1;

	V3(("Opened listen socket on fd %d for connections from '%s', port %d", nodeP->listenFd, nodeP->host, nodeP->port));
}



/* ****************************************************************************
*
* acceptConnection - 
*/
void acceptConnection(Node* nodeP)
{
    V3(("Accepting connection on fd %d", nodeP->listenFd));
    nodeP->fd = accept(nodeP->listenFd, NULL, 0);
	if (nodeP->fd == -1)
        X(6, ("accept: %s", strerror(errno)));

    if (nodeP == &sniffer)
    {
        V1(("Resetting sniffer packet count"));
        packetsForSniffer = 0;
    }
    // int set = 1;
    // setsockopt(nodeP->fd, SOL_SOCKET, SO_NOSIGPIPE, (void*) &set, sizeof(int));
} 



/* ****************************************************************************
*
* nodeParse - 
*/
void nodeParse(char* nodeInfo, Node* nodeP)
{
	char* nodeInfoCopy = strdup(nodeInfo);
	char* portStart;

	V5(("parsing node '%s'", nodeInfo));

    nodeP->listenFd = -1;
    nodeP->fd       = -1;

	char* tmP = strstr(nodeInfo, ":");
	if (tmP == NULL)
    {
        nodeP->host[0] = 0;
        nodeP->port    = atoi(nodeInfo);
    }
    else
    {
        *tmP   = 0;
        ++tmP;
        strcpy(nodeP->host, nodeInfo);
        portStart = tmP;
        nodeP->port = atoi(portStart);
    }

	V4(("host: '%s', port: '%d'", nodeP->host, nodeP->port));
	free(nodeInfoCopy);
}



/* ****************************************************************************
*
* nodeInit - 
*/
void nodeInit(Node* nodeP, char* name)
{
    nodeP->name = strdup(name);

    V1(("Initializing '%s' server for host '%s' and port %d", nodeP->name, nodeP->host, nodeP->port));
    serverInit(nodeP);
}



/* ****************************************************************************
*
* tmoHandler - 
*/
void tmoHandler(void)
{
    V2(("Got a TIMEOUT ..."));
}



/* ****************************************************************************
*
* filteredTunnel - 
*/
void filteredTunnel(void)
{
	E(("Sorry, filter not implemented ..."));
	exit(51);
}



/* ****************************************************************************
*
* nodeClose - 
*/
void nodeClose(Node* nodeP)
{
    close(nodeP->fd);
    nodeP->fd = -1;
}



/* ****************************************************************************
*
* forward - 
*/
void forward(Node* nodeP, char* buf, int size, int retries)
{
    int nb;
    int total;
    int* bfr = (int*) buf;


    //
    // Check if we can write ...
    //
    int             fds;
    fd_set          wFds;
    struct timeval  timeVal;

    timeVal.tv_sec  = 0;
    timeVal.tv_usec = 0;
    
    FD_ZERO(&wFds);
    FD_SET(nodeP->fd, &wFds);

    do
    {
        fds = select(nodeP->fd + 1, NULL, &wFds, NULL, &timeVal);
    } while ((fds == -1) && (errno == EINTR));

    if (fds == -1)
       X(37, ("select error: %s", strerror(errno)));
    else if (fds == 0)
    {
        if (retries == 0)
            E(("Cannot write to node '%s' - loosing packet %d", nodeP->name, packetsSent));
        usleep(1000);
        forward(nodeP, buf, size, ++retries);
        // nodeClose(nodeP);
        return;
    }
    else if (!FD_ISSET(nodeP->fd, &wFds))
        X(33, ("Cannot write to '%s'. Should REALLY never get here ...", nodeP->name));

    V2(("Buffer size of packet: %d", ntohl(bfr[0])));
    total = 0;
    while (total < size)
    {
        nb = write(nodeP->fd, &buf[total], size - total);
        if (nb == -1)
        {
            E(("writing data to node '%s': %s", nodeP->name, strerror(errno)));
            nodeClose(nodeP);
            return;
        }
        else if (nb == 0)
        {
            E(("written ZERO bytes to node '%s'!", nodeP->name));
            nodeClose(nodeP);
            return;
        }

        if (retries != 0)
            M(("Tunneled packet %d of %d bytes to '%s'", packetsSent, nb, nodeP->name));
        else
            V2(("Tunneled %d bytes to '%s'", nb, nodeP->name));
        total += nb;
    }
}




/* ****************************************************************************
*
* bufPush - 
*/ 
void bufPush(char* buf, int size)
{
    int    packetLen;

    //
    // Any data leftover from last push?
    // Add the data from buf to make it a whole packet,
    // and then forward this 'incomplete' packet first
    //
    if (savedLen != 0)
    {
        int* dataLenP = (int*) savedBuffer;
        int  dataLen  = ntohl(*dataLenP);
		int  totalLen = dataLen + 4;

		if (size < savedMissing)
		{
			V1(("Got a packet not big enough to fill entire saved buffer - adding %d bytes to saved buffer", size));
			memcpy(&savedBuffer[savedLen], buf, size);
			savedMissing -= size;
			savedLen     += size;
			return;
		}

		V1(("adding %d bytes to saved packet", savedMissing));
        memcpy(&savedBuffer[savedLen], buf, savedMissing);
        int* iP = (int*) savedBuffer;
        V1(("Restoring a saved packet (adding %d bytes): 0x%x 0x%x 0x%x 0x%x", savedMissing, iP[0], iP[1], iP[2], iP[3]));
        if (samson.fd != -1)
            forward(&samson, savedBuffer, totalLen, 0);
        if (sniffer.fd != -1)
        {
            V1(("packet %d (%d for sniffer) has dataLen %d (0x%x)", packetsSent, packetsForSniffer, dataLen, dataLen));
			bufPresent("saved buffer", savedBuffer, totalLen);
            forward(&sniffer, savedBuffer, totalLen, 0);
            ++packetsForSniffer;
        }

        buf    = &buf[savedMissing];
        size  -= savedMissing;

        savedLen     = 0;
        savedMissing = 0;
        ++packetsSent;
    }

    while (size > 0)
    {
		int* packetLenP = (int*) buf;
        packetLen = ntohl(*packetLenP);

        if (packetLen > 3000)
           X(1, ("packet len: %d (0x%x) - afraid we're out of sync ...", packetLen, packetLen));

        V2(("parsed a packet of %d data length (bigendian: 0x%08x)", packetLen, *packetLenP));
        if (size >= packetLen + 4)
        {
            ++packetsSent;
			V1(("Forwarding packet %d of %d bytes (including header of 4 bytes)", packetsSent, packetLen + 4));
            if (samson.fd != -1)
                forward(&samson, buf, packetLen + 4, 0);
            if (sniffer.fd != -1)
            {
                forward(&sniffer, buf, packetLen + 4, 0);
                ++packetsForSniffer;
            }
        }
        else
        {
            memcpy(savedBuffer, buf, size);
            savedLen       = size;
            savedMissing   = packetLen + 4 - savedLen;
			V1(("Saved %d bytes (%d missing for a full packet) (0x%x 0x%x 0x%x 0x%x)", savedLen, savedMissing,
				savedBuffer[0] & 0xFF , savedBuffer[1] & 0xFF, savedBuffer[2] & 0xFF, savedBuffer[3] & 0xFF));

            if (sniffer.fd != -1)
            {
                // V1(("Saved packet %d of %d bytes to 'savedBuffer' (packetLen: %d, so %d bytes missing ...)",
				//	packetsForSniffer, size, packetLen, savedMissing));

                // int* xP = (int*) savedBuffer;
                // V1(("saved: 0x%x 0x%x 0x%x 0x%x", xP[0], xP[1], xP[2], xP[3]));
            }
            return; // read more ...
        }

        buf = &buf[packetLen + 4];
        size  -= packetLen + 4;
    }
}



int nbAccumulated = 0;
/* ****************************************************************************
*
* tunnel - 
*/
void tunnel(void)
{
	int    nb;

	if (filter == 1)
	{
		filteredTunnel();
		return;
	}

    // memset(buffer, 0, sizeof(buffer));
	
    //
    // Read as much as we possibly can ...
    //
    nb = read(tektronix.fd, buffer, sizeof(buffer));
    if (nb == -1)
    {
        E(("Error reading from the TEKTRONIX node: %s", strerror(errno)));
        nodeClose(&tektronix);
        return;
    }
    else if (nb == 0)
    {
        E(("The TEKTRONIX node closed the connection"));
        nodeClose(&tektronix);
        return;
    }

    V1(("Read %d bytes of data from tektronix", nb));
	bufPresent("buffer from tektronix", buffer, nb);

    bufPush(buffer, nb);
    nbAccumulated += nb;
}



/* ****************************************************************************
*
* run - 
*/
void run(void)
{
	int             fds;
	int             max;
	fd_set          rFds;
	struct timeval  timeVal;

	while (1)
	{
        timeVal.tv_sec  = 5;
        timeVal.tv_usec = 0;
		
        max = 0;
        FD_ZERO(&rFds);


        //
        // Tektronix
        //
        if (tektronix.fd != -1)
        {
            FD_SET(tektronix.fd, &rFds);
            max = MAX(max, tektronix.fd);
        }
        else if (tektronix.listenFd != -1)
        {
            FD_SET(tektronix.listenFd, &rFds);
            max = MAX(max, tektronix.listenFd);
        }


        //
        // Samson
        //
        if (samson.fd != -1)
        {
            FD_SET(samson.fd, &rFds);
            max = MAX(max, samson.fd);
        }
        else if (samson.listenFd != -1)
        {
            FD_SET(samson.listenFd, &rFds);
            max = MAX(max, samson.listenFd);
        }


        //
        // Sniffer
        //
        if (sniffer.fd != -1)
        {
            FD_SET(sniffer.fd, &rFds);
            max = MAX(max, sniffer.fd);
        }
        else if (sniffer.listenFd != -1)
        {
            FD_SET(sniffer.listenFd, &rFds);
            max = MAX(max, sniffer.listenFd);
        }


        do
		{
			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			X(31, ("select error: %s", strerror(errno)));
		else if (fds == 0)
			tmoHandler();
        else if ((sniffer.listenFd != -1) && FD_ISSET(sniffer.listenFd, &rFds))
            acceptConnection(&sniffer);
        else if ((tektronix.listenFd != -1) && FD_ISSET(tektronix.listenFd, &rFds))
            acceptConnection(&tektronix);
        else if ((samson.listenFd != -1) && FD_ISSET(samson.listenFd, &rFds))
            acceptConnection(&samson);
		else if ((tektronix.fd != -1) && FD_ISSET(tektronix.fd, &rFds))
			tunnel();
		else if ((samson.fd != -1) && FD_ISSET(samson.fd, &rFds))
            nodeClose(&samson);
        else if ((sniffer.fd != -1) && FD_ISSET(sniffer.fd, &rFds))
            nodeClose(&sniffer);
		else
			X(32, ("What happened? select says OK (%d), but nothing to read ... ?", fds));
	}
}



/* ****************************************************************************
*
* usage - 
*/
void usage(char* progName)
{
	printf("Usage:\n");
	printf("  %s -u\n", progName);
	printf("  %s [-tektronix (ip:port)] [-samson (ip:port)] [-sniffer (ip:port)] [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-filter]\n", progName);
	exit(1);
}



/* ****************************************************************************
*
* sigHandler - 
*/
void sigHandler(int sigNo)
{
    E(("Got signal %d", sigNo));
}




/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
    char* tektronixInfo = NULL;
    char* samsonInfo  = NULL;
    char* snifferInfo = NULL;

    if (argC == 1)
    {
       usage(argV[0]);
       exit(1);
    }

 	if (strcmp(argV[1], "-u") == 0)
		usage(argV[0]);

	if (argC < 3)
		usage(argV[0]);

    //
    // default sniffer host: localhost
    //
    strcpy(sniffer.host, "localhost");

	int ix = 1;
	while (ix < argC)
	{
        if (strcmp(argV[ix], "-tektronix") == 0)
        {
            tektronixInfo = argV[ix + 1];
            ++ix;
        }
        else if (strcmp(argV[ix], "-samson") == 0)
        {
            samsonInfo = argV[ix + 1];
            ++ix;
        }
        else if (strcmp(argV[ix], "-sniffer") == 0)
        {
            snifferInfo = argV[ix + 1];
            ++ix;
        }
        else if (strcmp(argV[ix], "-v") == 0)
			verbose = 1;
		else if (strcmp(argV[ix], "-vv") == 0)
			verbose = 2;
		else if (strcmp(argV[ix], "-vvv") == 0)
			verbose = 3;
		else if (strcmp(argV[ix], "-vvvv") == 0)
			verbose = 4;
		else if (strcmp(argV[ix], "-vvvvv") == 0)
			verbose = 5;
		else if (strcmp(argV[ix], "-filter") == 0)
		{
			X(51, ("Sorry, filter not implemented ..."));
			filter = 1;
		}
		else if (strcmp(argV[ix], "-u") == 0)
			usage(argV[0]);
		else
		{
 			E(("%s: unrecognized option '%s'\n\n", argV[0], argV[ix]));
			usage(argV[0]);
		}

		++ix;
	}


    if (tektronixInfo == NULL)
    {
        printf("Option '-tektronix' not set. Using default values ");
        tektronixInfo = "172.74.200.200:1234";
    }

    if (samsonInfo == NULL)
    {
        printf("Option '-samson' not set. Using default values ");
        samsonInfo = "1099";
    }

    if (snifferInfo == NULL)
    {
        printf("Option '-sniffer' not set. Using default values ");
        snifferInfo = "localhost:1199";
    }



    //
    // For Solaris ...
    //
    memset(&samson, 0,  sizeof(samson));
    memset(&tektronix, 0, sizeof(tektronix));
    memset(&sniffer, 0, sizeof(sniffer));
    samson.fd         = -1;
    samson.listenFd   = -1;
    tektronix.fd        = -1;
    tektronix.listenFd  = -1;
    sniffer.fd        = -1;
    sniffer.listenFd  = -1;


    nodeParse(tektronixInfo, &tektronix);
	nodeParse(samsonInfo,  &samson);
	nodeParse(snifferInfo, &sniffer);

	nodeInit(&samson,  "samson");
	nodeInit(&tektronix, "tektronix");
    nodeInit(&sniffer, "sniffer");

    signal(SIGPIPE, sigHandler);
    sigignore(SIGPIPE);

	run();

	return 0;
}