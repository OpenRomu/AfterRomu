#include "stdafx.h"
#include "rand.h"
#include "ticker.h"
#include "grenadeSimple.h"

void pGrenSystemSimple_t::Init(vec3_t &pos, vec3_t &dir, DPNID amoi)
{
    Allocate(50);

    spawn = pos;
    dirspawn = dir * 300.0f;

    spawnTime = 1.0f;
    passed = 0.0f;
    Spawn(amoi);
}

bool pGrenSystemSimple_t::Frame(float &frametime, vec3_t &grav, vector<vec3_t> *lespos, vector<DPNID> *amoi,
                                vector<vec3_t> *lesposjoueur, DPNID mon_id, vector<DPNID> *lesid_joueur,
                                vector<Aplayer *> *lesjoueurs)
{
    passed += frametime;

    if (spawnTime > 0 && passed > spawnTime)
    {
        passed = 0.0f;
        // Spawn();
    }

    return Cycle(frametime, grav, lespos, amoi, lesposjoueur, mon_id, lesid_joueur, lesjoueurs);
}

void pGrenSystemSimple_t::Die(void)
{
    spawnTime = -1.0f;
}

void pGrenSystemSimple_t::Spawn(DPNID amoi)
{
    for (int i = 0; i < 1; i++)
    {
        pGren_t *n = GrenNew();

        if (!n)
            return;

        n->life = ticker_t::PassedI() + (uint)3000.0f;
        n->size = 5.0f;
        n->vel = dirspawn; //
        n->pos = spawn;
        n->pold = spawn;
        n->gravity = 1;
        n->alpha = 1.0f;
        n->id_lanceur = amoi;
        n->rebond = true;
        n->colle = false;
    }
}