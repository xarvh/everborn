/*
 SPELL_CHAOS.CPP

*/

#include "main.h"
#include "spell.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"
#include "race.h"


/*=============================================================================
 * CHAOS
 */
static realmC sRealm_Chaos(
    REALM_CHAOS,
    "Chaos",
    "Destroy, mutate, change",
    1,.1,.1,
    true
    );




/*=============================================================================
 * DE CHAOS ARMONIA
 */
static bookC sBook_DeArmoniaChaoticii(
    &sRealm_Chaos,
    "dechaosarmonia",
    "DeChaosArmonia",//    "De Chaos Armonia",
    "\"Only Chaos can shape perfection into beauty.\"",
    true);


/*
 * CALL THE VOID
 */
static void spellCast_CallTheVoid(spellLaunchC* l)
{
 pe("%s has Called the Void at %d %d %d!\n",
    l->Caster->Name, l->X, l->Y, l->Z);
}


static spellC spell_CallTheVoid(
    &sBook_DeArmoniaChaoticii,
    "callthevoid",
    "Call the Void",
    "KaBoOm. Like, BIG.",
    "",
    &spellTypeMap,
    0,			// picture
    SPELL_RARE,
    0,			// upkeep cost
    1000,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_CallTheVoid
);



/*
 * CALL CHAOS
 */
static void spellCast_CallChaos(spellLaunchC* l)
{
 pe("%s has cast CallChaos!\n", l->Caster->Name);
}


static spellC spell_CallChaos(
    &sBook_DeArmoniaChaoticii,
    "callchaos",
    "Call Chaos",
    "@@Does a mess.",
    "",
    &spellTypeBattle,
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
    spellCast_CallChaos
);



/*
 * CHAOS SPAWN
 */
static void spellCast_ChaosSpawn(spellLaunchC* l)
{
 pe("%s has summoned a Chaos Spawn!\n", l->Caster->Name);
}


static spellC spell_ChaosSpawn(
    &sBook_DeArmoniaChaoticii,
    "chaosspawn",
    "Chaos Spawn",
    "Summons a Beho.. ehm, Chaos Spawn.",
    "",
    &spellTypeSummon,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    0,			// battle cost
    500,		// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_ChaosSpawn
);





//EOF =========================================================================
