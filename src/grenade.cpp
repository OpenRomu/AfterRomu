#include "stdafx.h"
#include "glinc.h"
#include "ticker.h"
//#include "util3d.h"
#include "grenade.h"
#include "particleTorch.h"
#include "particleSmoke.h"
#include "particleBurst.h"
#include "particleBoom.h"
#include "particleheSmoke.h"
// pPart_t 
pGren_t::pGren_t()
{
   next = prev = 0;
   life = 0;
   size = 0;
   gravity = true;
   alpha = 1.0f;
   rebond=false;
   colle=false;

}

// pSystem_t
pGrenSystem_t::pGrenSystem_t()
{
   next = prev = 0;
   pAll = pFree = 0;
   pUse.next = &pUse;
   pUse.prev = &pUse;
}

pGrenSystem_t::~pGrenSystem_t()
{
   delete [] pAll;
}

void pGrenSystem_t::Allocate(int num)
{
   pAll = new pGren_t[num];   

   pUse.next = &pUse;
   pUse.prev = &pUse;

   pFree = pAll;

   for (int i = 0; i < num - 1; i++)
      pAll[i].next = &pAll[i + 1];
}

pGren_t *pGrenSystem_t::GrenNew(void)
{
   if (!pFree)
      return 0;
   
   pGren_t *n = pFree;
   
   pFree = pFree->next;
   
   n->prev = &pUse;
   n->next = pUse.next;
   pUse.next->prev = n;
   pUse.next = n;

   return n;
}

void pGrenSystem_t::GrenDelete(pGren_t *d)
{
   d->prev->next = d->next;
   d->next->prev = d->prev;
   
   d->next = pFree;
   pFree = d;
}

bool pGrenSystem_t::Cycle(float &frametime, vec3_t &grav,vector<vec3_t>* lespos,vector<DPNID>* amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* lesid_joueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars)
{
   pGren_t *curr = pUse.next;
   pGren_t *next = 0;

   if (pUse.next == &pUse)
      return 0;
	uint tps=ticker_t::PassedI();

   while (curr != &pUse)
   {
      next = curr->next;
      curr->pold=curr->pos;
	  if(!curr->colle)
		curr->pos += (curr->vel * 2 * frametime); // hack hack hack
      if (curr->gravity && !curr->colle)
	  {
			if (curr->rebond)
				curr->vel += (grav * 5.0f * frametime);
			else
			{
				if (curr->vel.len()<1000.0f)
					curr->vel+=curr->vel*4.0f*frametime;
				curr->vel += (grav * 1.0f * frametime);
		
			}
	  }
      if (curr->life > 0 && (curr->life < tps))
	  {
			curr->pos=curr->pold;
			engineparts->SystemNew(new   pSystemTorch_t , curr->pos   , 1);
			engineparts->SystemNew(new   pSystemHeSmoke_t , curr->pos   , 1);
			if (type!=2)
			{
				lespos->push_back (curr->pos);
				amoi->push_back (curr->id_lanceur);
			}
			GrenDelete(curr);
	  }
	  else
	  {
		  if (!curr->colle)
		  {
			  int dd=curr->life-tps;

			  if ((dd)% 30<5)
			  {
				  engineparts->SystemNew(new   pSystemSmoke_t , curr->pos   , 1);
				  if (type ==2)
				  {
					  
					if (dd<50000 && dd%60<5)
						engineparts->SystemNew(new   pSystemHeSmoke_t   , curr->pos   , 1);
				  }
			  }

			  for(int ex=0;ex<lescars.size();ex++ )
				{
					CPhysEnv * car=lescars.at(ex);
					if (curr->life-95000 < tps)
					{
						vec3_t a=curr->pos-(car->AxeG+vec3_t(0.0f,0.0f,-30.0f));
						if (a.len()<50.0f)
						{

										curr->life=1.0f;
							
						}
					}
				}

		  }
		  else
		  {
				vec3_t normal = curr->vel;
				normal.normalize ();

				for(int e=0;e<lesjoueurs->size();e++ )
				{
					Aplayer * play=lesjoueurs->at(e);
					if (curr->life-95000 < tps)
					{
						vec3_t a=curr->pos-play->pos;
						if (a.len()<curr->vel.len())
						{

							float t=play->modele.intersect_sphere(curr->pos,normal,play->pos,30);
							if (t>0  )
							{
								int last_id=play->modele.calculeboite(curr->pos,normal);
								if (last_id!=-1)
								{
								
										curr->life=1.0f;
								}
							
							}
						}
					}
				}

				


		  }
	  }

      curr = next;
   }
   return 1;
}

// vertex arrays?
void pGrenSystem_t::Render(vec3_t &x, vec3_t &y)
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

// reflect the velocity vector
void pGrenSystem_t::Collide(pGren_t *part, vec3_t &n)
{
   ReflectVectorx(part->vel, n);
   part->vel=part->vel*0.4f;
   
   part->pos=part->pold;
}

void pGrenSystem_t::ReflectVectorx(vec3_t &vel, vec3_t &n)
{
   vec3_t vn = n * (n.dot(vel));
   vec3_t vt = vel - vn;
   vel = vt - vn;
}

 void pGrenSystem_t::drawBox (vec3_t *v)
{
	
		glDisable (GL_TEXTURE_2D);
		glDisable (GL_CULL_FACE);
		glDisable (GL_DEPTH_TEST);
		glColor4f (1, 0, 0, 1);
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

	
	
	
	glBegin (GL_QUAD_STRIP);
	for (int i = 0; i < 10; i++)
		glVertex3fv (v[i & 7]);
	glEnd ();
	
	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[6]);
	glVertex3fv (v[0]);
	glVertex3fv (v[4]);
	glVertex3fv (v[2]);
	glEnd ();

	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[1]);
	glVertex3fv (v[7]);
	glVertex3fv (v[3]);
	glVertex3fv (v[5]);
	glEnd ();

		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_CULL_FACE);
		glEnable (GL_DEPTH_TEST);

			glColor4f(1.0f,1.0f,1.0f,1.0f); // BART fix

}
 