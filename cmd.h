/*
 CMD.H			v0.30
 Command system.

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
#ifndef H_FILE_CMD
#define H_FILE_CMD
//=============================================================================

#define CMD_MAX_LENGHT		100
#define CMD_ARGSTRING_BF	CMD_MAX_LENGHT

class cmdC { public:
    char*	Name;		// command name, to be parsed
    char*	Syntax;		// formatted syntax
    char*	Help;		// help text, describing what the command does
				// 0 means 'system command'!
    int	(*Exec)();		// local part of the command, executed only by cmd_bin and cmd_str
    int	(*Game)();		// if this is present, the command uses it to modify the course of the game
    int	(*Check)();		// launched by server before cmd->game(), to validate cmd: if it returns non-zero, the cmd is ignored

    void*	Obj;		// spell cmds only, stores spell reference

    cmdC*	Next;
    cmdC(char*, char*, char*, int (*)(), int(*)(), int (*)());
};

typedef struct argS {		// holds a single cmd argument: used just for arg0, arg1, arg2..... globals //
    float	f;
    long	i;
//    unsigned	u;
    char	s[CMD_ARGSTRING_BF];
    void*	p;
    bool	miss;		// a non-mandatory argument is not present
} argT;



typedef char* (*cmd_syntax_t)(argT *, char *);
class cmdSyntaxC { public: cmdSyntaxC(char k, cmd_syntax_t re); };



/*=============================================================================
 * CMD.CPP
 */
extern cmdC* arg_cmd;
extern argT arg0;
extern argT arg1;
extern argT arg2;
extern argT arg3;
extern argT arg4;
extern argT arg5;
extern argT arg6;
extern argT arg7;
extern argT* cmd_args[];


char* cmd_copy_str(char*, char*);
cmdC* cmdParse(char* str);
cmdC* cmdInterprete(char* s, char* fe);
int cmd(const char* fmt, ...);
int cmdInit();
int cmdFile(char* fn);



/*=============================================================================
 * cmd_exec.c
 */
class brainC;
brainC* cmdGetBrain();
int cmdGetBrainId();
int cmdSelectBrain(int b);
int cmdBrainIdToChar(int id);
int cmdCharToBrainId(char c);
int cmd_execSendToServer(char* bf);

int cmdFromServer(char* s);
int cmdFromClient(char* s);



#endif
//EOF =========================================================================
