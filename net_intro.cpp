/*
 NET_INTRO.CPP		v0.02
 Intro strings io.

 Copyright (c) 2004-2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "game.h"
#include "race.h"
#include "net.h"





/*=============================================================================
 * SERVER INTRO STRING
 * Format is:
 *
 *	[game string]#[player id]@[env: game]
 *
 */
int net_introServerWrite(char* s, int playerID)
{
 char* serverGetGameString();
 sprintf(s, "%s#%d@%s", game_string, playerID, serverGetGameString());
}



int net_introServerRead(char *s)
{
 char *c, *n;
 long self_id;

 // check '#' //
 c = s + strlen(game_string);
 if(*c != '#') return -1;
 *c++ = '\0';

 // check game string //
 if(strcmp(s, game_string)) return -1;


 // read player ID //
 self_id = strtol(c, &n, 10);
 if(n == c) return -1;
 if(self_id < 0 && self_id > NET_MAX_CONNECTIONS) return -1;

 // check '@' //
 if(*n++ != '@') return -1;


 // read races count and seed
 int races_cnt, seed, mapw, maph;
 if(4 > sscanf(n, "%d %d %x %d", &mapw, &maph, &seed, &races_cnt)) return -1;
 if(races_cnt < 1) return -1;


 // we have a game!!!!
 game = new gameC(self_id, seed, mapw, maph);


 // request information!
 void gameRequest();
 gameRequest();

 void raceRequestAll(int);
 raceRequestAll(races_cnt);

 return 0;
}





/*=============================================================================
 * CLIENT INTRO STRING
 * Format is:
 *
 *	[game string]#[player name]
 *
 */
int net_introClientWrite(char *s)
{
 sprintf(s, "%s#%s", game_string, envGet("name"));
}



char* net_introClientRead(char *s)
{
 char *n, *c;

 // check '#'
 n = s + strlen(game_string);
 if(*n != '#') return NULL;
 *n++ = '\0';

 // check game string
 if(strcmp(s, game_string)) return NULL;

 // check player name
 n[31] = '\0';				// ensure termination
 if(strlen(n) == 0) return NULL;	// ensure non-empty string

 if(*n == ' ') return NULL;		// cannot start with space

 for(c = n; *c; c++)			// ensure valid characters
    if(*c < ' ' || *c >= '~') return NULL;

 c = (char*)malloc(strlen(n) + 1);
 if(c == NULL) return NULL;

 strcpy(c, n);
 return c;
}





//EOF =========================================================================
