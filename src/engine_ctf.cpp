#include <windows.h>
#include "stdafx.h"

#include "common.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "engine.h"
#include "romu.h"
#include "texture.h"
#include "wad.h"
#include "bsp.h"
#include "Shellapi.h"

#include "exception.h"
#include "image.h"
#include "bitset.h"
#include "Model_MDL.h"
#include "glext.h"
#include "dxutil.h"
#include <dxerr8.h>
#include "matrix.h"
#include <math.h>
#include <mmsystem.h>
#include "rand.h"
#include <stdio.h>
#include <string>
#include <cmath>

/////////////////////////////sound bat///////////////////////////////////
#include "resource.h"
#include "dsound.h"
#include "dsutil.h"
/////////////////////////////sound bat///////////////////////////////////
#include "vectormath.h"
#include "particleSimple.h"
#include "particleBurst.h"
#include "particleBoom.h"
#include "particleSmoke.h"
#include "particleTorch.h"
#include "grenadeSimple.h"
#include "lazer.h"
#include "grenadesmoke.h"
#include "missile.h"
#include "ticker.h"
#include "texman.h"
#include "bouton.h"
#include "console.h"
#include "option_bouton.h"
#include "bubsock.h"
#include "MD5.h"

using namespace std;

HRESULT Engine::CFT_HOST_envoi_recapitulatif()
{

    if ((g_lNumberOfActivePlayers > 0) && m_bHostPlayer)
    {
        // Send a message to all of the players
        GAMEMSG_CFT_TOTALE msgWave;
        msgWave.dwType = GAME_MSGID_CFT_TOTALE;

        msgWave.Id_Flag_Cs = FlagCS.ID;
        msgWave.Id_Flag_Tr = FlagTR.ID;

        msgWave.Nb_Cs = CFT_nb_gign;
        msgWave.Nb_Tr = CFT_nb_terro;

        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_CFT_TOTALE);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;

        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED);
    }

    return S_OK;
}

HRESULT Engine::CFT_HOST_recoit_recapitulatif(DPNID idplayer, GAMEMSG_CFT_TOTALE *ret)
{
    lockequipe(); //----------------------------------------------LOCK

    if (FlagTR.ID != ret->Id_Flag_Tr)
    { // c la capture
        FlagTR.ID = ret->Id_Flag_Tr;
        FlagTR.eta_attrape();
        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if ((lejoueur[j]->etat == true) && (lejoueur[j]->ID == FlagTR.ID))
            {
                CFT_affiche_message(j, TEXT("a capture le flag des TERRO"));
                break;
            }
        }
    }
    if (FlagCS.ID != ret->Id_Flag_Cs)
    { // c la capture
        FlagCS.ID = ret->Id_Flag_Cs;
        FlagCS.eta_attrape();
        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if ((lejoueur[j]->etat == true) && (lejoueur[j]->ID == FlagCS.ID))
            {
                CFT_affiche_message(j, TEXT("a capture le flag des COUNTER"));
                break;
            }
        }
    }

    unlockequipe(); //----------------------------------------------LOCK

    CFT_nb_gign = ret->Nb_Cs;
    CFT_nb_terro = ret->Nb_Tr;

    return S_OK;
}