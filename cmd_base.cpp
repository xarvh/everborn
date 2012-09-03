/*
 CMD_BASE.CPP		v0.03
 Base (ie: system) commands.

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

#include "main.h"
#include "cmd.h"





/*=============================================================================
 * CLOSE
 *
 * REALLY USEFUL!?
 */
static int cmdFClose()	{ int mainCloseAll(); mainCloseAll(); }

static cmdC cmdClose(
    "close",		// name
    ":",		// syntax
    "close the game",
    cmdFClose,		// exec()
    NULL, NULL		// game() and check(), always unused here
);





/*=============================================================================
 * ECHO
 */
static int cmdFEcho()	{ pf("%s\n", arg1.s);		}

static cmdC cmdEcho(
    "echo",				// name
    "S: message",			// syntax
    "output 'message' to console",
    cmdFEcho,				// exec()
    NULL, NULL				// game() and check(), always unused here
);





/*=============================================================================
 * SET
 */
static int cmdFSet()
{
 char *c;
 int k;

 if(*arg1.s)
    if(*arg2.s)
	{
	    if(envSet(arg1.s, arg2.s)) pe("envSet: invalid variable `%s`.\n", arg1.s);
	}
    else
	if(c = envGet(arg1.s)) pf("%s=%s\n", arg1.s, c);
	else pe("envGet: invalid variable `%s`.\n", arg1.s);
 else
    for(k = 0; c = envGetName(k); k++) pf("%s=%s\n", c, envGet(c));
}

static cmdC cmdSet(
    "set",				// name
    "ns: variable [value]",		// syntax
    "sets environment variables",
    cmdFSet,				// exec()
    NULL, NULL				// game() and check(), always unused here
);





/*=============================================================================
 * EXEC
 */
static int cmdFExec()
{
 cmdFile(arg1.s);
}

static cmdC cmdExec(
    "exec",				// name
    "s: filename",			// syntax
    "executes the commands in a file",
    cmdFExec,				// exec()
    NULL, NULL				// game() and check(), always unused here
);





/*=============================================================================
 * QUIT
 */
static int cmdFQuit()	{ pf("quit.\n"); exit(0); }

static cmdC cmdQuit(
    "quit",			// name
    ":",			// syntax
    "quit the game",
    cmdFQuit,			// exec()
    NULL, NULL			// game() and check(), always unused here
);



//EOF =========================================================================
