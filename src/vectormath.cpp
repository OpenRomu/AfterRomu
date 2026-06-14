//-----------------------------------------------------------------------------
// File: Vectormath.cpp
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#define D3D_OVERLOADS
#define STRICT
#include <math.h>
#include <stdio.h>
#include "vectormath.h"
#include "geom.h"

#define PI 3.14155

// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - vector describing direction of ray in world space
//         pOrigin - Origin of plane
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------
double intersectRayPlane(vec3_t rOrigin, vec3_t rVector, vec3_t pOrigin, vec3_t pNormal)
{

    double d = -(dot(pNormal, pOrigin));

    double numer = dot(pNormal, rOrigin) + d;
    double denom = dot(pNormal, rVector);

    if (denom == 0) // normal is orthogonal to vector, cant intersect
        return (-1.0f);

    return -(numer / denom);
}

// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - vector describing direction of ray in world space
//         sO - Origin of sphere
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------

double intersectRaySphere(vec3_t rO, vec3_t rV, vec3_t sO, double sR)
{

    vec3_t Q = sO - rO;

    double c = lengthOfVector(Q);
    double v = dot(Q, rV);
    double d = sR * sR - (c * c - v * v);

    // If there was no intersection, return -1
    if (d < 0.0)
        return (-1.0f);

    // Return the distance to the [first] intersecting point
    return (v - sqrt(d));
}

// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         a - first vertex in triangle
//         b - second vertex in triangle
//         c - third vertex in triangle
// Notes : Triangle should be defined in clockwise order a,b,c
// Return: TRUE if point is in triangle, FALSE if not.
// -----------------------------------------------------------------------

BOOL CheckPointInTriangle(vec3_t point, vec3_t a, vec3_t b, vec3_t c)
{

    double total_angles = 0.0f;

    // make the 3 vectors
    vec3_t v1 = point - a;
    vec3_t v2 = point - b;
    vec3_t v3 = point - c;

    normalizeVector(v1);
    normalizeVector(v2);
    normalizeVector(v3);

    total_angles += acos(dot(v1, v2));
    total_angles += acos(dot(v2, v3));
    total_angles += acos(dot(v3, v1));

    if (fabs(total_angles - 2 * PI) <= 0.005)
        return (TRUE);

    return (FALSE);
}

// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------

vec3_t closestPointOnLine(vec3_t &a, vec3_t &b, vec3_t &p)
{

    // Determine t (the length of the vector from ‘a’ to ‘p’)
    vec3_t c = p - a;
    vec3_t V = b - a;

    double d = lengthOfVector(V);

    normalizeVector(V);
    double t = dot(V, c);

    // Check to see if ‘t’ is beyond the extents of the line segment
    if (t < 0.0f)
        return (a);
    if (t > d)
        return (b);

    // Return the point between ‘a’ and ‘b’
    // set length of V to t. V is normalized so this is easy
    V[0] = V[0] * t;
    V[1] = V[1] * t;
    V[2] = V[2] * t;

    return (a + V);
}

// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
// Input : a - first vertex in triangle
//         b - second vertex in triangle
//         c - third vertex in triangle
//         p - point we wish to find closest point on triangle from
// Notes :
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------

vec3_t closestPointOnTriangle(vec3_t a, vec3_t b, vec3_t c, vec3_t p)
{

    vec3_t Rab = closestPointOnLine(a, b, p);
    vec3_t Rbc = closestPointOnLine(b, c, p);
    vec3_t Rca = closestPointOnLine(c, a, p);

    double dAB = lengthOfVector(p - Rab);
    double dBC = lengthOfVector(p - Rbc);
    double dCA = lengthOfVector(p - Rca);

    double min = dAB;
    vec3_t result = Rab;

    if (dBC < min)
    {
        min = dBC;
        result = Rbc;
    }

    if (dCA < min)
        result = Rca;

    return (result);
}

// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere
// Notes :
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------

BOOL CheckPointInSphere(vec3_t point, vec3_t sO, double sR)
{

    float d = lengthOfVector(point - sO);

    if (d <= sR)
        return TRUE;
    else
        return FALSE;
}

// ----------------------------------------------------------------------
// Name  : tangentPlaneNormalOfEllipsoid()
// Input : point - point we wish to compute normal at
//         eO - Origin of ellipsoid
//         eR - radius vector of ellipsoid
// Notes :
// Return: a unit normal vector to the tangent plane of the ellipsoid in the point.
// -----------------------------------------------------------------------
vec3_t tangentPlaneNormalOfEllipsoid(vec3_t point, vec3_t eO, vec3_t eR)
{

    vec3_t p = point - eO;

    double a2 = eR[0] * eR[0];
    double b2 = eR[1] * eR[1];
    double c2 = eR[2] * eR[2];

    vec3_t res;
    res[0] = p[0] / a2;
    res[1] = p[1] / b2;
    res[2] = p[2] / c2;

    normalizeVector(res);
    return (res);
}

// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify
//         pO - Origin of plane
//         pN - Normal to plane
// Notes :
// Return: One of 3 classification codes
// -----------------------------------------------------------------------

DWORD classifyPoint(vec3_t point, vec3_t pO, vec3_t pN)
{

    vec3_t dir = pO - point;
    double d = dot(dir, pN);

    if (d < -0.001f)
        return PLANE_FRONT;
    else if (d > 0.001f)
        return PLANE_BACKSIDE;

    return ON_PLANE;
}
