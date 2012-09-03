/*
 TXT.H			v0.03
 Text handlers.

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
#ifndef H_FILE_TXT
#define H_FILE_TXT
//=============================================================================

#define SPC_ACTIVE	(1<<0)	//spc.flag

typedef struct stat_s {
    short*	targ; // points to the target field of a temporary structure. //
    char*	id;
    char*	name;
    char*	info;
} stat_t;

class spcC { public:
    long	Code;
    long	Flag;
    int		Gfx;
    char*	Id;
    char*	Name;
    char*	Info;
};





/*=============================================================================
 * txt.c
 */
int txtIsId(char c);
int txtIsSpace(char c);

char *txtWordTerminate(char *s);

char *txtSkipSpaces(char *s);
char *txtSkipString(char *s);
char *txtSkipLine(char *s);

int txtParse(char *id, char *str);
char *txtSearch(char *id, char *s);

char *txtName(char *dest, char *src, unsigned max_buffer);
char *txtId(char *dest, char *src, unsigned max_buffer);
char *txtString(char *dest, char *src, unsigned max_buffer);
char *txtWrString(char *w, char *r);

char *txtReadStats(char *s, char *e, stat_t *statA, short *targ);
char *txtWriteStats(char *bf, stat_t *statA, short *targ);

int txtClearSpecials(spcC *spc);
char *txtReadSpecials(char *s, char *e, spcC *spc);

char *txtStrip(char *bf);
char *txtLoad(char *fn);



#endif
//EOF =========================================================================
