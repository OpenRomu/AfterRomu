//-----------------------------------------------------------------------------
// File: Vectormath.h
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------
#ifndef VECTORMATH_H
#define VECTORMATH_H
#include <d3d.h>
#include "geom.h"
#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT 0x000010
#define ON_PLANE 0x000100

// basic vector operations (inlined)
inline float dot(vec3_t& v1, vec3_t& v2) { return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]); }

inline void normalizeVector(vec3_t& v)
{
    float len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

inline double lengthOfVector(vec3_t v) { return (sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])); }

inline void setLength(vec3_t& v, float l)
{
    float len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] *= l / len;
    v[1] *= l / len;
    v[2] *= l / len;
}

inline BOOL isZeroVector(vec3_t& v)
{
    if ((v[0] == 0.0f) && (v[1] == 0.0f) && (v[2] == 0.0f))
        return TRUE;

    return FALSE;
}

inline vec3_t wedge(vec3_t v1, vec3_t v2)
{
    vec3_t result;

    result[0] = (v1[1] * v2[2]) - (v2[1] * v1[2]);
    result[1] = (v1[2] * v2[0]) - (v2[2] * v1[0]);
    result[2] = (v1[0] * v2[1]) - (v2[0] * v1[1]);

    return (result);
}

// ray intersections. All return -1.0 if no intersection, otherwise the distance along the
// ray where the (first) intersection takes place
double intersectRayPlane(vec3_t rOrigin, vec3_t rVector, vec3_t pOrigin, vec3_t pNormal);
double intersectRaySphere(vec3_t rO, vec3_t rV, vec3_t sO, double sR);

// Distance to line of triangle
vec3_t closestPointOnLine(vec3_t& a, vec3_t& b, vec3_t& p);
vec3_t closestPointOnTriangle(vec3_t a, vec3_t b, vec3_t c, vec3_t p);

// point inclusion
BOOL CheckPointInTriangle(vec3_t point, vec3_t a, vec3_t b, vec3_t c);
BOOL CheckPointInSphere(vec3_t point, vec3_t sO, double sR);

// Normal generation
vec3_t tangentPlaneNormalOfEllipsoid(vec3_t point, vec3_t eO, vec3_t eR);

// Point classification
DWORD classifyPoint(vec3_t point, vec3_t pO, vec3_t pN);

#endif // VECTORMATH_H