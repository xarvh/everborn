/*
 SPELL.H	v0.01

 Copyright (c) 2004-2008 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "cmd.h"
#include "game.h"



#define REALM_NEUTRAL	0
#define REALM_LIFE	1
#define REALM_DEATH	2
#define REALM_CHAOS	3
#define REALM_NATURE	4
#define REALM_SORCERY	5


#define SPELL_OBJ_NONE	-1



/*=============================================================================
 * SUMMON CODES
 */
#define SPELL_SUMMON_MAGIC_SPIRIT	0

#define SPELL_SUMMON_GUARDIAN_SPIRIT	1
#define SPELL_SUMMON_UNICORNS		2
#define SPELL_SUMMON_ANGEL		3
#define SPELL_SUMMON_ARCHANGEL		4






/*=============================================================================
 * SPELL FX
 */
class effectC {
    char*	Name;
    void	(*Display)(effectC*, double time);	// normalized time
    long	ObjectID;
    long	SoundID;
};





/*=============================================================================
 * SPELL LAUNCH
 *
 * Stores a spell with all its launch parameters
 */
class unitC;
class cityC;
class spellC;

class spellLaunchC { public:
    spellLaunchC* Next;

    spellC* Spell;	// spell to be launched
    brainC* Caster;
    unsigned Charge;	// extra mana
    union {		// target
	unitC* Unit;
	//cityC* City;
	brainC* Wizard;
	struct { short X; short Y; char Z; };
    };

    bool Temp;		// temporary version of the spell

    //
    unsigned Cost();
    signed TurnsRequired();

    spellLaunchC();
    ~spellLaunchC();
};





/*=============================================================================
 * REALM
 */
class bookC;
class realmC { public:
    realmC*	Next;

    float	Color[3];

    int		Code;
    char*	Name;
    char*	Desc;	// sucks, I will provide only a adjective.

    bookC*	BooksHook;
    float	PowerNode;

    //
    realmC(int code, char* name, char* desc, float r, float g, float b, bool pn);
};





/*=============================================================================
 * BOOK
 */
class spellC;
class pickC;
class bookC { public:
    bookC*	Next;

    char*	Id;
    char*	Name;
    char*	History;

    pickC*	Pick;
    realmC*	Realm;
    spellC*	SpellsHook;

    bool	Maior;		// Maior vs Minor book
				// when added, Rare spells set this true
    //
    bookC(realmC* realm, char* id, char* name, char* history, bool m);
};





/*=============================================================================
 * SPELL TYPE
 */
class spellC;
class spellTypeC { public:
    char* Name;
    char* Syntax;    	// cmd stuff
    char* DefaultMenu;	// default menu used to select target

    void (*CmdToLaunch)(spellLaunchC* launch);
    int (*Check)(spellC*);	// checks target alliance/status
};



extern spellTypeC spellTypeUnit;
extern spellTypeC spellTypeCity;
extern spellTypeC spellTypeMap;
extern spellTypeC spellTypeWizard;
extern spellTypeC spellTypeGlobal;
extern spellTypeC spellTypeSummon;
extern spellTypeC spellTypeBattle;
extern spellTypeC spellTypeBattleArea;
extern spellTypeC spellTypeNode;
extern spellTypeC spellTypeNewItem;	// "enchant item" and "create artifact"
extern spellTypeC spellTypeWall;	// "disrupt"
extern spellTypeC spellTypeGlobalSpell;	// "spell binding"
extern spellTypeC spellTypeCastedSpell;	// "spell blast"





/*=============================================================================
 * SPELL
 */
#define SPELL_NEUTRAL			0
#define SPELL_COMMON			1
#define SPELL_UNCOMMON			2
#define SPELL_RARE			3

// target alliance
#define SPELL_TALLIANCE_IGNORE		0
#define SPELL_TALLIANCE_ENEMY		1
#define SPELL_TALLIANCE_FRIENDLY	2
#define SPELL_TALLIANCE_NEUTRAL		3

// all those flags are 'AND' requirements
#define SPELL_TSTATUS_IGNORE		(1<<0)
#define SPELL_TSTATUS_ALIVE		(1<<1)
#define SPELL_TSTATUS_DEAD		(1<<2)
#define SPELL_TSTATUS_ROAD		(1<<3)
#define SPELL_TSTATUS_HERO		(1<<4)
#define SPELL_TSTATUS_DONTALREADYHAVEIT	(1<<5)	//can't be repeated on the same target
#define SPELL_TSTATUS_SUMMONED		(1<<6)
// the following specials may be put in a avl() function
#define SPELL_TSTATUS_NOVOLCANO		(1<<7)	// "raise volcano"
#define SPELL_TSTATUS_TRANSMUTABLE	(1<<8)	// "transmute"
#define SPELL_TSTATUS_NOSUMMONINGCIRCLE	(1<<0)	// "summoning circle"
#define SPELL_TSTATUS_NOFORTRESS	(1<<10)	// "move fortress"
#define SPELL_TSTATUS_HASAMMO		(1<<11)	// "warp wood"
#define SPELL_TSTATUS_ELITE		(1<<12)	// "ascention"
#define SPELL_TSTATUS_DEATH		(1<<13)	// Death/animated creature
#define SPELL_TSTATUS_CHARGEABLE	(1<<14)	// Spell can be charged



#define SPELL_CON_PARMS				\
	bookC*		book,			\
	char*		id,			\
        char*		name,			\
	char*		oneliner,		\
	char*		description,		\
	spellTypeC*	type,			\
        long		picture,		\
        char		level,			\
	char		upkeepCost,		\
        short		researchCost,		\
        short		battleCost,		\
	short		overlandCost,		\
        char		targetAlliance,		\
	short		targetStatus,		\
	long		object,			\
        char*		animation,		\
	void		(*cast)(spellLaunchC*)	\

class spellC { public:
    spellC*	BookNext;
    spellC*	SpellNext;
    bookC*	Book;

    spellTypeC*	Type;

    char*	Name;
    char*	OneLiner;
    char*	Description;
    long	Picture;	// -1 for summoned creatures

    char	Level;
    char	UpkeepCost;

    short	ResearchCost;

    // sono tentato di mettere battle = overland/5, ma poi come faccio
    // a sapere se c'e' il temp?
    // a parita' di variabili, tanto vale tenere i costi separati
    short	BattleCost;	// -1 means special, 0 means not available.
    short	OverlandCost;	// -1 means special, 0 means not available.

    char	TargetAlliance;
    short	TargetStatus;	// bitmask
    long	Object;		// summoned creature ID or enchantement ID.

    effectC*	Animation;	// to be displayed when spells enters in effect
    cmdC	Cmd;
    void	(*Cast)(spellLaunchC*);	// actual spell effect!

    //
    signed	ResearchLeft[GAME_MAX_BRAINS];	// how much each has to research on this

    //
    spellC(SPELL_CON_PARMS);
    bool HasTemporaryVersion() {return BattleCost != 0;};
    void ApplyStartingResearchBonus(int mi, int ma);
    int CheckTargetAlliance(brainC* caster, brainC* target);
};



/*=============================================================================
 *
 */
extern realmC* realmsHook;
extern spellC* spellsHook;
spellC* spellGet(char *);
void spellResetAll();



void spellEnchUnit(spellLaunchC* l);
void spellEnchCity(spellLaunchC* l);
void spellEnchBattle(spellLaunchC* l);
void spellEnchGlobal(spellLaunchC* l);



/*=============================================================================
 * GUI CONTROL
 */
spellC* spellGui();

bool spellGuiUnit();
bool spellGuiCity();
bool spellGuiMap();
bool spellGuiWizard();
bool spellGuiBattle();
bool spellGuiArea();
bool spellGuiNode();
bool spellGuiWall();

void spellGuiCancel();
void spellGuiDo(int target);
void spellGuiDo(int targX, int targY, int targZ);

void spellGuiOpenBook();

//class menuC;
//void spellGuiMENUIdle(menuC* menu);
//void spellGuiMENUControl(menuC* menu);
//void spellGuiDisplay();
//bool spellGuiInput(int key, int ext);





/*=============================================================================
 * LIFE
 */
extern spellC spell_Crusade;
extern spellC spell_ArchAngel;
extern spellC spell_HolyWord;
extern spellC spell_Righteousness;
extern spellC spell_Invulnerability;
extern spellC spell_CharmOfLife;
extern spellC spell_MassHealing;
extern spellC spell_StreamOfLife;
extern spellC spell_HighPrayer;
extern spellC spell_Lionheart;
extern spellC spell_PlanarSeal;
extern spellC spell_Tranquillity;
extern spellC spell_LifeForce;
extern spellC spell_Angel;
extern spellC spell_AngelicArmor;
extern spellC spell_PlanarTravel;
extern spellC spell_TrueLight;
extern spellC spell_HolyArms;
extern spellC spell_Prosperity;
extern spellC spell_Healing;
extern spellC spell_AstralGate;
extern spellC spell_Heroism;
extern spellC spell_Incarnation;
extern spellC spell_Bless;
extern spellC spell_JustCause;
extern spellC spell_Prayer;
extern spellC spell_AltarOfBattle;
extern spellC spell_HeavenlyLight;
extern spellC spell_HolyArmor;
extern spellC spell_DispelEvil;
extern spellC spell_TrueSight;
extern spellC spell_Consecration;
extern spellC spell_Inspirations;
extern spellC spell_GuardianSpirit;
extern spellC spell_PlaneShift;
extern spellC spell_Resurrection;
extern spellC spell_Ascention;
extern spellC spell_Endurance;
extern spellC spell_RaiseFallen;
extern spellC spell_HolyWeapon;
extern spellC spell_StarFires;



//EOF =========================================================================
