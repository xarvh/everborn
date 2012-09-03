/*
 subBLO.cpp		v0.01
 subprogram: BLOck viewer

 Copyright (c) 2006 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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

 NOTE:
 Once this was a standalone.
*/

#include <GL/gl.h>
#include <stdlib.h>

#include "main.h"
#include "boxgui.h"

#include "colors.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "race.h"

#include "flesh.h"
#include "ske.h"
#include "txt.h"



/*=============================================================================
 * GLOBALS
 */
static double Ax = 0.0, Ay = 180.0, Az = 0.0;

static double sp = 0.004;
static double t = 0.0;

static raceC* race = NULL;

static int unitID = 0;
static unsigned color = 0;



extern double udParm[];
static double* skeParms = udParm;



/*=============================================================================
 * SWITCHES
 */
static int BLOske(unsigned pa, double d)
{
 extern skeParmT skeParmA[];
 skeParmT *p;

 if(pa >= PARMS_CNT) return 0;
 p = skeParmA + pa;

 d += skeParms[pa];
 if(d > p->max) d = p->max;
 if(d < p->min) d = p->min;

 skeParms[pa] = d;
}


static int BLOcolor(signed dc)
{
 color += dc;
}


static void BLOunit(int u)
{
 if(!race) return;
 if(race->GetUtype(unitID + u)) unitID += u;
 pf("%s\n", race->GetUtype(unitID)->Name);
}


static int displaySpeed(int w)
{
 sp += w * 0.004;
}



static int displayRotate(double x, double y, double z)
{
 Ax += x;
 Ay -= y;
 Az += z;
}


static void BLOrace(raceC* r)
{
 if(race) delete race;
 race = r;
 if(race) pf("race: %s\n", race->Name);
}





/*=============================================================================
 * LOAD
 */
static envC envBLOrace("BLOrace", "EI.raz", 0);


static int BLOrelo()
{
 char *fn, *path, *file, err[1000];
 raceC* r;

 fn = envGet("BLOrace");

 path = dataFindPath(fn);
 if(path == NULL)
    {
	pe("can't find race file %s\n", fn);
	return -1;
    }

 file = txtLoad(path);
 free(path);
 if(file == NULL)
    {
	pe("can't load race file %s\n", fn);
	return -1;
    }

 r = raceLoad(file, err);
 free(file);
 if(r == NULL)
    {
	pe("error: %s\n", err);
	return -1;
    }

 BLOrace(r);

 return 0;
}



static int BLOreloBase()
{
 char *path, *file, err[1000];
 int r;

 if(!game) game = new gameC(0,0,0,0);

 path = dataFindPath("everborn.game");
 if(path == NULL)
    {
	pe("can't find game\n");
	return -1;
    }

 file = txtLoad(path);
 free(path);
 if(file == NULL)
    {
	pe("can't load game\n");
	return -1;
    }

 r = game->Read(file, err);
 free(file);
 if(r)
    {
	pe("error: %s\n", err);
	return -1;;
    }

 return 0;
}





/*=============================================================================
 * DISPLAY
 */
static void BLODisplay(fleshC *flesh, double time, int colorID)
{
 double angles[SKE_TOTAL];
 int k;

 animationC* ani = animationC::Get("walk");

 glColor4d(1.,1.,1.,1.);

 for(k = 0; k < SKE_TOTAL; k++)
    angles[k] = ani->Status(skeParms, time, k);

 fleshDraw(flesh, angles, colorGet(colorID));
}





static void MENUdraw(boxC& b)
{
 utypeC* u;

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glLoadIdentity();
 glTranslated(.5, b.H/2, 0);
 glScaled(.5, .5, .5);
 glDisable(GL_DEPTH_TEST);

 glDisable(GL_ALPHA_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_TEXTURE_2D);

 glPushMatrix();
    glTranslated(0.0, -0.35, 0.0);
    glScaled(0.7, 0.7, 0.7);
    glRotated(Ax, 1.0, 0.0, 0.0);
    glRotated(Az, 0.0, 0.0, 1.0);

    glColor4f(.0, .0, 1., 1.);
    screenLineWidth(2);
    glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 1.0);
	glVertex2f(+0.5, 1.0);
	glVertex2f(+0.5, 0.0);
	glVertex2f(-0.5, 0.0);
    glEnd();

    glRotated(Ay, 0.0, 1.0, 0.0);

    if(race)
    {
	int id = unitID;

	for(u = race->Units;
	    u &&
	     u->Next
	      && id--;
		u = u->Next);
        BLODisplay(&u->Flesh, t, color);
    }
 glPopMatrix();

 glTranslated(-0.8, 0.0, 0.0);

 glPushMatrix();
    glScaled(0.15, 0.15, 0.15);
    glRotated(Ax, 1.0, 0.0, 0.0);
    glRotated(Az, 0.0, 0.0, 1.0);
    glRotated(Ay, 0.0, 1.0, 0.0);
    glTranslated(0.0, -0.5, 0.0);
    if(race) BLODisplay(&u->Flesh, t, color);
 glPopMatrix();


 t += sp;
 if(t > 1.0) t = 0.0;
 if(t < 0.0) t = 1.0;
}






/*=============================================================================
 * MENU STUFF
 */
static bool MENUinput(boxC& b)
{
 switch(inputK()) {
    case 27: exit(0); break;

    case 32: BLOrelo(); break;

    case 'a': displayRotate(0.0, -5.0, 0.0); break;
    case 'd': displayRotate(0.0, +5.0, 0.0); break;

    case 'w': displayRotate(+5.0, 0.0, 0.0); break;
    case 's': displayRotate(-5.0, 0.0, 0.0); break;

    case 'q': displayRotate( 0.0, 0.0,+5.0); break;
    case 'e': displayRotate( 0.0, 0.0,-5.0); break;

    case 'z': BLOcolor(+1); break;
    case 'x': BLOcolor(-1); break;

    case '+': displaySpeed(+1); break;
    case '-': displaySpeed(-1); break;

//    case '.': displaySwitchMode(); break;

    case 'm': BLOunit(+1); break;
    case 'n': BLOunit(-1); break;

    case 'r': BLOske(0, +5.0); break;
    case 'f': BLOske(0, -5.0); break;
    case 't': BLOske(1, +5.0); break;
    case 'g': BLOske(1, -5.0); break;
    case 'y': BLOske(2, +5.0); break;
    case 'h': BLOske(2, -5.0); break;
    case 'u': BLOske(3, +5.0); break;
    case 'j': BLOske(3, -5.0); break;
    case 'i': BLOske(4, +5.0); break;
    case 'k': BLOske(4, -5.0); break;
    case 'o': BLOske(5, +5.0); break;
    case 'l': BLOske(5, -5.0); break;

    default: break;
 }

 switch(inputE()) {
/*    case GLUT_KEY_UP:	visualScroll( 0.0, +sp); break;
    case GLUT_KEY_DOWN: visualScroll( 0.0, -sp); break;
    case GLUT_KEY_LEFT: visualScroll( -sp, 0.0); break;
    case GLUT_KEY_RIGHT:visualScroll( +sp, 0.0); break;*/
//    case GLUT_KEY_F1: quadSwitchSurfaceMode(); break;
//    case GLUT_KEY_F2: quadSwitchWireMode(); break;
    default: break;
 }

 // buttons
/* bt_t *a;
 if(key) if(a = guiBtKey(MENUbtset, key)) a->call(a);
 if(a = guiBtMouse(MENUbtset, guiMouseX(), guiMouseY()))
    if(a->call) a->call(a);
*/
}




static char* MENUstart()
{
 if(BLOreloBase())
 {
    pe("BLOreloBase: unable to load base\n");
    exit(-1);
 }

 BLOrelo();

 return 0;
}





/*=============================================================================
 * BLO MENU
 */
static bmenuDECLARE ( __blo ) {

    StartF = MENUstart;
    DisplayF = MENUdraw;
    InputF = MENUinput;

} bmenuINSTANCE;



//EOF =========================================================================
