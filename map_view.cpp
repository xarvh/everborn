/*
 MAP_VIEW.CPP		v0.02
 Map-to-screen visualization.

 Copyright (c) 2004-2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include <math.h>
#include <GL/gl.h>
#include <stdlib.h>
#include "main.h"

#include "map_view.h"




#define FOV	10


// this expresses the "screen sizes" covered in a second.
static envC envScrollspeed("scrollspeed", "1.5");



static char* envRotF(char* v)
{
 static char envRotBf[4];

 float rot;
 if(sscanf(v, "%f", &rot) < 1) rot = 30;
 else if(rot < 0) rot = 0;
 else if(rot > 60) rot = 60;

 sprintf(envRotBf, "%d", (int)rot);
 return envRotBf;
}

static envC envRot("rot", "30", envRotF);





/*=============================================================================
 * CONSTRUCTOR
 */
mapViewC::mapViewC()
{
 MapW = MapH = 1;
 WCycle = false;

 ScrX = ScrY = 0;
 ScrW = ScrH = 1;

 CellsInMidline = 10.;
 Rot = 30;
 MapX = MapW/2.;
 MapY = MapH/2.;
}





/*=============================================================================
 * NAVIGATION
 */
void mapViewC::CenterAt(float x, float y)
{
 MapX = x;
 MapY = y;
 Adjust();
}

void mapViewC::Scroll(float dx, float dy)
{
 MapX += dx;
 MapY += dy;
 Adjust();
}

void mapViewC::Zoom(float f)
{
 CellsInMidline *= pow(.9, f);
 Adjust();
}





/*=============================================================================
 * MOUSE SCROLL
 */
void mapViewC::MouseScroll()
{
 static unsigned long lastTime = 0;
 unsigned long deltaTime = timeGet() - lastTime;

 if(deltaTime < 1000)
 {
    double sp = atof(envGet("scrollspeed"))*deltaTime / 1000.;

    double sh = sp * CellsInMidline;
    double sv = sp * CellsInMidline;

    if(mouseX() == screenW()-1)	Scroll(+sh,  .0);
    else if(mouseX() == 0)	Scroll(-sh,  .0);

    if(mouseY() == screenH()-1)	Scroll( .0, -sv);
    else if(mouseY() == 0)	Scroll( .0, +sv);
 }

 lastTime += deltaTime;
}





/*=============================================================================
 * ADJUST
 */
double mapViewC::MinWidth() { return CellsInMidline/2; }
double mapViewC::MaxWidth() { return CellsInMidline*2; }

void mapViewC::Adjust()
{
 //Fix window size: too large...
 if(CellsInMidline < 3.) CellsInMidline = 3.;

 //... and too small
 // in maps that cycle, it's OK to have lower cells left out, so we use MapW.
 // in maps that don't cycle, MapW*2 allows the lower cells to be displayed.
 double maxW = WCycle ? MapW : MapW*2;
 if(CellsInMidline > maxW) CellsInMidline = maxW;


 //Check window Y position
 double oy;
 oy = MapYFromScreenY(.5) - MapH;
 if(oy > 0) MapY -= oy;

 oy = MapYFromScreenY(-.5) + 1;
 if(oy < 0) MapY -= oy;


 //Check window X position
 if(WCycle)
 {
    while(MapX < 0) MapX += MapW;
    while(MapX >= MapW) MapX -= MapW;
 }
 else
 {
    if(MapX - MinWidth()/2 < 0) MapX = MinWidth()/2;
    if(MapX + MinWidth()/2 > MapW) MapX = MapW-MinWidth()/2;
 }
}





/*=============================================================================
 * REVERSE TRANSFORMATION
 *
 * FROM SCREEN coordinates TO MAP coordinates
 */
double mapViewC::AspectRatio()
{
 return ScrW*screenW() / (double)(ScrH*screenH());
}



double mapViewC::MapYFromMouseY()
{
 return MapYFromScreenY(.5 - mouseY() / (ScrH*screenH()));
}



double mapViewC::MapYFromScreenY(double scrY)
{
 double Q = scrY;
 double a = M_PI * Rot / 180.;
 double g = .5 * M_PI * FOV / 180.;

 // this assumes that the scene is projected on a plane, and anyway is
 // is just an approximation for smaller FOVs.
 // I can't explain to myself why that tan(g) value works so well...
 if(Q < 0) g = -g;
 double y = Q / (cos(a) - tan(g)*sin(a));

 // this assumes that the scene is projected on a sphere.
 // it is more accurate (in particular with larger FOVs)
 // but I determined the 2.4 factor empirically, and it is a function
 // of aspect/viewport I can't determine.
 //
 // double l = 2 * Q * g;
 // double y = 2.4*(cos(a) * tan(l+a) - sin(a)) / rh;

 return (y * CellsInMidline / AspectRatio()) + MapY;
}





/*=============================================================================
 * LOAD PROJECTION
 */
int mvfov = 8;
double mvznear = 1;
double mvzfar = 8;



void mvInput(char k)
{
 switch(k) {
    case 'T': mvfov += 2; break;
    case 'G': mvfov -= 2; break;
    case 'Y': mvznear *= 1.1; break;
    case 'H': mvznear /= 1.1; break;
    case 'U': mvzfar *= 1.1; break;
    case 'J': mvzfar /= 1.1; break;
    default: return;
 }

 pf("fov %d    near %f    far %f\n", mvfov, mvznear, mvzfar);
}





void mapViewC::LoadProjection()
{
 // set viewport
 glViewport(
    (int) (ScrX * screenW()),
    (int) (ScrY * screenH()), //((1. - ScrH - ScrY) * screenH()),
    (int) (ScrW * screenW()),
    (int) (ScrH * screenH())
 );


 // select and reset projection matrix
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();


 // perspective transformation
 double fov = mvfov;
 double aspect = AspectRatio();
 double znear = mvznear;
 double zfar = mvzfar; //5; @@actually -calculating it would be better...


 // avoid using
 // gluPerspective(fov, aspect, znear, zfar);
 // and use glFrustum() instead
 double top = znear * tan(fov * M_PI /180. /2.);
 double bottom = -top;
 double right = aspect*top;
 double left = -right;
 glFrustum(left, right, bottom, top, znear, zfar);



 // trapezoid horizontal midline must be exactly contained by horizontal fov
 // move camera backwards accordingly
 double rh = 2 * aspect * tan( M_PI * .5 * fov / 180 );
 glTranslated(0, 0, -1/rh); 


 // rotate
 glRotated(Rot, -1, 0, 0);


 // scale from screen to map coordinates
 double zoom = 1/CellsInMidline;
 glScaled(zoom, zoom, zoom);


 // translate to map position
 glTranslated(-MapX, -MapY, 0);


 // select and reset modelview matrix
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
}





/*=============================================================================
 * Field Of View SWEEP
 */
float mapViewC::TLX(){ return -.5*CellsInMidline + MapX -0;}
float mapViewC::TRX(){ return +.5*CellsInMidline + MapX +0;}



void mapViewC::FOVSweep(mapSweepF* sweeps)
{
 LoadProjection();

 // iterate along the frustum intersection with the map plane
 //@@ -sigh-, I'm still unable to reverse the coordinates propely

 int tly = (int)(MapYFromScreenY(+.55)+.5555);
 int bly = (int)(MapYFromScreenY(-.55));

 for(int y = tly; y >= bly; y--)
    for(mapSweepF* sw = sweeps; *sw; sw++)
	for(int x = (int)TLX(); x <= TRX(); x++)
	{
	    if(y < 0 || y >= MapH) continue;
	    if(!WCycle) if(x < 0 || x >= MapW) continue;

	    glLoadIdentity();
	    glTranslated(x, y, 0);
	    (*sw)(x, y);
	}
}



//EOF =========================================================================
