/*
 MAP_DISPLAY.CPP	v0.21
 display_map() plus helper functions.

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
#include <stdlib.h>
#include "race.h"

#include "main.h"
#include "data.h"
#include "visual.h"

#include "game.h"
#include "city.h"
#include "unit.h"
#include "map.h"

#include "gselect.h"



#define ANIMATION_TIME		200
#define BATTLE_BLINK_RATE	400



static envC envRot("rot", "30");

extern dataC map_landData;


/*=============================================================================
 * TEMPO
 */
#include <GL/glut.h>
static long dlandTEMPO_last;

int dlandTEMPO()
{
 long T = glutGet(GLUT_ELAPSED_TIME);

 if(T - dlandTEMPO_last < ANIMATION_TIME) return 0;

 dlandTEMPO_last = T;
 return 1;
}





/*=============================================================================
 * TERRAIN
 */
static void dlandDisplayTerrain(cellC *c, bool sel)
{
 glPushMatrix();
    glScaled(1/256., 1/256., 8/256.);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA);
    c->Land->Render(c);

    if(gsOff())
	if(sel)
	    {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(4);
		glColor3f(.9, .2, .2);
		glBegin(GL_LINE_STRIP);
		    c->VertexDisplay(1);
		    c->VertexDisplay(2);
		    c->VertexDisplay(3);
		    c->VertexDisplay(4);
		    c->VertexDisplay(5);
		    c->VertexDisplay(6);
		    c->VertexDisplay(7);
		    c->VertexDisplay(0);
		    c->VertexDisplay(1);
		glEnd();
		glEnable(GL_DEPTH_TEST);
	    }
	else
	    {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glLineWidth(2);
	    	glColor4f(.2, .8, .2, .2);
		glBegin(GL_LINE_STRIP);
		    c->VertexDisplay(1);
		    c->VertexDisplay(2);
		    c->VertexDisplay(3);
		    c->VertexDisplay(4);
		    c->VertexDisplay(5);
		glEnd();
	    }

 glPopMatrix();
}





/*=============================================================================
 * UNIT CITY PLUS
 */
static void dlandDrawSquare(double sX, double sY, double eX, double eY)
{
 sX += (eX - sX) / 128.0;	// remove 0.5 pixel from texture
 eX -= (eX - sX) / 128.0;	// to avoid imprecise texture selection
 sY += (eY - sY) / 128.0;
 eY -= (eY - sY) / 128.0;

 glBegin(GL_QUADS);
    glTexCoord2d(sX, eY);	glVertex2f(-.5, +.5);
    glTexCoord2d(eX, eY);	glVertex2f(+.5, +.5);
    glTexCoord2d(eX, sY);	glVertex2f(+.5, -.5);
    glTexCoord2d(sX, sY);	glVertex2f(-.5, -.5);
 glEnd();
}



void dlandDisplayCity(cellC* c)
{
 c->City->Race->Land.BindTexture();
 glEnable(GL_TEXTURE_2D);
 glEnable(GL_BLEND);
 glDisable(GL_DEPTH_TEST);
 glColor3f(1, 1, 1);

 // P is city total population count
 // size factor is:
 // F = A log(P) + F0
 double Fmax = 2.6;
 double Fmin =  .7;
 double Pmax = 30000;
 double Pmin = CITY_SETTLERS_POPULATION;

 double A = (Fmax - Fmin) / log(Pmax/Pmin);
 double F0 = Fmax - A*log(Pmax);

 double f = F0 + A*log(c->City->TotalPop*1000 + c->City->Pop);
 glPushMatrix();
    glScaled(f, f, 1);
    dlandDrawSquare(1., 1., 0., 0.);
 glPopMatrix();
}


void dlandDisplayBattle(cellC* c)
{
 map_landData.BindTexture(); //@@ should be put somewhere else....
 glEnable(GL_TEXTURE_2D);
 glEnable(GL_BLEND);
 glDisable(GL_DEPTH_TEST);
 glColor3f(1, 1, 1);

 glPushMatrix();
    glScaled(2, 2, 1);
    dlandDrawSquare(1./16, 1./8, 0., 0.);
 glPopMatrix();
}


void dlandDisplayUnit(cellC* c, unitC* u)
{
 void dunitDisplay(unitC*);
 if(!u) return;

 glPushMatrix();
    glRotated(90, -1, 0, 0);
    if(u->TravelAngle == -1) glRotated(180, 0, 1, 0);
    glScaled(2.1,2.1,2.1);
    dunitDisplay(u);
 glPopMatrix();
}



void dlandDisplayMovingUnit(cellC* c, unitC* u)
{
 int a = u->TravelAngle;

 if(a == -1)
 {
    dlandDisplayUnit(c, u);
    return;
 }

 double dist = 0;
 if(u->ActionStatus)	dist = .4 + 1.6 * u->ActionStatus / 7.;
 else			dist = .4;
 if(a&1) dist *= 1.4;

 glPushMatrix();
    glRotated(a*45., 0,0,-1);
    glTranslated(0, dist, 0);
    dlandDisplayUnit(c, u);
 glPopMatrix();
}



/*
unitC* findNextTeam(unitC* start, unitC* tm)
{
 long prevTeam = tm ? tm->Team : 0;
 unitC* best = 0;

 for(unitC* u = start; u; u = u->Next) if(u->Team > prevTeam)
 {
    if(!best) best = u;
    else if(u->Team < best->Team) best = u;
 }

 return best;
}
*/




void dlandDisplayUnitCityPlus(cellC* c)
{
 glPushMatrix();
    glTranslated(0, 0, c->Center.h*8/256.);
    if(c->City) dlandDisplayCity(c);
    // else c->Plus->Render(c);

    if(c->Battle)
    {
	int t = timeElapsed() + (c-game->Map.C)*100;
	t /= BATTLE_BLINK_RATE;
	if(t & 1) dlandDisplayBattle(c);
    }
    else
    {
	// units
	for(int angle = -1; angle < 8; angle++)
	{
	    unitC* u;
	    for(u = c->Unit; u && u->TravelAngle != angle; u = u->Next);
	    if(u) dlandDisplayMovingUnit(c, u);
	}

    }
 glPopMatrix();
}






/*=============================================================================
 * SIGNALS
 */
static void dlandDisplayPathling(cellC* c, int a, unsigned l)
{
 glEnable(GL_BLEND);
 glRotated(a*45, 0, 0, -1);
 glScaled(1, 1, 8/256.);
 if(a&1) glScaled(1.2, 1.4, 1.);

 int hstart = c->Center.h;
 int hend = c->SMAC(a&7)->Center.h;

 int length = 5;
 l %= length;
 double q = (length+length+l) / (3.*length*255);

 glBegin(GL_TRIANGLES);
    glColor4f(q*20,q*180,q*20,230/255.); glVertex3f(  0.,124./60., hend);
    glColor4f(q*20,q*150,q*20,190/255.); glVertex3f(+20./60,   0., hstart);
    glColor4f(q*20,q*110,q*20,180/255.); glVertex3f(-20./60,   0., hstart);
 glEnd();
}




static void dlandDisplayPath(cellC* c, unitC* u)
{
 int t = timeElapsed()/250;

 glDisable(GL_DEPTH_TEST);

 char* start = u->LandOrders.Path.Status;
 if(u->IsTraveling()) start--;


 for(char* p = start; p < u->LandOrders.Path.End; p++)
 {
    int w = game->Map.W;

    glPushMatrix();
	glTranslated(scrX(c->X), scrY(c->Y), 0.0);
	glScaled(scrW(.5), scrH(.5), scrW(.5));
	dlandDisplayPathling(c, *p, t);
    glPopMatrix();

    glPushMatrix();
	glTranslated(scrX(c->X-w), scrY(c->Y), 0.0);
	glScaled(scrW(.5), scrH(.5), scrW(.5));
	dlandDisplayPathling(c, *p, t);
    glPopMatrix();

    glPushMatrix();
	glTranslated(scrX(c->X+w), scrY(c->Y), 0.0);
	glScaled(scrW(.5), scrH(.5), scrW(.5));
	dlandDisplayPathling(c, *p, t);
    glPopMatrix();
    t--;
    c = c->SMAC(*p&7);
 }
}





/*=============================================================================
 * PRINTCOORDS
 */
void findfrustum(double* wt, double *wb, double* ht, double* hb)
{
/* glColor4f(0,0,1,1);
 glBegin(GL_QUADS);
    glVertex3f(-30, +30, 0);
    glVertex3f(+30, +30, 0);
    glVertex3f(+30, -30, 0);
    glVertex3f(-30, -30, 0);
 glEnd();
*/

 GLint viewport[4];
 GLdouble mvmatrix[16];
 GLdouble projmatrix[16];

 glGetIntegerv(GL_VIEWPORT, viewport);
 glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
 glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

 int y;
 int x;
 float z;
/*
 x = mouseX();
 y = (screenH() - mouseY() -1);
 glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

 GLdouble mx, my, mz;
 gluUnProject(x, y, z, mvmatrix, projmatrix, viewport, &mx, &my, &mz);

 glDisable(GL_DEPTH_TEST);
 glColor4f(0,1,0,1);
 glPointSize(5.);
*/

 double ROT = atof(envGet("rot"));
 double TG = tan(3.14*ROT/180.);
 double tztan = 0.969388 + TG*0.01 * (23892./27422.);
 double bztan = 0.969388 - TG/47;

 //sia la costante 0.9XXXX che i coefficenti possono essere ricavati
 //con un paio di glReadPixels...
 // la costante � semplicemente il read pixels quando ROT � 0
 // le altre due vanno trovate con un piano blu

 // bl
 x = viewport[0] + viewport[2]/4;
 y = viewport[1] + viewport[3]/4;
// glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
 z = bztan;

 double blx, bly, blz;
 gluUnProject(x, y, z, mvmatrix, projmatrix, viewport, &blx, &bly, &blz);
// double bz = z;

 // tl
 x = viewport[0] + viewport[2]/4;
 y = viewport[1] + 6*viewport[3]/10;
// glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
 z = tztan;
 double tlx, tly, tlz;
 gluUnProject(x, y, z, mvmatrix, projmatrix, viewport, &tlx, &tly, &tlz);
// double tz = z;
/*
 glBegin(GL_POINTS);
    glVertex3f(mx, my, 0);
    glVertex3f(blx, bly, 0);
    glVertex3f(tlx, tly, 0);
 glEnd();
*/
// __f("%f %f %f %f\n", tlx, tly, blx, bly);

 *wt = tlx; // * 2.;
 *wb = blx; // * 2.;
 *ht = tly;
 *hb = bly;

// fprintf(stderr, "+%3f    %f %f     %f %f\n", ROT,
//    tz -0.969388,
//    tztan -0.969388,
//    bz -0.969388,
//    bztan -0.969388);
}





/*=============================================================================
 * DISPLAY LAND
 *
 * Main function.
 * Displays a w * h pixel rectangular map area centered on x, y.
 */
void dlandDisplay(void* selection)
{
 int upd = dlandTEMPO();
 int X, Y;
 int sX, sY, eX, eY;

 sX = (int)floor(mapX(-1.3)) -1;
 sY = (int)floor(mapY(-1.3));// -1;
 if(sY < 0) sY = 0;

 eX = (int)ceil(mapX( 1.3)) +1;
 eY = (int)ceil(mapY( 1.3));// +1;
 if(eY > game->Map.H) eY = game->Map.H;


 double fovy = 80;
 double aspect = 1.; //@@ to depend on window/viewport?
 double znear = .1;
 double zfar = 5;

 glLoadIdentity();
 gluPerspective(fovy, aspect, znear, zfar);
 glTranslated(0, 0, -2.);
 glRotated(atof(envGet("rot")), -1, 0, 0);
// glScaled(1., 1., 1.);


// glColor4f(1.0, 1.0, 1.0, 1.0);
// glEnable(GL_BLEND);
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 // ===========================================================================
 // Sweep 1: TERRAIN TEXTURE
 glEnable(GL_DEPTH_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA);

 glPushMatrix();
 glScaled(scrW(.5), scrH(.5), scrW(.5));
 double fwt, fwb, fht, fhb;
 findfrustum(&fwt, &fwb, &fht, &fhb);
 glPopMatrix();

 map_landData.BindTexture();
 for(Y = sY; Y < eY; Y++) for(X = sX; X < eX; X++)
    {
//	if(upd) dlandUpdateCellFrame(map(X, Y));

	glPushMatrix();
	    glTranslated(scrX(X), scrY(Y), .0);
	    glScaled(scrW(.5), scrH(.5), scrW(.5));
	    cellC* c = map(X, Y);
	    gsPush(c);
	    dlandDisplayTerrain(c, c == selection);
	glPopMatrix();
    }

 glPushMatrix();
 glScaled(scrW(.5), scrH(.5), scrW(.5));
 glDisable(GL_DEPTH_TEST);
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA);
 glColor4f(1,0,0,1);
 glLineWidth(2.);
 glBegin(GL_LINE_LOOP);
//    glVertex3f(fwt, fht, 0);
//    glVertex3f(fwb, fhb, 0);
    glVertex3f(+fwt, fht, 0);
    glVertex3f(-fwt, fht, 0);
    glVertex3f(-fwb, fhb, 0);
    glVertex3f(+fwb, fhb, 0);
 glEnd();
 glPopMatrix();
 

/*
 //============================================================================
 // Sweep 2: OTHER TEXTURES
 if(gsOff()) for(Y = sY; Y < eY; Y++) for(X = sX; X < eX; X++)
    {
	glPushMatrix();
	    glTranslated(scrX(X), scrY(Y), 0.0);
	    glScaled(scrW(.5), scrH(.5), scrW(.5));
	    cellC* c = map(X, Y);
	    dlandDisplayUnitCityPlus(c);
//	    dlandDisplaySignals(c);
	glPopMatrix();
    }


 // ===========================================================================
 // Sweep 3: UNIT PATH
 if(gsOff() && game->Self->LocalFocus)
 {
    unitC* u;
    for(u = game->Self->LocalFocus->Unit; u && u->Team != game->Self->SelectedTeam; u = u->Next);
    if(u) dlandDisplayPath(game->Self->LocalFocus, u);
 }
*/
 // done
}





/*=============================================================================
 * MINIMAP DRAW
 *
 * Draws a minimap.
 */
void minimapDraw(int pixelW, int pixelH)
{
 int w = game->Map.W;
 int h = game->Map.H;

 glViewport(0, 0, screenW(), screenH());
 glLoadIdentity();
 glClear(GL_DEPTH_BUFFER_BIT);
 glEnable(GL_DEPTH_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA);
 map_landData.BindTexture();

 double sw = pixelW / (double)screenW() / w;
 double sh = pixelH / (double)screenH() / h;

 glTranslated(-1, -1, 0);


// glPushMatrix();
 glScaled(sw, sh, sw);
 glTranslated(1,1,0);
 for(int Y = 0; Y < h; Y++) for(int X = 0; X < w; X++)
    {
	glPushMatrix();
	    glTranslated(X*2, Y*2, .0);
	    cellC* c = map(X, Y);
	    dlandDisplayTerrain(c, false);
	glPopMatrix();
    }
/*
 glPopMatrix();

 glColor4f(1,0,0,1);
 glDisable(GL_DEPTH_TEST);
 glBegin(GL_LINE_STRIP);
    glVertex2f(1, 0);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
 glEnd();
*/


}





//EOF =========================================================================
