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
#include "stdafx.h"
#include "rand.h"
#include "ticker.h"
#include "particleTorch.h"

void pSystemTorch_t::Init(vec3_t &pos, vec3_t &dir)
{
    Allocate(200);

    spawnTime = 0.1f;

    if (!(head = PartNew()))
        return;

    head->life = 100; // always alive
    head->size = 15.0f;
    head->vel = vec3_t(0.0f, 0.0f, 0.0f);
    head->pos = pos;
    head->pold = vec3_t(0.0f, 0.0f, 0.0f);
    head->gravity = 0;
    head->alpha = 1.0f;

    Spawn();

    passed = 0.0f;
}

bool pSystemTorch_t::Frame(float &frametime, vec3_t &grav)
{
    passed += frametime;

    if (passed > spawnTime && head)
    {
        passed = 0.0f;
        //  Spawn();
    }

    if (head && head->life != 0 && head->life < ticker_t::PassedI())
        head = 0;

    return Cycle(frametime, grav);
}

void pSystemTorch_t::Die(void)
{
    if (head)
        head->life = ticker_t::PassedI();
}

void pSystemTorch_t::Spawn(void)
{
    for (int i = 0; i < 10; i++)
    {
        pPart_t *n = PartNew();

        if (n)
        {
            n->life = ticker_t::PassedI() + (uint)(random_t::RandomRange(300.0f, 600.0f));
            n->size = random_t::RandomRange(10.0f, 150.0f);

            n->vel[0] = random_t::RandomRange(-300.0f, 300.0f);
            n->vel[1] = random_t::RandomRange(-300.0f, 300.0f);
            n->vel[2] = random_t::RandomRange(10.0f, 300.0f);

            n->pos = head->pos;
            n->pos[0] += random_t::RandomRange(-5.0f, 5.0f);
            n->pos[1] += random_t::RandomRange(-5.0f, 5.0f);
            n->pos[2] += random_t::RandomRange(-5.0f, 5.0f);
            n->pold = head->pos;
            n->gravity = 1;
            n->alpha = 1.0f;
        }
    }
}