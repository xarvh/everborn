/*
 SKE.CPP		v0.02
 Skeletal animations.
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


 TODO:
 Add a prev frame interpolation?
 Would require a status with:
    skeFrameT	*actualF, *prevF;
    double	actualT, prevT;

 How to handle timing between the two frames?

*/

#include <string.h>
#include "ske.h"



/*=============================================================================
 * UNIT ANIMATIONS
 */
static animationC Walk("walk",
    //D TP  TR    Lx Lz Le Lh   Rx Rz Re Rh    LH   LK  LF   RH   RK  RF 
     "4 tp   0   :hl sz ee  0    0 sz ee  0    hl -2hl  hl   hu -2hu   0 \n"
     "3 tp  tr     0 sz ee  0  :hl sz ee  0     0 -1hu  hl   hu -1hu   0 \n"
     "4 tp   0   -hl sz ee  0    0 sz ee  0    hu -2hu   0   hl -2hl  hl \n"
     "3 tp -tr     0 sz ee  0  -hl sz ee  0    hu -1hu   0    0 -1hu  hl \n"
);

static animationC Idle("idle",
    //D TP  TR    Lx Lz Le Lh   Rx Rz Re Rh    LH   LK  LF   RH   RK  RF 
     "4 tp   0     0 sz :ee 0    0 sz ee 1      3   -5   3    3   -5   1 \n"
     "1 tp :tr     0 sz :ee 0    0 sz ee 0      3   -5   1    3   -5   1 \n"
     "4 tp :tr     0 sz :ee 0    0 sz ee 0      3   -5   1    3   -5   1 \n"
     "4 tp   0     0 sz :ee 0    0 sz ee 0      3   -5   1    3   -5   3 \n"
     "1 tp -:tr    0 sz :ee 0    0 sz ee 0      3   -5   1    3   -5   1 \n"
     "4 tp -:tr    0 sz :ee 0    0 sz ee 0      3   -5   1    3   -5   1 \n"
);

static animationC Strike("strike",
    //D TP  TR    Lx Lz Le Lh   Rx Rz Re Rh    LH   LK  LF   RH   RK  RF 
    "1 tp   0     0  5  ee 0   110 11 50 22    -5   -5   3  :hl -:hl   1 \n"
    "2 tp  30     0 50 :ee 0     0 11  0  0  -:hl -:hl   1  :hu -:hu   1 \n"
);

static animationC Arrow("arrow",
    //D TP  TR    Lx Lz Le Lh   Rx  Rz  Re Rh    LH   LK  LF   RH   RK  RF 
    "5  0  40     80 50  0 0   150  20 160 22  -:hl -:hl   3   hl  -hl   1 \n"
    "9  0  40     80 50  0 0    80 -50   0  0  -:hl -:hl   1   hl  -hl   1 \n"
    "1  0  40     80 50  0 0    30 -10  50  0  -:hl -:hl   1   hl  -hl   1 \n"
    "3  0  40     80 50  0 0    30  20  50  0  -:hl -:hl   1   hl  -hl   1 \n"
);

static animationC Die("die",
    //D TP  TR    Lx Lz Le Lh   Rx Rz Re Rh    LH   LK  LF   RH   RK  RF 
     "1 tp   0    90 90  ee 0  -hl sz :ee 1    90    0   0   90    3   1 \n"
     "1 -90 :tr   90 90 :ee 0   hl sz :ee 0    90    0  10   90    2   1 \n"
);





/*=============================================================================
 *
 */
double skeValue(skeValueT *v, double *parms)
{
 if(v->parm_id < PARMS_CNT)
    return v->constant * parms[v->parm_id];
 return v->constant;
}




double animationC::Status(double *parms, double time, int jID)
{
 skeFrameT *f;
 skeValueT *v;

 double v0, vf;

 for(f = FirstFrame; time > f->duration; f = f->next) time -= f->duration;

 v0 = skeValue(f->joint + jID, parms);
 vf = skeValue(f->next->joint + jID, parms);

 return v0 + time * (vf - v0) / f->duration;
}





/*=============================================================================
 * HANDLE
 */
static animationC* animationsHook = 0;


animationC* animationC::Get(char* name)
{
 animationC* a;
 for(a = animationsHook; strcmp(a->Name, name); a = a->Next);
 return a;
}


void animationC::Hang()
{
 animationC** h;

 for(h = &animationsHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;
}





//EOF =================================================================
