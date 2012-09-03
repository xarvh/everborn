/*
 FNT.CPP		v0.30
 Font loadup and rendering.

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


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include <GL/gl.h>
#include "counters.h"
#include "data.h"
#include "pngio.h"
#include "fnt.h"





/*=============================================================================
 * HEADER
 */
#define FNT_FIRST	'!'
#define FNT_LAST	('~'+2)		//@@ WHY +2 instead of +1?????
#define FNT_CNT		(FNT_LAST - FNT_FIRST + 1)


class chC { public:
    double tx0;
    double txF;
    double ty0;
    double tyF;
    double w;
};


class fontC { public:
    fontC*	Next;

    char*	Name;
    chC		Ch[FNT_CNT];
    chC*	Last;
    double	Space;

    dataC	Text;

    //
    fontC(char* name, char* fn, char* pattern);
};





/*=============================================================================
 * LOCALS
 */
static fontC* fontHook = NULL;
static fontC* fontS = NULL;	// currently selected font
static float fontH = .1;
static float fontMaxW = .0;

static fontC fontLarge("", "fntLA.png", "");
static fontC fontGreek("El", "fntEL.png", "");

static float fontColor[4];	// used by fntSaveColor()





/*=============================================================================
 * SELECT
 * Selects font and font height used by subsequent fnt*() calls.
 */
void fntS(char* fname, float h, float maxw)
{
 fontS = fontHook;
 if(fname)
    while(fontS && strcmp(fontS->Name, fname)) fontS = fontS->Next;

 fontH = h;
 fontMaxW = maxw;
}



void fntS(char* fname, float h)
{
 fntS(fname, h, 0);
}

void fntS(float h)
{
 fntS(0, h, 0);
}





/*=============================================================================
 * CHAR
 */
static float fntChar(bool mute, char c)
{
 float w;

 if(c <= ' ')
 {
    w = fontS->Space * fontH;
    if(mute) return w;
 }
 else
    {
	// check character
	chC* ch = fontS->Ch + c - FNT_FIRST;
	if(ch > fontS->Last) ch = fontS->Last;

	w = ch->w * fontH;
	if(mute) return w;

	// prepare GL environment
	fontS->Text.BindTexture();
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	// draw character
	glBegin(GL_QUADS);
	    glTexCoord2d(ch->tx0, ch->ty0);	glVertex2d(0,  fontH);
	    glTexCoord2d(ch->txF, ch->ty0);	glVertex2d(w,  fontH);
	    glTexCoord2d(ch->txF, ch->tyF);	glVertex2d(w,  0);
	    glTexCoord2d(ch->tx0, ch->tyF);	glVertex2d(0,  0);
	glEnd();
    }

 glTranslated(w, .0, .0);
 return w;
}





/*=============================================================================
 * ESCAPE + basic escapes
 */
static fntEscapeC* fntEscapeHook = 0;
fntEscapeC::fntEscapeC(fntEscapeF fe)
{
 Next = fntEscapeHook;
 fntEscapeHook = this;
 EscapeF = fe;
}





static float fntEscape(bool mute, char* s, char** e)
{
 *e = 0;

 for(fntEscapeC* esc = fntEscapeHook; esc; esc = esc->Next)
 {
    float w = esc->EscapeF(mute, s, e, fontH);
    if(*e) return w;
 }

 // a real '?', not an escape!
 *e = s;
 return fntChar(mute, '?');
}





static void fntSaveColor()
{
 glGetFloatv(GL_CURRENT_COLOR, fontColor);
}

void fntRestoreColor()
{
 glColor4fv(fontColor);
}




static float fntERestoreF(bool mute, char* s, char** e, float h)
{
 if(*s != '/') return 0;
 *e = s+1;
 if(mute) return 0;

 fntRestoreColor();
 fontS = &fontLarge;

 return 0;
}
static fntEscapeC fntERestore(fntERestoreF);





static float fntEColorF(bool mute, char* s, char** e, float h)
{
 if(!isdigit(s[0]) || !isdigit(s[1]) || !isdigit(s[2])) return 0;
 *e = s + 3;
 if(mute) return 0;

 char r = s[0]-'0';
 char g = s[1]-'0';
 char b = s[2]-'0';
 glColor3f(r/9., g/9., b/9.);

 return 0;
}
static fntEscapeC fntEColor(fntEColorF);





static float fntEGreekF(bool mute, char* s, char** e, float h)
{
 if(*s != 'e') return 0;
 *e = s+1;

 if(fontS != &fontGreek)	fontS = &fontGreek;
 else				fontS = &fontLarge;

 return 0;
}
static fntEscapeC fntEGreek(fntEGreekF);





/*=============================================================================
 * LOW and INDENT
 * Mid-level functions.
 */
static float fntLow(bool measure_only, char* start, char* end)
{
 int lines = 1;
 float w = .0;
 float maxw = .0;
 char* r;


 for(char* c = start; *c && (!end || c < end); c++)
	if(*c == '?') // && (r = fntEscape(measure_only, c+1)))
	{
	    w += fntEscape(measure_only, c+1, &c);
	    c--;
	}
	else if(*c != '\n') w += fntChar(measure_only, *c);
	else
	{
	    if(maxw < w) maxw = w;
	    w = 0;
	    lines++;
	    if(!measure_only)
	    {
		glPopMatrix();
		glTranslated(0, -fontH, 0);
		glPushMatrix();
	    }
	}

 if(maxw < w) maxw = w;
 return measure_only ? maxw : lines*fontH;
}





static float fntIndent(bool translate, int indent, char* fmt, va_list arg)
{
 fntSaveColor();
 char bf[512];

 // prepare output
 vsnprintf(bf, sizeof(bf)-1, fmt, arg);
 va_end(arg);
 bf[sizeof(bf) -1] = '\0';


 // save height, as it may get modified
 float h = fontH;
 float w = -1;

 // calculate width only if necessary
 if(fontMaxW || indent != 'l')
    w = fntLow(true, bf, 0);


 // indent and print
 glPushMatrix();
    if(fontMaxW && w > fontMaxW)    // adjust to max width
    {
	glTranslated(0, fontH*(w-fontMaxW)/4, 0);
	fontH *= fontMaxW/w;
        w = fontMaxW;
    }

    if(indent == 'c') glTranslated(-w/2, 0, 0);
    if(indent == 'r') glTranslated(  -w, 0, 0);
    float r = fntLow(false, bf, 0);
 glPopMatrix();

 // restore height
 fontH = h;

 // return
 return r;
}





/*=============================================================================
 * HIGHER LEVEL:
 *
 * Left
 * Center
 * Middle
 */
float fntL(char* fmt, ...)
{
 va_list arg;
 va_start(arg, fmt);
 return fntIndent(false, 'l', fmt, arg);
}

float fntC(char* fmt, ...)
{
 va_list arg;
 va_start(arg, fmt);
 return fntIndent(false, 'c', fmt, arg);
}

float fntR(char* fmt, ...)
{
 va_list arg;
 va_start(arg, fmt);
 return fntIndent(false, 'r', fmt, arg);
}





/*=============================================================================
 * HIGHER LEVEL: TEXT
 *
 * Instead of reducing text height, adds newlines to fit in maxw.
 */
static int txtBull;
static char txtBullBf[] = { ' ', '~'+1, ' ', '\0'};

static float fntEBulletOpenF(bool mute, char* s, char** e, float h)
{
 if(*s != '>') return 0;
 *e = s+1;

 txtBull++;

 return fntLow(mute, txtBullBf, 0);
}

static float fntEBulletCloseF(bool mute, char* s, char** e, float h)
{
 if(*s != '<') return 0;
 *e = s+1;

 if(txtBull)
    txtBull--;

 return 0;
}

static fntEscapeC fntEBulletOpen(fntEBulletOpenF);
static fntEscapeC fntEBulletClose(fntEBulletCloseF);





float fntT(float maxw, char* fmt, ...)
{
 fntSaveColor();

 // prepare string
 char bf[2048];
 va_list arg;
 va_start(arg, fmt);
 vsnprintf(bf, sizeof(bf)-1, fmt, arg);
 va_end(arg);
 bf[sizeof(bf) -1] = '\0';



 // init
 txtBull = 0;
 float space_size = fntChar(true, ' ');
 float totalW = .0;
 int spaces = 0;
 int lines = 1;

 // newline macro
#define doNewline()			\
 {					\
    fntLow(false, "\n", 0);		\
    lines++;				\
    totalW = txtBull*space_size;	\
    spaces = 0;				\
    glTranslated(totalW, 0, 0);		\
 }					\



 // write word
 glPushMatrix();
 char* c = bf;
 while(*c)
    if(*c == '\n')
    {
	doNewline();
	c++;
    }
    else if(*c <= ' ')
    {
	totalW += space_size;
	spaces++;
	c++;
    }
    else
    {
	// write spaces
	glTranslated(space_size*spaces, 0, 0);
	spaces = 0;

	// find word length
	char* e;
	for(e = c; *e > ' '; e++);
	float w = fntLow(true, c, e);

	// newline if necessary
	if(totalW + w > maxw)
	    doNewline();

	// do write
	fntLow(false, c, e);
	totalW += w;	// can't use fntLow(false, ..): it returns h instead of w.
	c = e;
    }



#undef doNewline



 glPopMatrix();
 return lines*fontH;
}





/*=============================================================================
 * SET FONT SIZE
 */
#define ROWS	8


static bool fntIsVoid(pngT* png, int X, int Y)
{
 for(int y = Y; y - Y < png->h / ROWS; y++)
    if(png->p[X + png->w*y].a) return false;
 return true;
}

static int fontSkipVoid(pngT* p, int X, int Y)
{
 while(X < p->w - 1 && fntIsVoid(p, X, Y)) X++;
 return X;
}

static int fontSkipChar(pngT* p, int X, int Y)
{
 while(X < p->w - 1 && !fntIsVoid(p, X, Y)) X++;
 return X;
}



static int fontSetBoxes(pngT* png, fontC* fnt)
{
 chC* c = fnt->Ch;
 chC* e = fnt->Ch + FNT_CNT;

 double H = png->h / ROWS;
 double S = 0.;
 int xF, x0;
 int yF, y0;

 for(y0 = 0; y0 < png->h && c < e; y0 += png->h/ROWS, c--)
    for(xF = 0; xF < png->w && c < e; xF++, c++)
        {
	    x0 = fontSkipVoid(png, xF, y0);	// skip empty area
	    xF = fontSkipChar(png, x0, y0);	// skip character
	    yF = y0 + png->h / ROWS;

	    c->tx0 = (double)x0 / png->w;	// set font offset
	    c->txF = (double)xF / png->w;	// set texture width
	    c->ty0 = (double)y0 / png->h;
	    c->tyF = (double)yF / png->h;

	    c->w = (double)(xF - x0) / H;	// set screen width
	    S += c->w;
    }

 fnt->Last = c;				// c has been decreased by top loop
 if(fnt->Last <= fnt->Ch) return -1;	// no chars found!

 // set space width
 fnt->Space = S / (c - fnt->Ch + 1);


 // force underscore width, important for blinking cursor:
 fnt->Ch['_' - FNT_FIRST].w = fnt->Space;

 return 0;
}





/*=============================================================================
 * PROC
 */
static int fontProc(void* png, void* fnt)
{
 if(fontSetBoxes((pngT*)png, (fontC*)fnt)) return -1;

 return 0;
}





/*=============================================================================
 * CONSTRUCTOR
 */
fontC::fontC(char* name, char* fn, char* pattern)
    :Text(fn)
{
 fontC **h;

 for(h = &fontHook; *h; h = &(*h)->Next);
 *h = this;

 memset(Ch, 0, sizeof(Ch));

 Name = name;
 Last = Ch;

 Text.Proc = fontProc;
 Text.ProcV = this;
}





//EOF =========================================================================
