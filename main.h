/*
 MAIN.H			v0.21
 World vars and some miscellanea.

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
#ifndef H_FILE_MAIN
#define H_FILE_MAIN
//=============================================================================
#include <stdio.h>
#define __f printf

extern char game_string[];
extern int mainArgc;
extern char** mainArgv;

#ifndef NULL
#define NULL	((void *)0)
#endif





/*=============================================================================
 * INPUT
 * Input wrappers
 */
#define INPUT_KUP	1
#define INPUT_KDN	2
#define INPUT_KLF	3
#define INPUT_KRI	4
#define INPUT_MUP	5
#define INPUT_MDN	6
#define INPUT_KHOME	7
#define INPUT_KEND	8

#define INPUT_F1	9
#define INPUT_F2	10
#define INPUT_F3	11
#define INPUT_F4	12
#define INPUT_F5	13
#define INPUT_F6	14
#define INPUT_F7	15

int mouseX();
int mouseY();
int mouseR();
int mouseM();
int mouseL();
int mouseRdn();
int mouseMdn();
int mouseLdn();

int inputK();
int inputE();
int inputShift();
int inputCtrl();
int inputAlt();



/*=============================================================================
 * TIME
 */
class timerC {
    char*		Name;
    unsigned long	TStart;
    unsigned long	Eta;
    unsigned long	Frames;
 public:
    void StartFrame();
    void Start();
    void Stop();
    float FPS();
    timerC(char* name);
    ~timerC();
};


class tempoC {
    unsigned long LastTime;
    unsigned long Interval;
 public:
    bool operator()();
    tempoC(long interval);
};


unsigned long timeGet();



/*=============================================================================
 * CONSOLE
 * Print Functions
 */
void consoleDraw();
void consoleSetProcName(char*);
int pinline();					// inline start
int pi(const char *fmt, ...);			// inline print
int pf(const char *fmt, ...);			// std print
int pe(const char *fmt, ...);			// error print
int pr(const char *paf, const char *cif);	// errno print





/*=============================================================================
 * ENV
 * Environment variables
 */
char* envGet(char *var);
char* envGetName(int pos);
int envSet(char *var, char *val);
bool envIsTrue(char*);

class envC { public:
    envC(char* name, char *value_default, char* (*call)(char *value));
    envC(char* name, char *value_default);
    envC(char* name, bool value_default);
};





/*=============================================================================
 * SCREEN / SOUND
 */
void scrsndClose();
void scrsndInit();

void screenDraw();
void screenRequestDraw();
void screenLineWidth(double w);
int screenW();
int screenH();



/*=============================================================================
 * SERVER
 */
void serverMainLoop();



/*=============================================================================
 * MENU LAND
 */
void menuLand_CenterAt(float x, float y);



#endif
//EOF =========================================================================
