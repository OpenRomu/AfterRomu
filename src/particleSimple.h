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
#ifndef __PARTICLESIMPLEH__
#define __PARTICLESIMPLEH__

#include "particleMan.h"

struct pSystemSimple_t : public pSystem_t
{
    pSystemSimple_t()
    {
        type = pTypeSimple;
        spawnTime = 0;
    }

    void Init(vec3_t &pos, vec3_t &dir);
    bool Frame(float &frametime, vec3_t &grav);
    void Die(void);
    void Spawn(void);

  protected:
    float spawnTime;
    float passed;
    vec3_t spawn;
};

#endif