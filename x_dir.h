/*
 X_DIR.C		v0.02
 Directory Search

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
#ifndef H_FILE_X_DTREE
#define H_FILE_X_DTREE
//=============================================================================

typedef struct dtree_s {
    struct dtree_s	*n;
    char		s[0];
} dtree_t;


char *dir_path(char *fn, char *dir);
char *dir_search(char *fn);

void dtree_free(dtree_t *p);
dtree_t *dtree_seek(char *dirname, char *parse, dtree_t *h);
dtree_t *dtree_list(char *parse);

#endif
//EOF =========================================================================
