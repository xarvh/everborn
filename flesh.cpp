/*
 FLESH.CPP	v0.16
 Flesh render.

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
#include <math.h>
#include "gselect.h"
#include "main.h"
#include "colors.h"
#include "flesh.h"
#include "ske.h"




/*=============================================================================
 * HELPERS
 */
static double rad_from_deg(double deg)
{
 return deg * (3.141592654 / 180.0);
}





/*=============================================================================
 * FLESH
 */
fleshC::fleshC()
{
 for(blockC** b = BlocksStart; b < BlocksEnd; b++) *b = 0;
 width = 1;
}




/*=============================================================================
 * VERTEX DRAW
 */
void vertexC::Draw(colorC* c)
{
 if(R + G + B == 255*3)
    gsColor3ub(c->R, c->G, c->B);	// use player color
 else
    gsColor3ub(R, G, B);		// use normal color

 glTexCoord2f(TX, TY);
 glVertex3f(X, Y, Z);
}





/*=============================================================================
 * BLOCK
 */
blockC::blockC()
{
 Next = 0;
 *Name = '\0';
 Part = PartDefault = 0;
 Strips[0] = Strips[1] = 0;	// double terminate
 Length = Width = 1;
}

blockC::~blockC()
{
 if(Next) delete Next;
}



void blockC::Draw(colorC* color)
{
 if(!this) return;
 if(!Part) return;

 glEnable(GL_DEPTH_TEST);
 glPushMatrix();
 glScaled(Width/100., Length/100., Width/100.);
 for(char* c = Strips; *c; c++)
 {
    glBegin(GL_TRIANGLE_STRIP);
    while(*c) Vertex[fleshCharToVertex(*c)].Draw(color), c++;
    glEnd();
 }

 glPopMatrix();
}





/*=============================================================================
 * DRAW
 *
 * Draws an antropomorphic model.
 * `angle` is a list with all joints angles.
 */
void fleshDraw(fleshC* m, double* angle, colorC* color)
{
 double a, llh, rlh;

 // distance from right hip to ground
 rlh  = m->ruleg->Length * cos(a = rad_from_deg(angle[R_HIP]));
 rlh += m->rlleg->Length * cos(a + rad_from_deg(angle[R_KNEE]));


 // distance from left hip to ground
 llh  = m->luleg->Length * cos(a = rad_from_deg(angle[L_HIP]));
 llh += m->llleg->Length * cos(a + rad_from_deg(angle[L_KNEE]));


 // model height: maximum between rlh and llh
 a = llh > rlh ? llh : rlh;


 glEnable(GL_DEPTH_TEST);
 glPushMatrix();
    glScaled(1/100., 1/100., 1/100.);

    // translate to hip position
    glTranslated(.0, a, .0);

    // right leg
    glPushMatrix();
	if(m->torso) glTranslated(m->torso->Width / 2., 0, 0);
        glRotated(- angle[R_HIP], 1, 0, 0);
	m->ruleg->Draw(color);

	if(m->ruleg) glTranslated(0.0, - m->ruleg->Length, 0);
	glRotated(- angle[R_KNEE], 1, 0, 0);
        m->rlleg->Draw(color);

        if(m->rlleg) glTranslated(0, - m->rlleg->Length, 0);
        glRotated(- angle[R_FOOT], 1, 0, 0);
        m->rfoot->Draw(color);
    glPopMatrix();


    // left leg
    glPushMatrix();
	glScaled(-1, 1, 1);

        if(m->torso) glTranslated(m->torso->Width / 2., 0, 0);
	glRotated(- angle[L_HIP], 1, 0, 0);
        m->luleg->Draw(color);

	if(m->luleg) glTranslated(0, - m->luleg->Length, 0);
        glRotated(- angle[L_KNEE], 1, 0, 0);
	m->llleg->Draw(color);

        if(m->llleg) glTranslated(0, - m->llleg->Length, 0);
	glRotated(- angle[L_FOOT], 1, 0, 0);
        m->lfoot->Draw(color);
    glPopMatrix();


    // translate to neck
    glRotated(angle[TORSO_PITCH], 1, 0, 0);
    if(m->torso) glTranslated(0, m->torso->Length, 0);

    // head
    glPushMatrix();
	glRotated(angle[TORSO_PITCH], 1, 0, 0);
	m->head->Draw(color);
    glPopMatrix();

    // torso
    glRotated(angle[TORSO_ROLL], 0, 1, 0);
    m->torso->Draw(color);


    // right arm
    glPushMatrix();
	if(m->torso) glTranslated(m->torso->Width, 0, 0);
        glRotated(- angle[R_SHX], 1, 0, 0);
	glRotated(  angle[R_SHZ], 0, 0, 1);
        m->ruarm->Draw(color);

	if(m->ruarm) glTranslated(0, - m->ruarm->Length, 0);
        glRotated(- angle[R_ELB], 1, 0, 0);
	m->rlarm->Draw(color);

        if(m->rlarm) glTranslated(0, - m->rlarm->Length, 0);
	glRotated(  angle[R_HAND], 0, 1, 0);
        m->rwep->Draw(color);
        m->rhand->Draw(color);
     glPopMatrix();

    // left arm
    glPushMatrix();
	glScaled(-1, 1, 1);

        if(m->torso) glTranslated(m->torso->Width, 0, 0);
	glRotated(- angle[L_SHX], 1, 0, 0);
        glRotated(  angle[L_SHZ], 0, 0, 1);
        m->luarm->Draw(color);

        if(m->luarm) glTranslated(0, - m->luarm->Length, 0);
        glRotated(- angle[L_ELB], 1, 0, 0);
        m->llarm->Draw(color);

	if(m->llarm) glTranslated(0, - m->llarm->Length, 0);
        glRotated(  angle[L_HAND], 0, 1, 0);
        m->lwep->Draw(color);
        m->lhand->Draw(color);
     glPopMatrix();

 // all body done
 glPopMatrix();
}





//EOF =========================================================================
