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
#include "particleSimple.h"

void pSystemSimple_t::Init(vec3_t &pos,vec3_t &dir)
{
   Allocate(50);
    
   spawn = pos;
   spawnTime = 1.0f;
   passed = 0.0f;
   Spawn();
}

bool pSystemSimple_t::Frame(float &frametime, vec3_t &grav)
{
   passed += frametime;

   if (spawnTime > 0 && passed > spawnTime)
   {
      passed = 0.0f;
     //Spawn();
   }
   
 
   return Cycle(frametime, grav);
}

void pSystemSimple_t::Die(void)
{
   spawnTime = -1.0f;
}

void pSystemSimple_t::Spawn(void)
{
   for (int i = 0; i < 4; i++)
   {
	   vec3_t sp=spawn;
	   sp[2]=sp[2]+random_t::RandomRange(-20.0f, 20.0f);

      vec3_t dir=vec3_t(random_t::RandomRange(-30.0f, 30.0f),random_t::RandomRange(-30.0f, 30.0f),random_t::RandomRange(-5.0f, 5.0f));
	  dir=dir*5.0f;
	  //uint life =
	  for (float u=0;u<4;u++)
	  {
		  pPart_t *n = PartNew();

		  if (!n)
			 return;
		  n->life = ticker_t::PassedI() +
       (uint) (random_t::RandomRange(500.0f, 1500.0f));;
		  n->size = random_t::RandomRange(1.0f, 4.0f);
		  n->vel=dir*(1.0f+(u)/10.0f);//
		  n->pos=sp; 
		  


		  n->pold=sp;
		  n->gravity = 1;
		  n->alpha = 1.0f;
	  }
   }
}