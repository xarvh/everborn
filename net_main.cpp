/*
 NET_MAIN.CPP		v0.12
 Network main functionality module.
*/

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>


#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "net.h"





/*=============================================================================
 * GLOBALS
 */
connectionC  net_connections[NET_MAX_CONNECTIONS];
connectionC *net_connections_end = net_connections + sizeof(net_connections)/sizeof(connectionC);

static int listen_fd;





/*=============================================================================
 * CONNECTED
 * Just tells if the connection with server (ie: net_connections[0]) is up.
 */
int netConnected()
{
 return (listen_fd == -1) && (net_connections->socket_fast != -1);
}





/*=============================================================================
 * SLEEP
 * Not really a network function, but very useful when making cross connections
 * between client/server/daemon.
 */
int netSleep(int msec)
{
 unsigned long t;
 struct timeval s;

 gettimeofday(&s, NULL);
 t = s.tv_sec * 1000*1000 + s.tv_usec + msec * 1000;

 do gettimeofday(&s, NULL);
 while(t > s.tv_sec * 1000*1000 + s.tv_usec);
}





/*=============================================================================
 * ERR and ERRB
 * Generic error output function.
 */
int netErr(char *fe)
{
 pe("%s: %s.\n", fe, strerror(errno));
 return -1;
}

int netErrB(char *fe, char *bfe)
{
 pe("%s (%s): %s.\n", fe, bfe, strerror(errno));
 return -1;
}





/*=============================================================================
 * CLEAR
 * Cleans a connectionC structure.
 */
int netClear(connectionC *c)
{
 memset(c, 0, sizeof(connectionC));
 c->fast_p = c->fast_bf;
 c->socket_safe = -1;
 c->socket_fast = -1;
}



/*=============================================================================
 * FREE
 * Frees a connection structure resources.
 */
int netFree(connectionC *c)
{
 if(c->name) free(c->name);
 if(c->socket_safe > 0) close(c->socket_safe);
 if(c->socket_fast > 0) close(c->socket_fast);
}





/*=============================================================================
 * CLOSE
 */
int netClose(connectionC *c)
{
 netFree(c);
 netClear(c);
}





/*=============================================================================
 * INIT
 * Cleans up everything, prepares for a new network session.
 */
int netInit()
{
 connectionC *c;

 for(c = net_connections; c < net_connections_end; c++) netClear(c);
 listen_fd = -1;
}





/*=============================================================================
 * RESET
 * Closes and cleans a network session.
 * Can be called at any time both to shut all connections and to prepare for a
 * new network session.
 */
void netReset()
{
 connectionC *c;

 for(c = net_connections; c < net_connections_end; c++) netFree(c);
 if(listen_fd != -1) close(listen_fd);

 netInit();
}






/*=============================================================================
 * CONNECTION LOST
 */
int netConnectionLost(connectionC *c)
{
 pe("connection with %s (%s) lost.\n", c->name, c->addr);
 netClose(c);
 return -1;
}





/*=============================================================================
 * WAIT SOCKET
 * Waits for readable data to come from a socket within 2".
 */
int netWaitSocket(int sk)
{
 struct timeval tm;
 fd_set fds[1];
 int e;

 // wait up to 2" for the listen_fd to show off //
 tm.tv_sec = 2;
 tm.tv_usec = 0;
 FD_ZERO(fds);
 FD_SET(sk, fds);
 e = select(sk + 1, fds, NULL, NULL, &tm);
 if(e == -1)
    {
	netErr("select");
	return -1;
    }

 // if something arrived is all ok... //
 if(FD_ISSET(sk, fds)) return 0;

 // if not, we timed out! //
 errno = ETIMEDOUT;
 netErr("netWaitSocket");
 return -1;
}





/*=============================================================================
 * RESOLVE
 * Attempts to resolve a IP address.
 */
char *netResolve(char *name)
{
 char fe[] = "netResolve: ";
 struct hostent *host;
 struct in_addr *addr;

 host = gethostbyname(name);
 if(host == NULL)
    {
	// hstrerror is obsolete... shall I remove it? //
	pe("%s%s.\n", fe, hstrerror(h_errno));
	return NULL;
    }

 addr = (struct in_addr *)host->h_addr_list[0];
 return inet_ntoa(*addr);
}





/*=============================================================================
 * CREATE AND BIND SOCKET
 */
int netCreateAndBindSocket(int socket_type, int port)
{
 char fe[] = "netCreateAndBindSocket: ";
 struct sockaddr_in addr;
 int socket_fd;
 int yes;			// needed by setsockopt() //
 int e;

 // create a socket //
 socket_fd = socket(AF_INET, socket_type, 0);
 if(socket_fd == -1)
    {
	netErrB(fe, "socket");
	return -1;
    }

 yes = 1;
 e = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
 // setsockopt failing should not be a critical error //
 if(e == -1) pe("%ssetsockopt failed: %s.\n", fe, strerror(errno));


 // bind socket //
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = htons(port);
 memset(&(addr.sin_zero), 0, 8);

 e = bind(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
 if(e == -1)
    {
	close(socket_fd);
	netErrB(fe, "bind");
	return -1;
    }

 return socket_fd;
}





/*=============================================================================
 * CREATE AND CONNECT SOCKET
 */
int netCreateAndConnectSocket(int socket_type, char *ipstr, int port)
{
 char fe[] = "netCreateAndConnectSocket: ";
 struct sockaddr_in addr;
 int socket_fd;
 int e;

 // prepare struct sockaddr, check ipstr //
 if(!inet_aton(ipstr, &addr.sin_addr))
 {
    pe("%sinvalid ip address: %s.\n", fe, ipstr);
    return -1;
 }
 addr.sin_family = AF_INET;
 addr.sin_port = htons(port);
 memset(&(addr.sin_zero), 0, 8);

 // create a socket
 socket_fd = socket(AF_INET, socket_type, 0);
 if(socket_fd == -1)
 {
    netErrB(fe, "socket");
    return -1;
 }

 // connect socket
 for(int t = 0; t < 20; t++)
 {
    e = connect(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if(e != -1) break;
    netSleep(100);
 }

 if(e == -1)
 {
    close(socket_fd);
    netErrB(fe, "connect");
    return -1;
 }

 return socket_fd;
}






/*=============================================================================
 * CONNECT
 * Creates a full connection with a server.
 */
int netConnect(char *name)
{
 char introbf[200];
 connectionC *c;
 char *ip;
 int fd, e;

 // prepare for a new connection
 netReset();
 ip = netResolve(name);
 if(ip == NULL) goto ERROR;

 // initial connection attempt
 pf("connecting to %s...\n", name);
 fd = net_safeConnect(ip);
 if(fd == -1) goto ERROR;



 // init the connection struct
 c = net_connections;
 c->socket_safe = fd;
 c->name = (char*)malloc(strlen(name) + 1);
 strcpy(c->name, name);
 strcpy(c->addr, ip);

 // send intro string
 net_introClientWrite(introbf);
 e = net_safeSend(c, (unsigned char*)introbf, strlen(introbf)+1);
 if(e == -1) goto ERROR;


 // wait for server intro //
 if(netWaitSocket(c->socket_safe)) goto ERROR;

 // read server intro //
 e = net_safeRecv(c, (unsigned char*)introbf, sizeof(introbf));
 if(e == -1) goto ERROR;

 c->fast_bf[50] = '\0';		// ensure termination //

 if(net_introServerRead(introbf))
    {
	pe("netConnect: bad intro from sv, aborting\n");
	goto ERROR;
    }

 // ok, establish fast connection //
 pf("establishing main connection...\n");

 e = net_fastConnect(c);	// net_fastConnect() will also initialize c->socket_fast //
 if(e == -1) goto ERROR;

 // OK! //
 pf("connected with server %s (%s).\n", c->name, c->addr);
 return 0;


 ERROR:
 netReset();
 return -1;
}





/*=============================================================================
 * ACCEPT NEW CONNECTION
 * Establishes an incoming connection.
 */
int netAcceptNewConnection(int lfd, connectionC *c)
{
 char introbf[200];
 char *n;
 int fd, e;

 // if c == NULL, connection must be discarded //
 if(!c) return close(net_safeAccept(lfd, 0));

 // accept incoming connection, and store address in c->addr //
 fd = net_safeAccept(lfd, c->addr);
 if(fd == -1) return -1;

 // safe connection established //
 c->socket_safe = fd;
 pf("incoming connection from %s.\n", c->addr);

 // wait for client introduction //
 if(netWaitSocket(c->socket_safe)) goto ERROR;

 // read client intro //
 e = net_safeRecv(c, (unsigned char*)introbf, sizeof(introbf));
 if(e == -1) goto ERROR;

 // check intro //
 n = net_introClientRead(introbf);
 if(n == NULL)
    {
	pe("netAccept...: bad intro from client, refused\n");
	goto ERROR;
    }
 c->name = n;



 // send introduction: GameString, PlayerID, game args //
 net_introServerWrite(introbf, c - net_connections);
 e = net_safeSend(c, (unsigned char*)introbf, strlen(introbf)+1);
 if(e == -1) goto ERROR;


 // ok, establish fast connection //
 e = net_fastAccept(c);		// net_fastAccept() will also set c->socket_fast //
 if(e == -1) goto ERROR;

 // OK! //
 pf("%s connected.\n", c->name);
 *c->fast_p++ = 0;	// add some empty data, just to make server consider the new one! //
 return 0;


 ERROR:
 netClose(c);
 return -1;
}





/*=============================================================================
 * LISTEN
 * Initializes server stuff.
 */
int netListen()
{
 netReset();
 listen_fd = net_safeListen();
 if(listen_fd == -1) goto ERROR;

 pf("waiting for incoming connections.\n");

 // anything else? //
 return 0;

 ERROR:
 netReset();
 return -1;
}





/*=============================================================================
 * LOOP
 * Main network loop, for both client and server.
 * Polls network for incoming connections and data.
 * Returns -1 when the network goes down.
 */
int netLoop()
{
 connectionC *c;
 struct timeval tm;
 fd_set fds[1];
 int max, e;

 // initialize file-descriptor set, to use with select() //
 FD_ZERO(fds);
 max = listen_fd;	// for client listen_fd is -1! //
 if(listen_fd != -1) FD_SET(listen_fd, fds);
 
 for(c = net_connections; c < net_connections_end; c++) if(c->socket_safe != -1)
    {
	FD_SET(c->socket_safe, fds);
	FD_SET(c->socket_fast, fds);
	if(c->socket_safe > max) max = c->socket_safe;
	if(c->socket_fast > max) max = c->socket_fast;    
    }

 // set no timeout //
 tm.tv_sec = 0;
 tm.tv_usec = 0;

 // select()! //
 e = select(max + 1, fds, NULL, NULL, &tm);
 if(e == -1)	// unlikely to happen, but who knows? //
    {
	netErr("select");	
	return -1;
    }

 // recv incoming data //
 // mantaining loop if() as i suppose that FD_ISSET may not be fast //
 for(c = net_connections; c < net_connections_end; c++) if(c->socket_safe != -1)
    {
//debug	if(listen_fd == -1) pf("server socket: %d\n", c->socket_fast), exit(0);


	if(FD_ISSET(c->socket_fast, fds)) net_fastRecv(c);
	if(c->socket_fast != -1)
	    if(FD_ISSET(c->socket_safe, fds)) net_packRecv(c);
    }

 // check listen socket //
 if(listen_fd != -1 && FD_ISSET(listen_fd, fds))
    {
	// search for an empty connection struct //
	for(c = net_connections; c < net_connections_end && c->name; c++);
	if(c >= net_connections_end) c = 0;	// no connection available, drop request //

	netAcceptNewConnection(listen_fd, c);
    }

 // all done //
 if(max == -1) netReset();
 return max;
}





/*=============================================================================
 * Initializer
 */
class netInitC { public:
    netInitC() { netInit(); }
} static netInitializer;





//EOF =========================================================================
