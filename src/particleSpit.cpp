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
#include "particleSpit.h"

void pSystemSpit_t::Init(vec3_t &pos)
{
   Allocate(200);
   
   spawnTime = 0.1f;
   rotPerSec = 60.0f;

   if (!(head = PartNew()))
      return;
   
   head->life = 0; // always alive
   head->size = 15.0f;
   head->vel.Set(0.0f, 0.0f, 0.0f);
   head->pos.Set(pos);
   head->pold.Set(0.0f, 0.0f, 0.0f);
   head->gravity = 0;
   head->alpha = 1.0f;

   rot = 0.0f;
   passed = 0.0f;
}

bool pSystemSpit_t::Frame(float &frametime, vec3_t &grav)
{
   passed += frametime;
   rot += (frametime * rotPerSec);

   if (rot > 360.0f)
      rot = rot - 360.0f;

   if (passed > spawnTime && head)
   {
      vec3_t dir(1.0f, 0.0f, 0.0f); // static?
      dir.RotateY(rot, dir);

      passed = 0.0f;
      
      for (int i = 0; i < 10; i++)
      {
         pPart_t *n = PartNew();
      
         if (n)
         {
            n->life = ticker_t::PassedI() +
             (uint) (random_t::RandomRange(1000.0f, 3000.0f));
            n->size = 14;
            
            n->vel[0] = dir[0] * random_t::RandomRange(40.0f, 70.0f);
            n->vel[1] = random_t::RandomRange(50.0f, 90.0f);
            n->vel[2] = dir[2] * random_t::RandomRange(40.0f, 70.0f);

            n->pos.Set(head->pos);
            n->pos[0] += random_t::RandomRange(-5.0f, 5.0f);
            n->pos[2] += random_t::RandomRange(-5.0f, 5.0f);
            n->pold.Set(head->pos);
            n->gravity = 1;
            n->alpha = 1.0f;
         }
      }
   }

   if (head && head->life != 0 && head->life < ticker_t::PassedI())
      head = 0;
   
   return Cycle(frametime, grav);
}

void pSystemSpit_t::Die(void)
{
   if (head)
      head->life = ticker_t::PassedI();
}
