/*
 SPELL_SORCERY.CPP

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
static realmC sRealm_Sorcery(
    REALM_SORCERY,
    "Sorcery",
    "Control, manipulate, deceive",
    .1,.1,.8,
    true
    );




/*=============================================================================
 * CODEX DAEDALUM
 */
static bookC sBook_CodexDaedalus(
    &sRealm_Sorcery,
    "codexdaedalus",
    "Codex Daedalus",
    "@@blah blah",
    true);


/*
 * MIND STORM
 */
static void spellCast_MindStorm(spellLaunchC* l)
{
 pe("%s Mind Storms %s!\n",
    l->Caster->Name, l->Unit->Name());
}


static spellC spell_MindStorm(
    &sBook_CodexDaedalus,
    "mindstorm",
    "Mind Storm",
    "-5 to everything, no resist.",
    "",
    &spellTypeUnit,
    0,			// picture
    SPELL_RARE,
    0,			// upkeep cost
    1000,		// research cost
    50,			// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_ENEMY,
    SPELL_TSTATUS_ALIVE,
    0,			//@@ object?
    0,			// animation
    spellCast_MindStorm
);



/*
 * MASS INVISIBILITY
 */
static void spellCast_MassInvisibility(spellLaunchC* l)
{
 pe("%s has cast Mass Invisibility!\n", l->Caster->Name);
}


static spellC spell_MassInvisibility(
    &sBook_CodexDaedalus,
    "massinvisibility",
    "Mass Invisibility",
    "Makes all friendly troops invisible.",
    "",
    &spellTypeBattle,
    0,			// picture
    SPELL_UNCOMMON,
    0,			// upkeep cost
    1000,		// research cost
    150,		// battle cost
    0,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    0,			//@@ object?
    0,			// animation
    spellCast_MassInvisibility
);



/*
 * DISJUNCTION TRUE
 */
static void spellCast_DisjunctionTrue(spellLaunchC* l)
{
 pe("%s has cast Disjunction True!\n", l->Caster->Name);
}


static spellC spell_DisjunctionTrue(
    &sBook_CodexDaedalus,
    "disjunctiontrue",
    "Disjunction True",
    "Disjunction, three times more powerful.",
    "",
    &spellTypeGlobal,
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
    spellCast_DisjunctionTrue
);





//EOF =========================================================================
