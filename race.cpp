/*
 RACE.CPP		v0.10
 Races.

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

#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "city.h"
#include "main.h"
#include "race.h"
#include "utype.h"
#include "net.h"
#include "txt.h"





/*=============================================================================
 * CONSTRUCTOR / DISTRUCTOR
 */
raceC::raceC()	:
    City(4,4),
    Land(1,1)
{
 Next = 0;

 Growth = 0;
 Spc = 0;

 Flesh = 0;
 Units = 0;
 Blds = 0;

 // add to chain
 if(!game) return;
 raceC** h;
 for(h = &game->Creatures.Next; *h; h = &(*h)->Next);
 *h = this;
}



raceC::~raceC()
{
 delete Flesh;
 unitDestroyL(Units);
 bldDestroyList(Blds);

 // remove from chain
 for(raceC** h = &game->Creatures.Next; *h;)
    if(*h == this) *h = Next;
    else h = &(*h)->Next;
}





/*=============================================================================
 * HELPERS
 */
utypeC* raceC::GetUtype(int id)
{
 utypeC* u;
 for(u  = Units; id-- && u; u = u->Next);
 return u;
}

utypeC* raceC::GetSpearmen()
{
 utypeC* u;
 for(u = Units; u; u = u->Next)
    if(u->Role->code == ROLE_SPEARMEN) return u;
 return 0;
}

utypeC* raceC::GetSettlers()
{
 utypeC* u;
 for(u = Units; u; u = u->Next)
    if(u->Role->code == ROLE_SETTLERS) return u;
 return 0;
}

utypeC* raceC::GetPlayerMage()
{
 utypeC* u;
 for(u = Units; u; u = u->Next)
    if(u->Role->code == ROLE_PLAYERMAGE) return u;
 return 0;
}





raceC* raceGet(int id)
{
 raceC* r;
 for(r = game->Creatures.Next; r && id--; r = r->Next);
 return r;
}



raceC* raceGetRandom(long seed)
{
 int cnt = 0;
 while(raceGet(cnt)) cnt++;
 return raceGet(seed%cnt);
}





/*=============================================================================
 * RACE REQUEST
 */
static int raceCnt;

int racesCount()
{
 return raceCnt;
}

void raceRequestAll(int count)
{
 raceCnt = count;

 for(int k = 0; k < count; k++)
 {
    char bf[100];
    sprintf(bf, "%d", k);
    netClientPostRequest(net_connections, "race", bf); 
 }
}





/*=============================================================================
 * PACK IO
 */
static int racePackRead(connectionC* sv, netReqC* req)
{
 char fe[] = "racePackRead: ";
 char ebf[200];
 char *c;
 raceC *r;

 c = (char*)req->data;


 // check string termination
 if(c[req->size -1] != '\0')
 {
    pe("%sunterminated string from server.\n", fe);
    return -1;
 }


 // load
 txtStrip(c);
 r = raceLoad(c, ebf);
 if(r == NULL)
 {
    pe("bad race '%s' from server: %s.\n", req->item, ebf);
    return -1;
 }


 // done
 pf("from server: race %s\n", r->Name);
 dataLoadMissing();
 return 0;
}



static netPwtT* racePackWrite(netReqC* header)
{
 raceC* r = raceGet(atoi(header->item));
 if(r == NULL) return NULL;

 netPwtT* p = (netPwtT*)malloc(200*1000 + sizeof(netPwtT));
 if(p == NULL) return NULL;

 char* end = r->Write(p->data);
 *end++ = '\0';

 p->size = end - (char *)p->data;
 return p;
}


static int raceError(connectionC* sv, netReqC* req)
{
}

static netPackC netPackRace("race", raceError, racePackRead, racePackWrite);



//EOF =========================================================================
