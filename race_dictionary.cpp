/*
 RACE_DICTIONARY.CPP		v0.10
 Races languages.

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

#include "main.h"
#include "race.h"
#include "txt.h"





/*=============================================================================
 *
 */
dcstrC::dcstrC(char* str)
{
 Str = strdup(str);
 Next = 0;
}    

dcstrC::~dcstrC()
{
 free((void*)Str);
 if(Next) delete Next;
}



char* dcstrC::GetRandom(double rnd)
{
 int cnt = 1;
 for(char* c = Str; *c; c++) if(*c == ',') cnt++;

 int id = (int)(rnd * cnt);
 char* st = strchr(Str, ':') + 1;

 for(char* c = Str; *c; c++) if(*c == ',')
    if(id-- == 0) return st;
    else st = c + 1;

 return st;
}





dictionaryC::dictionaryC()
{
 StrHook = 0;
}

dictionaryC::~dictionaryC()
{
 if(StrHook) delete StrHook;
}



bool dictionaryC::AddStr(char* str)
{
 // requirement 1: uppercase name with trailing ':'
 char* c;
 for(c = str; *c >= 'A' && *c <= 'Z'; c++);
 if(c == str || *c != ':') return true;


 // create new object
 dcstrC* dcs = new dcstrC(str);

 // chain
 dcs->Next = StrHook;
 StrHook = dcs;

 // done, no errors
 return false;
}





/*=============================================================================
 *
 */
static char dicArray[128];

char* dictionaryC::Generate(char* type, double (*rnd)())
{
 char* str = dicArray;


 // build up
 Build(type, str, str+sizeof(dicArray), rnd);


 // fix case
 char* w = str;
 bool prevspace = true;

 for(char* r = str; *r; r++)
    if(*r == '_') prevspace = false;
    else
    {
	if(prevspace && *r >= 'a' && *r <= 'z')
	    *r = *r-'a' + 'A';

	prevspace = (*r == ' ');
	*w++ = *r;
    }

 *w = '\0';

 return dicArray;
}





/*=============================================================================
 * BUILD
 */
static bool dcmp(char* t, char* s)
{
 while(*t >= 'A' && *t <= 'Z')
    if(*t++ != *s++) return true;

 return *s >= 'A' && *s <= 'Z';
}



char* dictionaryC::Build(char* type, char* outbf, char* bfend, double (*rnd)())
{
 // search type
 dcstrC* dcs;
 for(dcs = StrHook; dcs && dcmp(type, dcs->Str); dcs = dcs->Next);
 if(!dcs) return 0;

 // get random expression
 char* s = dcs->GetRandom(rnd?rnd():0);
 if(*s == '\0') return 0;

 char* e = strchr(s, ',');
 if(!e) e = strchr(s, '\0');


 // interprete expression
 char* c = s;
 while(c < e)
 {
    // uppercase listname?
    char* t;
    for(t = c; *t >= 'A' && *t <= 'Z'; t++);
    if(t > c)
    {
	outbf = Build(c, outbf, bfend, rnd);

	if(!outbf) return 0;

	c = t;
	continue;
    }

    if(*c == '+') c++;
    else if(outbf < bfend-1) *outbf++ = *c++;
    else break;
 }

 *outbf = '\0';
 return outbf;
}





//EOF =========================================================================
