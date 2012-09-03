/*
 UNIT_DRAW.CPP		v0.05
 dunitDisplay().

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
#include <GL/gl.h>
#include <math.h>

#include "main.h"
#include "data.h"
#include "counters.h"
#include "txt.h"

#include "game.h"
#include "unit.h"
#include "race.h"
#include "utype.h"

#include "flesh.h"
#include "ske.h"
#include "colors.h"
#include "random.h"
#include "boxgui.h"
#include "spell.h"




// FAKES ======================================================================
double udParm[] = {
     20,	// hl: minimum walk hip angle
     40,	// hu: maximum walk hip angle
     20,	// ee: walk elbow angle
     10,	// sz: walk shoulders width
     10,	// tr: walk maximum torso roll
      1		// tp: walk torso pitch
      };
static double* ppp = udParm;





/*=============================================================================
 * HEALTHBAR
 */
void duHealth(unitC* u)
{
 glDisable(GL_DEPTH_TEST);
 glDisable(GL_ALPHA_TEST);
 glDisable(GL_TEXTURE_2D);

 glEnable(GL_BLEND);
 glColor4f(.4,.4,.7,.7);
 glBegin(GL_QUADS);
    glVertex2d(-.4,-.05);
    glVertex2d(-.4,+.05);
    glVertex2d(+.4,+.05);
    glVertex2d(+.4,-.05);
 glEnd();


 float n = u->Wounds / (double)(u->Type->Life*u->Type->Figures);

 glDisable(GL_BLEND);
	if(n > .6)	glColor3f(.8, .0, .0);
 else	if(n > .4)	glColor3f(.8, .8, .0);
 else			glColor3f(.0, .8, .0);

 float x = .37 * 2 * (.5-n);
 glBegin(GL_QUADS);
    glVertex2d(-.37,-.02);
    glVertex2d(-.37,+.02);
    glVertex2d(   x,+.02);
    glVertex2d(   x,-.02);
 glEnd();
}





/*=============================================================================
 * STATS
 */
#include "fnt.h"
static char* stru(unitC* u)
{
 static char bf[40];
 sprintf(bf, "%d %s", u - game->Units, u->Type->Name);
 return bf;
}



void duStats(unitC* u)
{
 //@@ movement type


 //@@ DEBUG--------------------------------------------------------------------
 if(envIsTrue("debug")) {
 glPushMatrix();
    glTranslated(220, 250, 0);
    fntS(70);
    glColor3f(1,0,0);
    fntL("%s team %d\n", stru(u), u->Team);
    if(u->Transported)	fntL("trans by %s\n", stru(u->Transported));
    else		fntL("NOT transported\n");

    if(u->Type->Transport)
    {
	fntL("Transports:\n");
	for(unitC* t = u->NextTransported; t; t = t->NextTransported)
	    fntL("   %s\n", stru(t));
    }
 glPopMatrix();
 }
 //----------------------------------------------------------------------------



 // combat stats
 glPushMatrix();
    glTranslated(.03, .01, 0);
    glScaled(.05, .05, 1);

    cntDrawStat(CNT_MELEE, u->Melee(), 0, 0);		glTranslated(0,-1,0);
    cntDrawStat(CNT_POISON, u->Poison(), 0, 0);		glTranslated(0,-1,0);
    cntDrawStat(CNT_RANGE, u->Range(), 0, 0);		glTranslated(0,-1,0);
    cntDrawStat(CNT_HITBONUS, u->HitModifier(), 0, 0);	glTranslated(0,-1,0);

    cntDrawStat(CNT_DEFENSE, u->Defense(), 0, 0);	glTranslated(0,-1,0);
    cntDrawStat(CNT_RESISTANCE, u->Resist(), 0, 0);	glTranslated(0,-1,0);

    int he = u->Health();
    cntDrawStat(CNT_HITS, he, 0, u->Wounds%he);		glTranslated(0,-1,0);
 glPopMatrix();


 // specials
 glTranslated(0, -.4, 0);
 glPushMatrix();

 glDisable(GL_DEPTH_TEST);
 glColor4f(1,1,1,1);
 glScaled(.1, .1, 1);
 glTranslated(.5 +.05, .5 -.2, 0);
 spcC* s;
 for(int i = 0; s = u->Type->Special(i); i++) if(s->Gfx != -1)
 {
    extern dataC dataSpc;
    dataSpc.Quad(s->Gfx);
    glTranslated(1, 0, 0);
 }
 glPopMatrix();


 //@@ items (heroes only!)

 // enchantements
 glTranslated(0, -.1, 0);
 fntS(.03);
 bTColor();
 for(spellC* s = spellsHook; s; s = s->SpellNext)
    if(s->Type == &spellTypeUnit && s->Object != SPELL_OBJ_NONE)
	if(u->EnchTemp[s->Object])
	    fntL("%s\n", s->Name);
}




#include "boxgui.h"


static spcC* duMouseSpc = 0;
bool duHSpc() { return duMouseSpc ? true : false; }


static unitC* duTargUnit = 0;
void duSetUnit(unitC* u) { duTargUnit = u; }



bool duSpcInput(boxC& b)
{
 if(!b.Mouse || !duMouseSpc) return false;

 if(mouseL() || mouseM())
 {
    popupHelpS::Open(duMouseSpc->Name, duMouseSpc->Info);
    return true;
 }

 return false;
}



void duSpcDisplay(boxC& b)
{
 unitC* u = duTargUnit;
 if(!u) return;
 if(!u->Type) return;

 glScaled(b.H, b.H, 1);
 glTranslated(.5, .5, 0);

 duMouseSpc = 0;
 int mid = b.Mouse ? (int)(b.MouseX/b.H) : -1;

 glColor3f(1,1,1);
 spcC* s;
 for(int i = 0; s = u->Type->Special(i); i++) if(s->Gfx != -1)
 {
    if(i == mid) duMouseSpc = s;
    extern dataC dataSpc;
    dataSpc.Quad(s->Gfx);
    glTranslated(1, 0, 0);
 }
}




boxC* duNewStatsAndSpecials()
{
 boxC* main = new boxC;
 boxSC* p = &main->DefaultS;

/*
 main->DisplayF = duSASDisplay;
*/


 p->AddWindow("Specials",
    "These icons represents special abilities of the troop unit."
    "\nSpecial abilities deeply influence a troop capabilities"
    " so be sure to know what they do."
    "\nTo get a description of a particular special ability,"
    " left or mid click on it.\n",
    0, 0, 1, .16,
    (boxInputF)duSpcInput, (boxDisplayF)duSpcDisplay);

/*
 float x, y, s;
 boxC* b;


 for(int i = 0; i < 20; i++)
 {
    b = p->AddWindow(0, 0, 
    "",
    0, 0, 
 );


 }
 // natural specials
 // hero specials
 // ench
 // item specials
 // playermage retorts
 b = p->AddWindow("Specials",
    "",
    0, 0, 
 );







 //
 b = p->AddWindow("","", 0, )
 b->Id = CNT_MELEE;

*/

 return main;
}








/*=============================================================================
 * UNIT
 */
void duUnit(unitC* u, float t)
{
 glDisable(GL_TEXTURE_2D);

 double angles[SKE_TOTAL];
 for(int k = 0; k < SKE_TOTAL; k++)
    angles[k] = u->Animation->Status(ppp, t, k);

 fleshDraw(&u->Type->Flesh, angles, u->Color());
}



void duUnit(unitC* u) { duUnit(u, u->AnimationTime); }





/*=============================================================================
 * TYPE
 * @@ really required?
 */
void duType(utypeC* u, colorC* color)
{
 glDisable(GL_TEXTURE_2D);

 animationC* ani = animationC::Get("idle");

 double angles[SKE_TOTAL];
 for(int k = 0; k < SKE_TOTAL; k++)
    angles[k] = ani->Status(ppp, 0, k);

 fleshDraw(&u->Flesh, angles, color);
}





/*=============================================================================
 * FLESH MULTI DISPLAY
 */


#define FLESH_MULTI_BF	50

static float fleshMultiStatus[FLESH_MULTI_BF];
static randomC fleshMultiRnd;
static unsigned fleshMultiId = 0;
static long fleshTEMPO_last = 0;

void fleshMultiStart() { fleshMultiId = 0;}

static tempoC fleshTempo(200);


static void fleshMultiUpdate()
{
 for(int i = 0; i < FLESH_MULTI_BF; i++)
    {
	float* f = fleshMultiStatus + i;
	*f += .05*(fleshMultiRnd() - .1);
	*f -= (int)(*f);
    }
}




void fleshMultiAnimate(fleshC* flesh, colorC* color)
{
 if(fleshTempo()) fleshMultiUpdate();

 float status = fleshMultiStatus[(fleshMultiId++)%FLESH_MULTI_BF];

 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA_TEST);
 glEnable(GL_DEPTH_TEST);

 animationC* ani = animationC::Get("idle");

 double angles[SKE_TOTAL];
 for(int k = 0; k < SKE_TOTAL; k++)
    angles[k] = ani->Status(ppp, status, k);

 fleshDraw(flesh, angles, color);
}





/*=============================================================================
 * STACK
 */
void duStack(unitC* u)
{
 float* stack = u->Stack();
 int fig = u->Figures();

 // init random
 randomC rnd(u->Seed);

 for(int i = 0; i < fig; i++)
 {
    // offset the animation time a bit
    float t = u->AnimationTime + .3*rnd() -.15;

    // draw
    glPushMatrix();
	glTranslated(stack[i*2], 0, stack[i*2+1]);
	float h = .9 + .2*rnd();
	float w = .8 + .4*rnd();
	glScaled(w, h, w);
	duUnit(u, t);
    glPopMatrix();
 }
}



void duTypeStack(utypeC* u, colorC* c)
{
 float* stack = u->Stack();
 int fig = u->Figures;

 fleshMultiStart();
 for(int i = 0; i < fig; i++)
 {
    glPushMatrix();
    	glRotated(180, 0, 1, 0);
	glTranslated(stack[i*2], -stack[i*2+1]/2, stack[i*2+1]);
	fleshMultiAnimate(&u->Flesh, c);
    glPopMatrix();
 }
}





//EOF =========================================================================
