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

#include "glinc.h"
// #include "util3d.h"
#include "particleMan.h"
#include "matrix.h"
#include "engine.h"
pParticleManager_t::pParticleManager_t()
{
    systems = 0;
    // geom = 0;
}

pParticleManager_t::~pParticleManager_t()
{
    pSystem_t *curr = systems, *temp;

    while (curr)
    {
        temp = curr;
        curr = curr->GetNext();
        delete temp;
    }
}

void pParticleManager_t::SystemNew(pSystem_t *sys, vec3_t &pos, bool die, vec3_t &dir)
{
    if (!sys)
        return;

    // if die == 1, kill all of the other systems before we add
    if (die)
        for (pSystem_t *curr = systems; curr; curr = curr->GetNext())
            curr->Die();

    // setup and link in
    sys->Init(pos, dir);
    sys->SetTexture(ids[sys->GetType()]);
    sys->SetPrev(0);
    sys->SetNext(systems);

    if (systems)
        systems->SetPrev(sys);
    systems = sys;
}

void pParticleManager_t::SystemDelete(pSystem_t *sys)
{
    if (sys->GetNext())
        sys->GetNext()->SetPrev(sys->GetPrev());
    if (sys->GetPrev())
        sys->GetPrev()->SetNext(sys->GetNext());

    if (!sys->GetNext() && !sys->GetPrev())
        systems = 0;
    if (!sys->GetPrev())
        systems = sys->GetNext();

    delete sys;
}

// this checks for collisions
void pParticleManager_t::Frame(float &frametime)
{
    pSystem_t *curr = systems;
    pSystem_t *n = 0;
    bool ok = 0;
    int tt = 0;
    while (curr)
    {
        tt++;

        n = curr->GetNext();

        if (!(ok = curr->Frame(frametime, gravity)))
            SystemDelete(curr);

        curr = n;
    }
}

void pParticleManager_t::Render(void)
{
    matrix_t mat, tmat; // static?
    vec3_t x, y;

    glGetFloatv(GL_MODELVIEW_MATRIX, mat.m);
    mat.Transpose(tmat);

    x[0] = -tmat[0];
    x[1] = -tmat[1];
    x[2] = -tmat[2];

    y[0] = tmat[4];
    y[1] = tmat[5];
    y[2] = tmat[6];

    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);

    for (pSystem_t *curr = systems; curr; curr = curr->GetNext())
    {
        curr->Render(x, y);
    }

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    //	glEnable(GL_DEPTH_FUNC);
}
