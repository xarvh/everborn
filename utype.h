/*
 UTYPE.H		v0.03
 Unit types.

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
#ifndef H_FILE_UTYPE
#define H_FILE_UTYPE
//=============================================================================
#include "data.h"
#include "flesh.h"



#define UNIT_NAME_BF		20
#define UNIT_INFO_BF		150





/*=============================================================================
 * Unit specials
 */
// 1<<0 reserved to SPC_ACTIVE
#define ATT	(1<<1)
#define CMD	(1<<2)
#define DEF	(1<<3)
#define MOVE	(1<<4)
#define SPC	(1<<5)
#define SPL	(1<<6)
#define AMMO	(1<<7)
#define HERO	(1<<8)


// .hero: hero's specials
#define HERO_ARCANEPOWER	(1<< 0) // +50% magic ranged attack
#define HERO_ARMSMASTER		(1<< 1)	// exp bonus to stacked units
#define HERO_CHARMED		(1<< 2) // BIG resistance bonus
#define HERO_FINESSE		(1<< 3) // adds level/2 to ToHit
#define HERO_LEADER		(1<< 4) // ?? combat bonuses to friendly units
#define HERO_LEGENDARY		(1<< 5)	// can go to anyone
#define HERO_LUCKY		(1<< 6) // +1 to ANY roll
#define HERO_MIGHTY		(1<< 7)	// adds level to Melee and Hits
#define HERO_NOBLE		(1<< 8) // +10G instead of upkeep
#define HERO_PRAYERMASTER	(1<< 9) // ?? bonuses to frindly units
#define HERO_SAGE		(1<<10) // research bonus
#define HERO_SCOUT		(1<<11) // FORESTER+MOUNTANEER+SCOUTING
#define HERO_FOOD		(1<<12)
#define HERO_ECONOMY		(1<<13)
#define HERO_CULT		(1<<14)
#define HERO_SCHOOLAR		(1<<15)
#define HERO_CRAFT		(1<<16)
#define HERO_DIPLOMAT		(1<<17)
#define HERO_ASSASSIN		(1<<18)


// .att: attack
#define ATT_DEATHG	(1<< 0)
#define ATT_CHAOSG	(1<< 1)
#define ATT_STONEG	(1<< 2)
#define ATT_DISEVIL	(1<< 3)
#define ATT_STONET	(1<< 4)
#define ATT_AP		(1<< 5)
#define ATT_ILLUSION	(1<< 6)
#define ATT_FS		(1<< 7)
#define ATT_WALL	(1<< 8)
//#define ATT_BOAT	(1<< 9)
#define ATT_LBRTH	(1<<10)
//#define ATT_BERSERK	(1<<11)

// .cmd: commands
#define CMD_BUILD_ROAD	(1<< 0)
#define CMD_PURIFY	(1<< 1)
#define CMD_OUTPOST	(1<< 2)
#define CMD_PLANE_SHIFT	(1<< 3)
#define CMD_CATAPULT	(1<< 4)
#define CMD_NODEMELD	(1<< 5)

// .def: defensive specials
#define DEF_COLD	(1<< 0)
#define DEF_DEATH	(1<< 1)
#define DEF_FIRE	(1<< 2)
#define DEF_ILLUSION	(1<< 3)
#define DEF_MAGIC	(1<< 4)
#define DEF_MISSILE	(1<< 5)
#define DEF_POISON	(1<< 6)
#define DEF_STONING	(1<< 7)
#define DEF_WEAPON	(1<< 8)
#define DEF_LASH	(1<< 9)
#define DEF_FDODGE	(1<<10)
#define DEF_NFS		(1<<11)

// .move: movement
#define MOVE_WALK	(1<< 0)
#define MOVE_SWIM	(1<< 1)
#define MOVE_SAIL	(1<< 2)
#define MOVE_FLY	(1<< 3)
#define MOVE_NOC	(1<< 4)
#define MOVE_TELEPORT	(1<< 5)
#define MOVE_MERGE	(1<< 6)
#define MOVE_MOUNTAIN	(1<< 7)
#define MOVE_PATH	(1<< 8)
#define MOVE_FOREST	(1<< 9)
#define MOVE_WIND	(1<<10)
#define MOVE_STREAM	(1<<11)	// unused
#define TEAM_MOVE	(MOVE_MOUNTAIN+MOVE_PATH+MOVE_FOREST+MOVE_WIND)

// .spc: specials and misc
#define SPC_FEAR	(1<< 0)
#define SPC_INVISIBLE	(1<< 1)
#define SPC_REGEN	(1<< 2)
#define SPC_HEALER	(1<< 3)
#define SPC_LUCKY	(1<< 4)
#define SPC_UNDEAD	(1<< 5)
#define SPC_SUNDEAD	(1<< 6)
//#define SPC_CARRIER	(1<< 7)
#define SPC_CATAPULT	(1<< 8)
#define SPC_LBRINGER	(1<< 9)
#define SPC_HOLY	(1<<10)
#define SPC_IMMOLATED	(1<<11)
#define SPC_RESIST	(1<<12)

// .spl: spells
#define SPL_PSI_BLAST	(1<< 0)
#define SPL_DOOM_BOLT	(1<< 1)
#define SPL_FIRE_BOLT	(1<< 2)
#define SPL_HEALING	(1<< 3)
#define SPL_DEMON	(1<< 4)
#define SPL_WEB		(1<< 5)
#define SPL_LIFE	(1<< 6)
#define SPL_DEATH	(1<< 7)
#define SPL_CHAOS	(1<< 8)
#define SPL_SORCERY	(1<< 9)
#define SPL_NATURE	(1<<10)





/*=============================================================================
 * Ammo types
 */
#define AMMO_ARROW	0
#define AMMO_SLING	1
#define AMMO_JAVELIN	2
#define AMMO_ROCK	3
#define AMMO_CANNON	4
#define AMMO_V_MAGIC	5
#define AMMO_B_MAGIC	6
#define AMMO_F_MAGIC	7
#define AMMO_E_MAGIC	8





/*=============================================================================
 * Unit roles
 */
#define ROLE_SPEARMEN	0
#define ROLE_SWORDSMEN	1
#define ROLE_FRONTLINE	2
#define ROLE_BOWMEN	3
#define ROLE_CAVALRY	4
#define ROLE_CLERIC	5
#define ROLE_MAGICIANS	6
#define ROLE_TRIREME	7
#define ROLE_GALLEY	8
#define ROLE_WARSHIP	9
#define ROLE_ENGINEERS	10
#define ROLE_SETTLERS	11
#define ROLE_ELITE	12
#define ROLE_FLYER	13
#define ROLE_CATAPULT	14
#define ROLE_PLAYERMAGE	15





/*=============================================================================
 * ROLE
 */
typedef struct unitRoleS {
    long	code;
    char*	id;
    char*	name;

    //@@ info?
    //@@ battlefield formation / deployement?
} unitRoleT;





/*=============================================================================
 * UTYPE
 */
class raceC;
class spcC;

class utypeC { public:
    utypeC*		Next;

    char		Name[UNIT_NAME_BF];
    char		Info[UNIT_INFO_BF];

    raceC*		Race;

    dataC		Skin;
    fleshC		Flesh;
//    bonesC		bones;

    unitRoleT*		Role;
    long		Req;
    long		HeroBase;
    long		HeroPlus;

    // specials
    long		Att;
    long		Cmd;
    long		Def;
    long		Move;
    long		Spc;
    long		Spl;

    // stats
    short StatsStart[0];

    short Std;		// name & info std id, 255 for none
    short RangeT;	// arrows/fireball/rocks...
    short Range;
    short Ammo;

    short Cost;		// prod cost
    short Food;		// food manteniance
    short Gold;		// gold manteniance
    short Mana;		// mana manteniance

    short Endurance;
    short Accuracy;	// to-hit bonus
    short Figures;
    short Speed;

    short Melee;
    short Defense;
    short Resist;
    short Life;

    short Breathe;	// if ATT_LBRTH is set, will use this value
    short Lifesteal;
    short Poison;
    short Transport;

    //
    bool	AmmoIsMagic();
    bool	CanBeHero();
    float*	Stack();
    spcC*	Special(int id);
};





/*=============================================================================
 * txt_unit.cpp
 */
class blockC;
class bldC;

utypeC* unitLoadAll(char* s, char* e, blockC* blockL, bldC** bldA);
char* unitWriteAll(char* w, utypeC* hook, bldC** bldA);
int   unitLoadExRace(char* s, char* e, raceC* race);
char* unitWriteExRace(char* w, raceC* r);
int   unitDestroyL(utypeC* hook);





#endif
//EOF =========================================================================
