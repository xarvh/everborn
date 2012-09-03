/*
 SKER.C		v0.00
 Skeletal animations loadup.

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
#include "ske.h"

skeParmT skeParmA[PARMS_CNT] = {
    { "hl",	 0,	 45, "minimum (lowest value) walk hip angle" },
    { "hu",	10,	 80, "maximum (highest value) walk hip angle" },
    { "ee",	 0,	170, "walk elbow angle" },
    { "sz",	 0,	 90, "walk shoulders width (walk Rz and Lz)" },

    { "tr",	 0,	 45, "walk maximum torso roll" },
    { "tp",	-90,	 90, "walk torso pitch" },
};



/*=============================================================================
 *
 */
static char *skipspaces(char *c)
{
 while(*c == ' ' || *c == '\t') c++;
 return c;
}




static char *skeValueRead(skeValueT *v, char *c)
{
 double f = 1.0;
 int i;

 if(*c == '-') f = -1., c++;
 if(*c == ':') f /= 2., c++;
 else if(*c >= '0' && *c <= '9') f *= strtod(c, &c);

 v->constant = f;
 v->parm_id = ~0;	// ~0 will be recognised as invalid value //
 if(*c == '\0' || *c == ' ' || *c == '\t') return c;

 // parse parameter //
 for(i = 0; i < PARMS_CNT; i++)
    if(skeParmA[i].name[0] == c[0])
    	if(skeParmA[i].name[1] == c[1])
	    {
		v->parm_id = i;
		return c + 2;
	    }

 return NULL;
}





animationC::animationC(char* name, char* str)
{
 skeFrameT *t, *f, **h;
 double total = 0.0;

 char* c;
 int p;

 Hang();
 Name = name;
 FramesCnt = 0;
 FirstFrame = NULL;
 h = &FirstFrame;

 str = skipspaces(str);
 for(c = str; *c; c = skipspaces(c))
    {
	// allocate //
	*h = (skeFrameT*)calloc(1, sizeof(skeFrameT));
	if(*h == NULL) goto ERROR;
	f = *h;
	h = &f->next;
	FramesCnt++;


	// read duration //
	f->duration = strtod(c, &c);
	if(f->duration <= 0.0) goto ERROR;
	total += f->duration;
	c = skipspaces(c);

	// read values //
	for(p = 0; *c && *c != '\n'; p++)
	    {
		if(p >= JOINTS_CNT) goto ERROR;

		c = skeValueRead(f->joint + p, c);
		if(c == NULL) goto ERROR;

		c = skipspaces(c);
	    }

	// skip terminating character //
	if(*c == '\n') c++;
    }

 // normalize frames, so that the whole animation lasts 1.0 //
 for(f = FirstFrame; f; f = f->next)
    f->duration /= total;

 // close frames loop and return successfully //
 *h = FirstFrame;


 ERROR:
 ;
/*
 // free everything //
 for(f = ani->first_frame; f; f = t)
    {
	t = f->next;
	free(f);
    }*/
}






//EOF =================================================================
