/*
 CMD_EXEC.C		v0.14
 Core command execution, higher level cmd functions and ext cmd syntax.

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
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "game.h"
#include "cmd.h"
#include "net.h"





/*=============================================================================
 * LAST TRANSMIT
 * Holds the game time of the last call to cmdRoute()
 */
static unsigned long int last_transmit;





/*=============================================================================
 * BRAIN SELECTION
 * These variables select the targets of all game commands.
 * Only cmd() and cmdReadBrain() will change this value.
 */
static unsigned	cmdSelectedBrain;

brainC* cmdGetBrain()		{ return game->Brains + cmdSelectedBrain; }
int cmdGetBrainId()		{ return cmdSelectedBrain;	}

int cmdSelectBrain(int b)	{ cmdSelectedBrain = b;		}





/*=============================================================================
 * BRAIN ID TO CHAR
 * CHAR TO BRAIN ID
 */
int cmdBrainIdToChar(int id)
{
 if(id == 0xff) return '*';

 if(id < 10) return '0' + id;
 return 'A' + (id-10);
}

int cmdCharToBrainId(char c)
{
 if(c >= '0' && c <= '9') return c - '0';
 if(c >= 'A' && c - 'A' + 10 < GAME_MAX_BRAINS) return c - 'A' + 10;
 return 0xff;
}





/*=============================================================================
 * SEND TO SERVER
 * Sends a cmd to server.
 */
int cmd_execSendToServer(char *bf)
{
 if(netConnected()) net_fastSend(net_connections, bf);
}





/*=============================================================================
 * ROUTE
 * Prepares a command string for routing (ie adds brainID) and
 * sends it to all clients.
 */
static int cmdRoute(char *s)
{
 char bf[CMD_MAX_LENGHT+10];

 sprintf(bf, "%c: %s", cmdBrainIdToChar(cmdGetBrainId()), s);
 net_fastSendAll(bf);
}





/*=============================================================================
 * READ BRAIN
 * Selects brain specifyed in command string s.
 */
static char *cmdReadBrain(char *s, char *fe)
{
 // read brain id, and check it //
 cmdSelectBrain(cmdCharToBrainId(*s));
//@@debug if(cmdGetBrainId() == 0xff) goto ERROR;
 s++;

 // ": " expected //
 if(strncmp(": ", s, 2)) goto ERROR;
 s += 2;

 return s;


 ERROR:
 pe("%sno valid BrainId in packet.\n", fe);
 return 0;
}





/*=============================================================================
 * EXEC REMOTE
 * Executes a remote command, ie a command from the network.
 */
static int cmdExecRemote(cmdC* c)
{
 char fe[] = "cmdExecRemote: ";

 if(!c->Game)
    {
	pe("%s`%s` is not a game command.\n", fe, arg0.s);
	return -1;
    }

 // assuming CURRENT BRAIN is already correctly set // 
 c->Game();		// execute command locally //
 return 0;
}





/*=============================================================================
 * FROM SERVER
 * Used by client to process incoming commands from server.
 */
int cmdFromServer(char *s)
{
 char fe[] = "cmdFromServer: ";
 cmdC* c;

 s = cmdReadBrain(s, fe);	if(!s) return -1;
 c = cmdInterprete(s, fe);	if(!c) return -1;
 if(cmdExecRemote(c)) return -1;	// exec as remote command //
 return 0;
}





/*=============================================================================
 * FROM CLIENT
 * cmd version used by server to process incoming commands from client.
 */
int cmdFromClient(char *s)
{
 char fe[] = "cmdFromClient: ";
 cmdC* c;

 c = cmdInterprete(s, fe);	if(!c) return -1;
 if(c->Check && c->Check())
 {
    pe("%serror, cmd ignored.\n", fe);
    return -1;
 }

 if(cmdExecRemote(c)) return -1;	// exec as remote command //

 // route the command to all clients //
 cmdRoute(s);

 return 0;
}





//EOF =========================================================================
