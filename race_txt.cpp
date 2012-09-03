/*
 RACE_TXT.CPP		v0.04
 Race text IO.

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


#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "data.h"
#include "txt.h"

#include "game.h"
#include "race.h"
#include "city.h"
#include "utype.h"




/*=============================================================================
 * SPECIALS
 */
static spcC raceSpcA[] = {
    { RACE_MINERS,	0,0, "RACE_MINERS",	0, 0 },
    { RACE_FARMERS,	0,0, "RACE_FARMERS",	0, 0 },
    { RACE_WORKERS,	0,0, "RACE_WORKERS",	0, 0 },
    { RACE_POPMANA,	0,0, "RACE_POPMANA",	0, 0 },
    { RACE_LAND,	0,0, "RACE_LAND",		0, 0 },
    { RACE_SHORE,	0,0, "RACE_SHORE",	0, 0 },
    { RACE_SEA,		0,0, "RACE_SEA",		0, 0 },
    { RACE_FOREST,	0,0, "RACE_FOREST",	0, 0 },

    { 0, 0, 0, 0, 0 }
};





/*=============================================================================
 * LOAD
 *
 * Also ensures necessary unit roles exist.
 */
static bool raceCheckLang(raceC* r, char* id, char* e)
{
 if(r->Language.Generate(id, 0))
    return false;

 sprintf(e, "required language item '%s' not specified or malformed", id);
 return true;
}



raceC* raceLoad(char* bf, char* e)
{
 raceC* race = new raceC;
 char* s = bf;

 // id
 s = txtId(race->Id, s, sizeof(race->Id));
 if(s == NULL)
 {
    sprintf(e, "bad race id `%s`", txtWordTerminate(bf));
    goto ERROR;
 }


 // add error token
 e += sprintf(e, "%s/", race->Id);


 // same line, growth
 race->Growth = strtol(s, &s, 10);
 if(race->Growth > +50) race->Growth = +50; // silently cut sensless values
 if(race->Growth < -50) race->Growth = -50;


 // same line, specials
 s = txtReadSpecials(s, e, raceSpcA);
 if(s == NULL) goto ERROR;

 for(spcC* spc = raceSpcA; spc->Id; spc++) if(spc->Flag & SPC_ACTIVE)
    race->Spc |= spc->Code;

 if(!race->Spc) race->Spc = RACE_LAND; // set default settling ability


 // name, info, CityFN, LandFN
 s = txtSkipLine(s); txtString(race->Name, s, sizeof(race->Name));
 s = txtSkipLine(s); txtString(race->Info, s, sizeof(race->Info));
 s = txtSkipLine(s); txtString(race->City.Fn, s, sizeof(race->City.Fn));
 s = txtSkipLine(s); txtString(race->Land.Fn, s, sizeof(race->Land.Fn));


 // language
 s = txtSearch("[LANG]", bf);
 if(s == NULL)
 {
    sprintf(e, "no [LANG] section");
    goto ERROR;
 }

 for(;*s >= 'A' && *s <= 'Z'; s = txtSkipLine(s))
    race->Language.AddStr(s);
 if(raceCheckLang(race, "NAMEF", e)) goto ERROR;
 if(raceCheckLang(race, "NAMEM", e)) goto ERROR;
 if(raceCheckLang(race, "CITY", e)) goto ERROR;



 // blocks
    {
	blockC **b;

	s = bf;
	b = &race->Flesh;
	while(s = txtSearch("[BLO]", s))
	{
	    *b = fleshLoadBlock(s, e);
	    if(*b == NULL) goto ERROR;
	    b = &(*b)->Next;
	}    
    }

 // blds
 race->Blds = bldLoadAll(bf, e);
 if(race->Blds == NULL) goto ERROR;


 // units
 if(unitLoadExRace(bf, e, race)) goto ERROR;

 // population
 {
    s = txtSearch("[POP]", bf);
    if(!s) { sprintf(e, "[POP] not found"); goto ERROR; }

    s = fleshRead(s, e, &race->Farmer, race->Flesh); if(!s) goto ERROR; s = txtSkipLine(s);
    s = fleshRead(s, e, &race->Worker, race->Flesh); if(!s) goto ERROR; s = txtSkipLine(s);
    s = fleshRead(s, e, &race->Rebel , race->Flesh); if(!s) goto ERROR; s = txtSkipLine(s);

    fleshApplyDefaultParts(&race->Farmer, race->Flesh);
    fleshApplyDefaultParts(&race->Worker, race->Flesh);
    fleshApplyDefaultParts(&race->Rebel, race->Flesh);
 }

 // success!!
 return race;


 ERROR:
 delete race;
 return NULL;
}





/*=============================================================================
 * WRITE
 */
char* raceC::Write(char *bf)
{
 char* w = bf;
 blockC* b;
 spcC* spc;


 // id and growth
 w += sprintf(w, "%s\t%d\t", Id, Growth);


 // same line, specials
 for(spc = raceSpcA; spc->Id; spc++)
    if(Spc & spc->Code)
	w += sprintf(w, " %s", spc->Id);
 *w++ = '\n';


 // name, info and gfx
 w = txtWrString(w, Name); *w++ = '\n';
 w = txtWrString(w, Info); *w++ = '\n';
 w = txtWrString(w, City.Fn); *w++ = '\n';
 w = txtWrString(w, Land.Fn); *w++ = '\n'; 


 // language
 w += sprintf(w, "[LANG]\n");
 for(dcstrC* s = Language.StrHook; s; s = s->Next)
    w += sprintf(w, "%s\n", s->Str);


 // blds
 w = bldWriteAll(w, Blds);
 *w++ = '\n';


 // blocks
 for(b = Flesh; b; b = b->Next)
 {
    w += sprintf(w, "[BLO]\n");
    w = fleshWriteBlock(w, b);
    *w++ = '\n';
 }

 // pop
 w += sprintf(w, "[POP]\n");
 w = fleshWrite(w, &Farmer);
 w = fleshWrite(w, &Worker);
 w = fleshWrite(w, &Rebel);

 // units
 w = unitWriteExRace(w, this);
 *w++ = '\n';

/*
 char nn[100];
 sprintf(nn, "/home/nw/%s", Id);
 FILE* fd = fopen(nn, "wt");
 fprintf(fd, "===========\n%s=======\n", bf);
 fclose(fd);
*/

 // terminate and return //
 *w = '\0';
 return w;
}





//EOF =========================================================================
