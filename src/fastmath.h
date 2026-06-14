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
 * hopefully fast math routines
 */

#ifndef __fastmath_h__
#define __fastmath_h__

#include <math.h>

#define __PI       (3.14159265358979323846264338327950288f)
#define __DEG2RAD  (__PI / 180)

#define deg2rad(x) ((x) * __DEG2RAD)
#define rad2deg(x) ((x) / __DEG2RAD)

inline static float fast_cos(float angle)
{
    return cosf(deg2rad(angle));
}

inline static float fast_sin(float angle)
{
    return sinf(deg2rad(angle));
}

#endif /* __fastmath_h__ */