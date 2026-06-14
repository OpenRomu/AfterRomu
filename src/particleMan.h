/*
   Copyright (C) 2000 Nate Miller nkmiller@calpoly.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
#ifndef __PARTICLEMANH__
#define __PARTICLEMANH__

#include "geom.h"
// #include "engine.h"
#include "particle.h"

// !!must update this when a new system is added
enum
{
    pNumTypes = 7,
    pTypeSimple = 0,
    pTypeSpit,
    pTypeBurst,
    pTypeBoom,
    pTypeTorch,
    pTypeSmoke,
    pTypeHeSmoke
};

struct pParticleManager_t
{
    pParticleManager_t();
    virtual ~pParticleManager_t();

    pSystem_t *GetSystems(void)
    {
        return systems;
    }

    void SetGravity(vec3_t &g)
    {
        gravity = g;
    }
    void SetId(int id, uint t)
    {
        if (id < pNumTypes)
            ids[id] = t;
    }
    // void SetGeom(gGeom_t *g) {geom = g;}

    void SystemNew(pSystem_t *sys, vec3_t &pos, bool die = 0, vec3_t &dir = vec3_t(0.0f, 0.0f, 0.0f));
    void SystemDelete(pSystem_t *sys);
    void Frame(float &frametime);
    void Render(void);

  protected:
    //	world_t  * pworld;
    uint ids[pNumTypes];
    vec3_t gravity;
    // gGeom_t *geom; // don't delete, we don't own it
    pSystem_t *systems;
};

#endif