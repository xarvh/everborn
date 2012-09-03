/*
 MENU_PLAYERS.CPP		v0.05
 Players menu.

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
    glTranslated(.1, b.H - .1, 0);
    fntS(0, .03, 0);

    fntT(.8,
	"This menu shall handle:\n"
	"?>Diplomacy: agreement management, proposal, status ?<\n"
	"?>overland enchantements ?<\n");

 glPopMatrix();
}



static bool MENUinput(boxC& b)
{
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
//    &spellTypeWizard,
    0
};





static bmenuDECLARE( players ) {
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
