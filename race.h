/*
 RACE.H			v0.11
 Races.

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
#ifndef H_FILE_RACE
#define H_FILE_RACE
//=============================================================================
#include "data.h"
#include "flesh.h"


#define RACE_ID_BF		20
#define RACE_NAME_BF		20
#define RACE_INFO_BF		600
#define RACE_UNIT_BF		14	// 12 + catapult + mage player
#define RACE_MAX_SPECIAL_BLDS	3


#define RACE_REALM_LIMIT	12	// research limit to be a realm race
#define RACE_MINERS		(1<<0)
#define RACE_FARMERS		(1<<1)
#define RACE_WORKERS		(1<<2)
#define RACE_POPMANA		(1<<3)
#define RACE_LAND		(1<<4)
#define RACE_SHORE		(1<<5)
#define RACE_SEA		(1<<6)
#define RACE_FOREST		(1<<7)





/*=============================================================================
 * LANGUAGE
 *
 * format: NAME:exp0,exp1,exp2,exp3, ...
 */
class dcstrC { public:
    dcstrC*	Next;
    char*	Str;

    //
    char*	GetRandom(double rnd);

    //
    dcstrC(char* str);
    ~dcstrC();
};



class dictionaryC { public:
    dcstrC* StrHook;

    //
    dictionaryC();
    ~dictionaryC();

    //
    bool AddStr(char* str);
    char* Generate(char* type, double (*rnd)());
    char* Build(char* type, char* outbf, char* bfend, double (*rnd)());
};





/*=============================================================================
 * struct: RACE
 */
class blockC;
class utypeC;
class bldC;

class raceC { public:
    raceC*	Next;

    char	Id[RACE_ID_BF];
    char	Name[RACE_NAME_BF];
    char	Info[RACE_INFO_BF];

    dictionaryC	Language;
    dataC	City;		// city gfx
    dataC	Land;		// city overland gfx

    fleshC	Farmer;
    fleshC	Worker;
    fleshC	Rebel;

    signed char	Growth;
    signed char __unused__;

    short	Spc;

    blockC*	Flesh;
    utypeC*	Units;
    bldC**	Blds;

    //
    raceC();
    ~raceC();


    utypeC*	GetUtype(int id);
    utypeC*	GetSpearmen();
    utypeC*	GetSettlers();
    utypeC*	GetPlayerMage();
    utypeC*	GetHero(int rnd);

    char*	Write(char* w);
};





/*=============================================================================
 * race.cpp
 */
raceC*	raceGet(int id);
raceC*	raceGetRandom(long seed);
int	racesCount();



/*=============================================================================
 * txt_race.cpp
 */
raceC* raceLoad(char* bf, char* e);



#endif
//EOF =========================================================================
