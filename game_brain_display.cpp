/*
 BRAIN_DISPLAY.CPP		v0.00


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

#include "main.h"
#include "colors.h"

#include "counters.h"

#include "game.h"
#include "race.h"
#include "fnt.h"



/*=============================================================================
 * HELPERS
 */
static colorC bdColor(brainC* brain)
{
 colorC dead = {0,80,80,80};

 if(brain->Status == BRAIN_ACTIVE) return *brain->Color;
 return dead;
}

static colorC bdBlink(colorC c)
{
 if((timeGet() / 300) & 1)
 {
    c.R = c.R / 2;
    c.G = c.G / 2;
    c.B = c.B / 2;
 }
 else
 {
    c.R = 5 * c.R / 4;
    c.G = 5 * c.G / 4;
    c.B = 5 * c.B / 4;
 }

 return c;
}



/*=============================================================================
 * SQUARE
 *
 * Requires screenLineWidth() to be set!!!
 */
static void bdSquare(colorC c)
{
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_DEPTH_TEST);
 glDisable(GL_ALPHA_TEST);
 glDisable(GL_BLEND);

 // square
 glBegin(GL_QUADS);
    glColor3ub(c.R/2,  c.G/2,  c.B/2);	glVertex2f(-.5, -.5);
    glColor3ub(c.R,    c.G,    c.B);	glVertex2f(-.5, +.5);
    glColor3ub(c.R,    c.G,    c.B);	glVertex2f(+.5, +.5);
    glColor3ub(c.R+50, c.G+50, c.B+50);	glVertex2f(+.5, -.5);
 glEnd();

 // contour
 glEnable(GL_BLEND);
 glColor4ub(c.R/2,  c.G/2,  c.B/2, 128);
 glBegin(GL_LINE_STRIP);
    glVertex2f(-.5, -.5);
    glVertex2f(-.5, +.5);
    glVertex2f(+.5, +.5);
    glVertex2f(+.5, -.5);
    glVertex2f(-.5, -.5);
 glEnd();
}





void bdSquare(brainC* b)
{
 bdSquare(*b->Color);
}





/*=============================================================================
 * PLAYERS LIST
 *
 * @@ add a button to make the area active!
 */
void brainC::Display(float h)
{
 // get player color
 colorC color = bdColor(this);

 // set line width for bdSquare()
 screenLineWidth(1);

 // display status
 glPushMatrix();
    glScaled(h, h, 1);
    glTranslated(.5, .5, 0);
    if(Ready)	bdSquare(bdBlink(color));
    else	bdSquare(color);
 glPopMatrix();


 // brain name
 fntS(0, h, .98-h);
 Color->Select();
 glTranslated(1.1*h, 0, 0);
 fntL("%s", Name);

}





//EOF =========================================================================
