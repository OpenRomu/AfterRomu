///////////////////////////////////////////////////////////////////////////////
//
// PhysEnv.cpp : Physical World implementation file
//
// Purpose:	Implementation of Particle Physics System
//
// Created:
//		JL 12/1/98
// Modified:
//		JL 3/6/99 - FIXED GRAVITY FORCE CALCULATION BUG
//		JL 3/8/99 - ADDED MORE POSSIBLE CONTACTS AS EACH VERTEX CAN CONTACT MORE
//					THEN ONE COLLISION SURFACE (SHOULD IT BE DYNAMICALLY ALLOC'ED?)
//		JL 3/20/99 - ADDED THE MIDPOINT AND RK INTEGRATOR NEEDED TO ALLOC 5 TEMP PARTICLE ARRAYS
//
// Notes:	A bit of this along with the organization comes from Chris Hecker's
//			Physics Articles from last year.  Hopefully this will get everyone
//			back up to speed before we dig deeper into the world of Dynamics.
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998-1999 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>
#include "image.h"
#include <math.h>
#include "particleTorch.h"
#include "particleSmoke.h"
#include "texman.h"
// #include "Squashy.h"
#include "PhysEnv.h"
// #include "SimProps.h"
// #include "VertMass.h"
#include "DSUtil.h"
#include "DXUtil.h"

#pragma warning(disable : 4244) // I NEED TO CONVERT FROM DOUBLE TO FLOAT

#define Q_PI 3.14159265358979323846
/////////////////////////////////////////////////////////////////////////////
// CPhysEnv

// INITIALIZE THE SIMULATION WORLD
CPhysEnv::CPhysEnv()
    : m_CurrentSys(NULL)
    , m_TargetSys(NULL)
{
    frein_a_main = false;

    AxeG = vec3_t(0.0f, 0.0f, 0.0f);
    rot_roue = 0.0f;

    m_Pick[0] = -1;
    m_Pick[1] = -1;
    // m_ParticleSys[0] = NULL;
    // m_ParticleSys[1] = NULL;
    // m_ParticleSys[2] = NULL;	// RESET BUFFER

    // THESE TEMP PARTICLE BUFFERS ARE NEEDED FOR THE MIDPOINT AND RK4 INTEGRATOR
    for (int i = 0; i < 5; i++)
        m_TempSys[i] = NULL;
    m_ParticleCnt = 0;
    m_Contact = NULL;
    m_Spring = NULL;
    m_SpringCnt = 0;
    pulse = 0.0f;
    //	pulse_roue=vec3_t(0.0f,0.0f,0.0f);

    m_UseGravity = false;
    m_DrawSprings = TRUE;
    m_DrawVertices = TRUE;
    m_MouseForceActive = FALSE;

    m_Gravity = vec3_t(0.0f, 0.0f, -1.0f); //-20

    m_UserForceMag = 100.0;
    m_UserForceActive = FALSE;
    m_Kd = 0.01f; // DAMPING FACTOR  fortement ac le sol 0.2 boue , 0.99 galce
    m_Kr = 0.1f;  // 1.0 = SUPERBALL BOUNCE 0.0 = DEAD WEIGHT
                  //	m_Ksh	= 0.06f;		// HOOK'S s CONSTANT
                  //	m_Ksd	= 0.05f;		// SPRING DAMPING CONSTANT

    m_MouseForceKs = 0.08f; // MOUSE SPRING CONSTANT

    // CREATE THE SIZE FOR THE SIMULATION WORLD
    m_WorldSizeX = 150.0f;
    m_WorldSizeY = 150.0f;
    m_WorldSizeZ = 150.0f;

    m_IntegratorType = EULER_INTEGRATOR;

    //	m_CollisionPlane = (tCollisionPlane	*)malloc(sizeof(tCollisionPlane) * 1);
    //	m_CollisionPlaneCnt = 1;

    // MAKE THE BOTTOM PLANE (FLOOR)
    //	m_CollisionPlane[0].normal=vec3_t(0.0f, 0.0f, 1.0f);

    // MAKE THE LEFT PLANE

    //	m_CollisionRootFinding = FALSE;		// ONLY SET WHEN FINDING A COLLISION

    AxeDevant = vec3_t(0.0f, 1.0f, 0.0f);
    AxeHaut = vec3_t(0.0f, 0.0f, 1.0f);

    AxeG = vec3_t(0, 0, 0);
    for (int i = 0; i < 5; i++) {
        m_TempSys[i] = (tParticle*)malloc(sizeof(tParticle) * 8);
    }
}

CPhysEnv::~CPhysEnv()
{
    /*if (m_ParticleSys[0])
            free(m_ParticleSys[0]);
    if (m_ParticleSys[1])
            free(m_ParticleSys[1]);
    if (m_ParticleSys[2])
            free(m_ParticleSys[2]);*/
    if (m_CurrentSys)
        free(m_CurrentSys);
    if (m_TargetSys)
        free(m_TargetSys);

    for (int i = 0; i < 5; i++) {
        if (m_TempSys[i])
            free(m_TempSys[i]);
    }
    if (m_Contact)
        free(m_Contact);
    if (m_Spring)
        free(m_Spring);
    //	free(m_CollisionPlane);
    //  if(m_tex)
    // delete(m_tex);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	RenderWorld
// Purpose:		Draw the current system (particles, springs, userforces)
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::RenderFake(vec3_t pos, vec3_t AxeX, vec3_t AxeY)
{

    AxeX.normalize();
    AxeY.normalize();
    vec3_t AxeZ = LaNormalA(AxeY, AxeX);
    // glEnable(GL_TEXTURE_2D);

    my_car.Render(pos, AxeZ, AxeX, AxeY);
    my_roueavd.Render(pos - AxeY * 30.0f + AxeZ * 30.0f + AxeX * 40.0f, AxeZ, AxeX, AxeY, 1.0f, 0.01f);
    my_roueavg.Render(pos - AxeY * 30.0f + AxeZ * 30.0f + AxeX * -40.0f, AxeZ, AxeX, AxeY, 1.0f, 0.01f);
    my_roueard.Render(pos - AxeY * 30.0f - AxeZ * 30.0f + AxeX * 40.0f, AxeZ, AxeX, AxeY, 1.0f, 0.01f);
    my_rouearg.Render(pos - AxeY * 30.0f - AxeZ * 30.0f + AxeX * -40.0f, AxeZ, AxeX, AxeY, 1.0f, 0.01f);
    // glDisable(GL_TEXTURE_2D);
}

void CPhysEnv::RenderWorld()
{

    glEnable(GL_CULL_FACE);
    // glDisable(GL_TEXTURE_2D);
    my_world->drawBoxEx(m_CurrentSys[0].pos);

    my_world->drawBoxEx(m_CurrentSys[1].pos);
    my_world->drawBoxEx(m_CurrentSys[2].pos);
    my_world->drawBoxEx(m_CurrentSys[3].pos);
    my_world->drawBoxEx(m_CurrentSys[4].pos);
    my_world->drawBoxEx(m_CurrentSys[5].pos);
    my_world->drawBoxEx(m_CurrentSys[6].pos);
    my_world->drawBoxEx(m_CurrentSys[7].pos);

    vec3_t AxeX
        = ((m_CurrentSys[4].pos + m_CurrentSys[7].pos) * 0.5f) - ((m_CurrentSys[5].pos + m_CurrentSys[6].pos) * 0.5f);
    vec3_t AxeY
        = ((m_CurrentSys[5].pos + m_CurrentSys[4].pos) * 0.5f) - ((m_CurrentSys[6].pos + m_CurrentSys[7].pos) * 0.5f);
    vec3_t AxeZ
        = ((m_CurrentSys[5].pos + m_CurrentSys[4].pos) * 0.5f) - ((m_CurrentSys[0].pos + m_CurrentSys[1].pos) * 0.5f);
    vec3_t old_axe = AxeG;
    AxeG = (m_CurrentSys[6].pos + m_CurrentSys[7].pos + m_CurrentSys[4].pos + m_CurrentSys[5].pos) * 0.25f;
    AxeDevant
        = ((m_CurrentSys[5].pos + m_CurrentSys[4].pos) * 0.5f) - ((m_CurrentSys[6].pos + m_CurrentSys[7].pos) * 0.5f);

    old_axe = AxeG - old_axe;

    GlobalVelo = old_axe.len();
    AxeHaut
        = ((m_CurrentSys[5].pos + m_CurrentSys[4].pos) * 0.5f) - ((m_CurrentSys[0].pos + m_CurrentSys[1].pos) * 0.5f);

    AxeZ = LaNormalA(AxeY, AxeX);

    AxeX.normalize();
    AxeZ.normalize();
    AxeY.normalize();
    // roue 0
    vec3_t AxeX0 = (m_CurrentSys[0].pos - m_CurrentSys[1].pos);
    vec3_t AxeY0 = (m_CurrentSys[0].pos - m_CurrentSys[3].pos);
    vec3_t AxeZ0 = (m_CurrentSys[0].pos - m_CurrentSys[4].pos);
    AxeZ0 = LaNormalA(AxeY0, AxeX0);

    AxeX0.normalize();
    AxeZ0.normalize();
    AxeY0.normalize();
    // roue 1
    vec3_t AxeX1 = (m_CurrentSys[0].pos - m_CurrentSys[1].pos);
    vec3_t AxeY1 = (m_CurrentSys[1].pos - m_CurrentSys[2].pos);
    vec3_t AxeZ1 = (m_CurrentSys[1].pos - m_CurrentSys[5].pos);
    AxeZ1 = LaNormalA(AxeY1, AxeX1);
    AxeX1.normalize();
    AxeZ1.normalize();
    AxeY1.normalize();

    AxeRoueAv = (AxeX0 + AxeX1) * 0.5f;
    AxeRoueAv.normalize();
    AxeRoueAvY1 = (AxeY0 + AxeY1) * 0.5f;
    AxeRoueAvY1.normalize();

    // roue 0
    vec3_t AxeX2 = (m_CurrentSys[2].pos - m_CurrentSys[3].pos);
    vec3_t AxeY2 = (m_CurrentSys[2].pos - m_CurrentSys[1].pos);
    vec3_t AxeZ2 = (m_CurrentSys[2].pos - m_CurrentSys[6].pos);
    AxeZ2 = LaNormalA(AxeY2, AxeX2);
    AxeX2.normalize();
    AxeZ2.normalize();
    AxeY2.normalize();
    // roue 0
    vec3_t AxeX3 = (m_CurrentSys[2].pos - m_CurrentSys[3].pos);
    vec3_t AxeY3 = (m_CurrentSys[3].pos - m_CurrentSys[0].pos);
    vec3_t AxeZ3 = (m_CurrentSys[3].pos - m_CurrentSys[7].pos);
    AxeZ3 = LaNormalA(AxeY3, AxeX3);
    AxeX3.normalize();
    AxeZ3.normalize();
    AxeY3.normalize();

    AxeRoueAr = (AxeX2 + AxeX3) * -0.5f;
    AxeRoueAr.normalize();
    glEnable(GL_TEXTURE_2D);

    my_car.Render(AxeG, AxeX, AxeY, AxeZ);

    float roue_tourne = m_CurrentSys[0].v.dot(AxeY) / 2.0f;

    my_roueavd.Render(m_CurrentSys[0].pos, AxeX0, AxeY0, AxeZ0, roue_tourne * -1.0f, rot_roue);
    my_roueavg.Render(m_CurrentSys[1].pos, AxeX1, AxeY1, AxeZ1, roue_tourne * -1.0f, rot_roue);
    my_roueard.Render(m_CurrentSys[3].pos, AxeX3, AxeY3, AxeZ3, roue_tourne, 0.0f);
    my_rouearg.Render(m_CurrentSys[2].pos, AxeX2, AxeY2, AxeZ2, roue_tourne, 0.0f);
    //	glDisable(GL_TEXTURE_2D);
}

/*void CPhysEnv::RenderWorldOld()
{
        tParticle	*tempParticle;
        tSpring		*tempSpring;

//	vec3_t direc=((m_CurrentSys[4].pos+m_CurrentSys[5].pos)*0.5f)-((m_CurrentSys[7].pos+m_CurrentSys[6].pos)*0.5f);
        //vec3_t direc=((m_CurrentSys[3].pos-m_CurrentSys[0].pos));
        vec3_t direc1=((m_CurrentSys[0].pos+m_CurrentSys[1].pos))*0.5f;
        //vec3_t direc=((m_CurrentSys[3].pos-m_CurrentSys[0].pos));
        vec3_t direc=((m_CurrentSys[0].pos+m_CurrentSys[1].pos)*0.5f)-((m_CurrentSys[2].pos+m_CurrentSys[3].pos)*0.5f);

        vec3_t normal=((m_CurrentSys[4].pos-m_CurrentSys[0].pos));
        vec3_t left=((m_CurrentSys[1].pos+m_CurrentSys[2].pos)*0.5f)-((m_CurrentSys[0].pos+m_CurrentSys[3].pos)*0.5f);
        //vec3_t
normal=((m_CurrentSys[0].pos+m_CurrentSys[1].pos)*0.5f)-((m_CurrentSys[5].pos+m_CurrentSys[4].pos)*0.5f);
        // vec3_t normal=vec3_t(0.0f,0.0f,1.0f);
//	vec3_t direc=vec3_t(1.0f,0.0f,0.0f);
vec3_t le_min;
vec3_t le_max;
le_min=m_CurrentSys[0].pos;
le_max=m_CurrentSys[0].pos;

for (int ii = 0; ii < 4; ii++)
{
        if (le_min[0]>m_CurrentSys[ii].pos[0])
                le_min[0]=m_CurrentSys[ii].pos[0];

        if (le_max[0]<m_CurrentSys[ii].pos[0])
                le_max[0]=m_CurrentSys[ii].pos[0];

        if (le_min[1]>m_CurrentSys[ii].pos[1])
                le_min[1]=m_CurrentSys[ii].pos[1];

        if (le_max[1]<m_CurrentSys[ii].pos[1])
                le_max[1]=m_CurrentSys[ii].pos[1];

        if (le_min[2]>m_CurrentSys[ii].pos[2])
                le_min[2]=m_CurrentSys[ii].pos[2];

        if (le_max[2]<m_CurrentSys[ii].pos[2])
                le_max[2]=m_CurrentSys[ii].pos[2];

}
vec3_t le_res=(le_max+le_min)*0.5f; // calcule du centre de gravite
//le_res[2]=m_CurrentSys[4].pos[2];
//direc=le_res-direc1;
direc.normalize();
normal.normalize();
left.normalize();
traceur=direc;

    glEnable(GL_CULL_FACE);

        //my_car.Render (m_CurrentSys[4].pos,direc,normal);
        my_car.Render( ((m_CurrentSys[2].pos+m_CurrentSys[3].pos))*0.5f,direc,normal);
//	my_car.Render (m_CurrentSys[0].pos,m_CurrentSys[3].pos,m_CurrentSys[4].pos);
        my_roueavd.Render (m_CurrentSys[0].pos);
        my_roueavg.Render (m_CurrentSys[1].pos);
        my_roueard.Render (m_CurrentSys[3].pos);
        my_rouearg.Render (m_CurrentSys[2].pos);
vec3_t le_dv=(m_CurrentSys[0].pos+direc);

my_world->drawBoxEx  (le_res);
 my_world->drawBoxEx  (direc1);

        if (m_CurrentSys)
        {
                if (m_Spring && m_DrawSprings)
                {
                        glBegin(GL_LINES);
                        glColor3f(0.0f,0.8f,0.8f);
                        tempSpring = m_Spring;
                        for (int loop = 0; loop < m_SpringCnt; loop++)
                        {
                                glVertex3fv((float *)&m_CurrentSys[tempSpring->p1].pos);
                                glVertex3fv((float *)&m_CurrentSys[tempSpring->p2].pos);
                                tempSpring++;
                        }
                        if (m_MouseForceActive)	// DRAW MOUSESPRING FORCE
                        {
                                if (m_Pick[0] > -1)
                                {
                                        glColor3f(0.8f,0.0f,0.8f);
                                        glVertex3fv((float *)&m_CurrentSys[m_Pick[0]].pos);
                                        glVertex3fv((float *)&m_MouseDragPos[0]);
                                }
                                if (m_Pick[1] > -1)
                                {
                                        glColor3f(0.8f,0.0f,0.8f);
                                        glVertex3fv((float *)&m_CurrentSys[m_Pick[1]].pos);
                                        glVertex3fv((float *)&m_MouseDragPos[1]);
                                }
                        }
                        glEnd();
                }
                if (m_DrawVertices)
                {
                        glBegin(GL_POINTS);
                        tempParticle = m_CurrentSys;
                        for (int loop = 0; loop < m_ParticleCnt; loop++)
                        {
                                if (loop == m_Pick[0])
                                        glColor3f(0.0f,0.8f,0.0f);
                                else if (loop == m_Pick[1])
                                        glColor3f(0.8f,0.0f,0.0f);
                                else
                                        glColor3f(0.8f,0.8f,0.0f);

                                glVertex3fv((float *)&tempParticle->pos);
                                tempParticle++;
                        }
                        glEnd();
                }
        }

}
*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	GetNearestPoint
// Purpose:		Use OpenGL Feedback to find the closest point to a mouseclick
// Arguments:	Screen coordinates of the hit
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::GetNearestPoint(int x, int y)
{
    /// Local Variables ///////////////////////////////////////////////////////////
    float* feedBuffer;
    int hitCount;
    tParticle* tempParticle;
    int loop;
    ///////////////////////////////////////////////////////////////////////////////
    // INITIALIZE A PLACE TO PUT ALL THE FEEDBACK INFO (3 DATA, 1 TAG, 2 TOKENS)
    feedBuffer = (float*)malloc(sizeof(GLfloat) * m_ParticleCnt * 6);
    // TELL OPENGL ABOUT THE BUFFER
    glFeedbackBuffer(m_ParticleCnt * 6, GL_3D, feedBuffer);
    (void)glRenderMode(GL_FEEDBACK); // SET IT IN FEEDBACK MODE

    tempParticle = m_CurrentSys;
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        // PASS THROUGH A MARKET LETTING ME KNOW WHAT VERTEX IT WAS
        glPassThrough((float)loop);
        // SEND THE VERTEX
        glBegin(GL_POINTS);
        glVertex3fv((float*)&tempParticle->pos);
        glEnd();
        tempParticle++;
    }
    hitCount = glRenderMode(GL_RENDER);                      // HOW MANY HITS DID I GET
    CompareBuffer(hitCount, feedBuffer, (float)x, (float)y); // CHECK THE HIT
    free(feedBuffer);                                        // GET RID OF THE MEMORY
}
////// GetNearestPoint ////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	CompareBuffer
// Purpose:		Check the feedback buffer to see if anything is hit
// Arguments:	Number of hits, pointer to buffer, point to test
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::CompareBuffer(int size, float* buffer, float x, float y)
{
    /// Local Variables ///////////////////////////////////////////////////////////
    GLint count;
    GLfloat token, point[3];
    int loop, currentVertex, result = -1;
    long nearest = -1, dist;
    ///////////////////////////////////////////////////////////////////////////////
    count = size;
    while (count) {
        token = buffer[size - count]; // CHECK THE TOKEN
        count--;
        if (token == GL_PASS_THROUGH_TOKEN) // VERTEX MARKER
        {
            currentVertex = (int)buffer[size - count]; // WHAT VERTEX
            count--;
        } else if (token == GL_POINT_TOKEN) {
            // THERE ARE THREE ELEMENTS TO A POINT TOKEN
            for (loop = 0; loop < 3; loop++) {
                point[loop] = buffer[size - count];
                count--;
            }
            dist = ((x - point[0]) * (x - point[0])) + ((y - point[1]) * (y - point[1]));
            if (result == -1 || dist < nearest) {
                nearest = dist;
                result = currentVertex;
            }
        }
    }

    if (nearest < 50.0f) {
        if (m_Pick[0] == -1)
            m_Pick[0] = result;
        else if (m_Pick[1] == -1)
            m_Pick[1] = result;
        else {
            m_Pick[0] = result;
            m_Pick[1] = -1;
        }
    }
}
////// CompareBuffer //////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	SetWorldParticles
// Purpose:		Inform the System of the particles under control
// Arguments:	List of vertices and count
///////////////////////////////////////////////////////////////////////////////
/*
void CPhysEnv::SetWorldParticles(vec3_t *coords,int particleCnt)
{
        tParticle *tempParticle;

        if (m_ParticleSys[0])
                free(m_ParticleSys[0]);
        if (m_ParticleSys[1])
                free(m_ParticleSys[1]);
        if (m_ParticleSys[2])
                free(m_ParticleSys[2]);
        for (int i = 0; i < 5; i++)
        {
                if (m_TempSys[i])
                        free(m_TempSys[i]);
        }
        if (m_Contact)
                free(m_Contact);
        // THE SYSTEM IS DOUBLE BUFFERED TO MAKE THINGS EASIER
        m_CurrentSys = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
        m_TargetSys = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
        m_ParticleSys[2] = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
        for (i = 0; i < 5; i++)
        {
                m_TempSys[i] = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
        }
        m_ParticleCnt = particleCnt;

        // MULTIPLIED PARTICLE COUNT * 2 SINCE THEY CAN COLLIDE WITH MULTIPLE WALLS
        m_Contact = (tContact *)malloc(sizeof(tContact) * particleCnt * 2);
        m_ContactCnt = 0;

        tempParticle = m_CurrentSys;
        for (int loop = 0; loop < particleCnt; loop++)
        {
                memcpy(tempParticle->pos,&coords,sizeof(vec3_t));//warning bat modif
                tempParticle->v=vec3_t( 0.0f, 0.0f, 0.0f);
                tempParticle->f=vec3_t(0.0f, 0.0f, 0.0f);
                tempParticle->oneOverM = 1.0f;							// MASS OF 1
                tempParticle++;
                coords++;
        }

        // COPY THE SYSTEM TO THE SECOND ONE ALSO
        memcpy(m_TargetSys,m_CurrentSys,sizeof(tParticle) * particleCnt);
        // COPY THE SYSTEM TO THE RESET BUFFER ALSO
        memcpy(m_ParticleSys[2],m_CurrentSys,sizeof(tParticle) * particleCnt);

        m_ParticleSys[0] = m_CurrentSys;
        m_ParticleSys[1] = m_TargetSys;
}*/
////// SetWorldParticles //////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	FreeSystem
// Purpose:		Remove all particles and clear it out
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::FreeSystem()
{
    m_Pick[0] = -1;
    m_Pick[1] = -1;
    /*if (m_ParticleSys[0])
    {
            m_ParticleSys[0] = NULL;
            free(m_ParticleSys[0]);
    }
    if (m_ParticleSys[1])
    {
            free(m_ParticleSys[1]);
            m_ParticleSys[1] = NULL;
    }
    if (m_ParticleSys[2])
    {
            free(m_ParticleSys[2]);
            m_ParticleSys[2] = NULL;	// RESET BUFFER
    }
    */
    for (int i = 0; i < 5; i++) {
        if (m_TempSys[i]) {
            free(m_TempSys[i]);
            m_TempSys[i] = NULL; // RESET BUFFER
        }
    }
    if (m_Contact) {
        free(m_Contact);
        m_Contact = NULL;
    }
    if (m_Spring) {
        free(m_Spring);
        m_Spring = NULL;
    }
    m_SpringCnt = 0;
    m_ParticleCnt = 0;
}
////// FreeSystem //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	LoadData
// Purpose:		Load a simulation system
// Arguments:	File pointer
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::LoadData(vec3_t offset)
{
    m_UseGravity = true;
    m_UseDamping = true;
    m_UserForceActive = true;
    m_UserForce = vec3_t(0.0f, 0.0f, 0.0f);
    m_UserForceMag = 1.0f;
    m_ParticleCnt = 0;
    m_SpringCnt = 0;

    int tot_particle = 8;
    int tot_spring = 0;

    //	m_ParticleSys[2] = (tParticle *)malloc(sizeof(tParticle) * tot_particle);
    //	m_ParticleSys[0] = m_CurrentSys;
    //	m_ParticleSys[1] = m_TargetSys;
    //	m_Spring = (tSpring *)malloc(sizeof(tSpring) * (tot_spring));
    m_Contact = (tContact*)malloc(sizeof(tContact) * tot_particle * 2);
    m_CurrentSys = (tParticle*)malloc(sizeof(tParticle) * tot_particle);
    m_TargetSys = (tParticle*)malloc(sizeof(tParticle) * tot_particle);

    /*	fread(&m_UseGravity,sizeof(BOOL),1,fp);
            fread(&m_UseDamping,sizeof(BOOL),1,fp);
            fread(&m_UserForceActive,sizeof(BOOL),1,fp);
            fread(&m_Gravity,sizeof(vec3_t),1,fp);
            fread(&m_UserForce,sizeof(vec3_t),1,fp);
            fread(&m_UserForceMag,sizeof(float),1,fp);
            fread(&m_Kd,sizeof(float),1,fp);
            fread(&m_Kr,sizeof(float),1,fp);
            fread(&m_Ksh,sizeof(float),1,fp);
            fread(&m_Ksd,sizeof(float),1,fp);
            fread(&m_ParticleCnt,sizeof(int),1,fp);
    */
    //	m_CurrentSys
    /*
    m_CurrentSys = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
    for (int i = 0; i < 5; i++)
    {
            m_TempSys[i] = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
    }
    fread(m_ParticleSys[0],sizeof(tParticle),m_ParticleCnt,fp);
    fread(m_ParticleSys[1],sizeof(tParticle),m_ParticleCnt,fp);
    fread(m_ParticleSys[2],sizeof(tParticle),m_ParticleCnt,fp);
    fread(&m_SpringCnt,sizeof(int),1,fp);
    fread(m_Spring,sizeof(tSpring),m_SpringCnt,fp);
    fread(m_Pick,sizeof(int),2,fp);
    */

    AddVertex(0, vec3_t(60.0f, 0.0f, 0.0f) + offset);
    AddVertex(1, vec3_t(0.0f, 0.0f, 0.0f) + offset);
    AddVertex(0, vec3_t(0.0f, 60.0f, 0.0f) + offset);
    AddVertex(1, vec3_t(60.0f, 60.0f, 0.0f) + offset);

    AddVertex(0, vec3_t(60.0f, 0.0f, 50.0f) + offset);
    AddVertex(1, vec3_t(0.0f, 0.0f, 50.0f) + offset);
    AddVertex(0, vec3_t(0.0f, 60.0f, 50.0f) + offset);
    AddVertex(1, vec3_t(60.0f, 60.0f, 50.0f) + offset);

    float suspension = 3.1f;
    float chassis = 3.0f;

    AddSpring(chassis, 0, 1);
    AddSpring(chassis, 1, 2);
    AddSpring(chassis, 2, 3);
    AddSpring(chassis, 3, 0);

    AddSpring(chassis, 4, 5);
    AddSpring(chassis, 5, 6);
    AddSpring(chassis, 6, 7);
    AddSpring(chassis, 7, 4);

    AddSpring(suspension, 5, 1);
    AddSpring(suspension, 6, 2);
    AddSpring(suspension, 7, 3);
    AddSpring(suspension, 4, 0);

    AddSpring(chassis, 0, 2);
    AddSpring(chassis, 1, 3);

    AddSpring(suspension, 1, 6);
    AddSpring(suspension, 5, 2);

    AddSpring(chassis, 5, 7);
    AddSpring(chassis, 4, 6);

    AddSpring(suspension, 4, 3);
    AddSpring(suspension, 0, 7);

    AddSpring(suspension, 4, 1);
    AddSpring(suspension, 5, 0);

    AddSpring(suspension, 3, 6);
    AddSpring(suspension, 7, 2);

    //	AddSpring(suspension,0, 6);
    //	AddSpring(suspension,2, 4);

    memcpy(m_TargetSys, m_CurrentSys, sizeof(tParticle) * tot_particle);
}
vec3_t CPhysEnv::Get_Global_velocity()
{
    vec3_t moy;

    for (int i = 0; i < 7; i++) {
        moy = (moy + m_CurrentSys[i].v) / 2.0f;
    }
    return moy;
}

void CPhysEnv::SetColision(vec3_t velo)
{

    for (int u = 0; u < 7; u++) {
        m_CurrentSys[u].v += velo;
    }
}

void CPhysEnv::SetShoot(vec3_t velo)
{
    vec3_t a = velo + vec3_t(0.0f, 0.0f, -30.0f);
    for (int u = 0; u < 7; u++) {
        vec3_t patate = a - m_CurrentSys[u].pos;
        float pousse = 50.0f / patate.len();
        patate.normalize();
        m_CurrentSys[u].v -= patate * pousse * 100.0f + vec3_t(0.0f, 0.0f, -10.0f);
    }
}

void CPhysEnv::SetPos(vec3_t offset, vec3_t velo)
{
    vec3_t offsett = offset;

    vec3_t a = AxeDevant;
    a.normalize();
    a = a * 30.0f;
    vec3_t h = AxeHaut;
    a.normalize();
    h.normalize();
    vec3_t z = LaNormalA(AxeHaut, AxeDevant);
    h = h * 20.0f;

    a = a * 45.0f;
    z = z * 60.0f;

    m_CurrentSys[0].pos = a + z / 2.0f + offsett - h;
    m_CurrentSys[1].pos = a - z / 2.0f + offsett - h;
    m_CurrentSys[2].pos = -a - z / 2.0f + offsett - h;
    m_CurrentSys[3].pos = -a + z / 2.0f + offsett - h;

    m_CurrentSys[4].pos = a + z / 2.0f + offsett + h;
    m_CurrentSys[5].pos = a - z / 2.0f + offsett + h;
    m_CurrentSys[6].pos = -a - z / 2.0f + offsett + h;
    m_CurrentSys[7].pos = -a + z / 2.0f + offsett + h;

    m_CurrentSys[0].v = velo;
    m_CurrentSys[1].v = velo;
    m_CurrentSys[2].v = velo;
    m_CurrentSys[3].v = velo;

    m_CurrentSys[4].v = velo;
    m_CurrentSys[5].v = velo;
    m_CurrentSys[6].v = velo;
    m_CurrentSys[7].v = velo;

    reinitsprings();
    AxeG = (m_CurrentSys[6].pos + m_CurrentSys[7].pos + m_CurrentSys[4].pos + m_CurrentSys[5].pos) * 0.25f;

    memcpy(m_TargetSys, m_CurrentSys, sizeof(tParticle) * m_ParticleCnt);
}

/*
void CPhysEnv::SetPos(vec3_t offset)
{
        vec3_t offsett=offset;

        m_CurrentSys[0].pos =vec3_t(30.0f,-40.0f,00.0f)+offsett;
        m_CurrentSys[1].pos =vec3_t(-30.0f,-40.0f,00.0f)+offsett;
        m_CurrentSys[2].pos =vec3_t(-30.0f,40.0f,00.0f)+offsett;
        m_CurrentSys[3].pos =vec3_t(30.0f,40.0f,00.0f)+offsett;

        m_CurrentSys[4].pos =vec3_t(30.0f,-40.0f,40.0f)+offsett;
        m_CurrentSys[5].pos =vec3_t(-30.0f,-40.0f,40.0f)+offsett;
        m_CurrentSys[6].pos =vec3_t(-30.0f,40.0f,40.0f)+offsett;
        m_CurrentSys[7].pos =vec3_t(30.0f,40.0f,40.0f)+offsett;

        m_CurrentSys[0].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[1].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[2].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[3].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[4].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[5].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[6].v =vec3_t(0.0f,0.0f,0.0f);
        m_CurrentSys[7].v =vec3_t(0.0f,0.0f,0.0f);




        reinitsprings();
AxeG=(m_CurrentSys[6].pos+m_CurrentSys[7].pos+m_CurrentSys[4].pos+m_CurrentSys[5].pos)*0.25f;

        memcpy(m_TargetSys,m_CurrentSys,sizeof(tParticle) * m_ParticleCnt );

}*/
////// LoadData //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	SaveData
// Purpose:		Save a simulation system
// Arguments:	File pointer
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::SaveData(FILE* fp)
{
    fwrite(&m_UseGravity, sizeof(BOOL), 1, fp);
    fwrite(&m_UseDamping, sizeof(BOOL), 1, fp);
    fwrite(&m_UserForceActive, sizeof(BOOL), 1, fp);
    fwrite(&m_Gravity, sizeof(vec3_t), 1, fp);
    fwrite(&m_UserForce, sizeof(vec3_t), 1, fp);
    fwrite(&m_UserForceMag, sizeof(float), 1, fp);
    fwrite(&m_Kd, sizeof(float), 1, fp);
    fwrite(&m_Kr, sizeof(float), 1, fp);
    fwrite(&m_Ksh, sizeof(float), 1, fp);
    fwrite(&m_Ksd, sizeof(float), 1, fp);
    fwrite(&m_ParticleCnt, sizeof(int), 1, fp);
    // fwrite(m_ParticleSys[0],sizeof(tParticle),m_ParticleCnt,fp);
    // fwrite(m_ParticleSys[1],sizeof(tParticle),m_ParticleCnt,fp);
    //	fwrite(m_ParticleSys[2],sizeof(tParticle),m_ParticleCnt,fp);
    fwrite(&m_SpringCnt, sizeof(int), 1, fp);
    fwrite(m_Spring, sizeof(tSpring), m_SpringCnt, fp);
    fwrite(m_Pick, sizeof(int), 2, fp);
}
////// SaveData //////////////////////////////////////////////////////////////

// RESET THE SIM TO INITIAL VALUES
void CPhysEnv::ResetWorld()
{
    //	memcpy(m_CurrentSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);
    //	memcpy(m_TargetSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);
}

vec3_t CPhysEnv::LaNormalA(vec3_t P1, vec3_t P2)

{
    vec3_t Normal;

    Normal[0] = P1[1] * P2[2] - P1[2] * P2[1];
    Normal[1] = P1[2] * P2[0] - P1[0] * P2[2];
    Normal[2] = P1[0] * P2[1] - P1[1] * P2[0];
    Normal.normalize();
    return Normal;
}

void CPhysEnv::SetWorldProperties()
{
    /*	CSimProps	dialog;
            dialog.m_CoefRest = m_Kr;
            dialog.m_Damping = m_Kd;
            dialog.m_GravX = m_Gravity.x;
            dialog.m_GravY = m_Gravity.y;
            dialog.m_GravZ = m_Gravity.z;
            dialog.m_SpringConst = m_Ksh;
            dialog.m_SpringDamp = m_Ksd;
            dialog.m_UserForceMag = m_UserForceMag;
            dialog.m_MouseSpring = m_MouseForceKs;
            if (dialog.DoModal() == IDOK)
            {
                    m_Kr = dialog.m_CoefRest;
                    m_Kd = dialog.m_Damping;
                    m_Gravity.x = dialog.m_GravX;
                    m_Gravity.y = dialog.m_GravY;
                    m_Gravity.z = dialog.m_GravZ;
                    m_UserForceMag = dialog.m_UserForceMag;
                    m_Ksh = dialog.m_SpringConst;
                    m_Ksd = dialog.m_SpringDamp;
                    for (int loop = 0; loop < m_SpringCnt; loop++)
                    {
                            m_Spring[loop].Ks = m_Ksh;
                            m_Spring[loop].Kd = m_Ksd;
                    }

                    // GET THE NEW MOUSESPRING FORCE
                    m_MouseForceKs = dialog.m_MouseSpring;
            }*/
}

void CPhysEnv::SetVertexMass()
{
    /*	CVertMass	dialog;
            dialog.m_VertexMass = m_CurrentSys[m_Pick[0]].oneOverM;
            if (dialog.DoModal() == IDOK)
            {
                    m_ParticleSys[0][m_Pick[0]].oneOverM = dialog.m_VertexMass;
                    m_ParticleSys[0][m_Pick[1]].oneOverM = dialog.m_VertexMass;
                    m_ParticleSys[1][m_Pick[0]].oneOverM = dialog.m_VertexMass;
                    m_ParticleSys[1][m_Pick[1]].oneOverM = dialog.m_VertexMass;
                    m_ParticleSys[2][m_Pick[0]].oneOverM = dialog.m_VertexMass;
                    m_ParticleSys[2][m_Pick[1]].oneOverM = dialog.m_VertexMass;
            }
    */
}

void CPhysEnv::ApplyUserForce(vec3_t* force)
{
    //	ScaleVector(force,  m_UserForceMag, &);
    /*	m_UserForce=m_UserForceMag*force;
     */
    m_UserForceActive = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SetMouseForce
// Purpose:		Allows the user to interact with selected points by dragging
// Arguments:	Delta distance from clicked point, local x and y axes
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::SetMouseForce(int deltaX, int deltaY, vec3_t* localX, vec3_t* localY)
{
    /*
/// Local Variables ///////////////////////////////////////////////////////////
    vec3_t tempX,tempY;
///////////////////////////////////////////////////////////////////////////////
    tempX=localX*  (float)deltaX * 0.03f;
    tempY=localY*  -(float)deltaY * 0.03f;
    if (m_Pick[0] > -1)
    {
            VectorSum(&m_CurrentSys[m_Pick[0]].pos,&tempX,&m_MouseDragPos[0]);
            VectorSum(&m_MouseDragPos[0],&tempY,&m_MouseDragPos[0]);
    }
    if (m_Pick[1] > -1)
    {
            VectorSum(&m_CurrentSys[m_Pick[1]].pos,&tempX,&m_MouseDragPos[1]);
            VectorSum(&m_MouseDragPos[1],&tempY,&m_MouseDragPos[1]);
    }
    */
}
/// SetMouseForce /////////////////////////////////////////////////////////////

void CPhysEnv::AddSpring(float a_Ksd, int a, int b)
{
    tSpring* spring;
    tSpring* t;

    spring = (tSpring*)malloc(sizeof(tSpring) * (m_SpringCnt + 1));
    if (m_Spring != NULL)
        memcpy(spring, m_Spring, sizeof(tSpring) * m_SpringCnt);
    t = m_Spring;
    m_Spring = spring;
    free(t);

    spring = &m_Spring[m_SpringCnt++];
    spring->Ks = 0.002f;
    spring->Kd = 0.5f; // a_Ksd;
    spring->p1 = a;
    spring->p2 = b;
    vec3_t tmp;
    tmp = m_CurrentSys[a].pos - m_CurrentSys[b].pos;
    spring->restLen = tmp.len();
}

void CPhysEnv::AddVertex(int id, vec3_t pos)
{
    tParticle* particle;
    tParticle* t;
    // MAKE SURE TWO PARTICLES ARE PICKED

    particle = (tParticle*)malloc(sizeof(tParticle) * (m_ParticleCnt + 1));
    if (m_CurrentSys != NULL)
        memcpy(particle, m_CurrentSys, sizeof(tParticle) * m_ParticleCnt);
    t = m_CurrentSys;
    m_CurrentSys = particle;
    free(t);
    particle = &m_CurrentSys[m_ParticleCnt++];
    particle->f = vec3_t(0.0f, 0.0f, 0.0f);
    if (id < 4)
        particle->oneOverM = 1.0f;
    else
        particle->oneOverM = 1.0f;

    particle->v = vec3_t(0.0f, 0.0f, 0.0f);
    particle->pos = pos;
    particle->oldpos = pos;
    particle->contacting = FALSE;
}
/*
void CPhysEnv::AddVertex( int id,vec3_t pos)
{
        tParticle *particle;
        particle = &m_CurrentSys[m_ParticleCnt++];
        particle->f =vec3_t(0.0f,0.0f,0.0f);
        if(id<4)
        particle->oneOverM =40.0f;
        else
        particle->oneOverM =40.0f;


        particle->v =vec3_t(0.0f,0.0f,0.0f);
        particle->pos  =pos;
        particle->oldpos  =pos;
        particle->contacting = FALSE;
}
*/
double VectorSquaredLength(vec3_t v) { return ((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2])); }

#define STATIC_THRESHOLD 0.3f // default bat 0.03f
#define m_Csf 0.9f            // Default Static Friction
#define m_Ckf 1.8f            // Default Kinetic Friction
#define m_UseFriction true

vec3_t CPhysEnv::projeter(vec3_t normal, vec3_t v)
{
    vec3_t tmp;
    vec3_t t = v;
    t.normalize();

    float delt = normal.dot(t);
    if (fabs(delt) < 0.01f) {
        // v est dans le plan defini par normal
        // v et le plan sont parallele
        tmp = v;

    } else {
        // v n'est pas dans le plan defini par normal
        // v et le plan ne sont pas parallele
        float len = v.len();

        tmp = normal * delt * len;
    }

    return (v - tmp);
}
void CPhysEnv::reinitsprings(void)
{
    tParticle *p1, *p2;
    int loop;
    tSpring* spring;
    spring = m_Spring;
    for (loop = 0; loop < m_SpringCnt; loop++) {
        p1 = &m_CurrentSys[spring->p1];
        p2 = &m_CurrentSys[spring->p2];
        vec3_t tmp;
        tmp = p1->pos - p2->pos;
        spring->restLen = tmp.len();

        spring++; // DO THE NEXT SPRING
    }
}
void CPhysEnv::Calc_velocity(tParticle* system, tParticle* system_target)
{
    tParticle *curParticle, *curParticle_t;

    curParticle = system;
    curParticle_t = system_target;

    for (int loop = 0; loop < m_ParticleCnt; loop++) {
        curParticle->v = curParticle_t->pos - curParticle->pos;

        curParticle++;

        curParticle_t++;
    }
}
void CPhysEnv::ComputeForces(tParticle* system, BOOL duringIntegration, float delta)
{

    int loop;
    tParticle *curParticle, *p1, *p2;
    tSpring* spring;
    float dist, Hterm, Dterm;
    vec3_t springForce, deltaV, deltaP, Vn, Vt;
    //	float		FdotN,VdotN,Vmag;

    curParticle = system;

    for (loop = 0; loop < m_ParticleCnt; loop++) {
        curParticle->f = vec3_t(0.0f, 0.0f, 0.0f); // CLEAR FORCE VECTOR

        // BUG (3/8/99): THERE WAS A BUG HERE, I FORGOT YOU NEED TO DIVIDE
        //		BY 1/M TO GET THE TRUE FORCE ACTING ON THE PARTICLE INSTEAD OF A MULTIPLY
        //		THANKS TO ED POVAZ FOR THE SPOT...
        if (m_UseGravity && curParticle->oneOverM != 0) // && curParticle->type != CONTACTING)
        {
            curParticle->f = curParticle->f + m_Gravity / curParticle->oneOverM;
        }

        // if (curParticle->f.len()>10.0f)
        // curParticle->f=curParticle->f;

        // curParticle->f=vec3_t(0.0f,0.0f,-0.01f);

        // Handle Friction forces for Particles in contact with collision plane
        // Do not apply friction During integration phase
        if (curParticle->contacting && !duringIntegration && m_UseFriction) {
            // Calculate Fn
            //		 if ((loop==0) || (loop==1) || (loop==3) || (loop==2)    ) //|| ((loop==3) || (loop==2) )))
            //			curParticle->f+=(pulse+curParticle->contactN*curParticle->contactN.dot(pulse)*-1.0f);
            /*

                     FdotN = curParticle->contactN.dot(curParticle->f);
                    // Calculate Vt Velocity Tangent to Normal Plane
                    VdotN = curParticle->contactN.dot(curParticle->v);
                    Vn=curParticle->contactN*VdotN;
                    Vt=curParticle->v-Vn;
                    Vmag = VectorSquaredLength(Vt);
                    // Check if Velocity is faster then threshold
                      Vt.normalize();
                            Vt=Vt*(FdotN * m_Ckf);
                    //			curParticle->f=curParticle->f+Vt;

                    curParticle->f=curParticle->f+Vt;
*/
            /*
                                    }
                                    else	// Use Static Friction Model
                                    {
                                    */

            /*	Vmag = Vmag / STATIC_THRESHOLD;
                    Vt.normalize();
                    Vt=Vt*(FdotN * m_Ckf*Vmag);
                    curParticle->f=curParticle->f+Vt;
                    */
            //}

            if (false && ((loop == 0) || (loop == 1) || (loop == 3) || (loop == 2))) {
                //	 vec3_t Av=AxeRoueAv+pulse_roue;

                vec3_t Av = Rotate_roueX(AxeRoueAv, AxeHaut, rot_roue);
                vec3_t Cote = Rotate_roueX(AxeRoueAvY1, AxeHaut, rot_roue);

                Av.normalize();
                Cote.normalize();
                float patate = curParticle->f.len();

                vec3_t no = AxeDevant;
                Av.normalize();
                no.normalize();
                vec3_t dirr = Cote - curParticle->contactN * curParticle->contactN.dot(Cote);
                vec3_t dirrar = Cote - curParticle->contactN * curParticle->contactN.dot(no);
                vec3_t tmpv;

                if ((loop == 0) || (loop == 1))
                    curParticle->f += dirr * pulse * 10.0f;

                vec3_t ttt = AxeDevant;
                ttt.normalize();
                // projeter de la velocité sur le plan contact tmpv

                tmpv = (curParticle->v - curParticle->contactN * curParticle->contactN.dot(curParticle->v)); // toue a
                vec3_t ausolav;
                if ((loop == 3) || (loop == 2))                                                           // arriere
                    ausolav = (AxeRoueAv - curParticle->contactN * curParticle->contactN.dot(AxeRoueAv)); // toue a
                else
                    ausolav = (Av - curParticle->contactN * curParticle->contactN.dot(Av)); // toue a
                //	tmpv=tmpv-Av*Av.dot(tmpv);

                // ajout frotement
                vec3_t last = ausolav * tmpv.dot(ausolav);

                curParticle->f += last * -0.7f;
            }
            // curParticle->contacting = FALSE;
        }

        curParticle++;
    }

    // CHECK IF THERE IS A USER FORCE BEING APPLIED

    //	 / NOW DO ALL THE SPRINGS

    if (true) {
        spring = m_Spring;
        for (loop = 0; loop < m_SpringCnt; loop++) {
            p1 = &system[spring->p1];
            p2 = &system[spring->p2];
            deltaP = p1->pos - p2->pos; // Vector distance
            dist = deltaP.len();        /// /!\ arrondi a la con a changer					// Magnitude of
                                        /// deltaP
                                        //	double d =VectorLength(deltaP);

            Hterm = (dist - spring->restLen) * 0.01f; // spring->Ks Ks * (dist - rest) 0.9 raide
            /*		if(Hterm>-0.01f && Hterm<0.01f)
                            {
                            Hterm=0.0f;
                            }
            */
            deltaV = p1->v - p2->v; // Delta Velocity Vector

            // original Dterm = (DotProduct(&deltaV,&deltaP) * spring->Kd) / dist; // warning bat
            Dterm = (deltaP.dot(deltaV) * 0.02) / dist; // Damping Term0.05

            //	ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
            springForce = deltaP; // Normalize Distance Vector
            springForce.normalize();
            springForce = springForce * -(Hterm * 0.4f); //+ Dterm	//Calc Force
                                                         // springForce=springForce*(Hterm - Dterm);	// Calc Force
            //		if (p1->type != CONTACTING)
            p1->f = p1->f + springForce; // Apply to Particle 1
                                         //		if (p2->type != CONTACTING)
            p2->f = p2->f - springForce; // - Force on Particle 2
            //	p1->pos=p1->pos+springForce;			// Apply to Particle 1
            //		if (p2->type != CONTACTING)
            //	p2->pos=p2->pos-springForce;	// - Force on Particle 2

            spring++; // DO THE NEXT SPRING
        }
    }

    // APPLY THE MOUSE DRAG FORCES IF THEY ARE ACTIVE
    /*	if (m_MouseForceActive)
            {
                    // APPLY TO EACH PICKED PARTICLE
                    if (m_Pick[0] > -1)
                    {
                            p1 = &system[m_Pick[0]];
                            VectorDifference(&p1->pos,&m_MouseDragPos[0],&deltaP);	// Vector distance
                            dist = VectorLength(&deltaP);					// Magnitude of deltaP

                            if (dist != 0.0f)
                            {
                                    Hterm = (dist) * m_MouseForceKs;					// Ks * dist

                                    ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
                                    ScaleVector(&springForce,-(Hterm),&springForce);	// Calc Force
                                    VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
                            }
                    }
                    if (m_Pick[1] > -1)
                    {
                            p1 = &system[m_Pick[1]];
                            VectorDifference(&p1->pos,&m_MouseDragPos[1],&deltaP);	// Vector distance
                            dist = VectorLength(&deltaP);					// Magnitude of deltaP

                            if (dist != 0.0f)
                            {
                                    Hterm = (dist) * m_MouseForceKs;					// Ks * dist

                                    ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
                                    ScaleVector(&springForce,-(Hterm),&springForce);	// Calc Force
                                    VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
                            }
                    }
            }
    */
}

inline double CPhysEnv::VectorLength(const vec3_t& v) const
{
    int i;
    double length;

    length = 0;
    for (i = 0; i < 3; i++)
        length += v[i] * v[i];
    length = sqrt(length); // FIXME

    return length;
}

vec3_t CPhysEnv::Rotate_roueX(vec3_t axeAvant, vec3_t axeHautR, float angle)
{
    float mat[16];
    float Matrix[16];

    mat[0] = 1.0f;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 0.0f;
    mat[5] = 1.0f;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = axeAvant[0];
    mat[13] = axeAvant[1];
    mat[14] = axeAvant[2];
    mat[15] = 1.0f;

    mat[0] = 1.0f;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 0.0f;
    mat[5] = 1.0f;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = axeAvant[0];
    mat[13] = axeAvant[1];
    mat[14] = axeAvant[2];
    mat[15] = 1.0f;

    glPushMatrix();
    glLoadIdentity();

    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    //	glTranslatef(10.0f,0.0,10.0f);

    glRotatef(angle, axeHautR[0], axeHautR[1], axeHautR[2]);
    glMultMatrixf(mat);
    //	 vec3_t Av=rotate( ctor Av.
    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    vec3_t Av;
    Av[0] = Matrix[12];

    Av[1] = Matrix[13];

    Av[2] = Matrix[14];

    glPopMatrix();
    return Av;
}

vec3_t CPhysEnv::Rotate_vect(vec3_t axeAvant, vec3_t axeHautR, vec3_t src)
{
    float mat[16];
    float mata[16];
    float Matrix[16];

    mat[0] = axeAvant[0];
    mat[1] = axeAvant[1];
    mat[2] = axeAvant[2];
    mat[3] = 0.0f;

    mat[4] = axeHautR[0];
    mat[5] = axeHautR[1];
    mat[6] = axeHautR[2];
    mat[7] = 0.0f;

    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;

    mat[12] = src[0];
    mat[13] = src[1];
    mat[14] = src[2];
    mat[15] = 1.0f;

    mata[0] = 0.0f;
    mata[1] = 0.0f;
    mata[2] = 0.0f;
    mata[3] = 0.0f;

    mata[4] = 1.0f;
    mata[5] = 0.0f;
    mata[6] = 0.0f;
    mata[7] = 0.0f;

    mata[8] = 0.0f;
    mata[9] = 0.0f;
    mata[10] = 1.0f;
    mata[11] = 0.0f;

    mata[12] = 0.0f;
    mata[13] = 0.0f;
    mata[14] = 0.0f;
    mata[15] = 1.0f;

    glPushMatrix();
    glLoadIdentity();

    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    glMultMatrixf(mat);
    glTranslatef(10.0f, 0.0, 10.0f);
    glRotatef(0.5f, axeHautR[0], axeHautR[1], axeHautR[2]);

    // glMultMatrixf(mata);

    // glTranslatef(( GLfloat)src[0],( GLfloat) src[1],( GLfloat)  src[2]);

    //	 vec3_t Av=rotate( ctor Av.
    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    vec3_t Av;
    Av[0] = Matrix[12];

    Av[1] = Matrix[13];

    Av[2] = Matrix[14];

    glPopMatrix();
    return Av;
}

vec3_t CPhysEnv::Rotate_roueY(vec3_t axeAvant, vec3_t axeHautR, float angle)
{
    float mat[16];
    float Matrix[16];

    mat[0] = 1.0f;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 1.0f;
    mat[5] = 0.0f;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = axeAvant[0];
    mat[13] = axeAvant[1];
    mat[14] = axeAvant[2];
    mat[15] = 1.0f;

    mat[0] = 1.0f;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 0.0f;
    mat[5] = 1.0f;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = axeAvant[0];
    mat[13] = axeAvant[1];
    mat[14] = axeAvant[2];
    mat[15] = 1.0f;

    glPushMatrix();
    glLoadIdentity();

    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    //	glTranslatef(10.0f,0.0,10.0f);

    glRotatef(angle, axeHautR[0], axeHautR[1], axeHautR[2]);
    glMultMatrixf(mat);
    //	 vec3_t Av=rotate( ctor Av.
    glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); // get current matrix
    vec3_t Av;
    Av[0] = Matrix[12];

    Av[1] = Matrix[13];

    Av[2] = Matrix[14];

    glPopMatrix();
    return Av;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	IntegrateSysOverTime
// Purpose:		Does the Integration for all the points in a system
// Arguments:	Initial Position, Source and Target Particle Systems and Time
// Notes:		Computes a single integration step
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::IntegrateSysOverTime(tParticle* initial, tParticle* source, tParticle* target, float deltaTime)
{
    /// Local Variables ///////////////////////////////////////////////////////////
    int loop;
    float deltaTimeMass;
    ///////////////////////////////////////////////////////////////////////////////
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        deltaTimeMass = deltaTime * initial->oneOverM;
        // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
        /*	target->v.x = initial->v.x + (source->f.x * deltaTimeMass);
                target->v.y = initial->v.y + (source->f.y * deltaTimeMass);
                target->v.z = initial->v.z + (source->f.z * deltaTimeMass);
*/
        target->v = initial->v + (source->f * deltaTimeMass);

        target->oneOverM = initial->oneOverM;

        // SET THE NEW POSITION
        /*target->pos.x = initial->pos.x + (deltaTime * source->v.x);
        target->pos.y = initial->pos.y + (deltaTime * source->v.y);
        target->pos.z = initial->pos.z + (deltaTime * source->v.z);
*/
        target->pos = initial->pos + (source->v * deltaTime);

        initial++;
        source++;
        target++;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Function:	EulerIntegrate
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses Euler's method
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::EulerIntegrate(float DeltaTime)
{
    // JUST TAKE A SINGLE STEP
    IntegrateSysOverTime(m_CurrentSys, m_CurrentSys, m_TargetSys, DeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MidPointIntegrate
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Midpoint method
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::MidPointIntegrate(float DeltaTime)
{
    /// Local Variables ///////////////////////////////////////////////////////////
    float halfDeltaT;
    ///////////////////////////////////////////////////////////////////////////////
    halfDeltaT = DeltaTime / 2.0f;

    // TAKE A HALF STEP AND UPDATE VELOCITY AND POSITION
    IntegrateSysOverTime(m_CurrentSys, m_CurrentSys, m_TempSys[0], halfDeltaT);

    // COMPUTE FORCES USING THESE NEW POSITIONS AND VELOCITIES
    // ComputeForces(m_TempSys[0],true);

    // TAKE THE FULL STEP WITH THIS NEW INFORMATION
    IntegrateSysOverTime(m_CurrentSys, m_TempSys[0], m_TargetSys, DeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	RK4Integrate
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Runga-Kutta 4 method
//				This could use a generic function 4 times instead of unrolled
//				but it was easier for me to debug.  Fun for you to optimize.
///////////////////////////////////////////////////////////////////////////////

/*

int CPhysEnv::CheckForCollisions( tParticle	*system_old,tParticle	*system )
{
    // be optimistic!
    int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 0.2f;//3.8f;
 float const contactEpsilon = 0.35f;//5.0f;



        int loop;
        tParticle *curParticle;

tParticle *curParticle_old;



        m_ContactCnt = 0;		// THERE ARE CURRENTLY NO CONTACTS


        curParticle = system;
        curParticle_old = system_old;

        for (loop = 0; (loop < m_ParticleCnt);
                        loop++,curParticle++)
        {

 //my_world->process_visible_faces3(curParticle_old->pos);
                                my_world->process_visible_faces_collide(curParticle_old->pos,curParticle->pos-curParticle_old->pos);
                my_world->epsilon=3.0f;
                my_world->radius=2.0f;
                my_world->collide =false;
                my_world->normale_face_touchee[0]=0.0f;
                my_world->normale_face_touchee[1]=0.0f;
                my_world->normale_face_touchee[2]=0.0f;

                vec3_t dst = my_world->check_collisions(curParticle->pos,curParticle->pos-curParticle_old->pos,0);

                curParticle->pos=dst;

//curParticle->v=curParticle->pos-curParticle_old->pos;
                collisionState = NOT_COLLIDING;

                curParticle_old++;
                        }
                        return  collisionState;
}
*/
int CPhysEnv::CheckForCollisions(tParticle* system_old, tParticle* system)
{
    // be optimistic!
    int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 2.2f;    // 3.8f;
    float const contactEpsilon = 2.35f; // 5.0f;

    int loop;
    tParticle* curParticle;
    tParticle* oldcurParticle;

    m_ContactCnt = 0; // THERE ARE CURRENTLY NO CONTACTS

    curParticle = system;
    oldcurParticle = system_old;
    // vec3_t tmpdir=vec3_t(0.0f,0.0f,0.0f);

    for (loop = 0; (loop < m_ParticleCnt) && (collisionState != PENETRATING); //
         loop++, curParticle++) {

        if (true || (loop == 0 || loop == 1 || loop == 2 || loop == 3)) {
            //	tCollisionPlane *plane = &m_CollisionPlane[planeIndex];

            // float axbyczd = curParticle->pos.dot(plane->normal) ;
            //

            // float axbyczd = DotProduct(&curParticle->pos,&plane->normal) + plane->d;

            /*	if (my_world->find_leaf(curParticle->pos)==0)
                          {
                                          collisionState = PENETRATING; // d'office on est e nvrac on divise en deux et
               quit curParticle->contacting = TRUE;

                          }
                          else
                          {
  */

            collision_tir col_tir;
            vec3_t tmpdir = (curParticle->v);
            // my_world->process_visible_faces_collide(AxeG,tmpdir);

            // my_world->process_visible_faces3(curParticle->oldpos);

            // tmpdir=tmpdir*10.0f;

            my_world->process_visible_faces_collide(curParticle->pos, curParticle->v);
            my_world->epsilon = 0.05f;
            my_world->radius = 15.0f; // ne seert a rien
            my_world->collide = false;
            my_world->normale_face_touchee[0] = 0.0f;
            my_world->normale_face_touchee[1] = 0.0f;
            my_world->normale_face_touchee[2] = 0.0f;

            vec3_t normal;

            my_world->check_collisions_bat(curParticle->pos, tmpdir, &col_tir, 5);

            //	curParticle->pos=col_tir.pt;

            if ((col_tir.found)) {

                // vec3_t tmp=(curParticle->pos-col_tir.pt);

                vec3_t tmp = (curParticle->v);
                float axbyczd = tmpdir.dot(col_tir.normal);
                if (axbyczd < 0.5f) {
                    collisionState = COLLIDING;
                    m_Contact[m_ContactCnt].type = COLLIDING;
                    m_Contact[m_ContactCnt].particle = loop;
                    memcpy(&m_Contact[m_ContactCnt].normal, &col_tir.normal, sizeof(vec3_t));
                    m_ContactCnt++;
                    curParticle->contacting = true;
                    memcpy(&curParticle->contactN, &col_tir.normal, sizeof(vec3_t));
                }
            } else {
                curParticle->contacting = false;
            }
            //	}
            // oldcurParticle++;
        }
    }
    return collisionState;
}

void CPhysEnv::saveold(tParticle* system)
{
    // be optimistic!

    int loop;
    tParticle* curParticle;

    curParticle = system;
    for (loop = 0; (loop < m_ParticleCnt); loop++, curParticle++) {
        curParticle->oldpos = curParticle->pos;
    }
}

void CPhysEnv::copyold(tParticle* system)
{
    // be optimistic!

    int loop;
    tParticle* curParticle;
    curParticle = system;
    for (loop = 0; (loop < m_ParticleCnt); loop++, curParticle++) {
        curParticle->pos = curParticle->oldpos;
        // curParticle->f=vec3_t(0.0f,0.0f,0.0f);
        // curParticle->v=vec3_t(0.0f,0.0f,0.0f);

        //	ocurParticle++;
    }
}
/*
void CPhysEnv::copyold( tParticle	*system )
{
    // be optimistic!

        int loop;
        tParticle *curParticle;
        tParticle *ocurParticle;

        ocurParticle=m_TargetSys ;

        curParticle = system;
        for (loop = 0; (loop < m_ParticleCnt);
                        loop++,curParticle++)
        {
                curParticle->pos=ocurParticle->oldpos;
                curParticle->f=vec3_t(0.0f,0.0f,0.0f);
                curParticle->v=vec3_t(0.0f,0.0f,0.0f);

                curParticle->pos=ocurParticle->oldpos;



                ocurParticle++;

        }


}
*/
int CPhysEnv::CheckForCollisions2(tParticle* system)
{
    // be optimistic!
    int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 5.8f;
    float const contactEpsilon = 8.0f;

    int loop;
    tParticle* curParticle;

    m_ContactCnt = 0; // THERE ARE CURRENTLY NO CONTACTS

    curParticle = system;

    for (loop = 0; (loop < m_ParticleCnt) && (collisionState != PENETRATING); loop++, curParticle++) {

        //	tCollisionPlane *plane = &m_CollisionPlane[planeIndex];

        // float axbyczd = curParticle->pos.dot(plane->normal) ;
        //
        collision_tir col_tir;
        /*col_tir=myworld->check_tirs(part->pos,(part->pos-part->pold));*/
        my_world->process_visible_faces3(curParticle->pos);
        vec3_t tmpdir = (curParticle->pos - curParticle->oldpos);
        tmpdir.normalize();
        tmpdir = tmpdir * 3;

        col_tir = my_world->check_tirs_rec(curParticle->pos, tmpdir, 1);
        if ((col_tir.found) && (col_tir.nearest != 0.0f)) {
            vec3_t tmp = (curParticle->pos - col_tir.pt);
            float axbyczd = tmp.dot(col_tir.normal);
            // float axbyczd = DotProduct(&curParticle->pos,&plane->normal) + plane->d;

            if (axbyczd < -depthEpsilon) {
                // ONCE ANY PARTICLE PENETRATES, QUIT THE LOOP
                collisionState = PENETRATING;

            } else if (axbyczd < depthEpsilon) {
                float relativeVelocity;
                relativeVelocity = col_tir.normal.dot(curParticle->v);
                /*if(relativeVelocity < 0.01f && relativeVelocity > -0.01f)
                {
                        collisionState = CONTACTING;
                        curParticle->v=vec3_t(0.0f,0.0f,0.0f);
                        curParticle->type = CONTACTING;
                }
                else */
                if (relativeVelocity < 0.0f) {
                    collisionState = COLLIDING;
                    m_Contact[m_ContactCnt].type = COLLIDING;
                    m_Contact[m_ContactCnt].particle = loop;
                    memcpy(&m_Contact[m_ContactCnt].normal, &col_tir.normal, sizeof(vec3_t));
                    m_ContactCnt++;
                }
            }
            if (axbyczd < contactEpsilon) {
                curParticle->contacting = TRUE;
                // Save the contact normal for later
                memcpy(&curParticle->contactN, &col_tir.normal, sizeof(vec3_t));
            }

        } else {
            if (my_world->find_leaf(curParticle->pos) == 0) {
                collisionState = PENETRATING;
            }
        }
    }

    return collisionState;
}

void CPhysEnv::SetTraction(bool av, bool ar, bool dr, bool ga, bool fr)
{
    vec3_t AxeX
        = ((m_CurrentSys[5].pos + m_CurrentSys[4].pos) * 0.5f) - ((m_CurrentSys[6].pos + m_CurrentSys[7].pos) * 0.5f);
    AxeX.normalize();
    frein_a_main = fr;

    if ((av || ar)) {
        if (av) {
            // if(m_CurrentSys[0].v.len()<10.0f)
            pulse = 1.0f;

        } else {
            pulse = -1.0f;
            //	pulse_roue=pulse_roue*-1.0f;
        }
    }
    if (dr)
        rot_roue = -20.0f;

    if (ga)
        rot_roue = 20.0f;

    if (!(av || ar)) {
        pulse = 0.0f;
    }
    if (!(dr || ga)) {
        rot_roue = 0.0f;
    }
}

void CPhysEnv::ResolveCollisions(tParticle* system)
{
    tContact* contact;
    tParticle* particle; // THE PARTICLE COLLIDING
    float VdotN;
    vec3_t Vn, Vt; // CONTACT RESOLUTION IMPULSE
    contact = m_Contact;
    for (int loop = 0; loop < m_ContactCnt; loop++, contact++) {
        particle = &system[contact->particle];
        // CALCULATE Vn
        VdotN = contact->normal.dot(particle->v);
        Vn = contact->normal * VdotN;

        // CALCULATE Vt
        Vt = particle->v - Vn;
        // SCALE Vn BY COEFFICIENT OF RESTITUTION
        Vn = Vn * 0.01f; // m_Kr;
        //		Vn=Vn*0.3f;
        // SET THE VELOCITY TO BE THE NEW IMPULSE
        particle->v = Vt - Vn;
        particle->v = Vt - Vn;
    }
}
void CPhysEnv::ResolveCollisions_bat(tParticle* system_current, tParticle* system_target)
{
    tContact* contact;
    tParticle* particle_target; // THE PARTICLE COLLIDING
    float VdotN;
    vec3_t Vn, Vt; // CONTACT RESOLUTION IMPULSE
    contact = m_Contact;
    for (int loop = 0; loop < m_ContactCnt; loop++, contact++) {
        particle_target = &system_target[contact->particle];

        // CALCULATE Vn
        vec3_t velocity = particle_target->v;
        VdotN = contact->normal.dot(velocity);
        Vn = contact->normal * VdotN;

        // CALCULATE Vt
        Vt = velocity - Vn;
        // SCALE Vn BY COEFFICIENT OF RESTITUTION
        Vn = Vn * 0.0f; // m_Kr;
        //		Vn=Vn*0.3f;
        // SET THE VELOCITY TO BE THE NEW IMPULSE

        particle_target->v = (Vt - Vn);
    }
}

/*
void CPhysEnv::Simulate(float DeltaTime, BOOL running){

        float my_epsilon=0.001f;

    float		CurrentTime = 0.0f;
    float		TargetTime = DeltaTime;
        tParticle	*tempSys;
        int			collisionState;
        bool echec=false;
        collisionState=0;

char s[100];

    while((CurrentTime < DeltaTime))
    {
        //	sprintf(s,"%d",DeltaTime-CurrentTime );
                //	toto.push_back (s);
                if (running)
                {


                        ComputeForces(m_CurrentSys,FALSE,TargetTime);


                        // IN ORDER TO MAKE THINGS RUN FASTER, I HAVE THIS LITTLE TRICK
                        // IF THE SYSTEM IS DOING A BINARY SEARCH FOR THE COLLISION POINT,
                        // I FORCE EULER'S METHOD ON IT. OTHERWISE, LET THE USER CHOOSE.
                        // THIS DOESN'T SEEM TO EFFECT STABILITY EITHER WAY
                        EulerIntegrate(TargetTime-CurrentTime);
                }



                //col_tir.found=pos!=dir;
                //col_tir.nearest=1.0f;
                //col_tir.normal=

                collisionState = CheckForCollisions(m_CurrentSys,m_TargetSys);

                if (collisionState==PENETRATING)
                TargetTime = TargetTime/2.0f;
                else
                CurrentTime = TargetTime;

                //TargetTime = DeltaTime;

                // SWAP MY TWO PARTICLE SYSTEM BUFFERS SO I CAN DO IT AGAIN
//		saveold(m_CurrentSys);


        }//while

        tempSys = m_CurrentSys;
                m_CurrentSys = m_TargetSys;
                m_TargetSys = tempSys;

}




*/

void CPhysEnv::save_contact(tParticle* src, tParticle* dst)
{
    int loop;
    tParticle* curParticle;
    tParticle* curParticle_t;

    curParticle = src;
    curParticle_t = dst;
    for (loop = 0; (loop < m_ParticleCnt); loop++, curParticle++, curParticle_t++) {
        curParticle_t->contacting = curParticle->contacting;
    }
}

void CPhysEnv::restore_contact(tParticle* src, tParticle* dst)
{
    int loop;
    tParticle* curParticle;
    tParticle* curParticle_t;

    curParticle = src;
    curParticle_t = dst;
    for (loop = 0; (loop < m_ParticleCnt); loop++, curParticle++, curParticle_t++) {
        curParticle->contacting = curParticle_t->contacting;
    }
}
void CPhysEnv::RK4Integrate(float DeltaTime)
{
    /// Local Variables ///////////////////////////////////////////////////////////
    int loop;
    float halfDeltaT, sixthDeltaT;
    tParticle *source, *target, *accum1, *accum2, *accum3, *accum4;
    ///////////////////////////////////////////////////////////////////////////////
    halfDeltaT = DeltaTime / 2.0f; // SOME TIME VALUES I WILL NEED
    sixthDeltaT = 1.0f / 6.0f;

    // FIRST STEP
    source = m_CurrentSys; // CURRENT STATE OF PARTICLE
    target = m_TempSys[0]; // TEMP STORAGE FOR NEW POSITION
    accum1 = m_TempSys[1]; // ACCUMULATE THE INTEGRATED VALUES
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        accum1->f = source->f * halfDeltaT * source->oneOverM;

        accum1->v = source->v * halfDeltaT;
        // DETERMINE THE NEW VELOCITY FOR THE PARTICLE OVER 1/2 TIME
        target->v = source->v + (accum1->f);

        target->oneOverM = source->oneOverM;

        // SET THE NEW POSITION
        target->pos = source->pos + (accum1->v);

        source++;
        target++;
        accum1++;
    }

    ComputeForces(m_TempSys[0], TRUE, 1); // COMPUTE THE NEW FORCES

    // SECOND STEP
    source = m_CurrentSys; // CURRENT STATE OF PARTICLE
    target = m_TempSys[0]; // TEMP STORAGE FOR NEW POSITION
    accum1 = m_TempSys[2]; // ACCUMULATE THE INTEGRATED VALUES
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        accum1->f = target->f * halfDeltaT * source->oneOverM;
        accum1->v = target->v * halfDeltaT;

        // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
        target->v = source->v + (accum1->f);

        target->oneOverM = source->oneOverM;

        // SET THE NEW POSITION
        target->pos = source->pos + (accum1->v);

        source++;
        target++;
        accum1++;
    }

    ComputeForces(m_TempSys[0], TRUE, 1); // COMPUTE THE NEW FORCES

    // THIRD STEP
    source = m_CurrentSys; // CURRENT STATE OF PARTICLE
    target = m_TempSys[0]; // TEMP STORAGE FOR NEW POSITION
    accum1 = m_TempSys[3]; // ACCUMULATE THE INTEGRATED VALUES
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        // NOTICE I USE THE FULL DELTATIME THIS STEP
        accum1->f = target->f * DeltaTime * source->oneOverM;
        accum1->v = target->v * DeltaTime;

        // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
        target->v = source->v + (accum1->f);

        target->oneOverM = source->oneOverM;

        // SET THE NEW POSITION
        target->pos = source->pos + (accum1->v);

        source++;
        target++;
        accum1++;
    }

    ComputeForces(m_TempSys[0], TRUE, 1); // COMPUTE THE NEW FORCES

    // FOURTH STEP
    source = m_CurrentSys; // CURRENT STATE OF PARTICLE
    target = m_TempSys[0]; // TEMP STORAGE FOR NEW POSITION
    accum1 = m_TempSys[4]; // ACCUMULATE THE INTEGRATED VALUES
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        // NOTICE I USE THE FULL DELTATIME THIS STEP
        accum1->f = target->f * DeltaTime * source->oneOverM;

        accum1->v = target->v * DeltaTime;

        // THIS TIME I DON'T NEED TO COMPUTE THE TEMPORARY POSITIONS
        source++;
        target++;
        accum1++;
    }

    source = m_CurrentSys; // CURRENT STATE OF PARTICLE
    target = m_TargetSys;
    accum1 = m_TempSys[1];
    accum2 = m_TempSys[2];
    accum3 = m_TempSys[3];
    accum4 = m_TempSys[4];
    for (loop = 0; loop < m_ParticleCnt; loop++) {
        // DETERMINE THE NEW VELOCITY FOR THE PARTICLE USING RK4 FORMULA
        target->v = source->v + ((accum1->f + ((accum2->f + accum3->f) * 2.0f) + accum4->f) * sixthDeltaT);
        // DETERMINE THE NEW POSITION FOR THE PARTICLE USING RK4 FORMULA
        target->pos = source->pos + ((accum1->v + ((accum2->v + accum3->v) * 2.0f) + accum4->v) * sixthDeltaT);

        source++;
        target++;
        accum1++;
        accum2++;
        accum3++;
        accum4++;
    }
}

void CPhysEnv::Simulate(float DeltaTime, BOOL running)
{
    float CurrentTime = 0.0f;
    float TargetTime = DeltaTime;
    tParticle* tempSys;
    int collisionState;
    saveold(m_CurrentSys);
    int rec = 0;

    while (CurrentTime < DeltaTime) {
        if (running) {

            ComputeForces(m_CurrentSys, FALSE, DeltaTime - CurrentTime);

            // IN ORDER TO MAKE THINGS RUN FASTER, I HAVE THIS LITTLE TRICK
            // IF THE SYSTEM IS DOING A BINARY SEARCH FOR THE COLLISION POINT,
            // I FORCE EULER'S METHOD ON IT. OTHERWISE, LET THE USER CHOOSE.
            // THIS DOESN'T SEEM TO EFFECT STABILITY EITHER WAY
            if (m_CollisionRootFinding) {
                EulerIntegrate(TargetTime - CurrentTime);
            } else {
                RK4Integrate(TargetTime - CurrentTime);
            }
        }

        collisionState = CheckForCollisions(m_CurrentSys, m_TargetSys);
        // sauv contact
        save_contact(m_TargetSys, m_CurrentSys);
        if (collisionState == PENETRATING) {
            // TELL THE SYSTEM I AM LOOKING FOR A COLLISION SO IT WILL USE EULER
            m_CollisionRootFinding = TRUE;
            // we simulated too far, so subdivide time and try again
            TargetTime = (CurrentTime + TargetTime) / 2.0f;
            rec++;
            // blow up if we aren't moving forward each step, which is
            // probably caused by interpenetration at the frame start
            if (fabs(TargetTime - CurrentTime) < 0.001) // EPSILON
            {
                tempSys = m_TargetSys;
                m_TargetSys = m_CurrentSys;
                m_CurrentSys = tempSys;
                CurrentTime = DeltaTime;
                /*copyold(m_TargetSys);*/

                // pas de sol machine arriere
            }

        } else {
            rec = 0;
            // either colliding or clear
            if (collisionState == COLLIDING) {
                int Counter = 0;
                do {
                    ResolveCollisions_bat(m_CurrentSys, m_TargetSys);
                    Counter++;
                } while ((CheckForCollisions(m_CurrentSys, m_TargetSys) == COLLIDING) && (Counter < 100));

                if (Counter == 100) {
                    int s = CheckForCollisions(m_CurrentSys, m_TargetSys);
                }

                m_CollisionRootFinding = FALSE; // FOUND THE COLLISION POINT BACK TO NORMAL
                restore_contact(m_TargetSys, m_CurrentSys);

                TargetTime = DeltaTime;
                CurrentTime = TargetTime;
                tempSys = m_CurrentSys;
                m_CurrentSys = m_TargetSys;
                m_TargetSys = tempSys;

                /*	tempSys = m_CurrentSys;
                m_CurrentSys = m_TargetSys;
                m_TargetSys = tempSys;*/

            } else {
                // we made a successful step, so swap configurations
                // to "save" the data for the next step

                CurrentTime = TargetTime;
                TargetTime = DeltaTime;

                // SWAP MY TWO PARTICLE SYSTEM BUFFERS SO I CAN DO IT AGAIN

                tempSys = m_CurrentSys;
                m_CurrentSys = m_TargetSys;
                m_TargetSys = tempSys;
            }
        }
    }
}
