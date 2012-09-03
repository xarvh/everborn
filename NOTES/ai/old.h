/*





    join localhost
    ready
    pick


    usare un
	bool gui
    per decidere se la gui é attiva o no

*/
class aiC {
    aiC*	Next;

 public:
    void	Execute();	// si occupa della connessione e della base

 protected:
    aiC(char* name, char* descr);

    char*	Name;
    char*	Description;

    //
    brainC*	Self;

    gameC*	Game;
    brainC*	BrainsHook;

    unitC*	Units;
    unitC*	UnitsEnd;
    cellC*	Cells;
    cellC*	CellsEnd;

 private:
    virtual void AiPick() = 0;
    virtual void AiThink() = 0;
};





/*=============================================================================
 *
 */
#include <string.h>

static aiC* aiHook = 0;

void aiC::aiC(char* name, char* desc)
{
 Next = aiHook;
 aiHook = this;
 Name = name;
 Description = desc;

 Self = 0;
 Game = 0;
 BrainsHook = 0;
 Units = UnitsEnd = 0;
 Cells = CellsEnd = 0; 
}



aiC* aiGet(char* name)
{
 aiC* ai;
 for(ai = aiHook; ai && strcmp(ai->Name, name); ai = ai->Next);
 return ai;
}



aiC* aiGet(int id)
{
 for(aiC* ai = aiHook; ai; ai = ai->Next)
    if(id-- == 0) return ai;
 return 0;
}





/*=============================================================================
 *
 */
void aiC::Execute(bool serverHasUpdatedGameState)
{
 switch(Status) {
    case AI_STARTED:
	if(connected) Status++;
	else if(Lastcommand - timeGet() > 2000)
	    cmd("join localhost")
	    Lastcommand = timeGet();

    case AI_WAITING_PLAYERS:
	if(game) Status++;
	else
	    //if(playercount() > 1)
		if(Lastcommand - timeGet() > 2000)
		    cmd("ready");
		    Lastcommand = timeGet();
    case AI_PICK:
	if(Self->Status == BRAIN_ACTIVE) Status++;
	else if(Lastcommand - timeGet() > 2000)
	    AiPick();
	    Lastcommand = timeGet();

    case AI_GAME:
	if(serverHasUpdatedGameState)
	    AiThink();
 }
}










/*=============================================================================
 *
 */
class aiCustomC : aiC {
 public:
    aiCustomC() : aiC("Custom", "This is the first attempt at an AI"){}

 private:
    Pick();
    Think();
} aiCustom;





void aiCustomC::AiPick()
{
 //
 // put any initialization here
 //

 // select picks
 cmd("pick");
}




void aiCustomC::AiThink()
{
 // for any player 'b'
 for(brainC* b = BrainsHook; b; b = b->Next)
    ;

 // for any unit 'u'
 for(unitC* u = Units; u < UnitsEnd; u++)
    ;

 // for any map cell 'c'
 for(cellC* c = Cells; c < CellsEnd; c++)
    ;

 // for any city 'city'
 for(cellC* c = Cells; c < CellsEnd; c++) if(cityC* city = c->City)
    ;
}





//EOF
