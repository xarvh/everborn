/*
 NET_FAST.C		v0.09
 Wannabe-fast connection module.

 -> TCP will be replaced with UDP
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "main.h"
#include "net.h"





/*=============================================================================
 * STRING SHIFT
 *
 * If an incomplete packet is retrived, last string inside the packet
 * is incomplete.
 * Instead of reading it, it must be shifted to the beginning of fast_bf, so
 * that next network polls can complete it.
 * Not really a net_fast module, but useful to handle _fast strings.
 */
char *netStringShift(char *str, connectionC *conn)
{
 char *c;

 // packet end, nothing more to read //
 if(str == conn->fast_p)
    {
	conn->fast_p = conn->fast_bf;
	return 0;
    }

 // if whole packet is correctly terminated, just return the string //
 if(conn->fast_p[-1] == '\0') return str;

 // the packet is not complete: we have a fast_bf or network overflow //

 // check if the string is correctly terminated //
 for(c = str; c < conn->fast_p; c++)
    if(*c == '\0') return str;	// string correctly terminated, return it //

 // string is NOT complete, shift it to the beginning of fast_bf //
 c = conn->fast_bf;
 while(str < conn->fast_p) *c++ = *str++;

 // hopefully next recv() will complete the string from here //
 conn->fast_p = c;

 // nothing more to read //
 return 0;
}





/*=============================================================================
 * CONNECT
 * For this SOCK_STREAM version, listin()ing is a duty of the client.
 */
int net_fastConnect(connectionC *conn)
{
 char fe[] = "net_fastConnect: ";
 int listen_fd, e;


 // create listening socket //
 listen_fd = netCreateAndBindSocket(SOCK_STREAM, NET_FAST_PORT);
 if(listen_fd == -1) return -1;

 // listen //
 e = listen(listen_fd, 2);
 if(e == -1)	{ netErrB(fe, "listen"); goto ERROR; }

 // wait for a connection in short time //
 if(netWaitSocket(listen_fd)) goto ERROR;

 // server replyed, catch it! //
 conn->socket_fast = net_safeAccept(listen_fd, 0);
 close(listen_fd);
 return conn->socket_fast;

 ERROR:
 close(listen_fd);
 return -1;
}





/*=============================================================================
 * ACCEPT
 * Again, for this SOCK_STREAM version the server will actually connect() to
 * the client.
 */
int net_fastAccept(connectionC *conn)
{
 char fe[] = "net_fastAccept: ";
 int fd;

 // this is just to lose some time to let the client set up listening... //
 //@@ UDP should make it smarter... //
 // wait 200 milliseconds //
 netSleep(200);

 fd = netCreateAndConnectSocket(SOCK_STREAM, conn->addr, NET_FAST_PORT);
 if(fd == -1) return -1;

 conn->socket_fast = fd;
 return conn->socket_fast;
}





/*=============================================================================
 * SEND
 */
int net_fastSend(connectionC *c, char* data)
{
 int size = strlen(data) + 1;
 int e;

 e = send(c->socket_fast, data, size, 0);
 if(e == -1)
    {
	netErr("send");
	return -1;
    }

 if(e < size)
    {
	pe("AAAgh! come puoi voler spedire %d bytes?\n", size);
	return -1;
    }

 return e;
}





/*=============================================================================
 * SEND ALL
 */
int net_fastSendAll(char* data)
{
 connectionC *c;

 for(c = net_connections; c < net_connections_end; c++)
    if(c->socket_fast != -1)
	net_fastSend(c, data);

 return 0;
}





/*=============================================================================
 * RECV
 */
int net_fastRecv(connectionC *c)
{
 int max;
 int s;



 // calculate max space available for packet storage //
 max = NET_FAST_BF - (c->fast_p - c->fast_bf);

 // recv data //

 //@@multithread: lock buffer (c->fast_bf)
 s = recv(c->socket_fast, c->fast_p, max, 0);
 if(s != -1) c->fast_p += s;
 //@@multithread: unlock buffer

 if(s == 0) netConnectionLost(c);
 if(s == -1) return netErr("recv");

 return s;
}





//EOF =========================================================================
