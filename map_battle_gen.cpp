/*
 MAP_BATTLE_GEN.CPP		v0.01
 Battlefield generation.

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
#include "game.h"
#include "city.h"
#include "unit.h"
#include "race.h"



/*=============================================================================
 * MOST IMPORTANT BLD
 *
 * Determines what blds are actually to be shown on the battlefield inside the
 * city.
 */
static int cityMostImportantOf(cityC* city, long blds)
{
 int best_value = 0;
 int best = -1;

 for(int k = 0; k < BLD_MAX; k++) if(blds & (1<<k))
 {
    int value = city->Race->Blds[k]->Cost;
    if(value > best_value)
    {
	best_value = value;
	best = k;
    }
 }

 return best;
}



static int cityMostImportantBld(cityC* city, int NthBest)
{
 long blds = city->VisibleBlds();

 int bld;
 for(int i = 0; i < NthBest+1; i++)
 {
    // find best
    bld = cityMostImportantOf(city, blds);
//    pf("step %d -> best %d\n", NthBest, bld);

    if(bld == -1) return -1;

    // remove bld from available
    blds &= ~(1<<bld);
 }

 return bld;
}





/*=============================================================================
 * HOUSES
 */
static int cityHouses(cityC* city, int id)
{
 int s = city->Seed;
 int pop = city->TotalPop;

 return ((s + pop + id) % 6) -1;

/* int count[4];
 int tot = 0;

 for(int i = 3; i >= 0; i--)
 {
    count[i] = s % (pop>>i);
    pop -= count[i]<<i;
    tot += count[i];
 }

 id *= tot/5;

 for(int i = 0; i < 4; i++)
    if(id < count[i]) id -= count[i];
    else return i;

 return -1;
*/
}





/*=============================================================================
 * FEATURES
 */
int battleWall(int x, int y)
{
 int n = BATTLE_H/2 + BATTLE_WALL_SIZE/2;
 int s = BATTLE_H/2 - BATTLE_WALL_SIZE/2;
 int w = BATTLE_W/2 + BATTLE_WALL_SIZE/2;
 int e = BATTLE_W/2 - BATTLE_WALL_SIZE/2;

 // horizontal walls
 if(y == n || y == s)
    if(x == BATTLE_W/2) return BCF_GATE;
    else if(e <= x && x <= w) return BCF_WALL;

 // vertical walls
 if(x == w || x == e)
    if(y == BATTLE_H/2) return BCF_GATE;
    else if(s <= y && y <= n) return BCF_WALL;

 // inner city area
 // if(s < y && y < n) if(e < x && x < w) return ...;

 // outiside city
 return BCF_NONE;
}




int battleCity(signed x, signed y)
{
 x -= BATTLE_W/2;
 y -= BATTLE_H/2;

 if(x == -2)
	 if(y ==  2) return 1;
    else if(y ==  0) return 7;
    else if(y == -2) return 6;
 if(x ==  2)
	 if(y ==  2) return 2;
    else if(y ==  0) return 4;
    else if(y == -2) return 3;
 if(x == 0)
	 if(y ==  2) return 8;
    else if(y == -2) return 5;

 return 0;
}



void battleSetFeature(battleC* b, bcC* c, int x, int y)
{
 c->Feature = 0;

 // city features
 if(cityC* city = b->Cell->City)
 {
    // wall
    if(city->Spc() & CSPC_WALL)
	if(int f = battleWall(x, y))
	{
	    c->Feature = f;
	    c->FeatureHits = 1;
	}

    if(int id = battleCity(x, y))
    {
	id = (id+city->Seed) % 8;
	if(id < 3)	// 3 most important bld
	{
	    int b = cityMostImportantBld(city, id);
	    pf("bld: %d\n", b);
	    if(b != -1)
	    {
		c->Feature = BCF_BLD;
		c->FeatureId = b;
		c->FeatureHits = 1;
	    }
	}
	else
	{
	    int b = cityHouses(city, id-3);
	    if(b >= 0)
	    {
		c->Feature = BCF_HOUSE;
		c->FeatureId = b;
		c->FeatureHits = 1;
	    }
	}
    }
 }
}





/*=============================================================================
 * HEIGHTMAP
 */
static void battleGenerateHeightmap(battleC* b)
{
 int w = BATTLE_W+1;
 int h = BATTLE_H+1;
 signed long hmA[w*h];
 signed long hmB[w*h];

 signed long* src = hmA;
 signed long* dst = hmB;

 signed maxZ = -1000;
 signed minZ = +1000;



 // clean
 for(int k = 0; k < w*h; k++) dst[k] = 0;


 // set starting nodes
 for(int k = 0; k < 9; k++)
 {
    mapVC v = b->Cell->Vertex(k);

    signed x = w*(cellC::SMACX(k)*64 + v.dx);		x /= 100;
    signed y = h*(cellC::SMACY(k)*64 + v.dy);		y /= 100;
    signed z = v.h;

    x += w/2;
    if(x < 0) x = 0;
    else if(x > w-1) x = w-1;

    y += h/2;
    if(y < 0) y = 0;
    else if(y > h-1) y = h-1;

    // set generator node
    dst[x + y*w] = z * 1024;

    // get max/min
    if(z > maxZ) maxZ = z;
    if(z < minZ) minZ = z;
 }


 // iterate
 for(int i = 0; i < 30; i++)
 {
    signed long* t = dst;
    dst = src;
    src = t;


    for(int k = 0; k < w*h; k++) dst[k] = 0;


#define DST(X, Y)		\
    if((Y) >= 0 && (Y) < h)	\
     if((X) >= 0 && (X) < w)	\
      dst[(X) + (Y)*w] += v;

    for(int y = 0; y < h; y++) for(int x = 0; x < w; x++)
    {
	signed long v = src[x + y*w] / 5;
	DST(x+0, y+0);
	DST(x+1, y+0);
	DST(x-1, y+0);
	DST(x+0, y+1);
	DST(x+0, y-1);
    }
 }



 // get heightmap max and min
 signed long maxH = -1024*1024*1024;
 signed long minH = +1024*1024*1024;
 for(int k = 0; k < w*h; k++)
 {
    signed long v = dst[k];
    if(v > maxH) maxH = v;
    if(v < minH) minH = v;
 }


 // calculate scale factors:
 // allowed interval must be proportional to the actual
 // vertexes height [-128, 128]
 double highestH = (127 * maxZ) / 128;
 double lowestH = (127 * minZ) / 128;

 double scale = (highestH-lowestH)/(double)(maxH-minH);
 double shift = lowestH - minH/scale;

 // adjust heightmaps
 for(int k = 0; k < w*h; k++)
    dst[k] = (int)(dst[k]*scale + shift);

 // write
 for(int y = 0; y < BATTLE_H; y++) for(int x = 0; x < BATTLE_W; x++)
 {
    bcC* c = b->BC(x,y);

    c->Height[0] = dst[(x+1) + (y+1)*w];
    c->Height[1] = dst[(x+1) + (y+0)*w];
    c->Height[2] = dst[(x+0) + (y+0)*w];
    c->Height[3] = dst[(x+0) + (y+1)*w];
 }
}





/*=============================================================================
 * GENERATE MAP
 */
void battleC::GenerateMap()
{
 battleGenerateHeightmap(this);


 //@@ write something //
 for(int y = 0; y < BATTLE_H; y++) for(int x = 0; x < BATTLE_W; x++)
 {
	bcC* c = BC(x,y);

	c->BaseS = Rnd(8)+1; //.Raw16();
	c->Occupied = 0;

	battleSetFeature(this, c, x, y);
 }
}





//EOF =========================================================================
