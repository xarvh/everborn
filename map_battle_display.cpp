/*
 MAP_DISPLAY_BATTLE.CPP		v0.05
 display_battle() plus helper functions.

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
#include "city.h"

#include "fnt.h"
#include "main.h"
#include "data.h"
#include "game.h"
#include "map.h"
#include "gselect.h"
#include "particles.h"

#include "map_view.h"


#define ANIMATION_TIME	200




static dataC dbData("land.png", 16, 8);

static tempoC dbTempo(ANIMATION_TIME);

static envC envDDBattle("debug", false);	//Debug Display Battle
static bool dbDD;				//used to speed things up
static bool dbEscape[8];
static float dbRot;



/*=============================================================================
 * DRAW SQUARE
 *
static int dbDrawSquare(double sX, double sY, double eX, double eY)
{
 sX += (eX - sX) / 128.;	// remove .5 pixel from texture
 eX -= (eX - sX) / 128.;	// to avoid imprecise texture selection
 sY += (eY - sY) / 128.;
 eY -= (eY - sY) / 128.;

 glBegin(GL_QUADS);
    glTexCoord2d(sX, eY);	glVertex2f(-.5,+.5);
    glTexCoord2d(eX, eY);	glVertex2f(+.5,+.5);
    glTexCoord2d(eX, sY);	glVertex2f(+.5,-.5);
    glTexCoord2d(sX, sY);	glVertex2f(-.5,-.5);
 glEnd();
}
*/




/*=============================================================================
 * DISPLAY CELL
 */
static int dbDisplayCell(bcC* c)
{
 glBegin(GL_TRIANGLE_FAN);
    glTexCoord2d(-.49, +.49);	glVertex3f(-.5,-.5, c->Height[2]/64.);
    glTexCoord2d(-.49, -.49);	glVertex3f(-.5,+.5, c->Height[3]/64.);
    glTexCoord2d(+.49, -.49);	glVertex3f(+.5,+.5, c->Height[0]/64.);
    glTexCoord2d(+.49, +.49);	glVertex3f(+.5,-.5, c->Height[1]/64.);
 glEnd();
}





static void dbDisplayCellSweep1(bcC* c, int x, int y)
{
 if(gsOn()) glDisable(GL_TEXTURE_2D);
 else
 {
    glEnable(GL_TEXTURE_2D);

    bool isb = battleIsBorder(x, y);
    bool isoob = battleIsOOBorder(x, y);

    if(isoob)		glColor3d(.2, .2, .2);
    else if(isb)	glColor3d(.5, .5, .5);

    else	if(c->Feature == BCF_WALL) glColor3d(.3,.3,.3);
    else	if(c->Feature == BCF_GATE) glColor3d(.7,.5,.1);
    else	glColor3d(1., 1., 1.);

    dbData.Select(c->BaseS*16, 1, 1);
 }


 dbDisplayCell(c);
}





/*=============================================================================
 * SWEEP 2
 */
static int dbDisplayCellSweep2(battleC* b, int x, int y)
{
 bcC* c = b->BC(x, y);

 if(c->Feature == BCF_HOUSE) if(cityC* city = b->Cell->City)
 {
    city->Race->Land.BindTexture();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glColor3f(1,1,1);

    glPushMatrix();
        float f = .2 * (c->FeatureId+1);
	glScaled(f, f, f);
	city->Race->Land.Quad(0);
    glPopMatrix();
 }


 if(c->Feature == BCF_BLD) if(cityC* city = b->Cell->City)
 {
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();
	glScaled(1,-1,1);
	city_displayBld(city, c->FeatureId);
    glPopMatrix();
 }


 int e = battleEscape(x, y);
 if(e > -1 && dbEscape[e])
 {
    float s = sin(b->Time/4. + (x+y)/4.);
    float a = .2 + .1*(1+s);
    glPushMatrix();
	glScaled(1./80, 1/80., 1/80.);
	glRotated(e*45, 0, 0, -1);
	glTranslated(0, 30*s, .1);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	if(e&1)	glColor4f(.9,.0,.3,a);
	else	glColor4f(.3,.0,.9,a);
	glBegin(GL_TRIANGLE_FAN);
	    glVertex3f(  0,  30, .1);
	    glVertex3f( 22, -30, .1);
	    glVertex3f(  0, -10, .1);
	    glVertex3f(-22, -30, .1);
	glEnd();
    glPopMatrix();
 }

 if(dbDD) umDrawPathGrid(c, x, y);
}





/*=============================================================================
 * SELECTION
 */
static void drawSelectionCircle(unitC* u)
{
 int points = 30;

 glDisable(GL_TEXTURE_2D);
 if(u->Owner == self)	glColor3d(0,1,0);
 else			glColor3d(1,0,0);

 screenLineWidth(2);
 glPushMatrix();
    glScaled(.5, .5, .5);
    glBegin(GL_LINE_LOOP);
    double step = (2 * M_PI) / points;
    for(double x = .0; x < 2*M_PI; x += step)
	glVertex3d( cos(x), sin(x), .1);
    glEnd();
 glPopMatrix();
}



/*=============================================================================
 * DISPLAY BATTLE
 *
 * Main function.
 */
static battleC* battle;

static void sweep1(int x, int y)
{
 dbData.BindTexture();

 bcC* c = battle->BC(x, y);
 gsPush(c);
 dbDisplayCellSweep1(c, x, y);
}

static void sweep2(int x, int y)
{
 dbDisplayCellSweep2(battle, x, y);
}




bool inBattlefield(unitC* u)
{
 if(u->X < 1) return false;
 if(u->X > BATTLE_W-1) return false;
 if(u->Y < 1) return false;
 if(u->Y > BATTLE_H-1) return false;
 return true;
}



void dbDisplay(cellC* cell, mapViewC& mapview)
{
 // set some globals...
 battle = cell->Battle;
 dbDD = envIsTrue("debug");	// global
 dbRot = mapview.Rot;

 bool upd = dbTempo();

 for(int i = 0; i < 8; i++)
    dbEscape[i] = !cell->SMAC(i)->CheckBattle(self->Bit);



 // terrain
 mapSweepF normalSweep[] = {
    sweep1,
    sweep2,
    0
 };

 mapSweepF gsSweep[] = {
    sweep1,
    0
 };

 glEnable(GL_DEPTH_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_ALPHA);

 if(gsOff())	mapview.FOVSweep(normalSweep);
 else		mapview.FOVSweep(gsSweep);



 // units
 for(unitC* u = cell->Unit; u; u = u->Next) //if(inBattlefield(u))
 {
    glLoadIdentity();
    glTranslated(u->X, u->Y, .0);

    if(gsOff() && u->IsAlive())
    	if(u->IsSelectedBy(self)) drawSelectionCircle(u);

    glScaled(1, -1, 1);
    glRotated(90, 1, 0, 0);
    glRotated(u->A, 0, 1, 0);
    gsPush(u);
    duStack(u);
 }


 // health bars
 if(gsOff()) for(unitC* u = cell->Unit; u; u = u->Next) //if(inBattlefield(u))
    if(u->IsAlive())
    {
	glLoadIdentity();
	glTranslated(u->X, u->Y, .0);

	glRotated(dbRot+180, 1, 0, 0);
	glScaled(.5,.5,.5);
	duHealth(u);
    }




 // particles
 if(gsOff()) for(particleC* p = battle->ParticlesHook; p; p = p->Next)
 {
    glLoadIdentity();
    glTranslated(p->X, p->Y, p->Z);
    p->Display();
 }


 /* debug to test mapViewC::MapYFromMouseY()
 if(gsOff())
 {
 double y = mapview.MapYFromMouseY();
 glLoadIdentity();
 glDisable(GL_TEXTURE_2D);
 screenLineWidth(3.);
 glColor4d(1,0,0,.8);
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





//EOF =========================================================================
