/*
 SPELL_LIFE.CPP

*/

#include "main.h"
#include "spell.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"
#include "race.h"




/*
 global ench:	2B
 unit ench:	5B
 summon:	2B
 city ench:	5B
 battle		20B
 battle ench	25B


 recall hero		r350		b20		?360
 dispel evil		r450		b25		?450
 raise dead		r620		b35		?630
 holy word		r1700		b60		?1080
 mass healing		r1600		b50		?

 prayer			r800		b30		?780

 unicorns		r560	o250		u5	?550
 incarnation		r960	o500		u12	?

 summoning circle	r150	o50			?200
 heavenlylight		r740	o150		u2	?640
 stream of life		r1500	o300		u8	?1760

 true sight		r300	o100	b20	u2	?440
 lionheart		r880	o200	b40	u4	?880
 righteousness		r1120	o200	b40	u2

 planar seal		r500	o500		u5 


*/



/*
 * 
 *
static void spellCast_(spellLaunchC* l)
{
 pe("%s has cast %s on %s!\n", l->Spell->Name, l->Caster->Name, l->Unit->Name());
}


spellC spell_(
    &sBook_,
    "",		//id
    "",		//name
    "",		//info
    &spellType,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    0,		// research cost
    500,		// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_
);
*/



/*=============================================================================
 * HELPERS
 */
static void spellLife_EnchUnit(spellLaunchC* l)
{
 spellEnchUnit(l);
 //@@ spawn some Life gfx
}



static void spellLife_EnchCity(spellLaunchC* l)
{
 spellEnchCity(l);
 //@@ spawn some Life gfx
}



static void spellLife_EnchBattle(spellLaunchC* l)
{
 spellEnchBattle(l);
 //@@ spawn some Life gfx
}



static void spellLife_EnchGlobal(spellLaunchC* l)
{
 spellEnchGlobal(l);
}



static void spellLife_Summon(spellLaunchC* l)
{
 brainC* b = l->Caster;
 cellC* sc = b->SummoningCircle;

 // must have a valid summoning circle
 // and if player is not Conjurer, the circle must be in a city
 if(!sc || (!sc->City && !b->IsThe(Conjurer)))
 {
    if(b == self) pe("No summoning circle, spell failed!\n");
    return;
 }

 // create new unit
 unitC* u = unitNew(game->Creatures.GetUtype(l->Spell->Object), sc);
    u->Owner = b;

 //@@ event!
 pf("%s summons a %s.\n", b->Name, u->Name());
}





/*=============================================================================
 * LIFE
 */
static realmC sRealm_Life(
    REALM_LIFE,
    "Life",
    "Protect, enhance, punish",
    1,1,1,
    false
    );





/*=============================================================================
 * TRIUMPHUS ASTRORUM
 */
static bookC sBook_TriumphusAstrorum(
    &sRealm_Life,
    "triumphus_astrorum",
    "Triumphus A.",//    "Triumphus astrorum",
    "@@blah blah",
    true);


/*
 * CRUSADE
 */
spellC spell_Crusade(
    &sBook_TriumphusAstrorum,
    "crusade",
    "Crusade",
    "All normal troops are one rank higher.",
    "",
    &spellTypeGlobal,
    0,			// picture
    4,
    10,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    1100,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    GENCH_CRUSADE,	// object
    0,			// animation
    spellLife_EnchGlobal
);



/*
 * ARCH ANGEL
 */
spellC spell_ArchAngel(
    &sBook_TriumphusAstrorum,
    "archangel",
    "Arch Angel",
    "Summons an ArchAngel.",
    "",
    &spellTypeSummon,
    0,			// picture
    3,
    20,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    950,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    SPELL_OBJ_NONE, //SPELL_SUMMON_ARCHANGEL,	// object
    0,			// animation
    spellLife_Summon
);



/*
 * HOLY WORD
 */
static void spellCast_HolyWord(spellLaunchC* l)
{
 pe("%s has cast HolyWord at %d %d %d!\n", l->Caster->Name, l->X, l->Y, l->Z);
}


spellC spell_HolyWord(
    &sBook_TriumphusAstrorum,
    "holyword",
    "Holy Word",
    "Destroys Death creatures.",
    "",
    &spellTypeBattle,
    0,			// picture
    2,
    0,			// upkeep cost
    999,		// research cost
    60,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_HolyWord
);





/*
 * RIGHTEOUSNESS
 */
spellC spell_Righteousness(
    &sBook_TriumphusAstrorum,
    "righteousness",
    "Righteousness",
    "Gives immunity to Death and Chaos spells.",
    "",
    &spellTypeUnit,
    0,			// picture
    1,
    2,			// upkeep cost
    999,		// research cost
    40,			// battle cost
    200,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_RIGHTEOUSNESS,// object
    0,			// animation
    spellLife_EnchUnit
);





/*=============================================================================
 *
 */
static bookC sBook_DeSummoCaelo(
    &sRealm_Life,
    "desummocaelo",
    "de Summo Caelo",//    "",
    "@@blah blah",
    true);



/*
 * INVULNERABILITY
 */
spellC spell_Invulnerability(
    &sBook_DeSummoCaelo,
    "invulnerability",		//id
    "Invulnerability",		//name
    "Gives inv to nonmagic weapons, -2 to damage",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    4,
    5,			// upkeep cost
    999,		// research cost
    40,			// battle cost
    200,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_INVULNERABILITY,	// object?
    0,			// animation
    spellLife_EnchUnit
);





/*
 * CHARM OF LIFE
 */
spellC spell_CharmOfLife(
    &sBook_DeSummoCaelo,
    "charmoflife",		//id
    "Charm of Life",		//name
    "+25% do all unit's life (min 1).",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    3,
    10,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    1250,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    GENCH_CHARM_OF_LIFE, // object
    0,			// animation
    spellLife_EnchGlobal
);





/*
 * 
 */
static void spellCast_MassHealing(spellLaunchC* l)
{
 pe("%s has cast %s!\n", l->Spell->Name, l->Caster->Name);
}


spellC spell_MassHealing(
    &sBook_DeSummoCaelo,
    "masshealing",		//id
    "Mass Healing",		//name
    "Heals all frienldy units in the battlefield",	//info
    "",
    &spellTypeBattle,
    0,			// picture
    2,
    0,			// upkeep cost
    500,		// research cost
    50,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_MassHealing
);





/*
 * 
 */
spellC spell_StreamOfLife(
    &sBook_DeSummoCaelo,
    "streamoflife",		//id
    "Stream of Life",		//name
    "doubles city growth rate, unrest disappears.",		//info
    "",
    &spellTypeCity,
    0,			// picture
    1,
    8,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    300,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_STREAM_OF_LIFE,// object
    0,			// animation
    spellLife_EnchCity
);





/*=============================================================================
 *
 */
static bookC sBook_ChoriAngelorum(
    &sRealm_Life,
    "choriangelorum",
    "Chori Angelorum",//    "",
    "@@blah blah",
    true);



/*
 * HIGH PRAYER
 */
spellC spell_HighPrayer(
    &sBook_ChoriAngelorum,
    "highprayer",		//id
    "High Prayer",		//name
    "Gives a ridicolous bonus to all friendly units in battle",		//info
    "",
    &spellTypeBattle,
    0,			// picture
    4,
    0,			// upkeep cost
    999,		// research cost
    60,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    BENCH_HIGH_PRAYER,	// object
    0,			// animation
    spellLife_EnchBattle
);





/*
 * LIONHEART
 */
spellC spell_Lionheart(
    &sBook_ChoriAngelorum,
    "lionheart",		//id
    "Lionheart",		//name
    "+3 to Melee, resist, Hits and non arrow,non magic ranged attack",	//info
    "",
    &spellTypeUnit,
    0,			// picture
    3,
    4,			// upkeep cost
    999,		// research cost
    40,			// battle cost
    200,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_LIONHEART,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * PLANAR SEAL
 */
spellC spell_PlanarSeal(
    &sBook_ChoriAngelorum,
    "planarseal",		//id
    "Planar Seal",		//name
    "Forbids any travel between a Plane and the other.",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    2,
    5,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    150,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    GENCH_PLANAR_SEAL,	// object
    0,			// animation
    spellLife_EnchGlobal
);





/*
 * 
 */
spellC spell_Tranquillity(
    &sBook_ChoriAngelorum,
    "tranquillity",		//id
    "Tranquillity",		//name
    "Attempts o dispel any chaos spell asa is casted.",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    1,
    10,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    1000,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    GENCH_TRANQUILLITY,	// object
    0,			// animation
    spellLife_EnchGlobal
);





/*=============================================================================
 *
 */
static bookC sBook_MercuriSermo(
    &sRealm_Life,
    "mercurisermo",
    "Mercuri Sermo",//    "",
    "@@blah blah",
    false);



/*
 * LIFE FORCE
 */
spellC spell_LifeForce(
    &sBook_MercuriSermo,
    "lifeforce",		//id
    "Life Force",		//name
    "Attempts to dispell any death seppl as soon as it's cast.",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    5,
    10,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    1000,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    GENCH_LIFE_FORCE,	// object
    0,			// animation
    spellLife_EnchGlobal
);





/*
 * ANGEL
 */
spellC spell_Angel(
    &sBook_MercuriSermo,
    "angel",		//id
    "Angel",		//name
    "summon an Angel",		//info
    "",
    &spellTypeSummon,
    0,			// picture
    4,
    15,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    550,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE, //SPELL_SUMMON_ANGEL,	// object
    0,			// animation
    spellLife_Summon
);





/*
 * PLANAR TRAVEL
 */
spellC spell_PlanarTravel(
    &sBook_MercuriSermo,
    "planartravel",		//id
    "Planar Travel",		//name
    "Allows a unit to travel between the Planes.",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    3,
    5,			// upkeep cost
    680,		// research cost
    0,			// battle cost
    150,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_PLANAR_TRAVEL,// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * 
 */
spellC spell_TrueLight(
    &sBook_MercuriSermo,
    "truelight",		//id
    "True Light",		//name
    "malus to death, bonus to life",		//info
    "",
    &spellTypeBattle,
    0,			// picture
    1,
    0,			// upkeep cost
    100,		// research cost
    500,		// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    BENCH_TRUE_LIGHT,	// object
    0,			// animation
    spellLife_EnchUnit
);






/*=============================================================================
 *
 */
static bookC sBook_VenerisCollectionis(
    &sRealm_Life,
    "veneriscolectionis",
    "Veneris Collectionis",//    "",
    "@@blah blah",
    false);



/*
 * HOLY ARMS
 */
spellC spell_HolyArms(
    &sBook_VenerisCollectionis,
    "holyarms",		//id
    "Holy Arms",	//name
    "global Holy Weapon (+1 to hit)",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    5,
    10,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    900,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    GENCH_HOLY_ARMS,	// object
    0,			// animation
    spellLife_EnchGlobal
);






/*
 * PROSPERITY
 */
spellC spell_Prosperity(
    &sBook_VenerisCollectionis,
    "prosperity",		//id
    "Prosperity",		//name
    "+50% to gold from a city",		//info
    "",
    &spellTypeCity,
    0,			// picture
    4,
    2,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    250,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_PROSPERITY,	// object
    0,			// animation
    spellLife_EnchCity
);






/*
 * HEALING
 */
static void spellCast_Healing(spellLaunchC* l)
{
 pe("%s has cast %s on %s!\n", l->Spell->Name, l->Caster->Name, l->Unit->Name());
}


spellC spell_Healing(
    &sBook_VenerisCollectionis,
    "healing",		//id
    "Healing",		//name
    "each injury has XX% chance of healing.",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    1,
    0,			// upkeep cost
    999,		// research cost
    15,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_Healing
);





/*
 * ASTRAL GATE
 */
spellC spell_AstralGate(
    &sBook_VenerisCollectionis,
    "astralgate",		//id
    "Astral Gate",		//name
    "Allows planar travel in a city",		//info
    "",
    &spellTypeCity,
    0,			// picture
    3,
    5,			// upkeep cost
    100,		// research cost
    0,			// battle cost
    350,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_ASTRAL_GATE,	// object
    0,			// animation
    spellLife_EnchCity
);





/*
 * HEROISM
 */
spellC spell_Heroism(
    &sBook_VenerisCollectionis,
    "heroism",		//id
    "Heroism",		//name
    "A unit becomes Elite.",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    2,
    2,			// upkeep cost
    999,		// research cost
    20,			// battle cost
    100,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_HEROISM,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*=============================================================================
 *
 */
static bookC sBook_CanticumSolis(
    &sRealm_Life,
    "canticumsolis",
    "Canticum Solis",//    "",
    "@@blah blah",
    false);



/*
 * INCARNATION
 */
static void spellCast_Incarnation(spellLaunchC* l)
{
 pe("%s has cast %s!\n", l->Spell->Name, l->Caster->Name, l->Unit->Name());
}


spellC spell_Incarnation(
    &sBook_CanticumSolis,
    "incarnation",		//id
    "Incarnation",		//name
    "summons a great hero",	//info
    "",
    &spellTypeSummon,
    0,			// picture
    5,
    12,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_Incarnation
);





/*
 * BLESS
 */
spellC spell_Bless(
    &sBook_CanticumSolis,
    "bless",		//id
    "Bless",		//name
    "+3 vs any death/chaos",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    1,
    1,			// upkeep cost
    999,		// research cost
    8,			// battle cost
    40,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_BLESS,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * JUST CAUSE
 */
spellC spell_JustCause(
    &sBook_CanticumSolis,
    "justcause",		//id
    "Just Cause",		//name
    "+10 to fame, -1 unrest in every city",		//info
    "",
    &spellTypeGlobal,
    0,			// picture
    2,
    3,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    150,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    GENCH_JUST_CAUSE,	// object
    0,			// animation
    spellLife_EnchGlobal
);





/*
 * PRAYER
 */
spellC spell_Prayer(
    &sBook_CanticumSolis,
    "prayer",		//id
    "Prayer",		//name
    "+1 to rolls",		//info
    "",
    &spellTypeBattle,
    0,			// picture
    3,
    0,			// upkeep cost
    800,		// research cost
    30,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    BENCH_PRAYER,	// object
    0,			// animation
    spellLife_EnchBattle
);










/*=============================================================================
 *
 */
static bookC sBook_TestamentiumMartium(
    &sRealm_Life,
    "testamentiummartium",
    "Testamentium Martium",//    "",
    "@@blah blah",
    false);



/*
 * 
 */
spellC spell_AltarOfBattle(
    &sBook_TestamentiumMartium,
    "altarofbattle",		//id
    "Altar Of Battle",		//name
    "All units produced are Elite",		//info
    "",
    &spellTypeCity,
    0,			// picture
    5,
    5,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    300,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_ALTAR_OF_BATTLE,// object
    0,			// animation
    spellLife_EnchCity
);





/*
 * HEAVENLY LIGHT
 */
spellC spell_HeavenlyLight(
    &sBook_TestamentiumMartium,
    "heavenlylight",		//id
    "Heavenly Light",		//name
    "permanent true light",		//info
    "",
    &spellTypeCity,
    0,			// picture
    4,
    2,			// upkeep cost
    740,		// research cost
    0,			// battle cost
    150,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_HEAVENLY_LIGHT,// object
    0,			// animation
    spellLife_EnchCity
);





/*
 * HOLY ARMOR
 */
spellC spell_HolyArmor(
    &sBook_TestamentiumMartium,
    "holyarmor",		//id
    "Holy Armor",		//name
    "+2 to defense",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    1,
    2,			// upkeep cost
    999,		// research cost
    18,			// battle cost
    90,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_HOLY_ARMOR,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * DISPEL EVIL
 */
static void spellCast_DispelEvil(spellLaunchC* l)
{
 pe("%s has cast %s on %s!\n", l->Spell->Name, l->Caster->Name, l->Unit->Name());
}


spellC spell_DispelEvil(
    &sBook_TestamentiumMartium,
    "dispelevil",		//id
    "Dispel Evil",		//name
    "dispels death creature",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    3,
    0,			// upkeep cost
    450,		// research cost
    25,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_ENEMY,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_DispelEvil
);





/*
 * TRUE SIGHT
 */
spellC spell_TrueSight(
    &sBook_TestamentiumMartium,
    "truesight",		//id
    "True Sight",		//name
    "gives illusion immunity",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    2,
    2,			// upkeep cost
    300,		// research cost
    20,			// battle cost
    100,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_TRUE_SIGHT,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*=============================================================================
 *
 */
static bookC sBook_IovisLaude(
    &sRealm_Life,
    "iovislaude",
    "Iovis Laude",//    "",
    "@@blah blah",
    false);



/*
 * 
 */
spellC spell_Consecration(
    &sBook_IovisLaude,
    "consecration",		//id
    "Consecration",		//name
    "protects a city from chaos/death spells, city or globals",		//info
    "",
    &spellTypeCity,
    0,			// picture
    4,
    8,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_CONSECRATION,	// object
    0,			// animation
    spellLife_EnchCity
);





/*
 * INSPIRATIONS
 */
spellC spell_Inspirations(
    &sBook_IovisLaude,
    "inspirations",		//id
    "Inspirations",		//name
    "+50% production",		//info
    "",
    &spellTypeCity,
    0,			// picture
    5,
    2,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    350,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    CENCH_INSPIRATIONS, // object
    0,			// animation
    spellLife_EnchCity
);





/*
 * GUARDIAN SPIRIT
 */
spellC spell_GuardianSpirit(
    &sBook_IovisLaude,
    "guardianspirit",			//id
    "Guardian Spirit",			//name
    "Incorporeal, meld with node",	//info
    "",
    &spellTypeSummon,
    0,			// picture
    1,
    1,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    80,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    SPELL_SUMMON_GUARDIAN_SPIRIT,	// object
    0,					// animation
    spellLife_Summon
);





/*
 * PLANE SHIFT
 */
static void spellCast_PlaneShift(spellLaunchC* l)
{
 pe("%s has cast %s at %d %d!\n", l->Spell->Name, l->Caster->Name, l->X, l->Y);
}


spellC spell_PlaneShift(
    &sBook_IovisLaude,
    "planeshift",		//id
    "Plane Shift",		//name
    "shift troops to the other plane",
    "",
    &spellTypeMap,
    0,			// picture
    2,
    0,			// upkeep cost
    350,		// research cost
    0,			// battle cost
    125,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_PlaneShift
);





/*
 * RESURRECTION
 */
static void spellCast_Resurrection(spellLaunchC* l)
{
 pe("%s has cast %s!\n", l->Spell->Name, l->Caster->Name);
}


spellC spell_Resurrection(
    &sBook_IovisLaude,
    "resurrection",		//id
    "Resurrection",		//name
    "",		//info
    "",
    &spellTypeSummon,
    0,			// picture
    3,
    0,			// upkeep cost
    999,		// research cost
    0,			// battle cost
    250,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_ALIVE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_Resurrection
);





/*=============================================================================
 *
 */
static bookC sBook_SaturniusNuntius(
    &sRealm_Life,
    "saturniusnuntius",
    "Saturnius Nuntius",//    "",
    "@@blah blah",
    false);



/*
 * ASCENTION
 */
spellC spell_Ascention(
    &sBook_SaturniusNuntius,
    "ascention",		//id
    "Ascention",		//name
    "....",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    5,
    20,			// upkeep cost
    100,		// research cost
    500,		// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ELITE,
    UENCH_ASCENTION,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * ENDURANCE
 */
spellC spell_Endurance(
    &sBook_SaturniusNuntius,
    "endurance",		//id
    "Endurance",		//name
    "+2 to combat speed (endurance bonus?)",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    3,
    1,			// upkeep cost
    999,		// research cost
    6,			// battle cost
    30,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_ENDURANCE,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * RAISE FALLEN
 */
static void spellCast_RaiseFallen(spellLaunchC* l)
{
 pe("%s has cast %s!\n", l->Spell->Name, l->Caster->Name);
}


spellC spell_RaiseFallen(
    &sBook_SaturniusNuntius,
    "raisefallen",		//id
    "Raise Fallen",		//name
    "resurrects a dead unit",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    4,
    0,			// upkeep cost
    620,		// research cost
    35,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_DEAD,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_RaiseFallen
);





/*
 * HOLY WEAPON
 */
spellC spell_HolyWeapon(
    &sBook_SaturniusNuntius,
    "holyweapon",		//id
    "Holy Weapon",		//name
    "+1 to Hit",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    1,
    1,			// upkeep cost
    999,		// research cost
    10,			// battle cost
    50,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    UENCH_HOLY_WEAPON,	// object
    0,			// animation
    spellLife_EnchUnit
);





/*
 * STAR FIRES
 */
static void spellCast_StarFires(spellLaunchC* l)
{
 pe("%s has cast %s on %s!\n", l->Spell->Name, l->Caster->Name, l->Unit->Name());
}


spellC spell_StarFires(
    &sBook_SaturniusNuntius,
    "starfires",		//id
    "Star Fires",		//name
    "damages death and chaos",		//info
    "",
    &spellTypeUnit,
    0,			// picture
    2,
    0,			// upkeep cost
    999,		// research cost
    5,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_ENEMY,
    SPELL_TSTATUS_DEATH,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_StarFires
);





/*=============================================================================
 *
 *
static bookC sBook_PrecisTerrae(
    &sRealm_Life,
    "precisterrae",
    "Precis Terrae",//    "",
    "@@blah blah",
    false);

static bookC sBook_VoluminisLunae(
    &sRealm_Life,
    "voluminislunae",
    "Voluminis Lunae",//    "",
    "@@blah blah",
    false);
*/




//EOF =========================================================================
