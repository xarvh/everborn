/*
 CITY_OUT.CPP		v0.14
 City Rules

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

#include "main.h"

#include "game.h"
#include "race.h"
#include "map.h"

#include "city.h"
#include "unit.h"

#include "counters.h"





/*
int calc(cellC* Cell)
{
 int par = 0;

 // Land->MaxFood is stored as double the display value.
 // so we multiply it by only 50 instead of the canonical 100.
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) par += c->Land->MaxFood;
 for(int k = 0; c = Cell->BACI(k); k++) par += c->Land->MaxFood;

 return par;
}

int bohe()
{
 long tot = 0;
 long cnt = 0;

 for(int y = 4; y < game->Map.H-4; y++)
    for(int x = 0; x < game->Map.W; x++)
    {
	tot += calc(map(x, y));
	cnt++;
    }

 pf("%f", tot/(float)cnt);
}

#include "cmd.h"
static cmdC boh("boh", ":", "", bohe, 0, 0);
*/









bool cityC::IsTradingGoods() { return Queue->Type == Q_SPC && Queue->Id == QS_TRADE_GOODS; }
bool cityC::IsHousing() { return Queue->Type == Q_SPC && Queue->Id == QS_HOUSING; }




/*=============================================================================
 * AVL
 * If cell resources are actually available to a city, returns the cell itself,
 * otherwise, NULL is returned.
 */
cellC* cityC::CellAvl(cellC* cell)
{
 if(cell->Flags & CELL_CORRUPT) return 0;
 if(cell->Battle) return 0;

 for(unitC* u = cell->Unit; u; u = u->Next)
    if(Owner)
    {
	if(!u->Owner) return 0;
	else if(u->Owner->EngageMask & Owner->Bit) return 0;
    }
    else if(u->Owner) return 0;

 return cell;
}





/*=============================================================================
 * FOOD
 */
signed cityC::Food()
{
 countersZero();	// reset counters
 long spc = Spc();


 //Farmers
 bool foodBonus = spc & CSPC_FOOD || Race->Spc&RACE_FARMERS;
 int maxFarming = TerrainBasePop();
 int fullFarmers = FarmPop > maxFarming ? maxFarming : FarmPop;
 int base = fullFarmers * (foodBonus ? 300:200);
 base += (FarmPop - fullFarmers)*100;
 countersAdd("Farmers", base);


 // Land
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c))
    countersAdd(c->Plus->Name, c->Plus->Food * 100);

 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c))
    countersAdd(c->Plus->Name, c->Plus->Food * 100);


 // Buildings
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAdd(bld[k]->Name, bld[k]->Food * 100);


 //@@ Enchantements


 // Population (they eat also...)
 countersAdd("Population", -TotalPop*100 );

 return countersTotal()/100;
}





/*=============================================================================
 * GOLD
 */
signed cityC::Gold()
{
 // Calculate now Trade Goods, so that it will not interfere with Gold counters //
 int work = 0;
 if(IsTradingGoods()) work = Work();


 countersZero();	// reset counters


 // Taxes
 int base = (WorkPop + FarmPop) * 50 * (Owner ? Owner->Taxes : 2);
 countersAdd("Taxes", base);

 // Terrain
 if(int k = TerrainGoldBonus()) countersAddP("Terrain", (base*k)/100, k);


 // Trade Goods
 if(work) countersAdd("TradeGoods", work / 2);


 // Land Pluses
 cellC* c;
 long spc = Spc();

 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c))
 {
    int par = c->Plus->Gold * 100;
    if(Race->Spc & RACE_MINERS) par *= 2;
    if(spc & CSPC_MINE) par += par/2;
    countersAdd(c->Plus->Name, par);
 }

 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c))
 {
    int par = c->Plus->Gold * 50;
    if(Race->Spc & RACE_MINERS) par *= 2;
    if(spc & CSPC_MINE) par += par/2;
    countersAdd(c->Plus->Name, par);
 }


 // Blds
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAddP(bld[k]->Name, (bld[k]->GoldB) * base /100, bld[k]->GoldB);


 //@@ Enchantements


 // Buildings Manteniance
 int par = 0;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))	par -= 100 * bld[k]->Gold;
 countersAdd("Manteniance", par);

 return countersTotal() / 100;
}





/*=============================================================================
 * WORK
 */
signed cityC::Work()
{
 countersZero();	// reset counters


 // Workers
 int base;
 if(Race->Spc&RACE_WORKERS)	base = WorkPop*300;
 else 				base = WorkPop*200;
 countersAdd("Workers", base);


 // Farmers
 int par = FarmPop*50;
 base += par;
 countersAdd("Farmers", par);


 // Terrain
 if(int k = TerrainWorkBonus()) countersAddP("Terrain", (base*k)/100, k);


 // Buildings
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAddP(bld[k]->Name, (bld[k]->ProdB) * base /100, bld[k]->ProdB);


 //@@ Enchantements


 return countersTotal() / 100;
}





/*=============================================================================
 * MANA
 */
signed cityC::Mana()
{
 countersZero();	// reset counters


 // Buildings
 // @@ Add Enchantements effects!
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAdd(bld[k]->Name, bld[k]->Mana *100);


 // Land
 cellC* c;
 long spc = Spc();
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c))
 {
    int par = c->Plus->Mana * 100;
    if(Race->Spc & RACE_MINERS) par *= 2;
    if(spc & CSPC_MINE) par += par/2;
    countersAdd(c->Plus->Name, par);
 }

 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c))
 {
    int par = c->Plus->Mana * 50;
    if(Race->Spc & RACE_MINERS) par *= 2;
    if(spc & CSPC_MINE) par += par/2;
    countersAdd(c->Plus->Name, par);
 }


 // Population
 if(Race->Spc&RACE_POPMANA || spc&CSPC_POPMANA)
    // rebels _do_ count for the bonus
    countersAdd("Pupulation", TotalPop * 50);


 // Enchantements
 //@@ no info on spell manteniance cost!

 return countersTotal() / 100;
}





/*=============================================================================
 * KNOW
 */
signed cityC::Know()
{
 countersZero();	// reset counters


 // Buildings
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAdd(bld[k]->Name, bld[k]->Know);

 return countersTotal();
}





/*=============================================================================
 * TERRAIN GOLD BONUS
 */
signed cityC::TerrainGoldBonus()
{
 int par = 0;
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c)) par += c->Land->GoldB;
 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c)) par += c->Land->GoldB/2;

 if(par > 100) par = 100;
 return par;
}





/*=============================================================================
 * TERRAIN WORK BONUS
 */
signed cityC::TerrainWorkBonus()
{
 int par = 0;
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c)) par += c->Land->ProdB;
 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c)) par += c->Land->ProdB/2;

 return par;
}





/*=============================================================================
 * TERRAIN COST BONUS
 */
signed cityC::TerrainCostBonus()
{
 int par = 0;
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c)) par += c->Plus->CostB;
 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c)) par += c->Plus->CostB/2;

 return par;
}





/*=============================================================================
 * TERRAIN BASE POP
 *
 * Maximum population supported by the area
 */
signed cityC::TerrainBasePop()
{
 int par = 0;
 cellC* c;

 // Land->MaxFood is stored as double the display value.
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c)) par += c->Land->MaxFood*2;
 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c)) par += c->Land->MaxFood;

 // max is: 1.5 x9 + .75 x12 = 22.5
 return par / 4;
}





/*=============================================================================
 * GROWTH
 */
signed cityC::Growth()
{
 // calculate those now, otherwise they're going to screw up the counters
 signed food = Food();
 signed work = Work();

 // NOW that counters won't be touched anymore, we can reset them.
 countersZero();

 // Terrain food
 countersAdd("Terrain", 10 * TerrainBasePop());

 // Buildings
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
    countersAdd(bld[k]->Name, bld[k]->PopB);

 // forest bonus
 if(Race->Spc & RACE_FOREST)
    if(Cell->Land == landForest)
	countersAdd("Forest Affinity", 20);

 // racial bonus
 countersAdd("Race Modifier", Race->Growth);

 // population
 countersAdd("City Size", -10*TotalPop);

 /*
 if(city->ench[STREAM_OF_LIFE])
	if(growth < 0) growth /= 2;
	else growth *= 2;

 if(city->ench[DARK_RITUALS])
	if(growth < 0) growth = (5*growth)/4;	/ +25%
	else growth = (3*growth)/4;		/ -25%
 */

 // Starvation
 if(food < 0) countersAdd("Starvation", food);

 // Housing
 if(countersTotal() > 0 && IsHousing())
    countersAdd("Housing", (countersTotal()*work)/50);

 // round population
 //@@ really required??
 if(countersTotal() == 0)
    countersAdd("", -Pop);

 return countersTotal();
}





/*=============================================================================
 * CITY UNREST
 * Calculates city population unrest.
 */
int cityC::Unrest()
{
 if(!Owner) return 0;

 countersZero();

 // @@ unrest_base? city->player_mod[city->owner->id];

 // Taxes
 int unrest;
 switch(Owner->Taxes)
 {
    case 0:	unrest =  0;	break;	//  0 Gold/Pop
    case 1:	unrest = 10;	break;	// .5 Gold/Pop
    case 2:	unrest = 20;	break;	// 10 Gold/Pop
    case 3:	unrest = 30;	break;	// 15 Gold/Pop
    case 4:	unrest = 45;	break;	// 20 Gold/Pop
    case 5:	unrest = 60;	break;	// 25 Gold/Pop
    case 6:	unrest = 75;	break;	// 30 Gold/Pop
 }
 countersAdd("Taxes", (unrest*TotalPop)/100);



 // Blds
 bldC** bld = Race->Blds;
 for(int k = 0; k < BLD_MAX; k++) if(Blds & (1<<k))
/*@@	if(	(Owner->Spc[RETORT_LIGHTLORD] ||
		 Owner->spc[RETORT_DARKLORD])
	&&	(bld[k]->unrest && bld[k]->mana)
	)	mod -= bld[k]->unrest*2;
	else*/
	countersAdd(bld[k]->Name, -bld[k]->Unrest);


 // Units
 //@@ must be decreased for non-stacked units
 int k = 0;
 for(unitC* u = Cell->Unit; u; u = u->Next) k++;
 countersAdd("City Garrison", -k/2);

 /*======== CITY SPELLS
 if(city->ench[PROSPERITY])	mod -= 1;
 if(city->ench[STREAM_OF_LIFE])	mod -= 10;
 if(city->ench[INSPIRATIONS])	mod -= 2;
 if(city->ench[DARK_RITUALS])	mod += 1;
 if(city->ench[FAMINE])		mod += 1;
 if(city->ench[CURSED_LANDS])	mod += 1;
 if(city->ench[PESTILENCE])	mod += 1;
 if(city->ench[CHAOS_RIFT])	mod += 1;
 if(city->ench[GAIAS_BLESSING])	mod -= 2;
 */

 if(countersTotal() < 0) return 0;
 if(countersTotal() > TotalPop) return TotalPop;
 return countersTotal();
}





//EOF =========================================================================
