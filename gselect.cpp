/*
 GSELECT.CPP		v0.01
 Selection from fake GL rendering.

 Copyright (c) 2005 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "gselect.h"



/*=============================================================================
 * GL OVERRIDES
 */
void gsColor3d(double r, double g, double b)
{
 if(gsOff()) glColor3d(r, g, b);
}



void gsColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
 if(gsOff()) glColor3ub(r, g, b);
}





/*=============================================================================
 * GLOBALS
 */
#define ITEMS_BF	(1024*256)
#define ITEM_MASK	(ITEMS_BF-1)	// power of 2

static unsigned gsId;
static void* gsItems[ITEMS_BF];
static bool gsIsOff = true;





/*=============================================================================
 * HELPERS
 */
static unsigned char colors[4];
static unsigned char* colorFromId(int id)
{
 colors[0] = id & 0xff;
 colors[1] = (id>>8)&0xff;
 colors[2] = (id>>16)&0xff;
 colors[3] = 0;
 return colors;
}

static int idFromColor(unsigned char* c)
{
 return (c[0] | (c[1]<<8) | (c[2]<<16)) & ITEM_MASK;
}



/*=============================================================================
 *
 */
bool gsOff()
{
 return gsIsOff;
}

bool gsOn()
{
 return !gsIsOff;
}

void gsBegin()
{
 gsId = 1;	// color #0: 00,00,00 is always NULL
 gsIsOff = false;
 memset(gsItems, 0, sizeof(gsItems));
 glClear(GL_COLOR_BUFFER_BIT);
}

void gsPush(void* obj)
{
 if(gsOff()) return;
 gsItems[gsId] = obj;
 glColor3ubv(colorFromId(gsId));

 if(gsId < ITEMS_BF-1) gsId++;	// silently ignore too many objects
}

void* gsEnd(unsigned x, unsigned y)
{
 gsIsOff = true;

 unsigned char bf[4];
 glReadPixels(x, screenH()-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, bf);

 return gsItems[idFromColor(bf)];
}





//EOF =========================================================================
