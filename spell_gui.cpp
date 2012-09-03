/*
  SPELL_GUI.CPP		v0.00

 * Once the spell to launch has been determined (and thus, the type of target
 * required for it) it's up to the menu to provide the target.
 * Target must be provided as a string to pass to spellGuiDo().
 * If menu is incapable of providing the target, it can either switch to a more
 * appropriate menu or to the default menu for that target type.
 */

#include <GL/gl.h>
#include "fnt.h"
#include "main.h"
#include "boxgui.h"

#include "spell.h"
#include "game.h"
#include "cmd.h"


//@@ charge and temporary vs permanent button





/*=============================================================================
 * LOCALS
 */

// this is used to collect all parameters (exept spell target) and obtain
// costs/turns and all other useful member of spellLaunchC.
static spellLaunchC spellGuiLaunch;

// this is compared with spellGuiLaunch.Spell:
// if it's different, spellGuiLaunch is reset to default values.
//
// this is made to remember casting parameters when a spell is repeated.
static spellC* spellPrev = 0;





/*=============================================================================
 * HELPERS / WRAPPER
 *
 * Used by menues to check if and what kind of target they are to provide.
 */
spellC* spellGui() { return spellGuiLaunch.Spell; }



#define TTYPE(i) { return spellGui() && spellGui()->Type == &i; }
// (default)
// ?not sure about this
// !direct, no GUI actually required
bool spellGuiUnit()	TTYPE(spellTypeUnit)	// (land), battle, ?city
bool spellGuiCity()	TTYPE(spellTypeCity)	// (land), !city, !prod, summary

//bool spellGuiMap()	TTYPE(spellTypeMap)	// (land), !battle
//bool spellGuiWizard()	TTYPE(spellTypeWizard)	// (diplomacy)
bool spellGuiBattle()	TTYPE(spellTypeBattle)	// (land), !battle
//bool spellGuiArea() 	TTYPE(spellTypeBattleArea)	// (battle)
//bool spellGuiNode()	TTYPE(spellTypeNode)	// (land), !battle
//bool spellGuiWall()	TTYPE(spellTypeWall)	// (battle)




static bool spellsUnavailable()
{
 if(!gameIsAvailable()) return true;
 if(!self) return true; //@@ required?
 if(!self->Race) return true;
 return false;
}




/*=============================================================================
 * STATUS CONTROL / EXECUTION
 */

// returns the menu to normal status
void spellGuiClose() { spellGuiLaunch.Spell = 0; }



// cancels the spell casting
void spellGuiCancel() { spellGuiClose(); }



// launches the spell command, with the obtained target
// returns the menu to normal status
void spellGuiDo(int target)
{
 if(!spellGui()) return;

 bool temp = spellGui()->HasTemporaryVersion() && spellGuiLaunch.Temp;
 cmd("%s%s %d %d",
    temp?"-":"",
    spellGui()->Cmd.Name,
    target,
    spellGuiLaunch.Charge);

 spellGuiClose();
}

void spellGuiDo(int targX, int targY, int targZ)
{
 if(!spellGui()) return;

 bool temp = spellGui()->HasTemporaryVersion() && spellGuiLaunch.Temp;
 cmd("%s%s %d %d %d %d",
    temp?"-":"",
    spellGui()->Cmd.Name,
    targX, targY, targZ,
    spellGuiLaunch.Charge);

 spellGuiClose();
}





/*=============================================================================
 * SPELL COMMAND
 */
static int cmdFCast()
{
 if(spellsUnavailable()) return -1;

 // set the gui status:
 spellGuiLaunch.Spell = (spellC*)arg1.p;

 // check if gui is actually required
 if(!spellGui()->Type->DefaultMenu)
    spellGuiDo(0);	// no targ required, launch at once!
}



static cmdC cmdCast(
    "cast",
    "E: SpellName",
    "summon spell GUI",
    cmdFCast,
    0, 0
);





/*=============================================================================
 * LIST
 */
static void spellGuiListDisplay(boxC& b)
{
 spellC* s = spellGui();
 bwinDisplay(b);

 fntS(0, .05, .98);
 bTColor();
 glTranslated(.01, b.H, 0);

 for(spellLaunchC* l = self->SpellQueue; l; l = l->Next)
    fntL("\n%s, ETA %d", l->Spell->Name, l->TurnsRequired());
}



/*=============================================================================
 * TARG
 */
static void spellGuiTargDisplay(boxC& b)
{
 spellC* s = spellGui();
 bwinDisplay(b);

 fntS(0, .05, .98);
 bTColor();
 glTranslated(.01, b.H, 0);
 fntL("\ncasting ?H%s?/: select ?Htarget %s?/!", s->Name, s->Type->Name);
 fntL("\nmagic power cost: %d", spellGuiLaunch.Cost());

 if(int t = spellGuiLaunch.TurnsRequired())
    fntL("\nabout %d turns required", t);
}





static bool spellGuiTargInput(boxC& box)
{
 if(inputK() == 27)
 {
    spellGuiCancel();
    return true;
 }

 return false;
}





/*=============================================================================
 *
 */
void spellGuiCheckMenu(void** arg)
{
 if(!spellGui()) return;
 spellTypeC** list = (spellTypeC**)arg;

 while(list && *list)
    if(*list == spellGui()->Type) return;
    else list++;

 if(bOpen(spellGui()->Type->DefaultMenu))
    spellGuiCancel();
}





/*=============================================================================
 * BOOK
 */
#define BOOK_X		(.1)
#define BOOK_Y		(.1)
#define BOOK_W		(1-.2)
#define BOOK_H		(BMENU_H-.2)

#define PAGE_ROWS	5
#define PAGE_S		.004

#define LPAGE_X		BOOK_X
#define LPAGE_Y		BOOK_Y
#define LPAGE_W		(BOOK_W/2 - PAGE_S/2)
#define LPAGE_H		BOOK_H

#define RPAGE_X		(BOOK_X + BOOK_W/2 + PAGE_S/2)
#define RPAGE_Y		BOOK_Y
#define RPAGE_W		LPAGE_W
#define RPAGE_H		BOOK_H




bool spellTypeMatch(spellTypeC* a, spellTypeC* b)
{
 if(!a || !b) return true;

 //@@ just that for now, grouping shall come soon
 return a == b;
}





namespace spellGuiBookS {

 spellTypeC* ForceTargetType = 0;
 int TargX, TargY, TargZ;	// (targX == -1) => no targ specifyed

 long SpellH;

 int Page = 0;			// yes, is a spellbook! page starts with 0!! =)
 bool Open = false;



 spellC* SpellExId(int id)
 {
    id += Page*PAGE_ROWS*2;

    bool battle = self->Focus && self->Focus->Battle;

    for(spellC* s = spellsHook; s; s = s->SpellNext)
	if((s->BattleCost && battle) || (s->OverlandCost && !battle))
	    if(spellTypeMatch(s->Type, ForceTargetType))
		if(self->CanCast(s))
		    if(id-- == 0)
			return s;

    return 0;
 }





 bool SpellInput(boxC& b)
 {
    if(!b.Mouse || !mouseL()) return false;
    spellC* s = SpellExId(b.Id);
    if(!s) return false;
    bClick(b);

    Open = false;
    cmd("cast %s", s->Cmd.Name);

    return true;
 }

 void SpellDisplay(boxC& b)
 {
    spellC* s = SpellExId(b.Id);
    if(!s) return;

    bHColor();
    fntS(.1);
    glTranslated(.01, b.H, 0);
    fntL("\n%s", s->Name);

    bTColor();
    fntS(.05);
    fntL("\n%s", s->OneLiner);

    spellLaunchC sp;
    sp.Caster = self;
    sp.Spell = s;
    int tr = sp.TurnsRequired();
    char bf[200];
    char* w = bf;

    w += sprintf(w, "\n%d?M1", sp.Cost());
    if(sp.Temp) w += sprintf(w, ", temporary");
    else if(tr > 1)	w += sprintf(w, ", cast in %d turns", tr);
    else if(tr == 0)	w += sprintf(w, ", cast immediatly");
    else if(tr == 1)	w += sprintf(w, ", cast next turn");
    if(!sp.Temp && s->UpkeepCost)
	w += sprintf(w, ", upkeep: ?M%d", s->UpkeepCost);

    fntL(bf);
 }



 bool Input(boxC& b)
 {
    switch(inputK()) {
	case 27:
	case ' ':
	    Open = false;
	    return true;
    }

    if(b.Mouse && inputE() == INPUT_MUP)
    {
	if(SpellExId((Page+1)*PAGE_ROWS*2)) Page++;
	return true;
    }

    if(b.Mouse && inputE() == INPUT_MDN)
    {
	if(Page > 0) Page--;
	return true;
    }

    return false;
 }



 void Display(boxC& b)
 {
    bLoadCoord();
 }


 void DisplayBk(boxC& b)
 {
    bdrawButtonBackground(b, true);
    bHColor();
    fntS(.05);
    glTranslated(.5, -.05, 0);
    bool bwd = Page > 0;
    bool fwd = SpellExId((Page+1)*PAGE_ROWS*2);
    fntC("%s %d %s", bwd?"<":" ", Page, fwd?">":" ");
 }


 void Add(boxSC& p)
 {
    p.InputF = Input;
    p.DisplayF = Display;

    char* name = "Spellbook";
    char* help =
	"This is the list of spells you can cast."
	"\nUse the mouse wheel to flip pages.";

    p.AddList(1, PAGE_ROWS,
	0, &SpellH, name, help,
	LPAGE_X, LPAGE_Y, LPAGE_W, LPAGE_H,
	SpellInput, SpellDisplay);

    p.AddList(1, PAGE_ROWS,
	0, &SpellH, name, help,
	RPAGE_X, RPAGE_Y, RPAGE_W, RPAGE_H,
	SpellInput, SpellDisplay)->Off = PAGE_ROWS;

    p.AddWindow(0,0, BOOK_X, BOOK_Y, BOOK_W, BOOK_H, 0, DisplayBk);
 }
}



void spellGuiOpenBook()
{
 spellGuiBookS::Open = true;
}

void spellGuiOpenBook(spellTypeC* ft, int t)
{
}

void spellGuiOpenBook(spellTypeC* ft, int tx, int ty, int tz)
{
}





/*=============================================================================
 *
 */
static boxC	spellGuiMain;
static boxSC&	spellGuiTarg = spellGuiMain.DefaultS;
static boxSC	spellGuiList(spellGuiMain, 0);
static boxSC	spellGuiBook(spellGuiMain, 0);



static boxSC* spellGuiSelect(boxC& b)
{
 if(spellsUnavailable())
    return 0;

 if(spellGuiBookS::Open)
    return &spellGuiBook;

 spellC* s = spellGui();
 if(!s) return &spellGuiList;

 // reset launch parameters
 spellLaunchC* sl = &spellGuiLaunch;
 if(sl->Spell != spellPrev)
 {
    spellPrev = sl->Spell;
    sl->Caster = self;
    sl->Charge = 0;
    sl->Temp = (self->Focus && self->Focus->Battle);
 }

 return &spellGuiTarg;
}







boxC& spellGuiInit()
{
 // main
 spellGuiMain.Wg = 1;
 spellGuiMain.Hg = BMENU_H;
 spellGuiMain.SelectF = spellGuiSelect;


 // book
 spellGuiBookS::Add(spellGuiBook);


 // targ
 float x = BDEF_BORDER+BDEF_BUTTON_W+.01;
 float h = .1;
 spellGuiTarg.AddWindow("Spell target selection",
    ".....",
    x, BMENU_H-h-.01,
    1-.22 - x - BDEF_BORDER,
    h, spellGuiTargInput, spellGuiTargDisplay);

 spellGuiList.AddWindow("Casting Queue",
    ".....",
    x, BMENU_H-h-.01,
    1-.22 - x - BDEF_BORDER,
    h, 0, spellGuiListDisplay);


 //
 return spellGuiMain;
}





/*=============================================================================
 * SYNTAX
 */
static char* cmd_spellSyntax(argT* arg, char* s)
{
 while(*s == ' ') s++;
 if(!*s)
 {
    sprintf(arg->s, "no spell specifyed", s);
    return 0;
 }

 arg->p = spellGet(s);
 if(!arg->p)
 {
    sprintf(arg->s, "unknown spell '%s'", s);
    return 0;
 }

 while(*s && *s != ' ') s++;
 return s;
}

static cmdSyntaxC syntaxReSpell('E', cmd_spellSyntax);



//EOF =========================================================================
