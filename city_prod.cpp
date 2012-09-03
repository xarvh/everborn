/*
 CITY_PROD.CPP		v0.11
 City Production

 Copyright (c) 2006 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "city.h"
#include "unit.h"
#include "utype.h"
#include "race.h"
#include "map.h"



/*=============================================================================
 * AVL WEAPONS
 *
 * Finds out if and what magical weapons are available.
 */
long cityC::AvlWeapons(queueC* lq)
{
 long flags = 1<<Q_NORMAL;



 // is the alchemists guild available?
 long avl = AvlBlds(lq);
 bldC** b = Race->Blds;
 int x;
 for(x = 0; x < BLD_MAX; x++) if(avl & (1<<x))
    if(b[x]->Spc & CSPC_ALCHEMIST) break;
 if(x == BLD_MAX) return flags;


 // we got an alchemists
 flags |= 1<<Q_ENCHANTED;


 // check terrain for mithril and adamantium
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++) if(CellAvl(c))
 {
    if(c->Plus == plusMithril) flags |= 1<<Q_MITHRIL;
    if(c->Plus == plusAdamantium) flags |= 1<<Q_ADAMANTIUM;
 }

 for(int k = 0; c = Cell->BACI(k); k++) if(CellAvl(c))
 {
    if(c->Plus == plusMithril) flags |= 1<<Q_MITHRIL;
    if(c->Plus == plusAdamantium) flags |= 1<<Q_ADAMANTIUM;
 }

 return flags;
}





/*=============================================================================
 * AVL UNITS
 * Returns a bitmask of all units buildable in a city at a queue point.
 * All blds from the start of the queue will modify item availability!
 */
long cityC::AvlUnits(queueC* lq)
{
 // blds ready
 long blds = Blds;

 // blds in queue
 if(lq) for(queueC* q = Queue; q<lq; q++)
    if(q->Type == Q_BLD) blds |= 1<<q->Id;

 // sea?
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++)
    if(c->Land == landShore || c->Land == landSea)
    { blds |= REQ_SEA; break; }

 // items with requirements fulfilled
 long able = 0;
 int i = 0;
 for(utypeC* u = Race->Units; u; i++, u = u->Next)
    if((blds & u->Req) == u->Req) able |= 1<<i;

 return able;
}





/*=============================================================================
 * AVL BLDS
 * Returns a bitmask of all blds buildable in a city at a queue point.
 * All blds from the start of the queue will modify item availability!
 */
long cityC::AvlBlds(queueC* lq)
{
 // blds ready
 long blds = Blds;

 // blds in queue
 if(lq) for(queueC* q = Queue; q < lq && q < QueueEnd; q++)
    if(q->Type == Q_BLD) blds |= 1<<q->Id;

 // sea?
 cellC* c;
 for(int k = 0; c = Cell->SMAC(k); k++)
    if(c->Land == landShore || c->Land == landSea)
    { blds |= REQ_SEA; break; }

 // items with requirements fulfilled
 long able = 0;
 bool library = false;
 int alchem = -1;
 bldC** b = Race->Blds;
 for(int x = 0; x < BLD_MAX; x++)
 {
    if((blds & b[x]->Req) == b[x]->Req)	able |= 1<<x;

    // check also for the alchimists availability
    if(blds & (1<<x)) if(b[x]->Spc & CSPC_LIBRARY)	library = true;
    if(b[x]->Spc & CSPC_ALCHEMIST) alchem = x;
 }


 // alchemist?
 if(Alchemist.Is(Owner) && library) able |= 1<<alchem;


 // remove blds already built
 able &= ~blds;

 return able;
}





/*=============================================================================
 * AVL BLDS
 * Returns a bitmask of all blds buildable in a city at a queue point.
 * All blds from the start of the queue will modify item availability!
 */
long cityC::AvlSpc(queueC* lq)
{
 long able = 1<<QS_TRADE_GOODS;

 // housing cannot be in first position if population growth is non-positive
 if(lq != Queue || Growth() > 0)
    able |= 1<<QS_HOUSING;

 return able;
}





/*=============================================================================
 * CITY AVL
 * Wrapper around all the above.
 */
long cityC::AvlItems(queueC* lq, int type)
{
 switch(type) {
    case Q_UNIT:	return AvlUnits(lq);
    case Q_BLD:		return AvlBlds(lq);
    case Q_SPC:
    default:		return AvlSpc(lq);
 }
}





/*=============================================================================
 * PRODUCTION QUEUE
 */
static queueC queueRez = {Q_SPC, QF_RETURN, 0, QS_TRADE_GOODS};



void cityC::QRm(queueC* q)
{
 queueC* queue = Queue;
 while(q < queue + CITY_QUEUE_BF-1 && q[1].Type)	{ q[0] = q[1]; q++; }
 q->Type = 0;
 if(q == queue) *q = queueRez;
}



void cityC::QPush(queueC* q)
{
 // copy items
 for(queueC* e = Queue + CITY_QUEUE_BF -1; e > q; e--)
    e[0] = e[-1];

 // set blank item
 q->Type = Q_SPC;
 q->Id = QS_TRADE_GOODS;
 q->Times = 1;
 q->Flags = 0;
}



void cityC::QPop()
{
 QRm(Queue);
 QCounter = Queue->Times;
}



utypeC* cityC::QUnit(queueC* q)
{
 return Race->GetUtype(q->Id);
}



bldC* cityC::QBld(queueC* q)
{
 return Race->Blds[q->Id];
}



char* cityC::QName(queueC* q)
{
 switch(q->Type) {
	case Q_SPC:	if(q->Id == QS_TRADE_GOODS)
			    return "Trade Goods";
			else
			    return "Housing";
	case Q_UNIT:	return Race->GetUtype(q->Id)->Name;
	case Q_BLD:	return Race->Blds[q->Id]->Name;
 }
}

long cityC::QBaseCost(queueC* q)
{
 switch(q->Type)    {
	case Q_BLD:	return Race->Blds[q->Id]->Cost;
	case Q_UNIT:	return Race->GetUtype(q->Id)->Cost;
	case Q_SPC:
	default:	return 0;
    }
}

long cityC::QUpkeep(queueC* q)
{
 utypeC* u;
 bldC* b;
 switch(q->Type) {
	case Q_UNIT:		u = Race->Units + q->Id;
				return (u->Mana<<16) | (u->Food<<8) | u->Gold;
	case Q_BLD:		b = Race->Blds[q->Id];
				if(b->Mana >= 0) return b->Gold;
				else return ((-b->Mana)<<16) | b->Gold;
	default: return 0;
 }
}



/*=============================================================================
 * QUEUE CHECK
 * Checks if an entire queue is available to a city.
 * Removes unavailable items.
 */
bool cityC::QCheck()
{
 long able;

 for(queueC* q = Queue; q-Queue < CITY_QUEUE_BF;)
 {
    if(q->Times > 100) q->Times = 100;

    if(q->Type != Q_SPC && !q->Times)
	q->Times = 1;

    switch(q->Type) {
	case Q_UNIT:
	    able = AvlUnits(q);
	    {	    // check magical weapons
		int f = q->Flags & QF_WPN;
		bool ok = (1<<f) & AvlWeapons(q);
		if(!ok) q->Flags &= ~QF_WPN;
	    }
	    break;

	case Q_BLD: able = AvlBlds(q); break;

	case Q_SPC: able = AvlSpc(q);  break;

	default:
	    // if at end of queue, and not first item,
	    // return, remaining items are ignored.
	    if(!q->Type && q != Queue)
		return true;
	    *q = queueRez;
	    able = 1<<q->Id;
	    break;
	}

	if(able & (1<<q->Id)) q++;
	else QRm(q);
 }

 return true;
}





/*=============================================================================
 * PRODUCTION CONTROL
 *
 * Control the workers/farmers allocation
 */
void cityC::ProdControl()
{
 if(Flags & CITY_FLAG_OPTIMIZE)	ProdOptimize();
 else				ProdPreserve();
// ProdCheck();
}



/* PRODUCTION OPTIMIZE
 * Optimize workers vs. farmers to terminate production less turn possible,
 * avoiding starvation.
 */
void cityC::ProdOptimize()
{
 int x, best_id = 0;
 int time, best_time = 1024*1024;

 int max = TotalPop - Unrest();
 int cost = QProdCost(Queue);

 if(!cost) cost = 1024*1024;
 else cost -= ProdStatus;

 for(int x = 0; x < max; x++)
 {
    WorkPop = x;
    FarmPop = max - x;
    if(Food() < 0) break;

    // choose best prod with positive food!
    int work = Work();
    if(work < 1) work = 1;
    time = (cost-1) / work;
    if(time < best_time)
    {
	best_time = time;
	best_id = x;
    }
 }

 WorkPop = best_id;
 FarmPop = max - best_id;
}



/* PRODUCTION PRESERVE
 * Balance workers vs. farmers in order to preserve work output, but avoid
 * starvation.
 */
void cityC::ProdPreserve()
{
/* int max = TotalPop - Unrest();
 int work = Work();

 int x;
 for(x = 0; x < max; x++)
    {
	WorkPop = x;
	FarmPop = max - x;
	if(Food() < 0) { x--; break; }
	if(Work() >= work) break;
    }

 WorkPop = x;
 FarmPop = max - x;*/
}



/* PRODUCTION CHECK
 * Ensures that
 *	workers + farmers + rebels = total pop
 * Tries to preserve work output and ignores eventual starvation.
 * Used by server to ensure a valid workers vs. farmers allocation.
 */
void cityC::ProdCheck()
{
 int work = Work();
 int maxW = TotalPop - Unrest();

 int w;
 for(w = 0; w < maxW; w++)
    {
	WorkPop = w;
	FarmPop = maxW - w;
	if(Work() >= work) break;
    }

 WorkPop = w;
 FarmPop = maxW - w;
}





/*=============================================================================
 * PROD COST
 * Returns a queue item production cost, including terrain bonuses.
 */
signed cityC::QProdCost(queueC* q)
{
 switch(q->Type) {
    case Q_SPC:	return 0;
    case Q_BLD:	return Race->Blds[q->Id]->Cost;
    default: break;
 }

 int bonus = TerrainCostBonus(); 		// base
 if(Race->Spc & RACE_MINERS) bonus *= 2;	// racial mining bonus
 if(Spc() & CSPC_MINE) bonus = (3*bonus) /2;	// miner's guild
 if(bonus > 60) bonus = 60;			// limit positive bonus to 60%

 switch(q->Flags&3) {				// special weapons 
	case Q_NORMAL:		break;
	case Q_ENCHANTED:	bonus -=  5;	break;
	case Q_MITHRIL:		bonus -= 10;	break;
	case Q_ADAMANTIUM:	bonus -= 20;	break;
	default: break;
 }

 return Race->GetUtype(q->Id)->Cost*(100-bonus)/100;
}





//EOF =========================================================================
