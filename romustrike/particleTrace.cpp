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
#include "particleTrace.h"

void pSystemTrace_t::Init(vec3_t &pos,vec3_t &dir)
{
   Allocate(200);

   spawn = pos;
   spawnTime = 2.0f;
   passed = 0.0f;
   
   MakeTrace();
}

bool pSystemTrace_t::Frame(float &frametime, vec3_t &grav)
{
   passed += frametime;

   if (spawnTime > 0 && passed > spawnTime)
   {
      passed = 0.0f;
    //  MakeTrace();
   }
   
   return Cycle(frametime, grav);
}

void pSystemTrace_t::Die(void)
{
   spawnTime = -1;
}

void pSystemTrace_t::MakeTrace(void)
{
   for (int i = 0; i < 10; i++)
   {
      pPart_t *n = PartNew();
      
      if (!n)
         return;
   
      n->life = ticker_t::PassedI() +
       (uint) (random_t::RandomRange(500.0f, 1000.0f));
      n->size = random_t::RandomRange(1.0f, 8.0f);
      n->vel=vec3_t(random_t::RandomRange(-150.0f, 150.0f), 
       random_t::RandomRange(-150.0f, 150.0f),
       random_t::RandomRange(-150.0f, 150.0f));
      n->pos=spawn; 
      n->pos[0] += random_t::RandomRange(-10.0f, 10.0f);
      n->pos[1] += random_t::RandomRange(-10.0f, 10.0f);
      n->pos[2] += random_t::RandomRange(-10.0f, 10.0f);
      n->pold=n->pos;
      n->gravity = 1;
      n->alpha = 1.0f;
   }
}
