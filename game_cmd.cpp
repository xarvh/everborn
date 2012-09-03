/*
 CMD_GAME.CPP		v0.20
 Generic game commands.

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

#include <string.h>
#include "colors.h"
#include "main.h"
#include "cmd.h"
#include "city.h"
#include "race.h"
#include "game.h"
#include "spell.h"





/*=============================================================================
 * GAME BUG
 */
#include "map.h"

static char gbug_syntax[] = "iii:";

static int cmdF_bug()
{
}

#include "utype.h"

static int cmdG_bug()
{
 cellC* c = map(arg1.i, arg2.i, arg3.i);
 pf("cell: %d %d %d\n", c->X, c->Y, c->Z);

// if(cellC* f = cmdGetBrain()->Focus)
//    pf("FOCUS %d %d %d\n", f->X, f->Y, f->Z);



// ENEMY BUG
// ---------
 raceC* r = raceGet(0);
// void initunits(raceC*, cellC*, brainC*);
// initunits(r, c, 0);
 unitNew(r->Units, c);
 unitNew(r->Units, c);

// AVL ITEMS BUG
// -------------
/* if(self->Focus && self->Focus->City)
 {
     cityC* c = self->Focus->City;
      pf("avl: %x\n", c->AvlItems(c->Queue, c->Queue->Type);
 }*/

// TRAVEL BUG
// ----------
// for(unitC* u = c->Unit; u; u = u->Next)
//    pf("%s: ActionStatus %d, ActionAngle %d, TimeAvl %d, TrA %d, TrC %d\n",
//	u->Action->Name, u->ActionStatus, u->ActionAngle,
//	u->TimeAvailable, u->TravelAngle, u->TravelCost);

// LAND BUG
// ----------
// pf("%s with %s\n", c->Land->Name, c->Plus->Name);
}

static cmdC cmdGBug(
    "g",			// name
    gbug_syntax,		// syntax
    0,
    cmdF_bug,			// exec()
    cmdG_bug,			// game()
    NULL			// check()
);





/*=============================================================================
 * Focus
 * Selects a cell.
 * All subsequent cmds will take effect on this cell.
 */
static int cmdSFocus(cellC** focus, brainC* brain)
{
 if(arg1.miss || arg2.miss || arg3.miss)
 {
    *focus = 0;
    brain->SelectedTeam = -1;
 }
 else
 {
    *focus = map(arg1.i, arg2.i, arg3.i);

    unitC* s;
    for(s = (*focus)->Unit; s && s->Team != brain->SelectedTeam; s = s->Next);
    if(!s)    // no units selected, select first team
	if((*focus)->Unit) brain->SelectedTeam = (*focus)->Unit->Team;
	else brain->SelectedTeam = -1;
 }
}



static int cmdFFocus()
{
 cmdSFocus(&self->LocalFocus, self);

//@@ follow focus?
// extern char mdZ;
// if(self->LocalFocus) mdZ = self->LocalFocus->Z;
}

static int cmdGFocus()
{
 cmdSFocus(&cmdGetBrain()->Focus, cmdGetBrain());
}

static cmdC cmdFocus(
    "focus",			// name
    "iii: X Y Z",		// syntax
    "select a cell",
    cmdFFocus,			// exec()
    cmdGFocus,			// game()
    NULL			// check()
);





/*=============================================================================
 * PICK
 */
static int cmdGPick()
{
 brainC* b = cmdGetBrain();

 int ppos = 0;	// pick position
 int rid = -1;	// race id

 for(int i = 0; cmd_args[i] && !cmd_args[i]->miss; i++)
 {
    char* s = cmd_args[i]->s;

    for(int r = 0; raceGet(r); r++)
	if(!strcasecmp(raceGet(r)->Id, s))
	    rid = r;

    for(pickC* p = picksHook; p; p = p->Next)
    	if(!strcasecmp(p->Id, s) && ppos < PICKS_MAX_CHOICE)
	    b->InitialPicks[ppos++] = p;
 }


 // clear remaining choices
 while(ppos < PICKS_MAX_CHOICE) b->InitialPicks[ppos++] = 0;

 // ensure a race
 b->Race = raceGet(rid);

/*
 pe("%s choices:\n", b->Name);
 for(int i = 0; i < PICKS_MAX_CHOICE; i++)
    if(b->InitialPicks[i]) pe("- %s\n", b->InitialPicks[i]->Name);
    else pe("x (random)\n");
 pe("race: %s\n", race? race->Name : "(random race)");
*/

 if(!b->Race) b->Race = raceGetRandom(game->Rnd.Raw16());




 // wait for all players
 for(int i = 0; i < GAME_MAX_BRAINS; i++)
    if(game->Brains[i].Status == BRAIN_INIT)
	if(!game->Brains[i].Race) return 1;


 // create dynamic list
 //@@ could be put before this point, just after the first 'ready' command is
 // issued
 game->BrainsHook = 0;
 for(brainC* b = game->Brains; b < game->BrainsEnd; b++) if(b->Status == BRAIN_INIT)
 {
    b->Next = game->BrainsHook;
    game->BrainsHook = b;
 }



 // assign picks
 pickAssign();

 // init everyone!
 for(int i = 0; i < GAME_MAX_BRAINS; i++)
    if(game->Brains[i].Status == BRAIN_INIT)
	game->Brains[i].InitGame();


 // init spell research
 for(brainC* b = game->BrainsHook; b; b = b->Next)
    for(realmC* r = realmsHook; r; r = r->Next)
    {
	// count number of minor and maior books
	int mi = b->MinorBooks(r);
	int ma = b->TotalBooks(r) - mi;
	if(!mi) continue;

	for(bookC* book = r->BooksHook; book; book = book->Next)
	    if(book->Pick->Owner == b)
		for(spellC* s = book->SpellsHook; s; s = s->BookNext)
		    s->ApplyStartingResearchBonus(mi, ma);
    }


 return 1;
}



static int cmdCPick()
{
 if(cmdGetBrain()->Race) return -1;
 return 0;
}

static cmdC cmdPick(
    "pick",				// name
    "nnnnnnn: p0 p1 p2 p3 p4 p5 p6 p7",	// syntax
    "select starting picks",
    NULL,				// exec()
    cmdGPick,				// game()
    cmdCPick				// check()
);





/*=============================================================================
 * RESEARCH
 */
static int cmdGResearch()
{
 brainC* b = cmdGetBrain();
 spellC* s = spellGet(arg1.s);
 bool o = (b != self);

 if(!s) return o?0:pe("%s: invalid spellname.\n", arg0.s);

 // does player have book?
 if(s->Book && s->Book->Pick->Owner != b)
    return o?0:pe("%s: %s is in the %s and you don't have it.\n",
	arg0.s, s->Name, s->Book->Name);

 // is spell unknown?
 if(s->ResearchLeft[b->Id] < 1)
    return o?0:pe("you already know this spell.\n");

 // set
 b->ResearchedSpell = s;
}


static int cmdCResearch()
{
 if(cmdGetBrain()->Race) return 0;
 return -1;
}


static cmdC cmdResearch(
    "research",				// name
    "N: spell",				// syntax
    "select spell to research",
    0,					// exec()
    cmdGResearch,			// game()
    cmdCResearch			// check()
);





/*=============================================================================
 * ALLOCATE
 */
static int cmdGAllocate()
{
 brainC* b = cmdGetBrain();

 unsigned sk = arg1.i;
 unsigned re = arg2.i;
 signed al = arg3.miss ? 0 : arg3.i;

 if(sk + re > 100) sk = re = 50;

 b->AllocateToSkill = sk;
 b->AllocateToResearch = re;

 if(al >= -100 && al <= 100)
    b->AlchemyManaToGold = al;
}



static cmdC cmdAllocate(
    "allocate",				// name
    "IIi: %Skill %Research Alchemy",	// syntax
    "select how much magic power to allocate to skill and research",
    0,					// exec()
    cmdGAllocate,			// game()
    0					// check()
);





/*=============================================================================
 * COLOR
 */
static int cmdGColor()
{
 brainC* b = cmdGetBrain();

 if(b->Status == BRAIN_INIT)
    if(game->IsPlayerColorAvailable(colorGet(arg1.s)))
	b->Color = colorGet(arg1.s);
}

static cmdC cmdColor(
    "color",				// name
    "S: color",				// syntax
    "select player color",
    NULL,				// exec()
    cmdGColor,				// game()
    NULL				// check()
);





/*=============================================================================
 * NAME
 */
static int cmdEName()
{
 cmd("set name %s", arg1.s);
}


static int cmdGName()
{
 brainC* b = cmdGetBrain();

 if(b->Status == BRAIN_UNUSED)    pf("%s is joining the game.\n", arg1.s);
 else if(strcmp(b->Name, arg1.s)) pf("%s renames as %s.\n", b->Name, arg1.s);

 b->Status = BRAIN_INIT;	// brain is initializing

 arg1.s[GAME_NAME_BF - 1] = 0;	// ensure correct termination
 strcpy(b->Name, arg1.s);	// copy name
}

static cmdC cmdName(
    "name",				// name
    "S: name",				// syntax
    "select player name",
    cmdEName,				// exec()
    cmdGName,				// game()
    NULL				// check()
);





/*=============================================================================
 * WHOAMI
 */
static int cmdFWhoami()
{
 if(!self) pf("no game\n");
 else	if(!self->Color) pf("%s\n", self->Name);
 else	pf("%s (%s)\n", self->Name, self->Color->Name);
}

static cmdC cmdWhoami(
    "whoami",			// name
    ":",			// syntax
    "who are you?",
    cmdFWhoami,			// exec()
    NULL,			// game()
    NULL			// check()
);





/*=============================================================================
 * READY
 */
static int cmdGReady()
{
 brainC* b = cmdGetBrain();
 b->Ready = true;
}

static cmdC cmdReady(
    "ready",				// name
    ":",				// syntax
    "ready for next turn?",
    NULL,				// exec()
    cmdGReady,				// game()
    NULL				// check()
);





/*=============================================================================
 * MSG
 */
static int cmdGMsg()
{
 pf("[%s]:%s\n", cmdGetBrain()->Name, arg1.s);
}

static cmdC cmdMsg(
    "msg",				// name
    "S: message",			// syntax
    "send a message to everyone",
    NULL,				// exec()
    cmdGMsg,				// game()
    NULL				// check()
);





/*=============================================================================
 * THINK
 */
static int cmdGThink()
{
 if(cmdGetBrainId() == 0) // only server can issue this!
    game->Think();
}

static cmdC cmdThink(
    "think",		// name
    ":",		// syntax
    NULL, // user cannot launch this!!!
    NULL,		// exec()
    cmdGThink,		// game()
    NULL		// check()
);



//EOF =========================================================================
