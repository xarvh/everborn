/*
 PARTICLES.H		v0.00
 Battle particles: projectiles, special effects, dust, blood....

 Copyright (c) 2007 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#ifndef H_FILE_PARTICLES
#define H_FILE_PARTICLES
//=============================================================================

#define PARTICLE_ARROW_SPEED	2



/*============================================================================
 * PARTICLES
 */
    //dependencies
    class battleC;
class particleC { public:
    virtual void Display() = 0;
    virtual void Think() = 0;

    float X;
    float Y;
    float Z;

    battleC* Battle;
    particleC* Next;
    particleC(battleC* battle, float x, float y);
    ~particleC();
};



// blood
    class unitC;
class particleBloodC: public particleC { public:
    void Display();
    void Think();

    unsigned short Zrot;
    unsigned short Size;

    particleBloodC(unitC* u);
};



// arrow
class particleArrowC: public particleC { public:
    void Display();
    void Think();

    unitC* Owner;
    unitC* Target;
    unsigned Landed;
    unsigned Figures;

    particleArrowC(unitC* u, unitC* target);
};

class particleLandedArrowC: public particleC { public:
    void Display();
    void Think();

    unitC* Target;
    unsigned Age;
    float Angle;

    particleLandedArrowC(unitC* u, float angle);
};




#endif
//EOF =========================================================================
