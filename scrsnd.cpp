/*
 SCRSND.CPP		v0.31
 Screen and sound initialization.

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
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include "main.h"
#include "boxgui.h"
#include "data.h"
#include "sound.h"





/*=============================================================================
 * GLOBALS
 */
static envC sndSampleRate("samplerate", "44100");



static bool scrRedraw = true;
static int scrw = 0, scrh = 0;

int screenW()	{ return scrw; }
int screenH()	{ return scrh; }



void screenLineWidth(double w)
{
 glEnable(GL_BLEND);
 glLineWidth(w * scrw / 800.);
}



/*=============================================================================
 * RESHAPE
 */
static void screenReshape(int w, int h)
{
 scrw = w;
 scrh = h;
 glViewport(0, 0, scrw, scrh);
}



/*=============================================================================
 * DRAW
 */
static timerC drawTimer("draw");
static timerC swapTimer("swap");

void screenDraw()
{
 if(!scrRedraw) return;

 drawTimer.StartFrame();
 bDisplay();
 consoleDraw();
 drawTimer.Stop();

 swapTimer.StartFrame();
 SDL_GL_SwapBuffers();
 swapTimer.Stop();

 scrRedraw = false;
}

void screenRequestDraw()
{
 scrRedraw = true;
}





/*=============================================================================
 * LOW
 */
static int screenLow(char* str)
{
 char fe[] = "screenLow: ";

 if(!SDL_WasInit(SDL_INIT_VIDEO)) return 0;

 // feedback
 pf("%s%s\n", fe, str);


 // windowed mode?
 bool windowed = false;
 if(char* s = strchr(str, 'w'))
 {
    windowed = true;
    str = s+1;
 }


 // resolution
 int w;
 int h;
 if(!*str || !strcmp(str, "auto"))
 {
    w = 0;
    h = 0;
 }
 else if(sscanf(str, "%dx%d", &w, &h) < 2)
 {
    pe("invalid 'screen' format: it must be \"[width]x[height]\" (ex: \"800x600\")\n");
    return -1;
 }
 else if(w < 320 || h < 200)
 {
    pe("Please! Give me at least 320x200!\n");
    return -1;
 }


 // SDL_GL attributes
 SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


 // video mode
 Uint32 flags = SDL_OPENGL;
 if(!windowed) flags |= SDL_FULLSCREEN;

 pf("%sSDL_SetVideoMode()\n", fe);
 scrw = 0;
 scrh = 0;
 SDL_Surface* scr;

 // go!
 scr = SDL_SetVideoMode(w, h, 0, flags);
 if(!scr)
 {
    pe("%sfailed initializing video mode: %s.\n", fe, SDL_GetError());
    pe("trying default...\n");
    scr = SDL_SetVideoMode(0, 0, 0, flags | SDL_FULLSCREEN);
    if(!scr)
    {
	pe("default failed, unable to start. =(\n");
	exit(-1);
    }
 }



 // we got a screen!
 screenReshape(scr->w, scr->h);
 SDL_WM_SetCaption(game_string, game_string);
 SDL_ShowCursor(SDL_ENABLE);

 // keyboard settings
 SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
 SDL_EnableUNICODE(1);

 // GL state
 glAlphaFunc(GL_GREATER, 1/128.);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glEnable(GL_LINE_SMOOTH);



 // reload textures
 dataReload();

 // recreate minimap
// bool gameIsAvailable();
// void minimapUpdate();
// if(gameIsAvailable()) minimapUpdate();

 return 0;
}





/*=============================================================================
 * ENV
 */
static char* screenInitEnv(char* a)
{
 screenLow(a);
 return a;
}

static envC envScreen("screen", "auto", screenInitEnv);





/*=============================================================================
 * INIT
 */
void scrsndClose()
{
 soundUnloadAll();
 Mix_CloseAudio();
 SDL_Quit();
}



void scrsndInit()
{
 char fe[] = "scrsndInit: ";


 // set process name, precedes all stdout and stderr output
 consoleSetProcName("cl");


 // init SDL
 pf("%sSDL_Init()\n", fe);
 if(-1 == SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
 {
    pe("%sSDL_Init() failed: %s.\n", fe, SDL_GetError());
    exit(-1);
 }
 atexit(SDL_Quit);



 // init SDL_Mixer
 pf("%sMix_OpenAudio()\n", fe);
 if(Mix_OpenAudio(atoi(envGet("samplerate")), AUDIO_S16, 2, 4096))
 {
    pe("%sMix_OpenAudio() failed: %s.\n", fe, Mix_GetError());
    exit(-1);
 }
 atexit(Mix_CloseAudio);



 // go to graphic mode
 if(screenLow(envGet("screen")))
    exit(-1);

 pf("%sOpenGL version is %s\n", fe, glGetString(GL_VERSION));
 pf("%sOpenGL renderer is %s\n", fe, glGetString(GL_RENDERER));
 pf("%sOpenGL vendor is %s\n", fe, glGetString(GL_VENDOR));




 char* do_music(char*);
 do_music(envGet("music"));
}






static Mix_Music* music = 0;

char* do_music(char* str)
{
 if(!SDL_WasInit(SDL_INIT_AUDIO)) return str;

 Mix_HaltMusic();
 if(music) Mix_FreeMusic(music);
 music = 0;

 if(*str)
    if(music = Mix_LoadMUS(str))
    {
	pf("playing: %s\n", str);
	Mix_PlayMusic(music, -1);
    }
    else pe("failed loading '%s'\n", str);

 return str;
}



static envC musicEnv("music", "", do_music);



//EOF =========================================================================
