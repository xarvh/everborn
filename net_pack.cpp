/*
 NET_PACK.C		v0.20
 Pack system, running on safe connection.

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

 TODO:
    - Request discard on error.
    - launch error() function on receive error.
    - reorder func names: all must start with net_pack**
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "net.h"





/*=============================================================================
 * GLOBALS
 */
static netPackC* netPackHook = NULL;





/*=============================================================================
 * CONSTRUCTOR
 */
netPackC::netPackC(
        char*		name,
	int		(*error)(connectionC* server, netReqC* request),
	int		(*read)(connectionC* server, netReqC* request),
	netPwtT*	(*write)(netReqC* header)
)
{
 netPackC** h;

 for(h = &netPackHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Name = name;
 Error = error;
 Read = read;
 Write = write;
}





/*=============================================================================
 * REMOVE REQUEST
 */
int netClientRemoveRequest(connectionC *sv, netReqC *req)
{
 netReqC **h;

 // find request //
 h = (netReqC **)&sv->requests;
 while(*h != req) h = &(*h)->next;	// assuming req is valid!! //


//@@ __f("unlinkin [%s->%s]\n", req->type->name, req->item);
 // unlink it //
 *h = req->next;

 //free it //
 if(req->data) free(req->data);
 free(req);

 return 0;
}





/*=============================================================================
 * HELPERS
 */
static netPackC *chTypeExName(char *name)
{
 netPackC* t;
 for(t = netPackHook; t && strcmp(t->Name, name); t = t->Next);
 return t;
}





static char *chChr(char c, char *s, char *end)
{
 while(s < end && *s++ != c);
 if(s < end) return s;
 return NULL;
}

static int chReadHeader(netReqC *h)
{
 char *end, *typeS, *itemS, *offS;
 char *c;
 int e, l;


 // this will be used to prevent overflows //
 end = (char*)h->data + h->size;


 // search game_string //
 l = strlen(game_string);
 for(c = (char*)h->data; strncmp(c, game_string, l); c++)
    if(c + l + 2 > end) return -1;
 c += l;


 // find header positions //
 typeS	= chChr(' ', c, end);		if(typeS == NULL) return -1;
 itemS	= chChr(' ', typeS, end);	if(itemS == NULL) return -1;
 offS	= chChr(' ', itemS, end);	if(offS  == NULL) return -1;


 // set packet data //
 h->data = chChr('\0', offS, end + 1);	if(h->data == NULL) return -1;
 h->size -= (char *)h->data - typeS;

 // terminate some typeS and itemS //
 itemS[-1] = '\0';
 offS[-1] = '\0';


 // search type //
 h->type = chTypeExName(typeS);
 if(h->type == NULL) return -1;


 // prepare data //
 if(strlen(itemS) > NET_PACK_ITEM_BF-1) return -1;
 strcpy(h->item, itemS);


 // if offS starts with '0', it is a size //
 // otherwise, it is an offset //
 if(*offS == '0')
    {
	h->off = 0;
	h->size = strtol(offS, &end, 16);
    }
 else
    {
	h->off = strtol(offS, &end, 16);
	h->size = 0;
    }
 if(end == offS) return -1;


 // all done! //
 return 0;
}





int chclSendRequest(connectionC *sv, netReqC *r)
{
 char bf[100];

 sprintf(bf, ":%s %s %s %x", game_string, r->type->Name, r->item, r->off);
 net_safeSend(sv, (unsigned char*)bf, strlen(bf) + 1);
}





/*=============================================================================
 * CLIENT POST REQUEST
 */
int netClientPostRequest(connectionC *sv, char *type, char *name)
{
 netReqC **h, *r;

 // find first free pointer //
 h = (netReqC **)&sv->requests;
 while(*h) h = &(*h)->next;

 // allocate request //
 *h = (netReqC*)calloc(1, sizeof(netReqC));
 if(*h == NULL) return -1;

 // fill structure //
 r = *h;
 r->type = chTypeExName(type);	// assuming valid type name! //
 strcpy(r->item, name);		// name lenght NOT checked!

 // if this is the only request, send it now. //
 if(sv->requests == r)
    if(netConnected()) chclSendRequest(sv, r);

 return 0;
}





/*=============================================================================
 * CLIENT READ
 */
static int netClientRead(connectionC *sv, netReqC *header, char *end)
{
 char fe[] = "netClientRead: ";
 int read_size;
 netReqC *r;


 // search corresponding request //
 for(r = sv->requests; r && (r->type != header->type || strcmp(r->item, header->item)); r = r->next);
 if(r == NULL)
    {
	pe("%sunrequested pack %s:%s from %s.\n", fe, header->type->Name, header->item, sv->name);
	return -1;
    }

 // check offset //
 if(header->off > r->off)
    {
	pe("%swrong offset (%d > %d) received from %s.\n", fe, header->off, r->off, sv->name);
	return -1;
    }

 // allocate? //
 if(r->data == NULL)
    {
	if(header->size == 0)
	    {
		pe("%sno size specifyed in first chunk received from %s.\n", fe, sv->name);
		return -1;
	    }

	if(header->size > 2*1024*1024)
	    {
		pe("%sdata from %s too large.\n", fe, sv->name);
		return -1;
	    }

	r->size = header->size;
	r->data = malloc(r->size);
	if(r->data == NULL) return -1;
    }

 read_size = end - (char *)header->data;

 // check size //
 if(header->off + read_size > r->size)
    {
	pe("%spack from %s will overflow!\n", fe, sv->name);
	return -1;
    }


 // and finally, copy! //
 memcpy((char*)r->data + header->off, header->data, read_size);
 r->off = header->off + read_size;

 // pack completed? //
 if(r->off == r->size)
    {
	r->type->Read(sv, r);		// read it //
	netClientRemoveRequest(sv, r);	// remove it from request queue //

	// make next request! //
	if(sv->requests)
	    chclSendRequest(sv, sv->requests);
    }
 else chclSendRequest(sv, r);		// request next data chunk

 return 0;
}










/*=============================================================================
 * SERVER HANDLE
 */
static int netServerHandle(connectionC *cl, netReqC *header)
{
 char fe[] = "netServerHandle: ";

 netPwtT *pack;
 char bf[100];

 unsigned char *start;
 int max;


 // write whole data //
 pack = header->type->Write(header);	// will output an error //
 if(pack == NULL) return -1;

 // check offset //
 if(header->off >= pack->size)
    {
	pe("%sbad request offset from %s.\n", fe, cl->name);
	free(pack);
	return -1;
    }


  // if first chunk, write a 0 followed by size instead of offset //
 if(header->off)
    sprintf(bf, "%s %s %s %x", game_string, header->type->Name, header->item, header->off);
 else
    sprintf(bf, "%s %s %s 0%x", game_string, header->type->Name, header->item, pack->size);


 // find chunk start position and maximum size available for sending //
 start = (unsigned char*)pack->data + header->off - strlen(bf) - 1;
 max = strlen(bf) + 1 + pack->size - header->off;
 if(max > NET_PACK_RECV_BF) max = NET_PACK_RECV_BF;

 strcpy((char*)start, bf);	 	// write header //
 net_safeSend(cl, start, max);	// send chunk //

 free(pack);			// destroy chunk //

 // done //
 return 0;
}





/*=============================================================================
 * RECV
 */
int net_packRecv(connectionC *c)
{
 char fe[] = "net_packRecv: ";
 unsigned char bf[NET_PACK_RECV_BF + 100];
 netReqC header;

 int e, i;


 // read //
 e = net_safeRecv(c, bf, sizeof(bf));
 if(e == -1) return -1;


 // empty remaining buffer //
 for(i = 0; e == sizeof(bf); i++)
    {
	unsigned char _bf[sizeof(bf)];
	e = net_safeRecv(c, _bf, sizeof(_bf));
	if(e == -1) return -1;
    }
 if(i) pf("net_packRecv: buffer was full %d times\n", i);


 // read header //
 header.data = bf;
 header.size = e;
 if(chReadHeader(&header))
    {
	pe("%sinvalid request header.\n", fe);
	return -1;
    }

 // is this a request or just a data chunk? //
 if(*bf == ':')	e = netServerHandle(c, &header);
 else 		e = netClientRead(c, &header, (char*)bf + e);

 return e;
}





//EOF =========================================================================
