/*
 MAP			v0.31
 Map, land, cells.

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
#ifndef H_FILE_G_MAP
#define H_FILE_G_MAP
//=============================================================================
#include "random.h"


#define BATTLE_W		45
#define BATTLE_H		45
#define BATTLE_DEFAULT_ACTION	10
#define BATTLE_AFTERMATH_TIME	50	// 5" aftermath
#define BATTLE_DEFAULT_DURATION	3000	//300	// one minute

// Wall
#define BATTLE_WALL_SIZE	9

#define BCF_NONE	0
#define BCF_WALL	1
#define BCF_GATE	2
#define BCF_BLD		3
#define BCF_HOUSE	4
#define BCF_TOWER	5



// Cell Flags
#define CELL_CORRUPT	(1<<0)
#define CELL_ENCHROAD	(1<<1)


// Movement Types
#define TERRAIN_UNCROSSABLE	0
#define TERRAIN_MOUNTAIN	1
#define TERRAIN_FOREST		2
#define TERRAIN_LAND		3
#define TERRAIN_SHORE		4
#define TERRAIN_SEA		5





/*=============================================================================
 * BATTLE ENCHANTEMENTS
 */
#define BENCH_HIGH_PRAYER	0
#define BENCH_TRUE_LIGHT	1
#define BENCH_PRAYER		2

#define BENCH_TOTAL		3





/*=============================================================================
 * LAND ANIMATIONS
 */
#define ANI_STATIC	1
#define ANI_RANDOM	2
#define ANI_STD		3
#define ANI_CYCLEBLINK	4
#define ANI_BLINK	5

class landAniC { public:
    unsigned char	Id;	//  Initial Id
    unsigned char	Alt;	//  Alternatives count
    unsigned char	Fpa;	//  Frames Per Alternative
    unsigned char	Mode;	//  Animation mode

    //
    unsigned NextFrame(unsigned status);
    unsigned Init(unsigned alt, unsigned s);
};





/*=============================================================================
 * LAND TYPES, NEUTRALS and PLUSES
 */
class cellC;
class brainC;
class realmC;



class neutralC { public:
    cellC*	Cell;
    brainC*	Owner;
    realmC*	Realm;

    bool	Node;
    char	Rating;
    char	Status;
    char	Ani;

    //
    neutralC(cellC* c, realmC* r, bool node);
    ~neutralC();

    //
    void Think();
};



class plusC { public:
    char*	Name;

    char	Food;
    char	Mana;
    char	Gold;
    char	CostB;

    landAniC	Ani;

    //
    //plusC();
};

class landC { public:
    char*	Name;

    char	GoldB;
    char	ProdB;
    char	MaxFood;
    char	Type;

    char	RoadAvailability;	// Amount added each step when builiding a road

    void	(*Render)(cellC*);
    plusC*	(*Plus)(double rnd);
    //
//    landC();
};






/*=============================================================================
 * BATTLES
 */
// battle sub cell:

class bcC { public:
    unsigned char BaseS;
    unsigned char Feature;
    unsigned char FeatureId;
    unsigned char FeatureHits;

    signed char Height[4];

    unsigned char Occupied;
};



// battle
	// dependencies
	class cellC;
	class particleC;

class battleC { public:
	char	Ench[BENCH_TOTAL];

	bcC	SubCellA[BATTLE_W * BATTLE_H];
	bcC	SubCellEnd[0];
	long	Time;
	long	TimeEnd;
	long	TimeCheck;
//	long	PauseMask;
	cellC*	Cell;
	particleC* ParticlesHook;

	bool	PrevCheck;


	void	GenerateMap();

	randomC	Rnd;

	//
	battleC(cellC*);
	~battleC();

	// unit layout
	void LayUnitsAtAngle(int angle);
	void LayUnitsAtCenter();
	void LayUnits();

	//
	void	Select();
	void	Think();
	void	Turn();		// when a battle lasts several turns
	long	IsPaused();
	bool	IsPostponed();	// paused until next turn
//	void	PauseToggle(int id);

	bcC*	BC(float x, float y);
	bcC*	BC(signed x, signed y);
};





/*=============================================================================
 * CELLS
 */
class mapVC { public:
    signed short	h;
    signed char		dx;
    signed char		dy;
};



class battleC;
class cityC;
class unitC;
class mapC;

class cellC { public:
    static mapC*	Map;
    short		X;
    short		Y;
    char		Z;

    //@@char*		AreaName;	// geographic name
    landC*		Land;
    plusC*		Plus;		// can't be NULL
    neutralC*		Neutral;

    // vertexes
    mapVC		VertStart[0];
    mapVC		Center;
    mapVC		East;
    mapVC		SouthEast;
    mapVC		South;
    mapVC		VertEnd[0];


    // optional Cities/Units/Battlefields
    cityC*		City;
    unitC*		Unit;
    battleC*		Battle;

    long		KnowMask;
    long		ViewMask;
    long		DetectMask;

    long		Seed;

    // Misc Features
    unsigned char	RoadStatus[8];	// 0 if road is available

    unsigned char	River;
    unsigned char	Flags;
    unsigned char	BaseS;
    unsigned char	OverS;
    unsigned char	PlusS;

    // animation Status
    unsigned char	Path;	//@@to be removed

    //
    cellC();
    ~cellC();

    bool	HasRoad(int angle);

    void	DisplacedVertex(int vertex, int angle, double d);
    void	VertexDisplay(unsigned k);
    mapVC&	Vertex(unsigned k);

    static signed	SMACX(unsigned k);
    static signed	SMACY(unsigned k);

    cellC*	SMAC(unsigned k);
    cellC*	BACI(unsigned k);
    cellC*	HUGS(unsigned k);
    cellC*	Mirror();


    void	SpawnBattle();
    void	CloseBattle();
    bool	CheckBattle();
    bool	CheckBattle(long players);
};





/*=============================================================================
 * MAP
 */
class mapC { public:
    unsigned	W;
    unsigned	H;

    cellC*	C;
    cellC*	Cend;

    //
    mapC();
    ~mapC();
    cellC* operator()(signed x, signed y, char z);

    void ClearPaths();

    void Generate(long seed);
    void Allocate(unsigned w, unsigned h);
    void Destroy();
};





/*=============================================================================
 * MAP
 */
signed mapSMACX(unsigned k);
signed mapSMACY(unsigned k);



/*=============================================================================
 * BATTLE
 */
int battleEscape(int x, int y);
bool battleIsBorder(int x, int y);
bool battleIsOOBorder(int x, int y);



/*=============================================================================
 * MINIMAP
 */
class mapViewC;
void minimapSetSize(double w, double h);
void minimapDisplay(mapViewC* mw, int z);
void minimapUpdate();

//void mdMinimap(int pixelW, int pixelH, int z);



/*=============================================================================
 * LAND
 */
extern landC landEoW[1];
extern landC landPlain[1];
extern landC landHills[1];
extern landC landSwamp[1];
extern landC landForest[1];
extern landC landDesert[1];
extern landC landTundra[1];
extern landC landSea[1];
extern landC landShore[1];
extern landC landMountains[1];

extern plusC plusNone[1];
extern plusC plusNightshade[1];
extern plusC plusMithril[1];
extern plusC plusAdamantium[1];
extern plusC plusIron[1];
extern plusC plusCoal[1];
extern plusC plusSilver[1];
extern plusC plusGold[1];
extern plusC plusGem[1];
extern plusC plusQuork[1];
extern plusC plusCrysx[1];
extern plusC plusCattle[1];



#endif
//EOF =========================================================================
