/*
 FNT.H			v0.30
 Font loadup and rendering.

 Copyright (c) 2004-2008 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#ifndef H_FILE_FNT
#define H_FILE_FNT
//=============================================================================

void fntS(char* fname, float h, float maxw);
void fntS(char* fname, float h);
void fntS(float h);

float fntL(char* fmt, ...);
float fntC(char* fmt, ...);
float fntR(char* fmt, ...);
float fntT(float maxw, char* fmt, ...);



// Escape
typedef float (*fntEscapeF)(bool mute, char* s, char** e, float h);
class fntEscapeC { public:
    fntEscapeC*	Next;
    fntEscapeF	EscapeF;

    //
    fntEscapeC(fntEscapeF);
};

void fntRestoreColor();

#endif
//EOF =========================================================================
