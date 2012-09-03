/*
 SHOT.CPP		v0.01
 Screenshot.

 Copyright (c) 2005 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include <GL/gl.h>
#include <stdio.h>
#include "main.h"
#include "cmd.h"
#include "pngio.h"

static int shotCnt = 0;



static int shot()
{
 int w = screenW();
 int h = screenH();

 // fetch
 rgbaT bf[w*h];
 glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bf);

 // swap
 for(int x = 0; x < w; x++) for(int y = 0; y < h/2; y++)
 {
    int a = x + y*w;
    int b = x + (h-y-1)*w;

    rgbaT t = bf[a];
    bf[a] = bf[b];
    bf[b] = t;
 }

 // max alpha channel
 for(int x = 0; x < w; x++) for(int y = 0; y < h; y++)
    bf[x + y*w].a = 255;

 // prepare filename
 char fn[64];
 snprintf(fn, sizeof(fn), "%sshot%04d.png", game_string, shotCnt++);
 for(char* c = fn; *c; c++)
    if(*c == ' ' || *c == '\t') *c = '_';

 // save
 if(pngSave(fn, (unsigned char*)bf, w, h)) pe("error taking screenshot!\n");
 else pf("Shot: '%s'\n", fn);

 return 0;
}


static cmdC cmdShot(
    "shot",
    ":",
    "take a screenshot",
    shot,
    0, 0);



//EOF =========================================================================
