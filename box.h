/*
 BOX.H		v0.00
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
#ifndef H_FILE_BOX
#define H_FILE_BOX
/*=============================================================================
 * LINK to SUB BOX
 *
 * Each boxSC is divided in many sub boxes.
 *
 * The link carries only a pointer to a sub box because any
 * given sub box may be used by several different boxSC.
 * For the same reason, the link carries also the X and Y for the sub box.
 */
class boxC;
class boxSC;
class boxLC { public:
    boxLC*	Next;

    boxC&	SubBox;
    float	Xg;
    float	Yg;

    //
    boxLC(boxC& box);
    boxLC(boxSC& b, boxC& box, float x, float y);
};





/*=============================================================================
 * BOX STATUS
 *
 * Each box can appear in different ways due to context and have different
 * behaviour accordingly.
 * Each status is described by a boxSC, that holds information about how
 * to display, how to handle input, and about sub boxes.
 */
class boxC;
class blistC;
typedef bool (*boxInputF)(boxC&);
typedef void (*boxDisplayF)(boxC&);

class boxSC { public:
    boxSC* Next;

    char* Name;
    char* Help;
    boxC& Box;
    boxLC* SubBoxesHook;	// subboxes, starting with lowest priority

    boxDisplayF	DisplayF;
    boxInputF	InputF;		// must return true if kes/buttons are used


    //
    void Display();
    bool Input();

    // alternate chain is used when 'boxC& box' is not guaranteed
    // to be initialized already.
    boxSC(boxC& box, boxSC** alternate_chain);


    // base Sub
    boxLC* AddBox(boxC& box);
    boxLC* AddBox(boxC& box, float x, float y);

    // other GUI Subs
    boxC* AddButton(char* name, char* help,
	float x, float y, float w, float h,
	int hotkey, void (*call)());

    boxC* AddTab(
	long id, long* targ, char* name, char* help,
	float x, float y, float w, float h);

    boxC* AddWindow(char* name, char* help,
	float x, float y,
	float w, float h,
	boxInputF input, boxDisplayF display);

    blistC* AddList(int cols, int rows,
	long* sel, long* hov,
	char* name, char* help,
	float x, float y,
	float w, float h,
	boxInputF iin, boxDisplayF idi);
};



/*=============================================================================
 * BOX
 *
 * This is the main structure, holds information on the geometry and on
 * how to choose among the possible states.
 */
class boxSC;
typedef boxSC* (*boxSelectF)(boxC&);

class boxC { public:
    char*	Name;
    long	Id;

    boxLC	DefaultL;
    boxSC	DefaultS;
    boxSC*	CurrentS;
    boxSelectF	SelectF;

    // geometry
    float	Wg;
    float	Hg;

    // internal height
    float	H;

    // mouse motion
    bool	Mouse;
    float	MouseX;
    float	MouseY;

    //
    boxC();

    void	Display(float x, float y, float mouseX, float mouseY);
    bool	Input();
};



bool boxHelp(char* n, char* h);



#endif
//EOF =========================================================================
