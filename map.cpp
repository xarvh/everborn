/*
 MAP.CPP		v0.30
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
#include "map.h"
#include "city.h"





/*=============================================================================
 * MAP
 */
mapC::mapC()	{ C = 0; }
mapC::~mapC()	{ Destroy(); }



cellC* mapC::operator()(signed x, signed y, char z)
{
 while(x < 0) x += W;
 while(y < 0) y += H;
 return C + (x % W) + ((y % H) + (z&1)*H) * W;
}



void mapC::ClearPaths()
{
 for(cellC* c = C; c < Cend; c++) c->Path = 0;
}



void mapC::Destroy()
{
 if(C) delete[] C;
 C = 0;
}



void mapC::Allocate(unsigned w, unsigned h)
{
 // control size
 W = w < 1024 ? w : 1024;
 H = h < 1024 ? h : 1024;

 // Allocate
 if(C) Destroy();
 C = new cellC[W*H*2];
 Cend = C + W*H*2;
 cellC::Map = this;

 for(int z = 0; z < 2; z++) for(int y = 0; y < H; y++) for(int x = 0; x < W; x++)
 {
    cellC* c = this->operator()(x, y, z);
    c->X = x;
    c->Y = y;
    c->Z = z;

    c->BaseS = 0;
    c->Path = 0;
 }
}





/*=============================================================================
 * CELL
 */
mapC* cellC::Map = 0;

cellC::cellC()
{
 City = 0;
 Unit = 0;
 Battle = 0;
 Neutral = 0;

 KnowMask = ViewMask = 0;
 River = Flags = 0;

 for(int d = 0; d < 8; d++) RoadStatus[d] = 100;

}

cellC::~cellC()
{
 if(City) delete City;
 if(Battle) delete Battle;
}





/*=============================================================================
 * HAS ROAD
 */
bool cellC::HasRoad(int angle)
{
 if(RoadStatus[angle&7]) return false;
 if(SMAC(angle&7)->RoadStatus[(angle+4)&7]) return false;
 return true;
}





/*=============================================================================
 * Vertex
 * Retrives one of the nine cell vertexes
 *
 *      7..|  0..|  1
 *      ...|  ...|
 *      o--o  o--o
 *
 *      6..o  X..o  2
 *      ...|  ...|
 *      o--o  o--o
 *
 *      5     4     3
 *
 * Each cellC stores only vertexes at East, SoutEast and South,
 * retriving the other vertexes from nearby cells using SMAC.
 * In the figure, the number are the SMAC code of the
 * neighbouring cells: only SMAC(0), SMAC(6) and SMAC(7) are
 * needed.
 */
mapVC& cellC::Vertex(unsigned k)
{
 switch(k) {
    case 0: return SMAC(0)->South;
    case 1: return SMAC(0)->SouthEast;
    case 2: return East;
    case 3: return SouthEast;
    case 4: return South;
    case 5: return SMAC(6)->SouthEast;
    case 6: return SMAC(6)->East;
    case 7: return SMAC(7)->SouthEast;
    default: return Center;
 }
}





/*=============================================================================
 * SMAC: SMall Around City
 */
#define SMAC_SIZE 9

static signed cellSmacX[SMAC_SIZE] = {  0,  1,  1,  1,  0, -1, -1, -1,  0};
static signed cellSmacY[SMAC_SIZE] = {  1,  1,  0, -1, -1, -1,  0,  1,  0};

cellC* cellC::SMAC(unsigned k)
{
 if(k > SMAC_SIZE-1) return 0; // use: for(k = 0; s = c->SMAC(k); k++)
 return (*Map)(X + cellSmacX[k], Y + cellSmacY[k], Z);
}

signed cellC::SMACX(unsigned k) { return cellSmacX[k]; }
signed cellC::SMACY(unsigned k) { return cellSmacY[k]; }

signed mapSMACX(unsigned k) { return cellSmacX[k]; }
signed mapSMACY(unsigned k) { return cellSmacY[k]; }





/*=============================================================================
 * BACI: Big Around CIty
 */
#define BACI_SIZE 12

static signed cellBaciX[BACI_SIZE] = { -1,  0,  1, -2, -2, -2,  2,  2,  2, -1,  0, 1};
static signed cellBaciY[BACI_SIZE] = { -2, -2, -2, -1,  0,  1, -1,  0,  1,  2,  2, 2};

cellC* cellC::BACI(unsigned k)
{
 if(k > BACI_SIZE) return 0; // use: for(k = 0; s = c->BACI(k); k += x)
 return (*Map)(X + cellBaciX[k], Y + cellBaciY[k], Z);
}





/*=============================================================================
 * HUGS: HUGe around Sity
 */
#define HUGS_SIZE	24

static signed cellHugsX[HUGS_SIZE*2] = {
/*X*/ 0,+1,+2, +2, +3,+3,+3,+3,+3, +2, +2,+1, 0 -1,-2, -2, -3,-3,-3,-3,-3, -2, -2,-1,
/*Y*/+3,+3,+3, +2, +2,+1, 0,-1,-2, -2, -3,-3,-3,-3,-3, -2, -2,-1, 0,+1,+2, +2, +3,+3,
};

static signed* cellHugsY = cellHugsX + HUGS_SIZE;

cellC* cellC::HUGS(unsigned k)
{
 if(k > HUGS_SIZE) return 0; // use: for(k = 0; s = c->HUGS(k); k += x)
 return (*Map)(X + cellHugsX[k], Y + cellHugsY[k], Z);
}



/*=============================================================================
 * MIRROR
 */
cellC* cellC::Mirror()
{
 return (*Map)(X, Y, (Z+1)&1);
}





/*=============================================================================
 * CELL FRAME HANDLERS
 */
unsigned landAniC::Init(unsigned alt, unsigned s)
{
 alt %= Alt;	// clamp to actual alternatives count
 s %= Fpa;	// clamp to actual frame

 return Id + alt*Fpa + s;
}


unsigned landAniC::NextFrame(unsigned s)
{
 static randomC rnd;

 int alt_s;
 int pos;

 switch(Mode) {
	case ANI_STD:
		alt_s = ((s - Id)/Fpa)*Fpa;
		pos = s - Id - alt_s;
		return Id + alt_s + (pos+1)%Fpa;

	case ANI_RANDOM:
		alt_s = ((s - Id)/Fpa)*Fpa;
		return Id + alt_s + rnd(Fpa);

	case ANI_BLINK:
		alt_s = ((s-Id)/Fpa)*Fpa;
		pos = s - Id - alt_s;
		s = Id + alt_s;
		if(pos) s += rnd(8) ? 1 : 0;
		else s += rnd(80) ? 0 : 1;
		return s;

	case ANI_CYCLEBLINK:
		alt_s = ((s-Id)/Fpa)*Fpa;
		pos = s - Id - alt_s;
		s = Id + alt_s;
		if(pos) s += (pos+1)%Fpa;
		else s += rnd(20) ? 0 : 1;
		return s;

	case ANI_STATIC:
	default: return s;
 }
}





//EOF =========================================================================
