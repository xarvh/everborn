/*
 SPELL_NATURE.CPP

*/

#include "main.h"
#include "spell.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"
#include "race.h"


/*=============================================================================
 * SORCERY
 */
static realmC sRealm_Nature(
    REALM_NATURE,
    "Nature",
    "",
    .1,.5,.1,
    true
    );



/*
 * XX
 *
static void spellCast_XX(spellLaunchC* l)
{
 pe("%s has cast XX at %x %d %d!\n",
    l->Caster->Name, l->X, l->Y, l->Z);
}


static spellC spell_XX(
    &sBook_N,
    "\",
    "XX",
    "@@descr",
    &spellType,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_XX
);
*/



/*=============================================================================
 *
 */
static bookC sBook_NMa1(
    &sRealm_Nature,
    "naM1",
    "Nature Ma1",
    "@@",
    true);



/*
 * GREAT WYRM
 */
static void spellCast_GreatWyrm(spellLaunchC* l)
{
 pe("%s has summoned a GreatWyrm!\n", l->Caster->Name);
}


static spellC spell_GreatWyrm(
    &sBook_NMa1,
    "greatwyrm",
    "Great Wyrm",
    "BIG snake",
    "",
    &spellTypeSummon,
    0,			// picture
    SPELL_RARE,
    20,			// upkeep cost
    1000,		// research cost
    0,			// battle cost
    100,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_GreatWyrm
);



/*
 * GAIA'S REIGN
 */
static void spellCast_GaiasReign(spellLaunchC* l)
{
 pe("%s has cast Gaia's Reign!\n", l->Caster->Name);
}


static spellC spell_GaiasReign(
    &sBook_NMa1,
    "gaiasreign",
    "Gaia's Reign",
    "Gaia's Blessing on all friendly cities.",
    "",
    &spellTypeGlobal,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_GaiasReign
);



/*
 * ICE STORM
 */
static void spellCast_IceStorm(spellLaunchC* l)
{
 pe("%s has cast Ice Storm at %x %d %d!\n",
    l->Caster->Name, l->X, l->Y, l->Z);
}


static spellC spell_IceStorm(
    &sBook_NMa1,
    "icestorm",
    "Ice Storm",
    "Rains ice on a map square.",
    "",
    &spellTypeMap,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    500,		// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_IceStorm
);





/*=============================================================================
 *
 */
static bookC sBook_NMi1(
    &sRealm_Nature,
    "naMi1",
    "Nature mi1",
    "@@",
    false);

/*
 * EarthElemental
 */
static void spellCast_EarthElemental(spellLaunchC* l)
{
 pe("%s has summoned a EarthElemental!\n",
    l->Caster->Name);
}


static spellC spell_EarthElemental(
    &sBook_NMi1,
    "eathelemental",
    "Earth Elemental",
    "@@descr",
    "",
    &spellTypeSummon,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_EarthElemental
);



/*
 * Web
 */
static void spellCast_Web(spellLaunchC* l)
{
 pe("%s has cast Web on %s!\n",
    l->Caster->Name, l->Unit->Name());
}


static spellC spell_Web(
    &sBook_NMi1,
    "web",
    "Web",
    "entangles.",
    "",
    &spellTypeUnit,
    0,			// picture
    SPELL_COMMON,
    0,			// upkeep cost
    40,			// research cost
    50,			// battle cost
    50,			// overland cost

    SPELL_TALLIANCE_ENEMY,
    SPELL_TSTATUS_ALIVE,
    0,			//@@ object?
    0,			// animation
    spellCast_Web
);



/*
 * Water Walking
 */
static void spellCast_WaterWalking(spellLaunchC* l)
{
 pe("%s has cast Water Walking on %s!\n",
    l->Caster->Name, l->Unit->Name());
}


static spellC spell_WaterWalking(
    &sBook_NMi1,
    "waterwalking",
    "Water Walking",
    "@@descr",
    "",
    &spellTypeUnit,
    0,			// picture
    SPELL_COMMON,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    0,			//@@ object?
    0,			// animation
    spellCast_WaterWalking
);



/*
 * PATHFINDING
 */
static void spellCast_Pathfinding(spellLaunchC* l)
{
 pe("%s has cast Pathfinding at %x %d %d!\n",
    l->Caster->Name, l->X, l->Y, l->Z);
}


static spellC spell_Pathfinding(
    &sBook_NMi1,
    "pathfinding",
    "Pathfinding",
    "Improves team movement.",
    "",
    &spellTypeUnit,
    0,			// picture
    SPELL_COMMON,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_ALIVE,
    0,			//@@ object?
    0,			// animation
    spellCast_Pathfinding
);





//EOF =========================================================================
