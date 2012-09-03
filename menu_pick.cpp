/*
 MENU_PICK.CPP		v0.10
 Picks Menu.

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
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "boxgui.h"
#include "game.h"
#include "race.h"
#include "fnt.h"
#include "cmd.h"
#include "net.h"
#include "spell.h"
#include "utype.h"

#define M_BORDER		BDEF_BORDER
#define M_HEIGHT		(.8*BDEF_BUTTON_H)
#define M_SHOULDER_W		(1.3*BDEF_BUTTON_W)

#define M_SHOW_RACE_X		(M_BORDER)
#define M_SHOW_RACE_Y		(BMENU_H - M_BORDER - M_SHOW_RACE_H)
#define M_SHOW_RACE_W		M_SHOULDER_W
#define M_SHOW_RACE_H		M_HEIGHT

#define M_SHOW_PICKS_X		M_SHOW_RACE_X
#define M_SHOW_PICKS_Y		(M_SHOW_RACE_Y -2* M_SHOW_PICKS_H)
#define M_SHOW_PICKS_W		M_SHOULDER_W
#define M_SHOW_PICKS_H		M_HEIGHT




#define M_RACE_LIST_X		(M_SHOW_RACE_X + M_SHOW_RACE_W + M_BORDER)
#define M_RACE_LIST_Y		M_BORDER
#define M_RACE_LIST_W		BDEF_BUTTON_W
#define M_RACE_LIST_H		(BMENU_H - 2*M_BORDER)
#define M_RACE_LIST_ITEMS	(3.5*M_RACE_LIST_H / M_RACE_LIST_W)

#define M_RACE_PORT_X		(M_RACE_LIST_X + M_RACE_LIST_W + M_BORDER)
#define M_RACE_PORT_Y		(BMENU_H - M_RACE_PORT_H - M_BORDER)
#define M_RACE_PORT_W		(1 - M_RACE_PORT_X - M_BORDER)
#define M_RACE_PORT_H		.8*BMENU_H //(3 * M_RACE_PORT_W / 4)




#define M_REALM_TAB_X		(M_SHOW_RACE_X + M_SHOW_RACE_W + M_BORDER)
#define M_REALM_TAB_Y		(M_PICK_LIST_Y + M_PICK_LIST_H)
#define M_REALM_TAB_W		(M_PICK_LIST_W/6)
#define M_REALM_TAB_H		M_REALM_TAB_W

#define M_PICK_LIST_X		M_REALM_TAB_X
#define M_PICK_LIST_Y		M_BORDER
#define M_PICK_LIST_W		(1-M_SHOW_RACE_W-BDEF_BUTTON_W -4*M_BORDER)
#define M_PICK_LIST_H		(6*BDEF_BUTTON_H)
#define M_PICK_LIST_ITEMS	(M_PICK_LIST_H / .03)

#define M_PICK_DETAILS_X	M_PICK_LIST_X
#define M_PICK_DETAILS_Y	(M_REALM_TAB_Y+M_REALM_TAB_H+M_BORDER)
#define M_PICK_DETAILS_W	(1-M_SHOW_RACE_W-3*M_BORDER)
#define M_PICK_DETAILS_H	(BMENU_H - M_PICK_DETAILS_Y - M_BORDER)



/*=============================================================================
 * GLOBALS
 */
static envC envFast("fast", false);
static envC envRace("race", "");

static long SRace, HRace;
static long SRealm;
static long HPick;
static pickC* CPick;

static bool ShowRace;

static pickC* SPicks[PICKS_MAX_CHOICE];





/*=============================================================================
 *
 */
void drawPick(pickC* p)
{
 char bf[8];
 char* c = bf;
 for(int i = 0; i < PICKS_MAX_CHOICE; i++)
    if(SPicks[i] == p) *c++ = '+';
 *c = '\0';

 if(p->Book)	glColor3fv(p->Book->Realm->Color);
 else		glColor3f(1,1,.1);
 glTranslated(.02,0,0);
 fntL("%s%s", bf, p->Name);
}





/*=============================================================================
 * SHOW SELECTED PICKS AND RACE
 */
static bool btfShowRace(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;
 if(!ShowRace)	ShowRace = true;
 else		SRace = -1;

 bClick(b);
 return true;
}

static void drawShowRace(boxC& b)
{
 bdrawWindowBackground(b, false, .4);
 bdrawContour(b, b.Mouse, false);

 fntS(0, b.H, .95);
 if(raceC* r = raceGet(SRace))
    fntL(" ?TRace:?H%s", r->Name);
 else
    fntL(" ?TRace:?HRandom!");
}



static bool btfShowPicks(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 if(ShowRace)	ShowRace = false;
 else		SPicks[b.Id] = 0;

 bClick(b);
 return true;
}



static void drawShowPicks(boxC& b)
{
 bdrawWindowBackground(b, false, .4);
 bdrawContour(b, b.Mouse, false);

 int i = b.Id;

 if(b.Mouse && SPicks[i])
    CPick = SPicks[i];

 fntS(0, b.H, .95);

 if(i != 6)
    if(SPicks[i])	{ drawPick(SPicks[i]);} //glColor3d(1., 1., 1.); fntL(" %s\n", SPicks[i]->Name); }
    else		{ glColor3d(.5, .5, .5); fntL(" random pick\n"); }
 else
    {
	glColor3d(1, 0, 0);
	fntL(" random bonus!");
    }
}



static boxSC* selectShowPicks(boxC& b)
{
 int cnt = 0;
 for(int i = 0; i < PICKS_MAX_CHOICE; i++)
    if(SPicks[i]) cnt++;

 if(cnt > 1) return 0;
 return &b.DefaultS;
}








 





/*=============================================================================
 * PICKS
 */
static bool pickIsRealm(pickC* p, int r)
{
 if(p->Book) return p->Book->Realm->Code == r;	// book
 if(p->ReqMask & (1<<r)) return true;		// realm-specific
 return !p->ReqMask && r == 0;			// neutral pick
}

static pickC* pickId(int i)
{
 for(pickC* p = picksHook; p; p = p->Next)
    if(pickIsRealm(p, SRealm))
	if(i-- == 0) return p;
 return 0;
}



static void drawPickDetails(boxC& b)
{
 pickC* p = CPick;
 if(!p) return;
 bwinDisplay(b);

 float fh = .07;
 glTranslated(.01, b.H-fh, 0);

 bHColor();
 fntS(0, fh, 1);
 fntL("%s", p->Name);

 bTColor();
 if(bookC* b = p->Book)
 {
    fntS(0, fh/2.5, 1);
    fntL("\n%s %s spellbook:\n", p->Book->Realm->Name, p->Book->Maior?"Major":"Minor");
    for(spellC* s = b->SpellsHook; s; s = s->BookNext)
        fntL(" (%d): %s (%s)\n", s->Level, s->Name, s->Description);
 }
 else
 {
    fntS(0, fh/2, 1);
    fntL("\n%s", p->Desc);
 }

}



static void listDiPick(boxC& b)
{
 pickC* p = pickId(b.Id);
 if(!p) return;

 if(b.Mouse) CPick = p;

 fntS(0, b.H, .95);
 drawPick(p);
}



static bool btfPick(boxC& b)
{
 if(!b.Mouse || !mouseL()) return false;

 pickC* p = pickId(b.Id);
 if(!p) return false;

 //
 bClick(b);

 int f;
 for(f = 0; f < PICKS_MAX_CHOICE && SPicks[f]; f++);
 if(f == PICKS_MAX_CHOICE)
 {
    //@@ add error sound
    pf("All picks made!\n");
    return true;
 }

 SPicks[f] = p;
 return true;
}



/*=============================================================================
 * RACE
 */
static void listAniU(fleshC* flesh)
{
 glPushMatrix();
    glRotated(190, 0, 1, 0);
    fleshMultiAnimate(flesh, self->Color);
 glPopMatrix();
}



static void listDiRace(boxC& b)
{
 raceC* r = raceGet(b.Id);
 if(!r) return;

 glPushMatrix();
    glScaled(.8*b.H, .8*b.H, 1);

    glTranslated(.5,0,.1); listAniU(&r->GetPlayerMage()->Flesh);
    glTranslated(.8,0,.0); listAniU(&r->GetSpearmen()->Flesh);
    glTranslated(.8,0,.0); listAniU(&r->GetSettlers()->Flesh);
    glTranslated(.8,0,.0); listAniU(&r->Worker);
    glTranslated(.8,0,.0); listAniU(&r->Farmer);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
 glPopMatrix();
}



static void drawRaceDetails(boxC& b)
{
 bwinDisplay(b);
 raceC* r = raceGet(HRace);
 if(!r) r = raceGet(SRace);
 if(!r) return;

 glTranslated(.02, b.H, 0);

 fntS(0, .1, .95);
 bHColor();
 fntL("\n%s", r->Name);

 fntS(0, .06);
 bTColor();
 fntT(.95, "\n\n%s", r->Info);
}





/*=============================================================================
 *
 */
static void btf_reset()
{
 SRace = -1;
 for(int i = 0; i < PICKS_MAX_CHOICE; i++) SPicks[i] = 0;
}



static void btf_ok()
{
 char bf[200];
 char* w = bf;
 *w = '\0';
 if(raceGet(SRace)) w += sprintf(w, " %s", raceGet(SRace)->Id);

 for(int p = 0; p < PICKS_MAX_CHOICE; p++) if(SPicks[p])
    w += sprintf(w, " %s", SPicks[p]->Id);

 cmd("pick%s", bf);
}





boxSC* selectRaceOrPicks(boxC& b)
{
 if(ShowRace) return &b.DefaultS;
 return b.DefaultS.Next;
}





/*=============================================================================
 * DEFAULT RACE
 */
#include <time.h>

static void mpDefaultRace()
{
 // default race selected?
 int r;
 for(r = 0; raceGet(r) && strcasecmp(raceGet(r)->Id, envGet("race")); r++);
 if(raceGet(r))
 {
    envSet("race", "");
    SRace = r;
 }


 if(envIsTrue("fast") && !*envGet("race"))
 {
    if(SRace == -1)
	SRace = time(0) % racesCount();
    if(!raceGet(SRace)) // can't select it until we know the name!
	return;

    btf_ok();
    envSet("fast", "no");
 }

 if(envIsTrue("fast")) envSet("fast", "no");
}





/*=============================================================================
 * MENU
 */
static void MENUdraw(boxC& b)
{
 // used by race list
 fleshMultiStart();

 //
 mpDefaultRace();
}





static bool MENUinput(boxC& b)
{
 switch(inputK()) {
    case 27: netReset(); bOpen("main"); break;
    default: break;
 }

 return false;
}




static char* MENUcheck()
{
 if(!gameIsAvailable()) "no game available";
 return 0;
}





static char* MENUstart()
{
 if(char* e = MENUcheck()) return e;

 // set gvars:
 HRace = -1;
 SRealm = 0;
 CPick = 0;
 ShowRace = true;
 btf_reset();

 return 0;
}








static bmenuDECLARE( pick ) {
    StartF = MENUstart;
    CheckF = MENUcheck;
    InputF = MENUinput;
    DisplayF = MENUdraw;
    Fallback = "main";


    boxC* win = AddWindow("Selected Race",
	"This is the race you have selected.\nDuring the game"
	" you'll be able to conquer cities of other races, but this is the race"
	" of your main character and of your starting troops.\n\n"
	"The people of each race has different traits and capabilities,"
	" that will greatly affect your strategy.\n\n",
    	M_SHOW_RACE_X, M_SHOW_RACE_Y,M_SHOW_RACE_W,M_SHOW_RACE_H,
	btfShowRace, drawShowRace);

    for(int i = 0; i < 7; i++)
    {
	float y = M_SHOW_PICKS_Y - i*1.1*M_SHOW_PICKS_H;
	boxC* b = AddWindow("Selected Picks",
	"Before the game starts, you select the books and special traits"
	" ('picks') you"
	" want: this is the list of the items you have choosen so far.\n\n"
	"If you choose nothing, you will be assigned something randomly.\n\n"
	"Each pick is unique and only one player can have it: if two players"
	" choose the same pick"
	" chances are that neither will get it.\n"
	"So, if you really really want something, you can choose it twice or"
	" more: the player that selects a pick more times gets it.\n\n"
	"If you choose only one or zero picks, you will also be awarded a seventh"
	" bonus pick.",
	    M_SHOW_PICKS_X, y, M_SHOW_PICKS_W, M_SHOW_PICKS_H,
	    btfShowPicks, drawShowPicks);

	b->Id = i;
	if(i == 6) b->SelectF = selectShowPicks;
    }


    // done button
    //@@ if(self->Race) display already pressed 
    AddButton("Done!",
	"This is to confirm your selections and start the game.\n"
	"Once pressed you shall wait for the other players to complete"
	" their choices.",
	1-BDEF_BUTTON_W-M_BORDER, M_BORDER, BDEF_BUTTON_W, BDEF_BUTTON_H,
	0, btf_ok);

    // done button
    AddButton("Reset",
	"This clears all your choices at once, if you feel lazy.\n",
	M_BORDER, M_BORDER, BDEF_BUTTON_W, BDEF_BUTTON_H,
	0, btf_reset);





    // races or picks details
    boxC* alt = new boxC;
    alt->Hg = Box.Hg;
    AddBox(*alt);

    boxSC* race = &alt->DefaultS;
    boxSC* pick = new boxSC(*alt, 0);
    alt->SelectF = selectRaceOrPicks;


    // race:
    race->AddList(1, (int)M_RACE_LIST_ITEMS, &SRace, &HRace,
	0, 0,
	M_RACE_LIST_X, M_RACE_LIST_Y,
	M_RACE_LIST_W, M_RACE_LIST_H,
	0, listDiRace);

    boxC* ww = race->AddWindow(0, 0,
	M_RACE_PORT_X, M_RACE_PORT_Y,
	M_RACE_PORT_W, M_RACE_PORT_H,
	0, drawRaceDetails);



    // picks:
    boxC* list = pick->AddList(1, (int)M_PICK_LIST_ITEMS, 0, &HPick,
	"Selection List",
    	"This is the list of available picks.\n"
	"You cannot select more than 6.",
	M_PICK_LIST_X, M_PICK_LIST_Y,
	M_PICK_LIST_W, M_PICK_LIST_H,
	btfPick, listDiPick);

    boxC* w = pick->AddWindow(0, 0,
	M_PICK_DETAILS_X, M_PICK_DETAILS_Y,
	M_PICK_DETAILS_W, M_PICK_DETAILS_H,
	0, drawPickDetails);

    float s = M_REALM_TAB_W; // -0.001;
    float x = M_REALM_TAB_X -s;
    pick->AddTab(REALM_LIFE, &SRealm,
	"Life",
	"The choice for religious zealots and warmongers of sorts.\n"
	"Recommended for lovers of the biggest gun.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "L";
    pick->AddTab(REALM_DEATH, &SRealm,
	"Death",
	"This is usually for goths and in general will appeal people with"
	" questionable fashion sense.\n"
	"Also, choose this if your skin can't stand sunlight.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "D";
    pick->AddTab(REALM_CHAOS, &SRealm,
	"Chaos",
	"Choose this stuff if you are inclined to nuke your opponents,"
	" if you like fractals"
	" or if you think you are a mentally disturbed genius.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "C";
    pick->AddTab(REALM_NATURE, &SRealm,
	"Nature",
	"New Age Flower-Power stuff.\n"
	"Only, it bites.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "N";
    pick->AddTab(REALM_SORCERY, &SRealm,
	"Sorcery",
	"A long time favourite of strategy freaks and UNIX Sys Admis,"
	" and in general of people able to elude enemies and sex with"
	" equal ease.\n"
	"Effective but hard to master.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "S";
    pick->AddTab(REALM_NEUTRAL, &SRealm,
	"Misc",
	"Special stuff."
	"\nSome of them just pump up your numbers, some"
	" of them will allow you to do strange things too.",
	x+=s, M_REALM_TAB_Y, M_REALM_TAB_W, M_REALM_TAB_H)->Name = "!!";



} bmenuINSTANCE;



//EOF =========================================================================
