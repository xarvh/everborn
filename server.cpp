/*
 SERVER.CPP		v0.15
 serverMain().

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
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "game.h"
#include "colors.h"
#include "net.h"
#include "cmd.h"





/*=============================================================================
 * GAME THINK
 * Will make everyone call gameThink().
 */
int serverGameThink()
{
 cmdSelectBrain(0);
 cmdFromClient("think");
}





/*=============================================================================
 * INIT PLAYER
 */
int serverInitPlayer()
{
 char bf[100];

 // serverThink() has already called cmdSelectBrain()
 sprintf(bf, "name %s", net_connections[cmdGetBrainId()].name);
 cmdFromClient(bf);

 // apply a color
 int c = time(0) % 8;
 while(!game->IsPlayerColorAvailable(colorGet(c))) c++;
 cmdGetBrain()->Color = colorGet(c);

 //@@ as chunks:
 //@@ send game string
 //@@ send map....

 // send all brains data to all brains
 for(brainC* b = game->Brains; b < game->BrainsEnd; b++) if(b->Status != BRAIN_UNUSED)
 {
    cmdSelectBrain(b->Id);

    sprintf(bf, "name %s", cmdGetBrain()->Name);
    cmdFromClient(bf);	// fake: send names as if they were sent by client!

    sprintf(bf, "color %s", cmdGetBrain()->Color->Name);
    cmdFromClient(bf);	// fake: send colors as if they were sent by client!
 }
}





/*=============================================================================
 * THINK
 */
static tempoC serverGameThinkTempo(100);

bool serverThink()
{
 connectionC* c;
 char* p;
 int brainID;

 // poll network
 if(netLoop() == -1) return false;

 // handle all incoming data
 for(c = net_connections; c < net_connections_end; c++) if(c->fast_p > c->fast_bf)
 {
    brainID = c - net_connections;
    cmdSelectBrain(brainID);

    if(cmdGetBrain()->Status != BRAIN_UNUSED)
	for(p = c->fast_bf; p = netStringShift(p, c); p += strlen(p) + 1)
	    cmdFromClient(p);	// check, execute & route
    else
    {
	// a new player awaits initialization
	serverInitPlayer();
	c->fast_p = c->fast_bf;
    }
 }



 // check that there are still players in the game
 // @@this check should be performed only by spawned server
 // @@ (not dedicated server)
 // @@ and only when a connection is lost
 int cnt = 0;
 for(c = net_connections; c < net_connections_end; c++)
    if(c->socket_safe != -1) cnt++;
 if(game->Brains->Status != BRAIN_UNUSED && !cnt)
 {
    printf("All players gone.\n");
    return false;
 }


 // time to close frame?
 if(game) if(serverGameThinkTempo()) serverGameThink();

 return true;
}





/*=============================================================================
 * SERVER MAIN LOOP
 */
void serverMainLoop()
{
 consoleSetProcName("sv");
 atexit(netReset);

 // set up listening
 if(netListen() == -1) exit(-1);

 // and prepare for a new game
 int serverCreateNewGameData();
 if(serverCreateNewGameData()) exit(-1);

 // loop
 while(serverThink());

 pf("quit.\n\n");

 // serverMainLoop() must NEVER return!!
 exit(0);
}





//EOF =========================================================================
