/*
 MENU_PROD		v0.03
 City production menu.

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

 TODO:
    hovering mouse on avl display details


*/

#include <GL/gl.h>
#include <stdio.h>

#include "main.h"
#include "boxgui.h"
#include "fnt.h"

#include "game.h"
#include "race.h"
#include "city.h"
#include "map.h"
#include "cmd.h"

#include "utype.h"
#include "colors.h"
#include "counters.h"
#include "spell.h"


dataC dataSpc("spc.png", 1024./102, 1024./102);




/*=============================================================================
 * GUI
 */
#define BORDER	(BDEF_BORDER/2)
#define ITEM_S 		((1 - (QUEUE_CNT+1)*BORDER) / QUEUE_CNT)

#define QUEUE_CNT	(CITY_QUEUE_BF -1)
#define QUEUE_X		BORDER
#define QUEUE_Y		(BMENU_H - QUEUE_H - BORDER)
#define QUEUE_W		(1 - 2*BORDER)
#define QUEUE_H		(ITEM_S*1.25)

#define FIRST_X		(1 - FIRST_W - BORDER)
#define FIRST_Y		(QUEUE_Y - BORDER - FIRST_H)
#define FIRST_W		ITEM_S
#define FIRST_H		QUEUE_H

#define DONE_X		(1 - DONE_W - BORDER)
#define DONE_Y		BORDER
#define DONE_W		ITEM_S
#define DONE_H		(.8 * BDEF_BUTTON_H)

#define DETAILS_X	BORDER
#define DETAILS_Y	BORDER
#define DETAILS_W	(AVL_X - 2*BORDER)
#define DETAILS_H	(QUEUE_Y - 2*BORDER)

#define AVL_X		(FIRST_X - AVL_W - BORDER)
#define AVL_Y		BORDER
#define AVL_W		(4 * ITEM_S)
#define AVL_H		(3 * ITEM_S) 

#define AVLT_X		AVL_X
#define AVLT_Y		(AVL_Y + AVL_H)
#define AVLT_W		AVL_W
#define AVLT_H		BDEF_TAB_H

#define ALCHEM_X	(DETAILS_X + DETAILS_W + BORDER)
#define ALCHEM_Y	(AVLT_Y + AVLT_H + BORDER)
#define ALCHEM_W	(ALCHEM_H * 4)
#define ALCHEM_H	(QUEUE_Y - ALCHEM_Y - BORDER)





/*=============================================================================
 * LOCALS
 */
static cityC*	City;
static long	mpAvlType = Q_UNIT;
static long	mpAvlHId = 0;
static long	mpSQueue = 0;
static long	mpHQueue = 0;

static unitC*	mpDetailsU = 0;




/*=============================================================================
 * COMMAND HELPERS
 */
static void mpQCmd(int qi, int type, int id, int times, int flags)
{
 cmd("queue %d %d %d %d %d", qi, type, id, times, flags);
}


static void mpQCmdRm(int qi)
{
 mpQCmd(qi, Q_FREE, 0, 0, 0);
}


static void mpQCmdPush(int qi)
{
 if(!City->Queue[qi-1].Type) return;
 mpSQueue = qi;
 mpQCmd(qi, Q_PUSH, 0, 0, 0);
}


static void mpQCmdTimes(int qi, signed dt)
{
 queueC* q = City->Queue + qi;
 if(q->Times + dt < 0) return;
 mpQCmd(qi, q->Type, q->Id, q->Times + dt, q->Flags);
}



static void mpQCmdFlag(int qi, int f)
{
 queueC* q = City->Queue + qi;
 mpQCmd(qi, q->Type, q->Id, q->Times, f);
}



static bool mpWheelInput(boxC& b, int qi, int type, int id)
{
 if(type == Q_BLD) return false;

 queueC* q = City->Queue + qi;

 if(inputE() == INPUT_MUP)
 {
    bClick(b);
    mpQCmd(qi, type, id, q->Times + 1, q->Flags);
    return true;
 }

 if(inputE() == INPUT_MDN)
 {
    bClick(b);
    if(q->Times == 0) return true;
    mpQCmd(qi, type, id, q->Times - 1, q->Flags);
    return true;
 }

 return false;
}




/*=============================================================================
 * QSELECT CHECK
 */
void mpSQCheck()
{
 if(City->Queue[mpSQueue].Type) return;

 mpSQueue = CITY_QUEUE_BF-1;
 while(mpSQueue > 0 && !City->Queue[mpSQueue-1].Type)
    mpSQueue--;
}





/*=============================================================================
 * AVAILABLE ITEMS
 */
static bool mpAvlInput(boxC& b)
{
 if(!b.Mouse) return false;


 if(mpWheelInput(b, mpSQueue, mpAvlType, b.Id))
    return true;


 if(mouseL())
 {
    bClick(b);
    queueC* q = City->Queue + mpSQueue;

    if(q->Type == mpAvlType && q->Id == b.Id) mpSQueue++;
    else mpQCmd(mpSQueue, mpAvlType, b.Id, q->Times, q->Flags);

    return true;
 }


 if(mouseM())
 {
    bClick(b);
    mpQCmdPush(mpSQueue+1);
    mpQCmd(mpSQueue, mpAvlType, b.Id, 1, 0);
    return true;    
 }


 return false;
}



static void mpAvlItemDisplay(boxC& b)
{
 glPushMatrix();
    glTranslated(.5, .5, 0);
    city_displayProdItem(City, mpAvlType, b.Id);
 glPopMatrix();
 glDisable(GL_DEPTH_TEST);
}



static void mpAvlIdSelect(boxC& b)
{
 mpAvlHId = -1;

 long able = City->AvlItems(City->Queue+mpSQueue, mpAvlType);

 boxLC* l = b.DefaultS.SubBoxesHook;
 for(int k = 0; k < 32 && l; k++) if(able & (1<<k))
 {
    l->SubBox.Id = k; // | (mpAvlType<<8);
    l->SubBox.CurrentS = &l->SubBox.DefaultS;
    l = l->Next;
 }

 for(; l; l = l->Next)
    l->SubBox.CurrentS = 0;
}



static void mpAvlAdd(boxSC* p)
{
 float s, x;

 s = AVLT_W/3;
 x = AVLT_X -s;
 p->AddTab(Q_UNIT, &mpAvlType, "Troops",
    "Citizens can be recruited and trained to serve in your army:"
    " each troop unit you build will drain a small amount of population"
    " from the city; although such troops are often no mach for magical"
    " creatures, they can improve with training, experience and magic.\n"
    "Troops will gain experience only when facing significant challenges.\n"
    "\n"
    "Sometimes, a gifted individual stands out the ranks to become a hero.\n"
    "Look after heroes, as they can progress and improve far beyond common"
    " troops.",
 x+=s, AVLT_Y, s, AVLT_H);
 p->AddTab(Q_BLD, &mpAvlType, "Blds",
    "Buildings offer various improvements for the city.\n"
    "As much as they are vital for the developement of a city, their"
    " maintenance cost can have a very bad impact on its economy, so"
    " don't build something you don't need: for many cities some buildings"
    " are often superfluos.\n",
 x+=s, AVLT_Y, s, AVLT_H);
 p->AddTab(Q_SPC, &mpAvlType, "Special",
    "These are special thing you can have the city do.\n",
    x+=s, AVLT_Y, s, AVLT_H);

 blistC* list = p->AddList(4, 3, 0, &mpAvlHId,
    "Available Items List",
    "This is the list of the items that you can build at this point of"
    " the queue: most troop units and buildings can be built only once"
    " certain buildings are present in the city.\n"
    "\nFor example, a ?HSwordmen?/ troop won't be available unless the"
    " ?HBarracks?/ building"
    " is queued already or already built in the city.\n"
    "\n"
    "?>Use the Left mouse button to set an item for the currently selected queue"
    "entry, and click again to move to the next queue entry. ?<\n"
    "?>Use the mouse wheel to increase and decrease the repetitions for troops"
    " and specials. ?<\n\n\n",
//    "?>Use the Middle mouse button to set an item"
    AVL_X, AVL_Y, AVL_W, AVL_H, mpAvlInput, mpAvlItemDisplay);
 list->IdSelect = mpAvlIdSelect;
// for(boxLC* l = list->DefaultS.SubBoxesHook; l; l = l->Next)
//    l->SubBox.DefaultS.InputF = mpAvlInput;
}





/*=============================================================================
 * QUEUE
 */
static bool mpQFlagInput(int f, boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 queueC* q = City->Queue + b.Id;
 if(q->Type == Q_SPC) return false;

 bClick(b);
 int flags;

 if(q->Flags & f)	flags = q->Flags & ~f;
 else			flags = q->Flags | f;
 mpQCmdFlag(b.Id, flags);

 return true;
}



static void mpQFlagDisplay(int f, char* n, boxC& b)
{
 queueC* q = City->Queue + b.Id;

 bool used = q->Type != Q_SPC;
 bool down = used && (q->Flags & f);

 bdrawButtonBackground(b, down);
 bdrawContour(b, b.Mouse, down);

 if(!used) return;
 if(!down) n += 2;	// skip escape sequence ?H
 bdrawName(b, n, b.Mouse, down);
}



static bool mpQAsapInput(boxC& b)
{
 return mpQFlagInput(QF_BUY, b);
}

static void mpQAsapDisplay(boxC& b)
{
 mpQFlagDisplay(QF_BUY, "?H$", b);
}



static bool mpQAlertInput(boxC& b)
{
 return mpQFlagInput(QF_WARN, b);
}

static void mpQAlertDisplay(boxC& b)
{
 mpQFlagDisplay(QF_WARN, "?H!", b);
}



static bool mpQRemoveInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;
 bClick(b);
 mpQCmdRm(b.Id);
 return true;
}



static bool mpQItemInput(boxC& b)
{
 if(!b.Mouse) return false;

 queueC* q = City->Queue + b.Id;


 if(mpWheelInput(b, b.Id, q->Type, q->Id))
    return true;


 if(mouseL())
 {
    bClick(b);

    if(mpSQueue == b.Id) mpQCmdTimes(b.Id, +1);
    else mpSQueue = b.Id;
 }

 return false;
}

static void mpQItemDisplay(boxC& b)
{
 bwinDisplay(b);

 if(b.Mouse)
    bdrawWindowBackground(b, true, true);

 if(mpSQueue == b.Id)
    bdrawSelect(b, true, false, false);


 queueC* q = City->Queue + b.Id;
 if(!q->Type) return;


 if(b.Mouse)
    mpHQueue = b.Id;


 glPushMatrix();
    glTranslated(.5, .5, 0);
    city_displayProdItem(City, q->Type, q->Id);
 glPopMatrix();
 glDisable(GL_DEPTH_TEST);



 // times
 glTranslated(.98,.02,0);
 bTColor();
 fntS(.2);
 if(q->Type == Q_UNIT && q->Times > 1) fntR("x%d", q->Times);
 if(q->Type == Q_SPC)
    if(!q->Times)		fntR("inf");
    else if(q->Times == 1)	fntR("%d turn", q->Times);
    else			fntR("%d turns", q->Times);
}





static bool mpQArrowInput(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 bClick(b);
 mpQCmdPush(b.Id);
 return true;
}

static void mpQFirstArrowDisplay(boxC& b)
{
 bool down = b.Mouse && mouseLdn();
 bool highlight = b.Mouse;
 bdrawButtonBackground(b, down);
 bdrawContour(b, highlight, down); 

 glRotated(90, 0, 0, -1);
 glTranslated(-1, 0, 0);
 bdrawName(b, b.Name, highlight, down);
}




static void mpQAddSub(boxSC* p, int id)
{
 p->DisplayF = 0;

 float iY, tY;
 if(id)	iY = 0, tY = 1;
 else	tY = 0, iY = .25;

 boxC* b;

 // buy asap
 b = p->AddWindow("Buy as soon as possible",
    "If this button is down, the item will be bought as soon as the necessary"
    " gold is available and without asking you any confirmation.",
    .0 , tY, .33, .25, mpQAsapInput, mpQAsapDisplay);
    b->Id = id;

 // alert
 b = p->AddWindow("Alert when finished",
    "Normally the city will continue silently along the production queue.\n"
    "If this button is down, you will receive a notification as soon as this"
    " item is complete.",
    .33, tY, .33, .25, mpQAlertInput, mpQAlertDisplay);
    b->Id = id;

 // remove one or all
 b = p->AddButton("Delete item",
    "Mercilessly removes the item from the queue.",
    .66, tY, .34, .25, 0, 0);
    b->Name = "X";
    b->Id = id;
    b->DefaultS.InputF = mpQRemoveInput;

 // item
 b = p->AddWindow("Queue entry",
    "This is one entry in the production queue."
    "\nIt will be started when all the entries that precede it in the queue"
    " (ie, at the right) will be completed."
    "\nYou can repeat the production of Troops by left clicking, or set"
    " an amount of turns after which Housing or Trade Goods should be discarded"
    " from the queue (normally they go on ad infinitum!)."
    "\nIf greater than one, the times an entry will be produced is shown"
    " at bottom right.\n\n",
    0, iY, 1. , 1., mpQItemInput, mpQItemDisplay);
 b->Id = id;
}



static void mpQAdd(boxSC* p)
{
 boxC* b;

 // arrows
 float size = .02;
 char* ard = ">";
 char* arn = "Queue marker";
 char* arh = "Left click to add a queue entry inbetween.";
 for(int i = 0; i < QUEUE_CNT-1; i++)
 {
    float x = BORDER/2 + (i+1)*(ITEM_S+BORDER) - size/2;
    float y = QUEUE_Y + QUEUE_H/2 - size/2;
    b = p->AddButton(ard,arh, x, y, size, size, 0, 0);
    b->Id = QUEUE_CNT - i;
    b->Name = ard;
    b->DefaultS.InputF = mpQArrowInput;
 }
 b = p->AddButton(ard,arh,
    FIRST_X + FIRST_W/2 - size/2,
    FIRST_Y + FIRST_H - size/2 + 0.006,
    size, size, 0, 0);
 b->Id = 1;
 b->Name = ard;
 b->DefaultS.InputF = mpQArrowInput;
 b->DefaultS.DisplayF = mpQFirstArrowDisplay;


 // boxes
 boxSC* ds;
 for(int i = 0; i < QUEUE_CNT; i++)
 {
    float x = BORDER + i*(ITEM_S+BORDER);
    ds = &p->AddWindow(0,0, x, QUEUE_Y, ITEM_S, QUEUE_H, 0, 0)->DefaultS;
    mpQAddSub(ds, QUEUE_CNT-i);
 }
 ds = &p->AddWindow(0,0, FIRST_X, FIRST_Y, FIRST_W, FIRST_H, 0, 0)->DefaultS;
 mpQAddSub(ds, 0);
}





/*=============================================================================
 * DETAILS PANEL
 */
static boxSC* mpDetailsBld;
static boxSC* mpDetailsUnit;
static boxSC* mpDetailsSpc;

static int mpDetailsId;
static int mpDetailsWep;
static queueC* mpDetailsQ;




static void mpDetailsPushHeader(boxC& b, char* name, char* cnu)
{
 float hh = .8/6.;
 float gfxsize = 2*hh;

 bwinDisplay(b);
 glPushMatrix();
    glTranslated(gfxsize, b.H-1.5*hh, 0);
    bTColor();
    fntS(hh/2);
    fntL("%s", cnu);

    glTranslated(0, 2*hh, 0);
    bHColor();
    fntS(0, hh, 1-gfxsize-.03);
    fntL("\n%s", name);

 glPopMatrix();
 glPushMatrix();
    glTranslated(gfxsize/2, b.H-gfxsize/2, 0);
    glScaled(gfxsize, gfxsize, 1);
}








static bool mpDetailsBldInput(boxC& b)
{
 return false;
}

static void mpDetailsBldDisplay(boxC& box)
{
 bldC* b = City->Race->Blds[mpDetailsId];

 char bf[32];
 char* w = bf;

 w += sprintf(w, "?W1 %d", b->Cost);
 w += sprintf(w, "\n?F%d?G%d?M%d", b->Food<0?-b->Food:0, b->Gold, b->Mana<0?-b->Mana:0);
 mpDetailsPushHeader(box, b->Name, bf);
    glScaled(.9, .9, 1);
    city_displayBld(City, mpDetailsId);
 glPopMatrix();

 bTColor();
 fntS(0, .3/6, .98);
 glTranslated(.01, box.H-.35, 0);



 // allows:
 queueC* sq = mpDetailsQ;
 queueC pq = *sq;
 sq->Id = mpDetailsId;
 sq->Type = Q_BLD;
 
 long alwUnits = ~City->AvlUnits(sq) & City->AvlUnits(sq+1);
 long alwBlds =  ~City->AvlBlds(sq) & City->AvlBlds(sq+1);
 if(alwUnits || alwBlds)
 {
    fntL("Allows:\n");
    for(int k = 0; k < 32; k++) if(alwUnits & (1<<k))
	fntL("  %s\n", City->Race->GetUtype(k)->Name);

    for(int k = 0; k < BLD_MAX; k++) if(alwBlds & (1<<k))
	fntL("  %s\n", City->Race->Blds[k]->Name);
 }
 fntL("\n");
 *sq = pq;
 

 // also required by:
 long reqUnits = 0;
 utypeC* u;
 for(int i = 0; u = City->Race->GetUtype(i); i++)
    if(u->Req & (1<<mpDetailsId)) reqUnits |= 1<<i;
 reqUnits &= ~alwUnits;

 long reqBlds = 0;
 for(int i = 0; i < BLD_MAX; i++) if(City->Race->Blds[i]->Req & ~REQ_NON_BUILDABLE)
    if(City->Race->Blds[i]->Req & (1<<mpDetailsId)) reqBlds |= 1<<i;
 reqBlds &= ~alwBlds;

 if(reqUnits || reqBlds)
 {
    fntL("Required by:\n");
    for(int k = 0; k < 32; k++) if(reqUnits & (1<<k))
	fntL("  %s\n", City->Race->GetUtype(k)->Name);

    for(int k = 0; k < BLD_MAX; k++) if(reqBlds & (1<<k))
	fntL("  %s\n", City->Race->Blds[k]->Name);
 }
 fntL("\n");

 if(b->Unrest) fntL("?>Reduces unrest by %d ?<\n", b->Unrest);
 if(b->ProdB) fntL("?>?W1 +%d%% ?<\n", b->ProdB);
 if(b->GoldB) fntL("?>?G1 +%d%% ?<\n", b->GoldB);
 if(b->PopB) fntL("?>Increases population growth by %d ?<\n", b->PopB);

 if(b->Mana > 0) fntL("?>?M%d per turn ?<\n", b->Mana);
 if(b->Food) fntL("?>?F%d per turn ?<\n", b->Food);
 if(b->Know) fntL("?>?K%d per turn ?<\n", b->Know);

 // specials
 if(b->Spc & CSPC_ALCHEMIST)	fntL("?>Equips troops with magical weapons ?<\n");
 if(b->Spc & CSPC_FOOD)		fntL("?>Each farmer produces 3 food instead of 2 ?<\n");
 if(b->Spc & CSPC_FORECAST)	fntL("?>Enlarges city sight, detects invisible units, forecasts disasters ?<\n");
 if(b->Spc & CSPC_POPMANA)	fntL("?>Each farmer, worker or rebel produces .5 mana per turn ?<\n");
 if(b->Spc & CSPC_REGULAR)	fntL("?>All new troops start as Regulars ?<\n");
 if(b->Spc & CSPC_VETERAN)	fntL("?>All new troops start as Veteran ?<\n");
 if(b->Spc & CSPC_MINE)		fntL("?>Increases all special mineral bonuses by 50% ?<\n");
 if(b->Spc & CSPC_WALL)	fntT(.98,
    "?>A wall protects the troops inside the city from ranged attacks, and"
    " prevents normal enemy troops from entering and damaging the city.\n"
    "An army that cannot breach somehow the wall cannot conquer and"
    " claim the city. ?<\n");

 fntT(.98, "\n%s", b->Info);
}





static bool mpDetailsUnitInput(boxC& b)
{
 return false;
}

static void mpDetailsUnitDisplay(boxC& box)
{
 utypeC* u = City->Race->GetUtype(mpDetailsId);
 mpDetailsU->Type = u;
 mpDetailsU->Owner = self;


 // find actual cost
 queueC* sq = mpDetailsQ;
 queueC pq = *sq;
 sq->Id = mpDetailsId;
 sq->Type = Q_UNIT;
 int r = City->QProdCost(City->Queue + mpSQueue);
 *sq = pq;

 int b = u->Cost;

 // write cost and upkeep
 char bf[32];
 char* w = bf;
 if(b == r)	w += sprintf(w, "?W1 %d", b);
 else		w += sprintf(w, "?W1 %d [%d]", r, b);
 w += sprintf(w, "\n?F%d?G%d?M%d", u->Food, u->Gold, u->Mana);
 mpDetailsPushHeader(box, u->Name, bf);
    glTranslated(0, -.5, 0);
    glScaled(.7, .7, .7);
    duTypeStack(u, City->Owner->Color);
    glDisable(GL_DEPTH_TEST);
 glPopMatrix();



 //
 void duSetUnit(unitC* u);
 //
 duSetUnit(mpDetailsU);
}




static bool mpDetailsSpcInput(boxC& b)
{
 return false;
}

static void mpDetailsSpcDisplay(boxC& b)
{
 bwinDisplay(b);
}








static boxSC* mpDetailsSelect(boxC& b)
{
 int type, id, wep;

 if(mpAvlHId != -1)
 {
    type = mpAvlType;
    id = mpAvlHId;
    wep = 0;

    mpDetailsQ = City->Queue + mpSQueue;
 }
 else
 {
    queueC* q = 0;
    if(mpHQueue != -1) q = City->Queue + mpHQueue;
    else q = City->Queue + mpSQueue;

    type = q->Type;
    id = q->Id;
    wep = q->Flags & QF_WPN;

    mpHQueue = -1;
    mpDetailsQ = q;
 }


 mpDetailsId = id;
 mpDetailsWep = wep;
 switch(type) {
    case Q_BLD: return mpDetailsBld;
    case Q_UNIT: return mpDetailsUnit;
    case Q_SPC: return mpDetailsSpc;
    default: return 0;
 }


 /*


 char* name;
 char cost[100];
 switch(type) {
    case Q_UNIT: {
	utypeC* u = City->Race->GetUtype(id);
	"\n?f%d ?g%d ?m%d"


    } break;

 }




 // name
 bHColor();
 glTranslated(M_DETAILS_X+150, M_DETAILS_Y, 0);
 fntS(h);
 fntL("\n%s", city->QName(sq));

 // cost [base cost]
 if(sq->type != Q_SPC)
 {
    glPushMatrix();
	glScaled(s, s, s);
	glTranslated(0, .5, 0);
	cntDrawRes(CNT_WORK, 1);
    glPopMatrix();
    glPushMatrix();
	glTranslated(1.2*s, s, 0);
	fntS(.8*h);

	int base = city->QBaseCost(sq);
	int real = city->ProdCost(sq);
	if(base == real)	fntL("%d", real/100);
	else			fntL("%d [%d]", real/100, base/100);
    glPopMatrix();
 }

 // prepare for drawing upkeep
 glTranslated(0, 1.6*s, 0);
 glScaled(s, s, s); 

 // details
 switch(sq->type) {
    case Q_UNIT: mpDrawUnitDetails(); break;
    case Q_BLD:  mpDrawBldDetails(); break;
    default: break;
 }



 glTranslated(.1, b.H, 0);
 bTColor();
 fntS(.1);
 fntL("\n\nType: %d Id: %d", type, id);
*/
}






/*
static void mpDrawUnitDetails()
{
 utypeC* u = city->QUnit(sq);
 cntDrawUpkeep(u->Food, u->Gold, u->Mana);

 //stack image
 glLoadIdentity();
 glTranslated(
    M_DETAILS_X + 70,
    M_DETAILS_Y + 140,
    -.5);
 glScaled(M_ITEM_W/2, M_ITEM_H/2, .5);
 duTypeStack(u, city->Owner->Color);


 // stats & specials
 guiLoadCoord();
 glLoadIdentity();
 glTranslated(M_DETAILS_X, M_DETAILS_Y, 0);
 dunitDrawUType(u, city->Owner);

 //description
}
*/





static void mpDetailsAdd(boxSC* p)
{
 boxC* main;
 main = p->AddWindow(0,0, DETAILS_X, DETAILS_Y, DETAILS_W, DETAILS_H, 0,0);
 main->SelectF = mpDetailsSelect;

 // blds
 mpDetailsBld = &main->DefaultS;
 mpDetailsBld->InputF = mpDetailsBldInput;
 mpDetailsBld->DisplayF = mpDetailsBldDisplay;
 mpDetailsBld->Name = "Building details";
 mpDetailsBld->Help = "";

 // units
 //
 boxC* duNewStatsAndSpecials();
 //
 mpDetailsU = new unitC;
 mpDetailsUnit = new boxSC(*main, 0);
 mpDetailsUnit->InputF = mpDetailsUnitInput;
 mpDetailsUnit->DisplayF = mpDetailsUnitDisplay;
 mpDetailsUnit->AddBox(*duNewStatsAndSpecials());
 mpDetailsUnit->Name = "Troop unit details";
 mpDetailsUnit->Help = "";

 // specials
 mpDetailsSpc = new boxSC(*main, 0);
 mpDetailsSpc->InputF = mpDetailsSpcInput;
 mpDetailsSpc->DisplayF = mpDetailsSpcDisplay;
 mpDetailsSpc->Name = "Special details";
 mpDetailsSpc->Help = "";
}





/*=============================================================================
 * MAIN FUNCTIONS
 */
static void btfExit()
{
 bOpen("city");
}





/*=============================================================================
 * GENERICS
 */
static void MENUdraw(boxC& b)
{
 mpSQCheck();
}





static bool MENUinput(boxC& b)
{
 switch(inputK()) {
    case 27:
    case '\n': bOpen("city"); break;
//    case 'a': pf("avl: %x\n", City->AvlItems(sq, mpType)); break;

    default: break;
 }

 return false;
}





static char* MENUcheck()
{
 if(!gameIsAvailable()) return "no game";

 cellC* cell = self->LocalFocus;
 if(!cell) return "no cell selected";

 City = cell->City;
 if(!City) return "no city here";

 if(City->Owner != self) return "not city owner";

 return 0;
}





static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 mpSQueue = 0;

 return 0;
}









/*=============================================================================
 * CITY MENU
 */
static spellTypeC* prspells[] = {
    &spellTypeCity,
    0
};


static bmenuDECLARE( prod ) {
    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "land";
    ProvidedSpellTargets = (void**)prspells;

    boxC* b;
    float x, s;


    // avl items
    mpAvlAdd(this);



    // item details
    mpDetailsAdd(this);






    // queue
    mpQAdd(this);




    // done button
    AddButton("Done", "", DONE_X, DONE_Y, DONE_W, DONE_H, '\n', btfExit);

    // alchemy
    AddWindow("Alchemy", "",
     ALCHEM_X,
      ALCHEM_Y,
       ALCHEM_W,
        ALCHEM_H,
	 0, 0);



} bmenuINSTANCE;



//EOF =========================================================================
