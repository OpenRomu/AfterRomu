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
#include "particleBurst.h"

void pSystemBurst_t::Init(vec3_t& pos, vec3_t& dir)
{
    Allocate(200);

    spawnTime = 0.5f;

    pPart_t* n = PartNew();

    if (!n)
        return;

    n->life = ticker_t::PassedI() + (uint)(random_t::RandomRange(50.0f, 100.0f));
    n->size = 5;
    n->vel = vec3_t(0.0f, 0.0f, 0.0f);
    n->pos = pos;
    n->pold = vec3_t(0.0f, 0.0f, 0.0f);
    n->gravity = 1;
    n->alpha = 1.0f;

    passed = 0.0f;
}

bool pSystemBurst_t::Frame(float& frametime, vec3_t& grav)
{
    passed += frametime;

    if (spawnTime > 0 && passed > spawnTime) {
        passed = 0.0f;
        //  MakeBoom();
    }

    return Cycle(frametime, grav);
}

void pSystemBurst_t::Die(void)
{
    // if (head)
    //  head->life = ticker_t::PassedI();

    spawnTime = -1;
}