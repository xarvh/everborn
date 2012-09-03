/*
 FLESH.H		v0.15
 3d Models.

 Copyright (c) 2004-2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#ifndef H_FILE_FLESH
#define H_FILE_FLESH
//=============================================================================



#define FLESH_VERTEXxBLOCK	(('Z'-'A'+1) + ('z'-'a'+1))
#define FLESH_STRIPS_BF		(FLESH_VERTEXxBLOCK*4)




// body parts
#define FLESH_HEAD	0
#define FLESH_TORSO	1
#define FLESH_TAIL	2

#define FLESH_RUARM	3
#define FLESH_LUARM	4
#define FLESH_RLARM	5
#define FLESH_LLARM	6
#define FLESH_RHAND	7
#define FLESH_LHAND	8
#define FLESH_RWEP	9
#define FLESH_LWEP	10


#define FLESH_RULEG	11
#define FLESH_LULEG	12
#define FLESH_RLLEG	13
#define FLESH_LLLEG	14
#define FLESH_RFOOT	15
#define FLESH_LFOOT	16

#define FLESH_RULIMB	17
#define FLESH_LULIMB	18
#define FLESH_RLLIMB	19
#define FLESH_LLLIMB	20
#define FLESH_TOTAL	21




/*=============================================================================
 * VERTEX
 */
class colorC;
class vertexC { public:
    short		X, Y, Z;	// vertex coords
    short		TX, TY;		// texture coords
    unsigned char	R, G, B;	// vertex color

    //
    vertexC();
    bool IsUsed();
    void Draw(colorC* c);
};





/*=============================================================================
 * BLOCK
 */
class colorC;
class blockC { public:
    blockC*	Next;			// stored as dyn list

    char	Name[32];
    long	Part;			// bitmask
    long	PartDefault;		// bitmask

    vertexC	Vertex[FLESH_VERTEXxBLOCK];
    char	Strips[FLESH_STRIPS_BF]; // double \0-term string list

    short	Length;
    short	Width;

    //
    blockC();
    ~blockC();
    void Draw(colorC* color);
};





/*=============================================================================
 * FLESH
 */
class fleshC { public:
    // these blocks MUST follow the FLESH_ sequence:
    // blocks will be assigned using BlocksStart as base address.
    blockC	*BlocksStart[0];
    blockC	*head;
    blockC	*torso;
    blockC	*tail;

    blockC	*ruarm;	// Right Upper Arm
    blockC	*luarm;
    blockC	*rlarm;
    blockC	*llarm;	// Left Lower Arm
    blockC	*rhand;
    blockC	*lhand;
    blockC	*rwep;
    blockC	*lwep;

    blockC	*ruleg;
    blockC	*luleg;
    blockC	*rlleg;
    blockC	*llleg;
    blockC	*rfoot;
    blockC	*lfoot;

    blockC	*rulimb;	// optional limbs: wings, 3rd leg pair o whatever...
    blockC	*lulimb;
    blockC	*rllimb;
    blockC	*lllimb;	// Left Lower Limb
    blockC	*BlocksEnd[0];

    double	width;

    fleshC();
};





/*=============================================================================
 * FLESH
 */
class colorC;
void fleshDraw(fleshC* m, double* angle, colorC* color);





/*=============================================================================
 * FLESH_TXT
 */
blockC* fleshLoadBlock(char* s, char* err);
char* fleshWriteBlock(char* w, blockC* b);
char* fleshRead(char* s, char* e, fleshC* flesh, blockC* blockL);
char* fleshWrite(char* w, fleshC* flesh);
void fleshApplyDefaultParts(fleshC* f, blockC* l);
unsigned fleshCharToVertex(char c);





#endif
//EOF =========================================================================
