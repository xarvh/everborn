/*
 MAP_LAND.CPP		v0.20
 Map stuff.

 Copyright (c) 2005 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "map.h"
#include "data.h"
#include "gselect.h"

#include "random.h"
#include "main.h"


/*=============================================================================
 * GLOBALS
 */
static randomC map_landRnd;


// not static as it is required by other modules
dataC mdData("land.png", 16, 8);





/*=============================================================================
 * DISPLACED VERTEX
 */
void cellC::DisplacedVertex(int vertex, int angle, double d)
{
 mapVC* v;

 v = &Vertex(vertex);
 int x1 = v->dx + SMACX(vertex)*256;
 int y1 = v->dy + SMACY(vertex)*256;
 int h1 = v->h;

 if(vertex == -1) vertex = angle&7;
 else vertex = (vertex+angle+8)&7;

 v = &Vertex(vertex);
 int x2 = v->dx + SMACX(vertex)*256;
 int y2 = v->dy + SMACY(vertex)*256;
 int h2 = v->h;

 double x = d*x2 + (1-d)*x1;
 double y = d*y2 + (1-d)*y1;
 double h = d*h2 + (1-d)*h1;

 glVertex3d(x, y, h+.5);
}


void drawDisplaced(cellC* c, double d1, double d2, int a, bool cw)
{
 signed da = cw ? 1 : -1;

 if(a&1) d1*=1.4, d2*=1.4;

 glBegin(GL_TRIANGLE_FAN);
    c->VertexDisplay(-1);	// center
    c->DisplacedVertex(-1, a-da, d1);
    c->VertexDisplay(a);
    c->DisplacedVertex(a, da, d2);
 glEnd();
}



void drawRoad(cellC* c, int angle)
{
 randomC rnd(c->Seed+1);

 glColor3ub(150, 90, 20);
 drawDisplaced(c, .1+.1*rnd(), .1+.1*rnd(), angle, false);
}

void drawRiver(cellC* c, int angle)
{
 randomC rnd(c->Seed-1);

 glColor3ub(50, 20, 220);
 drawDisplaced(c, .15+.05*rnd(), .15+.05*rnd(), angle, true);
}






/*=============================================================================
 * HELPERS
 */
void cellC::VertexDisplay(unsigned k)
{
 mapVC& v = Vertex(k);
 int dx = SMACX(k);
 int dy = SMACY(k);
 int x = v.dx + dx*256;
 int y = v.dy + dy*256;

 glTexCoord2d(.495*dx, .495*dy);
 glVertex3i(x, y, v.h);
}





void mlSquare(cellC* c)
{
 glBegin(GL_TRIANGLE_FAN);
    c->VertexDisplay(8);
    for(int i = 0; i < 8; i++)
	c->VertexDisplay(i);
    c->VertexDisplay(0);
 glEnd();
}





//@@ remove this as soon as land data is rerendered
int mdRot(int q)
{
 int x = q / (int)(mdData.Rows+.1);
 int y = q % (int)(mdData.Rows+.1);
 return x + y * (int)(mdData.Cols+.1);
}




void map_landiDisplayCell(cellC* c, int sq)
{
 if(gsOn())
 {
    glDisable(GL_TEXTURE_2D);
    mlSquare(c);
 }
 else
 {

    glDisable(GL_BLEND);
    mdData.Select(mdRot(sq), 1, 1);
    if(c->Z)	glColor3f(1.,.5,.5);
    else	glColor3f(1.,1.,1.);
    mlSquare(c);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    for(int i = 0; i < 8; i++) if(c->River & (1<<i)) drawRiver(c, i);
    for(int i = 0; i < 8; i++) if(!c->RoadStatus[i]) drawRoad(c, i);
    glEnable(GL_DEPTH_TEST);
 }
}
















/*=============================================================================
 * PLUS
 */
plusC plusNone[1]	= {0, 0,0,0,0, {0,1,1,0}};

plusC plusNightshade[1]	= {"Nightshade",0,0,0,0, {35,1,1, ANI_STATIC}};

plusC plusMithril[1]	= {"Mithril",	0,2,0,0, {29,1,6, ANI_CYCLEBLINK}};
plusC plusAdamantium[1]	= {"Adamantium",0,2,0,0, { 0,1,6, ANI_CYCLEBLINK}};

plusC plusIron[1]	= {"Iron",	0,0,0,5, {28,1,1, ANI_STATIC}};
plusC plusCoal[1]	= {"Coal",	0,0,0,10,{ 6,1,1, ANI_STATIC}};

plusC plusSilver[1]	= {"Silver",	0,0,2,0, {40,1,8, ANI_CYCLEBLINK}};
plusC plusGold[1]	= {"Gold",	0,0,4,0, {20,1,8, ANI_STD}};
plusC plusGem[1]	= {"Gem",	0,0,6,0, {12,1,8, ANI_STD}};

plusC plusQuork[1]	= {"Quork",	0,2,0,0, {36,1,4, ANI_CYCLEBLINK}};
plusC plusCrysx[1]	= {"Crysx",	0,4,0,0, { 7,1,5, ANI_STD}};

plusC plusCattle[1]	= {"Wild Game",	2,0,0,0, {48,1,2, ANI_BLINK}};





/*=============================================================================
 * LAND
 */
static plusC* landPlusNone(double p)
{
 return plusNone;
}

static double __chanceBuffer;

static void chanceSet(double rnd) { __chanceBuffer = rnd/5; }
static bool chance(double p){ return (__chanceBuffer -= p) < .0;}





/*=============================================================================
 * END OF THE WORLD
 */
static void landRenderEOW(cellC* c)
{
 //17-24
 map_landiDisplayCell(c, 17);
}

landC landEoW[1] = {"End of the World",
    0, 0, 0, TERRAIN_UNCROSSABLE, 0,
    landRenderEOW, landPlusNone};





/*=============================================================================
 * PLAIN
 */
static void landRenderPLAIN(cellC* c)
{
 //1-8
 map_landiDisplayCell(c, 1);
}

static plusC* landPlusPLAIN(double p)
{
 chanceSet(p);
 if(chance(.002)) return plusCattle;
 return plusNone;
}

landC landPlain[1] = {"Plain",
	0, 0, 3, TERRAIN_LAND, 6,
	landRenderPLAIN, landPlusPLAIN};





/*=============================================================================
 * HILLS
 */
static void landRenderHILLS(cellC* c)
{
 //38-41
 map_landiDisplayCell(c, 39);
}

static plusC* landPlusHILLS(double p)
{
 chanceSet(p);
 if(chance(.004)) return plusCattle;
 if(chance(.008)) return plusIron;
 if(chance(.006)) return plusCoal;
 if(chance(.004)) return plusQuork;
 if(chance(.002)) return plusCrysx;
 return plusNone;
}

landC landHills[1] = {"Hills",
	0, 5, 1, TERRAIN_LAND, 4,
	landRenderHILLS, landPlusHILLS};





/*=============================================================================
 * SWAMP
 */
static void landRenderSWAMP(cellC* c)
{
 //29-32
 map_landiDisplayCell(c, 30);
}
static plusC* landPlusSWAMP(double p)
{
 chanceSet(p);
 if(chance(.030)) return plusNightshade;
 if(chance(.008)) return plusCattle;
 if(chance(.008)) return plusQuork;
 if(chance(.005)) return plusCrysx;
 return plusNone;
}
landC landSwamp[1] = {"Swamp",
	0, 0, 0, TERRAIN_LAND, 3,
	landRenderSWAMP, landPlusSWAMP};





/*=============================================================================
 * FOREST
 */
//@@ use a list instead
static void cone(float r, float g, float b)
{
 float a[] = {
    +1.000000, +0.000000,
    +0.923880, +0.382683,
    +0.707107, +0.707107,
    +0.382683, +0.923880,
    +0.000000, +1.000000,
    -0.382683, +0.923880,
    -0.707107, +0.707107,
    -0.923880, +0.382683,
    -1.000000, +0.000000,
    -0.923880, -0.382683,
    -0.707107, -0.707107,
    -0.382683, -0.923880,
    -0.000000, -1.000000,
    +0.382683, -0.923880,
    +0.707107, -0.707107,
    +0.923880, -0.382683,
 };

 glBegin(GL_TRIANGLE_FAN);
    glColor3f(r/3,g/3,b/3);
    glVertex3f(0,0,1);
    glColor3f(r,g,b);
    for(int k = 0; k < 16; k += 2)
	glVertex3f(a[k*2], a[k*2 +1], 0);
    glVertex2fv(a);
 glEnd();
}




void coneTree(randomC rnd, unsigned char status)
{
 int cycle = 8;

 status %= cycle*8;

 if(status >= cycle) status = 0;
 if(status >= cycle/2) status = cycle-status;

 float f = .5 * (.8 + .4*rnd());
 float a = 1 + .3 * status / (double)cycle;
 float g = .4 + .3*rnd();

 glPushMatrix();
    glRotated(360*rnd(), 0, 0, 1);
    glScaled(f, f, f);

    // branch
    glPushMatrix();
	glScaled(.4, .4, a);
	cone(.5, .3, .1);
    glPopMatrix();

    glTranslated(0, 0, a/2);
    glScaled(1.2, 1.2, .8);

    for(int t = 0; t < 3; t++)
    {
	glRotated(30*(a*f-1), 0, 1, 0);
	glScaled(.8, .8, .8);
	cone(.1, g, .1);
	g *= .8;
	glTranslated(0, 0, .8*f);
    }

 glPopMatrix();
}







static void landRenderFOREST(cellC* c)
{
 // 42 per adesso
 map_landiDisplayCell(c, 42);
 if(gsOn()) return;


 randomC rnd(c->Seed);
 int n = 6 + 6*rnd();
 for(int i = 0; i < n; i++)
 {
    float x = .5 - rnd();
    float y = .5 - rnd();

    glPushMatrix();
	glScaled(256, 256, 128);
	glTranslated(2*x, 2*y, c->Center.h*4/512.);
	glScaled(.7, .7, .5);
	coneTree(rnd, c->OverS);
    glPopMatrix();
 }
}

static plusC* landPlusFOREST(double p)
{
 chanceSet(p);
 if(chance(.005)) return plusCattle;
 if(chance(.002)) return plusNightshade;
 return plusNone;
}
landC landForest[1]= {"Forest",
	0, 5, 1, TERRAIN_FOREST, 3,
	landRenderFOREST, landPlusFOREST};





/*=============================================================================
 * DESERT
 */
static void landRenderDESERT(cellC* c)
{
 //25 29
 map_landiDisplayCell(c, 25);
}

static plusC* landPlusDESERT(double p)
{
 chanceSet(p);
 if(chance(.004)) return plusGem;
 if(chance(.002)) return plusCrysx;
 return plusNone;
}

landC landDesert[1]= {"Desert",
	5, 5, 0, TERRAIN_LAND, 4,
	landRenderDESERT, landPlusDESERT};





/*=============================================================================
 * TUNDRA
 */
static void landRenderTUNDRA(cellC* c)
{
 //37 for now
 map_landiDisplayCell(c, 37);
}
static plusC* landPlusTUNDRA(double p)
{
 chanceSet(p);
 if(chance(.005)) return plusIron;
 if(chance(.005)) return plusSilver;
 if(chance(.003)) return plusCoal;
 if(chance(.003)) return plusQuork;
 return plusNone;
}
landC landTundra[1]= {"Tundra",
	0, 0, 0, TERRAIN_LAND, 4,
	landRenderTUNDRA, landPlusTUNDRA};





/*=============================================================================
 * SEA
 */
static void landRenderSEA(cellC* c)
{
 //9-16
 map_landiDisplayCell(c, 9);


}

landC landSea[1]   = {"Sea",
	5, 0, 3, TERRAIN_SEA, 0,
	landRenderSEA,landPlusNone};


/*=============================================================================
 * SHORE
 */
static void landRenderSHORE(cellC* c)
{
 //whatever
 map_landiDisplayCell(c, 16);
}


landC landShore[1] = {"Shore",
	5, 0, 1, TERRAIN_SHORE, 0,
	landRenderSHORE,landPlusNone};


/*=============================================================================
 * MOUNTAINS
 */
static void landRenderMOUNTAINS(cellC* c)
{
 //33-36
 map_landiDisplayCell(c, 33);
}
static plusC* landPlusMOUNTAINS(double p)
{
 chanceSet(p);
 if(chance(.008)) return plusCoal;
 if(chance(.006)) return plusIron;
 if(chance(.005)) return plusSilver;
 if(chance(.004)) return plusGold;
 if(chance(.004)) return plusQuork;
 if(chance(.004)) return plusCrysx;
 if(chance(.004)) return plusMithril;
 if(chance(.003)) return plusAdamantium;

 return plusNone;
}
landC landMountains[1]={"Mountains",
	0, 8, 0, TERRAIN_MOUNTAIN, 2,
	landRenderMOUNTAINS, landPlusMOUNTAINS};





//EOF =========================================================================
