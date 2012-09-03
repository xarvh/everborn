/*
 CMD_UNIT.CPP		v0.05
 Unit commands.

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

#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "cmd.h"
#include "race.h"
#include "game.h"
#include "unit.h"
#include "map.h"
#include "city.h"





/*=============================================================================
 * SELECT
 */
static int cmdFSelect()
{
 //@@playsound "unit ready"
// if(cmdGetBrain() == self) cmdSSelect(BRAIN_LOCAL_BIT);
}

static int cmdGSelect()
{
 brainC* b = cmdGetBrain();
 unitC* u = arg1.i + game->Units;

 if(u->Cell->Battle)
    switch(*arg0.s) {
	case '+':	u->Select(b->Bit); break;
	case '-':	u->Deselect(b->Bit); break;
	default:	u->SelectOnly(b->Bit); break;
    }
 else if(u->Owner == b) if(u->Cell == b->Focus)
    switch(*arg0.s) {
	case '+':	
	case '-':
	    if(u->Team == b->SelectedTeam)
		u->Team = game->GetNewTeam();	// degroup
	    else 
	    {
		u->Team = b->SelectedTeam;	// group
		uolSet(&uolWAIT, cmdGetBrain(), false, 0, 0, 0);
	    }
	    break;
	default:
	    b->SelectedTeam = u->Team;
	    break;
    }
}

static cmdC cmdSelect(
    "select",			// name
    "U: unit",			// syntax
    "select a unit",
    cmdFSelect,			// exec()
    cmdGSelect,			// game()
    NULL			// check()
);





/*=============================================================================
 * GOTO
 * Landmap move
 */
static int cmdFGoto()
{
 //@@playsound "unit ackn"
}

static int cmdGGoto()
{
 brainC* b = cmdGetBrain();

 if(!b->Focus) return -1;

 if(arg1.i == b->Focus->X && arg2.i == b->Focus->Y)
    uolSet(&uolWAIT, cmdGetBrain(), false, 0, 0, 0);
 else uolSet(&uolTRAVEL, cmdGetBrain(), false, 0, 0, map(arg1.i, arg2.i, b->Focus->Z));
}

static cmdC cmdGoto(
    "goto",		// name
    "II: X Y",		// syntax
    "move selected units along the map",
    cmdFGoto,		// exec()
    cmdGGoto,		// game()
    NULL		// check()
);





/*=============================================================================
 * ROAD
 *
 */
static int cmdFRoad()
{
 //@@playsound "unit ackn"
}

static int cmdGRoad()
{
 brainC* b = cmdGetBrain();

 if(!b->Focus) return -1;

 if(arg1.i == b->Focus->X && arg2.i == b->Focus->Y)
    uolSet(&uolWAIT, cmdGetBrain(), false, 0, 0, 0);
 else uolSet(&uolROAD, cmdGetBrain(), false, 0, 0, map(arg1.i, arg2.i, b->Focus->Z));
}

static cmdC cmdRoad(
    "road",		// name
    "II: X Y",		// syntax
    "",
    cmdFRoad,		// exec()
    cmdGRoad,		// game()
    NULL		// check()
);
/*=============================================================================
 * SETTLE
 *
 */
static int cmdFSettle()
{
 //@@playsound "unit ackn"
}

static int cmdGSettle()
{
 uolSet(&uolSETTLE, cmdGetBrain(), false, 0, 0, arg1.s);
}

static cmdC cmdSettle(
    "settle",		// name
    "n: city name",	// syntax
    "settle to a new city",
    cmdFSettle,		// exec()
    cmdGSettle,		// game()
    NULL		// check()
);





/*=============================================================================
 * ATTACK
 * Battle attack
 */
static int cmdFAttack()
{
 //@@playsound "unit attack"
}

static int cmdGAttack()
{
 uobSet(&uobSHOOT, cmdGetBrain(), false, 0, 0, game->Units+arg1.i);
}

static cmdC cmdAttack(
    "attack",		// name
    "U: TargetUnit",	// syntax
    "attack an enemy unit",
    cmdFAttack,		// exec()
    cmdGAttack,		// game()
    NULL		// check()
);





/*=============================================================================
 * MELEE
 * Attack, ignoring ranged attack capabilities
 */
static int cmdFMelee()
{
 //@@playsound "unit attack"
}

static int cmdGMelee()
{
 uobSet(&uobMELEE, cmdGetBrain(), false, 0, 0, game->Units+arg1.i);
}

static cmdC cmdMelee(
    "melee",		// name
    "U: TargetUnit",	// syntax
    "engage a enemy unit in close combat",
    cmdFMelee,		// exec()
    cmdGMelee,		// game()
    NULL		// check()
);





/*=============================================================================
 * WAVE
 * Battle wave
 */
static int cmdFWave()
{
 //@@playsound "unit ackn"
}

static int cmdGWave()
{
 uobSet(&uobWAVE, cmdGetBrain(), false, arg1.i, 0, NULL);
}

static cmdC cmdWave(
    "wave",		// name
    "I: Angle",		// syntax
    "Chaaaaaaaaarge! [direeeeection!]",
    cmdFWave,		// exec()
    cmdGWave,		// game()
    NULL		// check()
);





/*=============================================================================
 * MOVE
 * Battle move
 */
static int cmdFMove()
{
 //@@playsound "unit ackn"
}

static int cmdGMove()
{
 bool append = (arg0.s[0] == '+');

 int escape = battleEscape(arg1.i, arg2.i);
 if(escape > -1)
    uobSet(&uobESCAPE, cmdGetBrain(), append, escape, 0, NULL);
 else
    uobSet(&uobMOVE, cmdGetBrain(), append, arg1.i, arg2.i, NULL);
}

static cmdC cmdMove(
    "move",		// name
    "II: X, Y",		// syntax
    "Move to X,Y",
    cmdFMove,		// exec()
    cmdGMove,		// game()
    NULL		// check()
);





/*=============================================================================
 * STOP
 */
static int cmdFEngage()
{
 //@@playsound "unit ackn"
}

static int cmdGEngage()
{
 bool append = (*arg0.s == '+');
 uobSet(&uobENGAGE, cmdGetBrain(), append, 0, 0, NULL);
}

static cmdC cmdEngage(
    "engage",			// name
    ":",			// syntax
    "engage enemies at will",
    cmdFEngage,			// exec()
    cmdGEngage,			// game()
    NULL			// check()
);





/*=============================================================================
 * HOLD
 */
static int cmdFHold()
{
 //@@playsound "unit ackn"
}

static int cmdGHold()
{
 bool append = (*arg0.s == '+');
 uobSet(&uobHOLD, cmdGetBrain(), append, arg1.i, 0, NULL);
}

static cmdC cmdHold(
    "hold",			// name
    "i: delay",			// syntax
    "hold ground",
    cmdFHold,			// exec()
    cmdGHold,			// game()
    NULL			// check()
);





/*=============================================================================
 * RAZE
 */
static int cmdGRaze()
{
 cellC* c = cmdGetBrain()->Focus;

 if(!c) return pe("no focus\n");
 if(!c->City) return pe("no city\n");
 if(!cmdGetBrain()->SelectedTeam) return pe("no team\n");

 c->City->Raze(cmdGetBrain());
}

static cmdC cmdRaze(
    "raze",			// name
    ":",			// syntax
    "destroy a city",
    0,				// exec()
    cmdGRaze,			// game()
    0				// check()
);





/*=============================================================================
 * SHIFT
 */
static int cmdFShift()
{
}

static int cmdGShift()
{
 brainC* b = cmdGetBrain();

 if(!b->Focus) return -1;

 int dest_z = (b->Focus->Z + 1) & 1;
 uolSet(&uolSHIFT, cmdGetBrain(), false, dest_z, 0, 0);
}

static cmdC cmdShift(
    "shift",			// name
    ":",			// syntax
    "shift plane",
    cmdFShift,			// exec()
    cmdGShift,			// game()
    0				// check()
);





/*=============================================================================
 * EXTENDED UNIT SYNTAX
 *
 * Checks only if a unit id is _valid_, ie won't cause seg faults.
 * Does NOT check wherether the unit is alive/active/used or not.
 */
static char* cmd_unitSyntax_re(argT* arg, char* s)
{
 char* end;

 arg->i = strtol(s, &end, 0);	// read the integer //
 if(s == end)
    {
	cmd_copy_str(arg->s, "no unit ID specifyed for '%s'");
	return 0;
    }

 if(arg->i < 0 || arg->i > GAME_MAX_UNITS-1)
    {
	cmd_copy_str(arg->s, "invalid '%s' unit ID");
	return 0;
    }

 return end;			// ok //
}

static cmdSyntaxC syntaxReUnit('U', cmd_unitSyntax_re);



//EOF =========================================================================
