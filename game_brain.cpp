/*
 BRAIN		v0.03

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
#include "map.h"
#include "game.h"
#include "unit.h"
#include "city.h"
#include "race.h"
#include "utype.h"
#include "boxgui.h"
#include "spell.h"


#include "boxgui.h"	//shouldn't be here


/*=============================================================================
 *
 */
bool brainC::IsThe(pickC& pick)
{
 return this == pick.Owner;
}





bool brainC::HasGEnch(int id)
{
 return game->Ench[id] & Bit;
}





/*=============================================================================
 * INIT
 */
//@@ DANGER!!!! THIS IS ---NOT--- a REQUIREMENT!!!
// used for debug only!!!
// REMOVE REMOVE REMOVE
/*
static utypeC* startingCavalry(raceC* r)
{
 utypeC* u;
 for(u = r->Units; u->Role->code != ROLE_CAVALRY; u = u->Next);
 return u;
}
static utypeC* startingFrontline(raceC* r)
{
 utypeC* u;
 for(u = r->Units; u->Role->code != ROLE_ELITE; u = u->Next);
 return u;
}

void initunits(raceC* r, cellC* c, brainC* b)
{
 unitC* u;
 long team = b->PlayerMage->Team; //GetNewTeam();

 for(int i = 0; i < 8; i++)
 {
    u = unitNew(startingCavalry(r), c);
    u->Owner = b;
    u->Team = team;
 }
 for(int i = 0; i < 8; i++)
 {
    u = unitNew(startingFrontline(r), c);
    u->Owner = b;
    u->Team = team;
 }
}
*/
static double grnd()
{
 return game->Rnd();
}






void brainC::InitGame()
{
 // Name, Color and Race are already set

 // system
 Status = BRAIN_ACTIVE;


 // Empire
 EngageMask = ~Bit;	// engage all but yourself!!!
 FriendMask = Bit;
 GoldReserve = 500;
 ManaReserve = 100;
 Taxes = 2;		// 1g every 1000 citizens

 // Allocation
 AllocateToSkill = 40;
 AllocateToResearch = 40;
 AlchemyManaToGold = 0;

 // Magic
 Skill = 10;
 if(this->IsThe(Archmage)) Skill += 10;
 SkillAvl = 0;		// first turn, no spells!
 SkillCommitted = 0;
 SpellQueue = 0;


 // starting position
 cellC* c;
 int startx, starty, startz;
 startz = this->IsThe(Myrran) ? 1 : 0;

 do {
    startx = game->Rnd(game->Map.W);
    starty = game->Rnd(game->Map.H);
    c = map(startx, starty, startz);
 } while(c->Land->Type != TERRAIN_LAND || c->City != NULL);
 Focus = c;
 SummoningCircle = c;


 // give a view of the area
 float radius = 5;
 if(this->IsThe(Sage))
    radius = 10;

 for(cellC* r = game->Map.C; r < game->Map.Cend; r++) if(r->Z == startz)
 {
    int dx = r->X - startx;
    int dy = r->Y - starty;
    if(radius * radius > dx*dx + dy*dy) r->KnowMask |= Bit;
    dx = game->Map.W - (dx>0?dx:-dx);
    if(radius * radius > dx*dx + dy*dy) r->KnowMask |= Bit;
 }

 
 // starting city
 char* s = Race->Language.Generate("CITY", grnd);
 cityC* city = new cityC(s, c, Race);
 city->Owner = this;
 city->TotalPop = 8;
 city->ProdCheck();
 // add buildings
// for(int k = 0; k < BLD_MAX; k++) if(bldC* b = city->Race->Blds[k])
//    if(b->Req != REQ_NON_BUILDABLE) city->Blds |= 1<<k;


 // starting units:
 c = map(startx, starty, startz);
 unitC* u;
 long team;

 // playermage
 u = unitNew(Race->GetPlayerMage(), c);
    u->Owner = this;
    PlayerMage = u;

 team = PlayerMage->Team;

 // heroes
 //@@ starting heroes should have one special hero ability
 u = unitNew(Race->GetHero(game->Rnd.Raw16()), c);
 u->Owner = u->Hero->Owner = this;
// u->Team = team;

 u = unitNew(Race->GetHero(game->Rnd.Raw16()), c);
 u->Owner = u->Hero->Owner = this;
// u->Team = team;






// initunits(Race, c, this);






 // 1 basic fighting units:
 team = game->GetNewTeam();
 for(int i = 0; i < 1; i++)
 {
    u = unitNew(Race->GetSpearmen(), c);
    u->Owner = this;
    u->Team = team;
 }

 // 4 settlers
 team = game->GetNewTeam();
 for(int i = 0; i < 4; i++)
 {
    u = unitNew(Race->GetSettlers(), c);
    u->Owner = this;
    u->Team = team;
 }



 // enter menu
 if(this == self)
 {
    cmd("focus %d %d %d", startx, starty, startz);
    bOpen("land");
//    bOpen("mirror");
    menuLand_CenterAt(startx, starty);
 }
}





/*=============================================================================
 * BOOKS COUNT
 */
int brainC::MinorBooks(realmC* r)
{
 int c = 0;

 for(bookC* b = r->BooksHook; b; b = b->Next)
    if(b->Pick->Owner == this)
	if(!b->Maior) c++;

 return c;
}


int brainC::TotalBooks(realmC* r)
{
 int c = 0;

 for(bookC* b = r->BooksHook; b; b = b->Next)
    if(b->Pick->Owner == this)
	c++;

 return c;
}





/*=============================================================================
 * CAN RESEARCH
 */
bool brainC::CanResearch(spellC* s)
{
 if(s->ResearchLeft[Id] <= 0) return false;
 if(!s->Book) return true;

 if(s->Book->Pick->Owner != this) return false;
 return true;
}



/*=============================================================================
 * CAN CAST
 */
bool brainC::CanCast(spellC* s)
{
 // must have the book
 if(s->Book && !s->Book->Pick->Is(self)) return false;

 // must have researched
 if(s->ResearchLeft[self->Id] > 0) return false;

 return true;
}





/*=============================================================================
 * SPELL COST
 */
unsigned brainC::SpellCost()
{
 return SpellCost(SpellQueue);
}



unsigned brainC::SpellCost(spellLaunchC* spl)
{
 unsigned base = spl->Cost();


 //@@ check cost against retorts
 //@@ part of the code for bonuses is in spellLaunchC::Cost(), not sure if it
 //should be here instead

 unsigned cost = base;

 return cost;
}



/*=============================================================================
 * CHECK CAST
 */
void brainC::CheckCast()
{
 CheckCast(SpellQueue);
}



void brainC::CheckCast(spellLaunchC* l)
{
 signed cost = SpellCost(l);

 if(l == SpellQueue) cost -= SkillCommitted;
 if(cost < 0) cost = 0;	// really needed?

 // is the spell ready to be cast?
 if(cost <= SkillAvl && cost <= ManaReserve)
 {
    SkillAvl -= cost;
    ManaReserve -= cost;

    l->Spell->Cast(l);

    if(l == SpellQueue) SkillCommitted = 0;
    delete l;	// unqueues automatically
 }
}



/*=============================================================================
 * CLEAR SPELL QUEUE
 */
void brainC::ClearSpellQueue()
{
 SkillCommitted = 0;
 while(SpellQueue) delete SpellQueue;
}





/*=============================================================================
 * BRAIN
 */
brainC::brainC()
{
 for(char* z = ZeroStart; z < ZeroEnd; z++) *z = 0x00;
 Status = BRAIN_UNUSED;
 Race = 0;
}

brainC::~brainC()
{
 spellLaunchC* next;
 for(spellLaunchC* q = SpellQueue; q; q = next)
 {
    next = q->Next;
    delete q;
 }
}



/*=============================================================================
 * QUEUE SPELL
 */
void brainC::QueueSpell(spellLaunchC* q)
{
 spellLaunchC** h;
 for(h = &SpellQueue; *h; h = &(*h)->Next);

 *h = q;
 q->Next = 0;

 //@@ if q is first queue element check whether it can be cast
}




/*=============================================================================
 * THINK
 */
void brainC::Think()
{
 // Cast Status
 // max mana that can be committed is minimum between:
 //	Available Skill
 //	Available Mana
 //	Actual Casting cost
 if(SpellQueue)
 {
    int comm = SpellCost();
    if(comm > SkillAvl) comm = SkillAvl;
    if(comm > ManaReserve) comm = ManaReserve;
    if(comm > 0)
    {
	SkillAvl -= comm;
	ManaReserve -= comm;
	SkillCommitted += comm;
	CheckCast();
    }
 }


 // reset skill
 SkillAvl = Skill;


 // advance skill
 SkillPool += DeltaSkill();
 if(SkillPool >= Skill)
 {
    SkillPool -= Skill;
    if(IsThe(Archmage)) Skill += 2;
    else Skill++;
 }


 // advance research
 if(spellC* s = ResearchedSpell)
    if(!s->Book || s->Book->Pick->Owner == this)
    {
	s->ResearchLeft[Id] -= DeltaKnow();
	if(s->ResearchLeft[Id] < 0)
	{
	    //@@ don't waste any research
	    if(this == self)
	    {
		pe("completed researching: %s.\n", ResearchedSpell->Name);
		popupResearchS::Open();
	    }
	    ResearchedSpell = 0;
	}
    }

 // reserves
 GoldReserve += DeltaGold();
 ManaReserve += DeltaMana();



 // handle shortages ==========================================================
 //@@ sistemare: se una unitá viene tolta per il gold si recupera solo quello
 //@@--> eliminare le unitá PRIMA di modificare Gold/Mana-Reserve

 // food: eliminate units
 while(DeltaFood() < 0)
 {
    // search a suitable unit, starting from the newest
    unitC* u;
    for(u = game->UnitsEnd-1; u >= game->Units; u--) if(u->IsAlive())
	if(u->Owner == this && u->FoodUpkeep())
	    break;

    if(u < game->Units) break;

    //@@ event:
    if(this == self) pe("%s deserted, lack of food.\n", u->Name());
    u->Remove();
 }


 //@@ gold1: eliminate buildings


 // gold2: eliminate units
 while(GoldReserve < 0)
 {
    // search a suitable unit, starting from the newest
    unitC* u;
    for(u = game->UnitsEnd-1; u >= game->Units; u--) if(u->IsAlive())
	if(u->Owner == this && u->GoldUpkeep())
	    break;

    if(u < game->Units) break;

    //@@ event:
    if(this == self) pe("%s deserted, no gold.\n", u->Name());
    GoldReserve += u->GoldUpkeep();
    u->Remove();
 }


 // mana1: eliminate units
 while(ManaReserve)
 {
    // search a suitable unit, starting from the newest
    unitC* u;
    for(u = game->UnitsEnd-1; u >= game->Units; u--) if(u->IsAlive())
	if(u->Owner == this && u->Type->Mana)
	    break;

    if(u < game->Units) break;

    //@@ event:
    if(this == self) pe("%s unsummoned, no magic power.\n", u->Name());
    ManaReserve += u->Type->Mana;
    u->Remove();
 }


 // mana2: eliminate unit enchantements
 while(ManaReserve)
 {
    // search a suitable unit, starting from the newest
    unitC* u;
    for(u = game->UnitsEnd-1; u >= game->Units; u--) if(u->IsAlive())
	if(u->Owner == this && u->ManaUpkeep())
	    break;

    if(u < game->Units) break;

    for(int e = 0; e < UENCH_TOTAL && ManaReserve < 0; e++)
	if(u->EnchPerm[e] & Bit)
	{
	    spellC* s;
	    for(s = spellsHook; s; s = s->SpellNext)
		if(s->Type == &spellTypeUnit && s->Object == e)
		    break;

	    //@@ event:
	    if(this == self)
	        pe("%s lost %s, no magic power.\n", u->Name(), s->Name);
	    ManaReserve += s->UpkeepCost;
	    u->EnchPerm[e] &= ~Bit;
	}
 }

 //@@ mana 3: city ench
 //@@ mana 4: global ench



}





/*=============================================================================
 * DELTA
 */
signed brainC::DeltaSkill()
{
 return (DeltaRawMana() * AllocateToSkill) / 100;
}



signed brainC::DeltaRawMana()
{
 signed delta = 10;	// Each wizard has this regardless

 //@@ using counters may be nice
 //@@ with a fortress the wizard should have like 10 more mana/turn

 // units upkeep
 for(unitC* u = game->Units; u < game->UnitsEnd; u++) if(u->Owner == this && u->IsAlive())
    delta -= u->ManaUpkeep();

 // cities
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++) if(c->City && c->City->Owner == this)
    delta += c->City->Mana();

 return delta;
}





signed brainC::DeltaGold()
{
 signed delta = 0;
 //@@ using counters may be nice
 //@@ also, with no cities there should be no gold upkeep

 // units upkeep
 signed uupk = 0;
 for(unitC* u = game->Units; u < game->UnitsEnd; u++) if(u->Owner == this && u->IsAlive())
    uupk += u->GoldUpkeep();
 //@@ remove fame
 if(uupk < 0) uupk = 0;
 delta -= uupk;


 // cities
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++) if(c->City && c->City->Owner == this)
    delta += c->City->Gold();

 // alchemy
 int f = IsThe(Alchemist)? 1:2;
 if(AlchemyManaToGold > 0)
    delta += AlchemyManaToGold;
 if(AlchemyManaToGold < 0)
    delta += AlchemyManaToGold * f;

 return delta;
}



signed brainC::DeltaMana()
{
 signed delta = DeltaRawMana();

 // remove mana allocated for SkillPool and Research
 // it's important to keep the two items separed, so as to get the correct
 // rounding.
 delta -= ((delta * AllocateToSkill) / 100) + ((delta * AllocateToResearch) / 100);

 int f = IsThe(Alchemist)? 1:2;
 if(AlchemyManaToGold > 0)
    delta -= AlchemyManaToGold * f;
 if(AlchemyManaToGold < 0)
    delta -= AlchemyManaToGold;

 return delta;
}



signed brainC::DeltaFood()
{
 //@@ using counters would be nice
 signed delta = 0;

 // units upkeep
 for(unitC* u = game->Units; u < game->UnitsEnd; u++) if(u->Owner == this && u->IsAlive())
    delta -= u->FoodUpkeep();

 // cities
 bool cities = false;
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++) if(c->City && c->City->Owner == this)
 {
    delta += c->City->Food();
    cities = true;
 }


 // no cities, no food required...
 if(!cities) return 0;

 return delta;
}



signed brainC::DeltaKnow()
{
 //@@ using counters may be nice

 signed delta = 0;

 // cities
 for(cellC* c = game->Map.C; c < game->Map.Cend; c++) if(c->City && c->City->Owner == this)
    delta += c->City->Know();

 delta += (DeltaRawMana() * AllocateToResearch) / 100;

 return delta;
}





//EOF =========================================================================
