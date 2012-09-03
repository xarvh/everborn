/*
 UNIT_ACT.CPP		v0.03
 Unit orders and actions, handlers.

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



/*=============================================================================
 * ACTION HANDLERS
 */
static actionC* actionsHook = 0;



static void unitAction_default_Exec(unitC *u){}
static bool unitAction_default_IsCompleted(unitC *u) {return true;}



actionC::actionC(
    char*	n,
    void	(*e)(unitC*),
    char*	(*i)(unitC*),
    bool	(*c)(unitC*),
    bool	usesCell)
{
 actionC** h;

 // find first free hook
 for(h = &actionsHook; *h; h = &(*h)->Next);

 *h = this;
 Next = 0;

 Name = n;
 Exec = e ? e : unitAction_default_Exec;
 Init = i;
 IsCompleted = c ? c : unitAction_default_IsCompleted;
 UsesCell = usesCell;
}



actionC* actionC::Get(char* name)
{
 actionC* a;

 for(a = actionsHook; strcmp(name, a->Name); a = a->Next);
 return a;
}







/*=============================================================================
 * ORDER CLASS
 */
static bool unitOrder_default_is_available(unitC* u) { return true; }

// through orders
unitOrderC::unitOrderC(
    unitOrderC** hook,
    char* name,
    unitOrderC* fallback,
    bool (*is_available)(unitC*),
    bool (*is_completed)(unitC*),
    void (*set_action)(unitC*),
    long flags)
{
 unitOrderC** h;

 for(h = hook; *h; h = &(*h)->Next);
 *h = this;

 Next = 0;
 Name = name;
 Fallback = fallback;
 IsAvailable = is_available ? is_available : unitOrder_default_is_available;
 Exec = 0;
 IsCompleted = is_completed;
 SetAction = set_action;
 Flags = flags;
}


// instant orders
unitOrderC::unitOrderC(
    unitOrderC** hook,
    char* name,
    bool (*is_available)(unitC*),
    char* (*exec)(unitC*, signed, signed, void*),
    long flags)
{
 unitOrderC** h;

 for(h = hook; *h; h = &(*h)->Next);
 *h = this;

 Next = 0;
 Name = name;
 IsAvailable = is_available ? is_available : unitOrder_default_is_available;
 Exec = exec;
 Flags = flags;
}





/*=============================================================================
 * WAYPOINT CLASS
 */
unitOrderWaypointC::unitOrderWaypointC(unitOrderC* o)
{
 Next = 0;
 Order = o;
 X = Y = 0;
 P = 0;
}

unitOrderWaypointC::~unitOrderWaypointC()
{
 if(Next) delete Next;
}





/*=============================================================================
 * ORDER LIST CLASS
 */
unitOrderListC::unitOrderListC(unitOrderC* o)
{
 WP = new unitOrderWaypointC(o);
}

unitOrderListC::~unitOrderListC()
{
 delete WP;
}



void unitOrderListC::Pop()
{
 unitOrderWaypointC* w = WP->Next;
 WP->Next = 0;
 delete WP;
 WP = w;

 if(!WP) WP = new unitOrderWaypointC(OrderDefault());
}




void unitOrderListC::Set(unitOrderC* o, bool append)
{
 Set(o, append, 0, 0, 0);
}

void unitOrderListC::Set(unitOrderC* o, bool append, void* p)
{
 Set(o, append, 0, 0, p);
}

void unitOrderListC::Set(unitOrderC* o, bool append, signed x, signed y)
{
 Set(o, append, x, y, 0);
}

void unitOrderListC::Set(unitOrderC* o, bool append, signed x, signed y, void* p)
{
 if(!o) o = OrderDefault();
 unitOrderWaypointC* w = new unitOrderWaypointC(o);
 w->X = x;
 w->Y = y;
 w->P = p;

 if(append)
 {
    unitOrderWaypointC** h;
    for(h = &WP; *h; h = &(*h)->Next);
    *h = w;
 }
 else
 {
    delete WP;
    WP = w;
 }
}





//EOF =========================================================================
