/*
   Copyright (C) 2000 Nate Miller nkmiller@calpoly.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
/*

*/
#ifndef __TICKERH__
#define __TICKERH__

#pragma comment(lib, "winmm.lib") // blah, but it works...

#include "utility.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

struct ticker_t {
    static void Reset(void) { start = timeGetTime(); }
    static uint PassedI(void) { return (timeGetTime() - start); }
    static double Passed(void) { return ((timeGetTime() - start) / 1000.0); }

protected:
    static DWORD start;
};

struct frameTimer_t {
    frameTimer_t()
    {
        frames = 0;
        now = last = passed = 0;
        fps = 0;
    }
    double& GetNow(void) { return now; }
    double& GetLast(void) { return last; }
    double& GetPassed(void) { return passed; }
    int GetFrames(void) { return frames; }
    float GetDelta(void) { return (float)(now - last); }
    float GetFPS(void) { return fps; }
    void Frame(void);

protected:
    int frames;
    float fps;
    double now;
    double last;
    double passed;
};

#endif