/*
 MENU_MAIN.CPP		v0.03
 Main Menu.

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

#include <stdlib.h>
#include "main.h"
#include "boxgui.h"
#include "cmd.h"

#include "fnt.h"
#include "data.h"


static dataC mmLogo("logo.png");




/*=============================================================================
 * GUI
 */
static void btf_host_game()	{ cmd("host"); } //@@ "host [gamestring]"
static void btf_join_game()	{ cmd("join localhost"); }
static void btf_() { pe("not implemented! =(\n"); }

static void btf_training() { }
static void btf_options() { }
static void btf_exit() { exit(0); }





namespace popupHelpS { void Open(char*, char*); }

static void btfInstructions()
{
 popupHelpS::Open("Instructions! =)",
    "I commend you for your curiosity, but unfortunately"
    " the only purpose of this box is to show you the information you"
    " see, and there is not much point in pushing it.\n"
    "If you want a help window about mouse buttons, try right-clicking.");
}

static void drawInstructions(boxC& b)
{
 bwinDisplay(b);

 glTranslated(.01, b.H, 0);
 fntS(0, b.H/5);
 glColor3f(.7,.7,.9);
 fntL("Instructions:");
 glTranslated(0,-.029,0);
 fntS(0, b.H/5.3);
 glColor3f(1,1,1);
 fntL("\n LEFT mouse: PUSH, SELECT");
 glTranslated(0,-.05,0);
 fntL("\n RIGHT mouse: ORDER, HELP");
 glTranslated(0,-.05,0);
 fntL("\n MIDDLE mouse: DETAILS");
}





/*=============================================================================
 * GENERICS
 */
static void main_menu_draw(boxC& b)
{
 //@@ if game is open display a "resume game" button
 glColor4f(1,1,1,1);

 // logo
 glEnable(GL_BLEND);
 glPushMatrix();
    glTranslated(.72, .25/2, 0);
    glScaled(.5, .25, 1);
    mmLogo.Quad(0);
 glPopMatrix();

 // version
 glPushMatrix();
    glTranslated(.01, .01, 0);
    fntS(.03);
    fntL("%s", game_string);
 glPopMatrix();

}


static bool main_input(boxC& b)
{
 switch(inputK()) {
    case 27: exit(0); break;
    default: break;
 }

 return false;
}


static char* main_start()
{
 //DEBUG
 static bool done = false;
 if(!done && envIsTrue("fast")) {done = true; cmd("host"); }
 return 0;
}




/*=============================================================================
 * MAIN MENU
 */
static bmenuDECLARE( main ) {

 StartF = main_start;
 InputF = main_input;
 DisplayF = main_menu_draw;

 float x =  .07;
 float y =  .50;
 float s = -.07;
 AddButton("Host Game",
    "Create a new game your friends can Join to.\n"
    "\n?HBugs:?/\n"
    "?>If another server is running already, the connection may be refused.?<\n"
    "?>If the game data are not valid, the server will abort, resulting again"
    " in a refused connection. ?<\n",
  x, y   , 1.4*BDEF_BUTTON_W, BDEF_BUTTON_H, 'h', btf_host_game);

 AddButton("Join Game",
    "Join a game someone else has created.\n"
    "\n?HBugs:?/\n"
    "Right now it joins only the localhost (127.0.0.1), so if you"
    " want to join a game somewhere else you have to press F8 and"
    " use the 'join [address]' command.\n",
    x, y+=s, 1.4*BDEF_BUTTON_W, BDEF_BUTTON_H, 'j', btf_join_game);

 AddButton("Training",
    "It would be nice to have a trainer to teach the basics of the game,"
    " but it's pretty low on developement priorities.",
    x, y+=s, 1.4*BDEF_BUTTON_W, BDEF_BUTTON_H, 't', btf_);

 AddButton("Options",
    "Right now there is no GUI available for the game options: to change"
    " the game settings press F8 and use the console command 'set' to"
    " see and modify the environment variables.\n"
    "You can create a $(HOME)/.everbornrc to have all your variables"
    " set at startup, it's just a text file with a 'set' command on each"
    " line.",
    x, y+=s, 1.4*BDEF_BUTTON_W, BDEF_BUTTON_H, 'o', btf_);

 AddButton("Exit",
    "Close the game and return in the real world.\n"
    "Well, at least to your desktop...",
    x, y+=s, 1.4*BDEF_BUTTON_W, BDEF_BUTTON_H, 'e', btf_exit);


 boxC* w = AddButton("Instructions",
    "This is the mouse button policy Everborn tries to stick to.\n"
    "\n"
    "Use the LEFT button to:\n"
    "?>push buttons ?<\n"
    "?>select stuff ?<\n"
    "\n"
    "Use the RIGHT button to:\n"
    "?>Issue the default command (like move somehwere/attack something)"
    " to the selected troops. ?<\n"
    "?>Obtain help on the stuff you see around. ?<\n"
    "\n"
    "The MIDDLE button is for obtaining more game details on stuff"
    " like troops, items, map locations and other wizards.",
    .5, BMENU_H-.3, .49, .2, 0, btfInstructions);
 w->DefaultS.DisplayF = drawInstructions;



} bmenuINSTANCE;


//EOF =========================================================================
