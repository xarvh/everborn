/*
 BOXGUI.CPP	v0.00
 box gui, menu, menu elements

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

#include "main.h"
#include "data.h"
#include "sound.h"
#include "fnt.h"
#include "cmd.h"

#include "box.h"
#include "boxgui.h"
#include "colors.h"





/*=============================================================================
 * GUI COLORS
 */
static float R = 1;
static float G = 0;
static float B = 0;

static float gE(float v) { return v > .5 ? 0 : 1; }

void bGColor(float a) { glColor4f(R,G,B,a); }
void bTColor() { glColor3f(0,gE(G), gE(B)); }
void bHColor() { glColor3f(1,0,0); }





static float bFntHColorF(bool mute, char* s, char** e, float h)
{
 if(*s != 'H') return 0;
 *e = s+1;
 if(mute) return 0;

 bHColor();

 return 0;
}
static fntEscapeC bFntHColor(bFntHColorF);





static float bFntTColorF(bool mute, char* s, char** e, float h)
{
 if(*s != 'T') return 0;
 *e = s+1;
 if(mute) return 0;

 bTColor();

 return 0;
}
static fntEscapeC bFntTColor(bFntTColorF);





static char* setGuiColors(char* gc)
{
 if(*gc != '\0')
 {
    R = (gc[0]-'0') / 9.;
    G = (gc[1]-'0') / 9.;
    B = (gc[2]-'0') / 9.;    
 }
 else    
 {
    int l = timeGet()/100;
    R = .5*(l%3);	l/= 3;
    G = .5*(l%3);	l/= 3;
    B = .5*(l%3);	l/= 3;

    float max = R;
    if(G > max) max = G;
    if(B > max) max = B;
    if(max < .2) switch((timeGet()/100)%3) {
	case 0: R = 1; break;
	case 1: G = 1; break;
	case 2: B = 1; break;
    }
 }

 return gc;
}

static envC envGuiColor("guicolor", "", setGuiColors);





/*=============================================================================
 * BASE SOUND
 */
soundC bSndDown("btdown.ogg");



#include <math.h>

void bClick(boxC& b)
{
 int ox = (360*mouseX()) / screenW() - 180;
 int oy = (360*mouseY()) / screenH() - 180;

 int dist = 0; //(int)sqrt(ox*ox + oy*oy); // use a manhattan distance instead?
 int dir = 0; //360 + ox;

// pf("off:%d %d\n", dist, dir%360);



 bSndDown.Play(250, dist, dir);
}





/*=============================================================================
 * PRIMITIVES DRAWING
 */
void pText(boxC& b)
{
 glBegin(GL_QUADS);
    glTexCoord2d(-.5, +.5); glVertex2f( 0, b.H);
    glTexCoord2d(+.5, +.5); glVertex2f( 1, b.H);
    glTexCoord2d(+.5, -.5); glVertex2f( 1, 0);
    glTexCoord2d(-.5, -.5); glVertex2f( 0, 0);
 glEnd();
}




void bdrawWindowBackground(boxC& b, bool hl, double alpha)
{
 extern dataC mdData;

 int period = 700;
 int t = timeGet()/100;
 if(hl) t += 1;
 float f = (t%period) / (double)period;

 glEnable(GL_BLEND);
 glColor4f(.5+R/2, .5+G/2, .5+B/2, alpha);


 // upwards
 mdData.Select(14, 1, b.H);
 glMatrixMode(GL_TEXTURE);
    glTranslated(0, f*8, 0);
 glMatrixMode(GL_MODELVIEW);
 pText(b);

 // downwards
 mdData.Select(15, 1, b.H);
 glMatrixMode(GL_TEXTURE);
    glTranslated(0, -f*8, 0);
 glMatrixMode(GL_MODELVIEW);
 pText(b);
}



void bdrawButtonBackground(boxC& b, bool dn)
{
 extern dataC mdData;
 if(dn)	glColor3f(.7,.7,.7);
 else	glColor3f(1,1,1);
 glPushMatrix();
    glTranslated(.5, b.H/2, 0);
    mdData.Rect(33, 1, b.H);
 glPopMatrix();
 glDisable(GL_TEXTURE_2D);
}



void bdrawContour(boxC&b, bool highlight, bool down)
{
 glDisable(GL_TEXTURE_2D);

 // red outer line
 int m = highlight? 2:1;
 screenLineWidth(3.5);
 glBegin(GL_LINE_STRIP);
    glColor4f(.3*m*R,.3*m*G,.3*m*B,.7);	glVertex2d(0, 0);
    glColor4f(.5*m*R,.5*m*G,.5*m*B,.7);	glVertex2d(1, 0);
    glColor4f(.7*m*R,.7*m*G,.7*m*B,.7);	glVertex2d(1, b.H);
					glVertex2d(0, b.H);
    glColor4f(.3*m*R,.3*m*G,.3*m*B,.7);	glVertex2d(0, 0);
 glEnd();


 // we have to move according to actual object display size
 double mt[16];
 glGetDoublev(GL_MODELVIEW_MATRIX, mt);
 double sX = mt[0];
 double sY = mt[5];


 // dark outer line
 float a = down?.7:.5;
 double dx = .002 / sX;
 double dy = .002 / sY;
 screenLineWidth(2.5);
 glBegin(GL_LINE_STRIP);
    glColor4f(0,0,0,a);	glVertex2d(0+dx,   0+dy);
			glVertex2d(1-dx,   0+dy);
			glVertex2d(1-dx, b.H-dy);
			glVertex2d(0+dx, b.H-dy);
			glVertex2d(0+dx,   0+dy);
 glEnd();
}



void bdrawName(boxC& b, char* name, bool highlight, bool down)
{
 int m = highlight? 2:1;

 glPushMatrix();
    float s = down? .95:1;
    glTranslated(.5,0,0);
    fntS(0, b.H*s, 1*s);
	glColor3f(.4,.4,.4);
	glPushMatrix(); fntC("%s", name); glPopMatrix();
    glTranslated(-.007,-.007, 0);
	glColor3f(R*m,G*m,B*m);
        fntC("%s", name);
 glPopMatrix();
}




void bdrawBox(boxC& b, char* text, bool highlight, bool down)
{
 bdrawButtonBackground(b, down);
 bdrawContour(b, highlight, down);
 if(text) bdrawName(b, text, highlight, down);
}





/*=============================================================================
 * BUTTON
 */
void bbC_Display(boxC& b)
{
 bool mp = b.Mouse && mouseLdn();
// bdrawBox(b, b.CurrentS->Name, b.Mouse, mp);
 bdrawBox(b, b.Name, b.Mouse, mp);
}



static bool bbC_Input(bbC& b)
{
 if(
    (b.Mouse && mouseL())
    ||
    (b.HotKey && b.HotKey == inputK())
 ){
    bClick(b);
    b.Call();
    return true;
 }

 return false;
}



bbC::bbC(
 char* name,
 float x,
 float y,
 float w,
 float h,
 int hotkey,
 void (*call)())
{
 Name = name;
 DefaultL.Xg = x;
 DefaultL.Yg = y;
 Wg = w;
 Hg = h;
 H = Hg/Wg;
 HotKey = hotkey;
 Call = call;

 DefaultS.InputF = (boxInputF)bbC_Input;
 DefaultS.DisplayF = (boxDisplayF)bbC_Display;
}





/*=============================================================================
 * TAB
 */
void btabDisplay(btabC& tab)
{
 bool highlight = tab.Mouse;
 bool down = (*tab.Targ != tab.Id) || (tab.Mouse && mouseLdn());
 bdrawBox(tab, tab.Name, highlight, down);
}



bool btabInput(btabC& tab)
{
 if(!tab.Mouse || !mouseL()) return false;
 bClick(tab);
 *tab.Targ = tab.Id;
 return true;
}



btabC::btabC(
	long id,
	long* targ,
	char* name,
	float x, float y,
	float w, float h)
{
 Id = id;
 Targ = targ;
 Name = name;
 DefaultL.Xg = x;
 DefaultL.Yg = y;
 Wg = w;
 Hg = h;

 DefaultS.DisplayF = (boxDisplayF)btabDisplay;
 DefaultS.InputF = (boxInputF)btabInput;
}





/*=============================================================================
 * WINDOW
 */
void bwinDisplay(boxC& b)
{
 bdrawWindowBackground(b, b.Mouse, .45);
 bdrawContour(b, false, false);
}





/*=============================================================================
 * LIST
 */
class blistItemC : public boxC { public:
    blistC& List;

    //
    blistItemC(blistC& list);
};



void bdrawSelect(boxC& b, bool select, bool sv, bool sh)
{
 double mt[16];
 glGetDoublev(GL_MODELVIEW_MATRIX, mt);
 double dx = .002 / mt[0];
 double dy = .002 / mt[5];


 if(select)
 {
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    	glColor4f(.9*R,.9*G,.9*B,.3);	glVertex2d(0+dx,   0+dy);
	glColor4f(R,G,B,.4);		glVertex2d(1-dx,   0+dy);
	glColor4f(.5*R,.5*G,.5*B,.3);	glVertex2d(1-dx, b.H-dy);
	glColor4f(R,G,B,.4);		glVertex2d(0+dx, b.H-dy);
    glEnd();
 }


 if(sh) 
 {
    screenLineWidth(2);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
	glColor4f(R,G,B,.25); glVertex2d(0+dx, 0);
	glColor4f(R,G,B,.05); glVertex2d(1-dx, 0);
    glEnd();
 }


 if(sv)
 {
    screenLineWidth(2);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
	glColor4f(R,G,B,.25); glVertex2d(0, 0);
	glColor4f(R,G,B,.05); glVertex2d(0, b.H);
    glEnd();
 }
}



void blistItemDisplay(blistItemC& b)
{
 if(b.Mouse)
    *b.List.Hover = b.Id;

 if(b.Mouse)
 {
    *b.List.Hover = b.Id;
    bdrawWindowBackground(b, true, true);
 }

 bdrawSelect(b,
    b.List.Select && b.Id == *b.List.Select,
    b.DefaultL.Xg > .001,
    b.DefaultL.Yg > .001);

 b.List.ItemDisplay(b);
}





bool blistItemInput(blistItemC& b)
{
 if(!b.Mouse || !mouseL()) return false;
 bClick(b);
 if(b.List.Select) *b.List.Select = b.Id;
 return true;
}



blistItemC::blistItemC(blistC& list)
:List(list)
{
 DefaultS.DisplayF = (boxDisplayF)blistItemDisplay;
 DefaultS.InputF = (boxInputF)blistItemInput;
}





void blistDisplay(blistC& b)
{
 int k = 0;
 for(boxLC* l = b.DefaultS.SubBoxesHook; l; l = l->Next)
 {
    l->SubBox.Id = b.Off + k; //b.Rows*b.Cols -k -1;
    l->SubBox.Hg = b.H / b.Rows;
    l->SubBox.Wg = 1. / b.Cols;

    l->Xg = (k%b.Cols) * l->SubBox.Hg;
    l->Yg = b.H - (k/b.Cols +1) * l->SubBox.Hg;
    k++;
 }

 if(b.IdSelect)
    b.IdSelect(b);

 bwinDisplay(b);
}



blistC::blistC(int cols, int rows, long* sel, long* hover, boxDisplayF idi)
{
 Off = 0;
 Cols = cols;
 Rows = rows;
 Hover = hover;
 Select = sel;
 ItemDisplay = idi;
 IdSelect = 0;

 DefaultS.DisplayF = (boxDisplayF)blistDisplay;

 for(int i = 0; i < Rows; i++)
    for(int j = 0; j < Cols; j++)
    {
	blistItemC* row = new blistItemC(*this);
	DefaultS.AddBox(*row);
    }
}





/*=============================================================================
 * ADDs
 */
blistC* boxSC::AddList(int cols, int rows,
	long* sel, long* hov,
	char* name, char* help,
	float x, float y,
	float w, float h,
	boxInputF iin,
	boxDisplayF idi)
{
 blistC* list = new blistC(cols, rows, sel, hov, idi);
 list->DefaultL.Xg = x;
 list->DefaultL.Yg = y;
 list->Wg = w;
 list->Hg = h;

 list->DefaultS.Name = name;
 list->DefaultS.Help = help;

 if(iin)
    for(boxLC* l = list->DefaultS.SubBoxesHook; l; l = l->Next)
	l->SubBox.DefaultS.InputF = iin;

 AddBox(*list);
 return list;
}



boxC* boxSC::AddButton(char* name, char* help,
	float x, float y,
	float w, float h,
	int hotkey,
	void (*call)())
{
 boxC* box = new bbC(name, x, y, w, h, hotkey, call);
 box->DefaultS.Name = name;
 box->DefaultS.Help = help;
 AddBox(*box);
 return box;
}



boxC* boxSC::AddTab(
	long id,
	long* targ,
	char* name, char* help,
	float x, float y,
	float w, float h)
{
 boxC* tab = new btabC(id, targ, name, x, y, w, h);
 tab->DefaultS.Name = name;
 tab->DefaultS.Help = help;
 AddBox(*tab);
 return tab;
}



boxC* boxSC::AddWindow(
	char* name, char* help,
	float x, float y,
	float w, float h,
	boxInputF input, boxDisplayF display)
{
 boxC* box = new boxC();
 box->Wg = w;
 box->Hg = h;
 box->DefaultS.InputF = input;
 box->DefaultS.DisplayF = display ? display : bwinDisplay;

 box->DefaultS.Name = name;
 box->DefaultS.Help = help;

 AddBox(*box, x, y);
 return box;
}





/*=============================================================================
 * LOAD COORD
 */
void bLoadCoord()
{
 glViewport(0, 0, screenW(), screenH());
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glTranslated(-1, -1, 0);
 glScaled(2/1., 2/BMENU_RATIO, 1.);
     
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
}





/*=============================================================================
 * GUI MENU
 */
static boxC bMenu;
static boxSC* bMenuAlternateChain = 0;

bmenuC::bmenuC(char* name)
: boxSC(bMenu, &bMenuAlternateChain)
{
 Name = name;
 StartF = 0;
 CheckF = 0;
 Fallback = 0;
 ProvidedSpellTargets = 0;
}



bool bOpen(char* n)
{
 char* fe = "bOpen: ";

 // find the menu
 boxSC* s;
 for(s = bMenu.DefaultS.Next; s && strcasecmp(s->Name, n); s = s->Next);
 if(!s)
 {
    pe("bOpen: menu %s not found.\n", n);
    return true;
 }

 bmenuC* m = (bmenuC*)s;
 if(m->StartF) if(char* e = m->StartF())
 {
    pe("%scannot open menu %s: %s.\n", fe, n, e);
    return true;
 }

 bMenu.CurrentS = s;
 return false;
}



static int cmdFMenu()
{
 bOpen(arg1.s);
}

static cmdC cmdMenu(
    "menu",
    "N: MenuName",
    "open specifyed menu",
    cmdFMenu, 0, 0
);





/*=============================================================================
 * GUI MAIN
 */
static boxC bMain;





void bInit()
{
 static bool init = false;
 if(init) return;
 init = true;
 bMain.Wg = 1;
 bMain.Hg = BMENU_RATIO;




 // spell interface
 boxC& spellGuiInit();
 boxC& bSpell = spellGuiInit();
 bMain.DefaultS.AddBox(bSpell);


 // popups
 extern boxC bPopup;
 bPopup.Wg = 1;
 bPopup.Hg = BMENU_RATIO;
 bMain.DefaultS.AddBox(bPopup);


 // menues
 bMenu.DefaultS.Next = bMenuAlternateChain;
 bMenu.DefaultS.Name = "def";
 bMenu.Wg = 1;
 bMenu.Hg = BMENU_RATIO;
 bMain.DefaultS.AddBox(bMenu);
}



static bool bCheck()
{
 bmenuC* m = (bmenuC*)bMenu.CurrentS;
 if(!m) return false;
 if(!m->CheckF) return false;
 if(!m->CheckF()) return false;

 if(m->Fallback) bOpen(m->Fallback);
 return true;
}



void bInput()
{
 if(bCheck()) return;

 void spellGuiCheckMenu(void**);
 bmenuC* m = (bmenuC*)bMenu.CurrentS;
 spellGuiCheckMenu(m?m->ProvidedSpellTargets:0);

 bMain.Input();
}



void bDisplay()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 if(bCheck()) return;

 bLoadCoord();
 double mx = mouseX() * 1. / screenW();
 double my = (screenH()-mouseY()-1) / (double)screenH() * BMENU_RATIO;
 bMain.Display(0, 0, mx, my);
}



//EOF =========================================================================
