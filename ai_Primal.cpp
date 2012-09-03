/*


*/
#include "ai.h"
#include "game.h"
#include "unit.h"
#include "city.h"
#include "race.h"
#include "map.h"
#include "spell.h"





/*=============================================================================
 *
 */
static void aiThink()
{
 for_AnyPlayer(p)
    p;

 for_AnyUnit(u)
    u;

 for_AnyMapCell(c)
    c;

 for_AnyBattle(b)
    b;

 for_AnyCity(c)
    c;
}



/*=============================================================================
 *
 */
static void aiPick()
{
 //
 // put any initialization here
 //

 // select picks
 cmd("pick");
}



static aiC ai(
    "Primal",
    "This is the first attempt at an AI",
    aiThink,
    aiPick);



//EOF =========================================================================
