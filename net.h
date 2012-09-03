/*
 NET.H			v0.21
 Network module.

 Copyright (c) 2004 Francesco Orsenigo <francesco.orsenigo@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/
#ifndef H_FILE_NET
#define H_FILE_NET
//=============================================================================

#define NET_MAX_MSG		200
#define NET_MAX_CONNECTIONS	32
#define NET_LISTEN_BACKLOG	4
#define NET_SAFE_PORT		10490
#define NET_FAST_PORT		16681

#define NET_FAST_BF		NET_MAX_CONNECTIONS*NET_MAX_MSG
#define NET_PACK_ITEM_BF	10
#define NET_PACK_RECV_BF	32*1024





/*=============================================================================
 * PACK
 */
typedef struct netPwtS {
    unsigned	size;
    char	_fill_[NET_PACK_ITEM_BF*4];
    char	data[0];
} netPwtT;


class connectionC;
class netReqC;
class netPackC { public:
    netPackC*	Next;

    char*	Name;
    int		(*Error)(connectionC* server, netReqC* request);
    int		(*Read)(connectionC* server, netReqC* request);
    netPwtT*	(*Write)(netReqC* header);

    //
    netPackC(
        char*		name,
	int		(*error)(connectionC* server, netReqC* request),
	int		(*read)(connectionC* server, netReqC* request),
	netPwtT*	(*write)(netReqC* header)
    );
};



class netReqC { public:
    netReqC*	next;

    netPackC*	type;
    char	item[NET_PACK_ITEM_BF];

    void*	data;
    unsigned	size;
    unsigned	off;
};





/*=============================================================================
 * CONNECTION
 */
class connectionC { public:
    char*	name;		// allocate here host name
    char	addr[16];	// allocate here host IP address

    int		socket_safe;
    int		socket_fast;

    char	fast_bf[NET_FAST_BF], *fast_p;

    //int	timeout?-lastcomm?;
    netReqC*	requests;	// pending requests dyn list
};





/*=============================================================================
 * GLOBALS
 */
extern connectionC  net_connections[];
extern connectionC* net_connections_end;
extern connectionC* net_self;





/*=============================================================================
 * NET_PACK.CPP
 */
int netClientRemoveRequest(connectionC* sv, netReqC* req);
int netClientPostRequest(connectionC* sv, char* type, char* name);
int net_packRecv(connectionC* sv);
int chclSendRequest(connectionC *sv, netReqC *r);





/*=============================================================================
 * NET_MAIN.CPP
 */
int netErr(char *fe);
int netErrB(char *fe, char *bfe);

int netConnected();
int netSleep(int msec);
int netClear(connectionC *c);
int netFree(connectionC *c);
int netClose(connectionC *c);

int netInit();
void netReset();
int netConnectionLost(connectionC *c);
int netWaitSocket(int sk);
int netCreateAndBindSocket(int socket_type, int port);
int netCreateAndConnectSocket(int socket_type, char *ipstr, int port);
int netAcceptNewConnection(int lfd, connectionC *c);
char *netResolve(char *name);

int netConnect(char *name);
int netListen();
int netLoop();





/*=============================================================================
 * net_fast.c
 */
int net_fastConnect(connectionC *conn);
int net_fastAccept(connectionC *conn);
int net_fastSend(connectionC *c, char *data);
int net_fastSendAll(char *data);
int net_fastRecv(connectionC *c);





/*=============================================================================
 * net_safe.c
 */
char *netStringShift(char *str, connectionC *conn);
int net_safeConnect(char *ipstr);
int net_safeAccept(int listen_fd, char *addr_str);
int net_safeListen();
int net_safeSend(connectionC *c, unsigned char *data, int size);
int net_safeRecv(connectionC *c, unsigned char *bf, int bf_size);





/*=============================================================================
 * net_intro.c
 */
int netGetSelfId();
int net_introServerWrite(char *s, int playerID);
int net_introServerRead(char *s);
int net_introClientWrite(char *s);
char *net_introClientRead(char *s);






#endif
//EOF =========================================================================
