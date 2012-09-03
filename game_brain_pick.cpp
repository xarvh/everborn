/*
 BRAIN_PICK.CPP		v0.00
 Starting picks.

 Copyright (c) 2004-2008 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "game.h"
#include "spell.h"



#define PICK_REQ(realm)		(1<<(realm))



pickC Myrran(
    "myrran", "Myrran",
    "?>Race is mutated to its Myrran version. ?<\n"
    "?>Player starts on Myrror. ?<\n",
    0
);


pickC Warlord(
    "warlord", "Warlord",
    "?>All normal units are a level higher. ?<\n"
    "?>In battle, Warlord give Leadership bonus to his units. ?<\n"
    "?>+10 Wizard's own Melee attack, can equip melee items. ?<\n",
    0
);


pickC Sage(
    "sage", "Sage",
    "?>+50% research in all Realms. ?<\n"
    "?>Forecasts global events. ?<\n"
    "?>Knows the location of stray Items. ?<\n",
    0
);


pickC Lightlord(
    "lightlord", "Lightlord",
    "?>+50% to Mana obtained from religious buildings. ?<\n"
    "?>+50% to Unrest bonus from religious buildings. ?<\n"
    "?>In battle, Lightlord is Lightbringer. ?<\n",
    PICK_REQ(REALM_LIFE)
);



pickC NodeMaster(
    "nodemaster", "Nodemaster",
    "?>Doubles Mana obtained from Nodes. ?<\n"
    "?>Ignores Nodes dispel effects. ?<\n"
    "?>Knows location and power of all Nodes in both Planes. ?<\n",
    PICK_REQ(REALM_CHAOS) + PICK_REQ(REALM_NATURE) + PICK_REQ(REALM_SORCERY)
);


pickC ChaosMaster(
    "chaosmaster", "Chaosmaster",
    "?>Halves Chaos spells research and casting cost. ?<\n"
    "?>Makes twice as hard to dispel Chaos spells. ?<\n"
    "?>Doubles Mana obtained from Chaos Nodes. ?<\n",
    PICK_REQ(REALM_CHAOS)
);

pickC NatureMaster(
    "naturemaster", "Naturemaster",
    "?>Halves Nature spells research and casting cost. ?<\n"
    "?>Makes twice as hard to dispel Nature spells. ?<\n"
    "?>Doubles Mana obtained from Nature Nodes. ?<\n",
    PICK_REQ(REALM_NATURE)
);

pickC SorceryMaster(
    "sorcerymaster", "Master Sorceror",
    "?>Halves Sorcey spells research and casting cost. ?<\n"
    "?>Makes twice as hard to dispel Sorcery spells. ?<\n"
    "?>Doubles Mana obtained from Sorcery Nodes. ?<\n",
    PICK_REQ(REALM_SORCERY)
);


pickC Famous(
    "famous", "Famous",
    "?>Starts with Fame 10. ?<\n"
    "?>Doubles chances of receiving mercenaries, heroes, items. ?<\n"
    "?>@@ ?<",
    0    
);


pickC Darklord(
    "darklord", "Darklord",
    "?>+50% to Mana obtained from religious buildings. ?<\n"
    "?>+50% to Unrest bonus from religious buildings. ?<\n"
    "?>In battle, @@????. ?<\n",
    PICK_REQ(REALM_DEATH)
);


pickC Conjurer(
    "conjurer", "Conjurer",
    "?>Can summon any creature in combat or in overland. ?<\n"
    "?>25% bonus to research, cast and upkeep cost of summoned creatures. ?<\n"
    "?>Can have the Summoning Circle outside of cities?<\n",
0
);


pickC Charismatic(
    "charismatic", "Charismatic",
    "?>Halves initial cost of mercenaries, heroes and items. ?<\n"
    "?>Reduces Unrest malus for large empires. ?<\n"
    "?>Doubles diplomatic bonuses, halves diplomatic maluses. ?<\n",
0
);


pickC Channeller(
    "channeller", "Channeller",
    "?>Halves spell upkeep cost. ?<\n"
    "?>Casting cost does not increase with map distance. ?<\n",
    0
);


pickC Destroyer(
    "destroyer", "Destroyer",
    "?>+50% to direct attack spells damage. ?<\n"
    "?>+50% to Wizards's own Range attack. ?<\n"
    "?>+30% to research on direct attack spells. ?<\n",
    PICK_REQ(REALM_CHAOS)
);



pickC Artificier(
    "artificer", "Artificer",
    "?>Start game with Create Artifact and Enchant Item spells. ?<\n"
    "?>Halves Create Artifact and Enchant Item casting cost. ?<\n"
    "?>Allows the creation of more powerful items. ?<\n",
    0
);

pickC Archmage(
    "archmage", "Archmage",
    "?>+10 to starting Skill. ?<\n"
    "?>Skill increases twice as fast. ?<\n"
    "?>Archmage's spells are twice as hard to dispel. ?<\n",
    0
);
pickC Alchemist(
    "alchemist", "Alchemist",
    "?>All normal troops are produced with magic weapons. ?<\n"
    "?>Conversion between stored Mana and Gold is 1:1 instead than 2:1. ?<\n"
    "?>Any city with a Library can build a Alchemists's Guild. ?<\n",
    0
);







/*=============================================================================
 * class: PICK
 */
pickC* picksHook = 0;


pickC::pickC(char* id, char* name, char* desc, char req_mask)
{
 Id = id;
 Name = name;
 Desc = desc;
 ReqMask = req_mask;
 Book = 0;

 if(req_mask)	// has requirements, set it at the end
 {
    pickC** h;
    for(h = &picksHook; *h; h = &(*h)->Next);
    *h = this;
    Next = 0;
 }
 else		// no requirements, put at beginning
 {
    Next = picksHook;
    picksHook = this;
 }
}





bool pickC::Is(brainC* b) { return Owner == b; }



/*=============================================================================
 * HELPERS
 */
int brainC::CountPicks()
{
 int cnt = 0;
 for(pickC* p = picksHook; p; p = p->Next)
    if(p->Owner == this) cnt++;
 return cnt;
}





char* brainC::CheckRequirementsFor(pickC* p)
{
 // no requirements!
 if(!p->ReqMask) return 0;


 // standard requirements
 int total = 0;
 for(int k = 0; k < 8; k++) if(p->ReqMask & (1<<k))
 {
    int partial = 0;
    for(int i = 0; i < PICKS_MAX_CHOICE; i++) if(pickC* p = InitialPicks[i])
	if(p->Book && p->Book->Realm->Code == k)
	    partial++;

    if(partial < 1) return "no book of specific realm";
    total += partial;
 }
 if(total < 3) return "not enough books of required realm(s)";



 // life vs death requirements
 bool life = false;
 bool death = false;
    for(int i = 0; i < PICKS_MAX_CHOICE; i++) if(pickC* p = InitialPicks[i])
	if(p->Book)
		    if(p->Book->Realm->Code == REALM_LIFE) life = true;
	    else    if(p->Book->Realm->Code == REALM_DEATH) death = true;
 if(life && death) return "can't have both Life and Death";



 // allowed
 return 0;
}





/*=============================================================================
 * STARTING PICKS RULES
 */
/*
-------------------------->CAMBIARE TUTTO

una volta assegnati i pick in modo standard,

per ogni pick, viene stimato un livello di desiderabilitÃ¡
per ogni giocatore e si randomizza su quello.

--------------------------<



    * Ogni giocatore indica da 0 a 7 preferenze, che possono essere ripetute.


    * Per ogni pick
	Se c'é un pretendente che ha indicato piú preferenze degli altri,
	    il pick gli viene assegnato.
	Altr.
	    Se tra quelli che hanno dato piú preferenze ce n'é uno che assolve
	    meglio ai requisiti dell'oggetto,
		 il pick gli viene assegnato
	    Altr.
		il pick viene marcato come desiderato



    * Per ogni pick desiderato non assegnato
	se il pick ha requisiti che possono essere soddisfatti con i pick rimasti
	non assegnati
	    assegna il pick e i suoi requisiti a caso ad uno dei giocatori che hanno
	    indicato 0o1 preferenze e che ha abbastanza slots per il pick e i requisiti



    * Se un giocatore desidera un pick non desiderato da altri
	se il giocatore puó raggiungere i requisiti con i pick rimasti non assegnati
	e se ha abbastanza slots
	    assegnagli il pick e i suoi requisiti



    * Distribuisci gli oggetti rimasti a caso finché i giocatori che hanno indicato
	0o1 preferenze non hanno 7 oggetti e tutti gli altri non ne hanno 6
	> i giocatori che hanno giá libri di un certo regno hanno piú probabilitá
	    di ottenere libri di quel regno
	> pick con requisiti hanno piú probabilitá di essere scelti da chi puó
	    prenderli rispetto a pick generici
*/



/*=============================================================================
 * LOCAL
 */
static brainC* BHook;





/*=============================================================================
 * MAIN ASSIGNEMENT

    * Per ogni pick
	Se c'é un pretendente che ha indicato piú preferenze degli altri,
	    il pick gli viene assegnato.
	Altr.
	    Se tra quelli che hanno dato piú preferenze ce n'é uno che assolve
	    meglio ai requisiti dell'oggetto,
		 il pick gli viene assegnato
	    Altr.
		il pick viene marcato come desiderato
 */
static brainC* pickMainAssignement(pickC* p)
{
 brainC* best = 0;
 int best_value = 0;
 bool duplicate = false;


 //@@ ensure that players don't get more than 6 picks!!!


 // the player that chooses a pick more times gets it
 for(brainC* b = BHook; b; b = b->Next) if(!b->CheckRequirementsFor(p))
 {
    int cnt = 0;
    for(int i = 0; i < PICKS_MAX_CHOICE; i++) if(b->InitialPicks[i] == p)
	cnt++;

    if(!cnt) continue;
    else if(cnt == best_value)
	duplicate = true;
    else if(cnt  > best_value)
    {
	duplicate = false;
	best_value = cnt;
	best = b;
    }
 }



 // done already?
 if(!best) return 0;		// none wants it
 if(!duplicate) return best;	// only one wants it


 // someone wants it
 p->Desired = true;


 // can we use pick's requirements to sort it?
 if(!p->ReqMask) return 0;



 // pick player that best complies with requirements
 best = 0;
 best_value = 0;
 duplicate = false;
/*@@
 for(brainC* b = Brains; b < BrainsEnd; b++)
    if(int req_compliance = b->CompliesRequirementsFor(p))
	if(req_compliance == best_value)
	    duplicate = true;
	else if(req_compliance > best_value)
	{
	    duplicate = false;
	    best_value = req_compliance;
	    best = b;	
	}
*/
 if(duplicate) return 0;	// none is better suited than the others to have it...
 return best;
}



static void pickMainAssignement()
{
 for(pickC* p = picksHook; p; p = p->Next)
    p->Owner = pickMainAssignement(p);
}





/*=============================================================================
 * "WRONG HANDS" ASSIGNEMENT

    * Per ogni pick desiderato non assegnato
	se il pick ha requisiti che possono essere soddisfatti con i pick rimasti
	non assegnati
	    assegna il pick e i suoi requisiti a caso ad uno dei giocatori che hanno
	    indicato 0o1 preferenze e che ha abbastanza slots per il pick e i requisiti
*/
static bool pickFullRandom(brainC* b)
{
 int cnt = 0;
 for(int i = 0; i < PICKS_MAX_CHOICE; i++) if(b->InitialPicks[i])
    if(++cnt > PICK_FULL_RANDOM_LIMIT) return false;
 return true;
}



static void pickWrongHandsAssignement(pickC* p)
{
 //@@ ignore picks with requirements
 if(p->ReqMask) return;

 // count players with random choice
 int cnt = 0;
 for(brainC* b = BHook; b; b = b->Next)
    if(pickFullRandom(b) && b->CountPicks() < PICKS_MAX_RANDOM) cnt++;

 // no way
 if(!cnt) return;


 // way! =)
 int q = game->Rnd(cnt);
 for(brainC* b = BHook; b; b = b->Next)
    if(pickFullRandom(b) && b->CountPicks() < PICKS_MAX_RANDOM)
	if(q-- == 0)
	{
	    p->Owner = b;
	    return;
	}
}



static void pickWrongHandsAssignement()
{
 for(pickC* p = picksHook; p; p = p->Next) if(p->Desired && !p->Owner)
    pickWrongHandsAssignement(p);
}





/*=============================================================================
 * "FAILED REQUIREMENT" ASSIGNEMENT

    * Se un giocatore desidera un pick non desiderato da altri
	se il giocatore puó raggiungere i requisiti con i pick rimasti non assegnati
	e se ha abbastanza slots
	    assegnagli il pick e i suoi requisiti
*/
static void pickFailedRequirementAssignement(brainC* b, pickC* p)
{
 // ensure that none else wants it
 for(brainC* bb = BHook; bb; bb = bb->Next) if(bb != b)
    for(int i = 0; i < PICKS_MAX_CHOICE; i++) if(bb->InitialPicks[i] == p)
	return;

 //@@ check for available requirements, they have to be available
 //	and not marked as Desired
 //	the player must also have enough slots fo rthem all
}



static void pickFailedRequirementAssignement()
{
 for(brainC* b = BHook; b; b = b->Next)
    for(int i = 0; i < PICKS_MAX_CHOICE; i++)
	if(pickC* p = b->InitialPicks[i]) if(!p->Owner && p->ReqMask)
	    pickFailedRequirementAssignement(b, p);
}





/*=============================================================================
 * "RANDOM" ASSIGNEMENT

    * Distribuisci gli oggetti rimasti a caso finché i giocatori che hanno indicato
	0o1 preferenze non hanno 7 oggetti e tutti gli altri non ne hanno 6
	> i giocatori che hanno giá libri di un certo regno hanno piú probabilitá
	    di ottenere libri di quel regno
	> pick con requisiti hanno piú probabilitá di essere scelti da chi puó
	    prenderli rispetto a pick generici

*/
static void pickRandomAssignement(brainC* b)
{
 int cnt = 0;
 for(int i = 0; i < PICKS_MAX_CHOICE; i++)
    if(b->InitialPicks[i]) cnt++;

 int tot = cnt > PICK_FULL_RANDOM_LIMIT? PICKS_MAX_CHOICE:PICKS_MAX_RANDOM;

 while(b->CountPicks() < tot)
 {
    // count available picks
    cnt = 0;
    for(pickC* p = picksHook; p; p = p->Next)
	if(!p->Owner && !b->CheckRequirementsFor(p))
	    cnt++;

    //@@
    if(!cnt) throw "not enough picks!";


    // choose random pick
    cnt = game->Rnd(cnt);


    // take choosen pick
    for(pickC* p = picksHook; p; p = p->Next)
	if(!p->Owner && !b->CheckRequirementsFor(p))
	    if(cnt-- == 0) {p->Owner = b; break;}
 }
}



static void pickRandomAssignement()
{
 for(brainC* b = BHook; b; b = b->Next)
    pickRandomAssignement(b);
}





/*=============================================================================
 * ASSIGNEMENT
 */
void pickAssign()
{
 BHook = game->BrainsHook;


 // reset pick owners
 for(pickC* p = picksHook; p; p = p->Next)
 {
    p->Owner = 0;
    p->Desired = 0;
 }

 // assigne
 pickMainAssignement();
 pickWrongHandsAssignement();
 pickFailedRequirementAssignement();
 pickRandomAssignement();
}





// EOF =========================================================================
