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
#ifndef __PARTICLEH__
#define __PARTICLEH__

#include "geom.h"
#include "utility.h"
// #include "particleSimple.h"
struct pPart_t {
    pPart_t();

    uint life;
    float size;
    vec3_t pos;
    vec3_t pold;
    vec3_t vel;
    // vec3_t color; // could use, but i don't
    float alpha;
    int gravity;
    pPart_t *next, *prev;
};

static const int pSystemMaxPart = 50;

struct pSystem_t {
    pSystem_t();
    virtual ~pSystem_t();

    pSystem_t* GetNext(void) { return next; }
    pSystem_t* GetPrev(void) { return prev; }
    pPart_t* GetAlive(void) { return &pUse; }
    int GetType(void) { return type; }
    void SetNext(pSystem_t* n) { next = n; }
    void SetPrev(pSystem_t* p) { prev = p; }
    void SetTexture(uint id) { texture = id; }

    void Allocate(int num);
    pPart_t* PartNew(void);
    void PartDelete(pPart_t* d);
    bool Cycle(float& frametime, vec3_t& grav);
    void drawBox(vec3_t* v);
    virtual void Init(vec3_t& pos, vec3_t& dir) = 0;
    virtual bool Frame(float& frametime, vec3_t& grav) = 0;
    virtual void Render(vec3_t& x, vec3_t& y);
    virtual void Collide(pPart_t* part, vec3_t& n);
    virtual void Die(void) = 0;

protected:
    int type;     // !!important, must be set in derived classes constructor
    uint texture; // texture id

    pPart_t* pAll;  // single link
    pPart_t pUse;   // double link
    pPart_t* pFree; // single link
    pSystem_t *next, *prev;
};

#endif