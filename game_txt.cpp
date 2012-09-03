/*
 TXT_GAME.CPP		v0.01
 Text game io.

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
#include "city.h"
#include "game.h"
#include "unit.h"
#include "utype.h"
#include "race.h"
#include "txt.h"
#include "spell.h"




/*=============================================================================
 * GAME WRITE
 */
char* gameC::Write(char* w)
{
 // blds
 w = bldWriteBase(w);


 // write all blocks
 for(blockC* b = game->Creatures.Flesh; b; b = b->Next)
 {
    w += sprintf(w, "[BLO]\n");
    w = fleshWriteBlock(w, b);
    *w++ = '\n';
 }


 // write all units
 w = unitWriteAll(w, game->Creatures.Units, game->Creatures.Blds);


 // terminate
 *w++ = '\0';

 return w;
}





/*=============================================================================
 * GAME READ
 *
 * Load main game data from buffer.
 */
int gameC::Read(char* bf, char* e)
{
 //
 raceC* cre = &game->Creatures;
 sprintf(cre->Id, "GAME");
 sprintf(cre->Name, "Creatures");



 // read base buildings
 // stored for convenience in game->Creatures, so that also that has a
 // bld list
 cre->Blds = bldLoadBase(bf, e);
 if(!cre->Blds) return -1;



 // creatures blocks
 char* s = bf;
 blockC **b = &cre->Flesh;
 while(s = txtSearch("[BLO]", s))
 {
    *b = fleshLoadBlock(s, e);
    if(*b == NULL) return -1;
    b = &(*b)->Next;
 }    



 // creatures unit
 cre->Units = unitLoadAll(bf, e, cre->Flesh, cre->Blds);
 if(!cre->Units) return -1;


 // set all utypeC::Race = game->Creatures
 for(utypeC* u = cre->Units; u; u = u->Next) u->Race = cre;

 // check that all summon spell have their target
 // and copy spell name and description to the unit name and info
 for(spellC* s = spellsHook; s; s = s->SpellNext)
    if(s->Type == &spellTypeSummon && s->Object != SPELL_OBJ_NONE)
	if(utypeC* u = cre->GetUtype(s->Object))
	{
	    strncpy(u->Name, s->Name, UNIT_NAME_BF-1);
	    strncpy(u->Info, s->Description, UNIT_INFO_BF-1);
	    u->Mana = s->UpkeepCost;
	}
	else
	{
	    sprintf(e, "no %s unit (#%d)", s->Name, s->Object);
	    return -1;
	}

 //
 return 0;
}





//EOF =========================================================================
