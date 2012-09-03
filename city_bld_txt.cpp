/*
 CITY_BLD_TXT.CPP	v0.04
 Blds text io.

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
#include <stdio.h>
#include <string.h>

#include "city.h"
#include "txt.h"





/*=============================================================================
 * GLOBAL
 */
static bldC** bldBaseA = NULL;
static bldC bldVoid[1];
static bldC bldFakeAlchemist[1];





/*=============================================================================
 * STATS and SPECIALS
 */
static bldC bldCmp;
static stat_t bldStatA[] = {
    {  bldCmp.StatsStart,	0,	0, 0 },
    { &bldCmp.AniStart,		"An",	0, 0 },
    { &bldCmp.Unrest,		"Un",	0, 0 },
    { &bldCmp.ProdB,		"Prod",	0, 0 },
    { &bldCmp.GoldB,		"Gb",	0, 0 },
    { &bldCmp.PopB,		"Pop",	0, 0 },
    { &bldCmp.Gold,		"Go",	0, 0 },
    { &bldCmp.Mana,		"Ma",	0, 0 },
    { &bldCmp.Food,		"Fo",	0, 0 },
    { &bldCmp.Know,		"Kn",	0, 0 },
    { NULL, NULL, NULL, NULL }
};

static spcC bldSpcA[] = {
    { CSPC_ALCHEMIST,	0,0, "ALCHEMIST",	0, "Allows the creation of magical weapons." },
    { CSPC_FOOD,	0,0, "FOOD", 		0, "Farmers will produce 3 food instead of 2." },
    { CSPC_FORECAST,	0,0, "FORECAST", 	0, "Enlarges city sight, and forecasts city random catastrophes."	},
    { CSPC_LIBRARY,	0,0, "LIBRARY",	0, 0 },
    { CSPC_POPMANA,	0,0, "POPMANA", 	0, "Each 1000 citizens will produce 0.5 mana per turn." },
    { CSPC_REGULAR,	0,0, "REGULAR", 	0, "All new units will start as Regular." },
    { CSPC_VETERAN,	0,0, "VETERAN", 	0, "All new units will start as Veteran." },
    { CSPC_MINE,	0,0, "MINE",		0, "Increases by 50% all special mineral bonuses." },
    { CSPC_WALL,	0,0, "WALL",		0, 0 },
    { 0, 0, 0, 0 }
};





/*=============================================================================
 * DESTROY LIST
 */
int bldDestroyList(bldC **l)
{
 if(l == NULL) return 0;

 for(int k = 0; k < BLD_MAX; k++) if(l[BLD_MAX + k])
    free(l[BLD_MAX + k]);

 free(l);
 if(bldBaseA == l) bldBaseA = 0;
}





/*=============================================================================
 * READ REQ
 *
 * return requirement bitmask, -1 on error.
 * (-1 would mean ALL requirements are set).
 */
static int bldParse(bldC *b, char *s)
{
 if(b == NULL) return 1;
 if(b == bldVoid) return 1;
 if(b == bldFakeAlchemist) return 1;

 return txtParse(b->Id, s);
}


long bldReadReq(char *s, char *e, bldC **l)
{
 long req = 0;
 int k;

 while(*s)
    {
	if(!txtParse("SEA", s)) req |= REQ_SEA;
	else if(*s != '-')
	    {
		for(k = 0; k < BLD_MAX && bldParse(l[k], s); k++);
		if(k == BLD_MAX)
		    {
			sprintf(e, "undeclared requirement `%s`", txtWordTerminate(s));
			return -1;
		    }

		req |= 1 << k;
	    }

	s = txtSkipString(s);
    }

 return req;
}



/*=============================================================================
 * WRITE REQ
 *
 * Works even if l == NULL, but req MUST be 0.
 * This is consistent since no blds means no requirements.
 */
char *bldWriteReq(char *bf, long req, bldC **l)
{
 int k;
 char *w = bf;

 if(req & REQ_SEA) w += sprintf(w, " SEA");

 for(k = 0; k < BLD_MAX; k++) if(req & (1<<k))
    w += sprintf(w, " %s", l[k]->Id);

 if(w == bf) w += sprintf(w, " -");
 *w++ = '\n';

 // terminate and return //
 *w = '\0';
 return w;
}





/*=============================================================================
 * LOAD
 *
 * bldA is the bld list used to parse requirements.
 * bldA may be incomplete (some entries may be NULL).
 */
static int bldLoad(char* bf, char* e, bldC* bldA[])
{
 bldC* b;
 char* s;
 int k, pos;


 // find first free position
 for(pos = 0; pos < BLD_MAX && bldA[pos]; pos++);
 if(pos == BLD_MAX)
    {
	sprintf(e, "too many blds");
	return -1;
    }


 // allocate
 b = (bldC*)calloc(1, sizeof(bldC));
 if(b == NULL) return -1;


 // bld id
 s = txtId(b->Id, bf, sizeof(b->Id));
 if(s == NULL)
    {
	sprintf(e, "bad bld id `%s`", txtWordTerminate(bf));
	goto ERROR;
    }


 // add error token
 e += sprintf(e, "%s/", b->Id);


 // check duplicated id
 for(k = 0; k < BLD_MAX; k++) if(bldA[k] && !txtParse(b->Id, bldA[k]->Id))
    {
	sprintf(e, "duplicated bld id `%s`", b->Id);
	goto ERROR;
    }


 // cost
 s = txtSkipSpaces(s);
 b->Cost = strtol(s, &s, 10);
 if(b->Cost < 20) b->Cost = 20;


 // stats
 s = txtSkipLine(s);
 s = txtReadStats(s, e, bldStatA, b->StatsStart);
 if(s == NULL) goto ERROR;


 // special
 {
    spcC* spc;

    s = txtSkipLine(s);
    s = txtReadSpecials(s, e, bldSpcA);
    if(s == NULL) goto ERROR;

    for(spc = bldSpcA; spc->Id; spc++) if(spc->Flag & SPC_ACTIVE)
	b->Spc |= spc->Code;
 }


 // requirements
 s = txtSkipLine(s);
 b->Req = bldReadReq(s, e, bldA);
 if(b->Req == -1) goto ERROR;


 // read name and info strings
 s = txtSkipLine(s); s = txtString(b->Name, s, sizeof(b->Name));
 s = txtSkipLine(s); s = txtString(b->Info, s, sizeof(b->Info));


 // place bld inside list and return successfully
 bldA[pos] = b;

 bldA[BLD_MAX+pos] = b;
 return 0;


 ERROR:
 free(b);
 return -1;
}





/*=============================================================================
 * BLD WRITE
 * writes bld_tmp to a buffer
 */
static char *bldWrite(char *w, int id, bldC **l)
{
 bldC *b = l[id];


 // id and cost
 w += sprintf(w, " %s %d\n", b->Id, b->Cost);


 // stats
 w = txtWriteStats(w, bldStatA, b->StatsStart);


 // spc //
    {
	spcC *sp;
	char *e = w;

	for(sp = bldSpcA; sp->Id; sp++) if(b->Spc & sp->Code)
	    e += sprintf(e, " %s", sp->Id);

	if(e == w) e += sprintf(e, " -");
	*e++ = '\n';

	w = e;
    }


 // requirements
 w = bldWriteReq(w, b->Req, l);


 // name & info
 w = txtWrString(w, b->Name); *w++ = '\n';
 w = txtWrString(w, b->Info); *w++ = '\n';


 *w++ = '\n';

 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * PARSE ID
 */
static int bldParseId(char *s, bldC **base)
{
 char bf[20];
 int k;

 s = txtId(bf, s, sizeof(bf));
 if(s == NULL) return BLD_MAX;		// no more //

 for(k = 0; k < BLD_MAX; k++) if(base[k])
    if(!txtParse(bf, base[k]->Id)) return k;

 return BLD_MAX+1;			// error //
}





/*=============================================================================
 * LOAD LOW
 */
static bldC **bldLoadLow(char *bf, char *e, bldC **base)
{
 bldC **bldA;

 int k;
 char *s;


 bldA = (bldC**)calloc(BLD_MAX*2, sizeof(bldC *));
 if(bldA == NULL) return NULL;


 // standard blds
 if(base) if(s = txtSearch("[BLD_STD]", bf))
    {
	while((k = bldParseId(s, base)) < BLD_MAX)
	    {
		bldA[k] = base[k];
		s = txtSkipLine(s);
	    }

	if(k == BLD_MAX+1)
	{
	    sprintf(e, "unknown bld id `%s`", s);
	    goto ERROR;
	}

	s = txtSkipLine(s);
    }


 // non-standard blds
 s = bf;
 while(s = txtSearch("[BLD]", s))
    if(bldLoad(s, e, bldA)) goto ERROR;



 // complete empty entries with void bld
 for(k = 0; k < BLD_MAX; k++) if(bldA[k] == NULL)
    bldA[k] = bldVoid;



 // is there an Alchemists Guild?
 for(k = 0; k < BLD_MAX; k++)
    if(bldA[k]->Spc & CSPC_ALCHEMIST) break;
 // if not, add the fake one
 if(k == BLD_MAX) 
 {
    for(k = 0; k < BLD_MAX; k++) if(bldA[k] == bldVoid) break;
    if(k < BLD_MAX) bldA[k] = bldFakeAlchemist;
 }



 // is there a Library?
 for(k = 0; k < BLD_MAX; k++)
    if(bldA[k]->Spc & CSPC_LIBRARY) break;
 // if not, it's a error!
 if(k == BLD_MAX) 
 {
    sprintf(e, "no bld with LIBRARY special");
    goto ERROR;
 }
 // ensure library has no requirements
 bldA[k]->Req = 0;





 // done
 return bldA;


 ERROR:
 bldDestroyList(bldA);
 return NULL;
}





/*=============================================================================
 * LOAD ALL
 */
bldC** bldLoadAll(char *s, char *e)
{
 if(bldBaseA == NULL)
 {
    sprintf(e, "no base blds loaded");
    return NULL;
 }

 return bldLoadLow(s, e, bldBaseA);
}





/*=============================================================================
 * WRITE ALL
 */
char* bldWriteAll(char* w, bldC** l)
{
 int k;

 if(l != bldBaseA)
    {
	w += sprintf(w, "[BLD_STD]\n");

	for(k = 0; k < BLD_MAX; k++) if(l[k] != bldVoid)
	    if(l[k] == bldBaseA[k])
		w += sprintf(w, "%s\n", l[k]->Id);
    }

 *w++ = '\n';

 for(k = 0; k < BLD_MAX; k++) if(l[k] != bldVoid && l[k] != bldFakeAlchemist)
    if(l == bldBaseA || l[k] != bldBaseA[k])
	{
	    w += sprintf(w, "[BLD]\n");
	    w = bldWrite(w, k, l);
	    *w++ = '\n';
	}

 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * LOAD BASE
 * Loads base blds.
 * Also sets void bld and fake alchemists.
 */
bldC** bldLoadBase(char* bf, char* e)
{
 // clean
// bldDestroyList(bldBaseA);
 bldFakeAlchemist->Spc = 0;

 // load base
 bldBaseA = bldLoadLow(bf, e, NULL);
 if(bldBaseA == NULL) return 0;


 // find the Alchemist Guild
 int k;
 for(k = 0; k < BLD_MAX; k++)
    if(bldBaseA[k]->Spc & CSPC_ALCHEMIST) break;

 // not available?
 if(k == BLD_MAX)
 {
    sprintf(e, "no bld with ALCHEMIST special");
    goto ERROR;
 }

 // create fake Alchemists
 *bldFakeAlchemist = *bldBaseA[k];
 bldFakeAlchemist->Req = REQ_NON_BUILDABLE;


 // initialize void bld
 memset(bldVoid, 0, sizeof(bldC));
 bldVoid->Req = REQ_NON_BUILDABLE;


 // success
 return bldBaseA;


 ERROR:
 bldDestroyList(bldBaseA);
 bldBaseA = 0;
 return 0;
}





/*=============================================================================
 * WRITE BASE
 */
char* bldWriteBase(char* w)
{
 return bldWriteAll(w, bldBaseA);
}



//EOF =========================================================================
