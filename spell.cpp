/*
 SPELL.CPP		v0.00

*/

#include "main.h"
//#include "menu.h"

#include "spell.h"
#include "game.h"
#include "map.h"
#include "unit.h"
#include "city.h"
//#include "cmd.h"


/*=============================================================================
 *
 */
spellC* spellsHook = 0;
realmC* realmsHook = 0;





void spellResetAll()
{
 for(spellC* s = spellsHook; s; s = s->SpellNext)
    for(int i = 0; i < GAME_MAX_BRAINS; i++)
	s->ResearchLeft[i] = s->ResearchCost;
}





/*=============================================================================
 * BONUSES FOR MANY BOOKS OF THE SAME REALM 
 *
    3	Major Books:	4 spells
    6	MinorBooks:	5 spells

i,ii,iii,iv,v: starting %known on 1,2,3,4,5 level spell of minor books.
I,II,III,IV: starting %known on 1,2,3,4 level spell of major books.
Cast: spell casting cost bonus

		i   ii   iii   iv   v      I   II   III   IV		Cast
6books:
    6mi		K   80    70   60   50					30
    5mi+1ma	K   70    60   50   40     30  10			25
    4mi+2ma	K   60    50   40   30     50  30    10			20
    3mi+3ma	K   50    40   30   20     K   50    30   10		10

5books:
    5mi		K   60    50   40   30					25
    4mi+1ma	K   50    40   30   20     30  10  			20
    3mi+2ma	K   40    30   20   10     50  30    10			10

4books:
    4mi		K   40    30   20   	   10				20
    3mi+1ma	K   30    20   10	   30  10			10

3,2,1books:
    3mi		K   20    10						10
    2mi		K   10
    1mi		K
*/
int spellPercentualBonusToCastingCost(int nmin)
{
 if(nmin == 6) return 30;
 if(nmin == 5) return 25;
 if(nmin == 4) return 20;
 if(nmin == 3) return 10;
 return 0;
}



int spellPercentualBonusToResearchOnSpellsInMinorBooks(
    int nmaj,	// number of major books of this realm possessed
    int nmin,	// number of minor books of this realm possessed
    int level)	// level of the spell (1-5 minor, 1-4 major)
{
 // one spell is always known
 if(level == 1) return 100;

 signed b = 80;
 b -= 10 * (6-nmin);		// remove 10 for any minor book less than 6
 b -= 10 * (6-nmin-nmaj);	// remove 10 for any book less than 6
 b -= 10 * (level-2);		// remove 10 for any level over ii

 // there you go
 return b < 0 ? 0 : b;
}



int spellPercentualBonusToResearchOnSpellsInMaiorBooks(
    int nmaj,	// number of major books of this realm possessed
    int nmin,	// number of minor books of this realm possessed
    int level)	// level of the spell (1-5 minor, 1-4 major)
{
 // known spells
 if(level == 1 && nmaj == 3 && nmin == 3) return 100;

 signed b = 70;
 b -= 20 * (3-nmaj);		// remove 10 for any minor book less than 3
 b -= 20 * (6-nmin-nmaj);	// remove 10 for any book less than 6
 b -= 20 * (level-1);		// remove 10 for any level over I

 // there you go
 return b < 0 ? 0 : b;
}


void spellC::ApplyStartingResearchBonus(int mi, int ma)
{
 if(!Book) return;

 int b;
 if(Book->Maior)
    b = spellPercentualBonusToResearchOnSpellsInMaiorBooks(ma, mi, Level);
 else
    b = spellPercentualBonusToResearchOnSpellsInMinorBooks(ma, mi, Level);

 ResearchLeft[Book->Pick->Owner->Id] -= (b * ResearchCost) / 100;
}












/*=============================================================================
 * ENCHANTEMENT HELPERS
 */
void spellEnchUnit(spellLaunchC* l)
{
 spellC* s = l->Spell;
 brainC* b = l->Caster;
 unitC* u = l->Unit;

 u->EnchTemp[s->Object] |= b->Bit;
 if(!l->Temp) u->EnchTemp[s->Object] |= b->Bit;

 //@@ local event notification?
 pf("%s has cast %s on %s.\n", b->Name, s->Name, u->Name());
}



void spellEnchCity(spellLaunchC* l)
{
 spellC* s = l->Spell;
 brainC* b = l->Caster;
 cityC* c = map(l->X, l->Y, l->Z)->City;

 //@@ this should not happen, but let's debug....
 if(!c) { pe("%s: no city!\n", s->Name); return; }

 c->Ench[s->Object] |= b->Bit;

 //@@ local event notification?
 pf("%s has cast %s at %s.\n", b->Name, s->Name, c->Name);
}



void spellEnchBattle(spellLaunchC* l)
{
 spellC* s = l->Spell;
 brainC* b = l->Caster;
 battleC* ba = map(l->X, l->Y, l->Z)->Battle;

 //@@ this should not happen, but let's debug....
 if(!ba) { pe("%s: no battle!\n", s->Name); return; }

 ba->Ench[s->Object] |= b->Bit;

 //@@ local event notification?
 pf("%s has cast %s.\n", b->Name, s->Name);
}



void spellEnchGlobal(spellLaunchC* l)
{
 spellC* s = l->Spell;
 brainC* b = l->Caster;

 game->Ench[s->Object] |= b->Bit;

 //@@ local event notification?
 pf("%s has cast %s.\n", b->Name, s->Name);
}





/*=============================================================================
 * TYPE HELPERS
 */
int spellC::CheckTargetAlliance(brainC* caster, brainC* target)
{
 // check Target ALLIANCE
 switch(TargetAlliance) {
    case SPELL_TALLIANCE_IGNORE: return 0;
    case SPELL_TALLIANCE_NEUTRAL: return 0;

    case SPELL_TALLIANCE_ENEMY:
	if(!target) return 0;
	if(caster->EngageMask & target->Bit) return 0;
	return -1;

    case SPELL_TALLIANCE_FRIENDLY:
	if(!target) return -1;
	if(caster->FriendMask & target->Bit) return 0;
	return -1;
 }
}





/*=============================================================================
 * TYPE: UNIT SPELL
 */
void spellType_UnitG(spellLaunchC* l)
{
 l->Unit = game->Units + arg1.i;
}

int spellType_UnitC(spellC* s)
{
 brainC* b = cmdGetBrain();
 unitC* u = game->Units + arg1.i;

 if(s->CheckTargetAlliance(b, u->Owner)) return -1;


 if(s->TargetStatus & SPELL_TSTATUS_ALIVE) if(!u->IsAlive()) return -1;
 if(s->TargetStatus & SPELL_TSTATUS_DEAD) if(u->IsAlive()) return -1;
 if(s->TargetStatus & SPELL_TSTATUS_HERO) if(!u->Hero) return -1;
 if(s->TargetStatus & SPELL_TSTATUS_DONTALREADYHAVEIT)
    ;//@@

 if(s->TargetStatus & SPELL_TSTATUS_SUMMONED)
    ;//@@

 if(s->TargetStatus & SPELL_TSTATUS_HASAMMO)
    ;//@@

 if(s->TargetStatus & SPELL_TSTATUS_ELITE)
    ;//@@

 if(s->TargetStatus & SPELL_TSTATUS_DEATH)
    ;//@@

 return 0;
}

spellTypeC spellTypeUnit = {
    "unit",
    "U: TargetUnit",
    "land",
    spellType_UnitG,
    spellType_UnitC
};





/*=============================================================================
 * TYPE: CITY SPELL
 */
void spellType_CityG(spellLaunchC* l)
{
 l->X = arg1.i;
 l->Y = arg2.i;
 l->Z = arg3.i;
}

int spellType_CityC(spellC* s)
{
 brainC* b = cmdGetBrain();

 //@@ check that X and Y actually refer to a city

 //@@ check that target fits spell TALLIANCE and TSTATUS
 return 0;
}

spellTypeC spellTypeCity = {
    "city",
    "III: CityX CityY CityZ",
    "land",
    spellType_CityG,
    spellType_CityC
};





/*=============================================================================
 * TYPE: UNIT BATTLE
 */
void spellType_BattleG(spellLaunchC* l)
{
 l->X = arg1.i;
 l->Y = arg2.i;
}

int spellType_BattleC(spellC* s)
{
 //@@ check that there actually is a battle there

 //@@ check that player can cast in on that battle:
 //-> view of the cell only should be required
 return 0;
}

spellTypeC spellTypeBattle = {
    "battle",
    "II: BattleX BattleY",
    "land",
    spellType_BattleG,
    spellType_BattleC
};





/*=============================================================================
 * TYPE: MAP
 */
void spellType_MapG(spellLaunchC* l)
{
 l->X = arg1.i;
 l->Y = arg2.i;
 l->Z = arg3.i;
}

int spellType_MapC(spellC* s)
{
 //@@ check that player can cast in on that square:
 //-> view of the cell only should be required
 return 0;
}

spellTypeC spellTypeMap = {
    "map",
    "III: MapX MapY MapZ",
    "land",
    spellType_MapG,
    spellType_MapC
};





/*=============================================================================
 * TYPE: GLOBAL
 */
void spellType_GlobalG(spellLaunchC* l)
{
}

int spellType_GlobalC(spellC* s)
{
 return 0;
}

spellTypeC spellTypeGlobal = {
    "global",
    ":",
    0,
    spellType_GlobalG,
    spellType_GlobalC
};





/*=============================================================================
 * TYPE: SUMMON
 */
void spellType_SummonG(spellLaunchC* l)
{
}

int spellType_SummonC(spellC* s)
{
 return 0;
}

spellTypeC spellTypeSummon = {
    "summon",
    ":",
    0,
    spellType_SummonG,
    spellType_SummonC
};





/*=============================================================================
 * SPELL QUEUE
 */
spellLaunchC::spellLaunchC()
{
 Spell = 0;
 Caster = 0;
 Next = 0;

 Charge = 0;
 Temp = false;
}

spellLaunchC::~spellLaunchC()
{
 if(Caster)	// unqueue from brain's list
 {
    spellLaunchC** h = &Caster->SpellQueue;
    while(*h && *h != this) h = &(*h)->Next;
    if(*h) *h = Next;
 }
}



unsigned spellLaunchC::Cost()
{
 int mi = Spell->Book ? Caster->MinorBooks(Spell->Book->Realm) : 0;
 int b = spellPercentualBonusToCastingCost(mi);

 //@@ other bonuses apply

 if(Temp)	return ((Spell->BattleCost + Charge)*(100-b))/100;
 else		return ((Spell->OverlandCost + Charge)*(100-b))/100;
}

signed spellLaunchC::TurnsRequired()
{
 signed cost = Cost();
 cost -= Caster->SkillAvl;

 if(this == Caster->SpellQueue)
    cost -= Caster->SkillCommitted;

 if(cost <= 0) return 0;
 return 1 + (cost / Caster->Skill);
}




/*=============================================================================
 * REALM
 */
realmC::realmC(int code, char* name, char* desc, float r, float g, float b, bool pn)
{
 realmC** h;
 for(h = &realmsHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Code = code;
 Name = name;
 Desc = desc;
 Color[0] = r;
 Color[1] = g;
 Color[2] = b;
 BooksHook = 0;
 PowerNode = pn;
}





/*=============================================================================
 * BOOK
 */
bookC::bookC(realmC* realm, char* id, char* name, char* history, bool m)
{
 bookC** h;
 for(h = &realm->BooksHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Id = id;
 Name = name;
 History = history;
 Realm = realm;
 Maior = m;
 Pick = 0;
 SpellsHook = 0;

 // register book as a viable pick
 int req = Maior? (1<<Realm->Code) : 0;
 Pick = new pickC(Id, Name, History, req);
 Pick->Book = this;
}



/*=============================================================================
 * SPELL HELPERS
 */
static int scmp(char* a, char* b)
{
 while(*a && *a == *b) a++, b++;
 return *a || (*b && *b != ' ' && *b != '\t');
}

spellC* spellGet(char* id)
{
 for(spellC* s = spellsHook; s; s = s->SpellNext)
    if(!scmp(s->Cmd.Name, id)) return s;
 return 0;
}



/*=============================================================================
 * SPELL CMD FUNCTION WRAPPERS
 */
static int spellGameF()
{
 // retrive spell pointer
 spellC* spell = (spellC*)arg_cmd->Obj;

 // get brain
 brainC* brain = cmdGetBrain();

 // make spell launch parameters
 spellLaunchC* launch = new spellLaunchC;
 launch->Spell = spell;
 launch->Caster = brain;
 launch->Temp = (*arg0.s == '-');	// is this the temporary version of the spell?
 launch->Charge = 0;			//@@ check spell charge
 spell->Type->CmdToLaunch(launch);	// interprete command args

 // exec temporary spells at once
 if(launch->Temp)
 {
    //@@ if temp spell, player available skill must exceed spell cost

    brain->CheckCast(launch);
    return 0;
 }

 // queue non-temporary spell
 if(*arg0.s != '+')		// if spell is not to be queued
    brain->ClearSpellQueue();	// cancel current queue
 brain->QueueSpell(launch);

 // SkillAvailable may be enough for spell to be cast at once, so let's check
 brain->CheckCast();

 return 0;
}



static int spellCheckF()
{
 // get spell
 spellC* spell = (spellC*)arg_cmd->Obj;

 // get brain
 brainC* brain = cmdGetBrain();

 // check that playermage is alive
 if(!brain->PlayerMage->IsAlive()) return -1;

 // check that the player knows the spell
 if(spell->ResearchLeft[brain->Id]) return -1;

 // check that the player owns the book containing the spell
 if(spell->Book && spell->Book->Pick->Is(brain)) return -1;

 return spell->Type->Check(spell);
}



/*=============================================================================
 * SPELL
 */
spellC::spellC(SPELL_CON_PARMS) :
 // construct bound command
 Cmd(
    id,
    type->Syntax,	// will be set by subclass
    0,			// no help
    0,			// no exec function
    spellGameF,
    spellCheckF)
{
 // register spell address, so that Cmd can retrieve Spell
 Cmd.Obj = (void*)this;

 // add to main spell dynamic list
 spellC** h;
 for(h = &spellsHook; *h; h = &(*h)->SpellNext);
 *h = this;
 SpellNext = 0;


 // add to book dynamic list
 if(book)
 {
    for(h = &book->SpellsHook; *h; h = &(*h)->BookNext);
    *h = this;
 }
 BookNext = 0;

 // init attributes
 Book = book;
 Type = type;
 Name = name;
 OneLiner = oneliner;
 Description = description;
 Picture = picture;
 Level = level;
 UpkeepCost = upkeepCost;

 ResearchCost = researchCost;
 BattleCost = battleCost;
 OverlandCost = overlandCost;

 TargetAlliance = targetAlliance;
 TargetStatus = targetStatus;
 Object = object;

 Animation = 0; //getAnimation(animation);

 Cast = cast;	//@@ default se cast == 0?
}





//EOF =========================================================================
