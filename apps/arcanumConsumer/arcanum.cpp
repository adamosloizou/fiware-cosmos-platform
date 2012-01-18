/* ****************************************************************************
*
* FILE                     arcanumConsumer.cpp
*
* DESCRIPTION              
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 9 2011
*
*
* ToDo
*
*/
#include <stdio.h>              // printf, ...
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <signal.h>             // SIGPIPE, ...
#include <errno.h>              // errno



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
* Option variables
*/
char*           host      = (char*) "172.17.200.200";
unsigned short  port      = 1234;
int             sleepTime = 100000;
int             verbose   = 0;



/* ****************************************************************************
*
* connectToServer - 
*/
int connectToServer(void)
{
	struct hostent*     hp;
	struct sockaddr_in  peer;
	int                 fd;

	if (host == NULL)
		R(-1, ("no hostname given"));
	if (port == 0)
		R(-1, ("Cannot connect to '%s' - port is ZERO", host));

	if ((hp = gethostbyname(host)) == NULL)
		R(-1, ("gethostbyname(%s): %s", host, strerror(errno)));

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		R(-1, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	V2(("Connecting to %s:%d", host, port));
	int retries = 7200;
	int tri     = 0;

	while (1)
	{
		if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			++tri;
			E(("connect intent %d failed: %s", tri, strerror(errno)));
			usleep(500000);
			if (tri > retries)
			{
				close(fd);
				R(-1, ("Cannot connect to %s, port %d (even after %d retries)", host, port, retries));
			}
		}
		else
			break;
	}

	return fd;
}



/* ****************************************************************************
*
* writeToServer - 
*/
void writeToServer(int fd)
{
    int buf[100];
    int loopNo        = 1;
    int bytesWritten  = 0;
    int grandTotal    = 0;
    buf[0]            = htonl(99 * 4);  

    while (1)
    {
        int  nb;
        int  ix;

        for (ix = 1; ix < 100; ix++)
            buf[ix] = ix;

        nb = write(fd, buf, sizeof(buf));
        grandTotal += nb;
        V2(("Written %d bytes to server (total: %d)", nb, grandTotal));
        if (nb != sizeof(buf))
        {
           if (nb == -1)
           {
              E(("write to tunnel: %s", strerror(errno)));
              E(("Assuming connection closed, reconnecting ..."));
              close(fd);
              fd = connectToServer();
              continue;
           }
           else if (nb == 0)
           {
              E(("written ZERO bytes to tunnel - assuming connection closed, reconnecting ..."));
              close(fd);
              fd = connectToServer();
              continue;
           }

           E(("Not written all bytes (only %d of %d) - assuming tunnel closed connection", nb, (int) sizeof(buf)));
           close(fd);
           fd = connectToServer();
        }

        usleep(sleepTime);

        ++loopNo;
        bytesWritten += nb;

        if ((loopNo % 50) == 0)
            V1(("%d packets written, %d bytes in total", loopNo, bytesWritten));
        }
}



/* ****************************************************************************
*
* ignore - 
*/
void ignore(int sig)
{
}



/* ****************************************************************************
*
* usage - 
*/
void usage(char* progName)
{
	printf("Usage:\n");
	printf("  %s -u\n", progName);
	printf("  %s [-host (host)] [-port (port)] [-sleep (microsecs)] [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)]\n", progName);
	exit(1);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
    signal(SIGPIPE, ignore);

    if (argC == 1)
    {
       usage(argV[0]);
       exit(1);
    }

    int  ix = 1;

	while (ix < argC)
	{
        if (strcmp(argV[ix], "-u") == 0)
            usage(argV[0]);
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
        else if (strcmp(argV[ix], "-host") == 0)
        {
            host = strdup(argV[ix + 1]);
            ++ix;
        }
        else if (strcmp(argV[ix], "-port") == 0)
        {
            port = atoi(argV[ix + 1]);
            ++ix;
        }
        else if (strcmp(argV[ix], "-sleep") == 0)
        {
            sleepTime = atoi(argV[ix + 1]);
            ++ix;
        }
		else
		{
 			E(("%s: unrecognized option '%s'\n\n", argV[0], argV[ix]));
			usage(argV[0]);
		}

		++ix;
    }

	int fd = connectToServer();

	if (fd == -1)
		X(1, ("error connecting to host '%s', port %d", host, port));

	writeToServer(fd);

	return 0;
}
