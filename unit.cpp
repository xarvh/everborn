/*
 UNIT.CPP		v0.22
 Units stuff.

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
#include "unit.h"
#include "utype.h"

#include "colors.h"
#include "city.h"
#include "race.h"
#include "map.h"
#include "ske.h"





/*=============================================================================
 * UNIT STACKS
 */
static float stack8[] = {
 -.33,+.3,   +.0,+.3,   +.33,+.3,

         -.2,.0,      +.2,.0,

 -.33,-.3,   +.0,-.3,   +.33,-.3,
};

static float stack7[] = {
 -.33,+.2,   +.0,+.3,   +.33,+.2,

             +.0,+.0,

 -.33,-.2,   +.0,-.3,   +.33,-.2,
};

static float stack6[] = {
 -.33,+.25,   +.0,+.25,   +.33,+.25,

 -.33,-.25,   +.0,-.25,   +.33,-.25,
};

static float stack5[] = {
 -.33,+.2,   +.0,+.0,   +.33,+.2,

        -.3,-.2,   +.3,-.2,
};


static float stack4[] = {
 -.25,+.25,   +.25,+.25,

 -.25,-.25,   +.25,-.25,
};

static float stack3[] = {
 -.25,+.25,   +.25,+.25,

        +.0,-.25,
};

static float stack2[] = {
 -.25,+0,   +.25,+0,
};

static float stack1[] = {
	.0,.0
};


static float* unitStacks[] = {
    stack1,
    stack2,
    stack3,
    stack4,
    stack5,
    stack6,
    stack7,
    stack8,
};

float* utypeC::Stack()
{
 return unitStacks[(Figures-1)&7];
}

float* unitC::Stack()
{
 return Type->Stack();
}





/*=============================================================================
 * PLAYERMAGE
 *
 * Tells if a unit is the playermage, returning the brainC*
 */
brainC* unitC::PlayerMage()
{
 if(Owner && Owner->PlayerMage == this) return Owner;
 return 0;
}





/*=============================================================================
 * ID
 */
bool unitC::IsCreature()
{
 return Type->Race == &game->Creatures;
}





/*=============================================================================
 * ID
 */
long unitC::Id()
{
 return this - game->Units;
}





/*=============================================================================
 * NAME
 */
char* unitC::Name()
{
 if(Owner && Owner->PlayerMage == this) return Owner->Name;
 if(Hero) return Hero->Name;
 if(Type->Race == &game->Creatures) return Type->Name;

 static char name[1+GAME_NAME_BF*2];

 sprintf(name, "%s %s", Type->Race->Name, Type->Name);
 return name;
}





/*=============================================================================
 * MAIN
 */
unitC* unitNew(utypeC* type, cellC* cell)
{
 unitC* u;
 for(u = game->Units; u < game->UnitsEnd && u->Exists(); u++);
 if(u >= game->UnitsEnd)
 {
    pe("unit limit (%d) reached.\n", game->UnitsEnd - game->Units);
    return 0;
 }

 u->Init(type, cell, 0);
 u->Team = game->GetNewTeam();
 u->Seed = game->Rnd.Raw16();
 return u;
}





colorC* unitC::Color()
{
 if(Owner) return Owner->Color;
 return colorNeutral();
}





/*=============================================================================
 * SQUARED DISTANCE
 */
float unitC::operator*(unitC* b)
{
 float dw = X - b->X;
 float dh = Y - b->Y;
 return dw*dw + dh*dh;
}



/*=============================================================================
 * HERO BONUS
 */
int unitC::HeroBonus(long h)
{
 if(!Hero)
    if(Type->HeroBase & h)
	if(Hero->Bonuses & h) return 2;
	else return 1;
 return 0;
}





/*=============================================================================
 * AMMO IS MAGIC
 */
bool utypeC::AmmoIsMagic()
{
 if(Ammo == AMMO_V_MAGIC) return true;
 if(Ammo == AMMO_B_MAGIC) return true;
 if(Ammo == AMMO_F_MAGIC) return true;
 if(Ammo == AMMO_E_MAGIC) return true;
 return false;
}







/*=============================================================================
 * STATUS
 */
bool unitC::IsInOverland()	{ return Cell->Battle ? false : true; }
bool unitC::IsInBattle()	{ return Cell->Battle ? true : false; }
bool unitC::Exists()		{ return Type ? true : false;		}
bool unitC::IsDead()		{ return Exists() && Figures() < 1;	}
bool unitC::IsAlive()		{ return Exists() && Figures() > 0;	}





void unitC::Die()
{
 Action = actionC::Get("die");
 ActionStatus = 0;
 Animation = animationC::Get("die");

 BattleOrders.Set(&uobDEAD, false);

 SelectionMask = 0;
 Exp += 10;
}



/*=============================================================================
 * SELECTION
 */
bool unitC::IsSelectedBy(long mask)	{ return SelectionMask & mask;}
bool unitC::IsSelectedBy(brainC* b)	{ return IsSelectedBy(b->Bit); }
void unitC::Select(long mask)		{ SelectionMask |= mask; }
void unitC::Deselect(long mask)		{ SelectionMask &= ~mask;}
void unitC::DeselectAll(long mask)
{
 for(unitC* u = game->Units; u < game->UnitsEnd; u++) u->Deselect(mask);
}
void unitC::SelectOnly(long mask)
{
 DeselectAll(mask);
 Select(mask);
}





/*=============================================================================
 * FIND BEST TARGET
 */
unitC* unitC::FindBestTarget()
{
 unitC* u;
 unitC* best = 0;
 double bestD = 1.0e10;

 for(u = Cell->Unit; u; u = u->Next)
    if(u->Owner != Owner && u->IsAlive())
 {
    double w = u->X - X;
    double h = u->Y - Y;
    double d = w*w + h*h;
    if(d < bestD) bestD = d, best = u;
 }

 return best;    
}





/*=============================================================================
 * OVERLAND
 */
void unitC::NextTurn()
{
 TimeAvailable += 6;
 if(TimeAvailable > 6) TimeAvailable = 6;

 // heal
 //@@ if(!undead)
    if(Wounds) Wounds -= game->Roll(Type->Figures*Type->Life, 1)/2;
 if(Wounds < 0) Wounds = 0;

 // clear path, so that in battle units will be laid in the center
 // (ie, PrevDirection() will return -1)
 if(LandOrders.Path.NextDirection() == -1) LandOrders.Path.Clear();
}





/*=============================================================================
 * THINK
 */
void unitC::Think()
{
 unitOrderListC* orders;
 if(Cell->Battle)	orders = &BattleOrders;
 else			orders = &LandOrders;

 if(Action->IsCompleted(this))
 {
    // if order completed 
    if(orders->WP->Order->IsCompleted(this))
	orders->Pop();

    // reset some statuses
    ActionStatus = 0;
    orders->WP->Order->SetAction(this);	// change action

    // set animation
    char* ani = Action->Init(this);
    Animation = animationC::Get(ani);
 }

 // execute action
 Action->Exec(this);
 while(AnimationTime >= 1.)
    AnimationTime -= 1.;
}





/*=============================================================================
 * BATTLE INIT
 *//*
void unitC::BattleInit()
{
 Ammo = Type->Ammo; // only now ammo gets reloaded!
// (*Cell->Battle)((int)X, (int)Y)->Occupied = true;

// Action = actionC::Get("wait");
// unitOrderC* orderGet(char*);
// BattleOrders.Set(0, false);
}
*/




/*=============================================================================
 * PATH
 */
pathC::pathC()
{
 Start = Status = End = NULL;
}

pathC::~pathC()
{
 Clear();
}



void pathC::Make(char* path, unsigned length)
{
 Clear();
 Start = Status = new char[length];
 End = Start + length;
 for(int l = 0; l < length; l++) Start[l] = path[l];
}



void pathC::Clear()
{
 if(Start) delete[] Start;
 Start = Status = End = NULL;
}



void pathC::Pop()
{
 if(Status < End) Status++;
}



signed pathC::NextDirection()
{
 if(Status < End) return Status[0]&7;
 return -1;
}



signed pathC::PrevDirection()
{
 if(Status > Start) return Status[-1]&7;
 return -1;
}





/*=============================================================================
 * ENTER and EXIT TRANSPORT
 */
bool unitC::TeamCanEnterTransportsAt(cellC* dest)
{
 // how many units need transportation?
 int team_size = 0;
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->Team == Team)
    if(u->Type->Transport) return false; // transporters cannot be transported
    else team_size++;

 // how many places are available for transporting?
 signed avl_space = 0;
 for(unitC* u = dest->Unit; u; u = u->Next) if(u->Owner == Owner)
    if(u->Transported) avl_space--;
    else avl_space += u->Type->Transport;

 return avl_space >= team_size;
}





static unitC* get1stAvlTransporterFor(unitC* l)
{
 for(unitC* t = l->Cell->Unit; t; t = t->Next)
    if(t->Owner == l->Owner) if(t->Type->Transport)
	if(!t->Travel || t->Travel->StatusCode != TRAVEL_ANIMATING)
	{
    	    //count loaded units
	    int cnt = 0;
	    for(unitC* k = t->NextTransported; k; k = k->NextTransported) cnt++;
	    if(cnt < t->Type->Transport) return t;
	}
}

bool unitC::TeamEnterTransportsAt(cellC* dest)
{
 if(!TeamCanEnterTransportsAt(dest)) return false;


 // shall Owner->SelectedTeam be changed
 bool updateSelectedTeam = Owner && Owner->SelectedTeam == Team;

 // unload units, in case they are transported
 TeamExitTransport();

 // displace team
 pf("team enter trans displace\n");
 TeamDisplace(dest);

 // team will be spreaded among transporters, back it up
 long team = Team;


 // load all units
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->Team == team)
 {
    // find first available transporter
    unitC* t = get1stAvlTransporterFor(u);

    // chain unit
    unitC** h;
    for(h = &t->NextTransported; *h; h = &(*h)->NextTransported);
    *h = u;
    u->Transported = t;

    // change team
    u->Team = u->Transported->Team;

    // update unit
    u->TimeAvailable = 0;
    u->LandOrders.Clear();
 }


 // select a new team?
 if(updateSelectedTeam)
    Owner->SelectedTeam = Team;
}



void unitC::TeamExitTransport()
{
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->Team == Team)
    if(u->Transported)
    {
	u->Transported->TimeAvailable = 0;

	unitC** h;
	for(h = &u->Transported->NextTransported; *h != u; h = &(*h)->NextTransported);
	*h = u->NextTransported;
	u->NextTransported = 0;
	u->Transported = 0;
    }
}





/*=============================================================================
 * CONSTRUCTOR, MOVEMENT, INITIALIZATION, REMOVAL
 *
 * These functions must be the only ones to modify cellC::Unit
 * chains, including unitC::Next.
 *
 * When a unit is removed, its Cell and Type are 0.
 * When a unit is in game, both are guaranteed to have meaningful values.
 *
 * EVERY movement in the overland map is forced on the WHOLE team.
 */
unitC::unitC()
{
 Type = 0;
 Cell = 0;
 Next = 0;
 Owner = 0;
 Transported = 0;
 NextTransported = 0;
}





void unitC::TeamDisplace(cellC* dest)
{
 if(Cell == dest) return;

 // change player focus to follow moving units
 if(Owner->Focus == Cell)
    if(Owner->SelectedTeam == Team)
	Owner->Focus = Owner->LocalFocus = dest;

 // find last element of the chain
 // units will be attached here
 unitC** l;
 for(l = &dest->Unit; *l; l = &(*l)->Next);

 // displace units, from old chain to new chain
 unitC** h = &Cell->Unit;
 while(unitC* u = *h)
    if(	(!u->Transported && u->Team == Team)
	||		
	(u->Transported && u->Transported->Team == Team))
    {
	//pf("displacing unit of team %d\n", u->Team);

	// detatch from old chain
	*h = u->Next;

	// clean
	u->Next = 0;

	// attach at the end of dest chain
	*l = u;

	// advance last position of dest chain
	l = &u->Next;

	// update position
	u->Cell = dest;
    }
    else h = &u->Next; // go to next unit in the old chain
}





void unitC::Init(utypeC* type, cellC* cell, heroC* hero)
{
 // check
 if(!type) return;
 if(!cell) return;

 // clear
 for(char* z = ZeroStart; z < ZeroEnd; z++) *z = 0x00;
 Owner = 0;

 // set unit as existing and attach to a cell chain
 Type = type;
 Cell = cell;
 unitC** h;
 for(h = &Cell->Unit; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 // no travel
 Travel = 0;

 // set base actions and orders
 Action = actionC::Get("wait");
 Animation = animationC::Get("idle");

 TimeAvailable = 6;

 Wounds = 0;
 Ammo = 0;
 X = -1;

 if(!Type->HeroBase) Hero = 0;
 else
 {
    if(hero) Hero = hero;
    else Hero = new heroC(Type);

    Hero->Unit = this;
    Owner = Hero->Owner;
    Exp = Hero->Exp;
 }
}





void unitC::Remove()
{
 Type = 0;

 if(!Cell) return;

 unitC** h;
 for(h = &Cell->Unit; *h != this; h = &(*h)->Next);
 *h = Next;
 Cell = 0;
}





//EOF =========================================================================
