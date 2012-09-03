/*
 CITY_CMD.CPP		v0.01
 City commands.

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





/*=============================================================================
 * Taxes
 * Selects taxation.
 * Not strictly a city command.
 */
static int cmdFTaxes()
{
}

static int cmdGTaxes()
{
 int taxes = (int)(arg1.f * 2.);
 if(taxes >= 0 && taxes <= 6) cmdGetBrain()->Taxes = taxes;
}

static cmdC cmdTaxes(
    "taxes",			// name
    "f: Gold per 1000 citizens",	// syntax
    "selects amount of taxation in your empire",
    cmdFTaxes,			// exec()
    cmdGTaxes,			// game()
    0				// check()
);





/*=============================================================================
 * Sell
 * Sells away a building.
 */
static int cmdFSell()
{
 cellC* f = cmdGetBrain()->Focus;
 if(!f) return 0;				// no focus

 cityC* c = f->City;
 if(!c) return 0;				// no city in focus

 if(cmdGetBrain() != c->Owner) return 0;	// not owner

 if(c->Flags & CITY_FLAG_BLD_SOLD)
    pf("%s has already sold a building this turn.\n", c->Name); 
}

static int cmdGSell()
{
 cellC* f = cmdGetBrain()->Focus;
 if(!f) return 0;				// no focus

 cityC* c = f->City;
 if(!c) return 0;				// no city in focus

 if(cmdGetBrain() != c->Owner) return 0;	// not owner

 if(c->Flags & CITY_FLAG_BLD_SOLD) return 0;	// already sold a bld this turn

 // find bld
 int k;
 for(k = 0; k < BLD_MAX && strcmp(c->Race->Blds[k]->Id, arg1.s); k++);
 if(k == BLD_MAX) return 0;			// invalid bld

 if(!(c->VisibleBlds() & (1<<k))) return 0;	// this bld is required!

 // remove k bld
 c->Owner->GoldReserve += c->BldSellPrice(k);
 c->Blds &= ~(1<<k);
 c->Flags |= CITY_FLAG_BLD_SOLD;
}

static cmdC cmdSell(
    "sell",			// name
    "N: BldId",			// syntax
    "Sells away a building",
    cmdFSell,			// exec()
    cmdGSell,			// game()
    0				// check()
);





/*=============================================================================
 * Queue
 * Changes a queue item on focused city.
 */
static int cmdFQueue()
{
 //pf("changing queue\n");
}

static int cmdGQueue()
{
 cityC* c = cmdGetBrain()->Focus->City;
 queueC* q = c->Queue + ((unsigned)arg1.i)%CITY_QUEUE_BF;

 int type = arg2.i;
 switch(type) {
    case Q_FREE: pf("remove\n"); c->QRm(q); break;
    case Q_PUSH: c->QPush(q); break;
    default:
	q->Type	= type;
	q->Id	= arg3.i;
	q->Times= arg4.i;
	q->Flags= arg5.i;
	break;
 }

 c->QCheck();
 c->ProdControl();
}

static int cmdCQueue()
{
 cellC* c = cmdGetBrain()->Focus;
 if(!c) return pe("NOFOCUS\n"), -1;
 if(!c->City) return pe("NOCITY in %d %d %d\n", c->X, c->Y, c->Z), -1;	// no city present
 if(c->City->Owner != cmdGetBrain()) return pe("NOTOWNER\n"), -1;	// owner only!!
 return 0;
}

static cmdC cmdQueue(
    "queue",			// name
    "iiiii: # Type Id Repeat Flags",	// syntax
    "changes a queue item",
    cmdFQueue,			// exec()
    cmdGQueue,			// game()
    cmdCQueue			// check()
);





/*=============================================================================
 * Pop
 * Reallocates city population
 *
 * "pop X" sets X workers.
 * "pop" alone sets to optimization.
 */
static int cmdGPop()
{
 cityC* city = cmdGetBrain()->Focus->City;
 if(arg1.miss) city->Flags |= CITY_FLAG_OPTIMIZE;
 else
 {
    city->Flags &= ~CITY_FLAG_OPTIMIZE;

    int max = city->TotalPop - city->Unrest();
    int v = arg1.i;
    if(v > max) v = max;
    city->WorkPop = v;
    city->FarmPop = max - v;
 }

 city->ProdControl();
}

static int cmdCPop()
{
 cellC* c = cmdGetBrain()->Focus;
 if(!c) return -1;
 if(!c->City) return -1;			// no city present
 if(c->City->Owner != cmdGetBrain()) return -1;	// owner only!!
 return 0;
}

static cmdC cmdPop(
    "pop",			// name
    "i: Workers",		// syntax
    "allocates city workers",
    0,				// exec()
    cmdGPop,			// game()
    cmdCPop			// check()
);





//EOF =========================================================================
