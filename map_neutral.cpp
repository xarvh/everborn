/*
 MAP_NEUTRAL.CPP	v0.01
 Nodes and lairs and towers and stuff...

 Copyright (c) 2005 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include "map.h"
#include "spell.h"





/*=============================================================================
 * NEUTRAL
 */
neutralC::neutralC(cellC* c, realmC* r, bool node)
{
 if(c->Neutral) throw "neutralC::neutralC: cell has already a neutral";
 c->Neutral = this;
 Cell = c;

 Owner = 0;
 Realm = r;
 Node = node;
 Rating = 5;
 Status = 0;
 Ani = 0;
}

neutralC::~neutralC()
{
 Cell->Neutral = 0;
}



void neutralC::Think()
{
 if(Owner) return;

// count unOwned creatures
// if not enough spawn something from your realm
 Status++;
}





//EOF =========================================================================
