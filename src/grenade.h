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
#ifndef __GRENADEH__
#define __GRENADEH__

#include "geom.h"
#include "utility.h"
#include "particleMan.h"
#include <dplay8.h>
#include <vector>
#include "player.h"
using namespace std;
struct pGren_t
{
   pGren_t();

   uint life;
   float size;
   vec3_t pos;
   vec3_t pold;
   vec3_t vel;
   //vec3_t color; // could use, but i don't
   float alpha;
   bool gravity;
   bool rebond; // grenade missile 
   bool colle; // grenade missile 
	 vec3_t v[4];

   DPNID id_lanceur;
   pGren_t *next, *prev;
};

static const int pGrenMaxPart = 50;

struct pGrenSystem_t
{
   pGrenSystem_t();
   virtual ~pGrenSystem_t();

   pGrenSystem_t *GetNext(void)   {return next;}
   pGrenSystem_t *GetPrev(void)   {return prev;}
   pGren_t   *GetAlive(void)  {return &pUse;}
   int GetType(void)          {return type;}
   void SetNext(pGrenSystem_t *n) {next = n;}
   void SetPrev(pGrenSystem_t *p) {prev = p;}   
   void SetEXPLODE(pParticleManager_t *p) {engineparts = p;}   
   void SetTexture(uint id)   {texture = id;}
	void ReflectVectorx(vec3_t &vel, vec3_t &n);
   void Allocate(int num);
   pGren_t *GrenNew(void);
   void GrenDelete(pGren_t *d);
   bool Cycle(float &frametime, vec3_t &grav,vector<vec3_t>* lespos,vector<DPNID> * amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* lesid_joueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars); 
  void drawBox (vec3_t *v);
   virtual void Init(vec3_t &pos,vec3_t &dir,DPNID id_lanceur) = 0;
   virtual bool Frame(float &frametime, vec3_t &grav,vector<vec3_t >* lespos,vector<DPNID>* amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* lesid_joueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars) = 0;
   virtual void Render(vec3_t &x, vec3_t &y);
   virtual void Collide(pGren_t *part, vec3_t &n);
   virtual void Die(void) = 0;   
   
 
 int type;     // !!important, must be set in derived classes constructor
   
protected:
   uint texture; // texture id 
    pParticleManager_t * engineparts;
   pGren_t *pAll;  // single link
   pGren_t pUse;   // double link
   pGren_t *pFree; // single link
   pGrenSystem_t *next, *prev;
};

#endif