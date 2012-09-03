/*

*/
#include <string.h>
#include "main.h"
#include "game.h"
#include "net.h"
#include "cmd.h"
#include "ai.h"





/*=============================================================================
 *
 */
aiC* aiCurrent = 0;



char* aiCall(char* v)
{
 aiCurrent = aiGet(v);

 if(*v && !aiCurrent)
    pe("invalid ai: '%s'\n", v);

 return v;
}
static envC aiEnv("ai", "", aiCall);



void aiExecute(bool server_upd)
{
 if(aiCurrent)
    aiCurrent->Execute(server_upd);
}





/*=============================================================================
 *
 */
static aiC* aisHook = 0;

aiC::aiC(char* name, char* desc, aiF think, aiF pick)
{
 // check duplicate names
 for(aiC* ai = aisHook; ai; ai = ai->Next)
    if(!strcmp(ai->Name, name)) throw "duplicate AI name";

 // add to chain
 Next = aisHook;
 aisHook = this;

 // init
 Name = name;
 Description = desc;
 Think = think;
 Pick = pick;
}



aiC* aiGet(char* name)
{
 aiC* ai;
 for(ai = aisHook; ai && strcmp(ai->Name, name); ai = ai->Next);
 return ai;
}



aiC* aiGet(int id)
{
 for(aiC* ai = aisHook; ai; ai = ai->Next)
    if(id-- == 0) return ai;
 return 0;
}





/*=============================================================================
 *
 */
static int aiStatus = 0;
static long aiLastCommand = 0;



void aiC::Execute(bool serverHasUpdatedGameState)
{
 switch(aiStatus) {


    // just started, need to connect with server
    case 0:
	if(netConnected()) aiStatus++;
	else if(timeGet() - aiLastCommand > 2000)
	{
	    pf("ai %s, trying to connect:\n", Name);
	    cmd("name AI-%s", Name);
	    cmd("join localhost");
	    aiLastCommand = timeGet();
	}
    break;



    // waiting for other players to join
    case 1:
	if(game && game->Turn > 0) aiStatus++;
	else
	    //if(playercount() > 1)
		if(timeGet() - aiLastCommand > 2000)
		{
		    cmd("ready");
		    aiLastCommand = timeGet();
		}
    break;



    // choose picks
    case 2:
	if(self->Status == BRAIN_ACTIVE) aiStatus++;
	else if(timeGet() - aiLastCommand > 2000)
	{
	    Pick();
	    aiLastCommand = timeGet();
	}

    // play
    case 3:
	if(serverHasUpdatedGameState)
	    Think();
    break;
 }
}





//EOF =========================================================================
