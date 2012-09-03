/*
 UNIT_MOVE.CPP		v0.04
 Pathfinder.

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

#include "unit.h"
#include "utype.h"
#include "city.h"
#include "map.h"
#include "main.h"

#include "game.h"


//@@debug
nodeC* umEND;



/*=============================================================================
 * GEOMETRY
 */
#define SF	4
#define GW	(BATTLE_W * SF)
#define GH	(BATTLE_H * SF)



// Map to Grid




/*=============================================================================
 * FLAGS
 */
#define FLAG_ME		(1<<0)	// occupied by the unit itself: movement here is always possible

#define FLAG_TROOP	(1<<1)	// occupied by ground/water unit
#define FLAG_FLYER	(1<<2)	// occupied by flying unit

#define FLAG_WALL	(1<<3)	// city wall
#define FLAG_GATE	(1<<4)	// city gate, blocks ground enemy units
#define FLAG_ROCK	(1<<5)
#define FLAG_TREE	(1<<6)
#define FLAG_CITY	(1<<7)	// slows down ground enemy units

#define FLAGS_CLEAR	FLAG_ME





/*=============================================================================
 * GRID
 */
static unsigned umCost(int x, int y, int incoming_dir);
static unsigned umHeu(int sx, int sy, int __x, int __y);
static gridC umGrid(GW, GH);




signed gx(double mx) { return (int)(mx * SF + SF/2.); }
signed gy(double my) { return (int)(my * SF + SF/2.); }

nodeC* umNode(unitC* u)
{
 return umGrid.Node(gx(u->X), gy(u->Y));
}

bcC* umBC(battleC* b, nodeC* n)
{
 return b->BC(n->X/4, n->Y/4);
}





static int umSize(unitC* u)
{
 return 4; //@@ actually calculate the value
}





static void umSet(nodeC* n, int s, long f)
{
 int sx = n->X - s/2;
 int sy = n->Y - s/2;

 for(int y = 0; y < s; y++) for(int x = 0; x < s; x++)
    if(f) umGrid.Node(sx+x, sy+y)->Flags |= f;
    else  umGrid.Node(sx+x, sy+y)->Flags &= ~FLAGS_CLEAR;
}



static long umChk(nodeC* n, int s)
{
 int sx = n->X - s/2;
 int sy = n->Y - s/2;

 long f, flags = 0;

 for(int y = 0; y < s; y++) for(int x = 0; x < s; x++)
 {
    f = umGrid.Node(sx+x, sy+y)->Flags;
    if( ! (f & FLAG_ME) ) flags |= f;
 }

 return flags;
}





/*=============================================================================
 * INITIALIZE GRID
 */
static void umSetUnitFlags(unitC* u)
{
 long flags;
    if(1) //@@ walking
	flags = FLAG_TROOP;
    else if(1) //@@ flying
	flags = FLAG_FLYER;

 umSet(umNode(u), umSize(u), flags);
}



void umSetGrid(battleC* b)
{
 // reset grid, starting with battle features
 for(int y = 0; y < GH; y++) for(int x = 0; x < GW; x++)
 {
    nodeC* n = umGrid.Node(x, y);

    //@@add water, add ground and maybe even swamp
    long f = 0;
    switch(umBC(b, n)->Feature) {
	case BCF_WALL:	f = FLAG_WALL; break;
	case BCF_GATE:	f = FLAG_GATE; break;
	case BCF_BLD:	f = FLAG_CITY; break;
	case BCF_HOUSE:	f = FLAG_CITY; break;
	case BCF_TOWER:	f = FLAG_WALL; break;
//@@	case BCF_TREES:	f = FLAG_TREE; break;
//@@	case BCF_ROCKS:	f = FLAG_ROCK; break;
	default: break;
    }

    n->Flags = f;
 }



 // units
 for(unitC* u = b->Cell->Unit; u; u = u->Next) if(u->IsAlive())
    umSetUnitFlags(u);

 // debug
// umSet(GW/2., GH/2., 2, FLAG_DEBUG);
// umSet(GridX(4), GridY(4), 4, FLAG_WALL);


// umGrid.Node((int)MGX(5), (int)MGY(5))->Flags = FLAG_DEBUG;
// int y = H/2;
// for(int x = 0; x < W; x++)
//    umSet(x, y, 4, FLAG_TROOP);
}





/*=============================================================================
 * GLOBALS
 */
static long umBlock;	// what can stop this unit?
static long umSlow;	// what can slow down this unit?

static signed umEX;
static signed umEY;
static unitC* umU;





/*=============================================================================
 * COST, HEU
 *
 * Must be coherent with umBlock and umSlow fields, so i put the function here.
 */
static void umSetBlockAndSlow()
{
 umBlock = 0;
 umSlow = 0;

 if(1) //@@!flying || !incorporeal)
 {
    umBlock |= FLAG_TROOP + FLAG_WALL;

    if(umU->Cell->City && umU->Cell->City->Owner != umU->Owner)
    {
	umBlock |= FLAG_GATE;
    	umSlow |= FLAG_CITY;
    }

    if(1) //@@ !mountaneer)
	umSlow |= FLAG_ROCK;

    if(1) //@@ !forester)
	umSlow |= FLAG_TREE;
 }
}





static unsigned umCost(int x, int y, int incoming_dir)
{
 long f = umChk(umGrid.Node(x, y), umSize(umU));
 if(f & umBlock) return 0;

 if(f & umSlow)	f = 2;
 else		f = 1;

// if(incoming_dir & 1)	return f*142;
// else			return f*100;
 if(incoming_dir & 1)	return f*71;
 else			return f*50;
}



static signed ABS(signed q) { return q >0? q:-q;}

static unsigned umHeu(int sx, int sy, int ex, int ey)
{
 signed dw = ABS(sx - ex);
 signed dh = ABS(sy - ey);

 if(dw > dh)	return (int)(100*dw + 42*dh);
 else		return (int)(100*dh + 42*dw);
}





/*=============================================================================
 * MOVE LOW
 *
 * Performs the actual movement.
 */
static void umMoveLow(unitC* u, int dir)
{
 double step = u->Type->Speed / 30.; //@@ this should go in a function

 // check for slowed down movement
 if(umSlow & umChk(umNode(u), umSize(umU))) step /= 2;





 //??? clear unit position
 // not sure this is actually good or bad
/* int s = umSize(umU);
 nodeC* n = umNode(u);
 int sx = n->X - s/2;
 int sy = n->Y - s/2;
 for(int y = 0; y < s; y++) for(int x = 0; x < s; x++)
    umGrid.Node(sx+x, sy+y)->Flags = 0;
*/



 // update unit position and angle
 u->X = step*mapSMACX(dir) + u->X;
 u->Y = step*mapSMACY(dir) + u->Y;
 u->A = (dir*360)/8;

 // mark new position as occupied
 umSetUnitFlags(u);
}



/*=============================================================================
 * MOVE TO
 *
 * Main function.
 */
bool umMoveTo(unitC* u, double x, double y, double distance)
{
 // globals
 umU = u;

 // flags
 umSetBlockAndSlow();

 // parameters
 int sx = umNode(u)->X;
 int sy = umNode(u)->Y;
 int ex = gx(x);
 int ey = gy(y);
 distance *= 100 * SF;		// distance is stored in hundredths
 umGrid.MaxClosed = 200;


 // set starting position
 umSet(umNode(u), umSize(umU), FLAG_ME);

 // pathfind
 umGrid.Pathfind(sx, sy, ex, ey, distance, umCost, umHeu);

 //@@debug
 umEND = umGrid.Node(ex, ey);

 // restore starting position
 umSet(umNode(u), umSize(umU), 0);

 // move unit
 if(umGrid.PathLength < 1) return false;

 umMoveLow(u, umGrid.Path[0]&7);
 return true;
}





//=============================================================================

#include <GL/gl.h>
#include "fnt.h"

void umDrawPathGrid(bcC* c, int mx, int my)
{
 glDisable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);

 glColor4f(1, 0, 0, .3);

 double s = -(.5/SF) * (SF-1);
 double e = -s;

 for(double y = s; y <= e; y += 1./SF) for(double x = s; x <= e; x += 1./SF)
 {
    glDisable(GL_TEXTURE_2D);
    nodeC* n = umGrid.Node(gx(mx + x), gy(my + y));


    if(umEND == n)			glColor4f(1,1,1,.9);
    else if(n->Flags & FLAG_TROOP)	glColor4f(1,0,0,.5);
    else if(n->Flags & umBlock)		glColor4f(0,0,0,.7);
    else if(n->Closed)			glColor4f(0,0,1,.3);
    else if(n->Opened)			glColor4f(0,.3,0,.6);
    else continue;

    double l = .5/SF;
    glBegin(GL_QUADS);
	glVertex2f(x-l,y-l);
	glVertex2f(x+l,y-l);
	glVertex2f(x+l,y+l);
	glVertex2f(x-l,y+l);
    glEnd();



    glPushMatrix();
	glTranslated(x, y+l, 0);
	glScaled(1,-1,1);
	fntS(l);
	glColor4f(1,0,1,1);
	fntC("%d*%d", n->Heu/100, n->Cost/100);
    glPopMatrix();



    if(n->DBBest != -1) {
    glPushMatrix();
	glTranslated(x,y,0);
	glRotated(n->ParentDirection*45, 0, 0,-1);
	glBegin(GL_LINES);
	    glColor3f(1,0,0);
	    glVertex2f(0, 0);
	    glVertex2f(0, -l);
	glEnd();
    glPopMatrix();
    }


 }
}





//EOF =========================================================================
