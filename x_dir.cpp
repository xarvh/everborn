/*
 X_DIR.C		v0.02
 Directory Search.

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

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "x_dir.h"






/*=============================================================================
 * DIR SEEK
 */

char *dir_path(char *fn, char *dir)
{
 int slash = 1;
 char *s;

 if(dir[strlen(dir) -1] == '/') slash = 0;
 s = (char*)malloc(strlen(dir) + slash + strlen(fn) + 1);

 strcpy(s, dir);
 if(slash) s[strlen(dir)] = '/';
 strcpy(s + strlen(dir) + slash, fn);

 return s;
}



char *dir_search(char *fn)
{
 char *c, *p;

 if(p = getenv("userdir"))
    if(!access(c = dir_path(fn, p), R_OK))
	return c;

 if(p = getenv("basedir"))
    if(!access(c = dir_path(fn, p), R_OK))
	return c;

 if(!access(c = dir_path(fn, "."), R_OK))
    return c;

 return 0;
}





/*=============================================================================
 * DTREE FREE
 */
void dtree_free(dtree_t *p)
{
 dtree_t e;
 while(p)
    {
	e = *p;
	free(p);
	p = e.n;
    }
}





/*=============================================================================
 * DIR SEEK
 */
dtree_t *dtree_seek(char *dirname, char *parse, dtree_t *h)
{
 struct dirent *de;
 DIR *dir;
 dtree_t *e;

 char *c;
 int i;

 dir = opendir(dirname);
 if(!dir) return h;

 while(de = readdir(dir))
    {
	i = 0;
	for(c = de->d_name; *c; c++)
	    if(*c == parse[i]) i++;
	    else if(*c == *parse) i = 1;
	    else i = 0;
	if(!parse[i])
	    {
		e = (dtree_t*)malloc(sizeof(dtree_t) + strlen(dirname) + strlen(de->d_name) + 2);
		if(!e) return h;

		for(i = 0; e->s[i] = dirname[i]; i++);
		if(e->s[i-1] != '/') e->s[i++] = '/';
		strcpy(e->s +i, de->d_name);

		e->n = 0;
		h->n = e;
		h = e;
	    }
    }

 closedir(dir);
 return h;
}





/*=============================================================================
 * DTREE LIST
 */
dtree_t *dtree_list(char *parse)
{
 char *p;
 dtree_t h[1], *e;

 h->n = 0;
 e = h;

 if(p = envGet("userdir")) e = dtree_seek(p, parse, e);
 if(p = envGet("basedir")) e = dtree_seek(p, parse, e);
 e = dtree_seek(".", parse, e);

 return h->n;
}



//EOF =========================================================================
