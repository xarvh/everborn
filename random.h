/*
 RANDOM.H		v0.00
 Game portable random number generators.

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
#ifndef H_FILE_RANDOM
#define H_FILE_RANDOM
//=============================================================================

class randomC { public:
    unsigned long Seed;

    unsigned Raw16()
    {
	Seed = 1664525*Seed + 1013904233;
	return (Seed>>8)&0xffff;
    }

    double operator()(void)		{ return Raw16() / 65536.; }
    unsigned operator()(short m)	{ return Raw16() % m; }

    randomC()		{ Seed = 0;}
    randomC(long seed)	{ Seed = seed;}
};



#endif
//EOF =========================================================================
