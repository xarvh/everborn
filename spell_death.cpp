/*
 SPELL_DEATH.CPP

*/

#include "main.h"
#include "spell.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"
#include "race.h"


/*=============================================================================
 * DEATH
 */
static realmC sRealm_Death(
    REALM_DEATH,
    "Death",
    "Corrupt, twart, consume",
    .2,.2,.2,
    false
    );




/*=============================================================================
 * DOMINUS EX DAEMONIS
 */
static bookC sBook_DominusExDaemonis(
    &sRealm_Death,
    "dominus_ex_daemonis",
    "Dominus Ex.",//    "Dominus Ex Daemonis",
    "@@blah blah",
    true);


/*
 * DEATH WISH
 */
static void spellCast_DeathWish(spellLaunchC* l)
{
 pe("%s has cast DeathWish on unit %s!\n", l->Caster->Name, l->Unit->Name());
}


static spellC spell_DeathWish(
    &sBook_DominusExDaemonis,
    "deathwish",
    "Death Wish",
    "Slay a creature at -5.",
    "",
    &spellTypeUnit,
    0,			// picture
    SPELL_RARE,
    0,			// upkeep cost
    1000,		// research cost
    100,		// battle cost
    100,		// overland cost

    SPELL_TALLIANCE_ENEMY,
    SPELL_TSTATUS_ALIVE,
    0,			//@@ object?
    0,			// animation
    spellCast_DeathWish
);



/*
 * DEOMN LORD
 */
static void spellCast_DemonLord(spellLaunchC* l)
{
 pe("%s has summoned a Demon Lord!\n", l->Caster->Name);
}


static spellC spell_DemonLord(
    &sBook_DominusExDaemonis,
    "demonlord",
    "Demon Lord",
    "Summons a Demon Lord.",
    "",
    &spellTypeSummon,
    0,			// picture
    SPELL_UNCOMMON,
    20,			// upkeep cost
    1000,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_DemonLord
);



/*
 * ETERNAL NIGHT
 */
static void spellCast_EternalNight(spellLaunchC* l)
{
 pe("%s has cast Eternal Night!\n", l->Caster->Name);
}


static spellC spell_EternalNight(
    &sBook_DominusExDaemonis,
    "eternalnight",
    "Eternal Night",
    "All combats on both planes are always under Darkness.",
    "",
    &spellTypeGlobal,
    0,			// picture
    SPELL_UNCOMMON,
    20,			// upkeep cost
    1000,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_EternalNight
);





//EOF =========================================================================
