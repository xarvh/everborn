/*
 CITY_DISPLAY.CPP	v0.02
 City Display

 Copyright (c) 2006 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include <math.h>

#include "main.h"
#include "game.h"
#include "city.h"
//#include "unit.h"
#include "utype.h"
#include "race.h"
#include "colors.h"


dataC blds("city.png", 1024/170., 1024/170.);


/*=============================================================================
 * BLD
 */
void city_displayBld(cityC* city, int id)
{
 bldC* bld = city->Race->Blds[id];
 glEnable(GL_BLEND);
 glColor4f(1,1,1,1);
 if(bld->AniStart < 0) city->Race->City.Quad(0 - bld->AniStart);
 else blds.Quad(bld->AniStart);
}



/*=============================================================================
 * SPC
 */
void city_displaySpc(cityC* city, int id)
{
 glEnable(GL_BLEND);
 glColor4f(1,1,1,1);
 if(id == QS_TRADE_GOODS) blds.Quad(1);
 else city->Race->City.Quad(0);
}




/*=============================================================================
 * UNIT
 */
void city_displayUnit(cityC* city, int id)
{
 float angle = (timeGet()/100 + id*200)%360;

 glDisable(GL_BLEND);
 glTranslated(0, -.5, -.5);
 glScaled(.7, .7, .5);
 glRotated(angle, 0, 1, 0);

 duType(city->Race->GetUtype(id), city->Owner->Color);
}



/*=============================================================================
 * PROD ITEM
 */
void city_displayProdItem(cityC* city, int type, int id)
{
 glPushMatrix();
 switch(type) {
    case Q_UNIT: city_displayUnit(city, id); break;
    case Q_BLD: city_displayBld(city, id); break;
    case Q_SPC: city_displaySpc(city, id); break;
    default: break;
 }
 glPopMatrix();
}



/*=============================================================================
 * POP
 */
void city_displayPop(cityC* city, fleshC* f, colorC* c)
{
 glPushMatrix();
    glRotated(190, 0, 1, 0);
    fleshMultiAnimate(f, c);
 glPopMatrix();
}



void city_displayFarmer(cityC* city)
{
 colorC* c = city->Owner ? city->Owner->Color : colorGet(8);
 city_displayPop(city, &city->Race->Farmer, c);
}

void city_displayWorker(cityC* city)
{
 colorC* c = city->Owner ? city->Owner->Color : colorGet(8);
 city_displayPop(city, &city->Race->Worker, c);
}

void city_displayRebel(cityC* city)
{
 city_displayPop(city, &city->Race->Rebel, colorGet(8));
}





/*=============================================================================
 * HUT DISPLAY
 */
void cityC::Display(double a)
{
 glEnable(GL_BLEND);
 glEnable(GL_ALPHA_TEST);
 glDisable(GL_DEPTH_TEST);
 glColor3f(1, 1, 1);

 // P is city total population count
 // size factor is:
 // F = A log(P) + F0
 double Fmax = 1.3;
 double Fmin =  .4;
 double Pmax = 30000;
 double Pmin = CITY_SETTLERS_POPULATION;

 double A = (Fmax - Fmin) / log(Pmax/Pmin);
 double F0 = Fmax - A*log(Pmax);

 double f = F0 + A*log(TotalPop*1000 + Pop);
 glPushMatrix();
    glScaled(f, f, f);
    glTranslated(0, .2, .45);
    glRotated(a, 1,0,0);
    Race->Land.Quad(0);
 glPopMatrix();
}





//EOF =========================================================================
