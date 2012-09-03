/*
 MAP_GENERATE.CPP	v0.03
 Map random generator.

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
*/
#include "map.h"
#include "main.h"
#include "spell.h"
#include "random.h"


// moho
#define DENSITY				.7	//.6
#define ITERATIONS			12	//10

// terra
#define UNDER_LAYER_HEIGHT		.20	//.03	//.01
#define LOWER_LAYER_HEIGHT		.35	//.40	//.07
#define MEDIUM_LAYER_HEIGHT		.52	//.21	//.15

#define BARREN_ITERATIONS		1
#define FOREST_ITERATIONS		0	//5
#define FOREST_PROBABILITY		.50
#define SWAMP_PROBABILITY		.06
#define BARREN_PROBABILITY_AT_ONE	.35
#define BARREN_PROBABILITY_AT_HALF	.01





/*=============================================================================
 * LOCAL
 */
static char mgZ;	// generating Arcanus or Myrror?
static cellC* mgStart;
static cellC* mgEnd;





/*=============================================================================
 * OLD COLOR LEVELS
 * One day may be useful?

	if(c <= .45)	c = (c-.00)*10./4.5, r = 0, g = 0, b = 111 + 128*c;//, v->h = 0.;
 else	if(c <= .7)	c = (c-.45)*10./2.5, r = 0, g = 250 - 90*c, b = 0;
 else	if(c <= .95)	c = (c-.70)*10./2.5, r = 140-40*c, g = 99-30*c, b = 0;
 else	{
			if(c >= 1) c = 255./256;
 			c = (c-.95)*10./0.5, r=210+25*c, g=210+25*c, b=210+25*c;
	}

 v->color[0] = (int)r;
 v->color[1] = (int)g;
 v->color[2] = (int)b;
*/



/*=============================================================================
 * MOHO CLASS
 * Just for internal use.
 */
class mohoC { public:
    unsigned W;
    unsigned H;
    randomC Rnd;

    long* HM;

    signed char* Dx;
    signed char* Dy;

    long Min;
    long Max;

    // ========================================================================
    mohoC(unsigned w, unsigned h, unsigned seed)
    {
     W = w;
     H = h;
     Rnd.Seed = seed;
     Max = 0;
     Min = 1<<24;
     HM = new long[W*H];
     for(int i = 0; i < W*H; i++) HM[i] = 0;
    }

    ~mohoC()
    {
     delete[] HM;
    }


    // HELPERS ================================================================
    long Id(signed x, signed y) { return ((x+W)%W) + ((y+H)%H)*W; }
    long* He(signed x, signed y) { return HM + Id(x,y);}



    void Scale(mapVC* v, long id)
    {
     double h = (HM[id] - Min)/(double)(Max - Min);

     v->h = (int)(h*h*255);

     v->dx = Dx[id];
     v->dy = Dy[id];
    }



    //
    void CopyMap(mapC* map);
    void Iterate();
    void SpawnNode();
};




/*
    mohoC::mohoC(unsigned w, unsigned h, unsigned seed)
    {
     W = w;
     H = h;
     Rnd.Seed = seed;
     Max = 0;
     Min = 1<<24;
     HM = new long[W*H];
    }

    mohoC::~mohoC()
    {
     delete[] HM;
    }


    // HELPERS ================================================================
    long mohoC::Id(signed x, signed y) { return ((x+W)%W) + ((y+H)%H)*W; }
    long* mohoC::He(signed x, signed y){ return HM + Id(x,y);}



    void mohoC::Scale(mapVC* v, long id)
    {
     double h = (HM[id] - Min)/(double)(Max - Min);

     v->h = (int)(h*h*255);

     v->dx = Dx[id];
     v->dy = Dy[id];
    }
*/











/*=============================================================================
 * COPY MAP
 */
void mohoC::CopyMap(mapC* map)
{
 // Measure absolute levels
 for(int k = 0; k < H*W; k++)
 {
    if(HM[k] > Max) Max = HM[k];
    if(HM[k] < Min) Min = HM[k];    
 }

 // Set End Of the World
 for(int x = 0; x < W; x++)
    for(int e = 0; e < 6; e++)
    {
	long maxEOW = Min + e*((Max - Min)/8);
	long* h;

	h = He(x,     e);	if(*h > maxEOW)	*h = maxEOW;
	h = He(x, H-e-1);	if(*h > maxEOW)	*h = maxEOW;
    }

 signed char dx[W*H]; 
 signed char dy[W*H];
 for(int k = 0; k < W*H; k++)
    dx[k] = Rnd(101) - 51,
    dy[k] = Rnd(101) - 51;
 Dx = dx;
 Dy = dy;

 for(int y = 0; y < map->H; y++) for(int x = 0; x < map->W; x++)
 {
    cellC* c = (*map)(x, y, mgZ);
    int cx = x*2 +1;
    int cy = y*2 +1;

    Scale(&c->East	, Id(cx+1, cy+0));
    Scale(&c->SouthEast	, Id(cx+1, cy-1));
    Scale(&c->South	, Id(cx+0, cy-1));
    Scale(&c->Center	, Id(cx+0, cy+0));
 }
}



/*=============================================================================
 * ITERATE
 */
void mohoC::Iterate()
{
 long temp[W*H];
 int x, y;
 long v;

 for(int k = 0; k < W*H; k++) temp[k] = HM[k];

 for(y = 0; y < H; y++) for(x = 0; x < W; x++)
    {
	v = temp[x + y*W] / 5;

	*He(x  , y  ) -= v*4;

	*He(x-1, y+0) += v;
	*He(x+1, y+0) += v;
	*He(x+0, y+1) += v;
	*He(x+0, y-1) += v;
    }
}



/*=============================================================================
 * SPAWN NODE
 */
void mohoC::SpawnNode()
{
 long x = Rnd(W);
 long y = Rnd(H);
 *He(x, y) = Rnd.Raw16()*1024;
}





/*=============================================================================
 *
 */
static void mohoGenerateHeights(mapC* map, long seed)
{
 int w = map->W*2;	// must wrap around
 int h = map->H*2 + 1;	// must NOT wrap around

 mohoC moho(w, h, seed);

 // generate vertexes
 double t = w*h * DENSITY;
 int nodes = (int)(t + t*moho.Rnd()*.1);

 for(int i = 0; i < nodes; i++) moho.SpawnNode();
 for(int i = 0; i < ITERATIONS; i++) moho.Iterate();
 moho.CopyMap(map);
}





/*=============================================================================
 * TERRA
 */
#define UNDER_LAYER	0
#define LOWER_LAYER	1
#define MEDIUM_LAYER	2
#define HIGHER_LAYER	3



static unsigned terraHeightLayer(int height)
{
 if(height < 256*UNDER_LAYER_HEIGHT) return UNDER_LAYER;
 if(height < 256*LOWER_LAYER_HEIGHT) return LOWER_LAYER;
 if(height < 256*MEDIUM_LAYER_HEIGHT) return MEDIUM_LAYER;
 return HIGHER_LAYER;
}



/*=============================================================================
 * PROBABILITIES
 */
static double terraTundraProbability(double l)
{
 double a = 2*BARREN_PROBABILITY_AT_ONE - 4*BARREN_PROBABILITY_AT_HALF;
 double b = BARREN_PROBABILITY_AT_ONE - a;
 double p = l*(l*a + b);

 return p > 0 ? p : 0;
}

static double terraDesertProbability(double l)
{
 return terraTundraProbability(1. - l);
}





/*=============================================================================
 * MORPH
 */
static int terraForestValue(cellC* cell)
{
 int f = 0;

 cellC* s;
 for(int k = 1; s = cell->SMAC(k); k+=2)
    if(s->Land == landForest) f++;
 return f;
}



static void terraForestSwap(cellC* c)
{
 cellC* best = c;
 int bestF = 0;
 int f;

 cellC* s;
 for(int k = 0; s = c->SMAC(k); k+=2)
    if(	s->Land == landDesert
    ||	s->Land == landTundra
    ||	s->Land == landHills
    ||	s->Land == landPlain
    )
	if((f = terraForestValue(s)) > bestF)
		bestF = f, best = s;

 if(best == c) return;
 landC* t;
 t = s->Land;
 s->Land = c->Land;
 c->Land = t;
}



static void terraExpand(cellC* cell, randomC& rnd)
{
 cellC* smac;
 for(int k = 1; smac = cell->SMAC(k); k+=2)
    if(smac->Land == landPlain)
	 if(rnd() < .3)
	    smac->Land = cell->Land;
}



/*=============================================================================
 * BASE
 */
static landC* terraBase(cellC* c, mapC* map, randomC& rnd)
{
 // SHORE
 int under = 0;
 int lower = 0;
 for(int v = 0; v < 9; v++)
    switch(terraHeightLayer(c->Vertex(v).h)) {
	case UNDER_LAYER: under++; break;
	case LOWER_LAYER: lower++; break;
	default: break;
    }
 if(under > 2 && lower > 2 && under+lower > 6) return landShore;


 // classify ULMH
 long sum = 0;
 for(int v = 0; v < 9; v++)
    sum += c->Vertex(v).h;

 double p;
 double latitude;

 latitude = (c->Y / (double)map->H)*2 - 1;
 if(latitude < 0) latitude = -latitude;

 switch(terraHeightLayer(sum/9)) {
    case UNDER_LAYER: return landSea;

    case LOWER_LAYER:    
	p = rnd();

	p -= terraDesertProbability( latitude );	if(p < .0) return landDesert;
	p -= terraTundraProbability( latitude );	if(p < .0) return landTundra;
	p -= SWAMP_PROBABILITY;				if(p < .0) return landSwamp;
	return landPlain;

    case MEDIUM_LAYER:
	p = rnd();

	p -= terraDesertProbability( latitude )/2;	if(p < .0) return landDesert;
	p -= terraTundraProbability( latitude )/2;	if(p < .0) return landTundra;
	p -= FOREST_PROBABILITY;			if(p < .0) return landForest;
	return landHills;

    case HIGHER_LAYER: return landMountains;
 }
}



/*=============================================================================
 * GENERATE LAND
 */
static void terraGenerateLand(mapC* map, long seed)
{
 randomC rnd(seed);

 for(cellC* c = mgStart; c < mgEnd; c++)
    c->Land = terraBase(c, map, rnd);

 for(int i = 0; i < FOREST_ITERATIONS; i++)
     for(cellC* c = mgStart; c < mgEnd; c++)
	if(c->Land == landForest)
	    terraForestSwap(c);

 for(int i = 0; i < BARREN_ITERATIONS; i++)
    for(cellC* c = mgStart; c < mgEnd; c++)
	if(	c->Land == landForest
	||	c->Land == landDesert
	||	c->Land == landTundra
	||	c->Land == landSwamp
	)
	    terraExpand(c, rnd);

 // place everything to the correct offset: sea level is 0
 for(cellC* c = mgStart; c < mgEnd; c++)
    for(mapVC* v = c->VertStart; v < c->VertEnd; v++)
	v->h -= (int)(255*UNDER_LAYER_HEIGHT);


 // raise mountains
 for(cellC* c = mgStart; c < mgEnd; c++) if(c->Land == landMountains)
    c->Center.h *= 1.3;


 // place EoW
 for(int x = 0; x < map->W; x++)
 {
    (*map)(x, 0, mgZ)->Land = landEoW;
    (*map)(x, 1, mgZ)->Land = landEoW;
    (*map)(x, map->H-1, mgZ)->Land = landEoW;
    (*map)(x, map->H-2, mgZ)->Land = landEoW;
 }
}





/*=============================================================================
 * NEUTRAL
 */
void mapGenNeutral(cellC* c, randomC& rnd)
{
 int ri = rnd(5);
 realmC* r = realmsHook;
 for(int q = ri; q--; r = r->Next);

 bool node = rnd() < .3;

 neutralC* n = new neutralC(c, r, node);

 //@@
 n->Ani = 50 + rnd(7);
}





/*=============================================================================
 * GENERATE
 */
void mapC::Generate(long seed)
{
 // generate Arcanus
 mgZ = 0;
 mgStart = C;
 mgEnd = C + W*H;
 mohoGenerateHeights(this, seed);
 terraGenerateLand(this, seed);

 // generate Myrror
 mgZ = 1;
 mgStart += W*H;
 mgEnd += W*H;
 mohoGenerateHeights(this, seed+1);
 terraGenerateLand(this, seed+1);

 // set random seed and plus
 randomC rnd(seed);
 for(cellC* c = C; c < Cend; c++)
 {
    c->Seed = rnd.Raw16();
    c->Plus = c->Land->Plus(rnd());

    c->PlusS = c->Plus->Ani.Init(rnd.Raw16(), rnd.Raw16());

    if(!c->Plus->Ani.Mode) if(rnd() < .02)
	mapGenNeutral(c, rnd);
 }
}

//EOF =========================================================================
