/*
 BOX.CPP		v0.00
 box interface, lower level

 Copyright (c) 2004-2008 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
//#include "main.h"

/*
 DA PROVARE:
    -i tasti del rightpanel
    -le linee dei counters
*/

#include <GL/gl.h>
#include "box.h"




/*=============================================================================
 * LINK, BOXS, BOX
 */

boxLC* boxSC::AddBox(boxC& box)
{
 // can we use DefaultL?
 if(box.DefaultL.Next != &box.DefaultL) throw "default link only";

 box.DefaultL.Next = SubBoxesHook;
 SubBoxesHook = &box.DefaultL;
 return &box.DefaultL;
}


boxLC* boxSC::AddBox(boxC& box, float x, float y)
{
 // can we use DefaultL?
 if(box.DefaultL.Next != &box.DefaultL)
    return new boxLC(*this, box, x, y);

 box.DefaultL.Xg = x;
 box.DefaultL.Yg = y;
 box.DefaultL.Next = SubBoxesHook;
 SubBoxesHook = &box.DefaultL;
 return &box.DefaultL;
}





boxLC::boxLC(boxC& box)
: SubBox(box)
{
 Xg = 0;
 Yg = 0;
 Next = this;
}

boxLC::boxLC(boxSC& bs, boxC& box, float x, float y)
: SubBox(box)
{
 Xg = x;
 Yg = y;

 Next = bs.SubBoxesHook;
 bs.SubBoxesHook = this;
}



boxSC::boxSC(boxC& box, boxSC** alternate_chain)
    : Box(box)
{
    Next = 0;
    Name = 0;
    Help = 0;
    SubBoxesHook = 0;
    DisplayF = 0;
    InputF = 0;

    // chain
    if(this != &Box.DefaultS)
    {
	boxSC** h = (alternate_chain)?alternate_chain : &Box.DefaultS.Next;
	while(*h) h = &(*h)->Next;
	*h = this;    
    }
}



boxC::boxC()
    : DefaultS(*this, 0), DefaultL(*this)
{
    Name = 0;
    Id = 0;

    CurrentS = &DefaultS;
    SelectF = 0;

    Wg = 1;
    Hg = 1;
    H = 1;

    Mouse = false;
    MouseX = -1;
    MouseY = -1;
}





/*=============================================================================
 * INPUT
 */
bool boxSC::Input()
{
 // check sub boxes input, reverse order
 if(SubBoxesHook)
    for(boxLC *l, *n = 0; n != SubBoxesHook; n = l)
    {
	for(l = SubBoxesHook; l->Next != n; l = l->Next);
	if(l->SubBox.Input()) return true;
    }

 // check your own input
 if(InputF && InputF(Box))
    return true;


 // help?
 if(Name && Help && Box.Mouse)
    if(boxHelp(Name, Help))
	return true;


 // no input used
 return false;
}



bool boxC::Input()
{
 // skip input if inactive
 if(CurrentS)
    if(CurrentS->Input())
	return true;

 return false;
}





/*=============================================================================
 * DISPLAY
 */
void boxSC::Display()
{
 // do your own display
 if(DisplayF)
    DisplayF(Box);

 // display sub boxes
 for(boxLC* l = SubBoxesHook; l; l = l->Next)
    l->SubBox.Display(l->Xg, l->Yg, Box.MouseX, Box.MouseY);
}



void boxC::Display(float x, float y, float mouseX, float mouseY)
{
 H = Hg/Wg;
 MouseX = (mouseX-x) / Wg;
 MouseY = (mouseY-y) / Wg;
 Mouse = MouseX>=0 && MouseX<1 && MouseY>=0 && MouseY<H;


 // select current status with SelectF()
 if(SelectF)
    CurrentS = SelectF(*this);

 // skip display if inactive
 if(CurrentS)
 {
    glPushMatrix();
	glTranslated(x, y, 0);
	glScaled(Wg, Wg, 1);
	CurrentS->Display();
    glPopMatrix();
 }
}





//EOF =========================================================================
