#ifndef __GRENADEMANH__
#define __GRENADEMANH__
// #include "player.h"
#include "geom.h"
#include "world.h"
#include "grenade.h"
#include "particleMan.h"
//
// #include "vector"

// !!must update this when a new system is added
enum
{
    pgNumTypes = 5,
    pgTypeSimple = 0,
    pgTypeMissile = 1,
    pgTypeSmoke = 2,
    pgTypeLazer = 3,
};

struct pGrenadeManager_t
{
    world_t *myworld;
    pGrenadeManager_t();
    virtual ~pGrenadeManager_t();

    pGrenSystem_t *GetSystems(void)
    {
        return systems;
    }

    void SetGravity(vec3_t &g)
    {
        gravity = g;
    }
    void SetId(int id, uint t)
    {
        if (id < pgNumTypes)
            ids[id] = t;
    }
    void SetWorld(world_t *world)
    {
        myworld = world;
    }
    void SetEXPLODE(pParticleManager_t *parts)
    {
        engineparts = parts;
    }

    void SystemNew(pGrenSystem_t *sys, vec3_t &pos, vec3_t &dir, bool die = 0, DPNID id_lanceur = 0);
    void SystemDelete(pGrenSystem_t *sys);
    void Frame(float &frametime, vector<vec3_t> *lespos, vector<DPNID> *amoi, vector<vec3_t> *lesposjoueur,
               DPNID mon_id, vector<DPNID> *lesid_joueur, vector<Aplayer *> *lesjoueurs);
    void Render(void);

  protected:
    //	world_t  * pworld;
    uint ids[pgNumTypes];
    vec3_t gravity;
    DPNID MON_ID;

    // gGeom_t *geom; // don't delete, we don't own it
    pParticleManager_t *engineparts;
    //   vector<Aplayer*> * engineplayers;

    pGrenSystem_t *systems;
};

#endif