/*
 GSELECT.CPP		v0.00
 Selection from fake GL rendering.

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


void gsColor3d(double r, double g, double b);
void gsColor3ub(unsigned char r, unsigned char g, unsigned char b);

bool gsOff();
bool gsOn();
void gsBegin();
void gsPush(void* obj);
void* gsEnd(unsigned x, unsigned y);



//EOF =========================================================================
