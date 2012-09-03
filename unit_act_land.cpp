/*
 UNIT_ACT_LAND.CPP		v0.04
 Overland orders and actions.

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


 NOTES:
 Orders define a unit's task.
 Each time an order is issued, it will impose an action (see action.cpp) to
 a unit.
 An order by itself will not change what a unit is doing nor how it will be
 drawn (except for interface element).
*/

#include <string.h>

#include "main.h"

#include "game.h"
#include "unit.h"
#include "utype.h"
#include "city.h"

static unitOrderC* uolHook = 0;





/*=============================================================================
 * ACTION
 * TRAVEL
 */
static void unitActionTRAVEL_Exec(unitC* u)
{
 int dir = u->LandOrders.Path.NextDirection();
 if(!u->Travel && dir != -1)
     new travelC(u, TRAVELT_TRAVEL, dir);

 if(u->Travel)
    u->AnimationTime = (u->Travel->Status+1) / 6.;
}



static bool unitActionTRAVEL_IsCompleted(unitC *u)
{
 return !u->Travel; // || u->Travel->Status == 0;
}



static char* unitActionTRAVEL_Init(unitC* u)
{
 u->ActionStatus = 0;
 return "walk";
}


static actionC unitActionTRAVEL(
    "travel",
    unitActionTRAVEL_Exec,
    unitActionTRAVEL_Init,
    unitActionTRAVEL_IsCompleted,
    false);





/*=============================================================================
 * ORDER
 * TRAVEL
 */
static bool uolTRAVEL_IsCompleted(unitC* u)
{
 return u->Cell == u->LandOrders.WP->P;
}


static void uolTRAVEL_SetAction(unitC* u)
{
 bool ok = true;

 if(u->LandOrders.Path.Status >= u->LandOrders.Path.End)
    if(!u->Transported || u->Transported->Team != u->Team)
	ok = utTeamPath(u);

 if(ok)	u->Action = actionC::Get("travel");
 else	u->Action = actionC::Get("wait");
}

unitOrderC uolTRAVEL(
    &uolHook,
    "travel",
    0,
    0,
    uolTRAVEL_IsCompleted,
    uolTRAVEL_SetAction, 0);





/*=============================================================================
 * ORDER
 * Plane SHIFT
 */
static bool uolSHIFT_IsCompleted(unitC* u)
{
 return u->Cell->Z == u->LandOrders.WP->X;
}


static void uolSHIFT_SetAction(unitC* u)
{
 u->Action = actionC::Get("shift");
}

unitOrderC uolSHIFT(
    &uolHook,
    "shift",
    0,
    0,
    uolSHIFT_IsCompleted,
    uolSHIFT_SetAction, 0);





/*=============================================================================
 * ACTION
 * Plane SHIFT
 */
static void unitActionSHIFT_Exec(unitC* u)
{
 if(!u->Travel)
    new travelC(u, TRAVELT_SHIFT, 8);

 if(u->Travel)
    u->AnimationTime = (u->Travel->Status+1) / 6.;
}



static bool unitActionSHIFT_IsCompleted(unitC *u)
{
 return !u->Travel; // || u->Travel->Status == 0;
}



static char* unitActionSHIFT_Init(unitC* u)
{
 u->ActionStatus = 0;
 return "idle";
}


static actionC unitActionSHIFT(
    "shift",
    unitActionSHIFT_Exec,
    unitActionSHIFT_Init,
    unitActionSHIFT_IsCompleted,
    false);





/*=============================================================================
 * ORDER
 * WAIT
 */
static bool uolWAIT_IsCompleted(unitC* u)
{
 return false;
}

static void uolWAIT_SetAction(unitC* u)
{
 u->Action = actionC::Get("wait");
}

unitOrderC uolWAIT(
    &uolHook,
    "wait",
    0,
    0,
    uolWAIT_IsCompleted,
    uolWAIT_SetAction, 0);





/*=============================================================================
 * ACTION
 * BUILDROAD
 */
static void unitActionBUILDROAD_Exec(unitC *u)
{
 int dir = u->LandOrders.Path.NextDirection();
 if(!u->Travel && dir != -1)
    new travelC(u, TRAVELT_ROAD, dir);

 if(u->Travel)
 {
    bool engi = u->Type->Cmd & CMD_BUILD_ROAD;
    if(engi)	u->AnimationTime += .10;
    else	u->AnimationTime += .02;
 }
}



static bool unitActionBUILDROAD_IsCompleted(unitC *u)
{
 return !u->Travel;
}



static char* unitActionBUILDROAD_Init(unitC* u)
{
 if(u->Type->Cmd & CMD_BUILD_ROAD) return "strike";
 else return "idle";
}


static actionC unitActionBUILDROAD(
    "buildroad",
    unitActionBUILDROAD_Exec,
    unitActionBUILDROAD_Init,
    unitActionBUILDROAD_IsCompleted,
    false);




/*=============================================================================
 * ORDER
 * ROAD
 * Engineers build road
 */
static bool uolROAD_IsAvailable(unitC* u)
{
 for(unitC* t = u->Cell->Unit; t; t = t->Next) if(t->Team == u->Team)
    if(!t->Transported)
	if(t->Type->Cmd & CMD_BUILD_ROAD) return true;

 return false;
}

static bool uolROAD_IsCompleted(unitC* u)
{
 return u->Cell == u->LandOrders.WP->P;
}

static void uolROAD_SetAction(unitC* u)
{
 bool ok = true;

 if(u->LandOrders.Path.Status >= u->LandOrders.Path.End)
    if(!u->Transported)
	ok = utTeamPath(u);

 if(ok)	u->Action = actionC::Get("buildroad");
 else	u->Action = actionC::Get("wait");
}

unitOrderC uolROAD(
    &uolHook,
    "road",
    0,
    uolROAD_IsAvailable,
    uolROAD_IsCompleted,
    uolROAD_SetAction, 0);





/*=============================================================================
 * OVERLAND: SETTLE
 */
static bool uolSETTLE_IsAvailable(unitC* u)
{
 return u->Type->Cmd & CMD_OUTPOST;
}


static char* uolSETTLE_Exec(unitC* u, signed x, signed y, void* p)
{
 char* name = (char*)p;

 char* e = cityExSettlerCheck(u, name);
 if(e) return e;

 cityExSettler(u, name);
 return 0;
}


unitOrderC uolSETTLE(
    &uolHook,
    "settle",
    uolSETTLE_IsAvailable,
    uolSETTLE_Exec,
    0);





/*=============================================================================
 * ORDERS
 */
unitOrderC* unitOrderLandC::OrderDefault() { return &uolWAIT; }



void unitOrderLandC::Clear()
{
 Set(0, false);
 Path.Clear();
}



void uolSet(unitOrderC* o, brainC* b, bool append, signed x, signed y, void* t)
{
 // A focused cell must exist
 if(!b->Focus) return;

 // As well, ignore overland commands issued during battles
 if(b->Focus->Battle) return;

 for(unitC* u = game->Units; u < game->UnitsEnd; u++) if(u->Cell == b->Focus)
    if(u->Owner == b) if(u->Team == b->SelectedTeam)
	if(o->Exec)	// instant order?
	{
	    if(o->IsAvailable(u))
		if(char* e = o->Exec(u, x, y, t))
		    pe("%s failed: %s.\n", o->Name, e);
	}	    
	else
	{
	    if(o->IsAvailable(u))	u->LandOrders.Set(o, append, x, y, t);
	    else if(o->Fallback)	u->LandOrders.Set(o->Fallback, append, x, y, t);
	    else if(!append)		u->LandOrders.Set(0, false);

	    u->LandOrders.Path.Clear();
	    if(u->Travel) delete u->Travel;
	}
}





//EOF =========================================================================
