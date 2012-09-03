/*
 MENU_MULTI.CPP		v0.02
 Multiplayer Menu.

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

#include "main.h"
#include "boxgui.h"
#include "game.h"
#include "fnt.h"
#include "cmd.h"
#include "net.h"
#include "colors.h"


static void btf_ready() { cmd("ready"); }







/*=============================================================================
 *
 */
static void mmuDisplayPlayer(boxC& b)
{
 bdrawContour(b, b.Mouse, false);

 brainC* p = game->Brains + b.Id;
 if(!p->Name[0]) return;

 fntS(0, b.H, .95);
 if(p->Color)	// colors will arrive in matter of seconds!
    glColor4ub(p->Color->R, p->Color->G, p->Color->B, 255);
 fntL(" %s", p->Name);

 if(p->Ready)
 {
    glTranslated(1,0,0);
    p->Color->Select();
    fntL(" Ready!");
 }
}





/*=============================================================================
 * GENERICS
 */
static char* MENUcheck()
{
 if(!netConnected()) return "not connected";
 return 0;
}



static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 //@@
 static bool go = true;
 if(go && envIsTrue("fast")) { go = false; cmd("ready"); }

 return 0;
}



static void MENUdraw(boxC& b)
{
}


static bool MENUinput(boxC& b)
{
 switch(inputK()) {
    case 27: netReset(); bOpen("main"); break;
    default: break;
 }

 return false;
}


/*=============================================================================
 * MULTI MENU
 */
static bmenuDECLARE( multi ) {

    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "main";

    for(int i = 0; i < GAME_MAX_BRAINS; i++)
    {
	float s = BDEF_BUTTON_H;
	float y = BMENU_RATIO - .04 -1.1*s*(i+1);

	boxC* w = AddWindow("Players",
	    "This is the list of players that will take part in the game;"
	    " it also tells you which player has already pressed the"
	    " ?HReady?/ button.",
	.05, y, 1.5*BDEF_BUTTON_W, s,
	0,  mmuDisplayPlayer);
	w->Id = i;
    }


    AddButton("Ready",
	"When all the players in the list press this button, the game starts"
	" and no one else can join anymore.\n",
	.7, .05, BDEF_BUTTON_W, BDEF_BUTTON_H, 'r', btf_ready);

} bmenuINSTANCE;



//EOF =========================================================================
