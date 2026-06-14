// vehicle.cpp: implementation of the vehicle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vehicle.h"
#include <GL/glu.h>
#include "image.h"
#include "fastmath.h"
#include "jeep1.h"
#include "rouemodel.h"

#define Q_PI 3.14159265358979323846
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

vehicle::vehicle() : m_texid(0)
{
    sph01.vertices = main_vertex;
    sph01.v_idx = main_vidx;
    sph01.normals = main_normal;
    sph01.n_idx = main_nidx;
    sph01.uvs = main_uv;
    sph01.num_faces = (sizeof(main_vidx) / sizeof(long));
    // Image img;
    // img.load("data/env/jeep1.bmp");
    // m_tex = new Texture(img.data(), img.width(), img.height(), GL_RGBA);
}
void vehicle::init()
{
}

//	static GLfloat angle1 = 0.0;
//	angle1 += 1.0;
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//	glLoadIdentity();
//	glRotatef( angle1 / 3.0, 1.0, 0.0, 0.0 );

//	glRotatef( -90.0,       0.0, 1.0, 0.0 );
//	 glTranslatef(0.0,0.0,-5.0);
//
/*
void gl_printf( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha,
                GLint x, GLint y, GLuint font, const char *format, ... )
{
    va_list argp;
    char text[256];

    va_start( argp, format );
    vsprintf( text, format, argp );
    va_end( argp );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();

        glLoadIdentity();
        gluOrtho2D( 0.0, (GLdouble) width,
                    0.0, (GLdouble) height );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glColor4f( red, green, blue, alpha );
        glRasterPos2i( x, y );
        glListBase( font );
        glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}
*/

void vehicle::drawVector(const vec3_t pos, const vec3_t dir)
{
    vec3_t po[8];

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColor4f(0, 1, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3fv(pos + vec3_t(2.0f, 0.0f, 0.0f));
    glVertex3fv(pos + dir * 50.0f);
    glVertex3fv(pos - vec3_t(2.0f, 0.0f, 0.0f));
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}

void vehicle::Render(vec3_t pos, vec3_t AxeX, vec3_t AxeY, vec3_t AxeZ)
{

    float mat[16];

    mat[0] = AxeX[0];
    mat[1] = AxeX[1];
    mat[2] = AxeX[2];
    mat[3] = 0.0f;
    mat[4] = AxeY[0];
    mat[5] = AxeY[1];
    mat[6] = AxeY[2];
    mat[7] = 0.0f;
    mat[8] = AxeZ[0];
    mat[9] = AxeZ[1];
    mat[10] = AxeZ[2];
    mat[11] = 0.0f;
    mat[12] = pos[0];
    mat[13] = pos[1];
    mat[14] = pos[2];
    mat[15] = 1.0f;

    /*mat[0]=AxeX[0];
    mat[1]=0.0f;
    mat[2]=AxeZ[0];
    mat[3]=0.0f;
    mat[4]=AxeX[1];
    mat[5]=0.0f;
    mat[6]=AxeZ[1];
    mat[7]=0.0f;
    mat[8]=AxeX[2];
    mat[9]=1.0f;
    mat[10]=AxeZ[2];
    mat[11]=0.0f;
    mat[12]=pos[0];
    mat[13]=pos[1];
    mat[14]=pos[2];
    mat[15]=1.0f;
    */
    // glEnable(GL_BLEND);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glPushMatrix();
    // glTranslatef(( GLfloat) pos[0],( GLfloat) pos[1],( GLfloat) pos[2]);
    glMultMatrixf(mat);

    // glRotatef(90.0f,1.0f, 0.0f, 0.0f );
    // glColor4f(0.8f,0.8f,0.8f,0.4f);

    /*
    http://forum.games-creators.org/viewtopic_3037.htm
    [
    0  1  2  3
    4  5  6  7
    8  9  10 11
    12 13 14 15
    ]
    [AxeX.x][''''''][AxeZ.x]['''''']
    [AxeX.y][''''''][AxeZ.y]['''''']
    [AxeX.z][''''''][AxeZ.z]['''''']
    ['''''']['''''']['''''']['''''']

    [AxeX.x][Axey.x][AxeZ.x]['''''']
    [AxeX.y][Axey.y][AxeZ.y]['''''']
    [AxeX.z][Axey.z][AxeZ.z]['''''']
    ['''''']['''''']['''''']['''''']



    mat[0]=AxeX[0];
    mat[1]=AxeX[1];
    mat[2]=AxeX[2];
    mat[3]=0.0f;
    mat[4]=0.0f;
    mat[5]=0.0f;
    mat[6]=0.0f;
    mat[7]=0.0f;
    mat[8]=AxeZ[0];
    mat[9]=AxeZ[1];
    mat[10]=AxeZ[2];
    mat[11]=0.0f;
    mat[12]=0.0f;
    mat[13]=0.0f;
    mat[14]=1.0f;
    mat[15]=0.0f;
     */
    glTranslatef((GLfloat)0.0f, (GLfloat)-30.0f, (GLfloat)-45.0f);
    glScalef(8.0f, 8.0f, 8.0f);

    // glTranslatef(( GLfloat) pos[0]-20.0f,( GLfloat) pos[1],( GLfloat) pos[2]-25.0f );

    /*float rx=0,ry=0,rz=0;
    //rx=asin(dir[0])*360.0f;
    //rz=asin(dir[0])*180.0f/Q_PI;
    //rz=(dir[1])*180.0f/Q_PI;
    rz=atanf(dir[1]/dir[0])*180.0f/Q_PI; ///////// c la bonne
    //rz=acos(dir[0]/(sqrt(dir[0]*dir[0] + dir[1]*dir[1])))*180.0f/Q_PI;
    float LePi=rz;

    if (dir[0]>0.0f && dir[1]>0.0f)
        LePi=rz+0.0f;

    if (dir[0]<0.0f && dir[1]>0.0f)
        LePi=rz+180.0f;

    if (dir[0]<0.0f && dir[1]<0.0f)
        LePi=fabs(rz)+180.0f;

    if (dir[0]>0.0f && dir[1]<0.0f)
        LePi=rz+360.0f;

    glRotatef(LePi-90.0f,0.0f, 0.0f, 1.0f );*/

    //	glPushMatrix ();
    //
    // glulookAt(pos[0],-pos[1],pos[2],dir[0],dir[1],dir[2],	    	 -up[0],-up[1],-up[2]);
    // gluLookAt(pos[0],pos[1],pos[2],dir[0],dir[1],dir[2],up[0],up[1],up[2]);

    // up is in positivie Y direction

    /*	glRotatef(AngRot[0],1.0f, 0.0f, 0.0f );
        glRotatef(AngRot[1],0.0f, -1.0f, 0.0f );
        glRotatef(AngRot[2],0.0f, 0.0f, -1.0f );
    */
    // vec3_t left=pos. (dir);
    // left.normalize ();
    // awVector(pos,left);
    /*
    drawVector(vec3_t(0.0f,0.0f,0.0f),vec3_t(0.0f,0.0f,1.0f));
    drawVector(vec3_t(0.0f,0.0f,0.0f),vec3_t(0.0f,1.0f,0.0f));
    drawVector(vec3_t(0.0f,0.0f,0.0f),vec3_t(1.0f,0.0f,0.0f) );
    //	glScalef(5.0f,5.0f,5.0f);
    */

    glBindTexture(GL_TEXTURE_2D, m_texid);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < sph01.num_faces; i++)
    // for(int i=sph01.num_faces-1; i>=0; i--)
    {
        glTexCoord2f(sph01.uvs[sph01.v_idx[i]].x, sph01.uvs[sph01.v_idx[i]].y);

        glNormal3f(sph01.normals[sph01.n_idx[i]].x, sph01.normals[sph01.n_idx[i]].y, sph01.normals[sph01.n_idx[i]].z);
        glVertex3f(sph01.vertices[sph01.v_idx[i]].x, sph01.vertices[sph01.v_idx[i]].y,
                   sph01.vertices[sph01.v_idx[i]].z);
    }

    glEnd();

    // glGetFloatv(GL_MODELVIEW_MATRIX, Car_Matrix); //get current matrix

    glPopMatrix();
}

vehicle::~vehicle()
{
    /*if(m_tex) {
            m_tex->destroy();
            delete m_tex;
            m_tex = 0;
        }
    */
}

roue::roue() : m_texid(0)
{
    sph01.vertices = frw_vertex;
    sph01.v_idx = frw_vidx;
    sph01.normals = frw_normal;
    sph01.n_idx = frw_nidx;
    sph01.uvs = frw_uv;
    sph01.num_faces = (sizeof(frw_vidx) / sizeof(long));
    // Image img;
    // img.load("data/env/jeep1.bmp");
    // m_tex = new Texture(img.data(), img.width(), img.height(), GL_RGBA);
    tourne = 0.0f;
}

void roue::Render(vec3_t pos, vec3_t AxeX, vec3_t AxeY, vec3_t AxeZ, float vel, float rot)
{
    float mat[16];

    tourne = tourne + vel;

    mat[0] = AxeX[0];
    mat[1] = AxeX[1];
    mat[2] = AxeX[2];
    mat[3] = 0.0f;
    mat[4] = AxeY[0];
    mat[5] = AxeY[1];
    mat[6] = AxeY[2];
    mat[7] = 0.0f;
    mat[8] = AxeZ[0];
    mat[9] = AxeZ[1];
    mat[10] = AxeZ[2];
    mat[11] = 0.0f;
    mat[12] = pos[0];
    mat[13] = pos[1];
    mat[14] = pos[2];
    mat[15] = 1.0f;

    glPushMatrix();
    glMultMatrixf(mat);
    glTranslatef((GLfloat)0.0f, (GLfloat)-0.0f, (GLfloat)10.0f);
    glScalef(9.0f, 9.0f, 9.0f);
    glRotatef(rot, 0.0f, 0.0f, -1.0f);
    glRotatef(tourne, 1.0f, 0.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, m_texid);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < sph01.num_faces; i++)
    {
        //--- This is how to use the provided texture coordinates
        glTexCoord2f(sph01.uvs[sph01.v_idx[i]].x, sph01.uvs[sph01.v_idx[i]].y);

        glNormal3f(sph01.normals[sph01.n_idx[i]].x, sph01.normals[sph01.n_idx[i]].y, sph01.normals[sph01.n_idx[i]].z);
        glVertex3f(sph01.vertices[sph01.v_idx[i]].x, sph01.vertices[sph01.v_idx[i]].y,
                   sph01.vertices[sph01.v_idx[i]].z);
    }

    glEnd();
    glPopMatrix();
}

roue::~roue()
{
    /*if(m_tex) {
            m_tex->destroy();
            delete m_tex;
            m_tex = 0;
        }
    */
}
