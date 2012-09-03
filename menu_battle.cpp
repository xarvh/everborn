/*
 MENU_BATTLE.CPP		v0.02
 Battle Menu.

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

#include "spell.h"
#include "main.h"
#include "cmd.h"
#include "boxgui.h"
#include "fnt.h"
#include "map_view.h"
#include "gselect.h"

#include "utype.h"
#include "game.h"
#include "race.h"
#include "map.h"


static mapViewC mbMapView;	// initialized by MENUStart
static cellC* Cell;
static battleC* Battle;
static bcC* MCell;		// Mouse Cell
static unitC* MUnit;		// Mouse Unit
static float MX, MY;



static void menuBattle_MakeSelection(void* selection)
{
 // unit?
 MUnit = (unitC*)selection;
 if(MUnit < game->Units || MUnit >= game->UnitsEnd)
    MUnit = 0;
 else
 {
    MX = MUnit->X;
    MY = MUnit->Y;
 }


 // battle cell?
 MCell = (bcC*)selection;
 if(MCell < Battle->SubCellA || MCell >= Battle->SubCellA + BATTLE_W*BATTLE_H)
    MCell = 0;
 else
 {
    MX = (MCell - Battle->SubCellA)%BATTLE_W;
    MY = (MCell - Battle->SubCellA)/BATTLE_W;
 }

 // unit in a cell!!
 //@@ this isnt very accurate, but as long as reverse projection
 // is unavailable, should improve a bit unit selection
 if(MCell && !MUnit)
 {
    float x = MX;
    float y = MY;
    float best_d = 10000;
    for(unitC* u = Cell->Unit; u; u = u->Next) if(u->IsAlive())
    {
	float dw = u->X - x;
	float dh = u->Y - y;
	float d = dw*dw + dh*dh;
	if(d < best_d) { MUnit = u; best_d = d; }
    }

    if(best_d < .5*.5)	MCell = 0; // accept best unit
    else		MUnit = 0; // reject best unit

    if(MUnit)
    {
	MX = MUnit->X;
	MY = MUnit->Y;
    }
 }

// if(MUnit) pf("Unit %d (%f,%f)\n", MUnit - game->Units, MX, MY);
// else	pf("map %f %f\n", MX, MY);
}





/*=============================================================================
 * DISPLAY MAP
 */
static void* menuBattle_displayMap(int x, int y)
{
 if(!Cell || !Cell->Battle) return 0;


 void dbDisplay(cellC*, mapViewC&);

 mbMapView.Rot = atof(envGet("rot"));

 // fake step, to find selected object
 glClear(GL_DEPTH_BUFFER_BIT);
 gsBegin();
    dbDisplay(Cell, mbMapView);
 void* selection = gsEnd(x, y);		//@@ do soemthing with it!

 // real draw
 glClear(GL_DEPTH_BUFFER_BIT);
 dbDisplay(Cell, mbMapView);

 return selection;
}





/*=============================================================================
 * GENERICS
 */
/*static void mbDrawPause()
{
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);

 glBegin(GL_QUADS);
    glColor4f(.7,.7,.7,.8); glVertex2f( 0,   0);
    glColor4f(1.,1.,1.,.8); glVertex2f(40,   0);
    glColor4f(.8,.8,.8,.8); glVertex2f(40, 120);
    glColor4f(.5,.5,.5,.8); glVertex2f( 0, 120);
 glEnd();

 screenLineWidth(1.);
 glColor4f(.4,.4,.4, .8);
 glBegin(GL_LINE_STRIP);
    glVertex2f( 0,   0);
    glVertex2f(40,   0);
    glVertex2f(40, 120);
    glVertex2f( 0, 120);
    glVertex2f( 0,   0);
 glEnd();



 glTranslated(40*2,0,0);
}*/


static void MENUdraw(boxC& b)
{
 //@@ do something to make it system indipendent
 //@@ and envGet() dependent
 //@@ and zoom-dependent, too!
 mbMapView.MouseScroll();


 if(spellGuiCity()) spellGuiDo(Cell->X, Cell->Y, Cell->Z);


 void* selection = menuBattle_displayMap(mouseX(), mouseY());
 menuBattle_MakeSelection(selection);

/*
 if(Battle->IsPaused())
 {
    guiLoadCoord();
    glTranslated(1400, 50, .0);
    mbDrawPause();
    mbDrawPause();
 }*/

 if(Battle->IsPostponed())
 {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glColor4d(.4, .4, .4, .75);

    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(0, b.H);
        glVertex2f(1, b.H);
        glVertex2f(1, 0);
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
	glTranslated(1, 0, 0);
	fntS(.03);
	bHColor();
	fntR("To Be Continued....");
    glPopMatrix();
 }


// bdDrawBrainList(Battle);
}





static bool MENUinput(boxC& b)
{
 double sp = .4;	//@@ atof(envGet("scrollspeed"))

 void rotMod(signed);
 switch(inputK()) {
    case 27: bOpen("land"); break;

    case ' ': spellGuiOpenBook(); break;

    case 'r': rotMod(-5); break;
    case 'f': rotMod(+5); break;

    case '+': mbMapView.Zoom( 1); break;
    case '-': mbMapView.Zoom(-1); break;

    case 'w': cmd("wave 0"); break;
    case 'e': cmd("wave 1"); break;
    case 'd': cmd("wave 2"); break;
    case 'c': cmd("wave 3"); break;
    case 'x': cmd("wave 4"); break;
    case 'z': cmd("wave 5"); break;
    case 'a': cmd("wave 6"); break;
    case 'q': cmd("wave 7"); break;

    case 's': cmd("engage"); break;
    case 'h': cmd("hold"); break;

    case 'j': cmd("hold 30"); break;

    default:
	void mvInput(char);
	mvInput(inputK()); break;
 }

 switch(inputE()) {
    case INPUT_KUP: mbMapView.Scroll( .0, +sp); break;
    case INPUT_KDN: mbMapView.Scroll( .0, -sp); break;
    case INPUT_KLF: mbMapView.Scroll( -sp, .0); break;
    case INPUT_KRI: mbMapView.Scroll( +sp, .0); break;
    case INPUT_MUP: mbMapView.Zoom( 1); break;
    case INPUT_MDN: mbMapView.Zoom(-1); break;
    default: break;
 }

 if(mouseL())
 {
    if(MUnit)
	if(spellGuiUnit())	spellGuiDo(MUnit - game->Units);
	else if(inputShift())	cmd("+select %d", MUnit - game->Units);
	else if(inputCtrl())	cmd("-select %d", MUnit - game->Units);
	else			cmd("select %d", MUnit - game->Units);
 }

 if(mouseR())
 {
    if(MUnit && MUnit->Owner != self)
    {
	if(inputAlt())	cmd("melee %d", MUnit - game->Units);
	else		cmd("attack %d", MUnit - game->Units);
    }
    else
    {
	if(inputShift())	cmd("+move %d %d", (int)MX, (int)MY);
	else			cmd("move %d %d", (int)MX, (int)MY);
    }
 }

 if(mouseM())
 {
    void popupUnit(unitC*, float, float);
    if(MUnit) ;//popupUnit(MUnit, guiMouseX(), guiMouseY());
    //pf("X %d  Y %d\n", MX, MY);
 }
}







/*=============================================================================
 * GENERICS
 */
static char* MENUcheck()
{
 if(!gameIsAvailable()) return "no game available";

 Cell = self->LocalFocus;
 if(!Cell) return "no cell selected";

 Battle = Cell->Battle;
 if(!Battle) return "no battle here";

 return 0;
}







static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 // mapview
 mbMapView.MapW = BATTLE_W;
 mbMapView.MapH = BATTLE_H;
 mbMapView.WCycle = false;
 mbMapView.CenterAt(BATTLE_W/2., BATTLE_H/2.);

 mbMapView.ScrX = 0;
 mbMapView.ScrY = 0;
 mbMapView.ScrW = 1;
 mbMapView.ScrH = 1;

 mbMapView.CellsInMidline = BATTLE_W/4.;


 //@@ do something smarter
 for(unitC* u = Cell->Unit; u; u = u->Next)
    cmd("+select %d", u - game->Units);

 return 0;
}



/*=============================================================================
 * BATTLE MENU
 */
static spellTypeC* st[] = {
    &spellTypeUnit,
//    &spellTypeBattleArea,
    &spellTypeBattle,
    &spellTypeCity,
    0
};


static bmenuDECLARE( battle ) {
    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "land";
    ProvidedSpellTargets = (void**)st;


} bmenuINSTANCE;

//EOF =========================================================================
