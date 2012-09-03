/*
 UNIT_HERO.CPP		v0.01
 Units heroic stuff.

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
#include "main.h"

#include "game.h"
#include "unit.h"
#include "utype.h"

#include "colors.h"
#include "city.h"
#include "race.h"
#include "map.h"





/*=============================================================================
 * CAN BE HERO ?
 */
bool utypeC::CanBeHero()
{
 if(HeroBase) return false;
 if(!Next) return false;
 if(Next->HeroBase) return true;
 return false;
}





/*=============================================================================
 * HERO
 */
static double hrnd()
{
 return game->Rnd();
}



heroC::heroC(utypeC* type)
{
 heroC** h;
 for(h = &game->HerosHook; *h; h = &(*h)->Next);
 *h = this;
 Next = 0;

 Unit = 0;
 Bonuses = 0;
 Owner = 0;
 Type = type;
 Exp = 0;

 // generate name
 char* s = Type->Race->Language.Generate("NAMEM", hrnd);
 strncpy(Name, s, sizeof(Name));
}



heroC::~heroC()
{
 heroC** h;
 for(h = &game->HerosHook; *h && *h != this; h = &(*h)->Next);
 if(*h) *h = Next;

 //@@also return items to vault

 if(Unit) Unit->Remove();
}





/*=============================================================================
 * GET RANDOM HERO
 */
utypeC* raceC::GetHero(int rnd)
{
 int cnt = 0;
 for(utypeC* u = Units; u; u = u->Next) if(u->HeroBase) cnt++;

 cnt = rnd % cnt;

 for(utypeC* u = Units; u; u = u->Next) if(u->HeroBase)
    if(!cnt--) return u;
}





//EOF =========================================================================
