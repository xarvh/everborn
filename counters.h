/*
 COUNTERS.H		v0.01
 Counters

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

// stats
#define CNT_MELEE	2
#define CNT_POISON	6
#define CNT_LIFESTEAL	7
#define CNT_BREATHE	8
#define CNT_RANGE	10
#define CNT_HITBONUS	15
#define CNT_DEFENSE	16
#define CNT_RESISTANCE	19
#define CNT_HITS	20

// res
#define CNT_FOOD	21
#define CNT_WORK	24
#define CNT_GOLD	27
#define CNT_MANA	30
#define CNT_KNOW	32


void cntDrawStat(int id, int tot, int plus, int minus);
void cntDrawRes(int id, int tot);
void cntDrawUpkeep(int f, int g, int m);



signed countersTotal();
int countersZero();
int countersAdd(char *name, signed cnt);
int countersAddP(char *name, signed cnt, signed p);



//EOF =========================================================================

