/*
 TIME.CPP		v0.01
 Used for profiling and benchmarking.

 Copyright (c) 2006 Francesco Orsenigo <francesco.orsenigo@gmail.com>

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
#include <stdio.h>
#include <sys/time.h>
#include "main.h"



/*=============================================================================
 * GET
 */
unsigned long timeGet()
{
 struct timeval now;
 gettimeofday(&now, 0);
 return now.tv_sec*1000 + now.tv_usec/1000;
}



/*=============================================================================
 * CLASS: TIMER
 */
void timerC::StartFrame()	{ Start(); Frames++; }
void timerC::Start()		{ TStart = timeGet(); }
void timerC::Stop()		{ Eta += timeGet() - TStart; }
float timerC::FPS()
{
 float r = Frames*1000./Eta;
 if(Frames > 10) Frames = 0;
 return r;
}

timerC::timerC(char* name)
{
 Name = name;
 Frames = 0;
 Eta = 0;
}

timerC::~timerC()
{
 if(Eta)
    if(Frames)	printf("%10s timer: %8.1ffps (if only this was performed)\n", Name, FPS());
    else	printf("%10s timer: %6dms\n", Name, Eta);
}





/*=============================================================================
 * TEMPO
 */
bool tempoC::operator()()
{
 unsigned long t = timeGet();
 if(t - LastTime < Interval) return false;
 LastTime = t;
 return true;
}

tempoC::tempoC(long interval)
{
 LastTime = 0;
 Interval = interval;
}




//EOF =========================================================================
