/*
 INIT.CPP		v0.15
 Initialization.

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


#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "cmd.h"




/*=============================================================================
 * RC
 *
 * Executes:
 *	$(HOME)/.everbornrc
 *	everbornrc
 */
int initRC()
{
 // home
 char bf[1000];
 strncpy(bf, getenv("HOME"), sizeof(bf));
 strncat(bf, "/.everbornrc", sizeof(bf)-strlen(bf)-1);
 if(!access(bf, R_OK)) cmdFile(bf);

 // local
 if(!access("everbornrc", R_OK)) cmdFile("everbornrc");

 return 0;
}





/*=============================================================================
 * NAME
 */
static char* initName(char* v)
{
 if(strlen(v) == 0)
    if(char* c = getenv("USER")) return c;
    else return "player";
 else if(strlen(v) > 32) v[31] = '\0';

 //@@ check for valid chars?
 return v;
}

static envC envName("name", "", initName);





//EOF =========================================================================
