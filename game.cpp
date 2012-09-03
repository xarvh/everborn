/*
 GAME.CPP		v0.22

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
#include <stdlib.h>
#include <stdio.h>

#include "map.h"

#include "cmd.h"
#include "main.h"
#include "boxgui.h"
#include "game.h"
#include "unit.h"
#include "city.h"
#include "race.h"
#include "utype.h"
#include "spell.h"

#include "colors.h"
#include "data.h"
#include "txt.h"
#include "net.h"



/*=============================================================================
 * GAME
 */
brainC* self = 0;
gameC* game = 0;

void gameClose()
{
 if(game) delete game;
 game = 0;
}





gameC::gameC(int self_id, long seed, int mapw, int maph)
{
 for(char* z = ZeroStart; z < ZeroEnd; z++) *z = 0x00;
 TravelsHook = 0;
 HerosHook = 0;
 ActiveBattle = 0;

 if(self_id >= 0) self = Brains + self_id;

 Seed = Rnd.Seed = seed;

 for(brainC* b = Brains; b < BrainsEnd; b++)
 {
    b->Id = b - Brains;
    b->Bit = 1<<b->Id;
    b->Status = BRAIN_UNUSED;
    b->Ready = false;
 }



 // reset research values for spells
 spellResetAll();
    

 // create map
 Map.Allocate(mapw, maph);
}


gameC::~gameC()
{
 while(TravelsHook) delete TravelsHook;
 while(HerosHook) delete HerosHook;
 self = 0;
}





void gameC::Start()
{
 Map.Generate(Seed);
 Turn = 1;
 bOpen("pick");
}



unsigned gameC::Roll(int str)
{
 return Roll(str, 3);
}

unsigned gameC::Roll(int str, int chance)
{
 int cnt = 0;
 for(int i = 0; i < str; i++)
    if(game->Rnd(10) <= chance) cnt++;
 return cnt;
}


long gameC::GetNewTeam()
{
 return ++UnitTeam;
}





/*=============================================================================
 * HELPERS
 */
cellC* map(signed x, signed y, char z)	{ return game->Map(x, y, z); }
bool gameIsAvailable()			{ return game && game->Turn; }

bool gameC::IsPlayerColorAvailable(colorC* c)
{
 if(c == colorNeutral()) return false;	// reserved!
 
 for(brainC* p = Brains; p < BrainsEnd; p++)
    if(p->Color == c) return false;
 return true;
}





/*=============================================================================
 * CHECK READY
 */
bool gameC::CheckReady()
{
 unsigned brains_total = 0;
 unsigned brains_ready = 0;
 brainC* b;

 for(b = Brains; b < BrainsEnd; b++)
    if(b->Status != BRAIN_UNUSED)
	{
	    brains_total++;
	    if(b->Ready) brains_ready++;
	}

 if(!brains_total) return false;	// server has no players yet
 if(brains_ready != brains_total) return false;

 return true;
}





/*=============================================================================
 * THINK
 */
static void gameSetFog()
{
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++)
 {
    c->ViewMask = BRAIN_NEUTRAL;
    c->DetectMask = BRAIN_NEUTRAL;
 }

 for(cellC* c = game->Map.C; c < game->Map.Cend; c++)
 {
    long view = 0;
    long plane = 0;
    long detect = 0;
    long longview = 0;
    for(unitC* u = c->Unit; u; u = u->Next) if(u->IsAlive() && u->Owner)
    {
	view |= u->Owner->Bit;
	if(u->Type->Cmd & CMD_PLANE_SHIFT) plane |= u->Owner->Bit;
	//@@ if(LIFE_PLANE_TRAVEL) plane |= u->Owner->Bit;
	if(u->CanDetect()) detect |= u->Owner->Bit;
	if(!c->Battle) if(u->BACIView()) longview |= u->Owner->Bit;
    }

    if(c->City && c->City->Owner)
    {
	view |= c->City->Owner->Bit;
	if(c->City->CanDetect()) detect |= c->City->Owner->Bit;
	if(!c->Battle) if(c->City->BACIView()) longview |= c->City->Owner->Bit;
    }

    cellC* v;
    if(view) for(int k = 0; v = c->SMAC(k); k++)
    {
	v->KnowMask |= view;
	v->ViewMask |= view;
	v->DetectMask |= detect;
    }

    if(longview) for(int k = 0; v = c->BACI(k); k++)
    {
	v->KnowMask |= longview;
	v->ViewMask |= longview;
    }

    if(plane) for(int k = 0; v = c->Mirror()->SMAC(k); k++)
    {
	v->KnowMask |= plane;
	v->ViewMask |= plane;
    }
 }
}


static void gameAllUnitsThink()
{
 // exec movements
 travelC* n;
 for(travelC* t = game->TravelsHook; t; t = n)
 {
    n = t->Next;
//    pf("exec tr %x (team %d) Stat: %d\n", t, t->Team->Team, t->Status);
    if(t->Think()) delete t;
 }

 // overland think
 for(unitC* u = game->Units; u < game->UnitsEnd; u++) if(u->IsAlive())
    if(!u->Cell->Battle)
    {
	u->X = -1;	// mark unit battle position as uninitialized
	u->Think();
    }

 // lay units or spawn battles
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++) if(c->Unit)
    if(c->Battle) c->Battle->LayUnits();
    else if(c->CheckBattle()) c->SpawnBattle();
}





static void gameActivateBattle(cellC* c)
{
 game->ActiveBattle = c;

 // get involved players
 long players = 0;

 if(c->City && c->City->Owner) players |= c->City->Owner->Bit;
 if(c->Neutral && c->Neutral->Owner) players |= c->Neutral->Owner->Bit;

 for(unitC* u = c->Unit; u; u = u->Next) if(u->Owner)
    players |= u->Owner->Bit;

 // force them into the battle
 for(brainC* b = game->BrainsHook; b; b = b->Next) if(b->Bit & players)
 {
    b->Focus = b->LocalFocus = c;
    if(b == self) cmd("menu battle");
 }
}





static void gameThink_DuringTurn()
{
 // active battle?
 bool runAB =
    game->ActiveBattle
	&&
    game->ActiveBattle->Battle
	&&
    !game->ActiveBattle->Battle->IsPostponed()
	;



 // map
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++)
 {
    // run cities:
    if(c->City)
	if(c->City->TotalPop < 1)
	{
	    //@@ "city deserted"
	    delete c->City;
	}
	else
	{
	    c->City->ProdCheck();	// required to compute income!
	}

    // run battles
    if(c->Battle && !runAB)
	if(!c->Battle->IsPostponed())
	    gameActivateBattle(c), runAB = true;
 }

 // active battle
 if(runAB)
    game->ActiveBattle->Battle->Think();

 // units
 gameAllUnitsThink();

 // view
 gameSetFog();
}




static void gameThink_ToNextTurn()
{
 // make units complete pending commands
 for(int i = 0; i < 6; i++) gameAllUnitsThink();


 // Map
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++)
 {
    if(c->City) c->City->Think();
    if(c->Battle) c->Battle->Turn();
 }


 // Units
 for(unitC* u = game->Units; u < game->UnitsEnd; u++)
    if(u->IsAlive()) u->NextTurn();

 // Players
 for(brainC* b = game->Brains; b < game->BrainsEnd; b++)
    if(b->Status == BRAIN_ACTIVE) b->Think();


 //@@ garbage collection:
 //@@ dead heroes are never removed, so
 //@@ if a player has too many dead heros, remove some


 // turn!
 game->Turn++;
 pf("Turn %d\n", game->Turn);
}





static bool gameNoPendingBattles()
{
 // check for pending battles
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++)
    if(c->Battle && !c->Battle->IsPostponed()) return false;
 return true;
}



void gameC::Think()
{
 if(gameNoPendingBattles() && CheckReady())
 {
    // reset all ready flags
    for(brainC* b = Brains; b < BrainsEnd; b++) b->Ready = 0;

    if(gameIsAvailable())	gameThink_ToNextTurn();
    else			Start();
 }

 if(gameIsAvailable())
    gameThink_DuringTurn();
}





/*=============================================================================
 * REQUEST
 */
int gameRequest()
{
 netClientPostRequest(net_connections, "game", "game");
}



static int gamePackRead(connectionC *sv, netReqC *req)
{
 char ebf[200];
 char *c;

 c = (char*)req->data;
 // ensure termination
 c[req->size - 1] = '\0';


 txtStrip(c);
 if(game->Read(c, ebf))
    {
	pe("%s\n", ebf);
	return -1;
    }

 return 0;
}



static netPwtT *gamePackWrite(netReqC *header)
{
 char *end;
 netPwtT *p;

 p = (netPwtT*)malloc(50000 + sizeof(netPwtT));
 end = game->Write(p->data);
 p->size = end - (char *)p->data;

 return p;
}


static int gameError(connectionC *sv, netReqC *req)
{
}

static netPackC netPackGame("game", gameError, gamePackRead, gamePackWrite);





//EOF =========================================================================
