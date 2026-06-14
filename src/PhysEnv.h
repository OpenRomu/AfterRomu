#if !defined(AFX_PhysEnv_H__3DC11AC3_95FB_11D2_9D83_00105A124906__INCLUDED_)
#define AFX_PhysEnv_H__3DC11AC3_95FB_11D2_9D83_00105A124906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhysEnv.h : header file
//
// #include "MathDefs.h"
#include "geom.h"
#include "world.h"
#include "vehicle.h"
#include "particleMan.h"

// #define EPSILON  0.00001f				// ERROR TERM
#define DEFAULT_DAMPING 0.002f

enum tCollisionTypes
{
    NOT_COLLIDING,
    PENETRATING,
    COLLIDING,
    CONTACTING
};

enum tIntegratorTypes
{
    EULER_INTEGRATOR,
    MIDPOINT_INTEGRATOR,
    RK4_INTEGRATOR
};

// TYPE FOR A PLANE THAT THE SYSTEM WILL COLLIDE WITH
struct tCollisionPlane
{
    vec3_t normal; // inward pointing
    float d;       // ax + by + cz + d = 0
};

// TYPE FOR A PHYSICAL PARTICLE IN THE SYSTEM
struct tParticle
{
    vec3_t pos;      // Position of Particle
    vec3_t v;        // Velocity of Particle
    vec3_t f;        // Force Acting on Particle
    float oneOverM;  // 1 / Mass of Particle
    vec3_t contactN; // Normal of Contact
    BOOL contacting;
    vec3_t oldpos; // Position of Particle
};

// TYPE FOR CONTACTS THAT ARE FOUND DURING SIM
struct tContact
{
    int particle;  // Particle Index
    vec3_t normal; // Normal of Collision plane
    int type;      // COLLIDING OR CONTACT
};

// TYPE FOR SPRINGS IN SYSTEM
struct tSpring
{
    int p1, p2;    // PARTICLE INDEX FOR ENDS
    float restLen; // LENGTH OF SPRING AT REST
    float Ks;      // SPRING CONSTANT
    float Kd;      // SPRING DAMPING
};

class CPhysEnv
{
    // Construction
  public:
    CPhysEnv();
    void RenderWorld();
    vec3_t Rotate_vect(vec3_t axeAvant, vec3_t axeHautR, vec3_t src);
    void RenderFake(vec3_t pos, vec3_t AxeX, vec3_t AxeY);
    void SetWorldParticles(vec3_t *coords, int particleCnt);
    void SetTexture(GLuint texid)
    {
        my_car.SetTexture(texid);
        my_roueard.SetTexture(texid);
        my_rouearg.SetTexture(texid);
        my_roueavd.SetTexture(texid);
        my_roueavg.SetTexture(texid);
    }
    vec3_t LaNormalA(vec3_t P1, vec3_t P2);
    void SetEXPLODE(pParticleManager_t *parts)
    {
        engineparts = parts;
    }
    void ResetWorld();
    void Simulate(float DeltaTime, BOOL running);
    void save_contact(tParticle *src, tParticle *dst);
    void restore_contact(tParticle *src, tParticle *dst);
    void RK4Integrate(float DeltaTime);
    void ApplyUserForce(vec3_t *force);
    void SetMouseForce(int deltaX, int deltaY, vec3_t *localX, vec3_t *localY);
    void GetNearestPoint(int x, int y);
    void AddSpring(float a_Ksd, int a, int b);
    void AddVertex(int id, vec3_t pos);
    void SetVertexMass();
    void SetWorldProperties();
    void FreeSystem();
    void LoadData(vec3_t offset);
    void SetPos(vec3_t offset, vec3_t velo);
    void SaveData(FILE *fp);
    void SetWorld(world_t *world)
    {
        my_world = world;
    }
    void SetTraction(bool av, bool ar, bool dr, bool ga, bool fr);
    vec3_t Get_Global_velocity();
    void SetShoot(vec3_t velo);
    void SetColision(vec3_t velo);
    double VectorLength(const vec3_t &v) const;
    vec3_t AxeG;
    vec3_t AxeDevant;
    float GlobalVelo;
    vec3_t AxeHaut;
    vec3_t AxeRoueAv;
    vec3_t AxeRoueAvY1;
    vec3_t AxeRoueAr;
    Texture *m_tex;
    GLuint id_texture_jeep;
    //	texMan_t* m_tex;

    BOOL m_UseGravity;           // SHOULD GRAVITY BE ADDED IN
    BOOL m_UseDamping;           // SHOULD DAMPING BE ON
    BOOL m_UserForceActive;      // WHEN USER FORCE IS APPLIED
    BOOL m_DrawSprings;          // DRAW THE SPRING LINES
    BOOL m_DrawVertices;         // DRAW VERTICES
    BOOL m_MouseForceActive;     // MOUSE DRAG FORCE
    BOOL m_CollisionRootFinding; // TRYING TO FIND A COLLISION
    int m_IntegratorType;
    vec3_t traceur;
    vehicle my_car;
    roue my_roueavd;
    roue my_roueavg;
    roue my_roueard;
    roue my_rouearg;

    // Attributes
  private:
    pParticleManager_t *engineparts;

    world_t *my_world;
    float m_WorldSizeX, m_WorldSizeY, m_WorldSizeZ;
    vec3_t m_Gravity;   // GRAVITY FORCE VECTOR
    vec3_t m_UserForce; // USER FORCE VECTOR
    float pulse;
    bool frein_a_main;
    // vec3_t				pulse_roue;
    float rot_roue;

    float m_UserForceMag; // MAGNITUDE OF USER FORCE
    float m_Kd;           // DAMPING FACTOR
    float m_Kr;           // COEFFICIENT OF RESTITUTION
    float m_Ksh;          // HOOK'S SPRING CONSTANT
    float m_Ksd;          // SPRING DAMPING
    float m_MouseForceKs; // MOUSE SPRING COEFFICIENT
    // tCollisionPlane		*m_CollisionPlane;		// LIST OF COLLISION PLANES
    //	int					m_CollisionPlaneCnt;
    tContact *m_Contact; // LIST OF POSSIBLE COLLISIONS
    int m_ContactCnt;    // COLLISION COUNT
    // tParticle			*m_ParticleSys[3];		// LIST OF PHYSICAL PARTICLES
    tParticle *m_CurrentSys, *m_TargetSys;
    tParticle *m_TempSys[5]; // SETUP FOR TEMP PARTICLES USED WHILE INTEGRATING
    int m_ParticleCnt;
    tSpring *m_Spring; // VALID SPRINGS IN SYSTEM

    int m_SpringCnt;
    int m_Pick[2];            // INDEX COUNTERS FOR SELECTING
    vec3_t m_MouseDragPos[2]; // POSITION OF DRAGGED MOUSE VECTOR
    // Operations
  private:
    inline void IntegrateSysOverTime(tParticle *initial, tParticle *source, tParticle *target, float deltaTime);
    //	void	RK4Integrate( float DeltaTime);
    vec3_t Rotate_roueX(vec3_t axeAvant, vec3_t axeHautR, float angle);
    vec3_t Rotate_roueY(vec3_t axeAvant, vec3_t axeHautR, float angle);
    void MidPointIntegrate(float DeltaTime);
    void EulerIntegrate(float DeltaTime);
    void ComputeForces(tParticle *system, BOOL duringIntegration, float delta);
    void Calc_velocity(tParticle *system, tParticle *system_target);
    void reinitsprings(void);

    vec3_t projeter(vec3_t normal, vec3_t v);

    int CheckForCollisions(tParticle *system_old, tParticle *system);
    int CheckForCollisions2(tParticle *system);
    void ResolveCollisions(tParticle *system);
    void ResolveCollisions_bat(tParticle *system_current, tParticle *system_target);
    void CompareBuffer(int size, float *buffer, float x, float y);
    void saveold(tParticle *system);
    void copyold(tParticle *system);
    // Implementation

  public:
    virtual ~CPhysEnv();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PhysEnv_H__3DC11AC3_95FB_11D2_9D83_00105A124906__INCLUDED_)
