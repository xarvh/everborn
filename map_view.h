/*
 MAP_VIEW.H		v0.01
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
#ifndef H_FILE_MAP_VIEW
#define H_FILE_MAP_VIEW
//=============================================================================


typedef void (*mapSweepF)(int x, int y);


class mapViewC { public:
    // map features
    unsigned MapW;
    unsigned MapH;
    bool WCycle;

    // viewport selection: normalized to be indipendent of screen size
    double ScrX;
    double ScrY;
    double ScrW;
    double ScrH;

    // zoom: expressed as how many cells fit in the horizontal midline
    // of the trapezoid (the intersection between frustum and map plane)
    double CellsInMidline;

    // rotation angle: 0 is from above
    double Rot;

    // map coordinates centered by the window
    double MapX;
    double MapY;


    // -----------------
    mapViewC();
    // -----------------
    void CenterAt(float x, float y);
    void Scroll(float dx, float dy);
    void MouseScroll();
    void Zoom(float dz);
    void Adjust();

    // -----------------
    double AspectRatio();
    double MapYFromScreenY(double scrY);
    double MapYFromMouseY();

    double MaxWidth();		// trapezoid major base
    double MinWidth();		// trapezoid minor base
    float TLX();
    float TRX();
    float BLX();
    float BRX();

    // -----------------
    void LoadProjection();
    void FOVSweep(mapSweepF* sweeps);
//    bool Translate(float x, float y);  // true if x,y (1x1x1 box) is visible on screen
};



/*=============================================================================
 * MAP DISPLAY
 */
class cellC;
void mdSetPlane(int plane);
void mdDisplay(void* selection, mapViewC& mapview);
void mdSwitchPlane();
void dbDisplay(cellC* cell, mapViewC& mapview);



#endif
//EOF =========================================================================
