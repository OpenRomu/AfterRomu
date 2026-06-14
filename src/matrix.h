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
 * matrix class
 */

#ifndef __matrix_h__
#define __matrix_h__

#include "fastmath.h"
#include "geom.h"

class matrix_t
{
  public:
    matrix_t()
    {
        load_identity();
    }

    inline matrix_t &operator=(const matrix_t &s)
    {
        memcpy(m, s.m, sizeof(float) * 16);
        return *this;
    }

    inline void load_identity();
    inline void xrot(float angle);
    inline void yrot(float angle);
    inline void zrot(float angle);
    inline void scale(float x, float y, float z);

    inline void translate(float x, float y, float z);
    inline void translate(float *v);
    inline void Transpose(matrix_t &dest);

    inline matrix_t operator*(const matrix_t &s) const;
    inline void operator*=(const matrix_t &s)
    {
        *this = *this * s;
    }

    inline vec3_t operator*(const vec3_t &v) const;

    const float &operator[](int i) const
    {
        return m[i];
    }
    operator const float *(void) const
    {
        return m;
    }
    float m[16];
};

inline void matrix_t::load_identity()
{
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
}

inline void matrix_t::xrot(float angle)
{
    float c = fast_cos(angle);
    float s = fast_sin(angle);

    m[5] = c;
    m[6] = s;
    m[9] = -s;
    m[10] = c;
}

inline void matrix_t::yrot(float angle)
{
    float c = fast_cos(angle);
    float s = fast_sin(angle);

    m[0] = c;
    m[2] = -s;
    m[8] = s;
    m[10] = c;
}

inline void matrix_t::zrot(float angle)
{
    float c = fast_cos(angle);
    float s = fast_sin(angle);

    m[0] = c;
    m[1] = s;
    m[4] = -s;
    m[5] = c;
}

inline void matrix_t::scale(float x, float y, float z)
{
    m[0] = x;
    m[5] = y;
    m[10] = z;
}

inline void matrix_t::translate(float x, float y, float z)
{
    m[12] = m[0] * x + m[4] * y + m[8] * z + m[12];
    m[13] = m[1] * x + m[5] * y + m[9] * z + m[13];
    m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
    m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}

inline void matrix_t::translate(float *v)
{
    m[12] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12];
    m[13] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13];
    m[14] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14];
    m[15] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15];
}

inline matrix_t matrix_t::operator*(const matrix_t &s) const
{
    matrix_t r;
    r.m[0] = m[0] * s.m[0] + m[4] * s.m[1] + m[8] * s.m[2] + m[12] * s.m[3];
    r.m[1] = m[1] * s.m[0] + m[5] * s.m[1] + m[9] * s.m[2] + m[13] * s.m[3];
    r.m[2] = m[2] * s.m[0] + m[6] * s.m[1] + m[10] * s.m[2] + m[14] * s.m[3];
    r.m[3] = m[3] * s.m[0] + m[7] * s.m[1] + m[11] * s.m[2] + m[15] * s.m[3];
    r.m[4] = m[0] * s.m[4] + m[4] * s.m[5] + m[8] * s.m[6] + m[12] * s.m[7];
    r.m[5] = m[1] * s.m[4] + m[5] * s.m[5] + m[9] * s.m[6] + m[13] * s.m[7];
    r.m[6] = m[2] * s.m[4] + m[6] * s.m[5] + m[10] * s.m[6] + m[14] * s.m[7];
    r.m[7] = m[3] * s.m[4] + m[7] * s.m[5] + m[11] * s.m[6] + m[15] * s.m[7];
    r.m[8] = m[0] * s.m[8] + m[4] * s.m[9] + m[8] * s.m[10] + m[12] * s.m[11];
    r.m[9] = m[1] * s.m[8] + m[5] * s.m[9] + m[9] * s.m[10] + m[13] * s.m[11];
    r.m[10] = m[2] * s.m[8] + m[6] * s.m[9] + m[10] * s.m[10] + m[14] * s.m[11];
    r.m[11] = m[3] * s.m[8] + m[7] * s.m[9] + m[11] * s.m[10] + m[15] * s.m[11];
    r.m[12] = m[0] * s.m[12] + m[4] * s.m[13] + m[8] * s.m[14] + m[12] * s.m[15];
    r.m[13] = m[1] * s.m[12] + m[5] * s.m[13] + m[9] * s.m[14] + m[13] * s.m[15];
    r.m[14] = m[2] * s.m[12] + m[6] * s.m[13] + m[10] * s.m[14] + m[14] * s.m[15];
    r.m[15] = m[3] * s.m[12] + m[7] * s.m[13] + m[11] * s.m[14] + m[15] * s.m[15];

    return r;
}

inline vec3_t matrix_t::operator*(const vec3_t &v) const
{
    return vec3_t(m[0] * v[0] + m[1] * v[1] + m[2] * v[2], m[4] * v[0] + m[5] * v[1] + m[6] * v[2],
                  m[8] * v[0] + m[9] * v[1] + m[10] * v[2]);
}

inline void matrix_t::Transpose(matrix_t &dest)
{
    dest.m[0] = m[0];
    dest.m[1] = m[4];
    dest.m[2] = m[8];
    dest.m[3] = m[12];
    dest.m[4] = m[1];
    dest.m[5] = m[5];
    dest.m[6] = m[9];
    dest.m[7] = m[13];
    dest.m[8] = m[2];
    dest.m[9] = m[6];
    dest.m[10] = m[10];
    dest.m[11] = m[14];
    dest.m[12] = m[3];
    dest.m[13] = m[7];
    dest.m[14] = m[11];
    dest.m[15] = m[15];
}

#endif // __matrix_h__
