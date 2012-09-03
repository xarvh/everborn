/*
 DATA.CPP		v0.20
 GL Textures loadup/managing. For now.

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
#include <GL/gl.h>

#include <unistd.h>
#include <string.h>

#include "main.h"
#include "data.h"
#include "pngio.h"





/*=============================================================================
 * GLOBALS
 */
static dataC* dataBaseHook = 0;	// these are permanent
static dataC* dataTempHook = 0;	// these will be discarded by dataUnload()





/*=============================================================================
 * FIND PATH
 */
static char* dataTryPath(char* path, char* fn)
{
 if(!path) return NULL;

 char* r = new char[strlen(path) + strlen(fn) + 2];
 strcpy(r, path);

 char* c = r + strlen(path);
 *c++ = '/';
 strcpy(c, fn);

 if(!access(r, R_OK)) return r;
 delete[] r;

 return NULL;
}

char* dataFindPath(char* fn)
{
 char* c;

 if(c = dataTryPath(envGet("userdir"), fn)) return c;
 if(c = dataTryPath(envGet("basedir"), fn)) return c;
 if(c = dataTryPath(".", fn)) return c;

 return NULL;
}





/*=============================================================================
 * UNLOAD / LOAD
 */
void dataC::Unload()
{
 if(Id) glDeleteTextures(1, &Id);
 Id = 0;
}



int dataC::Load()
{
 char fe[] = "dataC::Load: ";

 if(Id) 	return 0;	// already loaded
 if(!Fn[0])	return 0;	// not real data, plain skip it


 //@@ debug
 pf("search [%s]\n", Fn);


 // search file
 char* path = dataFindPath(Fn);
 if(path == NULL)
 {
    pe("%scan't find '%s'.\n", fe, Fn);
    return -1;
 }


 // load file
 pngT* png = pngLoad(path);
 delete[] path;
 if(png == NULL)
 {
    pr(fe, "pngLoad");
    return -1;
 }

 // preprocess?
 if(Proc) if(Proc(png, ProcV))
 {
    pngFree(png);
    return -1;
 }

 // eat gl errors
 while(glGetError() != GL_NO_ERROR);

 // generate and load texture
 glGenTextures(1, &Id);
 glBindTexture(GL_TEXTURE_2D, Id);
 glTexImage2D(
    GL_TEXTURE_2D,	// target
    0,			// mipmap depth
    4,			// components
    png->w,		// width
    png->h,		// height
    0,			// border
    GL_RGBA,		// format
    GL_UNSIGNED_BYTE,	// pixel type
    png->d		// image data
 );

 QuadX = .5 - 2 * Cols / png->w;
 QuadY = .5 - 2 * Rows / png->h;

 pngFree(png);

 switch(glGetError()) {
    case GL_NO_ERROR: break;
    case GL_INVALID_ENUM: pe("%s`%s` GL_INVALID_ENUM.\n", fe, Fn); return -1;
    case GL_INVALID_VALUE: pe("%s `%s` invalid input texture.\n", fe, Fn); return -1;
    default: pe("%s`%s` GL error %d?\n", fe, Fn); return -1;
 }

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 return 0;
}





/*=============================================================================
 * LOAD / UNLOAD ALL
 */
int dataLoadMissing()
{
 for(dataC* d = dataBaseHook; d; d = d->Next) if(d->Load()) return -1;
 for(dataC* d = dataTempHook; d; d = d->Next) d->Load();
 return 0;
}



int dataReload()
{
 for(dataC* d = dataBaseHook; d; d = d->Next) d->Unload();
 for(dataC* d = dataTempHook; d; d = d->Next) d->Unload();

 return dataLoadMissing();
}



void dataUnload()
{
 for(dataC* d = dataTempHook; d; d = d->Next) d->Unload();
}





/*=============================================================================
 * CONSTRUCTORS
 */
void dataC::Init(char* fn, double cols, double rows, dataC** h)
{
 // clear
 memset(Fn, sizeof(Fn), '\0');
 Id = 0;
 Proc = 0;

 // set
 Cols = cols;
 Rows = rows;
 if(fn) strncpy(Fn, fn, sizeof(Fn)-1);

 // register
 while(*h) h = &(*h)->Next;
 *h = this;
 Next = 0;
}



dataC::dataC() 				  { Init(0,  1, 1, &dataTempHook); }
dataC::dataC(double c, double r)	  { Init(0,  c, r, &dataTempHook); }
dataC::dataC(char* fn)			  { Init(fn, 1, 1, &dataBaseHook); }
dataC::dataC(char* fn, double c, double r){ Init(fn, c, r, &dataBaseHook); }





/*=============================================================================
 * DISTRUCTOR
 */
static void dataUnRegister(dataC** h, dataC* d)
{
 while(*h)
    if(*h != d)	 h = &(*h)->Next;
    else	*h = d->Next;
}

dataC::~dataC()
{
 Unload();
 dataUnRegister(&dataBaseHook, this);
 dataUnRegister(&dataTempHook, this);
}





/*=============================================================================
 * BIND TEXTURE
 */
void dataBindTexture(int glText)
{
 glBindTexture(GL_TEXTURE_2D, glText);
 glEnable(GL_TEXTURE_2D);

 glMatrixMode(GL_TEXTURE);
 glLoadIdentity();
 glMatrixMode(GL_MODELVIEW);
}

void dataBindTexture(int glText, double tX, double tY, double sX, double sY)
{
 glBindTexture(GL_TEXTURE_2D, glText);
 glEnable(GL_TEXTURE_2D);

 glMatrixMode(GL_TEXTURE);
 glLoadIdentity();
 glTranslated(tX, tY, 0);
 glScaled(sX, sY, 1);
 glMatrixMode(GL_MODELVIEW);
}

void dataC::BindTexture()
{
 dataBindTexture(Id);
}





/*=============================================================================
 * DISPLAY
 */



//
// SELECT
//
void dataC::Select(int n, double w, double h)
{
 int x = n%(int)Cols;
 int y = n/(int)Cols;

 double tx = (x+w/2) / Cols;
 double sw = w / Cols;
 double ty = (y+h/2) / Rows;
 double sh = h / Rows;

 dataBindTexture(Id, tx, ty, sw, sh);
}



//
// QUAD, RECT
//
void dataC::Quad(int n) { Rect(n, 1, 1); }

void dataC::Rect(int n, double w, double h)
{
 Select(n, w, h);
 glBegin(GL_QUADS);
    glTexCoord2d(-QuadX, -QuadY); glVertex2f( -w/2, +h/2);
    glTexCoord2d(+QuadX, -QuadY); glVertex2f( +w/2, +h/2);
    glTexCoord2d(+QuadX, +QuadY); glVertex2f( +w/2, -h/2);
    glTexCoord2d(-QuadX, +QuadY); glVertex2f( -w/2, -h/2);
 glEnd();
}





/*=============================================================================
 * ENV
 */
static char* dataCheckDirs(char* v)
{
 char* c = v + strlen(v) -1;
 while(c >= v && *c == '/') c--;
 if(c[1] == '/') c[1] = '\0';

 return v;
}

static envC envUserDir("userdir", "Everborn", dataCheckDirs);
static envC envBaseDir("basedir", "/usr/share/Everborn", dataCheckDirs);





//EOF =========================================================================
