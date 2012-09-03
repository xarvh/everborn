/*
 NET_SAFE.C		v0.08
 Safer connection, used to establish connection and chunk transfer.
*/

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>

#include "net.h"





/*=============================================================================
 * CONNECT
 * Establishes a TCP/IP (SOCK_STREAM) connection with the server.
 */
int net_safeConnect(char *ipstr)
{
 return netCreateAndConnectSocket(SOCK_STREAM, ipstr, NET_SAFE_PORT);
}





/*=============================================================================
 * ACCEPT
 * Establishes a TCP/IP (SOCK_STREAM) connection with connect()ing client.
 */
int net_safeAccept(int listen_fd, char *addr_str)
{
 struct sockaddr_in addr;
 int fd, r;

 r = sizeof(addr);
 fd = accept(listen_fd, (struct sockaddr *)&addr, (socklen_t*)&r);
 if(fd == -1)
    {
	netErr("accept");
	return -1;
    }

 if(addr_str) strcpy(addr_str, inet_ntoa(addr.sin_addr));
 return fd;
}






/*=============================================================================
 * LISTEN
 */
int net_safeListen()
{
 int fd, e;

 fd = netCreateAndBindSocket(SOCK_STREAM, NET_SAFE_PORT);
 if(fd == -1) return -1;

 e = listen(fd, NET_LISTEN_BACKLOG);
 if(e == -1)
    {
	close(fd);
	netErr("listen");
	return -1;
    }

 return fd;
}






/*=============================================================================
 * SEND
 */
int net_safeSend(connectionC *c, unsigned char *data, int size)
{
 int s;

 s = send(c->socket_safe, data, size, 0);
 if(s == -1) netErr("send");

 return s;
}





/*=============================================================================
 * RECV
 */
int net_safeRecv(connectionC *c, unsigned char *bf, int bf_size)
{
 int e;

 e = recv(c->socket_safe, bf, bf_size, 0);
 if(e == -1) return netErr("recv");

 if(e == 0) return netConnectionLost(c);

 return e;
}





//EOF =========================================================================
