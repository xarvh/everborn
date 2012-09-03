/*
 UNIT_MECHANICS.CPP	v0.02
 Units rules.

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

#include "spell.h"
#include "city.h"
#include "race.h"
#include "map.h"
#include "particles.h"
#include "counters.h"


//@@
#define DAMAGE_TYPE_MELEE	0
#define DAMAGE_TYPE_RANGED	1



/*=============================================================================
 * RANKS
 *
 * Bonus and titles for each rank.
 * Bonus for heroes are calculated separedly.
 */
rankT unitRanksA[] = {		     //At Ac Def Re Li
 {   0,	"Hero",		"Recruit",	0, 0, 0, 0, 0},
 {  20,	"",		"Regular",	1, 0, 0, 1, 0},
 {  60,	"Captain",	"Veteran",	1, 0, 1, 2, 0},
 { 120,	"Commander",	"Elite",	2, 1, 1, 3, 1},
 { 200,	"Champion",	"UltraElite",	2, 2, 1, 4, 1},
 { 300,	"Lord",		"Champion",	3, 3, 2, 5, 2},
 { 450,	"GrandLord",	0,		0, 0, 0, 0, 0},
 { 650,	"SuprHero",	0,		0, 0, 0, 0, 0},
 {1000,	"Demi God",	0,		0, 0, 0, 0, 0},
};


//static int unitHeroMiscBonus(int rank)	{ return rank; }
//static int unitHeroArmorBonus(int rank) { return (rank+1)/2; }
//static int unitHeroToHitBonus(int rank) { return (rank+1)/3; }



rankT* unitC::Rank()
{
 if(IsCreature()) return 0;
 return unitRanksA + Level();
}



int unitC::Level()
{
 // magic creatures don't level up
 if(IsCreature()) return 0;

 // base level
 int limit = Hero ? 9 : 4;
 int k;
 for(k = 1; k < limit; k++)
    if(Exp < unitRanksA[k].ExpThreshold)
	break;
 k--;

 // heroism
 if(k < 3 && EnchTemp[UENCH_HEROISM]) k = 3;
 else EnchTemp[UENCH_HEROISM] = 0;

 // warlord
 if(!Hero && Owner && Owner->IsThe(Warlord)) k++;

 // crusade
 if(!Hero && Owner && Owner->HasGEnch(GENCH_CRUSADE)) k++;
 
 return k;
}





/*=============================================================================
 * UPKEEP
 */
signed unitC::FoodUpkeep()
{
 //@@ if(undead) return 0;
 return Type->Food;
}

signed unitC::GoldUpkeep()
{
 //@@ if(undead) return 0;


 if(Hero) return 0; //@@ just for now
 return Type->Gold;
}

signed unitC::ManaUpkeep()
{
 int par = Type->Mana;

 for(spellC* s = spellsHook; s; s = s->SpellNext)
    if(s->Type == &spellTypeUnit && s->Object != SPELL_OBJ_NONE)
	if(EnchPerm[s->Object])
	    par += s->UpkeepCost;

 if(Owner && Owner->IsThe(Channeller)) return par/2;
 return par;
}





/*=============================================================================
 * EXPERIENCE ADD
 */
void unitC::ExpAdd()
{
 if(brainC* b = PlayerMage())
 {
    b->SkillPool++;
    return;
 }

 Exp++;

 if(Hero)
    if(Exp == Rank()->ExpThreshold)
    {
	//@@ event: hero levelup
	if(Owner == self) pe("%s has made a level!!\n", Hero->Name);
    }
}





/*=============================================================================
 * FLIGHT
 */
bool unitC::CanFly()
{
 if(Type->Move & MOVE_FLY) return true;
 //@@ flight spell
 //@@ chaos channels, flight
}



/*=============================================================================
 * SIGHT
 */
bool unitC::CanDetect()
{
 if(Type->Def & DEF_ILLUSION) return true;
 if(Type->Spc & SPC_UNDEAD) return true;	//@@?

 if(EnchTemp[UENCH_TRUE_SIGHT]) return true;
 //@@ if( ZOMBIE ) return true;

 return false;
}



bool unitC::BACIView()
{
 if(CanFly()) return true;
 if(Type->Cmd & CMD_OUTPOST) return true;
 if(HeroBonus(HERO_SCOUT)) return true;

 return false;
}



/*=============================================================================
 * STATS
 */
signed unitC::Figures()
{
 return Type->Figures - Wounds/Health();
}



signed unitC::Health()
{
 countersZero();
 countersAdd(Type->Name, Type->Life);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, Level());
    else	countersAdd(r->NameUnit, r->Life);

 switch(HeroBonus(HERO_MIGHTY)) {
    case 1: countersAdd("Mighty", Level()); break;
    case 2: countersAdd("*Really* Mighty", 3*Level()/2); break;
 }

 if(EnchTemp[UENCH_LIONHEART]) countersAdd(spell_Lionheart.Name, 3);
 if(Owner && Owner->HasGEnch(GENCH_CHARM_OF_LIFE))
    countersAdd(spell_CharmOfLife.Name, Type->Life/4? Type->Life/4 : 1);

 return countersTotal();
}



signed unitC::Defense()
{
 countersZero();
 countersAdd(Type->Name, Type->Defense);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, (Level()+1)/2);
    else	countersAdd(r->NameUnit, r->Defense);

 //@@ +1 Mithril, +2 Adamantium

 //@@ +2 aligned node

 //@@ +1 stone skin
 if(EnchTemp[UENCH_HOLY_ARMOR]) countersAdd(spell_HolyArmor.Name, 2);
 //@@ +5 iron skin

 return countersTotal();
}



signed unitC::Resist()
{
 countersZero();
 countersAdd(Type->Name, Type->Resist);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, Level());
    else	countersAdd(r->NameUnit, r->Resist);

 //@@ +2 aligned node

 //@@ +2 Nearby unit with resist for all

 if(EnchTemp[UENCH_LIONHEART]) countersAdd(spell_Lionheart.Name, 3);

 return countersTotal();
}



signed unitC::Melee()
{
 countersZero();
 if(!Type->Melee) return countersTotal();
 countersAdd(Type->Name, Type->Melee);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, (Level()+1)/2);
    else	countersAdd(r->NameUnit, r->Attack);

 switch(HeroBonus(HERO_MIGHTY)) {
    case 1: countersAdd("Mighty", Level()); break;
    case 2: countersAdd("*Really* Mighty", 3*Level()/2); break;
 }

 if(brainC* player = PlayerMage())
    if(player->IsThe(Warlord))
	countersAdd(Warlord.Name, 6);




 //@@ +1 Mithril, +2 Adamantium


 //@@ +2 aligned node

 //@@ +1 giant strength
 //@@ +1 black channels
 //@@ +2 flame blade
 if(EnchTemp[UENCH_LIONHEART]) countersAdd(spell_Lionheart.Name, 3);

 //@@ X leadership bonus

 return countersTotal();
}



signed unitC::Poison()
{
 countersZero();
 if(!Type->Poison) return countersTotal();
 countersAdd(Type->Name, Type->Poison);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, (Level()+1)/2);
    else	countersAdd(r->NameUnit, r->Attack);

 //@@ +2 aligned node

 //@@ +1 giant strength
 //@@ +1 black channels

 return countersTotal();
}



signed unitC::Range()
{
 countersZero();
 if(!Type->Range) return countersTotal();
 countersAdd(Type->Name, Type->Range);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, (Level()+1)/2);
    else	countersAdd(r->NameUnit, r->Attack);


 if(Type->AmmoIsMagic()) switch(HeroBonus(HERO_ARCANEPOWER)) {
    case 1: countersAdd("Arcane Power", Level()/2); break;
    case 2: countersAdd("High Arcane Power", 3*Level()/4); break;
 }


 //@@ +1 Mithril, +2 Adamantium (no magic)

 //@@ +2 aligned node

 //@@ +2 flame blade (no magic)
 //@@ +1 black channels

 if(EnchTemp[UENCH_LIONHEART])
    if(Type->RangeT == AMMO_SLING || Type->RangeT == AMMO_JAVELIN)
	countersAdd(spell_Lionheart.Name, 3);

 return countersTotal();
}





signed unitC::HitModifier()
{
 countersZero();
 countersAdd(Type->Name, Type->Accuracy);

 if(rankT* r = Rank())
    if(Hero)	countersAdd(r->NameHero, (Level()+1)/3);
    else	countersAdd(r->NameUnit, r->Accuracy);

 switch(HeroBonus(HERO_FINESSE)) {
    case 1:
    case 2: countersAdd("Finesse", Level()/2); break;
 }



 if(EnchTemp[UENCH_HOLY_WEAPON]) countersAdd(spell_HolyWeapon.Name, 1);
 //@@ +1 Magic/Mithril/Adamantium

 return countersTotal();
}





/*=============================================================================
 * TAKE DAMAGE
 */
void unitC::TakeDamage(signed amount, int type)
{
 if(amount <= 0) return;

 // spawn blood
 if(Cell->Battle)	//@@ use different blood/wound type
    for(int i = 0; i < amount; i++) new particleBloodC(this);

 ExpAdd();
 Wounds += amount;
 if(Figures() < 1) Die();
}





/*=============================================================================
 * INFLICT MELEE DAMAGE
 */
void unitC::InflictMeleeDamage(unitC* targ)
{
 signed damage = 0;


 if(Melee())
    for(int f = 0; f < Figures(); f++)
    {
	signed d = game->Roll(Melee(), 3+HitModifier()) - game->Roll(targ->Defense());
	if(d > 0) damage += d;
    }


 if(Poison())
    for(int f = 0; f < Figures(); f++)
    {
	signed d = Poison() - game->Roll(targ->Resist());
	if(d > 0) damage += d;
    }



 if(damage > 0) targ->TakeDamage(damage, DAMAGE_TYPE_MELEE);
 else ExpAdd();
}





/*=============================================================================
 * INFLICT RANGED DAMAGE
 */
void unitC::InflictRangedDamage(unitC* targ, int times)
{
 signed d, damage = 0;
 for(int f = 0; f < times; f++)
 {
    d = game->Roll(Range(), 3+HitModifier()) - game->Roll(targ->Defense());
    if(d > 0) damage += d;
 }

 if(damage > 0) targ->TakeDamage(damage, DAMAGE_TYPE_RANGED);
 else ExpAdd();
}





/*=============================================================================
 * CAN SHOOT
 * Checks ranged attack and ammo.
 */
bool unitC::CanShoot()
{
 // utype
 if(Type->Range == 0) return false;

 // ammo
 if(Ammo < 1) return false;

 return true;
}





/*=============================================================================
 * CAN PLANE SHIFT
 */
bool unitC::CanPlaneShift()
{
 if(game->Ench[GENCH_PLANAR_SEAL]) return false;

 if(Cell->Battle) return false;	// no shifting during battles!

 if(Type->Cmd & CMD_PLANE_SHIFT) return true;
 if(EnchTemp[UENCH_PLANAR_TRAVEL]) return true;

 //@@ actually, astral gate could allow plane travel during battle,
 // so it could be placed before the check for battle
 if(Cell->City && Cell->City->Ench[CENCH_ASTRAL_GATE]) return true;

 return false;
}





/*=============================================================================
 * MELEE DISTANCE VS
 */
float unitC::MeleeDistanceVS(unitC* t)
{
 float distance = 1.3;

 if(Type->Att & ATT_FS)
    if(!(t->Type->Def & DEF_NFS))
	distance *= 1.3;

 return distance;
}





/*=============================================================================
 * RANGE DISTANCE VS
 */
float unitC::RangeDistanceVS(unitC* t)
{
 return 15;
}





/*=============================================================================
 * IS IN BATTLEFIELD
bool unitC::IsInBattlefield()
{
 if(X < 1.0) return false;
 if(Y < 1.0) return false;
 if(X > BATTLE_W - 2.0) return false;
 if(Y > BATTLE_H - 2.0) return false;
 return true;
}
 */



//EOF =========================================================================
