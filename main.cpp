/*
 MAIN.CPP		v0.22
 main() and some globals.

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
#include <errno.h>

#include "cmd.h"
#include "game.h"
#include "main.h"
#include "boxgui.h"
#include "data.h"
#include "net.h"




/*=============================================================================
 * GLOBALS
 */
#include "version.h"

char game_string[] = "Everborn v"EVERBORN_VERSION;
int mainArgc;
char** mainArgv;



/*=============================================================================
 * CLOSE ALL
 */
int mainCloseAll()
{
 netReset();
 bOpen("main");
 gameClose();
 dataUnload();
}





/*=============================================================================
 * PR
 */
int pr(const char *paf, const char *mif)
{
 if(mif) pe("%s(%s): %s.\n", paf, mif, strerror(errno));
 else pe("%s: %s.\n", paf, strerror(errno));
 return -1;
}





/*==============================================================================
 * MAIN LOOP
 */
static tempoC TempoRefresh(10);



static bool mainReceiveFromServer()
{
 int cmdFromServer(char*);

 connectionC* sv = net_connections;
 bool recvd = false;

 if(netLoop() != -1)
    for(char* p = sv->fast_bf; p = netStringShift(p, sv); p += strlen(p) + 1)
	cmdFromServer(p),
	recvd = true;

 return recvd;
}



void mainLoop()
{
 // prototypes
 void inputPoll();
 void aiExecute(bool);


 // handle gui input
 inputPoll();


 // receive gamestate from server and execute ai if meaningful
 bool re = !netConnected() || mainReceiveFromServer();
 aiExecute(re);


 //@@ ??? this is ambigous and must be cleared
 if(TempoRefresh())
    screenRequestDraw();
 screenDraw();
}





/*=============================================================================
 * MAIN
 */
int main(int argc, char* argv[])
{
 pf("%s, Copyright (C) 2004-2007 by Francesco Orsenigo.\n", game_string);
 pf("\n");
 pf("This program is distributed under the terms of the\n");
 pf("GNU General Public License version 2 as published\n");
 pf("by the Free Software Foundation (see file COPYING).\n");
 pf("This program comes WITHOUT ANY WARRANTY.\n");
 pf("  Francesco Orsenigo <francesco.orsenigo@gmail.com>\n");
 pf("\n");

 mainArgc = argc;
 mainArgv = argv;

 // init menu
 bInit();

 // execute rcfile if available
 void initRC();
 initRC();


 // parse arguments
 bool dedicated = false;
 bool blo = false;
 bool box = false;
 bool xml = false;

 for(int a = 1; a < argc; a++)
    if(!strcmp(argv[a], "dedicated"))
    {
	a++;
	void serverSetGameString(char*); serverSetGameString(argv[a++]);
	dedicated = true;
    }
    else if(!strcmp(argv[a], "blo"))
	blo = true;
    else if(!strcmp(argv[a], "box"))
	box = true;
    else if(!strcmp(argv[a], "xml"))
	xml = true;
    else
	cmd("%s", argv[a]);


 // session
 if(dedicated)
    serverMainLoop();
 else if(xml)
 {
    void xmlout();
    xmlout();
 }
 else if(blo)
 {
    scrsndInit();
    bOpen("blo");
    while(1) mainLoop();
 }
 else if(box)
 {
    scrsndInit();
    bOpen("bbox");
    while(1) mainLoop();
 }
 else
 {
    scrsndInit();
    bOpen("main");
    while(1) mainLoop();
 }

 return 0;
}





//EOF =========================================================================
