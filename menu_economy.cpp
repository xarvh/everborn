/*
 MENU_ECONOMY.CPP		v0.01
 Economy menu.

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
#include <GL/gl.h>

#include "colors.h"
#include "counters.h"
#include "spell.h"
#include "main.h"
#include "boxgui.h"
#include "cmd.h"

#include "game.h"
#include "city.h"
#include "race.h"
#include "map.h"
#include "fnt.h"





/*=============================================================================
 * LOCALS
 */





/*=============================================================================
 * GENERICS
 */
static void MENUdraw(boxC& b)
{
 bwinDisplay(b);

 glPushMatrix();
    bTColor();
    glTranslated(.1, b.H -.05, 0);
    fntS(0, .03, 0);
    fntT(.8,
	"This is the Economy menu; it shall handle:\n"
	"?>alchemy: transform gold to magic power and viceversa, per turn option?<\n"
	"?>magic power allocation bars: power, research skill ?<\n"
	"?>gold balance: armies (army list), cities (city list), taxes ?<\n");

    glTranslated(0, -.1, 0);
    int t = self->DeltaRawMana();
    fntL("\nCollected mp: %d?M1", t);
    fntL("\nAllocated for Skill: %d?M1 (%d%%)", self->DeltaSkill(), self->AllocateToSkill);
    fntL("\nAllocated for Research: %d?M1 (%d%%)", (t*self->AllocateToResearch)/100, self->AllocateToResearch);
    fntL("\nStored %d?M1", self->DeltaMana());

    fntL("\n\nAlchemy: %d", self->AlchemyManaToGold);
    fntL("\nTotal Research: %d?K1", self->DeltaKnow());
    fntL("\nGold Income: %d?G1", self->DeltaGold());
    fntL("\n\n(use the ?Hallocate?/ console command to adjust).");

 glPopMatrix();
}



static bool MENUinput(boxC& b)
{
 void countersF();

 switch(inputK()) {
    case 27: bOpen("land"); break;

//    case ' ': cmd("ready"); break;

    default: break;
 }

 return false;
}





static char* MENUcheck()
{
 if(!gameIsAvailable()) return "no game";

 // city menu ok! =)
 return 0;
}





static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 return 0;
}



/*=============================================================================
 * MENU
 */
static spellTypeC* st[] = {
//    &spellTypeCity,
    0
};





static bmenuDECLARE( economy ) {
    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "main";
    ProvidedSpellTargets = (void**)st;

    boxC* b;
    float y, s;

} bmenuINSTANCE;



//EOF =========================================================================
