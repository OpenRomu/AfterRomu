// player.h: interface for the player class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_)
//#define AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
#ifndef __player_h__
#define __player_h__


//#include "ressources.h"
#include "Model_MDL.h"
#include "geom.h"
#include "dplay8.h"
//#include "dsound.h"       
#include "son.h"

#include "particleMan.h"
#include "ticker.h"
#include "texman.h"
#include "exception.h" 
#include <fstream>
#include "PhysEnv.h"
#include "glwin.h"
#include "rand.h" //**MULTI MORT 06/01/2005
/*#include <string>
#include <stdio.h> 
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
*/
	/*Model_MDL leshommes[10];
	Model_MDL lesarmes[10];
	CSound* lessons[10];*/

class Aplayer  
{
public:
	Aplayer();
	virtual ~Aplayer();
	DPNID ID;
	char  playername[50];
	int QuelTeam ;
	vec3_t pos;
	vec3_t rot;
	vec3_t velocity;
	bool team;
 	CPhysEnv*		voiture;
	bool is_car ;
	Model_MDL modele;
	Model_MDL arme;
	Model_MDL vue;
	CSound* playersound;
	CSound* reload;
	CSound* marche;
	CSound* meurt;
	CSound* voice;
	
	bool Visible;
	texMan_t * m_tex1;
	texMan_t * m_tex2;
	texMan_t * m_tex3;
	texMan_t * m_tex4;
	

	pParticleManager_t* parts  ;

	DS3DBUFFER              g_dsBufferParams;               // 3D buffer properties
	LPDIRECTSOUND3DBUFFER   g_pDS3DBuffer         ;   // 3D sound buffer
	GLuint id_texture_jeep;

	int id_modele_recu;
	int id_arme_recu;
	int score;
//	bool tue;
	bool mort;
	int id_modele;
    int id_weapon;
	int calcul_rendu;
//	float ang_dos;
	int colonne;
	bool etat;
	int cartouche_12;
	frameTimer_t frameTime;
   
	void init(CSoundManager*          g_pSoundManager);
	void init_particle(world_t* world);
	void affecte_modele(Model_MDL *leshommes,BYTE id_mod,int max);
	void affecte_arme(Model_MDL *lesarmes,BYTE id_arm,int max);
 	void affecte_sound(CSound* lessons);
	void reload_arme();
	void affecte_effet(CSound* lessons,int lequel);
	void chg_phys(void);
	void SpawnBlood();
	void SpawnSmoke();
	void SpawnTorch();
	
	void Last_pos(void);
	float GetDelta(void);
//	void SetTexture(GLuint texid){m_PhysEnv.SetTexture(texid);}
	void SetTraction (bool av,bool ar,  bool dr,bool ga,bool fr);
	 
	void anim();
	void play_sound(void);
	void Render_particle(float delta);
	void Voix(CSound* lessons);


	char str_act[20];
	char str_dep[20];
	char action[20];
	char action2[20];

	char dep[20];
	//** MULTI MORT ajouté le 06/01/2005
	const char* RenvoieSeqMort();
	char dep_mort[20];
	bool sequence_mort_deja_affecte;
	//**
	float hauteur;
	float sens;
	float Offset_crouch;
	float fps;
	bool local;
	bool saut;
	float pulse;
	bool can_jump;

	bool tir;
	bool tir_grenade;
	bool fire;
	bool fireg;
	vec3_t yeux;
	vec3_t flash;
	int ammo;
	int grenade;
	int smoke_grenade;
	int nb_lazer;
	int missile;
	int vie;
	bool occupe;
	int fps1_recu;
	int id_seq1_recu;
	int fps2_recu;
	int id_seq2_recu;
	int killed;
	unsigned long  FrameTime;
	bool deja_en_cours;
	long cle_joueur;
};

#endif // !defined(AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_)
