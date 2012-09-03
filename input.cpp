/*
 INPUT.CPP		v0.10
 Keyboard and mouse handlers.

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

#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "main.h"
#include "boxgui.h"
#include "cmd.h"

int consoleIsActive();
int consoleOpen();
int consoleClose();
int consoleInput(int, int);




/*=============================================================================
 *
 */
#define MOUSE_R		(1<<0)
#define MOUSE_M		(1<<1)
#define MOUSE_L		(1<<2)


static int inputMouseX, inputMouseY;
static int inputMouseR = 0, inputMouseB = 0;
static int inputK_key = 0, inputE_key = 0;





/*=============================================================================
 * MENU
 */
int inputMenu(int k, int e)
{
 if(consoleIsActive())
 {
    consoleInput(k, e);
    inputK_key = inputE_key = 0;
 }
 else
 {
    inputK_key = k;
    inputE_key = e;
    bInput();
 }

 inputMouseR = 0;
}





/*=============================================================================
 * MOUSE
 */
static int inputMouseRe(int button)
{
 if(inputMouseR & button) return 1;

 return 0;
}

int mouseX()	{ return inputMouseX; }
int mouseY()	{ return inputMouseY; }
int mouseR()	{ return inputMouseRe(MOUSE_R); }	// is a request awaiting? //
int mouseM()	{ return inputMouseRe(MOUSE_M); }
int mouseL()	{ return inputMouseRe(MOUSE_L); }
int mouseRdn()	{ return inputMouseB & MOUSE_R; }	// is the button down? //
int mouseMdn()	{ return inputMouseB & MOUSE_M; }
int mouseLdn()	{ return inputMouseB & MOUSE_L; }





/*============================================================================
 * KEYBOARD
 */
int inputK()		{ return inputK_key; }
int inputE()		{ return inputE_key; }
int inputShift()	{ return SDL_GetModState() & (KMOD_RSHIFT|KMOD_LSHIFT); }
int inputCtrl()		{ return SDL_GetModState() & (KMOD_RCTRL|KMOD_LCTRL); }
int inputAlt()		{ return SDL_GetModState() & (KMOD_RALT|KMOD_LALT); }





/*=============================================================================
 * Keyboard handlers
 */
static void inputKey(int sym, int unicode)
{
 int c = 0;
 int e = 0;

 switch(sym) {
    case SDLK_F1: e = INPUT_F1; break;
    case SDLK_F2: e = INPUT_F2; break;
    case SDLK_F3: e = INPUT_F3; break;
    case SDLK_F4: e = INPUT_F4; break;
    case SDLK_F5: e = INPUT_F5; break;
    case SDLK_F6: e = INPUT_F6; break;
    case SDLK_F7: e = INPUT_F7; break;

    case SDLK_F8:
	if(consoleIsActive()) consoleClose();
	else consoleOpen();
	return;

    case SDLK_F10: cmd("shot"); return;

    case SDLK_UP:	case SDLK_KP8: e = INPUT_KUP; break;
    case SDLK_DOWN:	case SDLK_KP2: e = INPUT_KDN; break;
    case SDLK_LEFT:	case SDLK_KP4: e = INPUT_KLF; break;
    case SDLK_RIGHT:	case SDLK_KP6: e = INPUT_KRI; break;

    case SDLK_HOME:	e = INPUT_KHOME; break;
    case SDLK_END:	e = INPUT_KEND; break;

    case SDLK_KP_MINUS: c = '-'; break;
    case SDLK_KP_PLUS: c = '+'; break;

    default:
	if(unicode)
	    if(unicode & 0xff80) break;		//cannot be translated to ascii
	    else c = (int) unicode & 0xff;
	else
	    if(sym >= '\b' && sym <= '~') c = sym;
	break;
 }

 inputMenu(c, e);
}





/*=============================================================================
 * Mouse handlers
 */
static void inputMouse(int button, int state)
{
 int b = 0;
 int e = 0;

 switch(button) {
    case SDL_BUTTON_LEFT:	b = MOUSE_L;	break;
    case SDL_BUTTON_MIDDLE:	b = MOUSE_M;	break;
    case SDL_BUTTON_RIGHT:	b = MOUSE_R;	break;

    case SDL_BUTTON_WHEELUP:	e = INPUT_MUP;	break;
    case SDL_BUTTON_WHEELDOWN:	e = INPUT_MDN;	break;
    default: break;
 }

 if(e)
    if(state == SDL_RELEASED) e = 0;

 if(b)
    if(state == SDL_RELEASED) inputMouseB &= ~b;
    else
    {
	inputMouseB |= b;
	inputMouseR |= b;
    }

 SDL_GetMouseState(&inputMouseX, &inputMouseY);
 inputMenu(0, e);
 inputMouseR = 0;	// remove pending requests
}





/*==============================================================================
 * POLL
 */
void inputPoll()
{
 SDL_Event event;
 memset(&event, 0, sizeof(event));

 while(SDL_PollEvent(&event))
    switch(event.type) {
	case SDL_KEYDOWN:
	    inputKey(event.key.keysym.sym, event.key.keysym.unicode);
	    break;

	case SDL_MOUSEMOTION:
	    inputMouse(0, 0);
	    break;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	    inputMouse(event.button.button, event.button.state);
	    break;

	case SDL_QUIT:
	    exit(0);
	    break;

	case SDL_VIDEORESIZE: break;
	default: break;
    }
}



//EOF =========================================================================
