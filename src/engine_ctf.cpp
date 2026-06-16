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

void Engine::CFT_affiche_message(int r, char *mess)
{
    lockequipe(); //----------------------------------------------LOCK

    if (m_chat)
    {
        char team[40];
        char model[100];
        char tmp[100];

        model2str(lejoueur[r]->id_modele, model);
        CFT_renvoie_lib_team(lejoueur[r]->QuelTeam, team);
        sprintf(tmp, "%s %s", team, lejoueur[r]->playername);
        m_chat->addtext(tmp, 2);
        m_chat->addtext(mess, 2);
    }
    unlockequipe(); //----------------------------------------------LOCK
}


HRESULT Engine::CFT_recoit_message(DPNID idplayer, GAMEMSG_CFT *ret)
{

    if (ret->type_du_message == 2) // c une demande d'un joueur
    {
        if (m_bHostPlayer)
        {
            CFT_HOST_envoi_recapitulatif();
            //** CFT on envoie a tout le monde a voir si pas gourmand
        }
    }
    else
    {
        if (CFT_ON)
        {
            lockequipe(); //----------------------------------------------LOCK

            for (int j = 0; j < g_lNumberOfActivePlayers; j++)
            {
                if ((lejoueur[j]->etat == true) && (lejoueur[j]->ID == idplayer))
                {

                    if (ret->type_du_message == 0)
                    { // c la capture
                        if (lejoueur[j]->QuelTeam == 0)
                        { // c le gign il a attrape flagTR
                            FlagTR.ID = idplayer;
                            FlagTR.eta_attrape();
                            CFT_affiche_message(j, TEXT("a capture le flag des TERRO"));
                        }
                        else
                        {
                            FlagCS.ID = idplayer;
                            FlagCS.eta_attrape();
                            CFT_affiche_message(j, TEXT("a capture le flag des GIGN"));
                        }
                    }
                    else
                    {

                        // type message =1
                        int nb_quidam;
                        nb_quidam = 0;
                        int team_gagnant;
                        team_gagnant = lejoueur[j]->QuelTeam;

                        for (int cpt = 0; cpt < lejoueur.size(); cpt++)
                        {
                            if (lejoueur[cpt]->QuelTeam != team_gagnant)
                                nb_quidam++;
                        }

                        // ajout des pts en fonction noimbre danbsequipe advers 05/05/2008
                        if (lejoueur[j]->QuelTeam == 0)
                        { // c le gign il a attrape flagTR
                            CFT_affiche_message(j, TEXT("MARQUE pour les GIGN"));
                            FlagCS.eta_gagne();
                            CFT_nb_gign = CFT_nb_gign + nb_quidam;
                        }
                        else
                        {
                            CFT_affiche_message(j, TEXT("MARQUE pour les TERRO"));
                            FlagCS.eta_gagne();
                            CFT_nb_terro = CFT_nb_terro + nb_quidam;
                        }
                        CFT_eta = 0;
                        CFT_nouvelle_partie();
                    }
                }
            }
            unlockequipe(); //----------------------------------------------LOCK
        }
        else
        {
            if (TEAM_ON)
            {

                if (ret->type_du_message == 4)
                {
                    m_chat->addtext("GIGN gagne la partie", 2);
                    // on a recu du serveur la team qui gagne
                    JoueUnSon(36, lejoueur[VRAI]->pos);
                    CFT_nb_gign++;
                    CFT_eta = 0;
                    CFT_count = 0;
                    CFT_old_count = GetTickCount();
                    m_overlay = true;
                }
                if (ret->type_du_message == 3)
                {
                    m_chat->addtext("TERRO gagne la partie", 2);
                    JoueUnSon(36, lejoueur[VRAI]->pos);
                    CFT_nb_terro++;
                    // on a recu du serveur la team qui gagne
                    CFT_eta = 0;
                    CFT_count = 0;
                    CFT_old_count = GetTickCount();
                    m_overlay = true;
                }
                //			lejoueur[VRAI]->mort=true;
                init_player(VRAI);
                lejoueur[VRAI]->velocity = vec3_t(0.0f, 0.0f, 0.0f);
            }
        }
    }
    return S_OK;
}

HRESULT Engine::CFT_envoi_message(int quoi)
{
    // quoi
    // 0- capture
    // 1- nvl partie
    // 2- recap
    // 3- gign
    // 4- terro gagne
    // envoyer qd un nouveau joueur est cree ou voir ailleurs
    // permet de renovyer a tout le monde y compris
    // au nbouveau l'etat de la partie

    if (g_lNumberOfActivePlayers > 0)
    {
        // Send a message to all of the players
        GAMEMSG_CFT msgWave;
        msgWave.dwType = GAME_MSGID_CFT;

        msgWave.type_du_message = quoi;

        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_CFT);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;

        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED);
    }

    return S_OK;
}