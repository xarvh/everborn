/*
 MENU_LAND.CPP		v0.10
 Overland Menu.

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

#include "boxgui.h"

#include "main.h"
#include "colors.h"
#include "counters.h"

#include "city.h"

#include "spell.h"
#include "unit.h"
#include "utype.h"
#include "game.h"
#include "race.h"
#include "map.h"
#include "net.h"
#include "cmd.h"
#include "fnt.h"
#include "gselect.h"
#include "map_view.h"



/*=============================================================================
 * GLOBALS
 */
static envC envScrollspeed("scrollspeed", "10");
static cellC* MouseCell;
static unitC* mlMidpanelU;
static mapViewC MapView;



/*cellC* mlMouseCell()
{
 return MouseCell;
}*/


void menuLand_CenterAt(float x, float y)
{
 MapView.CenterAt(x,y);
}





/*=============================================================================
 * CMD
 */
static int cmdFPlane()
{
 if(cellC* f = self->Focus)
    cmd("focus %d %d %d", f->X, f->Y, f->Mirror()->Z);
 else mdSwitchPlane();
}
    
static cmdC cmdPlane(
    "plane",
    ":",
    "Switches plane",
    cmdFPlane,
    NULL, NULL
    );





/*=============================================================================
 * MAP
 */
static void btfMap()
{
 // handle
 if(mouseL())
    if(!MouseCell) cmd("focus");
    else
    {
	//is mouse on currently selected cell?
	bool f = self->LocalFocus == MouseCell;
        cmd("focus %d %d %d", MouseCell->X, MouseCell->Y, MouseCell->Z);

	if(MouseCell->Battle) cmd("menu battle");
	else if(MouseCell->City && (f || !MouseCell->Unit))
	{
	    if(spellGuiCity()) spellGuiDo(MouseCell->X, MouseCell->Y, MouseCell->Z);
	    else cmd("menu city");
	}
	else if(!MouseCell->Unit) cmd("focus");	// remove focus
    }

 if(mouseR() && MouseCell)
 {
    if(self->LocalFocus) cmd("goto %d %d", MouseCell->X, MouseCell->Y);
 }

 if(mouseM() && MouseCell)
    cmd("g %d %d %d", MouseCell->X, MouseCell->Y, MouseCell->Z);
}





static cellC* mlMap(int x, int y)
{
 void mdDisplay(void*, mapViewC&);

 MapView.Rot = atoi(envGet("rot"));

 // fake draw to find selected cell
 glClear(GL_DEPTH_BUFFER_BIT);
 gsBegin();
    mdDisplay(0, MapView);
 cellC* c = (cellC*)gsEnd(x, y);

 // actual draw
 glClear(GL_DEPTH_BUFFER_BIT);
 mdDisplay(MouseCell, MapView);

 return c;
}





/*=============================================================================
 * MINI MAP
 */
static bool mlMinimapInput(boxC& b)
{
 return false;
}

static void mlMinimapDisplay(boxC& b)
{
 glPushMatrix();
    glTranslated(.5,.5,0);
    void mdMinimap(mapViewC* mv);
    mdMinimap(&MapView);
 glPopMatrix();

 glDisable(GL_DEPTH_TEST);
 bdrawContour(b, false, false);
}





/*=============================================================================
 * MID PANEL: MAIN
 */
static boxSC* mlMidpanelBattle;
static boxSC* mlMidpanelUnits;
static cellC* mlMidpanelCell;

static boxSC* mlMidpanelSelect(boxC& b)
{
/* if(mlMidpanelCell = MouseCell)
    if(mlMidpanelCell->Battle) return mlMidpanelBattle;
    else if(mlMidpanelCell->Unit) return mlMidpanelUnits;
*/
 if(mlMidpanelCell = self->Focus)
    if(mlMidpanelCell->Battle) return mlMidpanelBattle;
    else if(mlMidpanelCell->Unit) return mlMidpanelUnits;

 return &b.DefaultS;
}





static bool mlMidpanelBattleInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 if(spellGuiBattle())
    spellGuiDo(self->LocalFocus->X, self->LocalFocus->Y, self->LocalFocus->Z);
 else
    bOpen("battle");

 return true;
}

static void mlMidpanelBattleDisplay(boxC& b)
{
 bdrawContour(b, false, false);

 // mapview
 mapViewC mv;
 mv.MapW = BATTLE_W;
 mv.MapH = BATTLE_H;
 mv.WCycle = false;
 mv.CenterAt(.5*BATTLE_W, .3*BATTLE_H);

 mv.ScrX = b.DefaultL.Xg;
 mv.ScrY = b.DefaultL.Yg + b.Hg/2;
 mv.ScrW = b.Wg;
 mv.ScrH = b.Hg;

 mv.CellsInMidline = BATTLE_W*1.5;
 mv.Rot = 22;

 void dbDisplay(cellC*, mapViewC&);
 dbDisplay(mlMidpanelCell, mv);
 glDisable(GL_DEPTH_TEST);
 bLoadCoord();
}



static bool mlMidpanelDefaultInput(boxC& b) { return false; }
static void mlMidpanelDefaultDisplay(boxC& b)
{
 bwinDisplay(b);
 glTranslated(.01, b.H, 0);
 fntS(0, b.H/6, .98);

 bTColor();
 fntL("\n");
 fntL("%4d Spell Skill of which\n", self->Skill);
 fntL("%4d is ready available this turn, because\n", self->SkillAvl);
 fntL("%4d has been committed to spell casting.\n", self->SkillCommitted);

 if(self->SpellQueue)
    fntL("Right now casting: %s (%d turns left)\n",
	self->SpellQueue->Spell->Name, self->SpellQueue->TurnsRequired());
 else fntL("Casting no spell\n");              


 fntL("Research: %d per turn.\n", self->DeltaKnow());
 if(spellC* s = self->ResearchedSpell)
    if(int dk = self->DeltaKnow())
	    fntL("Researching: %s (%d turns left)\n", s->Name, s->ResearchLeft[self->Id]/dk);
    else    fntL("Researching: %s (idle)\n", s->Name);
 else	fntL("no research\n");
}





/*=============================================================================
 * MID PANEL: UNITS
 */
static bool mlMidpanelUnitsInput(boxC& b){ return false;}


static bool mlMidpanelUnitInput(boxC& b)
{
 if(!b.Mouse || b.Id == -1) return false;

 unitC* u = game->Units + b.Id;

 if(mouseL())
 {
    if(spellGuiUnit())		spellGuiDo(u->Id());
    else if(inputShift())	cmd("+select %d", u->Id());
    else if(inputCtrl())	cmd("-select %d", u->Id());
    else
    {
	if(u->Team == self->SelectedTeam)
	    cmd("-select %d", u->Id());
	cmd("select %d", u->Id());
    }
    return true;
 }


 if(mouseR())
 {
    cmd("+select %d", u->Id());
    return true;
 }


 if(mouseM())
 {
    popupUnitS::Open(u);
//    void popupUnit(unitC*, float, float);
//    popupUnit(u, (x+.5)*100 + b->X, (y+.5)*100 + b->Y);
    return true;
 }

 return false;
}





static void mlFramedUnit(unitC* u)
{
 float x1 = 0;
 float y1 = 0;
 float x2 = 1;
 float y2 = 1;

 int r = u->Color()->R;
 int g = u->Color()->G;
 int b = u->Color()->B;

 if(u->Owner == self)
    if(u->Team == self->SelectedTeam)
	{
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_BLEND);

	    glBegin(GL_QUADS);
	        glColor4ub(r/2, g/2, b/2, 255);	glVertex2f(x1,y1);
	        glColor4ub(r, g, b, 255);	glVertex2f(x1,y2);
	        glColor4ub(r, g, b, 255);	glVertex2f(x2,y2);
	        glColor4ub(r+50, g+50, b+50, 255); glVertex2f(x2,y1);
	    glEnd();
	}

 // display TimeAvailable
 x1 = .03;
 y1 = .02;
 x2 = .12;
 y2 = y1 + (6 - u->TimeAvailable) * .96 / 6;
 glDisable(GL_DEPTH_TEST);
 glColor4ub(255-r, 255-g, 255-b, 255);
 glBegin(GL_QUADS);
    glVertex2f(x1,y1);
    glVertex2f(x1,y2);
    glVertex2f(x2,y2);
    glVertex2f(x2,y1);
 glEnd();

 // display unit
 glPushMatrix();
    glTranslated(.5, 0, 0);
    glRotated(180, 0, 1, 0);
    glScaled(.8, .8, .8);
    duUnit(u);
 glPopMatrix();
}



static void mlMidpanelUnitDisplay(boxC& b)
{
 if(b.Id == -1) return;

 unitC* u = game->Units + b.Id;

 if(b.Mouse)
    mlMidpanelU = u;

 mlFramedUnit(u);
 if(u->Wounds)
    glTranslated(.5, .9, 0), duHealth(u);
}



static void mlMidpanelUnitsDisplay(boxC& b)
{
 mlMidpanelU = 0;
 bwinDisplay(b);

 unitC* u = mlMidpanelCell->Unit;
 for(boxLC* l = mlMidpanelUnits->SubBoxesHook; l; l = l->Next)
    if(!u) l->SubBox.Id = -1;
    else
    {
	l->SubBox.Id = u->Id();
	u = u->Next;
    }
}





/*=============================================================================
 * RIGHTPANEL BUTTONS
 */
static void btfOptions()	{ bOpen("options"); }
static void btfTurn()		{ if(!self->Ready) cmd("ready"); }
static void btfSpell()		{ spellGuiOpenBook(); }



static void btfPurify()
{
 cellC* c = self->Focus;
 if(!c) return;

 for(unitC* u = c->Unit; u; u = u->Next)
    if(u->Team == self->SelectedTeam) if(u->Type->Cmd & CMD_PURIFY)
    {
	cmd("purify");
	return;
    }

 boxHelp("Purify",
    "To purify a corrupted land, you need troops with the ?HPurify?/ special.\n"
    "?HShamans?/ and ?HPriests?/ usually have this skill.\n");
}

static boxSC* mlRightpanelSelectPurify(boxC& b)
{
 cellC* c = self->Focus;
 if(!c) return 0;

 if(c->Flags & CELL_CORRUPT)
    return &b.DefaultS;
 return 0;
}





static boxSC* mlRightpanelRaze;
static boxSC* mlRightpanelMeld;
static boxSC* mlRightpanelSettle;
static boxSC* mlRightpanelSelectReMeSe(boxC& b)
{
 cellC* c = self->Focus;
 if(!c) return 0;

 for(unitC* u = c->Unit; u; u = u->Next)
    if(u->Team == self->SelectedTeam) if(u->Type->Cmd & CMD_OUTPOST)
	return mlRightpanelSettle;

 if(c->City)
    for(unitC* u = c->Unit; u; u = u->Next)
	if(u->Team == self->SelectedTeam) return mlRightpanelRaze;

 //@@ meld!

 return 0;
}

static void btfRaze()
{
 if(!self->Focus) return;
 if(!self->Focus->City) return;

 popupYesNoS::Open(
    "raze"
    "|"
    "\nOrdering your units to completely destroy the city of ?H%s?/."
    "\nProceed?", self->Focus->City->Name
 );
}

static void btfMeld()
{
 cmd("meld");
}

static void btfSettle()
{
 if(self->Focus && self->Focus->City) cmd("settle");
 else popupSettleS::Open();
}





static void btfRoad()
{
 float x = BDEF_BORDER+BDEF_BUTTON_W;
 float w = 1-.22 - x - BDEF_BORDER;
 popupBuildRoadS::Open(x, w, &MouseCell);
}

static boxSC* mlRightpanelSelectRoad(boxC& b)
{
 cellC* c = self->Focus;
 if(!c) return 0;

 for(unitC* u = c->Unit; u; u = u->Next)
    if(u->Team == self->SelectedTeam) if(u->Type->Cmd & CMD_BUILD_ROAD)
	return &b.DefaultS;
 return 0;
}





static void btfSiege()
{
 cmd("siege");
}
static boxSC* mlRightpanelSelectSiege(boxC& b)
{
 cellC* c = self->Focus;
 if(!c) return 0;

 for(unitC* u = c->Unit; u; u = u->Next)
    if(u->Team == self->SelectedTeam) if(u->Type->Cmd & CMD_CATAPULT)
	return &b.DefaultS;

 return 0;
}







static boxSC* mlRightpanelSwitch;
static boxSC* mlRightpanelShift;
static boxSC* mlRightpanelSelectPlane(boxC& b)
{
 if(!self->Focus)
    return mlRightpanelSwitch;

 int cnt = 0;
 for(unitC* u = self->Focus->Unit; u; u = u->Next)
    if(u->Team == self->SelectedTeam)
	if(!u->CanPlaneShift()) return mlRightpanelSwitch;
	else cnt++;

 if(cnt > 0) return mlRightpanelShift;
 return mlRightpanelSwitch;
}

static void btfSwitch() { cmd("plane"); }
static void btfShift() { cmd("shift"); }





/*=============================================================================
 * ECONOMY
 */
static void mlAsset(float size, int gfx, signed d, signed a)
{
 // move to the bottom of the available space
 glTranslated(0, -size, 0);
 fntS(size*1.2);
 glPushMatrix();

    // total
    bTColor();
    if(a >= 0) fntR("%5d  ", a);

    // gfx
    if(gfx)
    {
	glColor3f(1,1,1);
	glPushMatrix();
	    glScaled(size, size, 1);
	    glTranslated(-.5, +.5, 0);
    	    cntDrawRes(gfx, 1);
	glPopMatrix();
    }

    // delta/turn
    if(d < 0)	bHColor();
    else	bTColor();
    fntL("  %c%2d", d<0?'-':'+', abs(d));

 glPopMatrix();
}


static void mlEconomyDisplay(boxC& b)
{
 bwinDisplay(b);
 glTranslated(.5, b.H-.01, 0);

 float s = b.H/3;
 mlAsset(s, CNT_GOLD, self->DeltaGold(), self->GoldReserve);
 mlAsset(s, CNT_MANA, self->DeltaMana(), self->ManaReserve);
 mlAsset(s, CNT_FOOD, self->DeltaFood(), -1);
}



static bool mlEconomyInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;
 bClick(b);
 bOpen("economy");
 return true;
}





/*=============================================================================
 * PLAYERS
 */
void mlPlayerDisplay(boxC& b)
{
 brainC* p = game->Brains + b.Id;

// bwinDisplay(b);
 p->Display(b.H);
}



bool mlPlayerInput(boxC& b)
{
 if(!b.Mouse) return false;

 brainC* p = game->Brains + b.Id;

 if(mouseM())
 {
    bClick(b);
    popupPlayerS::Open(p);
    return true;
 }

 if(mouseL())
 {
    bClick(b);
    bOpen("players");
    return true;
 }

 return false;
}



static boxSC* mlPlayerStatus(boxC& b)
{
 brainC* p = game->Brains + b.Id;
 if(p->Status == BRAIN_UNUSED) return 0;
 return &b.DefaultS;
}





/*=============================================================================
 * MINI VIEW
 */
static void mlPRes(int id, signed n)
{
 glPushMatrix();
    cntDrawRes(id, n);
 glPopMatrix();
 glTranslated(0, -1, 0);
}


static void mlDisplayMiniCity(boxC& b, cityC* c)
{
 float h = .15;
 glTranslated(.02, b.H-h, 0);

 glPushMatrix();
    glColor3f(1,1,1);
    glTranslated(h/2, .02, 0);
    glScaled(h, h, h);
    c->Display(0);
 glPopMatrix();

 glPushMatrix();
    glTranslated(h, -.02, 0);
    if(c->Owner) c->Owner->Color->Select();
    else colorNeutral()->Select();
    fntS(0, h, .96-h);
    fntL("%s", c->Name);
 glPopMatrix();


 glPushMatrix();
    glTranslated(.001, -h-0.001, 0);
    h /= 2;
    bTColor();
    fntS(0, h, .96);
    fntL("Pop: %d (%+d /turn)\n", c->TotalPop*1000 + c->Pop, c->Growth());

    glScaled(.8*h, .8*h, 1);
    glTranslated(.1, .5, 0);
    mlPRes(CNT_FOOD, c->Food());
    mlPRes(CNT_WORK, c->Work());
    mlPRes(CNT_GOLD, c->Gold());
    mlPRes(CNT_MANA, c->Mana());
    mlPRes(CNT_KNOW, c->Know());
 glPopMatrix();

 glPushMatrix();
    glTranslated(.8, -.29, 0);
    glScaled(.25, .25, 1);
    city_displayProdItem(c, c->Queue->Type, c->Queue->Id);
    glTranslated(0, -.7, 0);
    bTColor();
    fntS(.23);
    int k = c->TurnsLeft();
    if(k != -1) fntC("%d Turns", k);
 glPopMatrix();
}



void mlDisplayMiniview(boxC& b)
{
 if(mlMidpanelU && mlMidpanelU->IsAlive())
 {
    bwinDisplay(b);
    popupUnitS::Draw(mlMidpanelU, b.H);
 }
 else if(MouseCell && MouseCell->City)
 {
    bwinDisplay(b);
    mlDisplayMiniCity(b, MouseCell->City);
 }
}





/*=============================================================================
 * GENERICS
 */
static void MENUdraw(boxC& b)
{
 // map
 MouseCell = mlMap(mouseX(), mouseY());
 bLoadCoord();

 MapView.MouseScroll();
}




// not static as it is used by other menues
void rotMod(signed k)
{
 char bf[20];
 sprintf(bf, "%.1f", atof(envGet("rot"))+k);
 envSet("rot", bf);
}





static bool MENUinput(boxC& b)
{
 // normal keys
 switch(inputK()) {
    case 27: exit(0); //netReset(); menuOpen("main"); break;
    case ' ': spellGuiOpenBook(); break;
    case '\t': bClick(b); cmd("plane"); return true;

    case '+': MapView.Zoom( 1); return true;
    case '-': MapView.Zoom(-1); return true;

    case 'r': rotMod(-5); return true;
    case 'f': rotMod(+5); return true;

    default:
	void mvInput(char);
	mvInput(inputK()); break;
 }

 // extended keys
 double sp = atof(envGet("scrollspeed"));
 switch(inputE()) {
    case INPUT_KUP: MapView.Scroll( 0.0, +sp); break;
    case INPUT_KDN: MapView.Scroll( 0.0, -sp); break;
    case INPUT_KLF: MapView.Scroll( -sp, 0.0); break;
    case INPUT_KRI: MapView.Scroll( +sp, 0.0); break;
    case INPUT_MUP: MapView.Zoom( 1); break;
    case INPUT_MDN: MapView.Zoom(-1); break;
    case INPUT_F2: popupPlayerS::Open(self); break;

    default: break;
 }

 btfMap();

 return false;
}




static char* MENUcheck()
{
 if(!gameIsAvailable()) return "no game available";

 return 0;
} 



static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 // init
 mlMidpanelU = 0;



 // mapview
 MapView.MapW = game->Map.W;
 MapView.MapH = game->Map.H;
 MapView.WCycle = true;

 MapView.ScrX = 0;
 MapView.ScrY = 1/3.;
 MapView.ScrW = 1.;
 MapView.ScrH = 2/3.;

 MapView.CellsInMidline = 12.;

 if(!self->ResearchedSpell)
    popupResearchS::Open();

 return 0;
}





/*=============================================================================
 * LAND MENU
 */
static spellTypeC* supportedtypes[] = {
    &spellTypeUnit,
    &spellTypeCity,
    &spellTypeBattle,
//@@    &spellTypeMap,
//@@    &spellTypeNode,
    0
};



static bmenuDECLARE( land ) {

    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "main";
    ProvidedSpellTargets = (void**)supportedtypes;

    boxC* b;
    boxC* sub;
    double x, y, s;

    // minimap
    b = AddWindow("Minimap",
	"",
	.002, .002,
	.25, .25,
	mlMinimapInput,
	mlMinimapDisplay);

    // midpanel
    b = AddWindow("Midpanel",
	"",
	.25+.002, .001, .5-.004, BMENU_H/3, 0, 0);
    b->SelectF = mlMidpanelSelect;
    b->DefaultS.DisplayF = mlMidpanelDefaultDisplay;
    b->DefaultS.InputF = mlMidpanelDefaultInput;

    //@@ add surveyor

    mlMidpanelBattle = new boxSC(*b, 0);
    mlMidpanelBattle->InputF = mlMidpanelBattleInput;
    mlMidpanelBattle->DisplayF = mlMidpanelBattleDisplay;
    mlMidpanelBattle->Name = "Mini Battle Status";
    mlMidpanelBattle->Help =
	"Shows you the battle status of the cell you have selected.";

    mlMidpanelUnits = new boxSC(*b, 0);
    mlMidpanelUnits->InputF = mlMidpanelUnitsInput;
    mlMidpanelUnits->DisplayF = mlMidpanelUnitsDisplay;
    mlMidpanelUnits->Name = "Troops";
    mlMidpanelUnits->Help =
	"This is the list of the troop units in the cell you have selected.\n"
	"\nFrom here you can group units so that they will move together,"
	" waiting for the slowest members but also exploiting group"
	" movement bouses like ?HForester?/, ?HMountaneer?/ and"
	" ?HPathfinder?/ that benefit the whole group even if only one"
	" unit has it.\n"
	"\nFor example, you main character has both ?HForester?/ and"
	" ?HMountaneer?/: if you group her with some of your normal troops,"
	" they will be able to pass high mountains or forests, normally"
	" unpassable.\n\n";
    int rows = 4;
    double size = mlMidpanelUnits->Box.Hg / mlMidpanelUnits->Box.Wg / rows;
    int cols = (int)(1. / size);
    for(int i = 0; i < rows; i++) for(int j = 0; j < cols; j++)
    {
	x = (cols-j-1) * size;
	y = i * size;

	mlMidpanelUnits->AddWindow(0,0, x, y, size, size,
	    mlMidpanelUnitInput,
	    mlMidpanelUnitDisplay);
    }



    // rightpanel
    b = AddWindow(0, 0, .749, .002, .25, BMENU_H/3., 0, 0);
    b->DefaultS.AddButton(" Next Turn ",
	"The game next turn will start as soon as every player has completed"
	" his moves and pressed the Next Turn button.\n\n"
	"Wizards that are faster in managing their empires have more time"
	" to dedicate to the arcane arts, as such if you are faster than"
	" the other players to call Next Turn, your ?HSpell Skill?/ will"
	" increse faster and you will receive a bonus on your ?HResearch?/.\n",
	.0, .0, 1, .2, 13, btfTurn);
    y = b->Hg/b->Wg -.25;
    // rightpanel: purify
    sub = b->DefaultS.AddButton(
	"Purify",
	"The clerics in the selected group will begin purifying corrupted land.\n"
	"The more cleric units in the group, the faster will be the process.\n",
	.25, y, .25,.25, 'p', btfPurify);
	sub->SelectF = mlRightpanelSelectPurify;
    // rightpanel: settle / raze / meld
    sub = b->DefaultS.AddWindow(0,0,
	.0, y, .25, .25, 0, 0);
	sub->SelectF = mlRightpanelSelectReMeSe;
	mlRightpanelRaze = &sub->DefaultS;
	mlRightpanelRaze->AddButton("Raze",
	    "Tells your units to destroy the city and slughter all the population.\n"
	    "No, it's not a nice hing to do.\n"
	    "Also, expect citizens to fight a desperate guerrilla: you will not be"
	    " able to intervene directly in the combat.\n",
	    0,0,1,1, 0, btfRaze);
	mlRightpanelMeld = new boxSC(*sub, 0);
	mlRightpanelMeld->AddButton("Meld",
	    "A ?HMagic Spirit?/ is able to meld with the Node, channelling"
	    " its magic power directly to you.\n"
	    "After melding, the Node will be surrounded by an aura with your"
	    " color; the wider the aura, the larger the magic power you obtain"
	    " each turn from the node.",
	    0,0,1,1, 'm', btfMeld);
	mlRightpanelSettle = new boxSC(*sub, 0);
	mlRightpanelSettle->AddButton("Settle",
	    "Settlers can create new cities or settle down in cities of their"
	    " same race that already exist.\n"
	    "If you create a new city,"
	    " the place you choose for the new city is very important, so"
	    " you'll want to use the ?HSurveryor?/ to check the resources"
	    " available for a city at a certain location.\n"
	    "To use the Surveyor hold down CTRL and move the mouse on the"
	    " main map.\n"
	    "You will want to choose a location where the Surveyor indicates"
	    " a large maximum population, that will also make the city"
	    " grow faster.\n",
	    0,0,1,1,'s', btfSettle);
    // rightpanel: road
    sub = b->DefaultS.AddButton(
	"Road",
	"Engineers can build roads, that allow troops to move faster.\n"
	"Different types of grounds will require different amount of time for"
	" roads to be built, but you can speed up the construction having more"
	" engineer units work together.\n"
	"You can add normal units to the group: they will move along with the"
	" engineers, escorting them.\n"
	"If the engineers are escorted by units that allow them to move"
	" in forests or mountains, they can build roads there as well.\n"
	"Once a road is build on mountains or forests, every land unit will"
	" be able to move through it.",
	.50, y, .25,.25, 'r', btfRoad);
	sub->SelectF = mlRightpanelSelectRoad;
    // rightpanel: siege
    sub = b->DefaultS.AddButton(
	"Siege",
	"Before attacking, Engineers can build siege engines (usually"
	" catapults).\n"
	"Once the siege engine has been built, the engineers can move it"
	" only once before they have to build it again.\n",
	.75, y, .25,.25, 'c', btfSiege);
	sub->SelectF = mlRightpanelSelectSiege;
    // rightpanel: spellbook
    y = .23;
    b->DefaultS.AddButton(
	"Spells",
	"Opens the Spellbook.",
	0, y, .33, .33, 'b', btfSpell);
    // rightpanel: plane
    sub = b->DefaultS.AddWindow(0,0,
	.34, y, .33, .33, 0, 0);
	sub->SelectF = mlRightpanelSelectPlane;
	mlRightpanelSwitch = &sub->DefaultS;
	mlRightpanelSwitch->AddButton("Plane",
	"",
	0,0,1,1, 0, btfSwitch);
	mlRightpanelShift = new boxSC(*sub, 0);
	mlRightpanelShift->AddButton("Shift",
	"",
	0,0,1,1, 'p', btfShift);


    // rightpanel: game options
    b->DefaultS.AddButton(
	"Game",
	"game options",
	.67, y, .33, .33, 'o', btfOptions);


    // minidetails
    b = AddWindow(0, 0,
	.99-.3, BMENU_H/3+0.01, .3, .23, 0, mlDisplayMiniview);


    // economy
    b = AddWindow("Economy",
	"The first row displays your ?Hgold?/ reserve (left) and your ?Hgold?/"
	" per-turn income (right).\n"
	"You use gold for maintaining troops and buildings in the cities,"
	" to speed up production, to hire mercenaries and buy items.\n"
	"Gold income is based on taxes your citizens pay to you.\n"
	"\n"
	"The second row displays your ?Hmagic power?/ reserve (left) and your"
	" ?Hmagic power?/ per-turn income (right).\n"
	"You use your magic power to cast and maintain spells and enchantements,"
	" and to summon and maintain magical creatures.\n"
	"The main sources of magic power are the religious buildings of your"
	" cities and the nodes you conquer and bind with a ?HMagic Spirit?/.\n"
	"\n"
	"The third row displays your ?Hfood?/ per-turn income.\n"
	"Having a positive value is useless, because food cannot be stored"
	" for the next turn.\n"
	"On the other hand, if it's negative your troops will starve to death"
	" and your cities may revolt against you.\n\n",
        1-.22, BMENU_H-.12, .2, .1,
	mlEconomyInput,
	mlEconomyDisplay);

    // players!
    s = .035;
    x = BDEF_BORDER;
    y = BMENU_H - BDEF_BORDER;
    for(int i = 0; i < GAME_MAX_BRAINS; i++)
    {
	boxC* b = AddWindow(
	    "Players",
	    "This is the list of all the Wizards in the game.\n"
	    "When a icon starts blinking, it means that that player has"
	    " pressed Next Turn.\n"
	    "Keep an eye to this, because a player that waits too long for"
	    " you to press Next Turn will receive bonus to Research and"
	    " spell Skill.",
	    x, y-=s, BDEF_BUTTON_W, s,
	    mlPlayerInput, mlPlayerDisplay);
	b->Id = i;
	b->SelectF = mlPlayerStatus;
    }

} bmenuINSTANCE;





//EOF =========================================================================
