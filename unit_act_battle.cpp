/*
 UNIT_ACT_BATTLE.CPP		v0.01
 Battle orders and actions

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

#include <math.h>
#include "game.h"
#include "main.h"
#include "unit.h"
#include "utype.h"
#include "ske.h"

#include "particles.h"





/*=============================================================================
 * GLOBALS
 */


// action/animation times
static float SHOOT_event_spawn	= 16; //BATTLE_DEFAULT_ACTION*1.65;
static float SHOOT_event_end	= 20; //BATTLE_DEFAULT_ACTION;
static float MELEE_event_damage	=  5; //BATTLE_DEFAULT_ACTION;
static float MELEE_event_end	= 15; //BATTLE_DEFAULT_ACTION*1.5;



// orders list hook
static unitOrderC* uobHook = 0;





/*=============================================================================
 * ACTION
 * MOVE
 */
/*static bool uabMOVE_IsCompleted(unitC *u)
{
 return true;
}*/



static void uabMOVE_Exec(unitC *u)
{
 if(umMoveTo(u, u->ActionTargX, u->ActionTargY, u->ActionDist))
 {
    u->AnimationTime += u->Type->Speed / 30.;
    u->ActionReport = 0;
 }
 else
 {
    u->ActionReport++;
 }
}



static char* uabMOVE_Init(unitC* u)
{
 return "walk";
}



static actionC uabMOVE(
    "move",
    uabMOVE_Exec,
    uabMOVE_Init,
    0, //uabMOVE_IsCompleted,
    false);





/*=============================================================================
 * ACTION
 * WAIT
 */
static void uabWAIT_Exec(unitC *u)
{
 u->AnimationTime += .02;
}


static char* uabWAIT_Init(unitC* u)
{
 return "idle";
}

static actionC uabWAIT(
    "wait",
    uabWAIT_Exec,
    uabWAIT_Init,
    0,
    true);





/*=============================================================================
 * ACTION
 * MELEE
 */
static void uabMELEE_Exec(unitC *u)
{
 unitC* targ = (unitC*)u->ActionTargP;

 if(!targ->IsAlive())
 {
    u->ActionStatus++;
    return;
 }

 double w = targ->X - u->X;
 double h = targ->Y - u->Y;
 u->A = (180/3.1415)*atan2(w, h);


 if(u->ActionStatus == MELEE_event_damage)
    u->InflictMeleeDamage(targ);
 u->ActionStatus++;
 u->AnimationTime += 1./MELEE_event_end;
}



static bool uabMELEE_IsCompleted(unitC *u)
{
 return
    u->ActionStatus >= MELEE_event_end;
    //|| u->ActionStatus < MELEE_event_damage;
}



static char* uabMELEE_Init(unitC* u)
{
 u->ActionStatus = 0;
 u->AnimationTime = .0;
 return "strike";
}


static actionC uabMELEE(
    "melee",
    uabMELEE_Exec,
    uabMELEE_Init,
    uabMELEE_IsCompleted,
    true);





/*=============================================================================
 * ACTION
 * SHOOT
 */
static void uabSHOOT_Exec(unitC* u)
{
 unitC* targ = (unitC*)u->ActionTargP;

 if(!targ->IsAlive())
 {
    u->ActionStatus++;
    return;
 }


 // turn to face target
 double w = targ->X - u->X;
 double h = targ->Y - u->Y;
 u->A = (180/3.1415)*atan2(w, h);

 if(u->ActionStatus == SHOOT_event_spawn)
 {
    u->Ammo--;
    new particleArrowC(u, targ);
 }

 u->ActionStatus++;
 u->AnimationTime += 1./SHOOT_event_end;
}


static bool uabSHOOT_IsCompleted(unitC *u)
{
 return
    u->ActionStatus >= SHOOT_event_end;
    //|| u->ActionStatus < SHOOT_event_spawn;
}


static char* uabSHOOT_Init(unitC* u)
{
 u->ActionStatus = 0;
 u->AnimationTime = .0;
 return "arrow";
}


static actionC uabSHOOT(
    "shoot",
    uabSHOOT_Exec,
    uabSHOOT_Init,
    uabSHOOT_IsCompleted,
    true);





/*=============================================================================
 * ACTION
 * ESCAPE
 */
static bool uabESCAPE_IsCompleted(unitC *u)
{
 // if this function is called, it means that the unit is still in the
 // battlefield: the action may be interrupted.
 return true;
}

static void uabESCAPE_Exec(unitC* u)
{
 int escape = battleEscape((int)u->X, (int)u->Y);
 if(escape < 0) return;

 if(!u->CanEscape(escape)) return;

 // escape!
 if(u->Owner) u->Deselect(u->Owner->Bit); // otherwise Displace() will switch focus

 u->Team = game->GetNewTeam();
 u->TeamDisplace(u->Cell->SMAC(escape));
 u->TimeAvailable = -6;
}

static char* uabESCAPE_Init(unitC* u)
{
 return "walk";
}


static actionC uabESCAPE(
    "escape",
    uabESCAPE_Exec,
    uabESCAPE_Init,
    uabESCAPE_IsCompleted,
    false);





/*=============================================================================
 * ORDER (helper)
 * GO
 * helper for all orders implying pathing
 */
static void uobGo(unitC* u, float x, float y, float d)
{
 u->Action = &uabMOVE;
 u->ActionTargX = x;
 u->ActionTargY = y;
 u->ActionDist = d;
}





/*=============================================================================
 * ORDER
 * WAVE
 */
static float min(float a, float b) { return a<b? a : b; }
static float ABS(float a) { return a<0? -a : a; }


static bool uobWAVE_IsCompleted(unitC* u)
{
 if(u->ActionReport > 20)
 {
    u->ActionReport = 0;
    return true;
 }

 int angle = u->BattleOrders.WP->X;
 int nx = (int)(u->X + mapSMACX(angle) +.555);
 int ny = (int)(u->Y + mapSMACY(angle) +.555);

 if(battleEscape(nx, ny) != -1) return true;

 // this is used when units enter the battlefield
 int stop_dist = u->BattleOrders.WP->Y;
 if(!stop_dist) return false;

 float x = u->X;
 float y = u->Y;
 float c = BATTLE_W/2.;
 float p = c - stop_dist;

 if(angle & 1) p *= 1.4;

 switch(angle) {
    case 3: case 7:	return ABS(x - y) < p;
    case 1: case 5:	return ABS(x + y - 2*c) < p;
    case 0: case 4:	return ABS(y - c) < p;
    case 2: case 6:	return ABS(x - c) < p;
 }
}



static void uobWAVE_SetAction(unitC* u)
{
 int angle = u->BattleOrders.WP->X;
 int nx = (int)(u->X + mapSMACX(angle) +.555);
 int ny = (int)(u->Y + mapSMACY(angle) +.555);

 uobGo(u, nx, ny, 0);
}

unitOrderC uobWAVE(
    &uobHook,
    "wave",
    0,
    0,
    uobWAVE_IsCompleted,
    uobWAVE_SetAction, 0);





/*=============================================================================
 * ORDER
 * MOVE
 */
static bool uobMOVE_IsCompleted(unitC* u)
{
 if(u->ActionReport > 1)
 {
    u->ActionReport = 0;
    return true;
 }

 if(u->BattleOrders.WP->X != (int)(u->X+.555)) return false;
 if(u->BattleOrders.WP->Y != (int)(u->Y+.555)) return false;
 return true;
}



static void uobMOVE_SetAction(unitC* u)
{
 uobGo(u, u->BattleOrders.WP->X, u->BattleOrders.WP->Y, 0);
}

unitOrderC uobMOVE(
    &uobHook,
    "move",
    0,
    0, 
    uobMOVE_IsCompleted,
    uobMOVE_SetAction, 0);





/*=============================================================================
 * ORDER
 * MELEE
 */
static bool uobMELEE_IsCompleted(unitC* u)
{
 unitC* target = (unitC*)u->BattleOrders.WP->P;
 return !target->IsAlive();
}


static void uobMELEE_SetAction(unitC* u)
{
 unitC* t = (unitC*)u->BattleOrders.WP->P;
 float  d = u->MeleeDistanceVS(t);

 if(*u*t <= d*d)
 {
    u->Action = &uabMELEE;
    u->ActionTargP = t;
 }
 else
 {
    uobGo(u, t->X, t->Y, .9*d);
//    pf("noattack: %f vs %f\n", sqrt(*u*t), sqrt(d*d));
 }
}

unitOrderC uobMELEE(
    &uobHook,
    "melee",
    0,
    0,
    uobMELEE_IsCompleted,
    uobMELEE_SetAction, 0);





/*=============================================================================
 * ORDER
 * SHOOT
 */
static bool uobSHOOT_IsAvailable(unitC* u)
{
 return u->Type->Range && u->Ammo;
}



static bool uobSHOOT_IsCompleted(unitC* u)
{
 if(!u->CanShoot()) return true;

 unitC* target = (unitC*)u->BattleOrders.WP->P;
 return !target->IsAlive();
}


static void uobSHOOT_SetAction(unitC* u)
{
 unitC* t = (unitC*)u->BattleOrders.WP->P;
 float  d = u->RangeDistanceVS(t);

 if(!u->CanShoot())
    u->Action = &uabWAIT;
 else if(*u*t < d*d)
 {
    u->Action = &uabSHOOT;
    u->ActionTargP = t;
 }
 else
    uobGo(u, (int)t->X, (int)t->Y, (int)d);
}

unitOrderC uobSHOOT(
    &uobHook,
    "shoot",
    &uobMELEE,
    uobSHOOT_IsAvailable,
    uobSHOOT_IsCompleted,
    uobSHOOT_SetAction, 0);





/*=============================================================================
 * ORDER
 * ESCAPE
 */
static bool uobESCAPE_IsCompleted(unitC* u)
{
 return !u->CanEscape(u->BattleOrders.WP->X);
}



static void uobESCAPE_SetAction(unitC* u)
{
 int escape_direction = u->BattleOrders.WP->X;
 int x = (int)u->X;
 int y = (int)u->Y;

 if(battleEscape(x, y) == escape_direction) u->Action = &uabESCAPE;
 else switch(escape_direction) {
	case 0: uobGo(u, x, BATTLE_H-1, 0); break;
	case 4: uobGo(u, x, 0, 0); break;

	case 2: uobGo(u, BATTLE_W-1, y, 0); break;
	case 6: uobGo(u, 0, y, 0); break;

	case 5:
	    if(x>y)	uobGo(u, x, 0, 0);
	    else	uobGo(u, 0, y, 0);
	    break;

	case 7:
	    if(x>BATTLE_H-y)	uobGo(u, x, BATTLE_H-1, 0);
	    else		uobGo(u, 0, y, 0);
	    break;

	case 1:
	    if(BATTLE_W-x>BATTLE_H-y)	uobGo(u, x, BATTLE_H-1, 0);
	    else			uobGo(u, BATTLE_W-1, y, 0);
	    break;

	case 3:
	    if(BATTLE_W-x>y)	uobGo(u, x, 0, 0);
	    else		uobGo(u, BATTLE_W-1, y, 0);
	    break;
    }
}

unitOrderC uobESCAPE(
    &uobHook,
    "escape",
    0,
    0, 
    uobESCAPE_IsCompleted,
    uobESCAPE_SetAction, 0);





/*=============================================================================
 * ORDER
 * ENGAGE
 */
static bool uobENGAGE_IsCompleted(unitC* u)
{
 return false;
}



static void uobENGAGE_SetAction(unitC* u)
{
 if(unitC* t = u->FindBestTarget())
 {
    float  d = u->MeleeDistanceVS(t);

    if(*u*t <= d*d)
    {
	u->Action = &uabMELEE;
	u->ActionTargP = t;
    }
    else uobGo(u, t->X, t->Y, .9*d);
 }
 else
 {
    u->Action = &uabWAIT;
 }
}

unitOrderC uobENGAGE(
    &uobHook,
    "engage",
    0,
    0,
    uobENGAGE_IsCompleted,
    uobENGAGE_SetAction, 0);





/*=============================================================================
 * ORDER
 * HOLD
 */
static bool uobHOLD_IsCompleted(unitC* u)
{
 signed& delay = u->BattleOrders.WP->X;
 signed& start = u->BattleOrders.WP->Y;

 // if no delay specifyed, hold is mantained indefinitely
 if(!delay) return false;

 // if no start time is specifyied, initialize start time
 if(!start) start = u->Cell->Battle->Time;

 // check if hold time has expired
 return start + delay < u->Cell->Battle->Time;
}



static void uobHOLD_SetAction(unitC* u)
{/*
 unitC* t = u->FindBestTarget();

 float d;
 if(t) d = u->MeleeDistanceVS(t);
 if(t && *u*t <= d*d)
 {
    u->Action = &uabMELEE;
    u->ActionTargP = t;
 }
 else*/
 {
     u->Action = &uabWAIT;
 }
}

unitOrderC uobHOLD(
    &uobHook,
    "hold",
    0,
    0,
    uobHOLD_IsCompleted,
    uobHOLD_SetAction, 0);





/*=============================================================================
 * DEATH ACTION & ORDER
 *
 * Special case, cannot be directly issued!!!
 */
static bool uabDIE_IsCompleted(unitC *u) { return false; }

static void uabDIE_Exec(unitC *u)
{
 if(u->AnimationTime < .9) u->AnimationTime += .1;
}

static char* uabDIE_Init(unitC* u)
{
 u->AnimationTime = .0;
 return "die";
}


static actionC uabDIE(
    "die",
    uabDIE_Exec,
    uabDIE_Init,
    uabDIE_IsCompleted,
    false);





static bool uobDEAD_IsAvailable(unitC* u)
{
 return false;
}


static bool uobDEAD_IsCompleted(unitC* u)
{
 return false;
}


static void uobDEAD_SetAction(unitC* u)
{
 u->Action = &uabDIE;
}

unitOrderC uobDEAD(
    &uobHook,
    "dead",
    0,
    uobDEAD_IsAvailable,
    uobDEAD_IsCompleted,
    uobDEAD_SetAction, 0);





/*=============================================================================
 * ORDERS
 */
unitOrderC* unitOrderBattleC::OrderDefault()	{ return &uobHOLD; }



void uobSet(unitOrderC* o, brainC* b, bool append, signed x, signed y, void* t)
{
 // A focused cell must exist
 if(!b->Focus) return;

 // A battle must also exist at the location
 if(!b->Focus->Battle) return;

 for(unitC* u = b->Focus->Unit; u; u = u->Next) if(u->IsAlive())
    if(u->Owner == b) if(u->IsSelectedBy(b->Bit))
	if(o->Exec)	// instant order?
	{
	    if(o->IsAvailable(u))
		if(char* e = o->Exec(u, x, y, t))
		    pf("%s: %s.\n", o->Name, e);
	}	    
	else
	{
	    if(o->IsAvailable(u) || (o = o->Fallback))
		u->BattleOrders.Set(o, append, x, y, t);
	    else if(!append)
		u->BattleOrders.Set(0, 0);
	}
}





//EOF =========================================================================
