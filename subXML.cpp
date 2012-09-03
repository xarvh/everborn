/*
 subXML.cpp		v0.00
 spell XML output.

 Copyright (c) 2008 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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

#include <stdio.h>
#include <stdarg.h>

#include "spell.h"



/*=============================================================================
 *
 */
static int indent = 0;

static void p(char* fmt, ...)
{
 FILE* out = stderr;

 if(*fmt) for(int i = 0; i < indent; i++)
    fprintf(out, "    ");

 va_list ap;
 va_start(ap, fmt);
 vfprintf(out, fmt, ap);

 fprintf(out, "\n");
}



/*=============================================================================
 *
 */
char* rank(int r)
{
 if(r == 0) return "neutral";
 if(r == 1) return "common";
 if(r == 2) return "uncommon";
 return "rare";
}



void printspell(spellC* s)
{
 p("<name>%s</name>", s->Name);
// p("<rank>%s</rank>", rank(s->Rank));
 p("<type>%s</type>", s->Type->Name);
 p("<info>%s</info>", s->Description);
 if(s->Picture) p("<gfx>%d</gfx>", s->Picture);
 if(s->UpkeepCost) p("<upkeep>%d</upkeep>", s->UpkeepCost);
 p("<know>%d</know>", s->ResearchCost);
 p("<bcost>%d</bcost>", s->BattleCost);
 p("<ocost>%d</ocost>", s->OverlandCost);
}



void xmlout()
{
 indent = 0;

 p("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>");
 p("<?xml-stylesheet type=\"text/css\" href=\"realm.css\"?>");
 p("<!DOCTYPE realm SYSTEM \"realm.dtd\">");
 p("<magic>");

 for(realmC* r = realmsHook; r; r = r->Next)
 {
    p("");
    p("<realm>");
    indent++;

	p("<name>%s</name>", r->Name);
	p("<info>%s</info>", r->Desc);

	for(bookC* b = r->BooksHook; b; b = b->Next)
	{
	    p("");
	    p("<book>");
	    indent++;
		p("<name>%s</name>", b->Name);
		p("<history>%s</history>", b->History);
		
		for(spellC* s = b->SpellsHook; s; s = s->BookNext)
		{
		    p("");
		    p("<spell>");
		    indent++;

			printspell(s);

		    indent--;
		    p("</spell>");
		}
	    
	    indent--;
	    p("</book>");

	}

    indent--;
    p("</realm>");
 }

 p("</magic>");
}



//EOF =========================================================================
