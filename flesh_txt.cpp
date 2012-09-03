/*
 FLESH_TXT		v0.15
 Flesh text IO.

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

#include "flesh.h"
#include "txt.h"




/*=============================================================================
 * LOCALS
 */
static blockC fleshFakeBlock[1];





/*=============================================================================
 * PARTS
 */
class fleshPartC { public:
    char*	Name;
    long	Bits;
};

fleshPartC fleshPartA[] = {
    {	"HEAD",		1<<FLESH_HEAD	},
    {	"TORSO",	1<<FLESH_TORSO	},
    {	"TAIL",		1<<FLESH_TAIL	},

    {	"RUARM",	1<<FLESH_RUARM	},
    {	"LUARM",	1<<FLESH_LUARM	},	
    {	"RLARM",	1<<FLESH_RLARM	},
    {	"LLARM",	1<<FLESH_LLARM	},
    {	"RHAND",	1<<FLESH_RHAND	},
    {	"LHAND",	1<<FLESH_LHAND	},
    {	"RWEP",		1<<FLESH_RWEP	},
    {	"LWEP",		1<<FLESH_LWEP	},
    {	"WEPS",		(1<<FLESH_LWEP) + (1<<FLESH_RWEP)	},
    {	"UARMS",	(1<<FLESH_RUARM) + (1<<FLESH_LUARM)	},
    {	"LARMS",	(1<<FLESH_RLARM) + (1<<FLESH_LLARM)	},
    {	"HANDS",	(1<<FLESH_RHAND) + (1<<FLESH_LHAND)	},

    {	"RULEG",	1<<FLESH_RULEG	},
    {	"LULEG",	1<<FLESH_LULEG	},
    {	"RLLEG",	1<<FLESH_RLLEG	},
    {	"LLLEG",	1<<FLESH_LLLEG	},
    {	"RFOOT",	1<<FLESH_RFOOT	},
    {	"LFOOT",	1<<FLESH_LFOOT	},
    {	"ULEGS",	(1<<FLESH_RULEG) + (1<<FLESH_LULEG)	},
    {	"LLEGS",	(1<<FLESH_RLLEG) + (1<<FLESH_LLLEG)	},
    {	"FEET",		(1<<FLESH_RFOOT) + (1<<FLESH_LFOOT)	},

    {	"RULIMB",	1<<FLESH_RULIMB	},
    {	"LULIMB",	1<<FLESH_LULIMB	},
    {	"RLLIMB",	1<<FLESH_RLLIMB	},
    {	"LLLIMB",	1<<FLESH_LLLIMB	},
    {	"ULIMBS",	(1<<FLESH_RULIMB) + (1<<FLESH_LULIMB)	},
    {	"LLIMBS",	(1<<FLESH_RLLIMB) + (1<<FLESH_LLLIMB)	},
    {	NULL, 0	}
};





/*=============================================================================
 * SEARCH DEFAULT PART
 */
static blockC *fleshSearchDefaultPart(int bits, blockC *b)
{
 while(b)
    if(b->PartDefault & bits) return b;
    else b = b->Next;

 //@@ default block?
// pe("no default found\n");
 return NULL;
}





/*=============================================================================
 * APPY DEFAULT PARTS
 */
void fleshApplyDefaultParts(fleshC *f, blockC *l)
{
 int k;

 for(k = 0; k < FLESH_TOTAL; k++) if(f->BlocksStart[k] == NULL)
    f->BlocksStart[k] = fleshSearchDefaultPart(1<<k, l);
}





/*=============================================================================
 * VERTEX
 */
vertexC::vertexC()
{
 X = Y = Z = 0;
 TX = TY = 0;
 R = G = B = 0;
}



bool vertexC::IsUsed()
{
 return R || G || B;
}

static int fleshCharIsVertex(char c)
{
 return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

unsigned fleshCharToVertex(char c)
{
 if(c >= 'A' && c <= 'Z') return c - 'A';
 return c - 'a' + ('Z'-'A') + 1;
}

static char fleshVertexToChar(int v)
{
 if(v <= 'Z'-'A') return v + 'A';
 return v - ('Z'-'A') + 'a'-1;
}





/*=============================================================================
 * LOAD BLOCK
 */
blockC* fleshLoadBlock(char *s, char *err)
{
 vertexC *v = NULL;
 char *w, bf[16];
 int def;

 blockC* b = new blockC;

 // read name
 s = txtString(b->Name, s, sizeof(b->Name));


 // read part
 s = txtName(bf, s, sizeof(bf));
 if(*bf == '@')	def = 1;
 else		def = 0;

 fleshPartC* l;
 for(l = fleshPartA; l->Name && txtParse(l->Name, bf + def); l++);
 if(l->Name == NULL)
 {
    sprintf(err, "block `%s`: unknown part `%s`", b->Name, bf + def);
    goto ERROR;
 }
 b->Part = l->Bits;
 if(def) b->PartDefault = l->Bits;

 s = txtSkipLine(s);


 // read lenght and width
 {
    int l, w;

    s = txtSkipSpaces(s);
    if(2 > sscanf(s, "%d %d", &l, &w))
    {
	sprintf(err, "block `%s`: bad lenght/width", b->Name);
	goto ERROR;
    }

    b->Length = l;
    b->Width = w;
 }

 s = txtSkipLine(s);


 // read vertexes
 while(s = txtSkipSpaces(s), fleshCharIsVertex(*s))
 {
    signed X, Y, Z, tX, tY, R, G, B;
    int r;

    v = b->Vertex + fleshCharToVertex(*s);

    // read vertex coord, text coord, color
    r = sscanf(s+1, ": %d,%d,%d %d,%d %d %d %d",
	    &X, &Y, &Z,	    &tX, &tY,	    &R, &G, &B);

    if(r < 8)
    {
        sprintf(err, "block `%s`: bad vertex definition: '%s'", b->Name, s);
        goto ERROR;
    }

    v->X = X;	v->Y = Y;	v->Z = Z;
    v->TX = tX;	v->TY = tY;
    v->R = R;	v->G = G;	v->B = B;

    // ensure vertex is marked as used
    if(!v->IsUsed())
        v->R = v->G = v->B = 1;	// pattern vertex, player color applyed.

    s = txtSkipLine(s);
 }

 if(v == NULL)
 {
    sprintf(err, "block `%s`: no vertexes.", b->Name);
    goto ERROR;
 }


 // read strips
 w = b->Strips;
 while(s = txtSkipSpaces(s), !txtParse(">", s))
    {
	// @@does not check for
	//	-strips with undeclared vertexes
	//	-strips with less than 3 vertexes
	s = txtSkipString(s);
	while(fleshCharIsVertex(*s))
	    if(w - b->Strips < sizeof(b->Strips) -2) *w++ = *s++;
	    else
		{
		    sprintf(err, "block `%s`: strips + vertexes > %d.", b->Name, sizeof(b->Strips));
		    goto ERROR;
		}
	*w++ = '\0';
    	s = txtSkipLine(s);
    }

 *w++ = '\0';	// double-null-terminate strips list //

 if(w - b->Strips < 4)
 {
    sprintf(err, "block `%s`: no valid strips declared!", b->Name);
    goto ERROR;
 }


 // done //
 return b;

 ERROR:
 free(b);
 return NULL;
}





/*=============================================================================
 * WRITE BLOCK
 */
char* fleshWriteBlock(char* w, blockC* b)
{
 // name
 w += sprintf(w, " \"%s\" ", b->Name);


 // same line, default
 fleshPartC* p;
 for(p = fleshPartA; p->Bits && p->Bits != b->Part; p++);	// assuming b->Part is one of p->Bits! //
 if(b->PartDefault) *w++ = '@';
 w += sprintf(w, "%s\n", p->Name);


 // lenght, width
 w += sprintf(w, " %03d %03d\n", b->Length, b->Width);


 // vertexes
 for(vertexC* v = b->Vertex; v - b->Vertex < FLESH_VERTEXxBLOCK; v++)
    if(v->IsUsed())
    {
	w += sprintf(w, " %c:", fleshVertexToChar(v - b->Vertex));
	w += sprintf(w, " %+04d,%+04d,%+04d", v->X, v->Y, v->Z);
	w += sprintf(w, "\t%03d,%03d", v->TX, v->TY);
	w += sprintf(w, "\t%03d %03d %03d\n", v->R, v->G, v->B);
    }


 // strips
 for(char* c = b->Strips; *c; c++)
 {
    w += sprintf(w, " > ");
    while(*c) *w++ = *c++;
    *w++ = '\n';
 }
 *w++ = '\n';


 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * READ
 */
char* fleshRead(char* s, char* e, fleshC* flesh, blockC* blockL)
{
 //@@read width
 flesh->width = 30;

 for(;*s; s = txtSkipString(s))
    if(*s == '!')
    {
	fleshPartC* l;
	for(l = fleshPartA; l->Name && txtParse(l->Name, s+1); l++);
	if(l->Name == NULL)
	{
	    sprintf(e, "unknown flesh part `%s`", s+1);
	    return NULL;
	}

	for(int k = 0; k < FLESH_TOTAL; k++) if((1<<k) & l->Bits)
	    flesh->BlocksStart[k] = fleshFakeBlock;
    }
    else
    {
	blockC* b;
	for(b = blockL; b && txtParse(b->Name, s); b = b->Next);
	if(b == NULL)
	{
	    sprintf(e, "undeclared block `%s`", txtWordTerminate(s));
	    return NULL;
	}

	for(int k = 0; k < FLESH_TOTAL; k++) if((1<<k) & b->Part)
	    flesh->BlocksStart[k] = b;
    }

 return s;
}





/*=============================================================================
 * WRITE
 */
char* fleshWrite(char* w, fleshC* flesh)
{
 //@@ write width

 for(blockC** b = flesh->BlocksStart; b < flesh->BlocksEnd; b++)
    if(*b != fleshFakeBlock)
    {
	if(*b) if(!(*b)->PartDefault)
	    w += sprintf(w, " %s", (*b)->Name);
    }
    else
    {
	int k = b - flesh->BlocksStart;
	fleshPartC* l;
	for(l = fleshPartA; (l->Bits & (1<<k)) != l->Bits; l++);
	w += sprintf(w, " !%s", l->Name);
    }

 // newline
 *w++ = '\n';

 // terminate and return
 *w = '\0';
 return w;
}





//EOF =========================================================================
