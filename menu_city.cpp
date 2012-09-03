/*
 MENU_CITY.CPP		v0.05
 City main menu.

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

#include "map_view.h"

#include "game.h"
#include "city.h"
#include "race.h"
#include "map.h"
#include "fnt.h"





/*=============================================================================
 * LOCALS
 */
static cityC* City;





/*=============================================================================
 * PRODUCTION
 */
static bool mcProductionInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 bClick(b);
 bOpen("prod");
 return true;
}



static void mcProductionDisplay(boxC& b)
{
 bwinDisplay(b);

 // item gfx
 glPushMatrix();
    glTranslated(b.H/2, b.H/2, 0);
    glScaled(b.H*.9, b.H*.9, 1);
    city_displayProdItem(City, City->Queue->Type, City->Queue->Id);
 glPopMatrix();

 glPushMatrix();
    glTranslated(.98, 4*b.H/5, 0);
    bTColor();
    fntS(0, b.H/5, 1);
    fntR("%s\n\n", City->QName(City->Queue));
    if(int cost = City->QProdCost(City->Queue))
    {
	fntR("%d of %d\n", City->ProdStatus, cost);
	fntR("%d Turns\n", City->TurnsLeft());
    }
    else if(City->Queue->Times)
	fntR("%d Turns\n", City->Queue->Times);

 glPopMatrix();

 // current item
}





/*=============================================================================
 * OUTPUT
 */
static bool mcOutInput(boxC& b)
{
 //@@middle/left: details

 return false;
}



static void mcOutDisplay(boxC& b)
{
 int cnt;
 switch(b.Id) {
    case CNT_FOOD: cnt = City->Food(); break;
    case CNT_WORK: cnt = City->Work(); break;
    case CNT_GOLD: cnt = City->Gold(); break;
    case CNT_MANA: cnt = City->Mana(); break;
    case CNT_KNOW: cnt = City->Know(); break;
 }

 glScaled(b.H, b.H, 1);
 glTranslated(.1, .5, 0);
 cntDrawRes(b.Id, cnt);
}





/*=============================================================================
 * VIEW
 */
#define BLDS_COLS	5
#define BLDS_SIZE	1/5.

static bool mcViewInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;


 long blds = City->VisibleBlds();

 int x = (int)(b.MouseX * BLDS_COLS);
 int y = (int)(b.MouseY * BLDS_COLS);
 int i = x + y*BLDS_COLS;

 int k;
 for(k = 0; k < BLD_MAX; k++) if(blds & (1<<k)) if(!i--) break;

 if(i == -1)
    popupYesNoS::Open(
	"sell %s"
	"|"
	"Sell %s's %s for %d gold?",
	City->Race->Blds[k]->Id,
	City->Name,
	City->Race->Blds[k]->Name,
	City->BldSellPrice(k));

 return true;
}



static void mcViewDisplay(boxC& box)
{
/*@@
 set a random X,Y position to all blds/ench, based on city seed

 load a perspective
 draw each bld as gSelect(), but with alpha test enabled
 get mouse target
 draw each bld normally
 */

 bwinDisplay(box);

 glScaled(BLDS_SIZE, BLDS_SIZE, 1);
 glTranslated(.5, .5, 0);

 long b = City->VisibleBlds();
 int i = 0;
 for(int k = 0; k < BLD_MAX; k++) if(b & (1<<k))
 {
    glPushMatrix();
	glTranslated(i % BLDS_COLS, i / BLDS_COLS, 0);
	city_displayBld(City, k);
    glPopMatrix();
    i++;
 }
}





/*=============================================================================
 * MAP
 */
static bool mcMapInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 //@@ mouseM shows surveyor

 bClick(b);
 menuLand_CenterAt(City->Cell->X, City->Cell->Y);
 bOpen("land");
 return true;
}



static void mcMapDisplay(boxC& b)
{
 mapViewC mv;
 mv.MapW = game->Map.W;
 mv.MapH = game->Map.H;
 mv.WCycle = true;

 mv.ScrX = b.DefaultL.Xg;
 mv.ScrY = b.DefaultL.Yg + b.Hg*.64;
 mv.ScrW = b.Wg;
 mv.ScrH = b.Hg/BMENU_H;

 mv.Rot = 5;
 mv.CellsInMidline = 5;
 mv.CenterAt(City->Cell->X, City->Cell->Y);

 glClear(GL_DEPTH_BUFFER_BIT);
 glPushMatrix();
    mdDisplay(0, mv);
    bLoadCoord();
 glPopMatrix();
 glDisable(GL_DEPTH_TEST);

 bdrawContour(b, true, false);
}





/*=============================================================================
 * NAME and POP
 */
#define CITIZEN_W	.345

static bool mcPopInput(boxC& b)
{
 if(!b.Mouse) return false;

 if(mouseL())
 {
    int p = 1 + (int)(b.MouseX / b.H / CITIZEN_W);
    int w = City->TotalPop - City->Unrest() - p;
    if(p <= City->FarmPop) w++;
    if(w < 0) w = 0;

    cmd("pop %d\n", w);
    bClick(b);
    return true;
 }

 if(mouseM())
 {
    //@@ popup workers/farmers/rebel
    //: production, food and mana (for popmana bonus) for each
    return true;
 }


 return false;
}


static void mcPopDisplay(boxC& b)
{
 bdrawContour(b, false, false);
 glScaled(b.H, b.H, 1);
 glTranslated(.2, 0, 0);

 fleshMultiStart();
 for(int i = 0; i < City->TotalPop; i++)
    {
	glPushMatrix();
	    glScaled(.9,.9,1);
	    if(i < City->FarmPop) city_displayFarmer(City);
	    else if(i < City->FarmPop + City->WorkPop) city_displayWorker(City);
	    else city_displayRebel(City);
	glPopMatrix();
	glTranslated(CITIZEN_W, .0, .0);
    }
 glDisable(GL_DEPTH_TEST);
}


static void mcNameDisplay(boxC& b)
{
 City->Owner->Color->Select();
 fntS(0, b.H, 1);
 fntL("%s", City->Name);

 glTranslated(1,0,0);
 if(City->Growth() < 0) bHColor();
 else glColor3f(1,1,0);
 fntS(0, b.H/4);
 fntR("Pop: %d, (%+d)", City->TotalPop*1000 + City->Pop, City->Growth());
}





/*=============================================================================
 * GENERICS
 */
static void MENUdraw(boxC& b)
{
 // select yourself!
 if(spellGuiCity())
    spellGuiDo(City->Cell->X, City->Cell->Y, City->Cell->Z);

 //@@
 glPushMatrix();
    bTColor();
    glTranslated(.65, .25, 0);
    fntS(0, .03, 0);
    fntL("Enchatements:\n");
    for(spellC* s = spellsHook; s; s = s->SpellNext)
	if(s->Type == &spellTypeCity && s->Object != SPELL_OBJ_NONE)
	    if(City->Ench[s->Object])
		fntL("?>?H%s?/ ?<\n", s->Name);
 glPopMatrix();

 glPushMatrix();
    bTColor();
    fntS(0, .03, 0);
    glTranslated(.65, .04, 0);
    fntL("@@Add:GarrisonView");
 glPopMatrix();
}



static bool MENUinput(boxC& b)
{
 void countersF();

 switch(inputK()) {
    case 27: bOpen("land"); break;
    case ' ': spellGuiOpenBook(); break;
    case 13: cmd("ready"); break;

    case 'b': pf("BasePop %d\n", City->TerrainBasePop()); break;
    case 'f': pf("Food\n"); City->Food(); countersF(); break;
    case 'w': pf("Work\n"); City->Work(); countersF(); break;
    case 'g': pf("Gold\n"); City->Gold(); countersF(); break;
    case 'p': pf("Pop\n"); City->Growth(); countersF(); break;
    case 'u': pf("Unrest\n"); City->Unrest(); countersF(); break;
    case 'G': pf("Growth %d\n", City->Growth()); countersF(); break;

    default: break;
 }

 return false;
}





static char* MENUcheck()
{
 if(!gameIsAvailable()) return "no game";


 // city menu requires a focused cell...
 cellC* c = self->LocalFocus;
 if(!c) return "no cell selected";


 // ...and focused cell must contain a city
 City = c->City;
 if(!City) return "no city in selected cell";


 // also, self must own the city
 if(City->Owner != self) return "not city owner";

 // city menu ok! =)
 return 0;
}





static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 return 0;
}



/*=============================================================================
 * CITY MENU
 */
#define BORDER	(BDEF_BORDER/2)

#define NAME_X	POP_X
#define NAME_Y	(POP_Y+POP_H)
#define NAME_W	POP_W
#define NAME_H	(BMENU_H - NAME_Y)

#define POP_X	BORDER
#define POP_Y	(BMENU_H - 2*BORDER - 2*POP_H)
#define POP_W	(1 - 3*BORDER - MAP_W)
#define POP_H	.065

#define OUT_X	BORDER
#define OUT_Y	(VIEW_Y+VIEW_H+BORDER)
#define OUT_W	((POP_W - BORDER)/2)
#define OUT_H	(POP_Y - OUT_Y - BORDER)

#define PROD_X	(OUT_X + OUT_W + BORDER)
#define PROD_Y	OUT_Y
#define PROD_W	(PROD_X - VIEW_X - BORDER)
#define PROD_H	OUT_H

#define MAP_X	(1 -BORDER -MAP_W)
#define MAP_Y	(BMENU_H -BORDER -MAP_H)
#define MAP_W	.3
#define MAP_H	.3

#define VIEW_X	BORDER
#define VIEW_Y	BORDER
#define VIEW_W	.6
#define VIEW_H	(BMENU_H - MAP_H - 3*BORDER)



static spellTypeC* st[] = {
    &spellTypeCity,
    0
};



static bmenuDECLARE( city ) {
    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "land";
    ProvidedSpellTargets = (void**)st;

    boxC* b;
    float y, s;

    // city name
    AddWindow(0, 0,
	NAME_X, NAME_Y, NAME_W, NAME_H,
	0, mcNameDisplay);

    // city
    AddWindow(
	"Population",
	"Each figure represents 1000 citizens.\n"
	"Each of those 1000 citizens needs one unit of food, and can also"
	" be assigned to farming or working.\n"
	"Farmers produce food and some little manpower, while workers"
	" provide manpower but no food at all.\n"
	"Rebel figures cannot be assigned any task.\n"
	"\n"
	"?HBugs:?/\n"
	"There is an automated function for assigning workers and farmers:"
	" it sets the right amount of workers to complete the construction in less"
	" turn possible while avoiding starvation, but there's no GUI to activate"
	" it.\n"
	"Use the command ?Hpop?/ without arguments to activate it.\n\n",
	POP_X, POP_Y, POP_W, POP_H, mcPopInput, mcPopDisplay);

    // output
    b = AddWindow(0,0,
	OUT_X, OUT_Y, OUT_W, OUT_H, 0, 0);
	y = OUT_H/OUT_W;
	s = y/5.;
	b->DefaultS.AddWindow("Food",
	    "?>Each ?F represents 1 food unit. ?<\n"
	    "?>Each ?F10 represents 10 food units. ?<\n"
	    "?HBlackened icons?/ (?F-1, ?F-10) mean a negative value:"
	    " each 1000 citizens require a unit of food,"
	    " if food is negative, the population will starve and die.\n"
	    "Excess food is used to sustain your troops.\n",
	    0, y-=s, 1, s, mcOutInput, mcOutDisplay)->Id = CNT_FOOD;
	b->DefaultS.AddWindow("Workforce",
	    "?>Each ?W represents 1 workforce unit. ?<\n"
	    "?>Each ?W10 represents 10 workforce units. ?<\n"
	    "This is the workforce available for creating new buildings,"
	    " training troops or manifacturing.\n",
	    0, y-=s, 1, s, mcOutInput, mcOutDisplay)->Id = CNT_WORK;
	b->DefaultS.AddWindow("Gold",
	    "?>Each ?G represents 1 gold unit. ?<\n"
	    "?>Each ?G10 represents 10 gold units. ?<\n"
	    "Gold comes mainly from taxation of citizens, and usually"
	    " a significant part of it is used for building maintenance:"
	    " each building has a different ?HGold Upkeep Cost?T.\n"
	    "Excess gold goes directly to your treasure and i's at your"
	    " disposal.\n"
	    "?HBlackened icons?/ (?G-1, ?G-10) mean a negative value: the city is draining"
	    " wealth instead of producing it.\n"
	    "Usually this is because"
	    " the city is too small and citizens taxes don't cover"
	    " the gold upkeep of too many expensive buildings.\n",
	    0, y-=s, 1, s, mcOutInput, mcOutDisplay)->Id = CNT_GOLD;
	b->DefaultS.AddWindow("Magic Power",
	    "?>Each ?M represents 1 magic power unit. ?<\n"
	    "?>Each ?M10 represents 10 magic power units. ?<\n"
	    "Magic power comes mainly from religious buildings.\n"
	    "The city does not normally use any of the magic power it produces,"
	    " unless to maintain city enchantements you may"
	    " have cast on the city itself or other special cases.\n"
	    "?HBlackened icons?/ (?M-1, ?M-10) mean a negative value.\n",
	    0, y-=s, 1, s, mcOutInput, mcOutDisplay)->Id = CNT_MANA;
	b->DefaultS.AddWindow("Research",
	    "?>Each ?K represents 1 research unit. ?<\n"
	    "?>Each ?K10 represents 10 research units. ?<\n"
	    "Centers of culture and knowledge carry on magical research for"
	    " you and will slowly provide you with new spells.",
	    0, y-=s, 1, s, mcOutInput, mcOutDisplay)->Id = CNT_KNOW;

    // prod
    AddWindow(
	"Production",
	"",
	PROD_X, PROD_Y, PROD_W, PROD_H, mcProductionInput, mcProductionDisplay);

    // city view
    AddWindow(
	"City View",
	"",
	VIEW_X, VIEW_Y, VIEW_W, VIEW_H, mcViewInput, mcViewDisplay);

    // map view
    AddWindow(
	"Map View",
	"Here you see the city and the surrounding areas.\n"
	"The city exploits the resources of these areas, depening on the"
	" distance:\n"
	"?>The four areas at the corners are too far and cannot be exploited. ?<\n"
	"?>The twelve areas in the outer circle are exploited, but yeld only half"
	" of their resources. ?<\n"
	"?>The nine areas of the inner circle, including the one where the city"
	" lies, are fully exploited and yeld the full amount of their resources."
	" ?<\n\n\n",
	MAP_X, MAP_Y, MAP_W, MAP_H, mcMapInput, mcMapDisplay);



} bmenuINSTANCE;



//EOF =========================================================================
