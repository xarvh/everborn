/*
 BOXGUI.H		v0.00
 box gui, menu, menu elements.

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



    Button		ha una scritta, lancia una funzione
	Tab		seleziona una tra diverse opzioni
	Toggle		Accende/Spegne

    Window		contiene altri elementi eterogenei


    list		contiene altri elementi tutti uguali
			se non ci sono abbastanza elementi il suo spazio
			é usato in modo non omogeneo



*/
#ifndef H_FILE_BOXGUI
#define H_FILE_BOXGUI
//=============================================================================
#include "box.h"





#define BMENU_RATIO	(800. / 1280.)
#define BMENU_H		BMENU_RATIO
#define BDEF_BORDER	.015
#define BDEF_BUTTON_H	.06
#define BDEF_BUTTON_W	.25
#define BDEF_TAB_H	(.8*BDEF_BUTTON_H)






/*=============================================================================
 * BUTTON
 */
class bbC : public boxC { public:
    void	(*Call)();
    int		HotKey;

    //
    bbC(
	char* name,
	float x,
	float y,
	float w,
	float h,
	int hotkey,
	void (*call)());
};





/*=============================================================================
 * LIST
 */
class blistC : public boxC { public:
    int Off;
    int	Rows;
    int Cols;
    long* Select;
    long* Hover;
    boxInputF ItemInput;
    boxDisplayF ItemDisplay;
    boxDisplayF IdSelect;

    //
    blistC(int cols, int rows, long* sel, long* hover, boxDisplayF idi);
};





/*=============================================================================
 * TAB
 */
class btabC : public boxC { public:
    long* Targ;

    btabC(
	long id,
	long* targ,
	char* name,
	float x, float y,
	float w, float h);

};





/*=============================================================================
 * MENU
 */
#define bmenuDECLARE( menu_name )			\
    class __menu ## menu_name : public bmenuC {		\
	     public: __menu ## menu_name():		\
	     bmenuC( # menu_name)


#define bmenuINSTANCE	} bmenuINSTANCE;


class bmenuC : public boxSC { public:
    char*	(*StartF)();
    char*	(*CheckF)();
    char*	Fallback;
    void**	ProvidedSpellTargets;

    //
    bmenuC(char* name);
};





/*=============================================================================
 *
 */
void bGColor(float a);
void bTColor();
void bHColor();
void bClick(boxC& b);
void pText(boxC& b);
void bdrawWindowBackground(boxC& b, bool hl, double alpha);
void bdrawButtonBackground(boxC& b, bool dn);
void bdrawContour(boxC& b, bool highlight, bool down);
void bdrawName(boxC& b, char* name, bool highlight, bool down);
void bdrawBox(boxC& b, char* text, bool highlight, bool down);
void bdrawSelect(boxC& b, bool select, bool sv, bool sh);
void bbC_Display(boxC& b);
void btabDisplay(btabC& tab);
bool btabInput(btabC& tab);
void bwinDisplay(boxC& b);

void bLoadCoord();
bool bOpen(char* n);
void bInit();
void bInput();
void bDisplay();



/*=============================================================================
 *
 */
class unitC;
class cellC;
class brainC;
namespace popupUnitS { void Open(unitC*); void Draw(unitC*, double h);}
namespace popupHelpS { void Open(char*, char*); }
namespace popupYesNoS { void Open(const char* fmt, ...); }
namespace popupSettleS { int Open(); }
namespace popupBuildRoadS { int Open(float x, float w, cellC** targ); }
namespace popupResearchS { void Open(); }
namespace popupPlayerS { void Open(brainC*); }

#endif
//EOF =========================================================================
