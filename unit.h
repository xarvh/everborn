/*
 UNIT.H			v0.22
 Units.

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
#ifndef H_FILE_UNIT
#define H_FILE_UNIT
//=============================================================================


#define UNIT_SHOOT_RANGE	8



#define UENCH_MIND_STORM	0
#define UENCH_WEB_ACTIVE	1
#define UENCH_WEB_BROKEN	2
#define UENCH_WATER_WALKING	3
#define UENCH_PATHFINDING	4
#define UENCH_RIGHTEOUSNESS	5
#define UENCH_INVULNERABILITY	6
#define UENCH_LIONHEART		7
//#define UENCH_ANGELIC_ARMOR	8
#define UENCH_PLANAR_TRAVEL	9
#define UENCH_HEROISM		10
#define UENCH_BLESS		11
#define UENCH_HOLY_ARMOR	12
#define UENCH_TRUE_SIGHT	13
#define UENCH_ASCENTION		14
#define UENCH_ENDURANCE		15
#define UENCH_HOLY_WEAPON	16

#define UENCH_TOTAL		17



/*=============================================================================
 * HERO
 */
class brainC;
class utypeC;
class unitC;
class heroC { public:
    heroC*	Next;

    char	Name[32];
    unitC*	Unit;	// can be 0 if hero is dead!!
    long	Bonuses;

    //@@itemC*	Items[3];

    // redundant with unitC
    brainC*	Owner;
    utypeC*	Type;
    unsigned	Exp;

    //
    heroC(utypeC*);
    ~heroC();
};





/*=============================================================================
 * RANK
 */
typedef struct {
    unsigned	ExpThreshold;

    char*	NameHero;
    char*	NameUnit;

    char	Attack;
    char	Accuracy;
    char	Defense;
    char	Resist;
    char	Life;
} rankT;





/*=============================================================================
 * PATH
 */
typedef unsigned (*costFunctionT)(int x, int y, int incoming_direction);
typedef unsigned (*heuFunctionT)(int sx, int sy, int ex, int ey);

class nodeC { public:
    nodeC* Next;

    int X;
    int Y;

    nodeC* Parent;
    long Cost;
    long Heu;

    char DBBest;
    char ParentDirection;
    bool Opened;
    bool Closed;

    long Flags;	// not touched by base pathing algorithm

    //
    nodeC();
};

class gridC { public:
    int		W;
    int		H;

    nodeC*	Grid;

    nodeC*	OpenedList;
    nodeC*	ClosedList;

    unsigned	MaxClosed;
    costFunctionT Cost;
    heuFunctionT  Heu;

    char*	Path;
    unsigned	PathLength;

    //
    gridC(int w, int h);
    ~gridC();
    nodeC* Node(int x, int y);
    nodeC* NodeSmac(nodeC* node, int direction);

    void Close(nodeC* n);
    void Open(nodeC* n, nodeC* parent, int direction, nodeC* end);
    float Pathfind(
	int sx, int sy, int ex, int ey,
	float targ_heu,
	costFunctionT cf, heuFunctionT hf);
};





class pathC { public:
    char*	Start;
    char*	Status;
    char*	End;

    pathC();
    ~pathC();

    void Make(char* path, unsigned path_length);
    void Clear();
    void Pop();
    signed NextDirection();
    signed PrevDirection();
};





/*=============================================================================
 * TRAVEL
 *
 * Attached to a team to perform a single cell-to-cell movement in the
 * overland map.
 * It must be created to make the team move, and destroys automatically when
 * the movement is done.
 * 
 * A travelC will be created by the movement order if no other travelC is
 * already allocated.
 * The travelC can be manually destroyed only when the movement has not
 * started.
 *
 * At units main loop, first all travelC are executed, then unit's orders.
 * This allows a destroyed travelC to be replaced at once, ensuring that no
 * loop cycle will be wasted.
 */
#define TRAVELT_TRAVEL	0
#define TRAVELT_ROAD	1
#define TRAVELT_SHIFT	2

#define TRAVEL_JUST_STARTED		0
#define TRAVEL_ANIMATING		1
#define TRAVEL_COMPLETED		2
#define TRAVEL_BLOCKED			3


class unitC;
class cellC;

class travelC { public:
    travelC*		Next;
    unitC*		Team;

    char		Type;
    char		StatusCode;
//    bool		Disembark;

    cellC*		Src;
    cellC*		Dest;
    unsigned char	Cost;		// Speed = 6 / cost
    unsigned char	Status;		// 0 not started, 6 arrived
    unsigned char	Direction;

    //
    bool Think();
    ~travelC();
    //private:
     travelC(unitC* team, char type, int dir);
};





/*=============================================================================
 * ACTION
 *
 * Tells what the unit is doing: determines both game effect and
 * animation.
 */
class unitC;

class actionC { public:
    char*	Name;
    void	(*Exec)(unitC*);
    char*	(*Init)(unitC*);	// must return animation name
    bool	(*IsCompleted)(unitC*);	

    bool	UsesCell;

    // constructor
    actionC(
	char*,
	void  (*)(unitC*),
	char* (*)(unitC*),
	bool  (*)(unitC*),
	bool  usesCell);

    static actionC* Get(char* );


    private:
	actionC*	Next;
};





/*=============================================================================
 * ORDER
 *
 * order->execute(unit) selects next unit->action and calls action->start(unit).
 * When the order has been fullfilled, returns true:
 * the order will be popped from the queue.
 */
class brainC;

class unitOrderC { public:
    unitOrderC*	Next;

    char*	Name;

    bool	(*IsAvailable)(unitC* u);	// can this order be issued on this unit?
    char*	(*Exec)(unitC*, signed, signed, void*);		// for instant orders

    bool	(*IsCompleted)(unitC* u);	// has the task been fulfilled?
    void	(*SetAction)(unitC* u);

    long	Flags; //@@unused?
    unitOrderC* Fallback;			// what happens if !IsAvailable?

    //
    unitOrderC(		// through orders: they SetAction()s
	unitOrderC** hook,
	char*,
	unitOrderC*,
	bool (*)(unitC*),
	bool (*)(unitC*),
	void (*)(unitC*),
	long);

    unitOrderC(		// instant orders: they get Exec()uted at once
	unitOrderC** hook,
	char*,
	bool (*)(unitC*),
	char* (*)(unitC*, signed, signed, void*), // instant orders are targetless
	long);
};



class unitOrderWaypointC { public:
    unitOrderWaypointC*	Next;

    const unitOrderC*	Order;
    signed		X;
    signed		Y;
    void*		P;

    unitOrderWaypointC(unitOrderC* order);
    ~unitOrderWaypointC();
};



class unitOrderListC { public:
    unitOrderWaypointC* WP;	// dynamic list

    virtual unitOrderC* OrderDefault() = 0;

    void Pop();
    void Set(unitOrderC*, bool append, signed x, signed y, void* t);
    void Set(unitOrderC*, bool append, signed x, signed y);
    void Set(unitOrderC*, bool append, void* t);
    void Set(unitOrderC*, bool append);

    unitOrderListC(unitOrderC* );
    ~unitOrderListC();
};



class unitOrderBattleC: public unitOrderListC { public:
    unitOrderBattleC() : unitOrderListC(OrderDefault()) {}

    unitOrderC* OrderDefault();
};


class unitOrderLandC: public unitOrderListC { public:
    unitOrderLandC() : unitOrderListC(OrderDefault()) {}

    pathC Path;
    unitOrderC* OrderDefault();

    void Clear();
};






/*=============================================================================
 * UNIT
 */
class animationC;

class bcC;
class cellC;
class brainC;
class colorC;
class utypeC;

class unitC { public:
    utypeC*	Type;
    brainC*	Owner;
    heroC*	Hero;

    // all these must be handled exclusively by
    // ::Displace()
    travelC*	Travel;
    unitC*	Next;
    cellC*	Cell;
    unitC*	Transported;		// points to transporter
    unitC*	NextTransported;	// transported units dynamic list
    // a non transported unit has both to 0
    // a loaded transporter has Transported to 0 and NextTransported set
    // a transported unit has Transported != 0

    long	Seed;

    // orders list
    unitOrderBattleC	BattleOrders;
    unitOrderLandC	LandOrders;

    // used to zero at initialization -----------------------------------------
    char ZeroStart[0];

    char EnchPerm[UENCH_TOTAL];
    char EnchTemp[UENCH_TOTAL];

    // Team ID
    long	Team;

    // battle position (-NOT- used in overland!)
    float	X;	// -1 if unit battle position has not been initialized
    float	Y;

    // angle
    float	A;

    // animation
    animationC*	Animation;
    float	AnimationTime;

    // action
    actionC*	Action;
    short	ActionStatus;	// zeroed when action completed

    // action parameters: their usage depends by the current action
    signed char ActionReport;	// used to communicate status to order func
    void*	ActionTargP;	// usually target unit
    float	ActionTargX;	// usually destination X
    float	ActionTargY;	// usually destination Y
    float	ActionDist;	// usually pathing targ_heu or wave stop

    //game
    signed char Ammo;
    signed char Wounds;		// it must be allowed to be negative.
    short Exp;


    // gui stuff
    long SelectionMask;

    // time available for overland movement
    signed char	TimeAvailable;	// may be negative is unit has fled

    // Zero End ---------------------------------------------------------------
    char ZeroEnd[0];

    //
    unitC();
    long Id();
    char* Name();
    colorC* Color();
    brainC* PlayerMage();
    float* Stack();
    float operator*(unitC*); // distance^2, this is as ridiculous as it is fun.
    unitC* FindBestTarget();
    int HeroBonus(long);
    bool IsCreature();


    // unit Mechanics
    rankT* Rank();
    int  Level();
    bool CanPlaneShift();
    bool CanShoot();
    float MeleeDistanceVS(unitC*);
    float RangeDistanceVS(unitC*);
    signed Figures();
    signed Health();
    signed Defense();
    signed Melee();
    signed Range();
    signed HitModifier();
    signed Resist();
    signed Poison();
    bool CanFly();
    bool CanDetect();
    bool BACIView();
    signed GoldUpkeep();
    signed ManaUpkeep();
    signed FoodUpkeep();

    // battle lay
    bool IsBattleEnterAngle(int angle);
    bool IsFront();
    bool IsMid();
    bool IsBack();
    bool IsSide();
    void BattleLay(int x, int y, int angle, int max_rows);

    // damage
    void TakeDamage(signed amount, int type);
    void InflictMeleeDamage(unitC* targ);
    void InflictRangedDamage(unitC* targ, int times);

    // status
    void Init(utypeC* type, cellC* cell, heroC* hero);
    void Remove();
    void Die();
    bool IsInOverland();
    bool IsInBattle();
    bool Exists();
    bool IsDead();
    bool IsAlive();

    // teaming and selection
    bool IsSelectedBy(long mask);
    bool IsSelectedBy(brainC* b);
    void Select(long mask);
    void Deselect(long mask);
    static void DeselectAll(long mask);
    void SelectOnly(long mask);

    // game
    void ExpAdd();
    void NextTurn();
    void Think();

    // movement
    bool CanEscape(int direction);
    bool TeamCanEnterTransportsAt(cellC*);
    bool TeamEnterTransportsAt(cellC*);
    void TeamExitTransport();
    bool TeamCanAffordTime(int time);
    void TeamDisplace(cellC* c);
};





unitC* unitNew(utypeC* type, cellC* cell);
extern rankT unitRanksA[];





/*=============================================================================
 * MOVE
 */
class battleC;
class bcC;
void umSetGrid(battleC* b);
bool umMoveTo(unitC* u, double x, double y, double distance);
void umDrawPathGrid(bcC* c, int mx, int my);



/*=============================================================================
 * TRAVEL
 */
bool utTeamPath(unitC* u);



/*=============================================================================
 * ACT
 */
int orderExecute(unitC* u);
int actionBegin(int actionID, unitC* u);
int actionGame(unitC* u);





/*=============================================================================
 * ACT BATTLE
 */
extern unitOrderC uobWAVE;
extern unitOrderC uobMOVE;
extern unitOrderC uobMELEE;
extern unitOrderC uobSHOOT;
extern unitOrderC uobESCAPE;
extern unitOrderC uobENGAGE;
extern unitOrderC uobHOLD;
extern unitOrderC uobDEAD;

void uobSet(unitOrderC* o, brainC* b, bool append, signed x, signed y, void* p);





/*=============================================================================
 * ACT LAND
 */
extern unitOrderC uolTRAVEL;
extern unitOrderC uolWAIT;
extern unitOrderC uolROAD;
extern unitOrderC uolSETTLE;
extern unitOrderC uolSHIFT;
void uolSet(unitOrderC* o, brainC* b, bool append, signed x, signed y, void* t);





/*=============================================================================
 * DISPLAY, FLESH
 */
class fleshC;
void duHealth(unitC* u);
void duStats(unitC* u);
void duUnit(unitC* u);
void duType(utypeC* u, colorC* color);
void fleshMultiStart();
void fleshMultiAnimate(fleshC* flesh, colorC* color);
void duStack(unitC* u);
void duTypeStack(utypeC* u, colorC* c);



#endif
//EOF =========================================================================
