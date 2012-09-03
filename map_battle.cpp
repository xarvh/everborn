/*
 MAP_BATTLE.CPP		v0.30
 Battle stuff.

 Copyright (c) 2004-2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include <math.h>

#include "main.h"

#include "map.h"
#include "game.h"
#include "city.h"
#include "unit.h"

#include "particles.h"





/*=============================================================================
 * CITY AFTERMATH
 * What happens to a city after a battle?
 */
void battleCityAftermath(cellC* cell)
{
 cityC* city = cell->City;

 // who's left on the cell?
 long players = 0;
 for(unitC* u = cell->Unit; u; u = u->Next) if(u->IsAlive())
    if(u->Owner) players |= u->Owner->Bit;
    else players |= BRAIN_NEUTRAL;

 // none survived, nothing happens
 if(!players) return;

 // raiders?
 if(players & BRAIN_NEUTRAL)
 {
    city->IsConqueredBy(0);
    return;
 }

 // successfully defended
 if(city->Owner)
    if(city->Owner->Bit & players) return;


 // either no owner, or owner not present
 // biggest army takes all
 int army_power[GAME_MAX_BRAINS];
 for(int i=0; i<GAME_MAX_BRAINS; i++) army_power[i] = 0;

 for(unitC* u = cell->Unit; u; u = u->Next) if(u->IsAlive())
    if(u->Owner) army_power[u->Owner->Id] += u->Figures();

 //start from a random player to evenly resolve draws
 int best_player = game->Rnd(GAME_MAX_BRAINS);
 for(int i=0; i<GAME_MAX_BRAINS; i++)
    if(army_power[i] > army_power[best_player]) best_player = i;

 city->IsConqueredBy(game->Brains + best_player);
}





/*=============================================================================
 * ESCAPE
 * Tells if a cell can be used to escape and if so in which direction it leads.
 */
int battleEscape(int x, int y)
{
 int up = BATTLE_H/4;
 int dn = BATTLE_H - up;

 if(x == 0)		// left border, West
	 if(y < up)	return 5;
    else if(y < dn)	return 6;
    else		return 7;

 if(x == BATTLE_W-1)	// right border, East
	 if(y < up)	return 3;
    else if(y < dn)	return 2;
    else		return 1;


 int lf = BATTLE_H/4;
 int ri = BATTLE_H - lf;

 if(y == 0)		// lower border, South
	 if(x < lf)	return 5;
    else if(x < ri)	return 4;
    else		return 3;

 if(y == BATTLE_H-1)	// upper border, North
	 if(x < lf)	return 7;
    else if(x < ri)	return 0;
    else		return 1;

 return -1;
}





/*=============================================================================
 * BORDER
 * Determine whether a coordinate couple refers to the border cells or is
 * outside the battlefield.
 */
bool battleIsBorder(int x, int y)
{
 if(x == BATTLE_W-1) return true;
 if(y == BATTLE_H-1) return true;
 if(x == 0) return true;
 if(y == 0) return true;
 return false;
}

bool battleIsOOBorder(signed x, signed y)
{
 if(x > BATTLE_W-1) return true;
 if(y > BATTLE_H-1) return true;
 if(x < 0) return true;
 if(y < 0) return true;
 return false;
}





/*=============================================================================
 * BattleCell
 */
bcC* battleC::BC(float x, float y)
{
 return BC((signed)x, (signed)y);
}

bcC* battleC::BC(signed x, signed y)
{
 while(x < 0) x += BATTLE_W;
 while(y < 0) y += BATTLE_H;

 return SubCellA + (x % BATTLE_W) + (y % BATTLE_H) * BATTLE_W;
}



/*=============================================================================
 * TURN
 */
void battleC::Turn()
{
 Time = 0;
 TimeEnd = BATTLE_DEFAULT_DURATION;
}





/*=============================================================================
 * IS POSTPONED
 */
bool battleC::IsPostponed()
{
 return PrevCheck && Time >= TimeEnd;
}





/*=============================================================================
 * THINK
 */
void battleC::Think()
{
 if(IsPaused()) return;
 if(IsPostponed()) return;

 // advance time
 Time++;

 // particles
 particleC* next;
 for(particleC* p = ParticlesHook; p; p = next)
 {
    next = p->Next;
    p->Think();	// may actually delete the particle!
 }


 // set pathing grid
 umSetGrid(this);

 // units
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->IsAlive())
    u->Think();

 // check battle status
 bool check = Cell->CheckBattle();

 // check && !PrevCheck -> new units entered, restart battle
 // !check && PrevCheck -> battle just finished, start counting for aftermath
 if(!check && PrevCheck) TimeCheck = Time;
 PrevCheck = check;

 // is battle finished?
 if(!check && Time - TimeCheck > BATTLE_AFTERMATH_TIME)
    Cell->CloseBattle();
}




/*=============================================================================
 * CHECK BATTLE
 *
 * Tells whether on a cell there are units engaging each other.
 */
bool cellC::CheckBattle(long players)
{
 if(!Battle && City)
    if(City->Owner)	players |= City->Owner->Bit;
    else		players |= BRAIN_NEUTRAL;

 for(unitC* u = Unit; u; u = u->Next) if(u->IsAlive())
    if(u->Owner) players |= u->Owner->Bit;
    else players |= BRAIN_NEUTRAL;

 for(brainC* b = game->Brains; b < game->BrainsEnd; b++) if(b->Bit & players)
    if(b->EngageMask & players) return true;

 return false;
}



bool cellC::CheckBattle()
{
 return CheckBattle(0);
}




/*=============================================================================
 * PAUSES
 */
/*void battleC::PauseToggle(int id)
{
 long mask = 1<<id;

 if(PauseMask & mask)	PauseMask &= ~mask;
 else			PauseMask |= mask;
}
*/


long battleC::IsPaused()
{
 return Cell != game->ActiveBattle;

/* unsigned long mask = 0;

 // you may have units fielded
 for(unitC* u = Cell->Unit; u; u = u->Next)
    if(u->Owner) mask |= u->Owner->Bit;

 // or just a city
 if(Cell->City && Cell->City->Owner)
    mask |= Cell->City->Owner->Bit;

 // discard ready players
 for(brainC* b = game->Brains; b < game->BrainsEnd; b++)
    if(b->Ready) mask &= ~b->Bit;

 return Cell->Battle->PauseMask & mask;
*/
}





/*=============================================================================
 * CONSTRUCTOR
 */
battleC::battleC(cellC* c)
{
 if(c->Battle) throw "battleC::battleC: cell already has a battle";

 Cell = c;	Cell->Battle = this;
 ParticlesHook = 0;
 Time = 0;
 TimeEnd = BATTLE_DEFAULT_DURATION;
// PauseMask = 0; //~0;
 PrevCheck = true;

 Rnd.Seed = c->X + c->Y + c->Land->Type;

 GenerateMap();
}



/*=============================================================================
 * DESTRUCTOR
 */
battleC::~battleC()
{
 // conquered city?
 if(Cell->City) battleCityAftermath(Cell);

 // remove dead units and reset unit orders
 unitC* u = Cell->Unit;
 while(u)
    if(u->IsAlive())
    {
	u->LandOrders.Path.Clear();
	u->LandOrders.OrderDefault()->SetAction(u);
	u = u->Next;
    }
    else
    {
	unitC* n = u->Next;
	u->Remove();
	u = n;
    }

 // destroy particles
 while(ParticlesHook) delete ParticlesHook;

 // detatch battle
 Cell->Battle = 0;


 //@@ if self has view, event with battle aftermath
 //@@ if self has focus, open popup with battle aftermath
}



/*=============================================================================
 * CELL
 */
void cellC::SpawnBattle()
{
 if(!Battle) new battleC(this);
}

void cellC::CloseBattle()
{
 if(Battle) delete Battle;
}



//EOF =========================================================================
