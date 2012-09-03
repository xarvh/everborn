/*
 ENV.CPP		v0.11
 Environment variables.

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



#include <stdlib.h>
#include <string.h>
#include "main.h"



typedef struct env_s {
    struct env_s* next;

    char* name;
    char* value;

    char* (*call)(char* value);
} env_t;



static env_t* env_hook = NULL;





/*=============================================================================
 * PARSE
 */
static env_t* envParse(char* name)
{
 env_t* e;

 for(e = env_hook; e && strcmp(e->name, name); e = e->next);
 return e;
}





/*=============================================================================
 * GET
 */
char* envGet(char* var)
{
 env_t *e;
 e = envParse(var);
 if(!e) return NULL;
 return e->value;
}





/*=============================================================================
 * GET NAME
 */
char* envGetName(int pos)
{
 env_t *e;
 for(e = env_hook; e && pos--; e = e->next);

 if(e) return e->name;
 return NULL;
}





/*=============================================================================
 * SET
 */
int envSet(char *var, char *val)
{
 env_t *e;

 e = envParse(var);
 if(!e) return -1;

 if(e->call) val = e->call(val);

 if(e->value) free(e->value);
 e->value = (char*)malloc(strlen(val)+1);
 strcpy(e->value, val);

 return 0;
}



/*=============================================================================
 * BOOLEANS
 */
bool envIsTrue(char* n)
{
 return envGet(n)[0] == 'y';
}



static char* envBoolCall(char* v)
{
 bool b = false;
 if(atof(v) != 0.) b = true;
 if(!strcasecmp(v, "true")) b = true;
 if(!strcasecmp(v, "yes")) b = true;

 if(b)	return "yes";
 else	return "no";
}





/*=============================================================================
 * ADD
 */
static void envAdd(char *name, char *value_default, char* (*call)(char *value))
{
 env_t **s, *e;

 for(s = &env_hook; *s; s = &(*s)->next);
 *s = e = (env_t*)malloc(sizeof(env_t));

 e->next = NULL;
 e->name = (char*)malloc(strlen(name) + 1); strcpy(e->name, name);
 e->value = NULL;
 e->call = call;

 envSet(e->name, value_default);
}



envC::envC(char* name, char* value_default, char* (*call)(char *value))
{
 envAdd(name, value_default, call);
}

envC::envC(char* name, char* value_default)
{
 envAdd(name, value_default, 0);
}

envC::envC(char* name, bool value_default)
{
 envAdd(name, (char*)(value_default?"yes":"no"), envBoolCall);
}



//EOF==========================================================================
