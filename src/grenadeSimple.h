#ifndef __GRENADESIMPLEH__
#define __GRENADESIMPLEH__

#include "GrenadeMan.h"

struct pGrenSystemSimple_t : public pGrenSystem_t
{
    pGrenSystemSimple_t()
    {
        type = pgTypeSimple;
        spawnTime = 0;
    }

    void Init(vec3_t &pos, vec3_t &dir, DPNID amoi);
    bool Frame(float &frametime, vec3_t &grav, vector<vec3_t> *lespos, vector<DPNID> *amoi,
               vector<vec3_t> *lesposjoueur, DPNID mon_id, vector<DPNID> *lesid_joueur, vector<Aplayer *> *lesjoueurs,
               vector<CPhysEnv *> lescars);
    void Die(void);
    void Spawn(DPNID amoi);

  protected:
    float spawnTime;
    float passed;
    vec3_t spawn;
    vec3_t dirspawn;
};

#endif