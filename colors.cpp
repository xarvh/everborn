/*
 COLORS.CPP		v0.04
 Player Colors.

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
#include <string.h>
#include "colors.h"



void colorC::Select(float a)
{
 glColor4f(R/255., G/255., B/255., a);
}



static colorC colorA[] = {
    {"black",	 75,  75,  75},
    {"blue",	 75,  75, 200},
    {"green",	 75, 200,  75},
    {"cyan",	 75, 200, 200},
    {"red",	200,  75,  75},
    {"purple",	200,  75, 200},
    {"yellow",	200, 200,  75},
    {"white",	200, 200, 200},
    {"brown",	135, 100,  35},
    { 0, 0, 0, 0 }
};



colorC* colorNeutral()
{
 return colorA + 8;
}



colorC* colorGet(char* n)
{
 colorC* c;
 for(c = colorA; c->Name && strcasecmp(c->Name, n); c++);
 if(c->Name) return c;
 return colorA;
}

colorC* colorGet(unsigned i)
{
 i %= sizeof(colorA)/sizeof(colorC) -1;
 return colorA + i;
}





//EOF =========================================================================
