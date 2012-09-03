/*
 PNGIO.H	v0.06
 Simple wrapper to libPNG.

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
#ifndef H_FILE_PNG
#define H_FILE_PNG
//=============================================================================

typedef struct rgbaS {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} rgbaT;

typedef struct pngS {
    unsigned short	w;		// image width (in pixels) //
    unsigned short	h;		// image height (in pixels) //
    unsigned char	d[0];		// image pixels:  each is 4-bytes, RGBA format, 1 byte per plane //
    rgbaT		p[0];		// image pixels, rgba_t format
} pngT;


char*	pngVersion();

pngT*	pngLoad(const char* fn);
pngT*	pngEmpty(int w, int h);
void	pngFree(pngT* png);
int	pngSave(const char* fn, unsigned char* img, int w, int h);
int	pngSave(const char* fn, pngT* png);


#endif
//EOF =========================================================================
