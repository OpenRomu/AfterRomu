#include "stdafx.h"

#include "rand.h"
#include "ticker.h"
#include "Missile.h"

void pGrenSystemMissile_t::Init(vec3_t &pos,vec3_t &dir,DPNID amoi )
{
   Allocate(50);
    
   spawn = pos;
	dirspawn = dir*150.0f;

   spawnTime = 1.0f;
   passed = 0.0f;
   Spawn(amoi);
}

bool pGrenSystemMissile_t::Frame(float &frametime, vec3_t &grav,vector<vec3_t>* lespos,vector<DPNID>* amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* les_idjoueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars)
{
   passed += frametime;

   if (spawnTime > 0 && passed > spawnTime)
   {
      passed = 0.0f;
     //Spawn();
   }
   
 
   return Cycle(frametime, grav, lespos,amoi,lesposjoueur,mon_id,les_idjoueur,lesjoueurs,lescars);
}
 
void pGrenSystemMissile_t::Die(void)
{
   spawnTime = -1.0f;
}
// vertex arrays?
void pGrenSystemMissile_t::Render(vec3_t &x, vec3_t &y)
{
   vec3_t pts[4];
   vec3_t mx = -x;   
   vec3_t my = -y;
  glDepthMask(0);
 
 //   glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1.0f, -2.0f);
   glBindTexture(GL_TEXTURE_2D, texture);

vec3_t s;

   for (pGren_t *curr = pUse.next; curr != &pUse; curr = curr->next)
   {
      pts[0] = curr->pos + ((mx + my) * curr->size);
      pts[1] = curr->pos + ((x + my) * curr->size);
      pts[2] = curr->pos + ((x + y) * curr->size);
      pts[3] = curr->pos + ((mx + y) * curr->size);
         
      glColor4f(1.0f, 1.0f, 1.0f, curr->alpha );//
      glBegin(GL_TRIANGLE_FAN);
	
         glTexCoord2f(0.0f, 0.0f);
         glVertex3fv(pts[0]);
         glTexCoord2f(1.0f, 0.0f);
         glVertex3fv( pts[1]);
         glTexCoord2f(1.0f, 1.0f);
         glVertex3fv(pts[2]);
         glTexCoord2f(0.0f, 1.0f);
         glVertex3fv(pts[3]);
      glEnd();
   }
 glDepthMask(1);
     
  glDisable(GL_POLYGON_OFFSET_FILL);
 
   
}

void pGrenSystemMissile_t::Spawn(DPNID a_moi)
{
   for (int i = 0; i < 1; i++)
   {
      pGren_t *n = GrenNew();

      if (!n)
         return;

		n->life = ticker_t::PassedI() +
		(uint) (random_t::RandomRange(3000.0f, 3500.0f));
		n->size = 10.0f;
		n->vel=dirspawn;//
		n->pos=spawn; 
		n->pold=spawn;
		n->gravity = true;
		n->alpha = 1.0f;
		n->rebond=false;
		n->colle=false;
		n->id_lanceur=a_moi;


   }
}