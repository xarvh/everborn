/*
 MAP_DISPLAY.CPP	v0.23
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

#include "main.h"
#include "data.h"
#include "cmd.h"

#include "game.h"
#include "race.h"
#include "city.h"
#include "unit.h"
#include "map.h"

#include "colors.h"
#include "gselect.h"
#include "map_view.h"
#include "fnt.h"

#define ANIMATION_TIME		100
#define BATTLE_BLINK_RATE	400


/*=============================================================================
 * GLOBALS
 */
dataC mdPlus("plus.png", 8, 8);


extern dataC mdData;

static tempoC mdTempo(ANIMATION_TIME);

static void*	mdSelectedCell;
static long	mdBit;
static float	mdRot;
static bool	mdUpd;

char mdZ = 0;





/*=============================================================================
 *
 */
void mdSwitchPlane()
{
 mdZ++;
 mdZ &= 1;
}





/*=============================================================================
 * FOCUS HALO
 */
static void mdFocusHalo(float h, cellC* c)
{
 float s = (self->Focus == c) ? .7 : .3;
 int t = (timeGet() / 50)%1000;

 double step = 2*M_PI/50;
 int r = self->Color->R;
 int g = self->Color->G;
 int b = self->Color->B;

 glEnable(GL_BLEND);
 glEnable(GL_DEPTH_TEST);
 glDisable(GL_TEXTURE_2D);

 glBegin(GL_TRIANGLE_STRIP);
 for(float a = .0; a < 2*M_PI+step; a += step)
 {
    glColor4ub(r,g,b, 20);
    glVertex3f(.2*sin(a), .2*cos(a), h);

    glColor4ub(r,g,b, 255);
    glVertex3f(s*sin(a), s*cos(a), h+.1+.3*sin(M_PI*t/1000));
 }
 glEnd();
}





/*=============================================================================
 * TERRAIN
 */
static void mdDisplayTerrain(cellC *c, bool sel)
{
 if(mdUpd) c->OverS++;

 glPushMatrix();
    glScaled(1/512., 1/512., 4/512.);
    glDisable(GL_ALPHA_TEST);

    // draw land
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    c->Land->Render(c);

    // draw lines
    if(gsOff())
    {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	if(sel)
	{
	    screenLineWidth(3);
	    glColor3f(.9, .2, .2);
	    glBegin(GL_LINE_STRIP);
	        for(int i = 0; i < 8; i++) c->VertexDisplay(i);
	        c->VertexDisplay(0);
	    glEnd();
	}
	else
	{
	    screenLineWidth(2);
	    glColor4f(.2, .8, .2, .2);
	    glBegin(GL_LINE_STRIP);
		for(int i = 1; i < 6; i++) c->VertexDisplay(i);
	    glEnd();
	}

	glEnable(GL_DEPTH_TEST);
    }

 glPopMatrix();
}





/*=============================================================================
 * UNIT
 */
static void mdDisplayUnit(cellC* c, unitC* u)
{
 if(!u) return;

 glPushMatrix();
    glScaled(1, -1, 1);
    glRotated(90, 1, 0, 0);
    if(!u->Travel || u->Travel->Type == TRAVELT_SHIFT)
	glRotated(180, 0, 1, 0);
    duUnit(u);
 glPopMatrix();
}



static void mdDisplayMovingUnit(cellC* c, unitC* u)
{
 if(!u->Travel)
 {
    mdDisplayUnit(c, u);
    return;
 }

 if(u->Travel->Type == TRAVELT_SHIFT)
 {
    float s = .9 - .6*(u->Travel->Status/5.);
    glScaled(s, s, s);
    mdDisplayUnit(c, u);
    return;
 }

 int a = u->Travel->Direction;
 double dist = 0;
 if(u->Travel)	dist = .2 + .8 * u->Travel->Status / 5.;
 if(a&1) dist *= 1.4;

 glPushMatrix();
    glRotated(a*45., 0,0,-1);
    glTranslated(0, dist, 0);

    if(u->Team == self->SelectedTeam) mdFocusHalo(0, c);

    mdDisplayUnit(c, u);
 glPopMatrix();
}




static void mdDisplayUnits(cellC* c)
{
 if( !(mdBit & c->ViewMask) ) return;

 glPushMatrix();
    glTranslated(0, 0, c->Center.h*4/512.);

    // display a non moving unit
    unitC* u;
    for(u = c->Unit; u && u->Travel && u->Transported; u = u->Next);
    if(u) mdDisplayMovingUnit(c, u);

    // display all traveling teams
    for(travelC* t = game->TravelsHook; t; t = t->Next) if(t->Src == c)
	mdDisplayMovingUnit(c, t->Team);
 glPopMatrix();
}





/*=============================================================================
 * PLUS
 */
static void mdDisplayPlus(cellC* c)
{
 if(!c->Plus->Ani.Mode) return; 

 glEnable(GL_BLEND);
 glEnable(GL_ALPHA_TEST);
 glDisable(GL_DEPTH_TEST);
 glColor4f(1,1,1,1);

 // update frame
 if(mdUpd) c->PlusS = c->Plus->Ani.NextFrame(c->PlusS);

 // display
 glPushMatrix();
    glTranslated(0, 0, c->Center.h*4/512.);
    glTranslated(0, .1, .25);
    glRotated(mdRot, 1,0,0);
    mdPlus.Quad(c->PlusS);
 glPopMatrix();
}



/*=============================================================================
 * NEUTRAL
 */
static void mdDisplayNeutral(cellC* c)
{
 glEnable(GL_BLEND);
 glEnable(GL_ALPHA_TEST);
 glDisable(GL_DEPTH_TEST);
 glColor4f(1,1,1,1);

 // update frame
 if(mdUpd); //@@ c->PlusS = c->Plus->Ani.NextFrame(c->PlusS);

 extern dataC blds;

 // display
 glPushMatrix();
    glTranslated(0, 0, c->Center.h*4/512.);
    glTranslated(0, .1, .25);
    glRotated(mdRot, 1,0,0);
    mdPlus.Quad(c->Neutral->Ani);
 glPopMatrix();
}



/*=============================================================================
 * FOW
 */
static void mdDisplayFogOfWar(cellC* c)
{
 glDisable(GL_DEPTH_TEST);
 glScaled(1/512., 1/512., 4/512.);

 //
 int period = 400; //700;
 int t = c->Seed + timeGet()/100;
 float f = (t%period) / (double)period;


 // upwards
 mdData.Select(15 - c->Z, 1, 1);
 glMatrixMode(GL_TEXTURE);
 glTranslated(0, c->Y + f*8, 0);
 glMatrixMode(GL_MODELVIEW);

 glEnable(GL_BLEND);
 glColor4f(1,1,1, .6);
 void mlSquare(cellC*); mlSquare(c);


 // downwards
 mdData.Select(15 - c->Z, 1, 1);
 glMatrixMode(GL_TEXTURE);
 glTranslated(0, c->Y - f*8, 0);
 glMatrixMode(GL_MODELVIEW);

 glEnable(GL_BLEND);
 glColor4f(1,1,1, .6);
 void mlSquare(cellC*); mlSquare(c);
}





/*=============================================================================
 * SIGNALS
 */
static void mdDisplayBattle(cellC* c)
{
 bool blink;
 if(c != game->ActiveBattle) blink = false;
 else
 {
    int t = timeGet() + c->Seed;
    t /= BATTLE_BLINK_RATE;
    blink = t & 1;
 }

 glEnable(GL_BLEND);
 glDisable(GL_DEPTH_TEST);
 glColor3f(1,1,1);

 glPushMatrix();
    if(blink) glScaled(1.2, 1.2, 1.2);
    mdData.Quad(0);
 glPopMatrix();
}





static void mdFocus(cellC* c)
{
 cellC* f = self->Focus;
 if(!f) return;
 if(!f->Unit) return;
 if(c->X != f->X || c->Y != f->Y) return;

 // check that selected target
 unitC* u;
 for(u = f->Unit; u && u->Team != self->SelectedTeam; u = u->Next);
 if(u && u->Travel) return;

 float h = c->Center.h*4/512.;
 mdFocusHalo(h, c);
}





static void mdDisplayPathling(cellC* c, int a, unsigned l)
{
 glEnable(GL_BLEND);
 glDisable(GL_TEXTURE_2D);
 glPushMatrix();
    glRotated(a*45, 0, 0, -1);
    glScaled(.5, .5, 1);
    if(a&1) glScaled(1.2, 1.4, 1);

    float hstart = c->Center.h * 4. / 512.;
    float hend = c->SMAC(a&7)->Center.h * 4. / 512.;

    int length = 5;
    l %= length;
    double q = (length+length+l) / (3.*length*255);

    glBegin(GL_TRIANGLES);
	glColor4f(q*20,q*180,q*20,230/255.); glVertex3f(  0.   ,124./60., hend);
	glColor4f(q*20,q*150,q*20,190/255.); glVertex3f(+20./60,      0., hstart);
	glColor4f(q*20,q*110,q*20,180/255.); glVertex3f(-20./60,      0., hstart);
    glEnd();
 glPopMatrix();
}




static void mdDisplayPath(cellC* c, unitC* u)
{
 int w = game->Map.W;
 int t = timeGet()/250;

 char* start = u->LandOrders.Path.Status;
// if(u->IsTraveling()) start--;

 if(u->Cell->Z == mdZ)	glEnable(GL_DEPTH_TEST);
 else			glDisable(GL_DEPTH_TEST);

 for(char* p = start; p < u->LandOrders.Path.End; p++)
 {
    glLoadIdentity();
    glTranslated(c->X-w, c->Y, .1);	mdDisplayPathling(c, *p, t);
    glTranslated(w, 0, 0);		mdDisplayPathling(c, *p, t);
    glTranslated(w, 0, 0);		mdDisplayPathling(c, *p, t);

    t--;
    c = c->SMAC(*p&7);
 }
}





/*=============================================================================
 * DISPLAY LAND
 *
 * Main function.
 * Displays a w * h pixel rectangular map area centered on x, y.
 */
static void sweepTerrain(int x, int y)
{
 cellC* c = map(x, y, mdZ);
 if(gsOff() && !(mdBit & c->KnowMask)) return;

 gsPush(c);
 mdDisplayTerrain(c, c == mdSelectedCell);
}



static void sweepCityPlus(int x, int y)
{
 cellC* c = map(x, y, mdZ);
 if(!(mdBit & c->KnowMask)) return;

 if(c->City) c->City->Display(mdRot);
 else if(c->Neutral) mdDisplayNeutral(c);
 else mdDisplayPlus(c);
}





static void sweepUnitBattle(int x, int y)
{
 cellC* c = map(x, y, mdZ);
 if(!(mdBit & c->ViewMask)) return;

 mdFocus(c);

 if(c->Battle) mdDisplayBattle(c);
 else if(c->Unit) mdDisplayUnits(c);
}



static void sweepFoW(int x, int y)
{
 cellC* c = map(x, y, mdZ);

 if(mdBit & c->KnowMask) if(!(mdBit & c->ViewMask))
    mdDisplayFogOfWar(c);
}





void mdDisplay(void* selection, mapViewC& mapview)
{
 if(gsOff()) mdUpd = mdTempo();


 mdSelectedCell = selection;
 mdBit = self->Bit;
 mdRot = mapview.Rot;
 if(self->Focus)
    if(inputAlt())	mdZ = self->Focus->Mirror()->Z;
    else		mdZ = self->Focus->Z;




// mapview.LoadProjection();

 mapSweepF gsSweep[] = {
    sweepTerrain,
    0
 };

 mapSweepF normalSweep[] = {
    sweepTerrain,
    sweepFoW,
    sweepCityPlus,
    sweepUnitBattle,
    0
 };


 // Sweep 1: TERRAIN TEXTURE
 glEnable(GL_DEPTH_TEST);
 glDisable(GL_BLEND);
 mdData.BindTexture();

 glClear(GL_DEPTH_BUFFER_BIT);

 if(gsOff())	mapview.FOVSweep(normalSweep);
 else		mapview.FOVSweep(gsSweep);

 // Sweep 4: UNIT PATH
 if(gsOff() && self->LocalFocus)
 {
    unitC* u;
    for(u = self->LocalFocus->Unit; u && u->Team != self->SelectedTeam; u = u->Next);
    if(u) mdDisplayPath(self->LocalFocus, u);
 }

 // done


/* debug to test mapViewC::MapYFromMouseY()
 if(gsOff())
 {
 double y = mapview.MapYFromMouseY();
 glLoadIdentity();
 glDisable(GL_TEXTURE_2D);
 screenLineWidth(3.);
 glColor4d(1,0,0,1);
 glBegin(GL_LINES);
    glVertex2d(game->Map.W, y);
    glVertex2d(0, y);
    glVertex2d(0, y);
    glVertex2d(game->Map.W, y);
 glEnd();

 glDisable(GL_DEPTH_TEST);
 glColor4d(1,0,0,.15);
 glBegin(GL_LINES);
    glVertex2d(game->Map.W, y);
    glVertex2d(0, y);
    glVertex2d(0, y);
    glVertex2d(game->Map.W, y);
 glEnd();
 }
*/
}





/*=============================================================================
 * MINIMAP DRAW
 *
 * Draws a minimap.
 */
void mdMinimap(mapViewC* mv)
{
 glClear(GL_DEPTH_BUFFER_BIT);
 glEnable(GL_DEPTH_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA);
 mdData.BindTexture();

 int w = game->Map.W;
 int h = game->Map.H;
 int offX = (int)(mv->MapX + w/2.);

 glTranslated(-.5, -.5, 0);
 glScaled(1./w, 1./h, 1./w);
// glTranslated(.5,.5,0);

 for(int Y = 0; Y < h; Y++) for(int X = 0; X < w; X++)
 {
    glPushMatrix();
	glTranslated(X, Y, .0);
	cellC* c = map(X + offX, Y, mdZ);
	if(mdBit & c->KnowMask)
	{
	    mdDisplayTerrain(c, false);
	    sweepFoW(c->X, c->Y);
	}
    glPopMatrix();
 }


 // fov
 float tly = mv->MapYFromScreenY(+.5);
 float bly = mv->MapYFromScreenY(-.5);
 float lx = w/2 + mv->CellsInMidline/2;
 float rx = w/2 - mv->CellsInMidline/2;

 glDisable(GL_TEXTURE_2D);
 glDisable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);
 glColor4f(1,1,1,.1);
 glBegin(GL_QUADS);
    glVertex2d(lx, tly);
    glVertex2d(rx, tly);
    glVertex2d(rx, bly);
    glVertex2d(lx, bly);
 glEnd();
 glColor4f(1,1,1,.8);
 screenLineWidth(1.5);
 glBegin(GL_LINE_STRIP);
    glVertex2d(lx, tly);
    glVertex2d(rx, tly);
    glVertex2d(rx, bly);
    glVertex2d(lx, bly);
    glVertex2d(lx, tly);
 glEnd();
}





//EOF =========================================================================
