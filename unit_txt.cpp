/*
 TXT_UNIT.CPP		v0.08
 Unit text io.

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

 @@BUGS
    strcpy does not check std names lenght.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "data.h"
#include "txt.h"

#include "utype.h"
#include "city.h"
#include "race.h"





/*=============================================================================
 * STAT A
 */
static utypeC unitTmp;
static stat_t unitStatA[] = {
    {  unitTmp.StatsStart,	NULL, NULL, NULL },
    { &unitTmp.Accuracy,	"Ac", "Accuracy",		"" },
    { &unitTmp.Ammo,		"Am", "Ammo",			"" },
    { &unitTmp.Breathe,		"Br", "Breathe attack",		"" },
    { &unitTmp.Cost,		"Co", "Cost",			"" },
    { &unitTmp.Defense,		"De", "Defense",		"" },
    { &unitTmp.Endurance,	"En", "Endurance",		"" },
    { &unitTmp.Figures,		"Fi", "Figures", 		"" },
    { &unitTmp.Food,		"Fo", "Food manteniance",	"" },
    { &unitTmp.Gold,		"Go", "Gold manteniance",	"" },
    { &unitTmp.Life,		"Li", "Life",			"" },
    { &unitTmp.Lifesteal,	"Ls", "Life steal",		"" },
    { &unitTmp.Mana,		"Ma", "Mana manteniance",	"" },
    { &unitTmp.Melee,		"Me", "Melee attack",		"" },
    { &unitTmp.Poison,		"Po", "Poison attack",		"" },
    { &unitTmp.Range,		"Ra", "Ranged attack",		"" },
    { &unitTmp.Resist,		"Re", "Magic resistance",	"" },
    { &unitTmp.Speed,		"Sp", "Speed", 			"" },
    { &unitTmp.Transport,	"Tr", "Transport Capacity",	"" },
    { 0, 0, 0, 0 }
};





/*=============================================================================
 * SPC A
 */
spcC unitSpcA[] = {

//=============================================================================
// .hero: hero's specials
{ HERO_ARCANEPOWER,	HERO, -1, "HERO_ARCANEPOWER",
    "Arcane power",
    "Increases the hero's magical ?HRange?/ attack by 1 per rank." },

{ HERO_ARMSMASTER,	HERO, -1, "HERO_ARMSMASTER",
    "Arms master",
    "" },

{ HERO_CHARMED,		HERO, -1, "HERO_CHARMED",
    "Charmed",
    "" },

{ HERO_FINESSE,		HERO, -1, "HERO_FINESSE",
    "Finesse",
    "Increases the hero's ToHit bonus by one every 2 ranks." },

{ HERO_LEADER,		HERO, -1, "HERO_LEADER",
    "Leader",
    "" },

{ HERO_LEGENDARY,	HERO, -1, "HERO_LEGENDARY",
    "Legendary",
    "" },

{ HERO_LUCKY,		HERO, -1, "HERO_LUCKY",
    "Lucky",
    "" },

{ HERO_MIGHTY,		HERO, -1, "HERO_MIGHTY",
    "Mighty",
    "" },

{ HERO_NOBLE,		HERO, -1, "HERO_NOBLE",
    "Noble",
    "" },

{ HERO_PRAYERMASTER,	HERO, -1, "HERO_PRAYERMASTER",
    "Prayer master",
    "" },

{ HERO_SAGE,		HERO, -1, "HERO_SAGE",
    "Sage",
    "" },

{ HERO_FOOD,		HERO, -1, "HERO_FOOD",
    "Minister of Developement",
    "+10%% food per level an -1 unrest in host city." },

{ HERO_ECONOMY,		HERO, -1, "HERO_ECO",
    "Minister of Economy",
    "+10%% gold per level in host city." },

{ HERO_CULT,		HERO, -1, "HERO_CULT",
    "Minister of Cult",
    "+10%% mana per level and +50% unrest from religious buildings in host city." },

{ HERO_SCHOOLAR,	HERO, -1, "HERO_SCHOOL",
    "High Scholar",
    "+20%% research per level in host city." },

{ HERO_CRAFT,		HERO, -1, "HERO_CRAFT",
    "Minister of Craft",
    "+10%% workforce per level in host city." },

{ HERO_DIPLOMAT,		HERO, -1, "HERO_DIPLOMAT",
    "Diplomat",
    "Host city cannot be attacked without incourring in the Wrath of the Everborns."
    "\nEach player can have at most one Diplomat." },

{ HERO_ASSASSIN,		HERO, -1, "HERO_ASSASSIN",
    "Assassin",
    "Once per turn, can attack a unit in a nearby area."
    "\nThe assassin does not require to be able to access the area, but the"
    "action costs the full turn." },

{ HERO_SCOUT,		HERO, -1, "HERO_SCOUT",
    "Scout",
    "The hero sees a larger area than most units, and can travel, along"
    " with its team, through forests and mountains." },

//=============================================================================
// .att: attack
{ ATT_DEATHG,	ATT,  9, "ATT_DEATHG",
    "Death Gaze",
    "" },

{ ATT_CHAOSG,	ATT,  6, "ATT_CHAOSG",
    "Chaos Gaze",
    "" },

{ ATT_STONEG,	ATT, 38, "ATT_STONEG",
    "Stoning Gaze",
    "" },

{ ATT_DISEVIL,	ATT, 99, "ATT_DISEVIL",
    "Dispel Evil",
    "" },

{ ATT_STONET,	ATT, 39, "ATT_STONET",
    "Stoning Touch",
    "" },

{ ATT_AP,	ATT,  0, "ATT_AP",
    "Armor Piercing",
    "All attacks ignore half of the target's Defense." },

{ ATT_ILLUSION,	ATT, 19, "ATT_ILLUSION",
    "Illusion",
    "Completely ignores the armor of the target."
    "\nIneffective against targets that are immune to illusions." },

{ ATT_FS,	ATT, 15, "ATT_FS",
    "First Strike",
    "Allows the unit to leap forward and deal damage before the target"
    " can respond." },

{ ATT_WALL,	ATT, 30, "ATT_WALL",
    "Wall Crusher",
    "Allows the unit to attack and destroy walls." },

{ ATT_LBRTH,	ATT, 99, "ATT_LBRTH",
    0, // changes Fire Breath to Lightning Breath
    0 },

//=============================================================================
// .cmd: commands
{ CMD_BUILD_ROAD,	CMD, 37, "CMD_BROAD",
    "Build Roads",
    "The unit can build roads."
    "\nNormal troops moving on roads move twice per turn instead of once." },

{ CMD_PURIFY,		CMD, 35, "CMD_PURIFY",
    "Purify",
    "The troop unit can remove corruption form a land area." },

{ CMD_OUTPOST,		CMD, 33, "CMD_OUTPOST",
    "Settle",
    "The unit can settle on a land area, creating a new city." },

{ CMD_PLANE_SHIFT,	CMD, 34, "CMD_PLANE_SHIFT",
    "Plane Shift",
    "The unit can shift to the opposite plane."
    "\nThe shift takes the whole turn." },

{ CMD_CATAPULT,		CMD,  7, "CMD_CATAPULT",
    "Siege",
    "The turn before a battle, the troop can build siege engine."
    "\nOnce built the siege engine can be moved only once before"
    " requiring being rebuilt."},

{ CMD_NODEMELD,		CMD, 99, "CMD_NODEMELD",
    "Meld with Node",
    "The unit can meld with a conquered magic Node, transferring the"
    " node raw power to the conrolling wizard." },

//=============================================================================
// .def: defensive specials
{ DEF_COLD,	DEF, 20, "DEF_COLD",
    "Immune to cold",
    "50 res vs cold" },

{ DEF_DEATH,	DEF, 21, "DEF_DEATH",
    "Immune to Death",
    "The unit is immune to Death spells, Fear and Life Stealing." },

{ DEF_FIRE,	DEF, 22, "DEF_FIRE",
    "Immune to Fire",
    "The unit is immune to fire attacks." },

{ DEF_ILLUSION,	DEF, 23, "DEF_ILLUSION",
    "Immune to Illusions",
    "The unit is immune to illusory attacks, warp reality, mind storm,"
    "and can see invisible units." },

{ DEF_MAGIC,	DEF, 24, "DEF_MAGIC",
    "Immune to Magic",
    "The unit is immune to all magical attacks and effects,"
    " with the exception of some Chaos spells." },

{ DEF_MISSILE,	DEF, 25, "DEF_MISSILE",
    "Immune to Missiles",
    "The unit is immune to small projectiles such as arrows and slings." },

{ DEF_POISON,	DEF, 26, "DEF_POISON",
    "Immune to Poison",
    "The unuit is immune to Poison attacks." },

{ DEF_STONING,	DEF, 27, "DEF_STONING",
    "Immune to stoning",
    "The unit is immune to petrify, stoning gaze, stoning touch." },

{ DEF_WEAPON,	DEF, 99, "DEF_WEAPON",
    "Weapon Immunity",
    "The unit is immune to non-magical, non-enchanted weapons." },

{ DEF_LASH,	DEF, 31, "DEF_LASH",
    "Large Shield",
    "The unit ignores one point of damage dealt by a projectile." },

{ DEF_FDODGE,	DEF, 12, "DEF_FDODGE",
    "Swift Didge",
    "..." },

{ DEF_NFS,	DEF, 32, "DEF_NFS",
    "Negate First Strike",
    "The unit cannot be attacked exploiting the First Strike ability." },

//=============================================================================
// .move: movement
{ MOVE_SWIM,	MOVE, -1, "MOVE_SWIM",
    0,
    0 },

{ MOVE_WALK,	MOVE, -1, "MOVE_WALK",
    0,
    0 },

{ MOVE_SAIL,	MOVE, -1, "MOVE_SAIL",
    0,
    0 },

{ MOVE_FLY,	MOVE, -1, "MOVE_FLY",
    0,
    0 },

{ MOVE_NOC,	MOVE, 99, "MOVE_NOC",
    "Non Corporeal",
    "The unit's movement or travel is almost unimpeded by terrain." },

{ MOVE_TELEPORT,MOVE, 99, "MOVE_TELEPORT",
    "Teleport",
    "The unit can instantly teleport anywhere in the battlefield." },

{ MOVE_MERGE,	MOVE, 99, "MOVE_MERGE",
    "Merge",
    "..." },

{ MOVE_MOUNTAIN,MOVE, 99, "MOVE_MOUNTAIN",
    "Mountaneer",
    "The unit and its team can travel through mountains;"
    " in battle, the unit moves unimpeded by rocks." },

{ MOVE_PATH,	MOVE, 99, "MOVE_PATH",
    "Pathfinder",
    "The unit and its team can travel twice on normal land, and cross"
    "mountains and forests." },

{ MOVE_FOREST,	MOVE, 99, "MOVE_FOREST",
    "Forester",
    "The unit and its team can travel through forests;"
    " in battle, the unit moves unimpeded by trees." },

{ MOVE_WIND,	MOVE, 99, "MOVE_WIND",
    "Windwalker",
    "The unit and its team can travel as if they fly." },

{ MOVE_STREAM,	MOVE, -1, "MOVE_STREAM",
    "Stream",
    "The unit can move in rivers and shallow waters." },

//=============================================================================
// .spc: specials and misc
{ SPC_FEAR,	SPC, 13, "SPC_FEAR",
    "Fear",
    "" },

{ SPC_INVISIBLE,SPC, 29, "SPC_INVISIBLE",
    "Invisible",
    "..." },

{ SPC_REGEN,	SPC, 36, "SPC_REGEN",
    "Regeneration",
    "..." },

{ SPC_HEALER,	SPC, 16, "SPC_HEALER",
    "Healer",
    "..." },

{ SPC_LUCKY,	SPC, 99, "SPC_LUCKY",
    "Lucky",
    "..." },

{ SPC_UNDEAD,	SPC, 99, "SPC_UNDEAD",
    "",
    "" },

{ SPC_SUNDEAD,	SPC, 99, "SPC_SUNDEAD",
    "",
    "" },

{ SPC_CATAPULT,	SPC, 99, "SPC_CATAPULT",
    "Siege Engine",
    "The unit has built a Siege Engine, and will remain in this state until"
    " moved." },

{ SPC_LBRINGER,	SPC, 30, "SPC_LBRINGHER",
    "Lightbringer",
    "..." },

{ SPC_HOLY,	SPC, 18, "SPC_HOLY",
    "Holy",
    "..." },

{ SPC_IMMOLATED,SPC, 28, "SPC_IMMOLATED",
    "Immolated",
    "..." },

{ SPC_RESIST,	SPC,  8, "SPC_RESIST",
    "Resist for all",
    "..." },

//=============================================================================
// .spl: spells
{ SPL_PSI_BLAST,SPL, 99, "SPL_PSI_BLAST",
    "Psionic Blast",
    "Once per turn, in battle, the unit can launch a psionic attack to a target"
    "troop unit." },

{ SPL_DOOM_BOLT,SPL, 11, "SPL_DOOM_BOLT",
    "Doom Bolt spell",
    "Once per turn, in battle, the unit can cast a Doom Bolt spell." },

{ SPL_FIRE_BOLT,SPL, 14, "SPL_FIRE_BOLT",
    "Fire Bolt",
    "Once per turn, in battle, the unit can cast a Fire Bolt spell." },

{ SPL_HEALING,	SPL, 17, "SPL_HEALING",
    "Healing",
    "Once per turn, in battle, the unit can cast a Healing spell." },

{ SPL_DEMON,	SPL, 99, "SPL_DEMON",
    "Summon demon",
    "Three times per turn, in battle, the unit can summon a Demon." },

{ SPL_WEB,	SPL, 99, "SPL_WEB",
    "Web",
    "Once per turn, in battle, the unit can entangle a enemy troop unit in a Web." },

{ SPL_LIFE,	SPL,  3, "SPL_LIFE",
    "Life Spellcaster",
    "" },

{ SPL_DEATH,	SPL,  2, "SPL_DEATH",
    "Death Spellcaster",
    "" },

{ SPL_CHAOS,	SPL,  1, "SPL_CHAOS",
    "Chaos Spellcaster",
    "" },

{ SPL_SORCERY,	SPL,  5, "SPL_SORCERY",
    "Sorcery Spellcaster",
    "" },

{ SPL_NATURE,	SPL,  4, "SPL_NATURE",
    "Nature Spellcaster",
    "" },

//=============================================================================
// .range_t: ammo types (an id, not a set of flags)
{ AMMO_ARROW,	AMMO, -1, "AMMO_ARROW",	0, 0 },

{ AMMO_SLING,	AMMO, -1, "AMMO_SLING",	0, 0 },

{ AMMO_JAVELIN,	AMMO, -1, "AMMO_JAVELIN",	0, 0 },

{ AMMO_ROCK,	AMMO, -1, "AMMO_ROCK",	0, 0 },

{ AMMO_CANNON,	AMMO, -1, "AMMO_CANNON",	0, 0 },

{ AMMO_V_MAGIC,	AMMO, -1, "AMMO_V_MAGIC",	0, 0 },

{ AMMO_B_MAGIC,	AMMO, -1, "AMMO_B_MAGIC",	0, 0 },

{ AMMO_F_MAGIC,	AMMO, -1, "AMMO_F_MAGIC",	0, 0 },

{ AMMO_E_MAGIC,	AMMO, -1, "AMMO_E_MAGIC",	0, 0 },


{ 0, 0, 0, 0, 0 }
};





/*=============================================================================
 * utype helper
 */
spcC* utypeC::Special(int id)
{
 for(spcC* s = unitSpcA; s->Id; s++)
 {
    short code = 0;
	 if(s->Flag & ATT ) code = Att;
    else if(s->Flag & CMD ) code = Cmd;
    else if(s->Flag & DEF ) code = Def;
    else if(s->Flag & MOVE) code = Move;
    else if(s->Flag & SPC ) code = Spc;
    else if(s->Flag & SPL ) code = Spl;

    if(code & s->Code)
	if(id) id--;
	else return s;
 }

 return 0;
}





/*=============================================================================
 * STANDARDS and ROLES
 */
stat_t unitStdA[] = {
    { 0, "STD_SPEARMEN",	"Spearmen",	"" },
    { 0, "STD_SWORDSMEN",	"Swordsmen",	"" },
    { 0, "STD_HALBERDIERS",	"Halberdiers",	"" },
    { 0, "STD_PIKEMEN",		"Pikemen",	"" },
    { 0, "STD_BOWMEN",		"Bowmen",	"" },
    { 0, "STD_CAVALRY",		"Cavalry",	"" },
    { 0, "STD_SHAMANS",		"Shamans",	"" },
    { 0, "STD_PRIESTS",		"Priests",	"" },
    { 0, "STD_MAGICIANS",	"Magicians",	"" },
    { 0, "STD_TRIREME",		"Trireme",	"" },
    { 0, "STD_GALLEY",		"Galley",	"" },
    { 0, "STD_WARSHIP",		"Warship",	"" },
    { 0, "STD_ENGINEERS",	"Engineers",	"" },
    { 0, "STD_SETTLERS",	"Settlers",	"" },
    { 0, "STD_CATAPULT",	"Catapult",	"" },
    { 0, "STD_HERO",		"Hero",		"" },
    { 0, "STD_CREATURE",	"Summoned creature", "" },
    { 0, 0, 0, 0 }
};

unitRoleT unitRoleA[] = {
    { ROLE_SPEARMEN,	"ROLE_SPEARMEN",	"Spearmen"	},
    { ROLE_SWORDSMEN,	"ROLE_SWORDSMEN",	"Swordsmen"	},
    { ROLE_FRONTLINE,	"ROLE_FRONTLINE",	"Frontline"	},
    { ROLE_BOWMEN,	"ROLE_BOWMEN",		"Bowmen"	},
    { ROLE_CAVALRY,	"ROLE_CAVALRY",		"Cavalry"	},
    { ROLE_CLERIC,	"ROLE_CLERIC",		"Cleric"	},
    { ROLE_MAGICIANS,	"ROLE_MAGICIANS",	"Magicians"	},
    { ROLE_TRIREME,	"ROLE_TRIREME",		"Trireme"	},
    { ROLE_GALLEY,	"ROLE_GALLEY",		"Galley"	},
    { ROLE_WARSHIP,	"ROLE_WARSHIP",		"Warship"	},
    { ROLE_ENGINEERS,	"ROLE_ENGINEERS",	"Engineers"	},
    { ROLE_SETTLERS,	"ROLE_SETTLERS",	"Settlers"	},
    { ROLE_ELITE,	"ROLE_ELITE",		"Elite"		},
    { ROLE_FLYER,	"ROLE_FLYER",		"Flyer"		},
    { ROLE_CATAPULT,	"ROLE_CATAPULT",	0,		},
    { ROLE_PLAYERMAGE,	"ROLE_PLAYERMAGE",	0, 		},
    { 0, 0, 0 }
};



/*=============================================================================
 * LOAD
 *
 * blockL is a dyn list of all available block that can be used to draw the unit
 * (usually the race BLOs)
 *
 * bldC is an array of pointers to blds available to be requirements (usually
 * race buildings)
 * if bldA is NULL no requirement parse will be made.
 */
static utypeC* unitLoad(char* s, char* e, blockC* blockL, bldC** bldA)
{
 utypeC* u = (utypeC*)calloc(1, sizeof(utypeC));
 if(u == NULL) return NULL;


 // standard vs. name
 if(*s == '"')			// non-standard unit
 {
    s = txtString(u->Name, s, sizeof(u->Name));
    s = txtSkipLine(s);
    s = txtString(u->Info, s, sizeof(u->Info));
    u->Std = 0xffff;		// non standard
 }
 else				// standard unit
 {
    stat_t* std;

    for(std = unitStdA; std->id && txtParse(std->id, s); std++);
    if(std->id == NULL)
    {
	sprintf(e, "invalid unit standard `%s`", txtWordTerminate(s));
	goto ERROR;
    }

    strcpy(u->Name, std->name);
    strcpy(u->Info, std->info);
    u->Std = std - unitStdA;
 }
 s = txtSkipLine(s);


 // role
 unitRoleT* role;
 for(role = unitRoleA; role->id && txtParse(role->id, s); role++);
 if(role->id == NULL)
 {
    sprintf(e, "invalid unit role `%s`", txtWordTerminate(s));
    goto ERROR;
 }
 u->Role = role;
 s = txtSkipLine(s);


 // texture file name
 s = txtString(u->Skin.Fn, s, sizeof(u->Skin.Fn));
 s = txtSkipLine(s);


 // stats
 s = txtReadStats(s, e, unitStatA, u->StatsStart);
 if(s == NULL) goto ERROR;
 s = txtSkipLine(s);


 // specials
 s = txtReadSpecials(s, e, unitSpcA);
 if(s == NULL) goto ERROR;
 for(spcC* spc = unitSpcA; spc->Id; spc++)
    switch(spc->Flag) {
	case HERO| SPC_ACTIVE: u->HeroBase |= spc->Code; break;
	case ATT | SPC_ACTIVE: u->Att |= spc->Code; break;
	case CMD | SPC_ACTIVE: u->Cmd |= spc->Code; break;
	case DEF | SPC_ACTIVE: u->Def |= spc->Code; break;
	case MOVE| SPC_ACTIVE: u->Move|= spc->Code; break;
	case SPC | SPC_ACTIVE: u->Spc |= spc->Code; break;
	case SPL | SPC_ACTIVE: u->Spl |= spc->Code; break;
	case AMMO| SPC_ACTIVE: u->RangeT = spc->Code; break;
	default: break;
    }
 s = txtSkipLine(s);

 // hero!! read hero's plus instead of requirements!
 if(u->HeroBase)
 {
    s = txtReadSpecials(s, e, unitSpcA);
    if(s == NULL) goto ERROR;
    for(spcC* spc = unitSpcA; spc->Id; spc++)
	if(spc->Flag == (HERO|SPC_ACTIVE))
	    u->HeroPlus |= spc->Code;

    u->Req = REQ_NON_BUILDABLE; // hero!
    //@@is this really necessary? u->Figures = 1;
    u->Food = 0;
 }
 else
 {
    if(bldA)
	if((u->Req = bldReadReq(s, e, bldA)) == -1) goto ERROR;
 }
 s = txtSkipLine(s);


 // body locations
 s = fleshRead(s, e, &u->Flesh, blockL);
 if(s == NULL) goto ERROR;
 s = txtSkipLine(s);


 // animator
 s = txtSkipLine(s);


 // some checkup
 if(u->Life < 1) u->Life = 1;
 if(!u->Move) u->Move = MOVE_WALK;
 if(u->Speed < 1) u->Speed = 1;
 if(u->Figures < 1) u->Figures = 1;
 if(u->Figures > 8) u->Figures = 8;

 if(u->Role->code == ROLE_CATAPULT) u->Req = REQ_NON_BUILDABLE;
 if(u->Role->code == ROLE_SETTLERS) u->Cmd |= CMD_OUTPOST;


 return u;

 ERROR:
 free(u);
 return 0;
}





/*=============================================================================
 * WRITE
 */
static char* unitWrite(char* w, utypeC* u, bldC** bldA)
{
 // standard vs name
 if(u->Std != ~0) w += sprintf(w, " %s\n", unitStdA[u->Std].id);
 else
    {
	w = txtWrString(w, u->Name); *w++ = '\n';
	w = txtWrString(w, u->Info); *w++ = '\n';    
    }


 // role
 w += sprintf(w, " %s\n", u->Role->id);


 // skin
 w = txtWrString(w, u->Skin.Fn); *w++ = '\n';


 // stats
 w = txtWriteStats(w, unitStatA, u->StatsStart);


 // specials
 {
    spcC *spc;
    int l = 0;

    txtClearSpecials(unitSpcA);
    for(spc = unitSpcA; spc->Id; spc++)
	{
	    int k = 0;
	    switch(spc->Flag) {
		case ATT: k = spc->Code & u->Att; break;
		case DEF: k = spc->Code & u->Def; break;
		case CMD: k = spc->Code & u->Cmd; break;
		case SPL: k = spc->Code & u->Spl; break;
		case SPC: k = spc->Code & u->Spc; break;
		case MOVE: k = spc->Code & u->Move; break;
		case HERO: k = spc->Code & u->HeroBase; break;
		case AMMO: if(u->Range && u->RangeT == spc->Code) k = 1; break;
	    }
	    if(k) l = 1, w += sprintf(w, " %s", spc->Id);
	}

    if(l) *w++ = '\n';
    else w += sprintf(w, " -\n");
 }


 if(u->HeroBase)	// hero pluses
 {
    spcC *spc;
    int l = 0;

    txtClearSpecials(unitSpcA);
    for(spc = unitSpcA; spc->Id; spc++)
	{
	    int k = 0;
	    switch(spc->Flag) {
		case HERO: k = spc->Code & u->HeroPlus; break;
	    }
	    if(k) l = 1, w += sprintf(w, " %s", spc->Id);
	}

    if(l) *w++ = '\n';
    else w += sprintf(w, " -\n");
 }
 else	// requirements
 {
    w = bldWriteReq(w, u->Req, bldA);
 }

 // body parts
 w = fleshWrite(w, &u->Flesh);


 // animator
 w += sprintf(w, " -\n");


 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * LOAD ALL
 */
utypeC* unitLoadAll(char* s, char* e, blockC* blockL, bldC** bldA)
{
 utypeC* hook = 0;
 utypeC* last = 0;

 while(s = txtSearch("[UNIT]", s))
 {
    utypeC* u = unitLoad(s, e, blockL, bldA);
    if(!u) goto ERROR;

    if(last) last->Next = u, last = u;
    else hook = last = u;
 }

 if(!hook)
 {
    sprintf(e, "no units found");
    goto ERROR;
 }


 //@@ ensure catapults for engineers
 return hook;

 ERROR:
 unitDestroyL(hook);
 return 0;
}





/*=============================================================================
 * WRITE ALL
 */
char *unitWriteAll(char *w, utypeC *hook, bldC **bldA)
{
 utypeC *u;

 for(u = hook; u; u = u->Next)
    {
	w += sprintf(w, "[UNIT]\n");
	w = unitWrite(w, u, bldA);
	*w++ = '\n';
    }

 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * CREATE MAGE PLAYER
 */
static void unitCreateMagePlayer(raceC* r)
{
 utypeC* mage = r->GetPlayerMage();

 mage->Req = REQ_NON_BUILDABLE;
 mage->HeroBase = 0;
 mage->HeroPlus = 0;

 // take stats and specials from spearmen
 utypeC* spearmen = r->GetSpearmen();
 mage->Att = spearmen->Att;
 mage->Cmd = spearmen->Cmd;
 mage->Def = spearmen->Def | DEF_MISSILE;
 mage->Move = spearmen->Move | MOVE_MOUNTAIN | MOVE_FOREST;
 mage->Spc = spearmen->Spc;
 mage->Spl = spearmen->Spl;

 // std: ok as it is

 //@@ not sure about this
 mage->Range = 5;
 mage->RangeT = AMMO_E_MAGIC;
 mage->Ammo = 8;

 // cost, g,f,m upkeep: ignored
 mage->Cost = 0;
 mage->Food = mage->Gold = mage->Mana = 0;

 mage->Endurance = 0;
 mage->Accuracy	= 0;
 mage->Figures	= 1;
 mage->Speed	= spearmen->Speed;

 mage->Melee	= spearmen->Melee;
 mage->Defense	= spearmen->Defense;
 mage->Resist	= 8 + spearmen->Resist;
 mage->Life	= 8 + spearmen->Life;

 mage->Breathe	= spearmen->Breathe;
 mage->Lifesteal= spearmen->Lifesteal;
 mage->Poison	= spearmen->Poison;
}





/*=============================================================================
 * CHECK and FIX HEROES
 */
static void unitFixUnitToHero(utypeC* h, utypeC* u)
{
 // add specials from original unit
 h->Att |= u->Att;
 h->Cmd |= u->Cmd;
 h->Def |= u->Def;
 h->Move |= u->Move;
 h->Spc |= u->Spc;
 h->Spl |= u->Spl;

 // Std: not used
 if(!h->Range && u->Range) h->Range = u->Range+3;
 h->RangeT = u->RangeT;
 if(!h->Ammo) h->Ammo = u->Ammo;

 // cost, g,f,m upkeep: ignored
 h->Cost = 0;
 h->Food =  h->Mana = 0;
 h->Gold = u->Gold * 2;


 if(!h->Endurance)	h->Endurance	= u->Endurance*2;
 if(!h->Accuracy)	h->Accuracy	= u->Accuracy;
 h->Figures = 1;
 if(h->Speed < 2)	h->Speed	= u->Speed+1;

 if(!h->Melee)		h->Melee	= u->Melee;
 if(h->Defense < 4)	h->Defense	= u->Defense+3;
 if(h->Resist < 3)	h->Resist	= u->Resist+2;
 if(h->Life < 5)	h->Life		= u->Life+4;

 if(!h->Breathe)	h->Breathe	= u->Breathe;
 if(!h->Lifesteal)	h->Lifesteal	= u->Lifesteal;
 if(!h->Poison)		h->Poison	= u->Poison;
}



static int unitCheckHeroes(raceC* r)
{
 int cnt = 0;
 for(utypeC* u = r->Units; u; u = u->Next)
    if(u->CanBeHero()) unitFixUnitToHero(u->Next, u);
    else if(u->HeroBase)
    {
	u->Req = REQ_NON_BUILDABLE;
	cnt++;
    }

 return cnt;
}





/*=============================================================================
 * LOAD EX RACE
 *
 * On error, non-zero is returned and unit list is NOT destroyed since it will
 * be destroyed along with the whole race.
 */
int unitLoadExRace(char* s, char* e, raceC* race)
{
 // load all units
 race->Units = unitLoadAll(s, e, race->Flesh, race->Blds);
 if(race->Units == NULL) return -1;


 // set utypeC.race to race!
 for(utypeC* u = race->Units; u; u = u->Next) u->Race = race;


 // check for required roles
 if(!race->GetSpearmen()) return sprintf(e, "spearmen required");
 if(!race->GetSettlers()) return sprintf(e, "settlers required");
 if(!race->GetPlayerMage()) return sprintf(e, "player mage required");

 // set player mage stats
 unitCreateMagePlayer(race);

 // check heroes
 if(unitCheckHeroes(race) < 2) return sprintf(e, "at least 2 heroes required");

 // add default body parts
 for(utypeC* u = race->Units; u; u = u->Next)
    fleshApplyDefaultParts(&u->Flesh, race->Flesh);

 // return successfully
 return 0;
}





/*=============================================================================
 * WRITE EX RACE
 */
char* unitWriteExRace(char *w, raceC* r)
{
 return unitWriteAll(w, r->Units, r->Blds);
}





/*=============================================================================
 * DESTROY LIST
 */
int unitDestroyL(utypeC *hook)
{
 utypeC *u;

 while(u = hook)
 {
    hook = u->Next;
    free(u);
 }
}





//EOF =========================================================================
