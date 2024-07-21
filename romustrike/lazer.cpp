#include "stdafx.h"
#include "rand.h"
#include "ticker.h"
#include "Lazer.h"

void pGrenSystemLazer_t::Init(vec3_t &pos,vec3_t &dir,DPNID amoi )
{
   Allocate(50);
    
   spawn = pos;
	dirspawn = dir;

   spawnTime = 1.0f;
   passed = 0.0f;
   Spawn(amoi);
}

bool pGrenSystemLazer_t::Frame(float &frametime, vec3_t &grav,vector<vec3_t>* lespos,vector<DPNID>* amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* les_idjoueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars)
{
   passed += frametime;

   if (spawnTime > 0 && passed > spawnTime)
   {
      passed = 0.0f;
     //Spawn();
   }
   
 
   return Cycle(frametime, grav, lespos,amoi,lesposjoueur,mon_id,les_idjoueur,lesjoueurs,lescars);
}

 
void pGrenSystemLazer_t::Die(void)
{
   spawnTime = -1.0f;
}
// vertex arrays?
void pGrenSystemLazer_t::Render(vec3_t &x, vec3_t &y)
{
   vec3_t pts[4];
   vec3_t mx = -x;   
   vec3_t my = -y;

	
	vec3_t s;

uint tps=ticker_t::PassedI();
uint deb=95000;

   for (pGren_t *curr = pUse.next; curr != &pUse; curr = curr->next)
   {
 
	   	   glEnable(GL_BLEND);

	   	glEnable (GL_TEXTURE_2D);
	  glDepthMask(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1.0f, -2.0f);
	   
		  glColor4f(1.0f, 1.0f, 1.0f, 1.0);

 
glBindTexture(GL_TEXTURE_2D, texture);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//glEnable(GL_ALPHA_TEST); 
glBlendFunc(GL_ONE,GL_SRC_ALPHA);
//glAlphaFunc(GL_GREATER,0);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(curr->pos+curr->v[0]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(curr->pos+curr->v[1]);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(curr->pos+curr->v[2]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(curr->pos+curr->v[3]);
		glEnd();
 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	  glDepthMask(1);
 
 		glDisable (GL_TEXTURE_2D);
		glDisable (GL_CULL_FACE);
		if (curr->life-deb < tps)
		{
			//glDisable (GL_DEPTH_TEST);
			glColor4f (0, 0, 1, 0.5f);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);


			vec3_t a=((curr->v[0]+curr->v[1])/2.0f)/10.0f;
			vec3_t b=((curr->v[2]+curr->v[3])/2.0f)/10.0f;
			vec3_t c=((curr->v[1]+curr->v[2])/2.0f)/10.0f;
			vec3_t d=((curr->v[3]+curr->v[0])/2.0f)/10.0f;


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
		}

	glEnable (GL_TEXTURE_2D);
		glEnable (GL_CULL_FACE);
		glEnable (GL_DEPTH_TEST);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  
   }
  
}
#define MyFab(a) (a > 0 ? a : -a)
static vec3_t axis[3] = 
{
   vec3_t(1.0f, 0.0f, 0.0f),
   vec3_t(0.0f, 1.0f, 0.0f),
   vec3_t(0.0f, 0.0f, 1.0f)
};

void pGrenSystemLazer_t::Spawn(DPNID a_moi)
{

vec3_t no=dirspawn;
no.normalize();
no=no*-1.0f;
   vec3_t fn(MyFab(no[0]), MyFab(no[1]), MyFab(no[2]));

   vec3_t up, right;
   int major = 0;
   right[1]=0.0f; //debug vs2005
right[2]=1.0f;
right[0]=0.0f;
   // find the major axis
   if (fn[1] > fn[major])
      major = 1;
   if (fn[2] > fn[major])
      major = 2;

   // build right vector by hand
   if (fn[0] == 1 || fn[1] == 1 || fn[2] == 1)
   {
      if ((major == 0 && no[0] > 0) || major == 1)
	  {//right.Set(0, 0, -1);
		  right[1]=0;
		  right[2]=1;
		  right[0]=0;
	  }

      else if (major == 0)
	  {  //right.Set(0, 0, 1);
		   right[1]=0;
		  right[2]=-1;
		  right[0]=0;
	  }
      else 
	  {
         //right.Set(1 * n[2], 0, 0);
		   right[1]=1*no[2];
		  right[2]=0;
		  right[0]=0;
	  }
   }
   else
   {
      //right.cross(axis[major], n);
	right=axis[major].cross(no);
	
   }

   up=no.cross(right);
   
   up.normalize();
 //  up=up*-1.00f;
	right.normalize();
//size=-size;

   // make the face
	float size=8.0f;
	float sizeh=8.0f;

	 

 

   for (int i = 0; i < 1; i++)
   {
      pGren_t *n = GrenNew();

      if (!n)
         return;

		n->life = ticker_t::PassedI() +
		(uint) (random_t::RandomRange(100000.0f, 100000.0f));
		n->size = 10.0f;
		n->vel=dirspawn;//
		n->pos=spawn-no; 
		n->pold=spawn;
		n->gravity = false;
		n->alpha = 1.0f;
		n->rebond=false;
		n->id_lanceur=a_moi;
		n->colle=true;
	n->v[0] = ((-right - up) * size);
	n->v[1] = ((right - up) * sizeh);
	n->v[2] =  ((right + up) * sizeh);
	n->v[3] =  ((-right + up) * size);
	

   }
}