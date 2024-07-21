// player.cpp: implementation of the player class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flag.h"
//#include "xmlmessaging.h"
//#include "son.h"

//#include "particleSimple.h"
//#include "particleBurst.h"
//#include "particleBoom.h"
//#include "particleSmoke.h"

//#include "son.h"
//#include <winnt.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


AFlag::AFlag():ID(NULL) ,son_init(0),son_attrape(0),son_gagne(0)
{
	Team=0;
	num_seq=0;
	rot=0;
	dt=1;
	pos_cur[0]=0.0f;
	pos_cur[1]=0.0f;
	pos_cur[2]=0.0f;                    

	pos_ini[0]=0.0f;
	pos_ini[1]=0.0f;
	pos_ini[2]=0.0f;                    

         

}

AFlag::~AFlag()
{

}


void 
AFlag::anim()
{

	
	modele.SetSequence(0);
	modele.AdvanceFrame( dt,0);
	modele.m_origin[0] =pos_cur[0];
	modele.m_origin[1] =pos_cur[1];
	modele.m_origin[2] =pos_cur[2];


	//rot += dt * 30*(__PI/180);//pseqdesc->fps;
	//rot=rot+1*__PI*dt*30;
	rot+=dt*60;
	if (rot>=360)
		rot=0;
	modele.m_angles[0] =0.0f;
	modele.m_angles[1] =rot;
	modele.m_angles[2] =-90;
	modele.DrawIni();

}
void 
AFlag::animMDL()
{

	
	modele.SetSequence(num_seq);
	modele.seq1 =num_seq;
	modele.AdvanceFrame( dt,0);
	modele.m_origin[0] =pos_cur[0];
	modele.m_origin[1] =pos_cur[1];
	modele.m_origin[2] =pos_cur[2]-40;

	/*if ((type==0)||((type==1)&&(ID==NULL)))
	{
		modele.m_origin[2]=modele.m_origin[2]-40;
	}*/
	rot+=dt*60;
	if (rot>=360)
		rot=0;
	modele.m_angles[0] =0.0f;
	modele.m_angles[1] =rot;
	modele.m_angles[2] =0.0f;
	modele.DrawIni();

}
void AFlag::affecte_son_init(CSound* lessons)
{
	
	son_init=(CSound*) lessons;
	son_init->Init();
}
void AFlag::affecte_son_attrape(CSound* lessons)
{
	
	son_attrape=(CSound*) lessons;
	son_attrape->Init();
}
void AFlag::affecte_son_gagne(CSound* lessons)
{
	
	son_gagne=(CSound*) lessons;
	son_gagne->Init();
}




void 
AFlag::affecte_modele(Model_MDL *lesobjets,BYTE id_mod,int max)
{
	
	if (id_mod>=max){id_mod=0;}
	modele=lesobjets[id_mod];
	modele.lie=0;
	modele.fps2=0;
	modele.fps1=0;

	modele.SetSequence(0);
	modele.m_frame =0.0f;

}
 
void 
AFlag::eta_depart()
{
	ID=NULL;
	num_seq=1;
	pos_cur=pos_ini;
	if (son_init)
	{
	son_init->Play( 0, 0);
	}
}
void 
AFlag::eta_attrape()
{
	//ID=NULL;
	//pos_cur=pos_ini;
	num_seq=2;
	if (son_attrape)
	{
	son_attrape->Play( 0, 0);
	}

}
void 
AFlag::eta_gagne()
{
	//ID=NULL;
	//pos_cur=pos_ini;
if(son_gagne)
	son_gagne->Play( 0, 0);

}
bool 
AFlag::collision_Flg(DPNID le_id,vec3_t src,int la_team)
{
	
	if ((ID!=NULL)|| (Team==la_team))
		return false;
	
	vec3_t dir;
	float dirlen=dir.len();
//	float ledot;
	vec3_t dirn=dir;
	dirn.normalize ();
	vec3_t retv=src+dir;
	
	vec3_t moi_joueur=(pos_cur-(src+dir));

	if  (moi_joueur.len()<30.0f )
	{
		ID=le_id;
		eta_attrape();
		return true;
	}
	return false;
}
bool 
AFlag::collision_Cmp(DPNID le_id,DPNID le_id_joueur,vec3_t src,int la_team)
{
	
	if ((le_id==le_id_joueur) &&  (Team==la_team))
	{	
		vec3_t dir;
		float dirlen=dir.len();
	//	float ledot;
		vec3_t dirn=dir;
		dirn.normalize ();
		vec3_t retv=src+dir;
		
		vec3_t moi_joueur=(pos_ini-(src+dir));

		if  (moi_joueur.len()<30.0f )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}