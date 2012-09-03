/*
 COUNTERS.CPP		v0.01
 Counters stuff

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
#include <GL/gl.h>

#include "main.h"
#include "data.h"
#include "fnt.h"

#include "counters.h"



#define COUNTERS_LIST_BF	1000

dataC counters("counters.png", 8, 8);



/*=============================================================================
 * GLOBALS
 */
static char countersList[COUNTERS_LIST_BF];
static char *countersListEnd;
static signed countersT;



signed countersTotal()
{
 return countersT;
}




#include <string.h>
int countersF()
{
 char* e;
 for(char* s = countersList; *s; s = e+1)
 {
    e = strchr(s, '\n');
    if(!e) return -1;
    *e = 0;
    pf("%s\n", s);
 }
}





/*============================================================================
 * ZERO
 * Prepares for a new count.
 */
int countersZero()
{
 countersListEnd = countersList;
 *countersList = '\0';
 countersT = 0;
}





/*=============================================================================
 * ADD
 */
int countersAdd(char *name, signed cnt)
{
 countersT += cnt;

 //@@ not yet all elements have names.... //
 if(name == NULL) name = "[]";

 if(countersList && cnt)
    countersListEnd += sprintf(countersListEnd, "%s\t%12d\n", name, cnt);
}



int countersAddP(char *name, signed cnt, signed p)
{
 countersT += cnt;

 //@@ not yet all elements have names.... //
 if(name == NULL) name = "[]";

 if(countersList && cnt)
    countersListEnd += sprintf(countersListEnd, "%s\t(%+4d\%) %4d\n", name, p, cnt);
}





/*=============================================================================
 * STAT COUNTER
 */
void cntDrawStat(int id, int tot, int plus, int minus)
{
 if(tot < 1) return;

 float width = 15;	// size of 15 icons
 float jump = 1.3;	// space between a group and the other

 // calcualte step size
 double s;
 int nj = (tot-1) / 5;
 s = (width - nj*jump) / (tot-nj);
 if(s > 1) s = 1;

 // marble or gold?
 int base = 0;

 glDisable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);
 glEnable(GL_ALPHA_TEST);
 glColor4f(1,1,1,1);
 glPushMatrix();
 for(int i = 0; i < tot; i++)
 {
    if(i >= tot - plus) base = 1;		// bonuses
    if(i >= tot - minus) glColor4f(.3,.3,.3,.3); // maluses

    counters.Quad(base);	// item base (marble or gold)
    counters.Quad(id);		// item
    if(i%5 == 4)	glTranslated(jump, 0, 0);
    else		glTranslated(s, 0, 0);

 }
 glPopMatrix();
}





/*=============================================================================
 * RES COUNTER
 */
float cntDrawRes(bool mute, int id, signed t)
{
 // irregular quads...
 float b;
 switch(id) {
    case CNT_FOOD:
    case CNT_GOLD:
    case CNT_WORK:
    case CNT_KNOW: b = 1.5; break;

    case CNT_MANA:
    default: b = 1; break;
 }

 if(!mute)
 {
    if(t < 0)	glColor3f(.1,.1,.1);
    else	glColor3f(1.,1.,1.);
    glEnable(GL_BLEND);
    //?glEnable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
 }

 // find width
 if(t < 0) t = -t;
 float s = 9.5 / ( (t/10)*b + (t%10) );
 if(s > 1) s = 1;
 b *= s;




 // total width
 float w = 0;
 int n;

 // big
 n = t / 10;
 w += b * n;
 if(!mute) for(int i = 0; i < n; i++)
    glTranslated(b/2, 0, 0),
    counters.Rect(id+1, b, 1),
    glTranslated(b/2,0,0);

 // small
 n = t % 10;
 w += s * n;
 if(!mute) for(int i = 0; i < n; i++)
    glTranslated(s/2,0,0),
    counters.Quad(id),
    glTranslated(s/2,0,0);

 return w;
}



void cntDrawRes(int id, int t)
{
 cntDrawRes(false, id, t);
}





static float fntECounterF(bool mute, char* s, char** e, float h)
{
 int id;
 switch(*s++) {
    case 'F': id = CNT_FOOD; break;
    case 'W': id = CNT_WORK; break;
    case 'G': id = CNT_GOLD; break;
    case 'M': id = CNT_MANA; break;
    case 'K': id = CNT_KNOW; break;
    default: return 0;
 }

 int n = strtol(s, e, 10);
 if(*e == s) n = 1;		// no number following, so it's just a unit
 if(n > 200) n = 200;		// cap


 if(!mute)
 {
    glPushMatrix();
    glScaled(h, h, 1);
    glTranslated(0, .5, 0);
 }


 float w = h * cntDrawRes(mute, id, n);


 if(!mute)
 {
    glPopMatrix();
    glTranslated(w, 0, 0);
    fntRestoreColor();
 }


 return w;
}
static fntEscapeC fntECounter(fntECounterF);





/*=============================================================================
 * UPKEEP
 */
void cntDrawUpkeep(int f, int g, int m)
{
 glPushMatrix();
 glDisable(GL_DEPTH_TEST);
 glColor4f(1,1,1,1);

 cntDrawRes(CNT_FOOD, f);
 cntDrawRes(CNT_GOLD, g);
 cntDrawRes(CNT_MANA, m);
 
 glPopMatrix();
}





//EOF =========================================================================
