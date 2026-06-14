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
#include "mark.h"
#include "rand.h"
#include "glinc.h"
#include "ticker.h"

static int markLife = 50000;
static int traceLife = 100;

mMarkManager_t::mMarkManager_t()
{
    marksMax = 0;
    marksAll = marksFree = 0;
    texture = 0;
}

mMarkManager_t::~mMarkManager_t()
{
    delete[] marksAll;
}

void mMarkManager_t::Init(int n)
{
    marksMax = n;

    marksFree = marksAll = new mMark_t[marksMax];

    marksUse.next = &marksUse;
    marksUse.prev = &marksUse;

    for (int i = 0; i < marksMax - 1; i++)
        marksAll[i].next = &marksAll[i + 1];
}

void mMarkManager_t::Frame(float &frametime)
{
    mMark_t *n = 0;

    for (mMark_t *curr = marksUse.next; curr != &marksUse; curr = n)
    {
        n = curr->next;

        if ((curr->life - ticker_t::PassedI()) < 1000) // fade out as it dies
            curr->alpha -= frametime;

        if (curr->life < ticker_t::PassedI())
            MarkDelete(curr);
    }
}

// could use vertex arrays, but blah to it
void mMarkManager_t::Render(void)
{
    glDepthMask(0);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); ok
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -2.0f);
    unsigned int rr = ticker_t::PassedI();
    for (mMark_t *curr = marksUse.next; curr != &marksUse; curr = curr->next)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, curr->texture);
        glColor4f(1.0f, 1.0f, 1.0f, curr->alpha);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3fv(curr->v[0]);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3fv(curr->v[1]);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3fv(curr->v[2]);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3fv(curr->v[3]);
        glEnd();
        //
        if (texture_trace == curr->texture && (curr->color < 5))
        {
            //					glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); //ok
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBindTexture(GL_TEXTURE_2D, curr->texture);
            if (curr->color == 1)
                glColor4f(0.6f, 0.5f, 0.5f, 0.4f);
            else if (curr->color == 2)
                glColor4f(0.1f, 0.5f, 0.6f, 0.5f);
            else
                glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
            //	vec3_t longueur=curr->normal;
            vec3_t n = curr->normal;
            n.normalize();
            vec3_t m = curr->normal;
            m.normalize();

            vec3_t longueur = curr->normal;
            // vec3_t longueur=n*200.0f;
            /*unsigned int  tt=(rr-(curr->life-(13000+ traceLife)));
            n=m*tt;

                vec3_t a=n-curr->normal;
                vec3_t b=n-curr->normal;
                vec3_t c=n-curr->normal;
                vec3_t d=n-curr->normal;*/
            vec3_t a = ((curr->v[0] + curr->v[1]) / 2.0f);
            vec3_t b = ((curr->v[2] + curr->v[3]) / 2.0f);
            vec3_t c = ((curr->v[1] + curr->v[2]) / 2.0f);
            vec3_t d = ((curr->v[3] + curr->v[0]) / 2.0f);

            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3fv(a);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3fv(b);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3fv(b + longueur);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3fv(a + longueur);
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3fv(b);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3fv(a);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3fv(a + longueur);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3fv(b + longueur);
            glEnd();
            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3fv(c + longueur);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3fv(d + longueur);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3fv(d);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3fv(c);
            glEnd();

            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3fv(d + longueur);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3fv(c + longueur);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3fv(c);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3fv(d);
            glEnd();
            /*
                    glBindTexture(GL_TEXTURE_2D, curr->texture);
                      glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
                      glBegin(GL_TRIANGLE_FAN);
                     glTexCoord2f(0.0f, 0.0f);
                     glVertex3fv(c+curr->normal);
                     glTexCoord2f(1.0f, 0.0f);
                     glVertex3fv(d+curr->normal);
                     glTexCoord2f(1.0f, 1.0f);
                     glVertex3fv(a);
                     glTexCoord2f(0.0f, 1.0f);
                     glVertex3fv(b);
                  glEnd();
                  */
            /*
                        glBegin  (GL_TRIANGLE_FAN);
                        glVertex3fv (curr->pos+b);
                        glVertex3fv (curr->pos+a);
                        glVertex3fv (curr->pos+curr->vel+a);
                        glVertex3fv (curr->pos+curr->vel+b);
                        glEnd ();
                        glBegin  (GL_TRIANGLE_FAN);
                        glVertex3fv (curr->pos+d);
                        glVertex3fv (curr->pos+c);
                        glVertex3fv (curr->pos+curr->vel+c);
                        glVertex3fv (curr->pos+curr->vel+d);
                        glEnd ();
            */
        }
    }

    glDepthMask(1);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

static vec3_t axis[3] = {vec3_t(1.0f, 0.0f, 0.0f), vec3_t(0.0f, 1.0f, 0.0f), vec3_t(0.0f, 0.0f, 1.0f)};

#define MyFab(a) (a > 0 ? a : -a)

void mMarkManager_t::AddMark(vec3_t &n, vec3_t &pt, int type_mark)
{
    mMark_t *m = MarkNew();
    vec3_t fn(MyFab(n[0]), MyFab(n[1]), MyFab(n[2]));

    if (!m)
        return;

    vec3_t up, right;
    int major = 0;

    // find the major axis
    if (fn[1] > fn[major])
        major = 1;
    if (fn[2] > fn[major])
        major = 2;
    right[1] = 0; // debug vs2005
    right[2] = 1;
    right[0] = 0;

    // build right vector by hand
    if (fn[0] == 1 || fn[1] == 1 || fn[2] == 1)
    {
        if ((major == 0 && n[0] > 0) || major == 1)
        { // right.Set(0, 0, -1);
            right[1] = 0;
            right[2] = 1;
            right[0] = 0;
        }

        else if (major == 0)
        { // right.Set(0, 0, 1);
            right[1] = 0;
            right[2] = -1;
            right[0] = 0;
        }
        else
        {
            // right.Set(1 * n[2], 0, 0);
            right[1] = 1 * n[2];
            right[2] = 0;
            right[0] = 0;
        }
    }
    else
    {
        // right.cross(axis[major], n);
        right = axis[major].cross(n);
    }

    up = n.cross(right);

    up.normalize();
    //  up=up*-1.00f;
    right.normalize();
    // size=-size;

    // make the face
    float size;
    float sizeh;

    if (type_mark == 0)
    {
        m->texture = texture;
        size = 2.0f;
        sizeh = 2.0f;
        m->life = ticker_t::PassedI() + markLife;
    }
    else
    {
        if (type_mark == 20)
        { // trace

            m->texture = texture_trace;
            size = (2.0f);
            ;
            sizeh = (2.0f);
            ;
            m->life = ticker_t::PassedI() + traceLife;
        }
        else
        {
            m->life = ticker_t::PassedI() + markLife;
            m->texture = texture_sang;
            size = (random_t::RandomRange(15.0f, 25.0f));
            ;
            sizeh = (random_t::RandomRange(15.0f, 25.0f));
            ;
        }
    }

    m->v[0] = pt + ((-right - up) * size);
    m->v[1] = pt + ((right - up) * sizeh);
    m->v[2] = pt + ((right + up) * sizeh);
    m->v[3] = pt + ((-right + up) * size);
    m->normal = n;
    m->color = random_t::RandomRange(0, 10);
    m->alpha = 1.0f;
}

mMark_t *mMarkManager_t::MarkNew(void)
{
    // take away the oldest mark, its at the end so we assume its the oldest
    if (!marksFree)
        MarkDelete(marksUse.prev);

    mMark_t *n = marksFree;

    marksFree = marksFree->next;

    n->prev = &marksUse;
    n->next = marksUse.next;
    marksUse.next->prev = n;
    marksUse.next = n;

    return n;
}

void mMarkManager_t::MarkDelete(mMark_t *m)
{
    // take out of list
    m->prev->next = m->next;
    m->next->prev = m->prev;
    // link into free list
    m->next = marksFree;
    marksFree = m;
}

void mMarkManager_t::drawBox(vec3_t *v)
{

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //	  glDepthMask(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, texture); // texture

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
