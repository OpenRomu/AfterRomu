#ifndef __GRENADELazerH__
#define __GRENADELazerH__

#include "GrenadeMan.h"

// @PORT {
#include <dplay8.h>
// }

struct pGrenSystemLazer_t : public pGrenSystem_t
{
   pGrenSystemLazer_t() {type = pgTypeLazer; spawnTime = 0;}

   void Init(vec3_t &pos,vec3_t &dir,DPNID amoi );
   bool Frame(float &frametime, vec3_t &grav,vector<vec3_t>* lespos,vector<DPNID> * amoi,vector<vec3_t>* lesposjoueur,DPNID mon_id,vector<DPNID>* lesid_joueur,vector<Aplayer*>* lesjoueurs,vector<CPhysEnv* > lescars);
   void Die(void);
	void Spawn(DPNID a_moi);
void Render(vec3_t &x, vec3_t &y);
   
protected:
   
   float spawnTime;
   float passed;
   vec3_t spawn;
	vec3_t dirspawn;


};

#endif