/* 
 * HL rendering engine
 * Copyright (c) 2000,2001 Bart Sekura
 *
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * timer: currently using multimedia routines
 * XXX add perf counter support
 */

#ifndef __timer_h__
#define __timer_h__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

class timer_t {
public:
    timer_t() {
        timeBeginPeriod(1); 
        freq = 1/1000.0;
        s = 0;
    }
    ~timer_t() {
        timeEndPeriod(1);
    }

    void start() { s = timeGetTime(); }
    double tick() {
        current_time = (double)(timeGetTime()-s)*freq;
        return current_time;
    }

    double time() const { return current_time; }

private:
    DWORD   s;
    double  freq;
    double  current_time;
};

#endif // __timer_h__
