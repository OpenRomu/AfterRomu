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
 * vector and plane
 */

#ifndef __geom_h__
#define __geom_h__

#include <math.h>

///////////////////////////////////////////////////////////////

class vec3_t {
public:
    //ctors
    vec3_t() { v[0]=v[1]=v[2]=0; }
    vec3_t(float x, float y, float z) {
        v[0]=x, v[1]=y, v[2]=z;
    }
    vec3_t(const float* _v) {
        v[0]=_v[0], v[1]=_v[1], v[2]=_v[2];
    }
    vec3_t(const vec3_t& _v) {
        v[0]=_v.v[0],v[1]=_v.v[1],v[2]=_v.v[2];
    }

    // assignment operators
    vec3_t& operator=(const vec3_t& _v) {
        v[0] = _v.v[0], v[1] = _v.v[1], v[2] = _v.v[2];
        return *this;
    }
    vec3_t& operator=(const float* _v) {
        v[0] = _v[0], v[1] = _v[1], v[2] = _v[2];
        return *this;
    }

    ////////////
    vec3_t& operator+=(const vec3_t& _v) {
        v[0] += _v.v[0], v[1] += _v.v[1], v[2] += _v.v[2];
        return *this;
    }
    vec3_t& operator-=(const vec3_t& _v) {
        v[0] -= _v.v[0], v[1] -= _v.v[1], v[2] -= _v.v[2];
        return *this;
    }
    vec3_t& operator*=(const vec3_t& _v) {
        v[0] *= _v.v[0], v[1] *= _v.v[1], v[2] *= _v.v[2];
        return *this;
    }
    vec3_t& operator*=(float x) {
        v[0]*=x,v[1]*=x,v[2]*=x;
        return *this;
    }
    vec3_t& operator/=(const vec3_t& _v) {
        v[0] /= _v.v[0], v[1] /= _v.v[1], v[2] /= _v.v[2];
        return *this;
    }
    vec3_t& operator/=(float x) {
        v[0]/=x,v[1]/=x,v[2]/=x;
        return *this;
    }

    //////////////////////////////////////////////////
    vec3_t operator+(const vec3_t& _v) const {
        return vec3_t(v[0]+_v.v[0],v[1]+_v.v[1],v[2]+_v.v[2]);
    }
    vec3_t operator-(const vec3_t& _v) const {
        return vec3_t(v[0]-_v.v[0],v[1]-_v.v[1],v[2]-_v.v[2]);
    }
    vec3_t operator*(const vec3_t& _v) const {
        return vec3_t(v[0]*_v.v[0],v[1]*_v.v[1],v[2]*_v.v[2]);
    }
    vec3_t operator*(float d) const {
        return vec3_t(v[0]*d,v[1]*d,v[2]*d);
    }
    vec3_t operator/(const vec3_t& _v) const {
        return vec3_t(v[0]/_v.v[0],v[1]/_v.v[1],v[2]/_v.v[2]);
    }
    vec3_t operator/(float d) const {
        d = 1.0f/d;
        return vec3_t(v[0]*d,v[1]*d,v[2]*d);
    }

    const float &operator[](int i) const { return v[i]; }
    float &operator[](int i) { return v[i]; }
    operator float*(void) { return v; }

    vec3_t operator-(void) const {
        return vec3_t(-v[0],-v[1],-v[2]);
    }

    // stuff
    float len() const {
        return (float) sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
    }

    float lenf() const {
        return (float) sqrtf((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
    }

    void normalize() {
        float l = this->len();
        if(l) {
            l = 1.0f / l;
            v[0] *= l;
            v[1] *= l;
            v[2] *= l;
        }
    }

    vec3_t cross(const vec3_t& _v) const {
        return vec3_t((v[1]*_v.v[2]) - (v[2]*_v.v[1]),
                      (v[2]*_v.v[0]) - (v[0]*_v.v[2]),
                      (v[0]*_v.v[1]) - (v[1]*_v.v[0]));
    }

    float dot(const vec3_t& _v) const {
        return v[0]*_v.v[0]+v[1]*_v.v[1]+v[2]*_v.v[2];
    }

    // misc
    bool zero() const {
        if(v[0]==0&&v[1]==0&&v[2]==0) return true;
        return false;
    }
 
	

private:
    float v[3];
};

typedef vec3_t vertex_t;

#define EPSILON (0.0003f)

class plane_t {
public:
    plane_t() : d(0.0f) {}
    plane_t(const vertex_t& a, const vertex_t& b, const vertex_t& c) {
        vertex_t p = b-a;
        vertex_t q = c-a;
        n = p.cross(q);
        n.normalize();
        d = -n.dot(a);
    }
    plane_t(const plane_t& p) {
        n = p.normal();
        d = p.dist();
    }
    plane_t(const vec3_t& _n, const vec3_t& pt) {
        n = _n;
        d = -n.dot(pt);
    }
    plane_t(const vec3_t& _n, float dist) {
        n = _n;
        d = dist;
    }

    void perpendicular(vertex_t& a, vertex_t& b, vec3_t& normal) {
        *this = plane_t(a,b,b+normal);
    }

    plane_t perpendicular(const vertex_t& a, const vertex_t& b) const {
        return plane_t(a,b,b+n);
    }

    float dist_to_point(const vec3_t& pt) const {
        return n.dot(pt)+d;
    }

    bool intersect(const vec3_t& start, 
                   const vec3_t& dir, 
                   const vec3_t& pt, 
                   vec3_t& ret) const
    {
        float denom = n.dot(dir);
        if( denom <= -EPSILON || denom >= EPSILON) { //
            float numer = n.dot(pt-start);
            float u = numer/denom;
            if(u > 0 && u <= 1.0f) {
                ret = start + dir*u;
                return true;
            }
        }
        return false;
    }
	bool intersect2(const vec3_t& start, 
                   const vec3_t& dir, 
                   const vec3_t& pt, 
                   vec3_t& ret) const
    {
        float denom = n.dot(dir);
        //if(denom <= -0.001f) {// || denom >= 0.001f
		if((denom <= -0.00001f) || (denom >= 0.00001f) )
		{
            float numer = n.dot(pt-start);
            float u = numer/denom;
            if(u > 0 && u <= 1.0f) {
                ret = start + dir*u;
                return true;
            }
        }
        return false;
    }

    // accessors
    const vec3_t& normal() const { return n; }
    float dist() const { return d; }

private:
    vec3_t    n;
    float    d;
};

#endif // __geom_h__
