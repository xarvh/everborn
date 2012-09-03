/*
 CMD.CPP			v0.30
 Lower level command handlers.

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

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"
#include "cmd.h"



/*=============================================================================
 * GLOBALS
 */
static cmdC* cmdsHook = NULL;
static char cmd_bf[100];	// errors and remote commands
static char cmd_word_bf[20];	// used to output command name in error messages



/*=============================================================================
 * ARG GLOBALS
 */
cmdC* arg_cmd;
argT arg0;
argT arg1;
argT arg2;
argT arg3;
argT arg4;
argT arg5;
argT arg6;
argT arg7;

argT* cmd_args[] = {		// arguments; again, no check is made
    &arg0, &arg1, &arg2, &arg3,
    &arg4, &arg5, &arg6, &arg7,
    0
};



/*=============================================================================
 * SYNTAX CRAP
 */
typedef char* (*cmd_syntax_t)(argT *, char *);
static cmd_syntax_t cmd_syntax_re_a[256];
cmdSyntaxC::cmdSyntaxC(char k, cmd_syntax_t re) { cmd_syntax_re_a[k] = re; }





/*=============================================================================
 * HELPERS / SUPPORT FUNCTIONS
 */
int cmd_ecmp(char *a, char *b)		// a strcmp() variant
{
 while(*a && *a == *b) a++, b++;
 return *a || (*b && *b != ' ' && *b != '\t');
}

char* cmd_copy_str(char* d, char* s)	// a strcpy() variant
{
 while(*d++ = *s++);
 return s;
}

static char *cmd_copy_word(char *d, char *s)	// another strcpy() variant, copies only a word //
{
 while(*s && *s != ' ') *d++ = *s++;
 *d = 0;
 return s;
}

static char *cmd_skip_spaces(char *s)		// just what the name says //
{
 while(*s == ' ') s++;
 return s;
}





/*=============================================================================
 * SYNTAX basic INTERPRETERS
 * Translates strings into arguments (_re_ functions) and
 * arguments into strings (_wr_ functions).
 */
static char *cmd_syntax_re_float(argT *arg, char *s)
{
 char *end;

 arg->f = strtod(s, &end);	// read the float //
 if(s != end) return end;	// 's == end' means 'no digits at all' //

 cmd_copy_str(arg->s, "no valid float specifyed for '%s'");
 return 0;			// ok //
}

static cmdSyntaxC syntaxReFloat('F', cmd_syntax_re_float);





static char *cmd_syntax_re_integer(argT *arg, char *s)
{
 char *end;

 arg->i = strtol(s, &end, 0);	// read the integer //
 if(s != end) return end;	// 's == end' means 'no digits at all' //

 cmd_copy_str(arg->s, "no valid integer specifyed for '%s'");
 return 0;			// ok //
}

static cmdSyntaxC syntaxReInteger('I', cmd_syntax_re_integer);





static char *cmd_syntax_re_name(argT *arg, char *s)
{
 s = cmd_skip_spaces(s);
 if(*s) return cmd_copy_word(arg->s, s);

 cmd_copy_str(arg->s, "no '%s' specifyed");
 return 0;			// an empty string is NOT acceptable //
}
static cmdSyntaxC syntaxReName('N', cmd_syntax_re_name);





static char *cmd_syntax_re_string(argT *arg, char *s)
{
 s = cmd_skip_spaces(s);
 s = cmd_copy_str(arg->s, s);
 return s;			// no errors, an empty string IS acceptable //
}

static cmdSyntaxC syntaxReString('S', cmd_syntax_re_string);





/*=============================================================================
 * SYNTAX
 * These functions fill the cmd_args_a array and check for argument errors.
 */
static char cmd_get_arg_name_bf[20];
static char *cmd_get_arg_name(char *sy)
{
 char *c;
 int p;

 for(c = sy; *c != ':'; c++);
 p = c - sy;

 // find sy end //
 while(*c++);
 while(p-- > 0)
    while(*--c != ' ');

 cmd_copy_word(cmd_get_arg_name_bf, c + 1);
 return cmd_get_arg_name_bf;
}



static char* cmd_syntax_re_lo(char* c, char* sy, argT* arg)
{
 char type;

 type = *sy;

 arg->miss = false;

 if(type >= 'a' && type <= 'z')	// not a required argument
    if(*c) type += 'A' - 'a';
    else
	{
	    arg->miss = true;
	    return c;
	}

 c = cmd_syntax_re_a[type](arg, c);
 if(c) return c;

 sprintf(cmd_bf, arg->s, cmd_get_arg_name(sy));
 return 0;
}


static char *cmd_syntax_re(char *c, char *sy)
{
 int k;

 sy--;
 c = cmd_syntax_re_name(cmd_args[0], c);	// retrive the command itself
 for(k = 1; sy[k] != ':'; k++)
 {
    c = cmd_syntax_re_lo(c, sy + k, cmd_args[k]);
    if(!c) return cmd_bf;
 }

 return 0;
}





/*=============================================================================
 * PARSE
 */
cmdC* cmdParse(char* str)
{
 int k;
 cmdC* c;

 // skip modificators //
 if(*str == '+') str++;
 if(*str == '-') str++;

 // parse //
 for(c = cmdsHook; c && cmd_ecmp(c->Name, str); c = c->Next);
 return c;
}





/*=============================================================================
 * INTERPRETE
 */
cmdC* cmdInterprete(char* s, char* fe)
{
 // clear args
 for(int i = 0; cmd_args[i]; i++)
    memset(cmd_args[i], 0, sizeof(argT));


 // convert '=' and ',' to spaces
 for(char* c = s; *c; c++)
    if(*c == '=' || *c == ',') *c = ' ';


 // process command or recognize environment variables
 cmdC* c = cmdParse(s);
 if(c == NULL)
 {
    // check if an env var with this name is present
    char *w = cmd_copy_word(arg1.s, s);
    if(envGet(arg1.s))
    {
        // fallback to 'set' command //
        strcpy(arg0.s, "set");
	strncpy(arg2.s, cmd_skip_spaces(w), CMD_ARGSTRING_BF-1);
	arg1.s[CMD_ARGSTRING_BF-1] = '\0';
	return cmdParse("set");
    }

    cmd_copy_word(arg0.s, s);
    pe("%sunknown command '%s'.\n", fe, arg0.s);
    return NULL;
 }

 // save command
 arg_cmd = c;

 // process arguments
 s = cmd_syntax_re(s, c->Syntax);
 if(s)
    {
	pe("%ssyntax error, %s.\n", fe, s);
	return 0;
    }

 // return gracefully
 return c;
}





/*=============================================================================
 * CMD
 * Issues a local cmd from a string.
 */
int cmd(const char* fmt, ...)
{
 char fe[] = "cmd: ";

 char bf[100];
 va_list arg;
 cmdC* c;

 va_start(arg, fmt);
 vsnprintf(bf, sizeof(bf), fmt, arg);
 bf[sizeof(bf) -1] = '\0';
 va_end(arg);

 c = cmdInterprete(bf, fe);	if(!c) return -1;

 c->Exec();			// this is a local command, must be executed //

 // game commands must be sent to server too //
 if(c->Game) cmd_execSendToServer(bf);


 return 0;
}





/*=============================================================================
 * FILE
 * Executes a file
 * I must rewrite it all.
 */
#define MAX_EXEC_LINE 100
#define MAX_EXEC_DEPTH 5


static unsigned _exec_depth = 0; // this keeps track of recursion //

int cmdFile(char *fn)
{
 char fe[] = "exec_file: ";
 char *c, bf[MAX_EXEC_LINE];
 FILE *f;
 int r = 0;

 _exec_depth++;
 if(_exec_depth > MAX_EXEC_DEPTH)
    {
	pe("%stoo many recursive calls (%d).\n", fe, MAX_EXEC_DEPTH);
	goto END;
    }

 f = fopen(fn, "r");
 if(f == NULL)
    {
	pr("fopen", NULL);
	goto END;
    }

 pf("%s`%s`.\n", fe, fn);

 while(fgets(bf, sizeof(bf), f))
    {
	if(c = strchr(bf, '#')) *c = '\0';
	else if(c = strchr(bf, '\n')) *c = '\0';
	if(*bf) cmd(bf);
    }

 fclose(f);

 END:
 _exec_depth--;
 return r;
}





/*=============================================================================
 * CONSTRUCTOR
 */
static int cmdFake() {}

cmdC::cmdC(char* nm, char* sy, char* he, int (*ex)(), int (*gm)(), int (*ch)())
{
 cmdC** h;

 for(h = &cmdsHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Name = nm;
 Syntax = sy;
 Help = he;

 Game = gm;
 Check = ch;
 Exec = ex ? ex : cmdFake;
}



/*=============================================================================
 * HELP
 */
static int cmdEHelp()
{
 if(arg1.miss)
 {
    for(cmdC* c = cmdsHook; c; c = c->Next) if(c->Help)
	pf("%10s: %s\n", c->Name, c->Help);
 }
 else
 {
    cmdC* c = cmdParse(arg1.s);
    if(!c) return pf("'%s' is not a command\n", arg1.s);

    pf("Syntax:\n");
    pf("%s %s\n", c->Name, strchr(c->Syntax, ':')+1);
    if(c->Help) pf("%s\n", c->Help);
    else pf("(system command)\n");
 }
}

static cmdC cmdHelp(
    "help",
    "n: Command",
    "type 'help [cmd]' to obtain help on command [cmd].",
    cmdEHelp,
    0,
    0
);



//EOF =========================================================================
