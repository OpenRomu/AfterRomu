// player.cpp: implementation of the player class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "player.h"
#include "xmlmessaging.h"
#include "particleSimple.h"
#include "particleBurst.h"
#include "particleBoom.h"
#include "particleSmoke.h"
#include "particleTorch.h"

#include "rand.h"
// #include "son.h"
// #include <winnt.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Aplayer::Aplayer()
    : cle_joueur(0), score(0), m_tex1(NULL), mort(false), m_tex2(NULL), m_tex3(NULL), m_tex4(NULL), parts(0),
      playersound(0), saut(false), can_jump(true), tir(false), tir_grenade(false), ammo(0), grenade(0),
      smoke_grenade(0), missile(2), Visible(true), killed(0), hauteur(0.0f), deja_en_cours(false), fire(false),
      fireg(false), is_car(false), id_texture_jeep(0), id_arme_recu(0), id_modele_recu(0), id_modele(0), id_weapon(0),
      nb_lazer(0), occupe(false)
{ // FrameTime(0),
    team = false;
    velocity = vec3_t(0.0f, 0.0f, 0.0f);

    colonne = 0;
    rot[0] = 0.0f;
    rot[1] = 0.0f;
    rot[2] = 0.0f;
    sens = 1;
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
    etat = false;
    occupe = false;
    ammo = 0;
    yeux[0] = 22;
    yeux[1] = -68;
    yeux[2] = 42;
    strcpy(playername, "");
    strcpy(dep_mort, "");
    strcpy(str_act, TEXT("reload"));
    strcpy(str_dep, TEXT("jump"));

    flash[0] = 0.0f;
    flash[1] = 285.0f;
    flash[2] = 0.0f;
    vie = 100;
    grenade = 5;
    smoke_grenade = 5;
    nb_lazer = 5;

    id_seq1_recu = 0;
    id_seq2_recu = 0;
    strcpy(action2, "idle1");
    strcpy(action, "idle1");
    strcpy(dep, "head");
    calcul_rendu = 0;
    Offset_crouch = 12.0f;
    pulse = 0.0f;
    fps = 1.0f;
    local = false;
    fps1_recu = 1;
    fps2_recu = 1;
    id_seq1_recu = 1;
    id_seq2_recu = 1;
    sequence_mort_deja_affecte = false; //** MULTI MORT 06/01/2005
    // ang_dos=0.0f;
}

Aplayer::~Aplayer()
{

    if (m_tex1)
    {

        delete (m_tex1);
    }
    if (m_tex2)
    {
        delete (m_tex2);
    }
    if (m_tex3)
    {
        delete (m_tex3);
    }
    if (m_tex4)
    {
        delete (m_tex4);
    }

    if (parts)
    {
        delete parts;
        parts = 0;
    }

    if (voiture)
        delete (voiture);

    // delete(model.m_pstudiohdr);
}

void Aplayer::init(CSoundManager *g_pSoundManager)
{
    /*
           HRESULT hr = g_pSoundManager->Create( playersound, "data/Door5.wav", DSBCAPS_CTRL3D  , DS3DALG_HRTF_FULL );
           if( FAILED( hr ) || hr == DS_NO_VIRTUALIZATION )
           {
               DXTRACE_ERR_NOMSGBOX( TEXT("Create"), hr );
               return false;
           }

           // Get the 3D buffer from the secondary buffer
           if( FAILED( hr = playersound->Get3DBufferInterface( 0, &g_pDS3DBuffer ) ) )
           {
               DXTRACE_ERR( TEXT("Get3DBufferInterface"), hr );
                return false;
           }
           g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
           g_pDS3DBuffer->GetAllParameters( &g_dsBufferParams );

   */
}
void Aplayer::init_particle(world_t *world)
{

    voiture = new CPhysEnv;

    voiture->LoadData(vec3_t(3478.0f, 225.0f, -200.0f));

    voiture->SetWorld(world);

    voiture->SetEXPLODE(parts);

    if (!(parts = new pParticleManager_t))
    {
        HRESULT hr = NULL;
        DXTRACE_ERR(TEXT("particle system"), hr);
    }

    parts->SetGravity(vec3_t(0.0f, 0.0f, -100.0f)); // gravity
    m_tex1 = new texMan_t;
    parts->SetId(pTypeSimple, m_tex1->Load("data/env/blood.tga"));
    m_tex2 = new texMan_t;
    parts->SetId(pTypeBurst, m_tex2->Load("data/env/fire.tga"));
    parts->SetId(pTypeTorch, m_tex2->Load("data/env/fire.tga"));
    m_tex3 = new texMan_t;
    parts->SetId(pTypeBoom, m_tex3->Load("data/env/smoke.tga"));
    m_tex4 = new texMan_t;
    parts->SetId(pTypeSmoke, m_tex4->Load("data/env/smoke2.tga"));
}
void Aplayer::Last_pos(void)
{
    //	frameTime.Frame();
}
float Aplayer::GetDelta(void)
{

    // float delta = 0.0f;
    //	delta = frameTime.GetDelta();
    return 0.04f;
}

void Aplayer::SetTraction(bool av, bool ar, bool dr, bool ga, bool fr)
{
    // m_PhysEnv->SetTraction(av,ar,dr,ga,fr );
}

void Aplayer::Render_particle(float delta)
{

    parts->Frame(delta);
    parts->Render();
}

void Aplayer::Voix(CSound *lessons)
{
    //	strcpy(action2,TEXT("start_reload"));
    voice = (CSound *)lessons;
    voice->Init();
    D3DVECTOR vpos;
    D3DVECTOR vv;
    vv.x = 0.0f;
    vv.z = 0.0f;
    vv.y = 0.0f;

    vpos.x = pos[0];
    vpos.y = pos[1];
    vpos.z = pos[2];

    voice->Positionne3Dbuffer(&vpos, &vv);
    voice->Play();
}
void Aplayer::play_sound(void)
{
    D3DVECTOR vpos;
    D3DVECTOR vv;
    if (playersound->IsSoundPlaying() == TRUE)
    {
        playersound->Reset();
    }
    vv.x = 0.0f;
    vv.y = -1.0f;
    vv.z = 0.0f;
    vpos.x = pos[0];
    vpos.y = pos[1];
    vpos.z = pos[2];

    playersound->Positionne3Dbuffer(&vpos, &vv);
    playersound->Play(0, 0); // DSBPLAY_LOOPING
}

void Aplayer::anim()
{

    char ligne[30];
    modele.fps_weapon = arme.fps_weapon;
    if (calcul_rendu != 1)
    {

        if (local == true)
        {

            if (is_car)
            {

                // voiture->Simulate(fps*2.0f,true);
                voiture->Simulate(fps * 10.0f, true); // 0.2f marche bien
                voiture->RenderWorld();               // DRAW THE SIMULATION
                pos = voiture->AxeG;
            }
            else
            {

                if (mort == true)
                {
                    occupe = true;
                    bool deja_en_cours = true;
                    int fps_avant;

                    //** MULTI MORT ajouté le 06/01/2005
                    if (sequence_mort_deja_affecte == false)
                    {
                        strcpy(dep_mort, RenvoieSeqMort());
                        sequence_mort_deja_affecte = true;
                    }
                    strcpy(dep, dep_mort);
                    strcpy(action, dep_mort);

                    // strcpy(dep,TEXT("head"));
                    // strcpy(action,TEXT("head"));
                    // dep="head";
                    // action="head";
                    if (strcmp(str_dep, dep) != 0)

                    {
                        strcpy(str_dep, dep);
                        // str_dep=dep;
                        modele.SetSequence2(str_dep, 0);
                        modele.fps1 = 0;
                        deja_en_cours = false;
                        reload->Stop();
                        if (meurt->IsSoundPlaying() == TRUE)
                        {
                            meurt->Reset();
                        }
                        D3DVECTOR vpos;
                        vpos.x = pos[0];
                        vpos.y = pos[1];
                        vpos.z = pos[2];
                        D3DVECTOR vv;
                        vv.x = 0.0f;
                        vv.z = 0.0f;
                        vv.y = 0.0f;

                        meurt->Positionne3Dbuffer(&vpos, &vv);

                        meurt->Play(0, 0);
                    }

                    if (strcmp(str_act, action) != 0)
                    {
                        strcpy(str_act, action);
                        modele.SetSequence2(str_act, 1);
                        modele.fps2 = 0;
                    }

                    if (occupe == true)
                    {
                        fps_avant = modele.fps1;
                        modele.AdvanceFrame(fps * sens, 0);
                        modele.AdvanceFrame(fps, 1);
                        arme.AdvanceFrame(fps, 0);
                        if ((modele.AniActFini == true))
                        {
                            modele.fps1 = fps_avant;
                            modele.fps2 = fps_avant;
                            occupe = false;
                        }
                    }
                    else
                    {
                        /*modele.AdvanceFrame( 0*sens,0);
                        modele.AdvanceFrame( 0,1);
                        arme.AdvanceFrame( 0,0);*/
                    }
                }
                else
                {
                    sequence_mort_deja_affecte = false; //**MULTI MORT 06/01/2005
                    if (arme.munition == 0 && strcmp(arme.name, "grenade") && strcmp(arme.name, "c4") &&
                        strcmp(arme.name, "fumigene") && strcmp(arme.name, "plasma") && occupe == false)
                    {
                        reload_arme();
                    }
                    else
                    {
                        if ((!strcmp(arme.name, "grenade") || !strcmp(arme.type, "fumigene")) && arme.munition <= 0 &&
                            occupe == false)
                        {
                            strcpy(action2, TEXT("pullpin"));
                            // modele.fps2=0;
                            // vue.fps2=0;
                        }
                    }

                    fire = false;

                    if ((tir == true) && (occupe == false))
                    {

                        if (action != "shoot")
                        // char ch[20];
                        // strcpy(ch,TEXT("shoot"));
                        // sprintf(ch,"%s\0",TEXT("shoot"));

                        // if (strcmp(action,ch)!=0)
                        {
                            modele.fps2 = 0;
                            vue.fps2 = 0;

                            occupe = true;
                            arme.munition--;
                            play_sound();
                        }
                        fire = (action != "shoot");
                        // fire=(strcmp(action,TEXT("shoot"))!=0);
                        // action="shoot";
                        strcpy(action, TEXT("shoot"));

                        if (!strcmp(arme.name, "grenade") || !strcmp(arme.name, "fumigene"))
                        {
                            strcpy(action2, TEXT("throw")); // throw interne marche // externe il faut reload ??
                        }
                        else
                        {
                            if (!strcmp(arme.name, "c4"))
                            {
                                strcpy(action2, TEXT("drop"));
                            }
                            else
                            {

                                strcpy(action2, TEXT("shoot1"));
                            }
                        }
                    }

                    // sprintf(ligne,"%s\0",dep);
                    if (strcmp(str_dep, dep) != 0)
                    {
                        strcpy(str_dep, dep);
                        // str_dep=dep;
                        // 	modele.fps1=0;
                        modele.SetSequence2(str_dep, 0);
                    }

                    sprintf(ligne, "%s%s%s\0", TEXT("ref_"), action, arme.type);
                    if (strcmp(str_act, ligne) != 0)
                    {
                        strcpy(str_act, ligne);
                        modele.SetSequence2(str_act, 1);
                    }

                    vue.SetSequence2(action2, 1);
                    vue.AdvanceFrame(fps, 1);
                    if (strcmp(str_dep, TEXT("jump")) == 0)
                    {
                        sens = 1;
                    }
                    modele.AdvanceFrame(fps * sens, 0);
                    // if (action=="aim")
                    char ch[20];
                    strcpy(ch, TEXT("aim"));
                    if (strcmp(action, ch) == 0)
                    {
                        if (!strcmp(arme.type, "_hegrenade"))
                        {
                            strcpy(action2, TEXT("idle"));
                            sprintf(ligne, "%s%s%s\0", TEXT("ref_"), TEXT("shoot"), "_grenade");
                            strcpy(str_act, ligne);
                        }
                        else
                        {

                            if (!strcmp(arme.type, "_plasma"))
                            {
                                strcpy(action2, TEXT("idle"));
                                sprintf(ligne, "%s%s%s\0", TEXT("ref_"), TEXT("shoot"), "_grenade");
                                strcpy(str_act, ligne);
                            }
                            else
                            {
                                if (!strcmp(arme.type, "_c4"))
                                {
                                    strcpy(action2, TEXT("idle"));
                                    sprintf(ligne, "%s%s%s\0", TEXT("ref_"), TEXT("shoot"), "_c4");
                                    strcpy(str_act, ligne);
                                }
                                else
                                {

                                    strcpy(action2, TEXT("idle1"));
                                    sprintf(ligne, "%s%s%s\0", TEXT("ref_"), TEXT("shoot"), arme.type);
                                    strcpy(str_act, ligne);
                                }
                            }
                        }

                        modele.SetSequence2(ligne, 1);
                        modele.fps2 = -1000;
                        modele.AdvanceFrame(0, 1);
                        arme.AdvanceFrame(0, 0);
                        // modele.fps2=0;
                        // modele.AdvanceFrame( 0,1);
                        // arme.AdvanceFrame( 0,0);
                    }
                    else
                    {
                        // modele.AdvanceFrame( fps,1);
                        arme.AdvanceFrame(fps, 0);
                        modele.fps2 = (int)(vue.fps2 * modele.maxframes / vue.maxframes);
                    }

                    /*modele.SetSequence2(str_act,1);
                    modele.fps2 =(int) (vue.fps2*modele.maxframes/vue.maxframes)  ;
                    strcpy(ch, TEXT("aim") );
                    if(strcmp(action,ch)==0)
                    {
                        modele.fps2=1;
                    }*/
                    strcpy(ch, TEXT("aim"));
                    // if ((vue.AniDepFini==true) && (action2 !="idle"))
                    // if ((vue.AniDepFini==true) && (action !="aim"))
                    if ((vue.AniDepFini == true) && (strcmp(action, ch) != 0))
                    {
                        if (strcmp(action2, "start_reload") == 0)
                        {

                            arme.munition = 1;
                            strcpy(action2, TEXT("insert"));
                            vue.fps2 = 0;
                        }
                        else if (strcmp(action2, "insert") == 0)
                        {
                            vue.fps2 = 0;
                            arme.munition++;
                            if (arme.munition < 6)
                            {
                            }
                            else
                            {
                                strcpy(action2, TEXT("after_reload"));
                            }
                        }
                        else
                        {
                            occupe = false;
                            // action ="aim";
                            strcpy(action, TEXT("aim"));
                            if (strcmp(action2, "throw") == 0)
                            {
                                strcpy(action2, TEXT("idle"));
                            }
                            else
                            {
                                strcpy(action2, TEXT("idle1"));
                            }
                        }
                        // tir=false;
                    }
                    /*vue.SetSequence2(action2,1);
                    char ch2[20];
                    strcpy(ch2, TEXT("reload") );
                    if(strcmp(action2,ch2)==0)
                    {
                        vue.m_frame =modele.fps2*vue.maxframes/modele.maxframes  ;
                    }
                    else
                    {
                        vue.m_frame =modele.fps2;
                    }*/
                }

            } // noty is_car
        }
        else // joueur distant
        {
            // fps1_recu=fps1_recu+1;

            if ((modele.fps1 != fps1_recu) || (modele.fps2 != fps2_recu) || (modele.seq1 != id_seq1_recu) ||
                (modele.seq2 != id_seq2_recu))
            {
                modele.fps1 = modele.fps1;
            }
            else
            {
                calcul_rendu = 1;
                //	calcul_rendu=2;
            }

            if (is_car)
                voiture->RenderFake(voiture->AxeG, voiture->AxeDevant, voiture->AxeHaut);
            else
            {
                pos = pos + velocity;

                modele.fps1 = fps1_recu;
                modele.seq1 = id_seq1_recu;
                modele.fps2 = fps2_recu;
                modele.seq2 = id_seq2_recu;
                /*modele.AdvanceFrame( fps*sens,0);
                modele.AdvanceFrame( fps,3);
                arme.AdvanceFrame( fps,0);*/
            }
        }
    }

    /*	g_pDS3DBuffer->SetPosition ((d3dvalue
     */

    modele.m_origin[0] = pos[0];
    modele.m_origin[1] = pos[1];
    modele.m_origin[2] = pos[2];
    arme.m_origin[0] = pos[0];
    arme.m_origin[1] = pos[1];
    arme.m_origin[2] = pos[2];

    if (mort)
    {
        modele.m_origin[2] = pos[2] + 12.0f;
        arme.m_origin[2] = pos[2] + 12.0f;
    }

    modele.m_angles[0] = rot[0];
    modele.m_angles[1] = rot[1];
    modele.m_angles[2] = rot[2];

    arme.m_angles[0] = rot[0];
    arme.m_angles[1] = rot[1];
    arme.m_angles[2] = rot[2];
    double a[3];
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    if (Visible && !(is_car))
    {

        modele.Draw(calcul_rendu, a);
        memcpy(arme.matrice_pere, modele.matrice_pere, sizeof(float) * 12);

        arme.Draw(calcul_rendu, a);
        modele.flash[0] = arme.flash[0];
        modele.flash[1] = arme.flash[1];
        modele.flash[2] = arme.flash[2];
    }
}
void Aplayer::reload_arme()
{
    modele.fps2 = 0;
    vue.fps2 = 0;

    occupe = true;
    arme.munition = arme.balles;
    // action="reload";
    strcpy(action, TEXT("reload"));
    strcpy(action2, TEXT("reload"));

    if (strcmp(arme.name, TEXT("pompe")) == 0)
        strcpy(action2, TEXT("start_reload"));

    // playersound->Stop();
    if (reload->IsSoundPlaying() == TRUE)
    {
        reload->Reset();
    }
    D3DVECTOR vpos;

    vpos.x = pos[0];
    vpos.y = pos[1];
    vpos.z = pos[2];
    D3DVECTOR vv;
    vv.x = 0.0f;
    vv.z = 0.0f;
    vv.y = 0.0f;

    reload->Positionne3Dbuffer(&vpos, &vv);

    reload->Play(0, 0);
}

void Aplayer::affecte_sound(CSound *lessons)
{

    /*if (playersound!=NULL)

    {

        if (playersound->IsSoundPlaying ()==TRUE)
        {
            //playersound->Stop ();
            playersound->Reset ();

        }
        //playersound->Play();
        //playersound=NULL;
        playersound->~CSound ();
    }*/
    // playersound= new CSound( ;

    /*playersound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );
    playersound = new CSound( lessons->m_apDSBuffer, lessons->m_dwDSBufferSize, lessons->m_dwNumBuffers,
    lessons->m_pWaveFile );

    //playersound= lessons->m_pWaveFile;
    //lessons->m_apDSBuffer */
    playersound = (CSound *)lessons;
    playersound->Init();
    // playersound->Reset ();
}
void Aplayer::chg_phys(void)
{
    if (!is_car)
    {
        // voiture->AxeG=pos;

        // voiture->SetPos(pos);
    }

    is_car = (!is_car);
}

void Aplayer::affecte_effet(CSound *lessons, int lequel)
{

    if (lequel == 0)
    {
        reload = (CSound *)lessons;
        reload->Init();
    }
    else if (lequel == 1)
    {
        meurt = (CSound *)lessons;
        meurt->Init();
    }
    else if (lequel == 2)
    {
        marche = (CSound *)lessons;
        marche->Init();
    }
}
void Aplayer::affecte_modele(Model_MDL *leshommes, BYTE id_mod, int max)
{

    if (id_mod >= max)
    {
        id_mod = 0;
    }
    id_modele = id_mod;
    modele = leshommes[id_modele];

    modele.lie = 0;
    modele.fps2 = 0;
    modele.fps1 = 0;
    arme.fps2 = 0;
    arme.fps1 = 0;
    vue.fps2 = 0;
    vue.fps1 = 0;

    if ((id_modele == 0) || (id_modele == 3) || (id_modele == 4) || (id_modele == 7) || (id_modele == 9) ||
        (id_modele == 11))
        QuelTeam = 0;
    else
        QuelTeam = 1;
}

void Aplayer::affecte_arme(Model_MDL *lesarmes, BYTE id_arm, int max)
{
    if (id_arm >= max)
    {
        id_arm = 0;
    }
    /*if (max==100)
    {
        ofstream o("armes.log");
        o << "1" << endl;


        arme.~Model_MDL();
        o << "2" << endl;
        id_weapon=id_arm;
        o << "3" << endl;

        arme=(Model_MDL)lesarmes[id_weapon];
        o << "4" << endl;

        arme.lie=1;
            o << "5" << endl;

        arme.scope=lesarmes[id_weapon].scope  ;
        arme.balles=lesarmes[id_weapon].balles  ;
        arme.puissance=lesarmes[id_weapon].puissance  ;
            o << "6" << endl;


        ammo=arme.balles;
            o << "7" << endl;



    }
    else
    {
        ofstream o("armes2.log");
        o << "1" << endl;

*/
    //	arme.~Model_MDL();
    // o << "2" << endl;
    int tmp = arme.munition;
    lesarmes[id_weapon].cur_scope = arme.cur_scope;

    lesarmes[id_weapon].munition = arme.munition;
    id_weapon = id_arm;
    // lesarmes[id_weapon].munition=arme.munition;

    // o << "3" << endl;
    arme = lesarmes[id_weapon];
    // o << "4" << endl;
    arme.munition = lesarmes[id_weapon].munition;
    arme.lie = 1;
    // o << "5" << endl;

    strcpy(arme.type, lesarmes[id_weapon].type);
    // o << "6" << endl;
    arme.scope = lesarmes[id_weapon].scope;
    arme.cur_scope = lesarmes[id_weapon].cur_scope;
    arme.balles = lesarmes[id_weapon].balles;
    arme.puissance = lesarmes[id_weapon].puissance;
    strcpy(arme.name, lesarmes[id_weapon].name);
    strcpy(vue.name, lesarmes[id_weapon + max].name);

    //}
    vue = lesarmes[id_weapon + max];
}

void Aplayer::SpawnBlood()
{
    // parts->SpawnPos(pos);
    // parts->SystemNew(new pSystemSimple_t, pos, 1);
    parts->SystemNew(new pSystemSimple_t, pos, 1);
    // parts->SystemNew(new pSystemBurst_t, pos, 1);
}
void Aplayer::SpawnSmoke()
{
    // parts->SpawnPos(pos);
    // parts->SystemNew(new pSystemSimple_t, pos, 1);
    // parts->SystemNew(new pSystemBoom_t, pos, 1);
    vec3_t bas;
    bas[0] = pos[0];
    bas[1] = pos[1];
    bas[2] = pos[2];

    // parts->SystemNew(new pSystemBoom_t, bas, 1);
    parts->SystemNew(new pSystemSmoke_t, bas, 1);
}
void Aplayer::SpawnTorch()
{
    // parts->SpawnPos(pos);
    vec3_t bas;
    bas[0] = pos[0];
    bas[1] = pos[1];
    bas[2] = pos[2] - 10;

    // parts->SystemNew(new pSystemBoom_t, bas, 1);
    parts->SystemNew(new pSystemTorch_t, bas, 1);
    /*parts->SystemNew(new pSystemSimple_t, pos, 1);
    parts->SystemNew(new pSystemBoom_t, pos, 1);
    */
}

const char *Aplayer::RenvoieSeqMort()
{
    int rndo = random_t::RandomRange(0, 8);
    switch (rndo)
    {
    case 0:
        return TEXT("death1");
        break;

    case 1:
        return TEXT("death2");
        break;

    case 2:
        return TEXT("death3");
        break;

    case 3:
        return TEXT("head");
        break;

    case 4:
        return TEXT("gutshot");
        break;

    case 5:
        return TEXT("left");
        break;

    case 6:
        return TEXT("back");
        break;

    case 7:
        return TEXT("right");
        break;

    case 8:
        return TEXT("forward");
        break;
    }
    return TEXT("head");
}
