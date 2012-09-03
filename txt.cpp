/*
 TXT.C			v0.03
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

 BUGS:
    txtSkipLine() may exit from superstring boundary.
*/


#include <unistd.h>
#include <stdio.h>
#include <string.h>	// strlen()
#include <stdlib.h>

//#include "main.h"
#include "txt.h"





/*=============================================================================
 * CHAR HELPERS
 */
int txtIsId(char c)
{
 if(c >= 'A' && c <= 'Z') return 1;
 return c == '_';
}


int txtIsSpace(char c)
{
 return c == ' ' || c == '\t';
}





/*=============================================================================
 * WORD TERMINATE
 */
char *txtWordTerminate(char *s)
{
 char *r = s;

 while(*s && !txtIsSpace(*s)) s++;
 *s = '\0';

 return r;
}





/*=============================================================================
 * READ HELPERS
 */
char *txtSkipSpaces(char *s)
{
 while(*s && txtIsSpace(*s)) s++;
 return s;
}



char *txtSkipString(char *s)
{
 while(*s && !txtIsSpace(*s)) s++;
 return txtSkipSpaces(s);
}



char *txtSkipLine(char *s)
{
 while(*s++);
 return s;
}




/*=============================================================================
 * INTERPRETERS
 */
int txtParse(char *id, char *str)
{
 while(*id && *id == *str) id++, str++;
 return *id;
}



char *txtSearch(char *id, char *s)
{
 //@@ vengono tolti gli spazi all'inizio della riga?
 while(*s && txtParse(id, s)) s = txtSkipLine(s);
 if(*s) return txtSkipLine(s);
 return NULL;
}





/*=============================================================================
 * NAME
 *
 * Stops at first space or buffer size reached.
 * Always returns first space or EOL.
 */
char *txtName(char *dest, char *src, unsigned max_buffer)
{
 src = txtSkipSpaces(src);

 while(*src && !txtIsSpace(*src))
    if(--max_buffer < 1) src++;
    else *dest++ = *src++;

 *dest = '\0';

 return src;
}



/*=============================================================================
 * ID
 *
 * Stops at first space or buffer size reached.
 */
char *txtId(char *dest, char *src, unsigned max_buffer)
{
 char *d = dest;

 src = txtSkipSpaces(src);

 while(txtIsId(*src) && --max_buffer) *dest++ = *src++;
 *dest = '\0';

 if(dest - d < 4)	return NULL;	// ID too short //
 if(txtIsId(*src))	return NULL;	// IDs REQUIRE to be entirely read! //
 if(txtIsSpace(*src))	return src;
 if(*src == '\0')	return src;
 return NULL;				// inconsistent ID. //
}





/*=============================================================================
 * STRING
 * 
 * From a '"' to the next '"' or EOL.
 */
char* txtString(char* dest, char* src, unsigned max_buffer)
{
 // find first available '"'
 while(*src && *src++ != '"');

 // read to next '"' encountered
 char* end = dest + max_buffer -1;
 while(*src && *src != '"')
    if(dest < end)	*dest++ = *src++;
    else		src++;

 *dest = '\0';
 return src + 1;
}



/*=============================================================================
 * WR STRING
 * 
 * Writes a string, with '"'s.
 */
char *txtWrString(char *w, char *r)
{
 *w++ = ' ';
 *w++ = '"';

 while(*r)
    if(*r == '\n') *w++ = '\\', *w++ = 'n', r++;
    else *w++ = *r++;

 *w++ = '"';

 // terminate and return //
 *w = '\0';
 return w;
}





/*=============================================================================
 * READ STATS
 *
 * targ offset is calculated as difference between starter (first statA
 * element) address and entry address.
 */
char* txtReadStats(char* s, char* e, stat_t* statA, short* targ)
{
 short* start = statA->targ;
 statA++;

 while(s = txtSkipSpaces(s), *s)
 {
    if(*s == '-') return s;

    stat_t* p;
    for(p = statA; p->id && txtParse(p->id, s); p++);
    if(p->id == NULL)
    {
	sprintf(e, "unknown stat %s", txtWordTerminate(s));
	return NULL;
    }

    s += strlen(p->id);
    targ[p->targ - start] = strtol(s, &s, 10);

    s = txtSkipString(s);
 }

 return s;
}



/*=============================================================================
 * WRITE STATS
 *
 * targ offset is calculated as difference between starter (first statA
 * element) address and entry address.
 */
char* txtWriteStats(char* bf, stat_t* statA, short* targ)
{
 char *w = bf;

 short* start = statA->targ;
 statA++;

 for(stat_t* st = statA; st->id; st++)
 {
    short* t = targ + (st->targ - start);
    if(*t) w += sprintf(w, " %s%d", st->id, *t);
 }
 
 if(w == bf) w += sprintf(w, " -");
 *w++ = '\n';

 // terminate and return
 *w = '\0';
 return w;
}





/*=============================================================================
 * CLEAR SPECIALS
 */
int txtClearSpecials(spcC* spc)
{
 while(spc->Id) spc->Flag &= ~SPC_ACTIVE, spc++;
}





/*=============================================================================
 * READ SPECIALS
 */
char* txtReadSpecials(char *s, char *e, spcC *spc)
{
 txtClearSpecials(spc);

 // search
 while(s = txtSkipSpaces(s), *s)
 {
    if(*s == '-') return s;

    spcC* p;
    for(p = spc; p->Id && txtParse(p->Id, s); p++);
    if(p->Id == NULL)
    {
	sprintf(e, "unknown special `%s`", txtWordTerminate(s));
	return NULL;
    }

    p->Flag |= SPC_ACTIVE;
    s = txtSkipString(s);
 }

 return s;
}





/*=============================================================================
 * STRIP
 *
 * Removes comments.
 * Converts '\n' inside strings into spaces, allowing a string to extend on multiple lines.
 * Converts "\n" inside strings into '\n', allowing to force line feed.
 * Removes start line spaces.
 * Removes multiple spaces.
 * Removes empty lines.
 *
 * Converts '\n' outside strings to '\0'.
 * Double-terminates the superstring.
 */
char *txtStrip(char *bf)
{
 int string;
 int prev;
 char *r, *w;


 // remove comments //
 string = 0;
 for(r = w = bf; *r; *r++)
    if(*r == '#' && !string)
	{
	    while(*r && *r != '\n') r++; // skip to end of line //
	    r--;
	}
    else if(*r == '"') *w++ = *r, string = !string;
    else *w++ = *r;
 *w = 0;


 // convert '\n' inside strings into spaces. //
 string = 0;
 for(r = bf; *r; *r++)
    switch(*r) {
	case '"': string = !string; break;
	case '\n': if(string) *r = ' ';	break;
	default: break;
    }


 // convert "\n" inside strings into '\n' //
 prev = 'n';
 string = 0;
 for(r = w = bf; *r; prev = *r++)
    switch(*r) {
	case 'n':
	    if(string && prev == '\\') w[-1] = '\n';
	    else *w++ = *r;
	    break;
	case '"': string = !string;
	default: *w++ = *r; break;
    }
 *w = 0;


 // remove unwanted spaces (beginning and multiple) //
 prev = 0;
 for(r = w = bf; *r; *r++)
    switch(*r) {
	case '\n': prev = 0, *w++ = *r; break;
	case ' ': if(prev) prev = 0, *w++ = *r; break;
	default: prev = 1, *w++ = *r; break;
    }
 *w = 0;


 // remove multiple '\n' //
 prev = 0;
 for(r = w = bf; *r; *r++)
    if(*r != '\n') prev = 1, *w++ = *r;
    else if(prev) prev = 0, *w++ = *r;
 *w = 0;


 // convert '\n' outside strings to terminators //
 string = 0;
 for(r = bf; *r; *r++)
    switch(*r) {
	case '"': string = !string; break;
	case '\n': if(!string) *r = 0; break;
	default: break;
    }


 *r++ = '\0'; // this is a superstring, so is terminated twice. //
 return bf;
}





/*=============================================================================
 * LOAD
 *
 * Loads a text file in memory, doing some preprocessing.
 */
char *txtLoad(char *fn)
{
 unsigned char *h = NULL;
 unsigned r, size, rd = 0;
 unsigned spaces = 0, chrs = 0;

 FILE *f;


 // open file //
 f = fopen(fn, "rt");
 if(f == NULL) return NULL;


 // determine file size (i'm sure there are better ways, but i'm too lazy to find out... //
 size = lseek(fileno(f), 0, SEEK_END);
 lseek(fileno(f), 0, SEEK_SET);
 if(size > 1024*128) goto ERROR;


 // allocate //
 h = (unsigned char*)malloc(size + 2);		// the string needs to be terminated twice //
 if(h == NULL) goto ERROR;


 // read //
 while(rd < size)
    switch(r = read(fileno(f), h + rd, size - rd)) {	// i hate fread() //
	case -1: goto ERROR;
	case 0: size = rd; break;
	default: rd += r;
    }
 fclose(f);


 // check if it can be considered a text file //
 for(r = 0; r < size; r++)
    if(h[r] < '\t') goto ERROR;


 // eliminate strange chars //
 for(r = 0; r < size; r++)
    if(h[r] < ' ' && h[r] != '\n') h[r] = ' ';


 // terminate the string //
 h[size + 0] = '\0';
 h[size + 1] = '\0';


 // strip some stuff and return //
 txtStrip((char *)h);
 return (char *)h;


 ERROR:
 if(h) free(h);
 fclose(f);

 return NULL;
}





//EOF =========================================================================
