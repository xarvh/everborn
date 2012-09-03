/*
 BPOPUP.CPP
 box menu popups.

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
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "main.h"
#include "data.h"
#include "sound.h"
#include "fnt.h"

#include "box.h"
#include "boxgui.h"

#include "counters.h"
#include "colors.h"
#include "race.h"
#include "game.h"
#include "random.h"
#include "unit.h"
#include "city.h"
#include "utype.h"
#include "cmd.h"
#include "spell.h"





/*=============================================================================
 *
    popupHelpBox	( titolo, testo, boxC* highlight )
    popupHelp		( titolo, testo, gfx )

    popupareyousure	(testo, comando, gfx?)

 */
#define BP_MARGIN	.01
#define BP_GFX_SIZE	.2
#define BP_TITLE_H	.1
#define BP_TEXT_H	.04



#define POPUP_MAX_ANI	15
#define POPUP_MIN_ANI	 5

class bpC : public boxC { public:
    signed	Ani;
    bool	Open;
    bool	HasGfx;

    double	Wm;
    double	Hm;

    //
    boxSC*	Select();
    void	ScaleAndDrawBase();
    void	DoOpen(double h);

    //
    void	PushMatrixHeaderGfx();
    void	PushMatrixHeaderTitle();
    void	PushMatrixMainContent();

    //
    bpC(void (*draw)(bpC& b), void (*init)(bpC& b));
};



/*=============================================================================
 * MAIN POPUP BOX
 */
boxC bPopup;

static bool bPopupInput(boxC& b)
{
/* if(mouseL() || mouseR() || mouseM())
    for(boxLC* l = bPopup.DefaultS.SubBoxesHook; l; l = l->Next)
	((bpC&)l->SubBox).Open = false;
*/
 return false;
}



/*=============================================================================
 * POPUP CLASS
 */
boxSC* bpC::Select()
{
 if(!Open)
    if(Ani > POPUP_MIN_ANI) Ani -= 1;
    else return 0;

 if(Open)
    if(Ani < POPUP_MAX_ANI) Ani += 1;

 if(Ani > POPUP_MAX_ANI)
    Ani = POPUP_MAX_ANI;


 return &DefaultS;
}

static boxSC* bpSelectF(bpC& b)
{
 return b.Select();
}





static bool bpInputF(bpC& b)
{
 if(inputK() == 27)
 {
    if(!b.Open) return false;	// closing already, don't eat the input
    b.Open = false;
    return true;
 }

 if(mouseL() || mouseR() || mouseM())
 {
    b.Open = false;
    return b.Mouse; // don't eat input if clicking outside
 }

 return false;
}





bpC::bpC(void (*draw)(bpC&), void (*init)(bpC&))
{
 SelectF = (boxSelectF)bpSelectF;
 DefaultS.InputF = (boxInputF)bpInputF;
 DefaultS.DisplayF = (boxDisplayF)draw;

 Ani = POPUP_MIN_ANI;
 Wm = .5;
 Wg = Hm = Hg = .01;
 H = .9;
 DefaultL.Xg = .5;
 DefaultL.Yg = .5*H;

 bPopup.DefaultS.AddBox(*this);
 bPopup.DefaultS.InputF = bPopupInput;

 if(init) init(*this);
}





/*=============================================================================
 *
 */
void bpC::DoOpen(double h)
{
 bClick(*this);
 if(!h) h = BMENU_H;
 Hm = Wm*h;
 Open = true;
}



void bpC::ScaleAndDrawBase()
{
 HasGfx = false;

 // update geometry (for next draw, won't impact current draw)
 Wg = Ani * Wm / POPUP_MAX_ANI;
 Hg = Ani * Hm / POPUP_MAX_ANI;
 DefaultL.Xg = (1 - Wg)/2;
 DefaultL.Yg = (BMENU_RATIO - Hg) / 2;

 // draw base
 bGColor(.5);
 glBegin(GL_QUADS);
    glVertex2f(0,H);
    glVertex2f(1,H);
    glVertex2f(1,0);
    glVertex2f(0,0);
 glEnd();
 bdrawWindowBackground(*this, true, 1);

 // draw contour
 bdrawContour(*this, true, true);
}





/*=============================================================================
 * HELPERS
 */
void bpC::PushMatrixHeaderGfx()
{
 HasGfx = true;

 glPushMatrix();
    glTranslated(BP_MARGIN, H-BP_MARGIN,0);
    glScaled(BP_GFX_SIZE, BP_GFX_SIZE, .5);
    glTranslated(.5, -.5, 0);
}



void bpC::PushMatrixHeaderTitle()
{
 float skip = HasGfx? BP_GFX_SIZE : 0;

 glPushMatrix();
    glTranslated(BP_MARGIN+skip, H - BP_MARGIN - BP_TITLE_H,0);
    bHColor();
    fntS(0, BP_TITLE_H, 1- 2*BP_MARGIN- skip);
}



void bpC::PushMatrixMainContent()
{
 glPushMatrix();
    glTranslated(BP_MARGIN, H-.25, 0);
}





/*=============================================================================
 * HELP
 */
namespace popupHelpS {

 char* Name;
 char* Help;

 void Draw(bpC& b)
 {
    b.ScaleAndDrawBase();

    glPushMatrix();
        bHColor();
        fntS(0, BP_TITLE_H, .9);
        glTranslated(.01, b.H-BP_TITLE_H, 0);
        fntL("%s", Name);

	bTColor();
        fntS(0, BP_TEXT_H);
	fntT(.9, "\n%s", Help);
    glPopMatrix();
 }


 bpC Popup(Draw, 0);


 void Open(char* n, char* h)
 {
    float H = BP_TITLE_H + BP_TEXT_H;

    // write text outside the screen to gauge text height
    glLoadIdentity();
    glTranslated(-2,0,0);
    fntS(BP_TEXT_H);
    H += fntT(1, "%s", h);

    // popup with estimated height
    Popup.DoOpen(H);
    Name = n;
    Help = h;
 }
}

bool boxHelp(char* n, char* h)
{
 if(!mouseR()) return false;
 popupHelpS::Open(n, h);
 return true;
}





/*=============================================================================
 * AREYOUSURE
 */
namespace popupYesNoS
{
 char Bf[1024];
 char* Cmd;
 char* Msg;



 void Draw(bpC& b)
 {
    b.ScaleAndDrawBase();

    glPushMatrix();
	glTranslated(.01, b.H, 0);

	bTColor();
    	fntS(0, BP_TEXT_H);
	fntT(.9, "\n%s", Msg);
    glPopMatrix();
 }


 void Init(bpC& p);
 bpC Popup(Draw, Init);



 void BtfNo() { Popup.Open = false; }
 void BtfYes() { cmd(Cmd); Popup.Open = false; }
 void Init(bpC& p)
 {
    p.DefaultS.AddButton("Yes", "Which part of 'Yes' didn't you understand? =P",
	.5-BDEF_BORDER-BDEF_BUTTON_W, BDEF_BORDER,
	BDEF_BUTTON_W, BDEF_BUTTON_H,
	'y', BtfYes);

    p.DefaultS.AddButton("No", 0,
	.5+BDEF_BORDER, BDEF_BORDER,
	BDEF_BUTTON_W, BDEF_BUTTON_H,
	'n', BtfNo);
 }




 void Open(const char* fmt, ...)
 {
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(Bf, sizeof(Bf)-1, fmt, arg);
    va_end(arg);

    Cmd = Bf;
    for(Msg = Bf; *Msg != '|'; Msg++);
    *Msg++ = '\0';

    // write text outside the screen to gauge text height
    glLoadIdentity();
    glTranslated(-2,0,0);
    fntS(BP_TEXT_H);
    float h = BDEF_BUTTON_H + BP_TEXT_H + fntT(1, "%s", Msg);

    // popup with estimated height
    Popup.DoOpen(h);
 }
}





/*=============================================================================
 * SETTLE
 */
namespace popupSettleS
{
 char	Name[CITY_NAME_BF];
 char*	Cursor;
 raceC*	Race;
 randomC Rnd;



 void Draw(bpC& b)
 {
    glPushMatrix();
	b.ScaleAndDrawBase();

	glTranslated(.01, b.H, 0);

	bTColor();
    	fntS(0, BP_TEXT_H);
	fntT(.9, "\nChoose a name for the new %s city.", Race->Name);
    glPopMatrix();
 }

 void Init(bpC& p);
 bpC Popup(Draw, Init);



 void BtfCancel() { Popup.Open = false; }
 void BtfOk() { cmd("settle %s", Name); Popup.Open = false; }



 bool NameInput(boxC& b)
 {
    switch(int k = inputK()) {
	case '\b': if(Cursor > Name) *--Cursor = '\0'; return true;
	case 27: BtfCancel(); return true;

	case 13:
	    if(Cursor > Name) BtfOk();
	    return true;

    default:
	if(k < ' ' || k > '~') return false;
	if(Cursor - Name >= CITY_NAME_BF-1) return true;
	*Cursor++ = k;
	*Cursor = '\0';
	return true;
    }

    if(mouseL())
    {
	BtfCancel();
	return true;
    }

    return false;
 }



 void NameDisplay(boxC& b)
 {
    bdrawContour(b, true, false);
    glTranslated(.02, 0, 0);
    bHColor();
    fntS(0, 1.3*BP_TITLE_H, .96);
    fntL("%s%s", Name, (timeGet()/250)&1?"_":" " );
 }



 void Init(bpC& p)
 {
    p.DefaultS.AddWindow("City Name",
	"This is a randomly generated name, taken from the Race's Dictionary."
	"\nPress again the Settle button to generate a new name."
	"\nYou can change a generated name or enter a completely new name."
	"\nUse escape sequences (starting with '?') at your risk.\n",
	.1, BDEF_BUTTON_H*1.5, .8, BP_TITLE_H,
	NameInput, NameDisplay);

    p.DefaultS.AddButton("Ok",
	"Be careful: once you choose a city name, you cannot change it.",
	.5-BDEF_BORDER-BDEF_BUTTON_W, BDEF_BORDER,
	BDEF_BUTTON_W, BDEF_BUTTON_H,
	0, BtfOk);

    p.DefaultS.AddButton("Cancel",
	"Well, pressing ESC would have the same effect....",
	.5+BDEF_BORDER, BDEF_BORDER,
	BDEF_BUTTON_W, BDEF_BUTTON_H,
	0, BtfCancel);
 }



 double RandomF() { return Rnd(); }

 int Open()
 {
    char* fe = "popupSettleS::Open: ";

    // find race
    if(!self->Focus) return pe("%sno focus.\n", fe);

    unitC* u;
    for(u = self->Focus->Unit;
    u && !(u->Team == self->SelectedTeam && (u->Type->Cmd & CMD_OUTPOST));
        u = u->Next);
    if(!u) return pe("%sno settlers selected.\n", fe);
    Race = u->Type->Race;

    // check
    if(char* e = cityExSettlerCheck(u, "-")) return pe("%s.\n", e);


    // generate a name
    Rnd.Seed = timeGet()/100;
    char* n = Race->Language.Generate("CITY", RandomF);
    strncpy(Name, n, sizeof(Name));
    Cursor = strchr(Name, '\0');

    // popup with estimated height
    Popup.DoOpen(BP_TEXT_H*2 + BP_TITLE_H + BDEF_BUTTON_H*2);
 }
}





/*=============================================================================
 * UNIT
 */
namespace popupUnitS
{
    unitC* Unit;

    void Draw(bpC& b, unitC* u)
    {

	b.PushMatrixHeaderGfx();
	    glTranslated(0,-.35,0);
	    glScaled(.7,.7,.7);
	    glRotated(-15, 1, 0, 0);
	    glRotated(135, 0, 1, 0);
	    duStack(u);
	glPopMatrix();

	b.PushMatrixHeaderTitle();
	    u->Color()->Select();
	    fntL("%s", u->Name());
	    bTColor();
	    fntS(0, BP_TEXT_H, 0);
	    if(!u->PlayerMage())
	    {
	        char* rank = u->Hero? u->Rank()->NameHero : u->Rank()->NameUnit;
		glTranslated(0, -BP_TEXT_H*.8, 0);
		fntL("%s (Experience %d)", rank, u->Exp);
		glTranslated(0, -.03, 0);
		glScaled(.05, .05, 1);
		cntDrawUpkeep(u->FoodUpkeep(), u->GoldUpkeep(), u->ManaUpkeep());
	    }
	glPopMatrix();

	b.PushMatrixMainContent();
	    duStats(u);
	glPopMatrix();
    }



    void Draw(bpC& b)
    {
	b.ScaleAndDrawBase();
	Draw(b, Unit);
    }


    bpC Popup(Draw, 0);


    void Open(unitC* u)
    {

	// popup with estimated height
	Popup.DoOpen(.8);
	Unit = u;
    }


    bpC NoPop(0, 0);
    void Draw(unitC* u, double h)
    {
	NoPop.Wg = 1;
	NoPop.Hg = NoPop.H = h;
	Draw(NoPop, u);
    }
}





/*=============================================================================
 * BUILD ROAD
 */
namespace popupBuildRoadS
{
 int	EngineersCnt;
 cellC* Start;
 cellC* End;
 cellC** MouseEnd;



 void DoPath(cellC* s, cellC* e)
 {
 }



 void Draw(bpC& b)
 {
    if(*MouseEnd && *MouseEnd != End)
	DoPath(Start, End = *MouseEnd);

    // draw base
    bGColor(.5);
    glBegin(GL_QUADS);
	glVertex2f(0,b.H);
	glVertex2f(1,b.H);
	glVertex2f(1,0);
	glVertex2f(0,0);
    glEnd();
    bdrawWindowBackground(b, true, 1);
    bdrawContour(b, true, true);


    glPushMatrix();
	glTranslated(.01, b.H, 0);

	bTColor();
    	fntS(0, BP_TEXT_H);
	fntT(.9,
	    "\nThe road will be completed in %d turns."
	    "\nOnce completed, normal troops will take %d turns to"
	    " travel it all along."
	    "\nLeft click to confirm.", -1, -1);
    glPopMatrix();
 }

 void Init(bpC& p);
 bpC Popup(Draw, Init);

 void BtfCancel() { Popup.Open = false; Popup.Ani = POPUP_MIN_ANI;}

 void Init(bpC& p)
 {
    p.DefaultS.AddButton("Cancel",
	"Well, pressing ESC would have the same effect....",
	.5-BDEF_BUTTON_W/2, BDEF_BORDER,
	BDEF_BUTTON_W, BDEF_BUTTON_H,
	0, BtfCancel);
 }


 int Open(float x, float w, cellC** targ)
 {
    char* fe = "popupBuildRoadS::Open: ";

    if(!self->Focus) return pe("%sno focus.\n", fe);

    EngineersCnt = 0;
    for(unitC* u = self->Focus->Unit; u; u = u->Next)
	if(u->Team == self->SelectedTeam && (u->Type->Cmd & CMD_OUTPOST)) //@@CMD_BUILDROAD
	    EngineersCnt++;

    if(EngineersCnt == 0) return pe("%sno engineers selected.\n", fe);

    // set other vars
    Start = self->Focus;
    MouseEnd = targ;


    // popup with estimated height
    Popup.DefaultL.Xg = x;
    Popup.Wg = w;
    Popup.Hg = w*(BP_TEXT_H*4 + BDEF_BUTTON_H + BDEF_BORDER*1.5);
    Popup.DefaultL.Yg = BMENU_H - Popup.Hg - BDEF_BORDER/3;
    Popup.DoOpen(0);
 }
}





/*=============================================================================
 * RESEARCH
 */
namespace popupResearchS
{
 float H = 1.9*BMENU_H;

 long HSpell;
 long SSpell;

 void Draw(bpC& b)
 {
    b.ScaleAndDrawBase();
    glPushMatrix();
	glTranslated(.01, b.H, 0);
	fntS(0, BP_TITLE_H, .98);
	bHColor();
	fntL("\nSelect a spell to research:");
    glPopMatrix();
 }


 void Init(bpC& p);
 bpC Popup(Draw, Init);


 spellC* SpellExId(int id)
 {
    int x = id;
    for(spellC* s = spellsHook; s; s = s->SpellNext)
	if(self->CanResearch(s))
	    if(id-- == 0) return s;
    return 0;
 }

 bool SpellInput(boxC& b)
 {
    if(!gameIsAvailable()) return false;

    if(!b.Mouse || !mouseL()) return false;

    spellC* s = SpellExId(b.Id);
    if(!s) return true;

    cmd("research %s", s->Cmd.Name);
    bClick(b);
    Popup.Open = false;
    return true;
 }

 void SpellDisplay(boxC& b)
 {
    if(!gameIsAvailable()) return;

    spellC* s = SpellExId(b.Id);
    if(!s) return;
    if(s == self->ResearchedSpell) SSpell = b.Id;

    bTColor();
    fntS(0, b.H, .98);

    int r = s->ResearchLeft[self->Id];
    int d = self->DeltaKnow();
    if(d)	fntL("%4d?K1 (%3dt) %s", r, (r+d-1)/d, s->Name);
    else	fntL("%4d?K1 %s", r, s->Name);
 }



 void Init(bpC& p)
 {
    p.DefaultS.AddList(1, 20, &SSpell, &HSpell,
    "Spells available for research",
    "",
    0, 0, 1, H - BP_TITLE_H,
    SpellInput, SpellDisplay);
 }


 void Open()
 {
    // popup with estimated height
    Popup.DoOpen(H);
    SSpell = -1;
 }
}





/*=============================================================================
 * PLAYER
 */
namespace popupPlayerS
{
 brainC* Player;



 void Draw(bpC& b)
 {
    b.ScaleAndDrawBase();
    b.PushMatrixHeaderGfx();
	glTranslated(0,-.35,0);
	glScaled(.7,.7,.7);
	glRotated(-15, 1, 0, 0);
	glRotated(135, 0, 1, 0);
	duStack(Player->PlayerMage);
    glPopMatrix();

    b.PushMatrixHeaderTitle();
	Player->Color->Select();
	fntL("%s", Player->Name);
	bTColor();
	fntS(0, BP_TEXT_H, 0);
	glTranslated(0, -BP_TEXT_H*.8, 0);
	fntL("Magic Skill %d", Player->Skill);
//	    if(Player->SpellQueue)
//		fntL("Casting: %s (%d turns left)\n",
//		    Player->SpellQueue->Spell->Name,
//		    Player->SpellQueue->TurnsRequired());
    glPopMatrix();


    bTColor();
    fntS(BP_TEXT_H);

    b.PushMatrixMainContent();
	glTranslated(.1, 0, 0);
	for(pickC* p = picksHook; p; p = p->Next) if(!p->Book && self == p->Owner)
    	    fntL("%s\n", p->Name);
    glPopMatrix();

    b.PushMatrixMainContent();
	glTranslated(.5, 0, 0);
	for(pickC* p = picksHook; p; p = p->Next) if(p->Book && self == p->Owner)
	    fntL("%s\n", p->Name);
    glPopMatrix();
 }



 bpC Popup(Draw, 0);



 void Open(brainC* b)
 {
    Popup.DoOpen(.8);	// popup with estimated height
    Player = b ? b : self;
 }
}





//EOF =========================================================================
