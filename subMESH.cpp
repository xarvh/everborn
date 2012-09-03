/*
 subMESH.cpp		v0.01
 subprogram:

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

 NOTE:
 Once this was a standalone.
*/

#include <GL/gl.h>
#include <stdlib.h>

#include "main.h"
#include "boxgui.h"

#include "mesh.h"



/*=============================================================================
 * GLOBALS
 */
static meshFileC MeshFile("boh.obj");


static int color;
static float sp, t;
static float Ax, Ay, Az;





/*=============================================================================
 *
 */
static void drawMesh()
{
 glShadeModel(GL_SMOOTH);

 GLfloat light_position[] = { 1, 1, 1, .0 };
 glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	    
 glEnable(GL_LIGHTING);
 glEnable(GL_LIGHT0);
 glEnable(GL_DEPTH_TEST);
		     





 glColor3f(1,1,1);
 glPushMatrix();
    glScaled(.5, .5, .5);

// MeshFile.Load();

 if(MeshFile.MeshesHook) // && MeshFile.MeshesHook->GLList)
 {
    GLuint list = MeshFile.MeshesHook->GLList;

//    if(glIsList(list)) pf("List\n");
//    else pf("NOT LIST!!!!\n");

    glCallList(list);
 }

 glPopMatrix();
 glDisable(GL_LIGHTING);
}



/*=============================================================================
 * SWITCHES
 */
static int BLOcolor(signed dc)
{
 color += dc;
}

static int displaySpeed(int w)
{
 sp += w * 0.004;
}



static int displayRotate(double x, double y, double z)
{
 Ax += x;
 Ay -= y;
 Az += z;
}





/*=============================================================================
 * LOAD
 */
static int BLOrelo()
{
 if(MeshFile.Load())
 {
    pe("unable to load obj\n");
//    exit(-1);
 }

// MeshFile.Load();
}





/*=============================================================================
 * DISPLAY
 */
static void MENUdraw(boxC& b)
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glLoadIdentity();
 glTranslated(.5, b.H/2, 0);
 glScaled(.5, .5, .5);
 glDisable(GL_DEPTH_TEST);

 glDisable(GL_ALPHA_TEST);
 glDisable(GL_BLEND);
 glDisable(GL_TEXTURE_2D);

 glPushMatrix();
    glTranslated(0.0, -0.35, 0.0);
    glScaled(0.7, 0.7, 0.7);
    glRotated(Ax, 1.0, 0.0, 0.0);
    glRotated(Az, 0.0, 0.0, 1.0);

    glColor4f(.0, .0, 1., 1.);
    screenLineWidth(2);
    glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 1.0);
	glVertex2f(+0.5, 1.0);
	glVertex2f(+0.5, 0.0);
	glVertex2f(-0.5, 0.0);
    glEnd();

    glRotated(Ay, 0.0, 1.0, 0.0);


    // draw
    drawMesh();



 glPopMatrix();

 glTranslated(-0.8, 0.0, 0.0);

 glPushMatrix();
    glScaled(0.15, 0.15, 0.15);
    glRotated(Ax, 1.0, 0.0, 0.0);
    glRotated(Az, 0.0, 0.0, 1.0);
    glRotated(Ay, 0.0, 1.0, 0.0);
    glTranslated(0.0, -0.5, 0.0);

    //
    drawMesh();

 glPopMatrix();


 t += sp;
 if(t > 1.0) t = 0.0;
 if(t < 0.0) t = 1.0;
}






/*=============================================================================
 * MENU STUFF
 */
static bool MENUinput(boxC& b)
{
 switch(inputK()) {
    case 27: exit(0); break;

    case 32: BLOrelo(); break;

    case 'a': displayRotate(0.0, -5.0, 0.0); break;
    case 'd': displayRotate(0.0, +5.0, 0.0); break;

    case 'w': displayRotate(+5.0, 0.0, 0.0); break;
    case 's': displayRotate(-5.0, 0.0, 0.0); break;

    case 'q': displayRotate( 0.0, 0.0,+5.0); break;
    case 'e': displayRotate( 0.0, 0.0,-5.0); break;

    case 'z': BLOcolor(+1); break;
    case 'x': BLOcolor(-1); break;

    case '+': displaySpeed(+1); break;
    case '-': displaySpeed(-1); break;

//    case '.': displaySwitchMode(); break;

//    case 'm': BLOunit(+1); break;
//    case 'n': BLOunit(-1); break;

//    case 'r': BLOske(0, +5.0); break;
//    case 'f': BLOske(0, -5.0); break;
//    case 't': BLOske(1, +5.0); break;
//    case 'g': BLOske(1, -5.0); break;
//    case 'y': BLOske(2, +5.0); break;
//    case 'h': BLOske(2, -5.0); break;
//    case 'u': BLOske(3, +5.0); break;
//    case 'j': BLOske(3, -5.0); break;
//    case 'i': BLOske(4, +5.0); break;
//    case 'k': BLOske(4, -5.0); break;
//    case 'o': BLOske(5, +5.0); break;
//    case 'l': BLOske(5, -5.0); break;

    default: break;
 }

 switch(inputE()) {
/*    case GLUT_KEY_UP:	visualScroll( 0.0, +sp); break;
    case GLUT_KEY_DOWN: visualScroll( 0.0, -sp); break;
    case GLUT_KEY_LEFT: visualScroll( -sp, 0.0); break;
    case GLUT_KEY_RIGHT:visualScroll( +sp, 0.0); break;*/
//    case GLUT_KEY_F1: quadSwitchSurfaceMode(); break;
//    case GLUT_KEY_F2: quadSwitchWireMode(); break;
    default: break;
 }

 // buttons
/* bt_t *a;
 if(key) if(a = guiBtKey(MENUbtset, key)) a->call(a);
 if(a = guiBtMouse(MENUbtset, guiMouseX(), guiMouseY()))
    if(a->call) a->call(a);
*/
}




static char* MENUstart()
{
 pf("MENUSTART\n");

 return 0;
}





/*=============================================================================
 * MESH MENU
 */
static bmenuDECLARE ( blo ) {

    StartF = MENUstart;
    DisplayF = MENUdraw;
    InputF = MENUinput;

} bmenuINSTANCE;



//EOF =========================================================================
