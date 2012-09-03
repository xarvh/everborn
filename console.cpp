/*
 CONSOLE.CPP		v0.24
 All console routines.

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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"

void freeglutFixed(char );





/*=============================================================================
 * HEADER
 */
#define CONSOLE_COLOR_DEFAULT		0.8, 0.8, 0.8
#define CONSOLE_COLOR_COMMAND		1.0, 1.0, 1.0
#define CONSOLE_COLOR_ERROR		0.8, 0.2, 0.2
#define CONSOLE_MAX_LINES		50			// max total lines (prompt excluded) //
#define CONSOLE_MAX_CHARS		100			// max chars per line //
#define CONSOLE_DEFAULT_FONT_H		12
#define CONSOLE_HEADER_FONT_H		10
#define CONSOLE_CURSOR_BLINK_RATE	200
#define CONSOLE_LINE_TIME		10
#define CONSOLE_PHANTOM_TIME		1000			// keep each phantom line for 2 sec //
#define CONSOLE_PHANTOM_MAX_LINES	4			// max 4 phantom lines


typedef struct console_line_s {
    char str[CONSOLE_MAX_CHARS];
    float color[3];
} console_line_t;





/*=============================================================================
 * GLOBALS
 */
static char console_prompt[] = "> ";
static char *console_cursor;
static long console_time = 0;
static long console_lines = 0;
static long console_phantom = 0;
static console_line_t  console_lines_a[CONSOLE_MAX_LINES];
static console_line_t *console_lines_end = console_lines_a + sizeof(console_lines_a)/sizeof(console_line_t);
static console_line_t  console_cmds_a[CONSOLE_MAX_LINES];
static console_line_t *console_cmds_end = console_cmds_a + sizeof(console_cmds_a)/sizeof(console_line_t);
static console_line_t *console_cmd;





/*=============================================================================
 * ETA
 */
static long consoleEta()
{
 return timeGet() - console_time;
}

static long consoleEtaZero()
{
 console_time += consoleEta();
}





/*=============================================================================
 * LOAD PROJECTION
 */
static void consoleLoadProjection()
{
 glViewport(0, 0, screenW(), screenH());
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
}





/*=============================================================================
 * BASIC HANDLERS
 */
static void consolePhantomSpawn()
{
 if(console_phantom < consoleEta())
    console_phantom = consoleEta();

 console_phantom += CONSOLE_PHANTOM_TIME;

 long max = consoleEta() + CONSOLE_PHANTOM_TIME*CONSOLE_PHANTOM_MAX_LINES;
 if(console_phantom > max) console_phantom = max;
}


int consoleIsActive()
{
 return console_lines;
}


int consoleOpen()
{
 if(consoleIsActive()) return 0;
 console_lines = 1;
}



int consoleClose()
{
 console_lines = 0;
}





/*=============================================================================
 * CONSOLE HANDLERS
 */
static int console_scroll(console_line_t *console_a)
{
 int k;

 for(k = CONSOLE_MAX_LINES-1; k > 0; k--) console_a[k] = console_a[k-1];	// move one line up //
 console_a[0].str[0] = 0;	// clear first line //
}

static char *console_newline(float r, float g, float b)
{
 console_line_t *l;
 l = console_lines_a;
 l->color[0] = r;
 l->color[1] = g;
 l->color[2] = b;
 return l->str;
}

static char *console_inline()
{
 return console_newline(CONSOLE_COLOR_DEFAULT);
}

static char *console_getline(float r, float g, float b)
{
 consolePhantomSpawn();
 console_scroll(console_lines_a);
 return console_newline(r, g, b);
}







/*=============================================================================
 * PROMPT HANDLERS
 */
static int console_reset_prompt()
{
 console_cmd = console_cmds_a;
 console_cursor = console_cmd->str;
 *console_cursor = 0;
}

static int console_up()
{
 if(console_cmd < console_cmds_end-1) console_cmd++, console_cursor = console_cmd->str + strlen(console_cmd->str);
}

static int console_down()
{
 if(console_cmd > console_cmds_a) console_cmd--, console_cursor = console_cmd->str + strlen(console_cmd->str);
}



static int console_add_r(char *c)
{
 if(*c) console_add_r(c + 1);
 c[1] = c[0];
}

static int console_add(char a)
{
 console_add_r(console_cursor);
 *console_cursor++ = a;
}

static int console_remove(char *c)
{
 if(*c) do c[0] = c[1];
 while(*c++);
}

static int console_delete()
{
 console_remove(console_cursor);
}

static int console_backspace()
{
 if(console_cursor > console_cmd->str) console_remove(--console_cursor);
}

static int console_exec()
{
 strncpy(console_getline(CONSOLE_COLOR_COMMAND), console_cmd->str, CONSOLE_MAX_CHARS-1);
 if(*console_cmd->str)
    {
	// issue command //
	int cmd(const char*, ...);
	cmd(console_cmd->str);

	// copy command to history //
	if(console_cmd != console_cmds_a)
	    strncpy(console_cmds_a->str, console_cmd->str, CONSOLE_MAX_CHARS-1);
	console_scroll(console_cmds_a);
    }
 console_reset_prompt();
}





/*=============================================================================
 * CONSOLE INPUT
 */
int consoleInput(int key, int ext)
{
 if(key) switch(key) {
    case 27:	consoleClose();		break;	// ESC //
    case 13:	console_exec();		break;	// Enter //
    case '\b':	console_backspace();	break;	// Backspace //
    case 127:	console_delete();	break;	// Delete //
    default:	console_add(key);	break;	// char //
 }

 if(ext) switch(ext) {
    case INPUT_KLF:	if(console_cursor > console_cmd->str) console_cursor--; break;
    case INPUT_KRI:	if(*console_cursor) console_cursor++; break;
    case INPUT_KHOME:	console_cursor = console_cmd->str; break;
    case INPUT_KEND:	while(*console_cursor) console_cursor++; break;

    case INPUT_KUP:	console_up(); break;
    case INPUT_KDN:	console_down(); break;
    default: break;
 }

 if(mouseL() || mouseM() || mouseR()) consoleClose();

}





/*=============================================================================
 * CONSOLE DRAW
 */
static double console_line_h(int font_h)
{
 return (2.0 * font_h) / screenH();
}

static void fake_raster(double x, double y)
{
 glRasterPos2d(x*2.0 / screenW() - 1.0, y*2.0 / screenH() - 1.0);
}





static void consoleDrawActive()
{
 double rasterp[4];
 double w, y, down;
 char *c;
 int l;



 // prepare everything //
 y = 1.0;
 y -= (console_lines+1)*console_line_h(CONSOLE_DEFAULT_FONT_H);	// console lines + command prompt
 down = y;
 down -= console_line_h(CONSOLE_DEFAULT_FONT_H / 2);		// last line //
 down -= console_line_h(2);					// console frame //

 consoleLoadProjection();
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_DEPTH_TEST);

 // draw background
 glEnable(GL_BLEND);
 glBegin(GL_QUADS);
    glColor4f(0.7, 0.7, 0.8, 0.9);
    glVertex2f( 1.0, 1.0);
    glColor4f(0.5, 0.5, 0.5, 0.9);
    glVertex2f( 1.0, down);
    glVertex2f(-1.0, down);
    glVertex2f(-1.0, 1.0);
 glEnd();
 glDisable(GL_BLEND);

 // draw console frame
 int headerwidth = 0;
 for(char* c = game_string; *c; c++)
    headerwidth += 9;

 w = 0.02 + (headerwidth*2.) / screenW(); // prompt width
 screenLineWidth(1.);
 glColor4f(1,1,1,1);
 glBegin(GL_LINE_STRIP);		// left and long bottom line
    glVertex2f(-0.99,  1.0);
    glVertex2f(-0.99, down + 0.01);
    glVertex2f( 0.98 - w, down + 0.01);
 glEnd();
 glBegin(GL_LINE_STRIP);		// short bottom line and right line
    glVertex2f( 0.98, down + 0.01);
    glVertex2f( 0.99, down + 0.01);
    glVertex2f( 0.99,  1.0);
 glEnd();

 // console_header
 glColor3f(.0, 1., .0);
 glRasterPos2f(0.99 - w, down + 0.01);		// font height: 10
 for(c = game_string; *c; c++) freeglutFixed(*c);


 // prompt
 glColor4f(0.0, 0.0, 1.0, 1.0);
 glRasterPos2f(-1.0 + 0.02, y);
 for(c = console_prompt; *c; c++) freeglutFixed(*c);
 glGetDoublev(GL_CURRENT_RASTER_POSITION, rasterp);
 glColor3f(CONSOLE_COLOR_COMMAND);
 fake_raster(rasterp[0], rasterp[1]);

 c = console_cmd->str;
 while(c < console_cursor) freeglutFixed(*c++);
 glGetDoublev(GL_CURRENT_RASTER_POSITION, rasterp);
 l = consoleEta() / CONSOLE_CURSOR_BLINK_RATE;
 if(l & 1) freeglutFixed('_');
 fake_raster(rasterp[0], rasterp[1]);
 while(*c) freeglutFixed(*c++);


 // console lines //
 for(l = 0; l < console_lines; l++)
    {
	y += console_line_h(CONSOLE_DEFAULT_FONT_H);
	glColor3fv(console_lines_a[l].color);
	glRasterPos2f(-1.0 + 0.02, y);
	for(c = console_lines_a[l].str; *c; c++) if(*c >= ' ')
	    freeglutFixed(*c);
    }
}





static void consoleDrawPhantom()
{
 signed lines = console_phantom - consoleEta();
 double y = 1.0;

 if(lines < 0) return;

 lines /= CONSOLE_PHANTOM_TIME;
 y -= (lines+2)*console_line_h(CONSOLE_DEFAULT_FONT_H);

 consoleLoadProjection();
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_DEPTH_TEST);

 for(int l = 0; l <= lines; l++)
    {
	y += console_line_h(CONSOLE_DEFAULT_FONT_H);
	glColor3fv(console_lines_a[l].color);
	glRasterPos2f(-1.0 + 0.02, y);
	for(char* c = console_lines_a[l].str; *c; c++) if(*c >= ' ')
	    freeglutFixed(*c);
    }
}





void consoleDraw()
{
 long t, p;

 if(consoleIsActive())
    {
	//console_lines = 1 + consoleEta() / CONSOLE_LINE_TIME;
	//if(console_lines > 25) console_lines = 25;
	if(console_lines < 35) console_lines++;
 	consoleDrawActive();
    }
 else consoleDrawPhantom();
}





/*=============================================================================
 * PRINT CALLS
 */
static char consoleProcName[20] = "";
void consoleSetProcName(char* n)
{
 strncpy(consoleProcName, n, sizeof(consoleProcName));
 consoleProcName[19] = '\0';
}



int pinline()				// prepare for inline calls
{
 console_getline(CONSOLE_COLOR_DEFAULT);
 consoleOpen();
}

int pi(const char *fmt, ...)		// print inline
{
 va_list arg;
 va_start(arg, fmt);
 vsnprintf(console_inline(), CONSOLE_MAX_CHARS-1, fmt, arg);
 va_end(arg);
}



int pe(const char *fmt, ...)		// print error
{
 va_list arg;

 // stderr
 va_start(arg, fmt);
 fprintf(stderr, "%s ", consoleProcName);
 vfprintf(stderr, fmt, arg);
 va_end(arg);
 fflush(stderr);

 // console, error
 va_start(arg, fmt);
 vsnprintf(console_getline(CONSOLE_COLOR_ERROR), CONSOLE_MAX_CHARS-1, fmt, arg);
 va_end(arg);

 // popup console to show the error
 consoleOpen();
}



int pf(const char *fmt, ...)		// print
{
 va_list arg;

 // stdout
 va_start(arg, fmt);
 fprintf(stdout, "%s ", consoleProcName);
 vfprintf(stdout, fmt, arg);
 va_end(arg);

 // console, default
 va_start(arg, fmt);
 vsnprintf(console_getline(CONSOLE_COLOR_DEFAULT), CONSOLE_MAX_CHARS-1, fmt, arg);
 va_end(arg);
}





/*=============================================================================
 * MODULE INIT
 */
class consoleC { public:
    consoleC()
    {
	memset(console_lines_a, 0, sizeof(console_lines_a));
	memset(console_cmds_a, 0, sizeof(console_cmds_a));
	console_reset_prompt();
    }
} static console;



//EOF =========================================================================
