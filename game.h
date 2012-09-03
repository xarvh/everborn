/*
 GAME.H			v0.22
 Game-dependent data, simple version.

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
#ifndef H_FILE_GAME
#define H_FILE_GAME
//=============================================================================
#include "map.h"
#include "unit.h"
#include "race.h"
#include "random.h"

#define GAME_NAME_BF		32
#define GAME_MAX_BRAINS		8		// 8 players
#define GAME_MAX_UNITS		5000		// 150xPlayer on a 32-players game
#define GAME_MODELSxBRAIN	50

//#define BRAIN_LOCAL_BIT		(1<<31)		// used to locally select units
#define BRAIN_NEUTRAL		(1<<30)		// used for neutral units

#define BRAIN_UNUSED		0		// just what it says
#define BRAIN_INIT		1		// waiting for it to send his init data
#define BRAIN_ACTIVE		2		// up and playing!
#define BRAIN_DEAD		3		// useless?

#define PICKS_MAX_CHOICE	6
#define PICKS_MAX_RANDOM	7
//#define PICKS_MAX		PICKS_MAX_RANDOM
#define PICK_FULL_RANDOM_LIMIT	1





#define GENCH_GAIAS_REIGN	0
#define GENCH_CRUSADE		1
#define GENCH_CHARM_OF_LIFE	2
#define GENCH_PLANAR_SEAL	3
#define GENCH_TRANQUILLITY	4
#define GENCH_LIFE_FORCE	5
#define GENCH_JUST_CAUSE	6
#define GENCH_ETERNAL_NIGHT	7
#define GENCH_HOLY_ARMS		8

#define GENCH_TOTAL		9





/*=============================================================================
 * STRUCTURES
 */
class colorC;
class utypeC;
class raceC;
class realmC;




/*=============================================================================
 * BRAIN (PLAYER)
 */
class pickC;
class spellC;
class spellLaunchC;
class brainC { public:
    char		ZeroStart[0];	//---------------------------------------------

    brainC*		Next;	// dynamic list of active brains

    // identity
    char		Name[GAME_NAME_BF];
    raceC*		Race;
    colorC*		Color;

    unitC*		PlayerMage;
    cellC*		SummoningCircle;

    pickC*		InitialPicks[PICKS_MAX_CHOICE];

    // system
    long		Bit;
    unsigned char	Id;
    unsigned char	Status;
    bool		Ready;

    // interface
    cellC*		Focus;
    cellC*		LocalFocus;	// used to change between menues
					// while the real focus routes through the net
    long		SelectedTeam;	// No "Local" trick

    // empire
    long		EngageMask;
    long		FriendMask;
    unsigned char	Taxes;		// taxes for 2000 citizens
    signed		GoldReserve;
    signed		ManaReserve;

    // allocation
    char		AllocateToSkill;
    char		AllocateToResearch;
    signed		AlchemyManaToGold;

    // magic    
    spellC*		ResearchedSpell;
    unsigned		Skill;
    unsigned		SkillPool;
    unsigned		SkillAvl;
    unsigned		SkillCommitted;
    spellLaunchC*	SpellQueue;

    char		ZeroEnd[0];	//-------------------------------------



    //
    brainC();
    ~brainC();
    void	Display(float h);
    void	InitGame();
    bool	IsThe(pickC& pick);
    bool	HasGEnch(int id);

    //
    int		MinorBooks(realmC* r);
    int		TotalBooks(realmC* r);
    bool	CanResearch(spellC* s);
    bool	CanCast(spellC* s);
    unsigned	SpellCost();
    unsigned	SpellCost(spellLaunchC* spl);
    void	CheckCast();
    void	CheckCast(spellLaunchC* spl);
    void	ClearSpellQueue();
    void	QueueSpell(spellLaunchC* spl);

    //
    void	Think();
    signed	DeltaRawMana();
    signed	DeltaSkill();
    signed	DeltaGold();
    signed	DeltaMana();
    signed	DeltaFood();
    signed	DeltaKnow();

    //
    int		CountPicks();
    char*	CheckRequirementsFor(pickC* p);
};





/*=============================================================================
 * PICK
 */
class brainC;
class cellC;
class bookC;

class pickC { public:
    pickC*	Next;


    char*	Id;
    char*	Name;
    char*	Desc;

    bookC*	Book;		// books only! =)

    brainC*	Owner;		// 0 if not owned
    cellC*	Location;

    char	ReqMask;
    bool	Desired;

    //
    pickC(char* Id, char* name, char* desc, char req_mask);

    //
    bool Is(brainC* b);
};



extern pickC* picksHook;





/*=============================================================================
 * GAME
 */
class travelC;
class gameC { public:
    mapC		Map;
    raceC		Creatures;
    unitC		Units[GAME_MAX_UNITS], UnitsEnd[0];
    brainC		Brains[GAME_MAX_BRAINS], BrainsEnd[0];
    brainC*		BrainsHook;
    randomC		Rnd;

    travelC*		TravelsHook;    // travels dynamic list
    heroC*		HerosHook;
    cellC*		ActiveBattle;

    char		ZeroStart[0];	//-------------------------------------
    char		Ench[GENCH_TOTAL];

    long		Turn;
    long		UnitTeam;	// used to assign a unique team id

    long		Seed;
    char		ZeroEnd[0];	//-------------------------------------

    //
    gameC(int self_id, long seed, int mapw, int maph);
    ~gameC();

    bool CheckReady();
    void Think();
    unsigned Roll(int str, int chance);
    unsigned Roll(int str);
    long GetNewTeam();

    void Start();
    char* Write(char* w);
    int Read(char* bf, char* ebf);

    bool IsPlayerColorAvailable(colorC* );
};





/*=============================================================================
 * GAME
 */
extern brainC* self;
extern gameC* game;
void gameClose();
bool gameIsAvailable();

class cellC;
cellC* map(signed x, signed y, char z);





/*=============================================================================
 * BRAIN
 */
class battleC;
void bdSquare(brainC* );





/*=============================================================================
 * PICK
 */
extern pickC Myrran;
extern pickC Warlord;
extern pickC Sage;
extern pickC Lightlord;
extern pickC NodeMaster;
extern pickC ChaosMaster;
extern pickC NatureMaster;
extern pickC SorceryMaster;
extern pickC Famous;
extern pickC Darklord;
extern pickC Conjurer;
extern pickC Charismatic;
extern pickC Channeller;
extern pickC Destroyer;
extern pickC Artificier;
extern pickC Archmage;
extern pickC Alchemist;

void pickAssign();





/*=============================================================================
 * UNIT
 * (why here?)
 */
unitC* unitNew(utypeC* type, cellC *cell);
unitC* unitSpawnExCity(cityC* city, utypeC* type);



#endif
//EOF =========================================================================
