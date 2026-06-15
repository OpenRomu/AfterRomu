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
#include "ticker.h"
// #include "util3d.h"
#include "particle.h"
#include "particleman.h"

// pPart_t
pPart_t::pPart_t()
{
    next = prev = 0;
    life = 0;
    size = 0;
    gravity = true;
    alpha = 1.0f;
}

// pSystem_t
pSystem_t::pSystem_t()
{
    next = prev = 0;
    pAll = pFree = 0;
    pUse.next = &pUse;
    pUse.prev = &pUse;
}

pSystem_t::~pSystem_t()
{
    delete[] pAll;
}

void pSystem_t::Allocate(int num)
{
    pAll = new pPart_t[num];

    pUse.next = &pUse;
    pUse.prev = &pUse;

    pFree = pAll;

    for (int i = 0; i < num - 1; i++)
        pAll[i].next = &pAll[i + 1];
}

pPart_t *pSystem_t::PartNew(void)
{
    if (!pFree)
        return 0;

    pPart_t *n = pFree;

    pFree = pFree->next;

    n->prev = &pUse;
    n->next = pUse.next;
    pUse.next->prev = n;
    pUse.next = n;

    return n;
}

void pSystem_t::PartDelete(pPart_t *d)
{
    // lespos.push_back(&lejoueur[j]->pos)	;

    d->prev->next = d->next;
    d->next->prev = d->prev;

    d->next = pFree;
    pFree = d;
}

bool pSystem_t::Cycle(float &frametime, vec3_t &grav)
{
    pPart_t *curr = pUse.next;
    pPart_t *next = 0;

    if (pUse.next == &pUse)
        return 0;

    while (curr != &pUse)
    {
        next = curr->next;
        curr->pold = curr->pos;
        curr->pos += (curr->vel * 2 * frametime); // hack hack hack
        if (type == 6)
            curr->size = curr->size + frametime;

        if ((curr->life - ticker_t::PassedI()) < 1000 && curr->life > 0)
            curr->alpha -= frametime;

        if (type != 6)
            curr->vel += (grav * curr->gravity * frametime);
        else
            curr->vel += (grav * curr->gravity / 10.0f * frametime);

        if (curr->life > 0 && (curr->life < ticker_t::PassedI()))
        {
            PartDelete(curr);
        }

        curr = next;
    }
    return 1;
}

// vertex arrays?
void pSystem_t::Render(vec3_t &x, vec3_t &y)
{
    vec3_t pts[4];
    vec3_t mx = -x;
    vec3_t my = -y;

    glDepthMask(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPolygonOffset(-1.0f, -2.0f);
    glBindTexture(GL_TEXTURE_2D, texture);

    vec3_t s;

    for (pPart_t *curr = pUse.next; curr != &pUse; curr = curr->next)
    {
        pts[0] = curr->pos + ((mx + my) * curr->size);
        pts[1] = curr->pos + ((x + my) * curr->size);
        pts[2] = curr->pos + ((x + y) * curr->size);
        pts[3] = curr->pos + ((mx + y) * curr->size);

        glColor4f(1.0f, 1.0f, 1.0f, curr->alpha); //
        glBegin(GL_TRIANGLE_FAN);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3fv(pts[0]);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3fv(pts[1]);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3fv(pts[2]);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3fv(pts[3]);
        glEnd();
    }
    glDepthMask(1);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

// reflect the velocity vector
void pSystem_t::Collide(pPart_t *part, vec3_t &n)
{
    //   ReflectVector(part->vel, n);
    //  part->pos.Set(part->pold);
}

void pSystem_t::drawBox(vec3_t *v)
{

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 10; i++)
        glVertex3fv(v[i & 7]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(v[6]);
    glVertex3fv(v[0]);
    glVertex3fv(v[4]);
    glVertex3fv(v[2]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(v[1]);
    glVertex3fv(v[7]);
    glVertex3fv(v[3]);
    glVertex3fv(v[5]);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}
