/*
 SESSION.CPP		v0.15
 Game creation/join

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

#include <unistd.h>
#include "main.h"
#include "game.h"
#include "net.h"
#include "boxgui.h"
#include "cmd.h"



/*=============================================================================
 * JOIN
 * Join a game.
 */
int sessionJoin(char* join)
{
 char* address = join? join : (char*)"localhost";

 // shut current game and network
 void mainCloseAll();
 mainCloseAll();

 // connect to server
 // calls gameSetSelf() too
 if(netConnect(address) == -1)
 {
    // if we're hosting a game, (ie, 'join' is 0) and the spawn failed,
    // it's better to inform the user.
    if(!join)
    {
	pe("Could not spawn server. Check that:\n");
	pe(" 1. all game files are correct. Try running 'everborn dedicated' and see the errors.\n");
	pe(" 2. no other everborn servers are running. Try 'killall -9 everborn'.\n");
    }

    return -1;
 }

 if(net_connections->requests)
    chclSendRequest(net_connections, net_connections->requests);


 // enter init menu
 bOpen("multi");

 return 0;
}



static int cmd_join()	{ sessionJoin(arg1.s); }
static cmdC cmdJoin(
    "join",				// name
    "N: address",			// syntax
    "join a game",
    cmd_join,				// exec()
    NULL,
    NULL
);





/*=============================================================================
 * HOST
 * Creates a new game.
 */
int sessionHost(char* game_str)
{
 // the game string contains all the paramters for a new game
 void serverSetGameString(char*);
 serverSetGameString(game_str);


 // spawn server
 pf("spawning server...\n");
 scrsndClose();
 if(!fork())		// child is server
     serverMainLoop();	// serverMainLoop() never returns
 scrsndInit();


 // from now on, you are a client with nothing special!
 return sessionJoin(0);
}



static int cmd_host()	{ sessionHost(arg1.s); }
static cmdC cmdHost(
    "host",				// name
    "S: game string",			// syntax
    "create a new game",
    cmd_host,				// exec()
    NULL,
    NULL
);





//EOF =========================================================================
