/*
 SPELL_COLORLESS.CPP

*/

#include "main.h"
#include "spell.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"
#include "race.h"


/*=============================================================================
 * SPELL: SUMMONING CIRCLE
 */
static void spellCast_SummoningCircle(spellLaunchC* l)
{
 cityC* c = map(l->X, l->Y, l->Z)->City;

 if(c)
 {
    l->Caster->SummoningCircle = c->Cell;
    pf("Summoning circle moved to %s\n", c->Name);
 }
 else	pe("Spell failed, no city!\n");
}


static spellC spell_SummoningCircle(
    0,
    "summoningcircle",
    "Summoning Circle",
    "[sets] the Summoning Circle in the selected city",
    "",
    &spellTypeCity,
    0,			// picture
    SPELL_NEUTRAL,
    0,			// upkeep cost
    150,		// research cost
    0,			// battle cost
    50,			// overland cost

    SPELL_TALLIANCE_FRIENDLY,
    SPELL_TSTATUS_NOSUMMONINGCIRCLE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_SummoningCircle
);





/*=============================================================================
 * SPELL: DISPEL MAGIC
 */
static void spellCast_DispelMagic(spellLaunchC* l)
{
 pe("dispelling magic on unit %d: %s %s\n",
    l->Unit-game->Units, l->Unit->Type->Race->Name, l->Unit->Type->Name);
}

static spellC spell_DispelMagic(
    0,
    "dispelmagic",
    "Dispel Magic",
    "Attempts to dispel a unit's enchantements.",
    "",
    &spellTypeUnit,
    0,			//picture
    SPELL_NEUTRAL,
    0,			// upkeep cost
    100,		// research cost
    10,			// battle cost
    50,			// overland cost

    SPELL_TALLIANCE_IGNORE,
    SPELL_TSTATUS_IGNORE,
    SPELL_OBJ_NONE,	// object
    0,			// animation
    spellCast_DispelMagic
);





//EOF =========================================================================
