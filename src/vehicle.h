// vehicle.h: interface for the vehicle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEHICLE_H__21CC3A21_FE6D_440B_8C01_E12B364E5940__INCLUDED_)
#define AFX_VEHICLE_H__21CC3A21_FE6D_440B_8C01_E12B364E5940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// #include <iostream>
//
#include "geom.h"

#include "texture.h"
// "Object" data structure

// ********** MESH DATA **********
typedef struct _point2
{
    double x, y;
} Point2;

typedef struct _point3
{
    double x, y, z;
} Point3;

typedef struct _obj
{
    Point3 *vertices;
    long *v_idx;
    Point3 *normals;
    long *n_idx;
    Point2 *uvs;
    int num_faces;
} Obj;

class vehicle
{
  public:
    vehicle();
    void init();
    virtual ~vehicle();
    void Render(vec3_t pos, vec3_t AxeX, vec3_t AxeY, vec3_t AxeZ);
    void drawVector(const vec3_t pos, const vec3_t dir);
    vec3_t AngRot;
    float Car_Matrix[16];
    GLuint m_texid;
    void SetTexture(GLuint texid)
    {
        m_texid = texid;
    };

  protected:
    Obj sph01;
};

class roue
{
  public:
    roue();
    virtual ~roue();
    void Render(vec3_t pos, vec3_t AxeX, vec3_t AxeY, vec3_t AxeZ, float vel, float rot);
    vec3_t AngRot;
    float tourne;
    GLuint m_texid;
    void SetTexture(GLuint texid)
    {
        m_texid = texid;
    };

  protected:
    Obj sph01;
};

#endif // !defined(AFX_VEHICLE_H__21CC3A21_FE6D_440B_8C01_E12B364E5940__INCLUDED_)
