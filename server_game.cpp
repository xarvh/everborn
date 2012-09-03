/*
 SERVER_GAME.CPP	v0.05
 serverCreateNewGameData()
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "main.h"
#include "txt.h"
#include "net.h"
#include "x_dir.h"

#include "game.h"
#include "race.h"
#include "map.h"





/*=============================================================================
 * LOAD ALL AVAILABLE RACES
 */
static char server_game_string[100];


void serverSetGameString(char* s)
{
 if(s) strncpy(server_game_string, s, sizeof(server_game_string));
 else server_game_string[0] = '\0';
}

char* serverGetGameString() { return server_game_string; }





/*=============================================================================
 * LOAD ALL AVAILABLE RACES
 */
static int serverLoadAllAvailableRaces()
{
 char fe[] = "serverLoadAllAvailableRaces: ";
 char ext[] = ".raz";
 char ebf[200];

 dtree_t *l, *d;
 raceC* r;
 char* s;
 int k;


// raceDestroyAll();


 // search for all .raz files
 l = dtree_list(ext);
 if(!l)
    {
	pe("%sno %s file found.\n", fe, ext);
	return -1;
    }


 // load any available race
 for(d = l; d; d = d->n)
    {
	// load the file
	s = txtLoad(d->s);
	if(s == NULL)
	    {
		pe("%serror reading %s.\n", fe, d->s);
		goto ERROR;
	    }


	// load the race
	r = raceLoad(s, ebf);
	free(s);
	if(r == NULL)
	    {
		pe("%s: %s\n", d->s, ebf);
		goto ERROR;
	    }


	// check for duplicated id
	// the new race is chained to last position, so if we find a race
	// with that ID earlier in the chain, it means it's duplicated
	for(k = 0; strcmp(raceGet(k)->Id, r->Id); k++);

	// was the last race?
	if(raceGet(k+1)) delete r;			// duplicated race, remove
	else pf("race: %s from %s.\n", r->Name, d->s);	// new race confirmed!!
    }

 // clear the tree
 dtree_free(l);
 return 0;

 ERROR:
 dtree_free(l);
// raceDestroyAll();
 return -1;
}





/*=============================================================================
 * CREATE NEW GAME DATA
 */
int serverCreateNewGameData()
{
 char fe[] = "serverCreateNewGameData: ";


 // load game file
 char* game_name = "everborn.game";
 char* game_fn = dataFindPath(game_name);
 if(!game_fn)
 {
    pe("%sunable to find game file `%s`.\n", fe, game_name);
    return -1;
 }

 char* game_data = txtLoad(game_fn);
 if(!game_data)
 {
    pe("%sunable to read game file `%s`.\n", fe, game_fn);
    goto ERROR;
 }

 pf("game file: %s.\n", game_fn);
 delete[] game_fn; game_fn = 0;




 // read game string
 int mapw, maph, seed;
 mapw = maph = seed = 0;
 sscanf(server_game_string, "%d %d %x", &mapw, &maph, &seed);

 if(mapw < 20 || mapw > 256) mapw = 20;
 if(maph < 20 || maph > 256) maph = 20;
 if(seed == 0) seed = time(0);

 // create game
 game = new gameC(-1, seed, mapw, maph);


 // read game info
 char ebf[200];
 if(game->Read(game_data, ebf))
 {
    pe("%s\n", ebf);
    goto ERROR;
 }


 // load races
 pf("loading races...\n", fe);
 if(serverLoadAllAvailableRaces()) goto ERROR;


 // count races
 int races;
 for(races = 0; raceGet(races); races++);


 // update game string
 sprintf(server_game_string, "%d %d %x %d", mapw, maph, seed, races);


 // done
 return 0;


 ERROR:
 if(game) delete game;
 if(game_fn) free(game_fn);
 if(game_data) free(game_data);
 return -1;
}



//EOF =========================================================================
