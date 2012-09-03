/*
 mapBattleLayArmy.cpp	v0.01
 Sets the army formation when units enter the battle.

 Copyright (c) 2004-2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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

#include <math.h>

#include "main.h"

#include "map.h"
#include "unit.h"
#include "utype.h"





/*
 Known variables:
    F	number of Frontline units
    M	number of Midline units
    B	number of Back units
    S	number of Side units
    C	maximum allowed army width

 Unknown variables to be determined:
    w	army width without side units
    f	number of rows to split Frontline units into
    m	number of rows to split Midline units into
    b	number of rows to split Back units into
    r	half number of columns to split Side units into

	  <----- w ------>
 *-------*----------------*-------*
 |.......|Frontline f rows|.......|
 | Side	.|----------------| Side .|
 |<- r ->| Midline m rows |<- r ->|
 |.......|----------------|.......|
 |.......|.. Back b rows .|.......|
 *-------*----------------*-------*
 <-------------- C --------------->

 The following inequalities must yeld:
    fw >= F
    mw >= M
    bw >= B
    2r(f+m+b) >= S
    2r+w <= C

 To stretch the army on the available width:
 
	w = C * (F+M+B) / (F+M+B+S)	(round down)

	r = (C-w)/2			(round up)

	f = F/w				(round up)
	m = M/w				(round up)
	b = B/w				(round up)


 All rows of each line are filled but the last one.
 If the last row has even units, the mid position is skipped.


 Example:
    F=9, M=3, B=7, S=5, C=7

 S F F F F F S
 S F F   F F S
 S   M M M
   B B B B B
     B   B


 Units at the center will be laid in a square spiral around the map center,
 starting with B units, then M, then F, then S, skipping cells occupied by
 walls, doors, buildings.
*/

// Available width: must be odd
#define C	15






/*=============================================================================
 * UNIT HELPERS
 */
bool unitC::IsBattleEnterAngle(int angle)
{
 //debug
 // if(angle == 0) angle = -1;

 return X == -1 && LandOrders.Path.PrevDirection() == angle;
}



bool unitC::IsFront()
{
 switch(Type->Role->code) {
    case ROLE_PLAYERMAGE:
    case ROLE_FRONTLINE:
    case ROLE_ELITE:
    case ROLE_TRIREME: return true;
    default: return false;
 }
}

bool unitC::IsMid()
{
 switch(Type->Role->code) {
    case ROLE_SPEARMEN:
    case ROLE_SWORDSMEN:
    case ROLE_BOWMEN:
    case ROLE_CLERIC: return true;
    default: return false;
 }
}

bool unitC::IsBack()
{
 switch(Type->Role->code) {
    case ROLE_MAGICIANS:
    case ROLE_ENGINEERS:
    case ROLE_SETTLERS:
    case ROLE_CATAPULT: return true;
    default: return false;
 }
}

bool unitC::IsSide()
{
 switch(Type->Role->code) {
    case ROLE_CAVALRY:
    case ROLE_FLYER: return true;
    default: return false;
 }
}



void unitC::BattleLay(int x, int y, int angle, int total_rows)
{
 // this must be reset whenever the unit is relaid
 Ammo = Type->Ammo;

 // also, reset this
 Action = actionC::Get("wait");


 if(angle == -1) // unit at center
 {
    // x,y origin is at map center
    X = x + BATTLE_W/2;
    Y = y + BATTLE_H/2;

    // unit faces outside
    A = atan2(x, y)*(180/M_PI);

    // unit is waiting idle
    Cell->Battle->BC(X, Y)->Occupied++;
    if(Owner)	BattleOrders.Set(&uobHOLD, false);
    else	BattleOrders.Set(&uobHOLD, false);//BattleOrders.Set(&uobENGAGE, false);
 }
 else		// unit entering from edge
 {
    // x,y origin refers to the middle of the bottom edge
    // units start at x,0 and are ordered to reach x,y
    // also move origin to the center, to ease rotation around the center
    float pX = x;
    float pY = -BATTLE_H/2;

    if(angle&1) pY /= 1.5;

    int yoff;
    if(angle&1) yoff = 2;
    else	yoff = 0;

    // rotate
    X =  pX * Cell->SMACY(angle) + pY * Cell->SMACX(angle);
    Y = -pX * Cell->SMACX(angle) + pY * Cell->SMACY(angle);

    // transform to actual battlefield coordinates
    X += BATTLE_W/2;
    Y += BATTLE_H/2;
    A = angle * 360/8;

    int delay = 1 + (1 + total_rows - y) * 16; //8

    BattleOrders.Set(&uobHOLD, false, delay, 0);
    BattleOrders.Set(&uobWAVE, true, angle, y+yoff);
    if(Owner)	BattleOrders.Set(&uobHOLD, true);
    else	BattleOrders.Set(&uobHOLD, true); //BattleOrders.Set(&uobENGAGE, true);
 }
}





/*=============================================================================
 * LAY AT ANGLE
 */
static int divideRoundingUp(int n, int q) { return (n + q - 1) / q; }



void battleC::LayUnitsAtAngle(int angle)
{
 int F = 0;
 int M = 0;
 int B = 0;
 int S = 0;

 // count units
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->IsBattleEnterAngle(angle))
 {
    if(u->IsFront())	F++;
    if(u->IsMid())	M++;
    if(u->IsBack())	B++;
    if(u->IsSide())	S++;
    // no other cases allowed!
 }

 // no units to lay?
 if(F+M+B+S == 0) return;

 // calculate main lines width
 int w = ((F+M+B) * C) / (F+M+B+S);

 // force w to be odd
 w |= 1;

 // split lines in multiple rows if they exceed w
 int f = divideRoundingUp(F, w);
 int m = divideRoundingUp(M, w);
 int b = divideRoundingUp(B, w);


 // lay!
 int fp = 0;
 int mp = 0;
 int bp = 0;
 int sp = 0;

 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->IsBattleEnterAngle(angle))
    if(u->IsSide())
    {
	int uxr = C - w;		// side units per row
	    if(uxr & 1) uxr++;		// uxr must be even

	int y = 1 + f+m+b - (sp/uxr);	// sp/uxr is row

	int x = (sp%uxr)/2 + w/2 +1;		// sp%uxr is col
	if(sp & 1) x = -x;

	// adjust coordinates, set initial position and give initial orders
	u->BattleLay(x, y, angle, f+m+b);
	sp++;
    }
    else
    {
	int T, t;
        int p;
	int yoff;

        if(u->IsFront()){ p = fp++; T = F; t = f; yoff = f+m+b; }
        if(u->IsMid())	{ p = mp++; T = M; t = m; yoff =   m+b; }
        if(u->IsBack())	{ p = bp++; T = B; t = b; yoff =     b; }
	// no other cases allowed!


	int y = 1 + yoff - p/w; // enter+escape+empty + off - pos


	int x = -w/2 + p%w;


	if(T%w)			// is last row not completely filled?
	    if(t*w - (p+1) < w)	// is this last row of the line?
	    {
	    	// center the units
		x += (w - T%w)/2;

		// if remaining units are even and they're at center,
		// skip one position:
		if( !((T%w)&1) && x >= 0) x++;
	    }

	// adjust coordinates, set initial position and give initial orders
	u->BattleLay(x, y, angle, f+m+b);
    }
}



/*=============================================================================
 * LAY UNITS AT CENTER
 *
 * Do not check angle: just take all uninitialized units.
 */
static void next(int* x, int *y)
{
 static int r, i, d;

 if(!*x && !*y) r = i = d = 0;	// reset if we're beginning from center

 // make a square spiral
 if(i == r) d++;
 if(i == r+r) { i=0; r++; d++; }
 i++;

 switch(d&3) {
    case 0: *y += 1; break;	// up
    case 1: *x += 1; break;	// right
    case 2: *y -= 1; break;	// down
    case 3: *x -= 1; break;	// left
 }
}



typedef  bool (*uptrT)(unitC*);
static bool isf(unitC* u) { return u->IsFront(); }
static bool ism(unitC* u) { return u->IsMid(); }
static bool isb(unitC* u) { return u->IsBack(); }
static bool iss(unitC* u) { return u->IsSide(); }

bool occupied(battleC* battle, int x, int y)
{
 bcC* b = battle->BC(x+BATTLE_W/2, y+BATTLE_H/2);

 if(b->Occupied) return true;
 if(b->Feature) return true;
 return false;
}

void battleC::LayUnitsAtCenter()
{
 int x = 0;
 int y = 0;

 uptrT isLine[4] = { isb, ism, isf, iss };
 for(int l = 0; l < 4; l++)
    for(unitC* u = Cell->Unit; u; u = u->Next) if(u->IsBattleEnterAngle(-1))
	if(isLine[l](u))
	{
	    // find first available cell
	    while(occupied(this, x, y)) next(&x, &y);
	
	    // lay unit
	    u->BattleLay(x, y, -1, 0);

	    // select next position
	    next(&x, &y);
	}
}





/*=============================================================================
 * LAY UNITS
 */
void battleC::LayUnits()
{
 for(int a = 0; a < 8; a++)
    LayUnitsAtAngle(a);
 LayUnitsAtCenter();
}



//EOF =========================================================================
