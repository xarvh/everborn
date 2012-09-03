/*
 CITY.CPP		v0.11
 City misc.

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
#include "main.h"

#include "game.h"
#include "race.h"
#include "map.h"

#include "city.h"
#include "unit.h"
#include "utype.h"

#include "counters.h"





/*=============================================================================
 * RAZE
 */
static unitC* getNthUnitIn(cellC* c, brainC* o, unsigned n)
{
 int prevn;

 while(1)
 {
    prevn = n;
    for(unitC* u = c->Unit; u; u = u->Next) if(u->Owner == o && u->IsAlive())
	if(n-- == 0) return u;

    if(n == prevn) return 0;	// no more units
 }
}



void cityC::Raze(brainC* b)
{
 //
 //@@ b->evil++

 // create the mob
 for(int i = 0; i < TotalPop; i++)
    unitNew(Race->GetSpearmen(), Cell);

 // spawn battle
 Cell->SpawnBattle();


 // team has 20 rounds to destroy the mob
 for(int t = 0; t < 20; t++)
 {
    int cnt;

    // troops attack mob
    cnt = 0;
    for(unitC* u = Cell->Unit; u; u = u->Next)
	if(u->IsAlive() && u->Owner == b)
	    if(unitC* targ = getNthUnitIn(Cell, 0, cnt++))
		u->InflictMeleeDamage(targ);

    // mob attack troops
    cnt = 0;
    for(unitC* u = Cell->Unit; u; u = u->Next)
	if(u->IsAlive() && u->Owner == 0)
	    if(unitC* targ = getNthUnitIn(Cell, b, cnt++))
		u->InflictMeleeDamage(targ);
 }


 bool citySurvived = getNthUnitIn(Cell, 0, 0);


 if(citySurvived)	// mob wins
 {
    // count survivors
    int cnt = 0;
    for(unitC* u = Cell->Unit; u; u = u->Next)
	if(u->IsAlive() && u->Owner == 0) cnt++;

    // reduce city pop according to damage
    TotalPop = cnt;
    Pop /= 2;

    //@@ b->moron++
    Owner = 0;	// make city indipendent

    //@@popup to team owner
    pf("\n"); pf("\n"); pf("\n");
    pf("%s proclamed indipendent!\n", Name);
    //@@event to players with view "city proclamed indipendent"
 }
 else			// team wins, city razed
 {
    //@@ plunder

    //@@popup to team owner "city raze, population slaughtered or scattered"
    //@@event to players with view "city was destroyed by wizard"
    pf("\n"); pf("\n"); pf("\n");
    pf("%s destroys %s.\n", b->Name, Name);
 }

 // remove units
 Cell->CloseBattle();

 // remove city
 if(!citySurvived) delete this;
}





/*=============================================================================
 * VISIBLE BLDS
 *
 * Finds buildings that are not required by any other building.
 * Those are usually the blds that must be displayed and can be destroyed.
 *
 * Also, Wall is always excluded as it has special display.
 */
long cityC::VisibleBlds()
{
 long exclude = 0;

 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
 {
    exclude |= Race->Blds[k]->Req;
    if(Race->Blds[k]->Spc & CSPC_WALL)
	exclude |= 1<<k;
 }

 return Blds & ~exclude;
}



/*=============================================================================
 * BLD SELL PRICE
 */
int cityC::BldSellPrice(int k)
{
 return Race->Blds[k]->Cost / 2;
}





/*=============================================================================
 * SPC
 */
long cityC::Spc()
{
 long spc = 0;

 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    spc |= Race->Blds[k]->Spc;

 return spc;
}



/*=============================================================================
 * SIGHT
 */
bool cityC::CanDetect()
{
 if(Spc() & CSPC_FORECAST) return true;
 //@@ nature's eye

 return false;
}


bool cityC::BACIView()
{
 if(Spc() & CSPC_WALL) return true;
 if(Spc() & CSPC_FORECAST) return true;
 //@@ nature's eye
 //@@ fortress

 return false;
}



/*=============================================================================
 * IS CONQUERED BY
 *
 * Handles city conquest from a player.
 */
void cityC::IsConqueredBy(brainC* player)
{
 //@@ calculate looted gold
 //should be proportional to city wealth
 //if(Owner), this should be proportional to a mage income

 //@@ message to previous owner and all players in the cell:
 if(player)	pe("%s has been conquered by %s!\n", Name, player->Name);
 else		pe("%s has been conquered!\n", Name);
 // "%d gold has been looted"

 //@@ remove all positive enchantements from previous owner

 // change owner
 Owner = player;
}





/*=============================================================================
 * CITY
 */
cityC::cityC(char* name, cellC* cell, raceC* race)
{
 for(char* z = ZeroStart; z < ZeroEnd; z++) *z = 0x00;

 strncpy(Name, name, sizeof(Name));

 if(cell->City) throw "cityC::cityC error, cell already has a city";

 Cell = cell;	Cell->City = this;
 Race = race;
 Pop = 100;
 Seed = game->Rnd.Raw16();
 Owner = 0;

 Queue->Type = Q_SPC;
 Queue->Id = QS_HOUSING;

 QCheck();
 Flags = CITY_FLAG_OPTIMIZE;
}



cityC::~cityC()
{
 Cell->City = 0;
}



bool cityC::IsOutpost() { return !TotalPop; }





/*=============================================================================
 * CITY EX SETTLERS
 */
char* cityExSettlerCheck(unitC* u, char* name)
{
 cellC* c = u->Cell;

 if(!(u->Type->Cmd & CMD_OUTPOST)) return "not a settler";


 // population transfer?
 if(cityC* city = c->City)
 {
    if(city->Race != u->Type->Race) return "can't settle in cities of different race";
    return 0;
 }

 // check race special
 switch(c->Land->Type) {
    case TERRAIN_SEA:	if(u->Type->Race->Spc & RACE_SEA) break;
			else return "can't settle in open sea";
    case TERRAIN_SHORE:	if(u->Type->Race->Spc & RACE_SHORE) break;
			else return "can't settle on shores";
    default:    	if(u->Type->Race->Spc & RACE_LAND) break;
			else return "can't settle on land";
 }


 //@@ check neutrals


 // check surroundings
 cellC* s;
 for(int k = 0; s = c->SMAC(k); k++) if(s->City) return "too close to an existing city";
 for(int k = 0; s = c->BACI(k); k++) if(s->City) return "still too close to a city...";
 for(int k = 0; s = c->HUGS(k); k++) if(s->City) return "there's stll a city somewhere around";

 if(!name || !name[0]) return "a name is required for the new city";

 //ok, no more objections to a city here...
 return 0;
}



void cityExSettler(unitC* u, char* name)
{
 cellC* c = u->Cell;
 cityC* city;

 if(c->City)
 {
    city = c->City;	// population transfer?
    city->TotalPop++;
    city->FarmPop++;
 }
 else			// create a new city
 {
    city = new cityC(name, u->Cell, u->Type->Race);
    city->Owner = u->Owner;
    city->Pop = 0;
    city->TotalPop = 1;
    city->FarmPop = 1;

    city->Queue->Type = Q_SPC;
    city->Queue->Flags = 0;
    city->Queue->Times = 0;
    city->Queue->Id = QS_HOUSING;
 }

 u->Remove();

 city->ProdControl();
}





/*=============================================================================
 *
 */
void cityC::SpawnUnit(int id)
{
 unitC* u;
 u = unitNew(Race->GetUtype(id), Cell);
 u->Owner = Owner;

 // starting level
 //@@ if altar of battle...
 bool regular = false;
 bool veteran = false;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    if(Race->Blds[k]->Spc & CSPC_VETERAN) veteran = true;
    else if(Race->Blds[k]->Spc & CSPC_REGULAR) regular = true;

 if(veteran) u->Exp = unitRanksA[2].ExpThreshold;
 else if(regular) u->Exp = unitRanksA[1].ExpThreshold;
}

void cityC::SpawnBld(int id)
{
 Blds |= 1<<id;
}




void cityC::FinalizeProduction()
{
 switch(Queue->Type) {
    case Q_UNIT: SpawnUnit(Queue->Id); break;
    case Q_BLD: SpawnBld(Queue->Id); break;
 }

 if(--Queue->Times <  1)
    QPop();

 if(ProdStatus > 0 && Owner)
    Owner->GoldReserve += ProdStatus/2;
 ProdStatus = 0;
}



void cityC::Think()
{
 // discard housing if growth is zero
 signed growth = Growth();


 // production
 QCheck();
 int q = QProdCost(Queue);
 if(TotalPop && !Cell->Battle)	// work does not proceed if city is under siege!
    if(q)
    {
	ProdStatus += Work();

	if(ProdStatus >= q)
	{
    	    ProdStatus -= q;
    	    FinalizeProduction();
	}
    }
    else if(Queue->Times)	// !Queue->Times means 'ad infinitum'
	if(--Queue->Times == 0)
	    QPop();

 // growth
 Pop += growth; // increase pop by growth
 int prevpop = TotalPop;
 while(Pop >= 1000) { Pop -= 1000; TotalPop++; }
 while(Pop <     0) { Pop += 1000; TotalPop--; }
 if(TotalPop != prevpop)
 {
    ProdControl();
    //@@ warn player
 }


 // clear BLD_SOLD flag
 Flags &= ~CITY_FLAG_BLD_SOLD;
}





/*=============================================================================
 *
 */
int cityC::TurnsLeft()
{
 int cost = QProdCost(Queue);
 if(!cost) return -1;

 int work = Work();
 int turns = 0;
 for(int c = cost-ProdStatus; c > 0; c -= work) turns++;

 return turns;
}



//EOF =========================================================================
