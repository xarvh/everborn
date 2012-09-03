/*
 PARTICLES.CPP		v0.00
 Battle particles.

 Copyright (c) 2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "map.h"
#include "unit.h"
#include "random.h"
#include "particles.h"



static randomC Rnd;



/*=============================================================================
 * PARTICLES
 */
particleC::particleC(battleC* battle, float x, float y)
{
 particleC** h;
 for(h = &battle->ParticlesHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Battle = battle;
 X = x;
 Y = y;
 Z = .0;
}

particleC::~particleC()
{
 particleC** h;
 for(h = &Battle->ParticlesHook; *h != this; h = &(*h)->Next);
 *h = Next;
}





/*=============================================================================
 * PARTICLE: BLOOD
 */
void particleBloodC::Display()
{
 float s = Size/100.;
 glRotated(Zrot, 0, 0, 1);
 glRotated(90, 0, 1, 0);
 glColor4f(1, 0, 0, 1);
 glScaled(1, 1, s);

 glDisable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);
 glDisable(GL_TEXTURE_2D);
 glBegin(GL_TRIANGLES);
    glColor4f(.9,0,0, 1-s); glVertex3f(0, 0,1.);
    glColor4f(.7,0,0, 1-s); glVertex3f(0,.1,.1);
    glColor4f(.9,0,0, 1  ); glVertex3f(0,.0,.0); //@@move it away 
 glEnd();
 glEnable(GL_TEXTURE_2D);
 glDisable(GL_BLEND);
 glEnable(GL_DEPTH_TEST);
}

void particleBloodC::Think()
{
 Size += 4*Rnd(20);
 if(Size > 160) delete this;
}

particleBloodC::particleBloodC(unitC* u)
    :particleC(u->Cell->Battle, u->X, u->Y)
{
 Z = .5;
 Zrot = (int)(360.*Rnd());
 Size = Rnd(40) + 30;
}





/*=============================================================================
 * PARTICLE: ARROW
 */
static void displayArrow()
{
 glColor3f(.3,.2,.2);
 glDisable(GL_DEPTH_TEST);
 glDisable(GL_TEXTURE_2D);
 glBegin(GL_TRIANGLES);
    glVertex3f(.06,-.6, 0);
    glVertex3f(.00,.0, .1);
    glVertex3f(.00,-.6,.0);
 glEnd();
 glEnable(GL_TEXTURE_2D);
 glEnable(GL_DEPTH_TEST);
}



void particleArrowC::Display()
{
 // Target may have escaped from battle
 if(Target->Cell != Battle->Cell) delete this;

 double w = Target->X - X;
 double h = Target->Y - Y;
 double angle = (180/3.1419)*atan2(w, h);

 glRotated(angle, 0, 0, -1);

 float* stack = Owner->Stack();
 for(int f = 0; f < Figures; f++)
 {
    glPushMatrix();
     glTranslated(stack[f*2], stack[f*2+1], 0);
     displayArrow();
    glPopMatrix();
 }
}

void particleArrowC::Think()
{
 //@@ Target X and Y should be guaranteed available even if the target dies

 // find direction components
 double w = Target->X - X;
 double h = Target->Y - Y;
 double l = sqrt(w*w + h*h);

 double dx = w * PARTICLE_ARROW_SPEED / l;
 double dy = h * PARTICLE_ARROW_SPEED / l;

 // stay a bit longer than actual reach, otherwise the arrow will turn abruptly.
 if(w*w + h*h < (dx*dx + dy*dy)*3)
 {
    Owner->InflictRangedDamage(Target, Figures);
    new particleLandedArrowC(Target, (180/3.1419)*atan2(dx, dy));
    delete this;
 }
 else
 {
    X += dx;
    Y += dy;
 }
}


particleArrowC::particleArrowC(unitC* u, unitC* target)
    :particleC(u->Cell->Battle, u->X, u->Y)
{
 Z = .5;
 Owner = u;
 Target = target;
 Landed = 0;
 Figures = Owner->Figures();
}



/*=============================================================================
 * PARTICLE: LANDED ARROW
 */
void particleLandedArrowC::Display()
{
 glTranslated(0,.7,0);	//@@ not here, Z should be used instead
 glRotated(Angle, 0, 0, -1);
// glTranslated();
 displayArrow();
}



void particleLandedArrowC::Think()
{
 if(Age++ > Rnd(5) + 2) delete this;
}

particleLandedArrowC::particleLandedArrowC(unitC* u, float angle)
    :particleC(u->Cell->Battle, u->X, u->Y)
{
 Target = u;
 Angle = angle;
 Age = 0;
}





//EOF =========================================================================
