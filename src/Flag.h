// player.h: interface for the player class.
//
//////////////////////////////////////////////////////////////////////

// #if !defined(AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_)
// #define AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000
#ifndef __flag_h__
#define __flag_h__

// #include "ressources.h"
#include "Model_MDL.h"
#include "geom.h"
#include "dplay8.h"

#include "son.h"

// #include "particleMan.h"
#include "ticker.h"
#include "texman.h"
#include "exception.h"
#include <fstream>
// #include "son.h"
// #include "PhysEnv.h"
#include "glwin.h"
#include "fastmath.h"

/*#include <string>
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
*/
/*Model_MDL leshommes[10];
Model_MDL lesarmes[10];
CSound* lessons[10];*/

class AFlag {
public:
    AFlag();
    virtual ~AFlag();
    DPNID ID;
    vec3_t pos_cur;
    vec3_t pos_ini;
    int Team;
    int num_seq;
    int type; // 0= base ; 1 =flag
    double dt;
    double rot;
    Model_MDL modele;
    CSound* son_init;
    CSound* son_attrape;
    CSound* son_gagne;

    void affecte_modele(Model_MDL* lesobjets, BYTE id_mod, int max);

    void affecte_son_init(CSound* lessons);
    void affecte_son_attrape(CSound* lessons);
    void affecte_son_gagne(CSound* lessons);
    bool collision_Flg(DPNID le_id, vec3_t src, int la_team);
    bool collision_Cmp(DPNID le_id, DPNID le_id_joueur, vec3_t src, int la_team);

    void eta_depart();
    void eta_attrape();
    void eta_gagne();

    void anim();
    void animMDL();
};

#endif // !defined(AFX_PLAYER_H__0AC17FEA_73B0_11D6_8858_0000E8EEE44A__INCLUDED_)
