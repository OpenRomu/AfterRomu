/*

 * core engine logic
 * XXX this file is way too big...
 */

#define TIMEOUT 0x0100

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
using namespace std;
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

// static Model_MDL model;
static vector<vec3_t> *lespos;

// les pragma attention a virer pour bon debug
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)

bool gravite;
bool les_boites;
bool boites_mdl;
bool camera_libre;
bool cycle;

bool switchcam;
int laface;

bool sniper;
////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define DPLAY_SAMPLE_KEY TEXT("Software\\Microsoft\\DirectX DirectPlay Samples")
#define MAX_PLAYER_NAME 14
#define WM_APP_UPDATE_STATS (WM_APP + 0)
#define WM_APP_DISPLAY_WAVE (WM_APP + 1)
#define EPSILONN 0.001f
GLfloat RVBAFog[] = {0.2, 0.2, 0.2, 0.2}; /* couleur du brouillard */

//-----------------------------------------------------------------------------
// App specific DirectPlay messages and structures
//-----------------------------------------------------------------------------

// Change compiler pack alignment to be BYTE aligned, and pop the current value
#pragma pack(push, 1)

typedef struct
{
    char _cData[256];
    char _Host[50];

} TServerConnectData; // ajout bat

typedef struct
{
    char file[50];
    int lie;
    char type[30];
    int scope;
    int balles;
    int puissance;
    int id_son;
    float fps_weapon_ext;
    float fps_weapon_vue;
    float multi;
    bool droitier;
} T_armes; // ajout bat

// Pop the old pack alignment
#pragma pack(pop)

#define _WIN32_DCOM
///////////////////////////////////////////////////////

static world_t world;

static matrix_t mm;
static matrix_t mm2;

////////////////////////////////////////////////////////////////
Engine::Engine()
    // Engine::Engine()
    : CFT_ON(false), SNIPER_ON(false), respawn_time(0.0f), time_acceleration(1.0f), offset(0), mode_op(false),
      is_op(false), active_input(0), mode_ecoute(false), msens(0.12f), m_tchat(0.0f), gamadelta(0.0f), maximun_arme(0),
      curr_tournois(0), DisplayGamma(false), TOURNOIS_ON(false), TEAM_ON(false), PASS_ON(false), m_font(0),
      type_download(0), est_fatigue(false), fatigue(0.0f), cours(1.0f), speed(0.0f), tps_menu(0), couvre_feu(false),
      mp3(NULL), m_romu(NULL), old_tick(0), old_son(0), old_tchat(0), old_tick_team(0), old_pas(0), m_overlay(true),
      modechat(false), menu_mode(false), menu_state(0), old_menu_state(0), m_xcenter(0.0f), earth_quake(0.0f),
      m_phrase(NULL), m_chat(NULL), m_ycenter(0.0f), m_playerfile(NULL), g_bWasLobbyLaunched(FALSE),
      g_dpnidLocalPlayer(0), g_lNumberOfActivePlayers(0), release_tir_car(true), m_bSearchingForSessions(FALSE),
      bConnectSuccess(FALSE), blend(0), max_joueur(4), max_modele(1), le_rendu(0), g_pSoundManager(NULL), old_model(0),
      cle_tournois(0), round(0), tps_earthquake(0.0f), romuchat_on(true), curmp3id(0), dphdl(NULL), release_mouse(true),
      g_camera_suivi_old(1), g_camera_suivi(0), m_bHostPlayer(false), release_tir(0), downloading(INET_IDLE),
      force_crouch(false), g_pDS3DBuffer(NULL), g_pDSListener(NULL), g_pSporte(NULL), g_pSound(NULL), m_tex_impact(0),
      marks(0), lagg_ms(50), idpartie(0), killer(0), g_pGraphBuilder(NULL), config("data/config.ini"), phys(false),
      le_socket(0), m_tex(NULL), parts(NULL), parts2(NULL), grenades(NULL), m_cross(NULL), m_panel(NULL),
      SCOPE_ETA(false), BTN_DROIT(false), lan_mode(false), storm_off(false), output_file(NULL), mode_storm(false),
      time_storm(0.0f), write_count(0)

//,pWorld(0)
{
    // timing des eclaires
    eclaire[0] = 5.0f;  // off
    eclaire[1] = 6.0f;  // on
    eclaire[2] = 10.0f; // off
    eclaire[3] = 11.0f; // on
    eclaire[4] = 13.0f; // off

    MAC[0] = '\0';

    for (int d = 0; d <= max_arme; d++)
    {
        les_armes_autorisees[d] = '*';
    }
    les_armes_autorisees[max_arme + 1] = '\0';
    //_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
    // _CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);
    // _CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR);
    //_CrtSetBreakAlloc(3440);
    strcpy(m_txt_tchat, "/0");

    for (int d = 0; d <= max_son; d++)
    {
        lessons[d] = NULL;
    }
    old_tchat = GetTickCount() + 5000;

    m_bHaveConnectionSettingsFromLobby = FALSE;
    m_hLobbyClient = NULL;
    m_hDlg = NULL;
    m_bConnecting = FALSE;
    m_hConnectAsyncOp = NULL;
    m_pDeviceAddress = NULL;
    m_pHostAddress = NULL;
    m_hEnumAsyncOp = NULL;
    m_bMigrateHost = FALSE;

    m_dwEnumHostExpireInterval = 0;
    VRAI = 0;

    //** CFT init du global avoirt si on le colle dans le xml
    CFT_ON = false;
    CFT_eta = 0;
    CFT_count = 0;
    CFT_old_count = 0;
    CFT_nb_gign = 0;
    CFT_nb_terro = 0;

    vue_cam = vec3_t(0.0f, 0.0f, 0.0f);

    sprintf(cur_md5, "");

    TEAM_ON = false;
    exterieur = false;
    // Set the max players unlimited by default.  This can be changed by the app
    // by calling SetMaxPlayers()
    m_dwMaxPlayers = 0;

    _tcscpy(m_strAppName, TEXT("ROMUSTRIKE SESSION"));
    _tcscpy(m_strPreferredProvider, TEXT("DirectPlay8 TCP/IP Service Provider"));
    InitializeCriticalSection(&m_csHostEnum);
    InitializeCriticalSection(&m_csTeam);

    m_hConnectCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hLobbyConnectionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    // Setup the m_DPHostEnumHead circular linked list
    ZeroMemory(&m_DPHostEnumHead, sizeof(DPHostEnumInfo));
    m_DPHostEnumHead.pNext = &m_DPHostEnumHead;
}

Engine::~Engine()
{
}

void Engine::resize()
{
    world.projection_setup(m_width, m_height);
    m_xcenter = (float)m_width / 2;
    m_ycenter = (float)m_height / 2;
}

void Engine::Run()
{
}

void Engine::init()
{

    //	unsigned int Client;

    /*
     */
    lejoueur.clear();
    strcpy(m_cur_map, "");
    strcpy(m_cur_host, "");

    cycle = true;

    lespos.clear();
    amoi.clear();

    FRIENDLY = 0;
    gravite = true;
    camera_libre = true;
    souris_inverse = 1;
    // dplay init
    HRESULT hr;

    SAFE_RELEASE(m_pDeviceAddress);
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,
                          (LPVOID *)&m_pDeviceAddress);
    // Create a host address
    SAFE_RELEASE(m_pHostAddress);
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,
                          (LPVOID *)&m_pHostAddress);

    m_xmlsession.mode(config.isdebug);
    msens = config.msens / 1000.0f;

    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, RVBAFog);
    glFogi(GL_FOG_START, 1.0);
    glFogi(GL_FOG_END, 2.0);
    glFogf(GL_FOG_DENSITY, 0.05);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    menu_mode = 1; // bat 06-06/2002
    menu_state = 1100; // etat init

    // misc
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_POLYGON_STIPPLE);
    glDisable(GL_DITHER);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LOGIC_OP);

    // texture settings
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // GL_LESS
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    //  nice perspective
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // culling
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    mp3 = new Cmp3(m_pgraph);

    if (mp3)
    {
        mp3->init(m_hwnd);
    }

    m_playerfile = new playerfile_t;

    m_font = new Font("data/env/font.tga");
    if (!m_font)
        throw out_of_memory();

    m_romu = new Romu("data/env/logo1.tga");
    if (!m_romu)
        throw out_of_memory();

    m_chat = new Cchat(10.0f, 120.0f, 10.0f);
    if (!m_chat)
        throw out_of_memory();

    if (m_chat)
        m_chat->TAB.clear();

    m_phrase = new Cchat(350.0f, 60.0f, 5.0f);
    if (!m_phrase)
        throw out_of_memory();

    if (m_phrase)
        m_phrase->TAB.clear();


    m_cross = new Cross("data/env/lescroix.bmp", "data/env/sniper.bmp", "data/env/noir.bmp");
    m_cross->position = config.viseur;


    m_panel = new Panel("data/env/ammog.bmp", "data/env/ammo.bmp", "data/env/smoke2.bmp", "data/env/vie.bmp",
                        "data/env/players.bmp", "data/env/lazer1.bmp");
    m_pings = new Pings(m_pDP);
    if (!(marks = new mMarkManager_t))
    {
        DXTRACE_ERR(TEXT("marks system"), hr);
    }

    marks->Init(geomNumMarks);
    m_tex_impact = new texMan_t;

    marks->SetTexture(m_tex_impact->Load("data/env/torch.tga"));
    marks->SetTexture_sang(m_tex_impact->Load("data/env/blood2.tga"));
    marks->SetTexture_trace(m_tex_impact->Load("data/env/hegrenade.tga"));

    if (!(parts = new pParticleManager_t))
    {
        HRESULT hr = NULL;
        DXTRACE_ERR(TEXT("particle system"), hr);
    }

    m_tex = new texMan_t;

    parts->SetGravity(vec3_t(0.0f, 0.0f, -100.0f)); // gravity
    parts->SetId(pTypeSimple, m_tex->Load("data/env/blood.tga"));
    parts->SetId(pTypeBurst, m_tex->Load("data/env/tire.tga"));
    parts->SetId(pTypeBoom, m_tex->Load("data/env/smoke.tga"));
    parts->SetId(pTypeTorch, m_tex->Load("data/env/fire.tga"));
    parts->SetId(pTypeSmoke, m_tex->Load("data/env/smoke2.tga"));
    parts->SetId(pTypeHeSmoke, m_tex->Load("data/env/smokehe.tga")); // smokehe
    /*-----------------------*/
    // parts2 pour les flash qui doievnt etre dans la matrice camera
    if (!(parts2 = new pParticleManager_t))
    {
        HRESULT hr = NULL;
        DXTRACE_ERR(TEXT("particle2 system"), hr);
    }
    parts2->SetGravity(vec3_t(0.0f, 0.0f, -100.0f)); // gravity
    parts2->SetId(pTypeSimple, m_tex->Load("data/env/blood2.tga"));
    parts2->SetId(pTypeBurst, m_tex->Load("data/env/tire.tga"));
    parts2->SetId(pTypeBoom, m_tex->Load("data/env/smoke.tga"));
    parts2->SetId(pTypeSmoke, m_tex->Load("data/env/smoke2.tga"));
    parts2->SetId(pTypeHeSmoke, m_tex->Load("data/env/smokehe.tga"));

    if (!(grenades = new pGrenadeManager_t))
    {

        DXTRACE_ERR(TEXT("grenade system"), hr);
    }

    grenades->SetGravity(vec3_t(0.0f, 0.0f, -100.0f)); // gravity
    grenades->SetId(pgTypeSimple, m_tex->Load("data/env/grenade.tga"));
    grenades->SetId(pgTypeSmoke, m_tex->Load("data/env/hegrenade.tga"));
    grenades->SetId(pgTypeMissile, m_tex->Load("data/env/missile.tga"));
    grenades->SetId(pgTypeLazer, m_tex->Load("data/env/lazer1.tga"));
    grenades->SetWorld(&world);
    grenades->SetEXPLODE(parts);

    /*-----------------*/

    sprintf(m_xmlsession.GServerName, "http://%s", config.server_xml);

    float x = 388;
    float y = 545;
    float z = -73;
    m_camera.move(x, y, z);
    m_pivot.move(x, y, z);
    m_pivot2.move(x, y, z);
    m_pivot2.rotate(0, 0, 90);
    m_camera.rotate(0, 0, 90);
    m_pivot.rotate(0, 0, 0);


    PROC swp = wglGetProcAddress("wglSwapIntervalEXT");
    if (swp)
    {
        typedef BOOL(WINAPI * swp_t)(int);
        swp_t s = (swp_t)swp;
    }

    if (world.use_multi)
    {
        world.use_multi = enable_multitexture();
    }

    m_overlay = true;
    load3d();
    // sound bat

    g_pSoundManager = new CSoundManager();


    hr = g_pSoundManager->Initialize(m_hwnd, DSSCL_PRIORITY, 2, 22050, 16);

    // Get the 3D listener, so we can control its params
    hr |= g_pSoundManager->Get3DListenerInterface(&g_pDSListener);

    if (FAILED(hr))
    {
        DXTRACE_ERR(TEXT("Get3DListenerInterface"), hr);
        return;
    }

    // Get the 3D buffer parameters
    D3DVECTOR top;
    D3DVECTOR topo;
    top.x = 0.0f;
    top.y = 0.0f;
    top.z = 1.0f;
    topo.x = 0.0f;
    topo.y = 1.0f;
    topo.z = 0.0f;
    g_pDSListener->SetOrientation(top.x, top.y, top.z, topo.x, topo.y, topo.z, DS3D_IMMEDIATE);
    g_pDSListener->SetRolloffFactor(5.0f, DS3D_IMMEDIATE);

    init_porte();

    LoadLesSons();

    // ticker_t::Reset();
    vec3_t pos;

    // prepare model matrix
    mm.translate(0.0f, 0.0f, 0.0f);
    matrix_t r;
    r.xrot(-90.0f);
    mm *= r;

    r.load_identity();
    r.zrot(90.0f);
    mm *= r;

    r.load_identity();
    r.scale(0.3f, 0.3f, 0.3f);
    mm *= r;

    mm2.translate(0.0f, 0.0f, 0.0f);

    r.load_identity();
    r.xrot(-90.0f);
    mm2 *= r;

    r.load_identity();
    r.zrot(90.0f);
    mm2 *= r;

    int bas = 920;
    int espaceh = 80;
    console *Presentation = new console;
    char re[500];
    int niv1 = 250;

    sprintf(re, "ROMUSTRIKE v%d*%s*", m_xmlsession.Version_Soft, config.server_xml);
    int consolex = 350;
    int consolew = 575;

    Presentation->SetText(re);
    /// 0

    Presentation->SetXY(consolex, 100, consolew, 200);
    Presentation->SetFont(m_font);
    Presentation->visible = true;
    lesconsoles.push_back(Presentation);
    // 2
    console *mode_cnx = new console;
    mode_cnx->SetText("Choisissez*un mode de connection*");
    mode_cnx->SetXY(consolex, 550, consolew, 200);
    mode_cnx->SetFont(m_font);
    mode_cnx->visible = true;
    lesconsoles.push_back(mode_cnx);
    // 3
    console *panel_joueur = new console;
    panel_joueur->SetText("Choisissez*un mode de connection*");
    panel_joueur->SetXY(consolex, niv1, consolew, 100);
    panel_joueur->SetFont(m_font);
    panel_joueur->visible = true;
    lesconsoles.push_back(panel_joueur);
    // 4
    console *cnx = new console;
    cnx->SetText("Liste des parties en cours*Cliquez pour rejoindre une partie**");
    cnx->SetXY(consolex, niv1, consolew, 100);
    cnx->SetFont(m_font);
    cnx->visible = true;
    lesconsoles.push_back(cnx);
    // 5
    console *sessionenum = new console;
    sessionenum->SetText(
        "Recherche de session*Veuillez patientez...**Entrez un mot de passe*pour les parties privees*");
    sessionenum->SetXY(consolex, niv1, consolew, 100);
    sessionenum->SetFont(m_font);
    sessionenum->visible = true;
    lesconsoles.push_back(sessionenum);
    int calage_btn = 780;

    ////

    // boutoninit ialisation //matrice 1000 * 1000
    // 0
    bouton *quitter = new bouton;
    quitter->SetTexture("vie.bmp");
    quitter->SetXY(calage_btn, 900);
    quitter->SetFont(m_font);
    lesbtn.push_back(quitter);

    // 1
    bouton *jeu_local = new bouton;
    jeu_local->SetTexture("jeu_rezo.bmp");
    jeu_local->SetXY(500, 700);
    jeu_local->SetFont(m_font);
    lesbtn.push_back(jeu_local);

    // 2
    bouton *jeu_rezo = new bouton;
    jeu_rezo->SetTexture("login.bmp"); // jeu_local
    jeu_rezo->SetXY(800, 100);
    jeu_rezo->SetFont(m_font);
    lesbtn.push_back(jeu_rezo);

    // 3
    bouton *jouer_0 = new bouton;
    jouer_0->SetTexture("jouer_0.bmp");
    jouer_0->SetXY(calage_btn, bas - espaceh * 2);
    jouer_0->SetFont(m_font);
    lesbtn.push_back(jouer_0);
    // 4
    bouton *quitter_0 = new bouton;
    quitter_0->SetTexture("quitter_0.bmp");
    quitter_0->SetXY(calage_btn, bas);
    quitter_0->SetFont(m_font);
    lesbtn.push_back(quitter_0);

    // 5
    bouton *creer_partie = new bouton;
    creer_partie->SetTexture("creer_partie.bmp");
    creer_partie->SetXY(calage_btn, bas - espaceh);
    creer_partie->SetFont(m_font);
    lesbtn.push_back(creer_partie);
    // 6
    bouton *quitter_2 = new bouton;
    quitter_2->SetTexture("quitter_2.bmp");
    quitter_2->SetXY(calage_btn, bas);
    quitter_2->SetFont(m_font);
    lesbtn.push_back(quitter_2);

    // 7
    bouton *nb_plus = new bouton;
    nb_plus->SetTexture("nb_plus.bmp");
    nb_plus->SetXY(840, 500);
    nb_plus->SetFont(m_font);
    lesbtn.push_back(nb_plus);
    // 8
    bouton *nb_moins = new bouton;
    nb_moins->SetTexture("nb_moins.bmp");
    nb_moins->SetXY(840, 600);
    nb_moins->SetFont(m_font);
    lesbtn.push_back(nb_moins);
    // 6
    console *nb_j = new console;
    nb_j->SetText("nb de joueurs:");
    nb_j->SetXY(600, 534, consolew, 100);
    nb_j->SetFont(m_font);
    nb_j->visible = true;
    lesconsoles.push_back(nb_j);
    // 9
    bouton *lancer_151 = new bouton;
    lancer_151->SetTexture("lancer_151.bmp");
    lancer_151->SetXY(calage_btn, bas - espaceh);
    lancer_151->SetFont(m_font);
    lesbtn.push_back(lancer_151);
    // 10
    bouton *quitter_151 = new bouton;
    quitter_151->SetTexture("quitter_151.bmp");
    quitter_151->SetXY(calage_btn, bas);
    quitter_151->SetFont(m_font);
    lesbtn.push_back(quitter_151);
    // 11
    bouton *suivant = new bouton;
    suivant->SetTexture("suivant.bmp");
    suivant->SetXY(450, 353);
    suivant->SetFont(m_font);
    lesbtn.push_back(suivant);
    // 12
    bouton *precedent = new bouton;
    precedent->SetTexture("precedent.bmp");
    precedent->SetXY(450, 445);
    precedent->SetFont(m_font);
    lesbtn.push_back(precedent);

    // 13
    bouton *lancer_51 = new bouton;
    lancer_51->SetTexture("lancer_51.bmp");
    lancer_51->SetXY(calage_btn, bas - espaceh);
    lancer_51->SetFont(m_font);
    lesbtn.push_back(lancer_51);
    // 14
    bouton *quitter_51 = new bouton;
    quitter_51->SetTexture("quitter_51.bmp");
    quitter_51->SetXY(calage_btn, bas);
    quitter_51->SetFont(m_font);
    lesbtn.push_back(quitter_51);

    // 15
    bouton *quitter_110 = new bouton;
    quitter_110->SetTexture("quitter_110.bmp");
    quitter_110->SetXY(calage_btn, bas);
    quitter_110->SetFont(m_font);
    lesbtn.push_back(quitter_110);

    // 16
    bouton *lancer_64 = new bouton;
    lancer_64->SetTexture("lancer_64.bmp");
    lancer_64->SetXY(calage_btn, bas - espaceh);
    lancer_64->SetFont(m_font);
    lesbtn.push_back(lancer_64);
    // 17
    bouton *quitter_64 = new bouton;
    quitter_64->SetTexture("quitter_64.bmp");
    quitter_64->SetXY(calage_btn, bas);
    quitter_64->SetFont(m_font);
    lesbtn.push_back(quitter_64);

    // 18
    bouton *suivant_s = new bouton;
    suivant_s->SetTexture("suivant.bmp");
    suivant_s->SetXY(850, 460);
    suivant_s->SetFont(m_font);
    lesbtn.push_back(suivant_s);
    // 19
    bouton *precedent_s = new bouton;
    precedent_s->SetTexture("precedent.bmp");
    precedent_s->SetXY(850, 650);
    precedent_s->SetFont(m_font);
    lesbtn.push_back(precedent_s);

    // 20
    bouton *ctf_btn = new bouton;
    ctf_btn->SetTexture("ctf_btn.bmp");
    ctf_btn->SetXY(820, niv1);
    ctf_btn->SetFont(m_font);
    lesbtn.push_back(ctf_btn);

    // 21
    bouton *team_btn = new bouton;
    team_btn->SetTexture("team_btn.bmp");
    team_btn->SetXY(820, niv1);
    team_btn->SetFont(m_font);
    lesbtn.push_back(team_btn);

    // 22
    bouton *normal_btn = new bouton;
    normal_btn->SetTexture("normal_btn.bmp");
    normal_btn->SetXY(820, niv1);
    normal_btn->SetFont(m_font);
    lesbtn.push_back(normal_btn);
    // 7
    console *normal_csl = new console;
    normal_csl->SetText("mode de jeu:");
    normal_csl->SetXY(600, 360, consolew, 100);
    normal_csl->SetFont(m_font);
    normal_csl->visible = true;
    lesconsoles.push_back(normal_csl);
    // 23
    bouton *sans_mdp = new bouton;
    sans_mdp->SetTexture("sans_mdp.bmp");
    sans_mdp->SetXY(600, 735);
    sans_mdp->SetFont(m_font);
    lesbtn.push_back(sans_mdp);

    // 24
    bouton *avec_mdp = new bouton;
    avec_mdp->SetTexture("avec_mdp.bmp");
    avec_mdp->SetXY(600, 735);
    avec_mdp->SetFont(m_font);
    lesbtn.push_back(avec_mdp);
    // 8
    console *mdp_csl = new console;
    mdp_csl->SetText("PARTIE PRIVEE : ");
    mdp_csl->SetXY(consolex, 708, consolew, 100);
    mdp_csl->SetFont(m_font);
    mdp_csl->visible = true;
    lesconsoles.push_back(mdp_csl);
    // 9
    console *degat_csl = new console;
    degat_csl->SetText("degats:");
    degat_csl->SetXY(consolex, 534, consolew, 100);
    degat_csl->SetFont(m_font);
    degat_csl->visible = true;
    lesconsoles.push_back(degat_csl);
    // 10
    console *map_csl = new console;
    map_csl->SetText("map: italy");
    map_csl->SetXY(consolex, 360, consolew, 100);
    map_csl->SetFont(m_font);
    map_csl->visible = true;
    lesconsoles.push_back(map_csl);
    // 11
    console *mdpa_csl = new console;
    mdpa_csl->SetText("entrez un mot de passe:");
    mdpa_csl->SetXY(consolex, 770, consolew, 100);
    mdpa_csl->SetFont(m_font);
    mdpa_csl->visible = true;
    lesconsoles.push_back(mdpa_csl);
    // 12
    console *mdps_csl = new console;
    mdps_csl->SetText("tototo");
    mdps_csl->SetXY(consolex, 812, consolew, 100);
    mdps_csl->SetFont(m_font);
    mdps_csl->visible = true;
    lesconsoles.push_back(mdps_csl);

    // 25
    bouton *quitter_1 = new bouton;
    quitter_1->SetTexture("quitter_64.bmp");
    quitter_1->SetXY(calage_btn, bas);
    quitter_1->SetFont(m_font);
    lesbtn.push_back(quitter_1);

    // 26
    bouton *tournois = new bouton;
    tournois->SetTexture("tournois.bmp");
    tournois->SetXY(calage_btn, bas - espaceh);
    tournois->SetFont(m_font);
    lesbtn.push_back(tournois);

    // 27
    bouton *quitter_t = new bouton;
    quitter_t->SetTexture("quitter_64.bmp");
    quitter_t->SetXY(calage_btn, bas);
    quitter_t->SetFont(m_font);
    lesbtn.push_back(quitter_t);
    // 28
    bouton *suivant_t = new bouton;
    suivant_t->SetTexture("suivant.bmp");
    suivant_t->SetXY(50, bas);
    suivant_t->SetFont(m_font);
    lesbtn.push_back(suivant_t);
    // 29
    bouton *precedent_t = new bouton;
    precedent_t->SetTexture("precedent.bmp");
    precedent_t->SetXY(350, bas);
    precedent_t->SetFont(m_font);
    lesbtn.push_back(precedent_t);
    // 30
    bouton *lancer_tournois = new bouton;
    lancer_tournois->SetTexture("lancer_64.bmp");
    lancer_tournois->SetXY(calage_btn, bas - espaceh);
    lancer_tournois->SetFont(m_font);
    lesbtn.push_back(lancer_tournois);

    // 31
    bouton *creer_tournois = new bouton;
    creer_tournois->SetTexture("lancer_64.bmp");
    creer_tournois->SetXY(calage_btn, bas - espaceh);
    creer_tournois->SetFont(m_font);
    lesbtn.push_back(creer_tournois);

    // 32
    bouton *quitter_tournois = new bouton;
    quitter_tournois->SetTexture("quitter_64.bmp");
    quitter_tournois->SetXY(calage_btn, bas);
    quitter_tournois->SetFont(m_font);
    lesbtn.push_back(quitter_tournois);

    // 33
    bouton *creer_tournoisdetail = new bouton;
    creer_tournoisdetail->SetTexture("creer_partie.bmp");
    creer_tournoisdetail->SetXY(50, 300);
    creer_tournoisdetail->SetFont(m_font);
    lesbtn.push_back(creer_tournoisdetail);

    // 34
    bouton *quitter_tournoisdetail = new bouton;
    quitter_tournoisdetail->SetTexture("quitter_64.bmp");
    quitter_tournoisdetail->SetXY(calage_btn, bas);
    quitter_tournoisdetail->SetFont(m_font);
    lesbtn.push_back(quitter_tournoisdetail);
    // 35
    bouton *fire_normal = new bouton;
    fire_normal->SetTexture("fire_normal.bmp");
    fire_normal->SetXY(470, 574);
    fire_normal->SetFont(m_font);
    lesbtn.push_back(fire_normal);
    // 36
    bouton *fire_friendly = new bouton;
    fire_friendly->SetTexture("fire_friendly.bmp");
    fire_friendly->SetXY(470, 574);
    fire_friendly->SetFont(m_font);
    lesbtn.push_back(fire_friendly);
    // 37
    bouton *fire_self = new bouton;
    fire_self->SetTexture("fire_self.bmp");
    fire_self->SetXY(470, 574);
    fire_self->SetFont(m_font);
    lesbtn.push_back(fire_self);
    // 38
    bouton *mode_op_on = new bouton;
    mode_op_on->SetTexture("avec_mdp.bmp");
    mode_op_on->SetXY(200, 200);
    mode_op_on->SetFont(m_font);
    lesbtn.push_back(mode_op_on);
    // 39
    bouton *mode_op_off = new bouton;
    mode_op_off->SetTexture("sans_mdp.bmp");
    mode_op_off->SetXY(200, 200);
    mode_op_off->SetFont(m_font);
    lesbtn.push_back(mode_op_off);

    // 40

    bouton *map_img = new bouton;
    map_img->SetXY(10, 300);
    map_img->SetFont(m_font);
    map_img->enabled = false;
    lesbtn.push_back(map_img);

    // 41
    bouton *valider_img = new bouton;
    valider_img->SetTexture("valider.bmp");
    valider_img->SetXY(consolex + 150, 700);
    valider_img->SetFont(m_font);
    lesbtn.push_back(valider_img);
    // 42
    bouton *valider2_img = new bouton;
    valider2_img->SetTexture("valider.bmp");
    valider2_img->SetXY(consolex + 150, 800);
    valider2_img->SetFont(m_font);
    lesbtn.push_back(valider2_img);

    // 43
    bouton *jeu_lan = new bouton;
    jeu_lan->SetTexture("jeu_local.bmp"); //
    jeu_lan->SetXY(500, 800);
    jeu_lan->SetFont(m_font);
    lesbtn.push_back(jeu_lan);

    // 44
    bouton *btn_mode_lan = new bouton;
    btn_mode_lan->SetTexture("valider.bmp"); //
    btn_mode_lan->SetXY(500, 800);
    btn_mode_lan->SetFont(m_font);
    lesbtn.push_back(btn_mode_lan);
    // 45
    bouton *btn_creer_lan = new bouton;
    btn_creer_lan->SetTexture("creer_partie.bmp"); //
    btn_creer_lan->SetXY(calage_btn, bas - espaceh);
    btn_creer_lan->SetFont(m_font);
    lesbtn.push_back(btn_creer_lan);

    // 46
    bouton *btn_refresh = new bouton;
    btn_refresh->SetTexture("nb_plus.bmp"); //
    btn_refresh->SetXY(850, 400);
    btn_refresh->SetFont(m_font);
    lesbtn.push_back(btn_refresh);

    // 47
    bouton *btn_logout = new bouton;
    btn_logout->SetTexture("logout.bmp"); //
    btn_logout->SetXY(800, 100);
    btn_logout->SetFont(m_font);
    lesbtn.push_back(btn_logout);

    // 48
    bouton *sniper_btn = new bouton;
    sniper_btn->SetTexture("sniper_btn.bmp");
    sniper_btn->SetXY(820, niv1);
    sniper_btn->SetFont(m_font);
    lesbtn.push_back(sniper_btn);

    // 13
    console *feedback = new console;
    feedback->SetText("FEEDBACK");
    feedback->SetXY(consolex, calage_btn, consolew, 200);
    feedback->SetFont(m_font);
    feedback->visible = true;
    lesconsoles.push_back(feedback);
    // 14
    console *csl_pings = new console;
    csl_pings->SetText("");
    csl_pings->SetXY(10, 360, consolew, 200);
    csl_pings->SetFont(m_font);
    csl_pings->visible = true;
    lesconsoles.push_back(csl_pings);

    // 15
    console *csl_desc = new console;
    csl_desc->SetText("DESCRIPTION");
    csl_desc->SetXY(10, 360, consolew, 200);
    csl_desc->SetFont(m_font);
    csl_desc->visible = true;
    lesconsoles.push_back(csl_desc);

    // 16
    console *csl_desc1 = new console;
    csl_desc1->SetText("");
    csl_desc1->SetXY(50, 750, consolew, 200);
    csl_desc1->SetFont(m_font);
    csl_desc1->visible = true;
    lesconsoles.push_back(csl_desc1);

    // 16
    console *csl_stat = new console;
    csl_stat->SetText("");
    csl_stat->SetXY(50, 600, consolew, 200);
    csl_stat->SetFont(m_font);
    csl_stat->visible = true;
    lesconsoles.push_back(csl_stat);

    // 0
    input_box *input_login = new input_box;
    input_login->SetLibelle("PSEUDO : ");
    input_login->SetText("");
    input_login->SetXY(consolex, 500, consolew, 200);
    input_login->SetFont(m_font);
    input_login->visible = true;
    lesinput_box.push_back(input_login);

    // 1
    input_box *input_pwd = new input_box;
    input_pwd->SetLibelle("Mot de Passe : ");
    input_pwd->SetText("");
    input_pwd->SetXY(consolex, 600, consolew, 200);
    input_pwd->SetFont(m_font);
    input_pwd->visible = true;
    input_pwd->max_len = 10;

    lesinput_box.push_back(input_pwd);

    // 2
    input_box *input_newlogin = new input_box;
    input_newlogin->SetLibelle("PSEUDO : ");
    input_newlogin->SetText("");
    input_newlogin->SetXY(consolex, 500, consolew, 200);
    input_newlogin->SetFont(m_font);
    input_newlogin->visible = true;
    lesinput_box.push_back(input_newlogin);

    // 3
    input_box *input_newpwd = new input_box;
    input_newpwd->SetLibelle("Mot de Passe : ");
    input_newpwd->SetText("");
    input_newpwd->SetXY(consolex, 600, consolew, 200);
    input_newpwd->SetFont(m_font);
    input_newpwd->visible = true;
    input_newpwd->max_len = 10;
    lesinput_box.push_back(input_newpwd);
    // 4
    input_box *input_newemail = new input_box;
    input_newemail->SetLibelle("EMAIL : ");
    input_newemail->SetText("");
    input_newemail->SetXY(consolex, 700, consolew, 200);
    input_newemail->SetFont(m_font);
    input_newemail->max_len = 40;
    input_newemail->visible = true;
    lesinput_box.push_back(input_newemail);

    // 5
    input_box *input_mdp = new input_box;
    input_mdp->SetLibelle("Mot de passe : ");
    input_mdp->SetText("");
    input_mdp->SetXY(consolex, 800, consolew, 200);
    input_mdp->SetFont(m_font);
    input_mdp->max_len = 10;
    input_mdp->visible = true;
    lesinput_box.push_back(input_mdp);

    // 6
    input_box *input_partiedesc = new input_box;
    input_partiedesc->SetLibelle("Regles de la parties: ");
    input_partiedesc->SetText("");
    input_partiedesc->SetXY(consolex - 50, 200, consolew, 200);
    input_partiedesc->SetFont(m_font);
    input_partiedesc->max_len = 100;
    input_partiedesc->visible = true;
    lesinput_box.push_back(input_partiedesc);

    // 7
    input_box *input_pseudo_lan = new input_box;
    input_pseudo_lan->SetLibelle("Ton PSEUDO: ");
    input_pseudo_lan->SetText("PLAYER 1");
    input_pseudo_lan->SetXY(consolex - 50, 550, consolew, 200);
    input_pseudo_lan->SetFont(m_font);
    input_pseudo_lan->max_len = 20;
    input_pseudo_lan->visible = true;
    lesinput_box.push_back(input_pseudo_lan);

    // 8
    input_box *input_ip_lan = new input_box;
    input_ip_lan->SetLibelle("IP du SERVEUR : ");
    input_ip_lan->SetText("");
    input_ip_lan->SetXY(consolex - 50, 650, consolew, 200);
    input_ip_lan->SetFont(m_font);
    input_ip_lan->max_len = 100;
    input_ip_lan->visible = true;
    lesinput_box.push_back(input_ip_lan);

    // 9
    input_box *input_tchat = new input_box;
    input_tchat->SetLibelle("MESSAGE : ");
    input_tchat->SetText("");
    input_tchat->SetXY(consolex - 100, 100, consolew, 200);
    input_tchat->SetFont(m_font);
    input_tchat->max_len = 100;
    input_ip_lan->visible = true;
    lesinput_box.push_back(input_tchat);
}

void Engine::init_sound()
{
    static TCHAR strFileName[MAX_PATH] = TEXT("data/nu_3_long.wav");

    CWaveFile waveFile;

    waveFile.Open(strFileName, NULL, WAVEFILE_READ);
    WAVEFORMATEX *pwfx = waveFile.GetFormat();
    if (pwfx == NULL)
    {
        return;
    }

    if (pwfx->nChannels > 1)
    {
        // Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
        return;
    }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
        // Sound must be PCM when using DSBCAPS_CTRL3D
        return;
    }

    // Load the wave file into a DirectSound buffer
    HRESULT hr = g_pSoundManager->Create(&g_pSound, strFileName, DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL);
    if (FAILED(hr) || hr == DS_NO_VIRTUALIZATION)
    {
        DXTRACE_ERR_NOMSGBOX(TEXT("Create"), hr);
        return;
    }

    // Get the 3D buffer from the secondary buffer
    if (FAILED(hr = g_pSound->Get3DBufferInterface(0, &g_pDS3DBuffer)))
    {
        DXTRACE_ERR(TEXT("Get3DBufferInterface"), hr);
        return;
    }

    g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    g_pDS3DBuffer->GetAllParameters(&g_dsBufferParams);

    // Set new 3D buffer parameters
    g_dsBufferParams.dwMode = DS3DMODE_NORMAL; //;
    g_pDS3DBuffer->SetAllParameters(&g_dsBufferParams, DS3D_IMMEDIATE);

    D3DVECTOR pos;
    D3DVECTOR dir;
    dir.z = 1;
    dir.x = 0.5f;

    pos.x = 0;

    //	SetBackSoundProperties(&pos,&dir);
}
void Engine::ChargeUnSon(char *strFileName, int id, float min, float max)
{
    CWaveFile waveFile;

    waveFile.Open(strFileName, NULL, WAVEFILE_READ);
    WAVEFORMATEX *pwfx = waveFile.GetFormat();
    if (pwfx == NULL)
    {
        throw basic_exception("r.son");
        return;
    }

    if (pwfx->nChannels > 1)
    {
        // Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
        return;
    }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
        // Sound must be PCM when using DSBCAPS_CTRL3D
        return;
    }

    // Load the wave file into a DirectSound buffer
    HRESULT hr =
        g_pSoundManager->Create(&lessons[id], strFileName, DSBCAPS_CTRL3D, DS3DALG_DEFAULT); // DS3DALG_HRTF_FULL );
    if (FAILED(hr) || hr == DS_NO_VIRTUALIZATION)
    {
        DXTRACE_ERR_NOMSGBOX(TEXT("Create"), hr);
        return;
    }

    lessons[id]->min = min;
    lessons[id]->max = max;
    // Get the 3D buffer from the secondary buffer
    if (FAILED(hr = lessons[id]->Get3DBufferInterface(0, &lessons[id]->g_pDS3DBuffer)))
    {
        DXTRACE_ERR_NOMSGBOX(TEXT("Get3DBufferInterface"), hr);
        return;
    }

    lessons[id]->g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    lessons[id]->g_pDS3DBuffer->GetAllParameters(&lessons[id]->g_dsBufferParams);

    // Set new 3D buffer parameters
    lessons[id]->g_dsBufferParams.dwMode = DS3DMODE_NORMAL;
    lessons[id]->g_pDS3DBuffer->SetAllParameters(&lessons[id]->g_dsBufferParams, DS3D_IMMEDIATE);
}

void Engine::init_les_sons()
{
    static TCHAR strFileName[MAX_PATH] = TEXT("data/weapons/awp-1.wav");

    CWaveFile waveFile;

    waveFile.Open(strFileName, NULL, WAVEFILE_READ);
    WAVEFORMATEX *pwfx = waveFile.GetFormat();
    if (pwfx == NULL)
    {
        return;
    }

    if (pwfx->nChannels > 1)
    {
        // Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
        return;
    }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
        // Sound must be PCM when using DSBCAPS_CTRL3D
        return;
    }

    // Load the wave file into a DirectSound buffer
    HRESULT hr = g_pSoundManager->Create(&lessons[0], strFileName, DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL);
    if (FAILED(hr) || hr == DS_NO_VIRTUALIZATION)
    {
        DXTRACE_ERR_NOMSGBOX(TEXT("Create"), hr);
        return;
    }

    // Get the 3D buffer from the secondary buffer
    if (FAILED(hr = lessons[0]->Get3DBufferInterface(0, &g_pDS3DBuffer)))
    {
        DXTRACE_ERR(TEXT("Get3DBufferInterface"), hr);
        return;
    }

    g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    g_pDS3DBuffer->GetAllParameters(&g_dsBufferParams);

    // Set new 3D buffer parameters
    g_dsBufferParams.dwMode = DS3DMODE_NORMAL;                          // DS3DMODE_NORMAL; //DS3DMODE_HEADRELATIVE;
    g_pDS3DBuffer->SetAllParameters(&g_dsBufferParams, DS3D_IMMEDIATE); // DS3D_IMMEDIATE );

    D3DVECTOR pos;
    D3DVECTOR dir;
    dir.z = 1;
    dir.x = 0.5f;

    pos.x = 0;
}

void Engine::init_porte()
{
    static TCHAR strFileName[MAX_PATH] = TEXT("data/sound/Door5.wav");

    CWaveFile waveFile;

    waveFile.Open(strFileName, NULL, WAVEFILE_READ);
    WAVEFORMATEX *pwfx = waveFile.GetFormat();
    if (pwfx == NULL)
    {
        return;
    }

    if (pwfx->nChannels > 1)
    {
        // Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
        return;
    }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
        // Sound must be PCM when using DSBCAPS_CTRL3D
        return;
    }

    // Load the wave file into a DirectSound buffer
    HRESULT hr = g_pSoundManager->Create(&g_pSporte, strFileName, DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL);
    if (FAILED(hr) || hr == DS_NO_VIRTUALIZATION)
    {
        DXTRACE_ERR_NOMSGBOX(TEXT("Create"), hr);
        return;
    }

    // Get the 3D buffer from the secondary buffer
    if (FAILED(hr = g_pSporte->Get3DBufferInterface(0, &g_pDS3DBuffer)))
    {
        DXTRACE_ERR(TEXT("Get3DBufferInterface"), hr);
        return;
    }
}

HRESULT
Engine::playbackground(DWORD flags)
{
    HRESULT hr;
    if (g_pSound)
    {
        g_pSound->Reset();
        hr = g_pSound->Play(0, flags);
    }

    return hr;
}

HRESULT
Engine::ouvertureporte()
{
    HRESULT hr;
    if (g_pSporte)
    {
        g_pSporte->Reset();
        g_pSporte->Stop();
        hr = g_pSporte->Play(0, 0);
    }

    return hr;
}

static void GlQuad(float x1, float y1, float x2, float y2)
{
    glEnable(GL_BLEND);

    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2f, 0.2f, 0.3f, 0.1f);

    glBegin(GL_QUADS);
    glVertex3f(x1, y1, -0.1f);
    glVertex3f(x2, y1, -0.1f);
    glVertex3f(x2, y2, -0.1f);
    glVertex3f(x1, y2, -0.1f);
    glEnd();

    glColor4f(0.3f, 0.3f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex3f(x1 + 5, y1 + 5, -0.1f);
    glVertex3f(x2 - 5, y1 + 5, -0.1f);
    glVertex3f(x2 - 5, y2 - 5, -0.1f);
    glVertex3f(x1 + 5, y2 - 5, -0.1f);
    glEnd();
}
void Engine::begin_orto()
{
    // save projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, m_width, 0, m_height, -1, 1);

    // save modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
}

void

Engine::end_orto()
{
    glEnable(GL_CULL_FACE);

    // restore projection and modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Engine::display_screen(float delta)
{
    float xd = m_width / 4.0f;
    // float yd =   m_height /4.0f;
    float yd = m_height * 0.25f;

    begin_orto();

    static char c[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ@_.0123456789";

    float lenghto;

    lenghto = m_height / 20.0f;

    static char *message;

    char buffer[200];

    glDisable(GL_DEPTH_TEST);

    //
    glEnable(GL_BLEND);

    //----------------------------------------------------------------------------------
    //				AUTOMATE MENU
    //----------------------------------------------------------------------------------

    //----------------------PAGE PRINCIPALE------------------------------------------------------------
    for (int da = 0; da < lesoptionsbouton.size(); da++)
    {
        lesoptionsbouton[da]->visible = (menu_state == 151);
    }

    for (int da = 0; da < lesbtn.size(); da++)
    {
        if (lesbtn[da])
        {
            if (lesbtn[da]->visible && da != 40) // 11 c l'image de la map pas ici plus loin
                lesbtn[da]->frame(m_input.xpos, m_input.ypos, m_width, m_height, m_input.left_button, config.isdebug,
                                  delta);
        }
        lesbtn[da]->visible = (

            ((da == 1 || da == 25 || da == 2 || da == 43) && menu_state == 1100) //
            || ((da == 25) && menu_state == 2052) ||

            ((da == 41 || da == 25) && menu_state == 444)    //
            || ((da == 42 || da == 25) && menu_state == 555) //
            || (((da == 47) || (da == 3 && m_playerfile->is_autorise) || (da == 4) ||
                 (da == 26 && false && m_playerfile->is_autorise) ||
                 (is_op && ((da == 38 && mode_op == false) || (da == 39 && mode_op == true)))) &&
                menu_state == 0) ||
            ((da == 5 || da == 6 || da == 18 || da == 19 || da == 46 || da == 40) && menu_state == 2) ||
            ((da == 7 || da == 8 || da == 9 || da == 10 || da == 11 || da == 40 || da == 12 || (da == 24 && PASS_ON) ||
              (da == 23 && !PASS_ON) || (da == 20 && CFT_ON) || (da == 21 && TEAM_ON) ||
              (da == 22 && !TEAM_ON && !SNIPER_ON && !CFT_ON) || (da == 35 && FRIENDLY == 0) ||
              (da == 36 && FRIENDLY == 1) || (da == 37 && FRIENDLY == 2) || (da == 48 && SNIPER_ON)) &&
             menu_state == 151) ||
            ((da == 13 || da == 14) && menu_state == 51) || ((da == 15) && (menu_state == 110)) ||
            ((da == 44 || da == 45 || da == 25) && menu_state == 990) || ((da == 16 || da == 17) && menu_state == 64) ||
            ((da == 27 || (da == 28 && curr_tournois < tournoisv.size() - 1) || (da == 29 && curr_tournois > 0) ||
              (da == 30 && tournoisv.size() > 0)) &&
             menu_state == 351) ||
            ((da == 31 || da == 32) && menu_state == 353) ||
            ((da == 4 || da == 41) && menu_state == 2565) // confirmation logout
        );
    }

    if (false) // config.isdebug
    {
        char men[12];
        sprintf(men, "menu state :%i", menu_state);
        lesconsoles[12]->SetText(men);
    }


    if ((menu_state == 51))
    {
        char str[200]; // vs2005 debug
        sprintf(str, "MAP : %s*attente joueurs ...*joueurs : %i", m_cur_map, g_lNumberOfActivePlayers);
        lesconsoles[12]->SetText(str);

        if ((lesbtn[14]->m_mouseclick))
        {
            // quit
            if (!lan_mode)
                menu_state = 1;
            else
                menu_state = 990;

            if (m_playerfile)
            {
                if (!lan_mode)
                    m_xmlsession.QuitServer(m_playerfile, idpartie, killer, lejoueur[VRAI]->killed);

            }

            m_pDP->Close(0);

            strcpy(lesinput_box[5]->text.text, "");

            world.cleanup();
            if (m_playerfile && !lan_mode)
                m_xmlsession.DeleteServer(m_serverid, m_playerfile);
        }
        if (m_input.keys[VK_RETURN] || lesbtn[13]->m_mouseclick)
        { // quit
            GlQuad(xd, yd + 147, xd * 3, yd + 167);

            menu_mode = false;
            if (m_romu)
                m_romu->hide();
            ouvertureporte(); // 05/12/2002
        }
    }
    else if (menu_state == 110)
    { // join session  enum

        if (m_input.xpos > xd && lesbtn[15]->m_mouseclick)
        { // quit

            if (m_hEnumAsyncOp)
            {
                if (lan_mode)
                    menu_state = 990;
                else
                    menu_state = 1;

                m_pDP->CancelAsyncOperation(m_hEnumAsyncOp, 0);
            }
        }
        SessionsDlgExpireOldHostEnums();

        SessionsDlgDisplayEnumList(xd, yd, m_input.xpos, m_input.ypos);


        char r[300];
        strcpy(r, "Recherche de session*Veuillez patientez...**Entrez un mot de passe*pour les parties privees*");
        lesconsoles[4]->SetText(r);
    }

    else

        if ((menu_state == 10))
    {
        if (m_playerfile)
        {

            m_playerfile->load();

            if (m_playerfile->player_id == 0)
            {
                menu_state = 444;
                active_input = 0;
                char bufff[500];
                sprintf(bufff,
                        "Vous etes deja inscris*Saisissez votre pseudo*et votre mot de passe.*Sinon allez vous "
                        "inscrire sur*%s",
                        m_xmlsession.GServerName);

                lesconsoles.at(2)->SetText(bufff);
                prev = GetTickCount();
            }
            else
            {
                if (S_OK == m_xmlsession.InfoJoueur(m_playerfile, config.server_tchat, MAC))
                {
                    is_op = m_playerfile->is_op;

                    lesconsoles[2]->SetText(m_playerfile->panel_joueur);
                    lesconsoles[16]->SetText(m_playerfile->panel_stats);

                    menu_state = 2000;
                    prev = GetTickCount();
                    if (m_romu)
                    {
                        strcpy(m_romu->Message, m_playerfile->scroll);
                        m_romu->init();
                    }
                    if (strlen(config.server_tchat) > 0)
                    {
                        if (!le_socket)
                        {
                            if (sockopen(&le_socket, config.server_tchat, 2001) == 0)
                            {
                                char tr[200];
                                sprintf(tr, "Connexion au serveur RomuChat %s Ok", config.server_tchat);
                                m_chat->addtext("Connexion au serveur RomuChat ", 2);
                            }
                            else
                            {
                                m_chat->addtext("Connexion au serveur RomuChat impossible", 2);
                            }
                        }
                    }
                }
                else
                {
                    menu_state = 1100;
                    m_chat->addtext("Erreur: login incorrecte ou fichiers modifies", 2);
                }
            }
        }
        else
        {
            menu_state = 4;
            prev = GetTickCount();
            if (m_romu)
                m_romu->init();
        }
    }
    else if (menu_state == 11)
    {

        //			Joueur_Creation ("21212",4544455454);
        menu_state = 10;
        lesconsoles[12]->SetText("RomuStrike -chargement-");

        if (m_romu)
            m_romu->init();
    }
    else if (menu_state == 2565)
    {
        if (lesbtn[4]->m_mouseclick)
        {
            menu_state = 11;
        }
        if (lesbtn[41]->m_mouseclick)
        {
            m_playerfile->unset();
            m_chat->addtext("Compte joueur supprime", 2);
            lesconsoles[2]->SetText("Choisissez*un mode de connection*");
            m_playerfile->player_id = 0;
            menu_state = 1100;
        }
    }

    else if (menu_state == 0)
    {
        cle_tournois = 0;
        curr_tournois = 0;
        round = 0;

        if (config.isdebug)
        {
            menu_state = 9;
        }

        if ((lesbtn[38]->m_mouseclick))
        { // quit
            mode_op = true;
        }
        if ((lesbtn[39]->m_mouseclick))
        { // quit
            mode_op = false;
        }
        if ((lesbtn[47]->m_mouseclick))
        { // quit
            menu_state = 2565;
            lesconsoles[2]->SetText("Confirmez vous la suppression *de votre compte joueur?");
        }

        if (lesbtn[4]->m_mouseclick)
        {
            menu_state = -1;
        }
        else if (m_playerfile->is_autorise && (m_input.keys[VK_RETURN] || lesbtn[3]->m_mouseclick))
        {
            menu_state = 9;
        }
        if (lesbtn[26]->m_mouseclick)
        {
            menu_state = 350;
        }
    }
    else if (menu_state == 1100)
    {
        if (lesbtn[25]->m_mouseclick)
        {
            menu_state = -1;
        }
        else if (m_input.keys[VK_RETURN] || lesbtn.at(1)->m_mouseclick == true)
        {
            bool e = lesbtn.at(1)->m_mouseclick;
            menu_state = 11;
        }

        if (lesbtn.at(2)->m_mouseclick == true)
        {
            menu_state = 444;
            active_input = 0;
            lesconsoles.at(2)->SetText("Vous etes deja inscris*identifiez vous*");
        }

        if (lesbtn[43]->m_mouseclick)
        {
            active_input = 7;
            lesconsoles.at(2)->SetText("Jeu reseau TCP IP*");

            menu_state = 990;
            lan_mode = true;
        }
    }
    else if (menu_state == 990)
    {

        if (lesbtn[44]->m_mouseclick) // rejoindre
        {
            active_input = 0;
            strcpy(m_playerfile->player_name, lesinput_box[7]->text.text);

            HRESULT hr;
            if (FAILED(hr = m_pDP->Initialize(NULL, dphdl, 0)))
            {
                menu_state = 990;
                m_chat->addtext("dplay Initialize error", 2);
            }
            else
            {
                menu_state = 110;
                m_chat->addtext("rejoindre un serveur", 2);
                ConnectionsDlgOnOK();
            }

            hr = m_pHostAddress->SetDevice(&ptcpGuid);
            WCHAR whostname[20];

            DXUtil_ConvertGenericStringToWide(whostname, lesinput_box[8]->text.text); // lists.list[i+offset-1].ip

            hr = m_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, whostname, (wcslen(whostname) + 1) * sizeof(WCHAR),
                                              DPNA_DATATYPE_STRING);

            active_input = 5;

            if (FAILED(hr = SessionsDlgEnumHosts()))
            {
                m_chat->addtext("SessionsDlgEnumHosts error", 2);
                menu_state = 990;
            }
        }
        if (lesbtn[45]->m_mouseclick) // creer
        {
            active_input = 0;
            strcpy(m_playerfile->player_name, lesinput_box[7]->text.text);
            menu_state = 150;
            HRESULT hr = m_pDP->Initialize(NULL, dphdl, 0);
            ConnectionsDlgOnOK();
            m_chat->addtext("creation partie", 2);
        }
        if (lesbtn[25]->m_mouseclick) // quit
        {
            // active_input=0;
            menu_state = 1100;
            lan_mode = false;
        }
    }
    else if (menu_state == 444)
    {
        if (lesbtn[25]->m_mouseclick)
        {
            menu_state = 1100;
            lesconsoles[2]->SetText("Choisissez*un mode de connection*");
        }

        if (m_input.keys[VK_RETURN] || lesbtn.at(41)->m_mouseclick == true)
        {
            menu_state = 445;
            active_input = 0;
        }
    }
    else if (menu_state == 445)
    {
        char pwd[50];
        int cle_joueur;

        cle_joueur = m_xmlsession.Get_Id(lesinput_box[0]->text.text, lesinput_box[1]->text.text);
        if (cle_joueur == 0)
        {
            menu_state = 444;
            m_chat->addtext("COMPTE JOUEUR INCONNU", 2);
        }
        else
        {
            m_chat->addtext("LOGIN OK", 2);
            m_playerfile->player_id = cle_joueur;
            strcpy(m_playerfile->player_pwd, lesinput_box[1]->text.text);
            m_playerfile->save();
            menu_state = 10;
        }
    }
    else if ((menu_state == 2001) && ((m_playerfile)))
    {

        if (mp3)
        {
            mp3->stop();
            if (config.music_on)
                mp3->load(m_playerfile->player_mp3);
        }
        menu_state = 0;
    }
    else if (menu_state == 2051)
    {
        if (Verify_Map(m_cur_map))
        {
            menu_state = 2052;
            lesconsoles[12]->SetText("Loading MAP ...");
        }
        else
        {
            menu_state = 2053;
            lesconsoles[12]->SetText("Downloading MAP ...");
        }
    }
    else if ((menu_state == 2052) && (downloading == INET_IDLE))
    {
        char chemin[100];
        sprintf(chemin, "data/map/%s.wad", m_cur_map);
        md5.GetMD5_new(chemin, cur_md5);
        char cur_md5_bsp[40];

        sprintf(chemin, "data/map/%s.bsp", m_cur_map);
        md5.GetMD5_new(chemin, cur_md5_bsp);

        if (!(!config.isdebug && !lan_mode && strlen(MapList.List[curmapid].wad_md5) > 0 &&
              (strcmp(cur_md5, MapList.List[curmapid].wad_md5) || strcmp(cur_md5_bsp, MapList.List[curmapid].bsp_md5))))
        {
            if (Verify_Map(m_cur_map))
            {
                load_map(m_cur_map);
                world.AffecteGammaSurUneFace(config.GAMMA);

                SCOPE_ETA = false;
                char ch[200];
                sprintf(ch, "Create session ... MAP %s", m_cur_map);

                lesconsoles[12]->SetText(ch);
                // HRESULT hr = g_pDP->Initialize(NULL, DirectPlayMessageHandler, 0 );
                if (SessionsDlgCreateGame() == S_OK)
                {
                    menu_state = 51;
                }
                else
                {

                    menu_state = 151;
                    active_input = 6;
                }
            }
            else
            {
                menu_state = 151;
                active_input = 6;
            }
        }
        else
        {
            char buf[300];
            sprintf(buf, "ERREUR MAP %s INVALIDE", m_cur_map);
            m_chat->addtext(buf, 2);

            menu_state = 151;
            active_input = 6;
        }
    }
    else if (menu_state == 2053 && (downloading == INET_IDLE))
    {
        // Download_Map(m_cur_host,m_cur_map);
        sprintf(distant_file, "romustrike/map/%s.exe", m_cur_map);
        sprintf(local_file, "%s.exe", m_cur_map);
        type_download = 0;

        downloading = INET_CONNECT;

        menu_state = 2052;

        lesconsoles[12]->SetText("Connection serveur");
    }
    else if (menu_state == 2000)
    { // start
        if (config.isdebug)
        {
            menu_state = 0;
        }
        else
        {

            if ((m_playerfile) && (mp3))
            {
                m_xmlsession.Get_Mp3(&Mp3List, m_playerfile);
                curmp3id = 0;
                for (int u = 0; u < Mp3List.count; u++)
                {
                    if ((m_playerfile->player_mp3id == Mp3List.List[u].id))
                    {
                        curmp3id = u;
                    }
                }

                if (Verify_Mp3(Mp3List.List[curmp3id].Name))
                {

                    char buf[300];
                    sprintf(buf, "%s %s", Mp3List.List[curmp3id].Name, Mp3List.List[curmp3id].mappeur);
                    m_chat->addtext(buf, 2);
                    mp3->stop();
                    if (config.music_on)
                        mp3->load(Mp3List.List[curmp3id].Name);
                }
                else
                {

                    sprintf(distant_file, "romustrike/mp3/%s.mp3", Mp3List.List[curmp3id].Name);
                    sprintf(local_file, "data/mp3/%s.mp3", Mp3List.List[curmp3id].Name);
                    sprintf(m_cur_host, "%s", Mp3List.List[curmp3id].host);
                    downloading = INET_CONNECT;
                    type_download = 1;

                    // menu_state=2052;

                    char dow[100];
                    sprintf(dow, "Downloading %s", m_playerfile->player_mp3);
                    lesconsoles[12]->SetText(dow);
                }

                menu_state = 0;
            }
            else
            {
                menu_state = 0;
            }
        }
    }
    else
        //----------------------TRANSITION--PAGE PRINCIPALE-->---LISTE
        //SERVEURS-------------------------------------------------------
        if (menu_state == 1)
        {

            if (m_playerfile)

            {
                // deux context soit en tournois soit pas
                // round et cle_tponois

                if (m_xmlsession.ServerEnCours(&lists, m_playerfile, cle_tournois, round) == S_OK)
                {

                    HRESULT hr = m_pDP->Initialize(NULL, dphdl, 0);
                    ConnectionsDlgOnOK();

                    menu_state = 2;
                    offset = 0;
                    if (m_romu)
                        m_romu->init();
                }
                else
                {
                    menu_state = 2000;
                    if (m_romu)
                        m_romu->init();
                }
            }
        }
        else
            //----------------------LISTE SERVEUR------------------------------------------------------------
            if (menu_state == 2)
            {
                if (lesbtn[46]->m_mouseclick)
                {
                    menu_state = 1;
                    m_chat->addtext("Refresh list", 2);
                }

                if (lesbtn[6]->m_mouseclick)
                {
                    if (config.isdebug)
                        menu_state = -1;
                    else
                        menu_state = 10;
                }
                else if (m_input.keys[VK_RETURN] || lesbtn[5]->m_mouseclick)
                {
                    lesconsoles[12]->SetText("LOADING MAP ...");

                    if (round == 0)
                        menu_state = 150;
                    else
                    {

                        sprintf(m_cur_map, "%s", tournoisv.at(curr_tournois).map);
                        sprintf(m_cur_host, "%s", config.server_xml);
                        menu_state = 2051; // existance de la map
                    }
                }

                float h;
                float x1, y1, w1, h1;

                int nb_ligne;
                nb_ligne = 12;

                h = (yd * 3 - yd) / (2 * 9);
                x1 = xd * 0.6f;
                y1 = yd * 2.6f;
                w1 = xd * 3 - x1;

                float pos_y_mouse;
                pos_y_mouse = m_height - m_input.ypos;

                if (lesbtn[19]->m_mouseclick && offset + 5 < lists.count)
                    offset++;
                if (lesbtn[18]->m_mouseclick && offset > 0)
                    offset--;
                bool dessus;

                for (int i = 1; i < nb_ligne; i++)
                {
                    dessus = false;
                    if (m_input.xpos > xd && m_input.xpos < xd * 3.4 && pos_y_mouse > y1 - (i + 1) * h &&
                        pos_y_mouse < y1 - (i)*h)
                    { // play

                        if (((i - 1 + offset) < lists.count))
                        {
                            char re[300];
                            sprintf(re, "Regles: %s", lists.list[i + offset - 1].regles);
                            lesconsoles[15]->SetText(re);
                            dessus = true;
                        }

                        if (m_input.left_button && ((i - 1 + offset) < lists.count))
                        {

                            HRESULT hr;
                            hr = m_pHostAddress->SetDevice(&ptcpGuid);
                            WCHAR whostname[20];

                            DXUtil_ConvertGenericStringToWide(
                                whostname, lists.list[i + offset - 1].ip); // lists.list[i+offset-1].ip

                            hr = m_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, whostname,
                                                              (wcslen(whostname) + 1) * sizeof(WCHAR),
                                                              DPNA_DATATYPE_STRING);

                            menu_state = 110;
                            active_input = 5;
                            if (FAILED(hr = SessionsDlgEnumHosts()))
                            {
                                DXTRACE_ERR(TEXT("SessionsDlgEnumHosts"), hr);
                            }

                            strcpy(lesinput_box[5]->text.text, "");
                            strcpy(cur_md5, lists.list[i + offset - 1].MD5);
                            m_serverid = lists.list[i + offset - 1].serverid;
                            char re[200];
                            sprintf(re, "Regles de la partie:*%s", lists.list[i + offset - 1].regles);
                            lesconsoles[2]->SetText(re);

                            lesconsoles[12]->SetText("List of Sessions");
                        }
                    }

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor3f(1.0f, 1.0f, 1.0f);

                    if ((i - 1 + offset) < lists.count)
                    {
                        char dess[5];
                        if (dessus)
                        {
                            sprintf(dess, " ");
                            char topt[50];
                            sprintf(topt, "%s.bmp", lists.list[i + offset - 1].la_map);

                            if (strcmp(topt, lesbtn[40]->file))
                            {

                                if (Verify_Bmp(lists.list[i + offset - 1].la_map))
                                {
                                    char rr[100];
                                    sprintf(rr, "%s.bmp", lists.list[i + offset - 1].la_map);
                                    lesbtn[40]->SetMap(rr);
                                }
                                else
                                {
                                    lesbtn[40]->SetMap("");
                                }
                            }
                        }
                        else
                            sprintf(dess, "");

                        sprintf(buffer, "%s%s-%s", dess, lists.list[i - 1 + offset].comment,
                                lists.list[i - 1 + offset].playername);

                        m_font->print(x1, y1 - i * h, buffer);
                    }
                }
            }
            else if (menu_state == 350)
            {
                tournoisv.clear();
                curr_tournois = 0;

                if (m_xmlsession.GetTournois(&tournoisv, m_playerfile) == S_OK)
                {
                    menu_state = 351;
                }
                else
                {
                    menu_state = 1;
                }
                // mode tournois
            }
    if (menu_state == 351)
    {
        if (lesbtn[27]->m_mouseclick)
        {
            menu_state = 0;
        }
        if (curr_tournois < tournoisv.size() && curr_tournois >= 0)
        {
            glColor3f(0.0f, 0.5f, 1.0f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            char bufft[200];
            m_font->print(50, yd * 2, "Numero :");
            m_font->print(50, yd * 2 - 30, "Round :");
            m_font->print(50, yd * 2 - 60, "Map:");
            m_font->print(50, yd * 2 - 90, "Description:");
            m_font->print(50, yd * 2 - 120, "Duree:");
            char buff[100];
            sprintf(buff, "%d min %d sec\0", (tournoisv.at(curr_tournois).timeout) / 60,
                    (tournoisv.at(curr_tournois).timeout) % 60);

            glColor3f(1.0f, 0.5f, 0.0f);
            sprintf(bufft, "#%i", tournoisv.at(curr_tournois).cle_tournois);
            m_font->print(xd + 100, yd * 2, bufft);
            sprintf(bufft, "%i", tournoisv.at(curr_tournois).cle_noeud);
            m_font->print(xd + 100, yd * 2 - 30, bufft);

            m_font->print(xd + 100, yd * 2 - 60, tournoisv.at(curr_tournois).map);
            m_font->print(xd + 100, yd * 2 - 90, tournoisv.at(curr_tournois).desc);
            m_font->print(xd + 100, yd * 2 - 120, buff);

            cle_tournois = tournoisv.at(curr_tournois).cle_tournois;
            round = tournoisv.at(curr_tournois).cle_noeud;
            TEAM_ON = false;
            CFT_ON = false;
            FRIENDLY = 1;
        }
        if (lesbtn[28]->m_mouseclick && curr_tournois < tournoisv.size() - 1)
        {
            curr_tournois++;
        }
        if (lesbtn[29]->m_mouseclick && curr_tournois > 0)
        {
            curr_tournois--;
        }

        if (tournoisv.size() == 0)
        {
            lesconsoles[12]->SetText("pas de tournois programme*allez sur le site de RS*pour vous inscrire");
        }
        if (lesbtn[30]->m_mouseclick)
        {
            menu_state = 352;
        }
    }
    else if (menu_state == 352)
    {
        // tournois visu
        lesekip.clear();
        // curr_tournois=0;

        if (m_xmlsession.GetTournoisDetail(&lesekip, m_playerfile, tournoisv.at(curr_tournois).cle_noeud,
                                           tournoisv.at(curr_tournois).cle_tournois) == S_OK)
        {
            menu_state = 353;
        }
        else
        {
            menu_state = 351;
        }
    }
    else if (menu_state == 353)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int o = 0; o < lesekip.size(); o++)
        {
            glColor3f(1.0f, 0.0f, 1.0f);
            m_font->print(50 + 350 * o, yd * 2 + 30, lesekip.at(o).nom);
            for (int oa = 0; oa < lesekip.at(o).list.size(); oa++)
            {
                glColor3f(1.0f, 1.0f, 0.0f);
                m_font->print(50 + 350 * o, yd * 2 - 30 * oa, lesekip.at(o).list.at(oa).login);
            }
        }
        if (lesbtn[31]->m_mouseclick)
        {
            menu_state = 1;
        }

        if (lesbtn[32]->m_mouseclick)
        {
            menu_state = 351;
        }
    }
    else

        //----------------------TRANSITION--LISTE
        //SERVEURS--->-PRINCIPALE---------------------------------------------------
        if (menu_state == 9)
        {
            if (m_playerfile)
            {
                if (m_playerfile->player_id == 0)
                { // pas enregistré
                    // aller enregistrement
                    menu_state = 4;
                    if (m_romu)
                        m_romu->init();
                }
                else
                {
                    glColor3f(0.0f, 0.5f, 1.0f);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    m_font->print(xd + 4, yd * 3 - 18, "Connecting");
                    // po ssede id
                    menu_state = 1;
                    if (m_romu)
                        m_romu->init();
                }
            }
        }
        else if (menu_state == 99)
        {
            glColor3f(1.0f, 1.0f, 0.0f);

            glDisable(GL_BLEND);
            //
            glColor4f(0.0f, 0.3f, 1.0f, 0.5f);

            if (m_input.xpos > xd && m_input.xpos < xd * 3 && m_input.ypos > yd * 3 - 60 && m_input.ypos < yd * 3 - 30)
            { // quit
                GlQuad(xd, yd + 47, xd * 3, yd + 67);

                if (m_input.left_button)
                {
                    menu_state = 11;
                    if (m_romu)
                        m_romu->init();
                }
            }
            else if (m_input.xpos > xd && m_input.xpos < xd * 3 && m_input.ypos > yd * 3 - 160 &&
                     m_input.ypos < yd * 3 - 130)
            { // quit
                GlQuad(xd, yd + 147, xd * 3, yd + 167);

                if (m_input.left_button)
                {
                    menu_state = -1;
                    if (m_romu)
                        m_romu->init();
                }
            }

            glEnable(GL_BLEND);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor3f(1.0f, 1.0f, 0.0f);

            m_font->print(xd + 4, yd * 3 - 18, "RomuStrike");
            char s[100];
            sprintf(s, "%s", m_xmlsession.GServerName);
            m_font->print(xd + 4, yd * 3 - 58, "connection impossible");
            m_font->print(xd + 4, yd * 3 - 70, s);

            m_font->print(xd + 4, yd + 50, "RESSAYER");
            m_font->print(xd + 4, yd + 150, "QUIT");
        }
        else

            //----------------------TRANSITION--LISTE
            //SERVEURS--->-PRINCIPALE---------------------------------------------------
            if (menu_state == 3)
            {
                if (!m_input.left_button)
                {

                    menu_state = 2000;
                    if (m_romu)
                        m_romu->init();
                }
                else
                {
                    if (config.isdebug)
                    {
                        menu_state = -1;
                    }
                }
            }

            else if (menu_state == 555)
            {
                if (lesbtn[25]->m_mouseclick)
                {
                    menu_state = 1100;
                }

                if (m_input.keys[VK_RETURN] || lesbtn.at(42)->m_mouseclick == true)
                {
                    active_input = 2;

                    menu_state = 556;
                }
            }
            else if (menu_state == 556)
            {

                bool pseudo_correct = (strlen(lesinput_box[2]->text.text) > 0);
                bool pwd_correct = (strlen(lesinput_box[3]->text.text) > 4);
                bool email_correct =
                    (strlen(lesinput_box[4]->text.text) > 10) && (strcspn(lesinput_box[4]->text.text, "@") > 0);

                if (pseudo_correct && email_correct && pwd_correct)
                {
                    strcpy(m_playerfile->player_name, lesinput_box[2]->text.text);

                    strcpy(m_playerfile->player_pwd, lesinput_box[3]->text.text);

                    strcpy(m_playerfile->playermail, lesinput_box[4]->text.text);

                    menu_state = 8;
                }
                else
                {

                    if (!pseudo_correct)
                        m_chat->addtext("Pseudo trop petit", 2);
                    if (!pwd_correct)
                        m_chat->addtext("Mot de passe trop petit", 2);
                    if (!email_correct)
                        m_chat->addtext("Email incorrect", 2);

                    active_input = 2;

                    menu_state = 555;
                }

                if (m_romu)
                    m_romu->init();
            }
            else
                //----------------------TRANSITION--LISTE
                //SERVEURS--->-PRINCIPALE---------------------------------------------------
                if (menu_state == 5)
                {
                    if (!m_input.left_button)
                    {

                        menu_state = 2000;
                        if (m_romu)
                            m_romu->init();
                    }
                }

                else if (menu_state == 8)
                { // sortie de new player

                    if (!m_input.left_button)
                    {
                        if (strlen(m_playerfile->player_name) == 0)
                        {
                            menu_state = 555;
                            active_input = 2;

                            m_chat->addtext("Premier connection. CREATION DU COMPTE JOUEUR.", 2);
                            if (m_romu)
                                m_romu->init();
                        }
                        else
                        {

                            if (m_playerfile)
                            {
                                char error_message[100];

                                m_playerfile->player_id =
                                    m_xmlsession.NewPlayer(m_playerfile->player_name, m_playerfile->playermail,
                                                           m_playerfile->player_pwd, error_message);
                                if (m_playerfile->player_id != 0)
                                {
                                    m_playerfile->save();

                                    menu_state = 10;
                                    if (m_romu)
                                        m_romu->init();
                                }
                                else
                                {
                                    menu_state = 555;
                                    m_chat->addtext(error_message, 2);
                                    if (m_romu)
                                        m_romu->init();
                                }
                            }
                        }
                    }
                }
                else if (menu_state == 150)
                { // sortie de new player
                    if (m_playerfile)
                    {
                        if (!lan_mode)
                            m_xmlsession.GetMap(&MapList, m_playerfile);
                        else
                        {

                            m_xmlsession.mode(true);
                            m_xmlsession.GetMap(&MapList, m_playerfile);
                            m_xmlsession.mode(config.isdebug);
                        }

                        curmapid = 0;
                        if (m_romu)
                        {
                            if (MapList.count > 0)
                            {
                                if (!config.isdebug)
                                {
                                    sprintf(m_romu->Message,
                                            "************************************************MERCI AUX MAPPEURS : %s - "
                                            "%s****************************************************",
                                            MapList.List[curmapid].Name, MapList.List[curmapid].mappeur);
                                    m_romu->scroll = 0.0f;
                                }
                                char topt[50];
                                sprintf(topt, "%s.bmp", MapList.List[curmapid].Name);

                                if (strcmp(topt, lesbtn[40]->file))
                                {
                                    if (Verify_Bmp(MapList.List[curmapid].Name))
                                    {
                                        char rr[100];
                                        sprintf(rr, "%s.bmp", MapList.List[curmapid].Name);
                                        lesbtn[40]->SetMap(rr);
                                        lesbtn[40]->tps_vie = 0.0f;
                                    }
                                }
                            }
                        }
                    }

                    menu_state = 151;
                    active_input = 6;
                }
                else if (menu_state == 151)
                { // sortie de new player

                    char str[100];

                    if (m_dwMaxPlayers != 0)
                        sprintf(str, "nb de joueurs: %i", m_dwMaxPlayers);
                    else
                        sprintf(str, "nb de joueurs: illimite");

                    lesconsoles[5]->SetText(str);

                    if (PASS_ON)
                    {
                        // lesconsoles[11]->SetText(m_password);
                        lesinput_box[5]->visible = true;
                    }
                    else
                        lesinput_box[5]->visible = false;

                    glColor4f(0.0f, 0.3f, 1.0f, 0.5f);

                    if (m_input.keys[VK_RETURN] || lesbtn[9]->m_mouseclick || lesbtn[40]->m_mouseclick)
                    { // Play

                        char re[500];
                        sprintf(re, "Regles :*%s", lesinput_box[6]->text.text);
                        lesconsoles[2]->SetText(re);
                        menu_state = 50;
                    }

                    if (curmapid + 1 < MapList.count)
                    {

                        if (lesbtn[11]->m_mouseclick)
                        { // Play

                            curmapid++;
                            // menu_state=121;
                            if (m_romu)
                                m_romu->init();
                        }
                    }
                    if (curmapid > 0)
                    {

                        if (lesbtn[12]->m_mouseclick)
                        {
                            curmapid--;
                            if (m_romu)
                                m_romu->init();
                        }
                    }
                    if (MapList.count > 0)
                    {
                        char topt[50];
                        sprintf(topt, "%s.bmp", MapList.List[curmapid].Name);
                        char str[100];

                        sprintf(str, "map :*%s", MapList.List[curmapid].Name);

                        lesconsoles[9]->SetText(str);

                        if (strcmp(topt, lesbtn[40]->file))
                        {

                            if (Verify_Bmp(MapList.List[curmapid].Name))
                            {
                                char rr[100];
                                sprintf(rr, "%s.bmp", MapList.List[curmapid].Name);
                                lesbtn[40]->SetMap(rr);
                                lesbtn[40]->tps_vie = 0.0f;
                            }
                            else
                            {
                                lesbtn[40]->SetMap("");
                            }
                        }
                    }

                    if (lesbtn[10]->m_mouseclick)
                    {
                        if (!lan_mode)
                            menu_state = 1;
                        else
                            menu_state = 990;

                        HRESULT hr = m_pDP->TerminateSession(NULL, 0, 0);
                    }

                    if (lesbtn[7]->m_mouseclick)
                    { // Play

                        if (m_dwMaxPlayers < 10)
                            m_dwMaxPlayers = m_dwMaxPlayers + 2;
                    }

                    if (lesbtn[22]->m_mouseclick)
                    {
                        CFT_ON = true;
                        TEAM_ON = false;
                        // SNIPER_ON=false;
                    }
                    if (lesbtn[20]->m_mouseclick)
                    {
                        CFT_ON = false;
                        TEAM_ON = true;
                        //				SNIPER_ON=false;
                    }
                    if (lesbtn[21]->m_mouseclick)
                    {
                        CFT_ON = false;
                        TEAM_ON = false;
                        //				SNIPER_ON=true;
                    }
                    /*
                    if(lesbtn[48]->m_mouseclick )
                    {
                        CFT_ON=false;
                        TEAM_ON=false;
        //				SNIPER_ON=false;
                    }
                    */

                    if (lesbtn[23]->m_mouseclick)
                        PASS_ON = true;

                    if (lesbtn[24]->m_mouseclick)
                    {
                        PASS_ON = false;
                        lesinput_box[5]->SetText("");

                        lesconsoles[10]->tps_vie = 0.0f;
                    }
                    if (lesbtn[35]->m_mouseclick)
                        FRIENDLY = 1;
                    if (lesbtn[36]->m_mouseclick)
                        FRIENDLY = 2;
                    if (lesbtn[37]->m_mouseclick)
                        FRIENDLY = 0;

                    if (lesbtn[8]->m_mouseclick)
                    { // Play

                        if (m_dwMaxPlayers > 0)
                            m_dwMaxPlayers = m_dwMaxPlayers - 2;
                    }
                }
                else if (menu_state == 121)
                { // sortie de new player

                    if (!m_input.left_button)
                    {

                        menu_state = 151;
                        active_input = 6;
                        if (m_romu)
                        {
                            sprintf(m_romu->Message,
                                    "************************************************MERCI AUX MAPPEURS : %s - "
                                    "%s****************************************************",
                                    MapList.List[curmapid].Name, MapList.List[curmapid].mappeur);
                            m_romu->scroll = 0.0f;
                        }
                    }
                }

                else

                    if (menu_state == 50)
                { // sortie de new player

                    /*SetSessionName (TEXT("RomuStrike"));
                /*SetPreferredProvider (TEXT("DirectPlay8 TCP/IP Service Provider"));
                SetPlayerName(m_playerfile->player_name  );
                */
                    m_font->print(xd + 4, yd * 3 - 18, "LOADING MAP ...");
                    sprintf(m_cur_map, "%s", MapList.List[curmapid].Name);
                    if (!lan_mode)
                        sprintf(m_cur_host, "%s", MapList.List[curmapid].host);
                    else
                        sprintf(m_cur_host, "%s", ((*this).config).server_xml);
                    menu_state = 2051; // existance de la map
                }

                else if (menu_state == 64)
                { // client en attente de deconnection
                    //	glColor3f(1.0f, 1.0f,0.0f);
                    //	glDisable(GL_BLEND);
                    //

                    glColor4f(0.0f, 0.3f, 1.0f, 0.5f);

                    if ((lesbtn[17]->m_mouseclick) || is_kikked)
                    {
                        m_bConnecting = FALSE;
                        if (lan_mode)
                            menu_state = 990;
                        else
                            menu_state = 1;
                        if (m_playerfile && config.isdebug == false && !lan_mode)
                            m_xmlsession.QuitServer(m_playerfile, idpartie, killer, lejoueur[VRAI]->killed);
                        HRESULT hr = m_pDP->Close(0);
                        world.cleanup();
                        m_chat->addtext("map cleanup", 2);
                        sprintf(lesinput_box[5]->text.text, "");
                    }
                    if (lesbtn[16]->m_mouseclick)
                    {
                        ouvertureporte(); // 05/12/2002
                        menu_mode = false;
                        if (m_romu)
                            m_romu->hide();
                    }
                    char cbuffer[256];
                    sprintf(cbuffer, "Map Serveur: %s*Joueurs :%d", m_cur_map, g_lNumberOfActivePlayers);

                    lesconsoles[12]->SetText(cbuffer);
                    // font->print(xd+4, yd*3-18, pDPHostEnum->pAppDesc.pvApplicationReservedData );
                }

                else if (menu_state == 63)
                { // attente fin click
                    if (Verify_Map(m_cur_map))
                    {
                        menu_state = 264;
                        // m_font->print(xd+4, yd*3-18, "CHARGEMENT MAP ...");
                        lesconsoles[12]->SetText("CHARGEMENT MAP ...");
                    }
                    else
                    {
                        menu_state = 2264;

                        lesconsoles[12]->SetText("TELECHARGEMENT MAP ...");
                    }
                }
                else if (menu_state == 2264 && downloading == INET_IDLE)
                { // donwload map
                    // Download_Map (m_cur_host,m_cur_map);
                    sprintf(distant_file, "romustrike/map/%s.exe", m_cur_map);
                    sprintf(local_file, "%s.exe", m_cur_map);
                    type_download = 0;

                    downloading = INET_CONNECT;
                    m_font->print(xd + 4, yd * 3 - 18, "Connection serveur");

                    menu_state = 264;
                }
                else if (menu_state == 62)
                { // attente fin click
                    char b[500];
                    sprintf(b, "ouverture de session*MAP %s  %i joueurs ", m_cur_map, g_lNumberOfActivePlayers);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_BLEND);

                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix

                    lesconsoles[12]->SetText(b);

                    m_font->print(xd + 4, yd + 50, "RETOUR");
                    if ((m_input.xpos > xd && m_input.xpos < xd * 3 && m_input.ypos > yd * 3 - 60 &&
                         m_input.ypos < yd * 3 - 30))
                    { // quit
                        GlQuad(xd, yd + 47, xd * 3, yd + 67);

                        if (m_input.left_button)
                        {

                            if (m_hEnumAsyncOp)
                            {
                                m_pDP->CancelAsyncOperation(m_hEnumAsyncOp, 0);
                            }
                            m_pDP->Close(0);
                            menu_state = 1;
                        }
                    }
                }
                else if ((menu_state == 264) && (downloading == INET_IDLE))
                { // load e la ma

                    lesconsoles[12]->SetText("LOADING COMPLETE...");
                    char chemin[100];
                    char prevu_md5[100];
                    sprintf(chemin, "data/map/%s.wad", m_cur_map);
                    // strcpy(prevu_md5,md5.GetMD5(chemin));
                    md5.GetMD5_new(chemin, prevu_md5);
                    if (Verify_Map(m_cur_map))
                    {
                        load_map(m_cur_map);
                        world.AffecteGammaSurUneFace(config.GAMMA);

                        SCOPE_ETA = false;

                        /*	vec3_t ret= world.RenvoiePosition(lejoueur[VRAI]->id_modele);
                            m_pivot.move( ret);
                            //if (m_playerfile)
                            //idpartie=m_xmlsession.JoinServer(m_playerfile,m_serverid);

                        */
                        if (!config.isdebug && !lan_mode && (strcmp(prevu_md5, cur_md5)) && false)
                        {
                            is_kikked = true; // pour 134 virer ce commentaire
                            m_chat->addtext("MAP INVALIDE", 2);
                        }
                        init_player(VRAI);

                        menu_state = 64;
                    }
                    else
                    {
                        is_kikked = true; // pour 134 virer ce commentaire
                        m_chat->addtext("probleme lecture du fichier map ", 2);
                    }

                    // else
                }

    if (menu_mode)
        donwload_frame();

    //----------------------TRANSITION--LISTE SERVEURS--->-PRINCIPALE---------------------------------------------------

    //----------------------------------------------------------------------------------
    //				FIN AUTOMATE MENU
    //----------------------------------------------------------------------------------

    // glDisable(GL_BLEND);
    // glDisable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA );
    //  mouse cursor

    /*if((m_input.keys['2']) && !(relkeys['2'])) {
        menu_mode = false;

        reset_mouse();
    }
    relkeys['2']=(m_input.keys['2']);
*/
    end_orto();
}

char *Engine::Left(char *Texte, int Nb_Caract)
{
    static char Resultat[1024 + 1];
    if (Nb_Caract >= 0 && Nb_Caract <= strlen(Texte))
    {
        strncpy(Resultat, Texte, Nb_Caract);
        Resultat[Nb_Caract] = '\0';
    }
    return Resultat;
}
void Engine::download_task_new()
{
    IN_ADDR Adresse;
    SOCKADDR_IN SockAddrIn;
    HOSTENT *Hote;
    WSADATA WSAData;
    int i, j, NbVirg = 0, Recu;

    char Buffer[1024], NomFich[1024], Requete[1100];

    char Chemin[1000];
    char *DossTel = "C:\\";
    bool Arret = false, Pause = false;

    switch (downloading)
    {
    case INET_IDLE:
        break;
    case INET_ERR:
        fclose(output_file);
        downloading = INET_IDLE;
        break;
    case INET_CONNECT:
        // On initialise le socket
        WSAStartup(0x0202, &WSAData);
        Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        Hote = gethostbyname(m_cur_host);
        memcpy(&Adresse, Hote->h_addr, Hote->h_length);
        SockAddrIn.sin_family = AF_INET;
        SockAddrIn.sin_port = htons(80);
        SockAddrIn.sin_addr = Adresse;
        sprintf(Chemin, "http://%s/%s", m_cur_host, distant_file);
        sprintf(NomFich, "%s", local_file);

        // On se connecte
        if (connect(Socket, (SOCKADDR *)&SockAddrIn, sizeof(SockAddrIn)))
        {
            envoi_msg("Connexion au serveur impossible !!!");
            downloading = INET_ERR;
        }

        // On prépare la requête HTTP v1.1 et on l'envoie
        strcpy(Requete, "GET ");
        strcat(Requete, Chemin);
        strcat(Requete, " HTTP/1.1\r\nHost: ");
        strcat(Requete, m_cur_host);
        strcat(Requete, "\r\n\r\n");
        send(Socket, Requete, strlen(Requete), 0);

        // On reçoit la réponse du serveur auquel on s'est connecté
        Recu = recv(Socket, Buffer, sizeof(Buffer), 0);

        // Vérification, si réponse valide => fichier valide et prêt à être téléchargé
        write_count = Buffer + 9;

        // On vérifie s'il y a une redirection
        if (write_count[0] == '3' && write_count[1] == '0' && (write_count[2] == '1' || write_count[2] == '2'))
        {
        }

        // On vérifie si le fichier a été trouvé
        if (write_count[0] != '2' || write_count[1] != '0' || write_count[2] != '0')
        {
            char bu[500];
            sprintf(bu, "download : URL invalide %s ", Requete);
            envoi_msg(bu);
            downloading = INET_ERR;
        }

        // On récupère la taille du fichier
        if ((write_count = strstr(Buffer, "Content-Length: ")) == NULL)
        {
            envoi_msg("URL invalide !!!\nAucune taille de fichier n'est spécifiée dans la réponse du serveur !");
            downloading = INET_ERR;
        }
        write_count += 16;
        char *ee;
        if ((ee = strstr(write_count, "\r\n")) == NULL)
        {
            envoi_msg("URL invalide !!!\nRéponse du serveur incorrecte !");
            downloading = INET_ERR;
        }
        m_dwTotalSize = _atoi64(Left(write_count, ee - write_count));

        // On place le pointeur au niveau des données binaires du fichier que l'on télécharge
        if ((write_count = strstr(Buffer, "\r\n\r\n")) == NULL)
        {
            envoi_msg("URL invalide !!!\nRéponse du serveur incorrecte !");
            downloading = INET_ERR;
        }
        write_count += 4;

        /*
        if(FichExist(NomFich))
        {
            envoi_msg("Le fichier que vous allez télécharger existe déjà à l'emplacement spécifié.\nSouhaitez-vous
        l'écraser ?"); goto Sortir;
        }
*/
        // On ouvre (ou crée) le fichier en local
        output_file = fopen(NomFich, "wb");
        if (!output_file)
        {
            envoi_msg("Fichier local impossible à créer !");
            downloading = INET_ERR;
        }
        fwrite(write_count, sizeof(char), Recu - (write_count - Buffer), output_file);
        total_down = Recu - (write_count - Buffer);

        downloading = INET_CHUNK;
        break;
    case INET_CHUNK:

        // TempsTel = 0;

        //	envoi_msg("Téléchargemant en cour");

        // On lance le timer de calcul de la progression du téléchargement + débit moyen
        // SetTimer(FenetrehWnd, 1, 1000, (TIMERPROC)NULL);

        // On commence à écrire dans le fichier et télécharger la suite, jusqu'à la fin ou arrêt du téléchargement
        Recu = 1;
        bool encours;
        encours = !Arret && Recu && total_down != m_dwTotalSize;

        if (encours)
        {
            Recu = recv(Socket, Buffer, sizeof(Buffer), 0);
            fwrite(Buffer, sizeof(char), Recu, output_file);
            total_down += Recu;
        }

        if (!encours)
            downloading = INET_DISCONNECT;
        break;

    case INET_DISCONNECT:
        fclose(output_file);

        closesocket(Socket);

        WSACleanup();

        downloading = INET_IDLE; // ok fini
        //
        if (type_download == 0)
        {
            if (!dezip(local_file))
            {
                downloading = INET_ERR;
            }
        }
        else
        {

            char buf[300];
            sprintf(buf, "%s %s", Mp3List.List[curmp3id].Name, Mp3List.List[curmp3id].mappeur);
            m_chat->addtext(buf, 2);

            mp3->stop();
            if (config.music_on)
                mp3->load(Mp3List.List[curmp3id].Name);
        }

        break;
    }
}

void Engine::download_task()
{
    if (downloading != INET_IDLE)
    {
        //		 begin_orto();
        glColor3f(0.6f, 0.5f, 0.5f);

        if (downloading == INET_ERR)
        {

            //	// o << "impossible de downloader la map" << endl;
            if (output_file)
            {
                fclose(output_file);
            }

            menu_state = -1;
            downloading = INET_IDLE;
        }

        if (downloading == INET_CONNECT)
        {

            //	// o << "inet_connect" << endl;

            LPCTSTR lpctstr[2] = {"*/*", NULL};

            //	cout << "url is : " << url<<endl;

            /******************/
            /* OPEN INTERNET */
            /******************/

            hOpen = WinHttpOpen(L"InetURL/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS, 0);

            //		'InternetOpen("HTTP-GET",LOCAL_INTERNET_ACCESS,NULL,0,0);

            if (!hOpen)
            {
                //	// o << GetLastError() << endl;

                //	m_xmlsession.ErrorOut ( GetLastError(), "InternetOpen");
                //	MessageBox (m_hwnd,GetLastError(),TEXT("InternetOpen"),MB_OK);
                downloading = INET_ERR;
            }
            else
            {

                /****************************/
                /* BREAK OUT URL COMPONENTS */
                /****************************/
                //	// o << "connect" << endl;
                port = 80;

                WCHAR lphost[100];
                DXUtil_ConvertAnsiStringToWide(lphost, m_cur_host);
                hConnect = WinHttpConnect(hOpen, (LPCWSTR)lphost, INTERNET_DEFAULT_HTTP_PORT, NULL); //(LPCWSTR) lphost
                // hConnect = InternetConnect(hOpen,m_cur_host,port,"","",
                //  INTERNET_SERVICE_HTTP,0,0);
                if (!hConnect)
                {
                    // ErrorOut (GetLastError(), "InternetConnect");
                    downloading = INET_ERR;
                }
                else
                {
                    //// o << "HTTP REQUEST" << endl;

                    /*********************/
                    /* OPEN HTTP REQUEST */
                    /*********************/

                    WCHAR lp_distant_file[100];
                    DXUtil_ConvertAnsiStringToWide(lp_distant_file, distant_file);
                    //
                    hReq = WinHttpOpenRequest(hConnect, L"GET", (LPCWSTR)lp_distant_file, NULL, WINHTTP_NO_REFERER,
                                              WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

                    //	hReq = HttpOpenRequest(hConnect,"GET",distant_file,HTTP_VERSION,"",lpctstr,
                    //															flags,0);
                    if (!hReq)
                    {
                        //		// o << GetLastError() << endl;

                        //	ErrorOut (GetLastError(), "HttpOpenRequest");
                        downloading = INET_ERR;
                    }
                    else
                    {
                        //// o << "SEND REQUEST" << endl;

                        /****************/
                        /* SEND REQUEST */
                        /****************/
                        char *buff;
                        if (!WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0,
                                                0))
                        {
                            // ErrorOut (GetLastError(), "HttpSend");
                            downloading = INET_ERR;
                            //	// o << "HttpSend" << GetLastError() << endl;
                        }
                        else
                        {
                            //	// o << "CHECK HTTP RESPONSE" << endl;

                            /****************************************/
                            /* CHECK HTTP RESPONSE CODE FROM SERVER */
                            /****************************************/

                            BOOL bResults = FALSE;

                            bResults = WinHttpReceiveResponse(hReq, NULL);

                            /*******************/
                            /* OPEN LOCAL FILE */
                            /*******************/

                            output_file = fopen(local_file, "wb");

                            if (output_file == NULL)
                            {
                                //							cerr << "Open local save file error: " << GetLastError()
                                //<<endl;
                                downloading = INET_ERR;
                                envoi_msg("probleme de telechargement VISTA ? ");
                                //	o << "Open local save file error" << GetLastError() << endl;
                            }
                            else
                            {
                                // ok ouvert
                                total_down = 0;
                                m_dwTotalSize = 100;

                                downloading = INET_CHUNK; // on attaque le dwload
                                //	m_font->print(m_width/4, haut, "Connection Ok");
                            }

                        } // httpsend
                    } // hreq
                } // conect
            } // hopen
        }
        else
        {

            if (downloading == INET_CHUNK)
            {

                /*******************************************/
                /* LOOP READING CHUNKS OF FILE FROM SERVER */
                /*                                         */
                /* We can't rely on the CONTENTS_LENGTH    */
                /* header to find the size of the file,    */
                /* since this header may not exist.        */
                /* (it is optional for HTTP/1.0)           */
                /*******************************************/
                // Check for available data.
                DWORD dwSize = 0;
                DWORD dwDownloaded = 0;
                LPSTR pszOutBuffer;

                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hReq, &dwSize))
                {
                    char ttt[200];

                    sprintf(ttt, "Error %u in WinHttpQueryDataAvailable.", GetLastError());
                    envoi_msg(ttt);
                }

                // Allocate space for the buffer.
                pszOutBuffer = new char[dwSize + 1];
                if (!pszOutBuffer)
                {

                    dwSize = 0;
                    envoi_msg("Out of memory");
                }
                else
                {
                    // Read the data.
                    ZeroMemory(pszOutBuffer, dwSize + 1);

                    if (!WinHttpReadData(hReq, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                    {
                        char ttt[200];

                        sprintf(ttt, "Error %u in WinHttpReadData.\n", GetLastError());
                        envoi_msg(ttt);
                    }
                    else
                    {

                        // write_count = fwrite(pszOutBuffer, 1, dwSize, output_file);
                    }

                    // envoi_msg("download");

                    // Free the memory allocated to the buffer.
                    delete[] pszOutBuffer;
                }
                if (!dwSize)
                {
                    downloading = INET_DISCONNECT;
                    // envoi_msg("Telechargement termine");
                    //	m_font->print(m_width/4, haut, "Decompression MAP");
                }
            }
            else
            {

                if (downloading == INET_DISCONNECT)
                {

                    /*********************/
                    /* CLOSE LOCAL FILE  */
                    /*********************/

                    fclose(output_file);

                    /*****************/
                    /* CLOSE REQUEST */
                    /*****************/

                    if (!WinHttpCloseHandle(hReq))
                    {
                        //	ErrorOut (GetLastError (), "CloseHandle on hReq");
                        downloading = INET_ERR;
                    }
                    else
                    {

                        /********************/
                        /* CLOSE CONNECTION */
                        /********************/

                        if (!WinHttpCloseHandle(hConnect))
                        {
                            //	ErrorOut (GetLastError (), "CloseHandle on hConnect");
                            downloading = INET_ERR;
                        }
                        else
                        {

                            /************************/
                            /* CLOSE HTTP INTERFACE */
                            /************************/

                            if (!WinHttpCloseHandle(hOpen))
                            {
                                //	ErrorOut(GetLastError(), "CloseHandle on hOpen");
                                downloading = INET_ERR;
                            }
                            else
                            {
                                downloading = INET_IDLE; // ok fini
                                //
                                if (type_download == 0)
                                {
                                    if (!dezip(local_file))
                                    {
                                        downloading = INET_ERR;
                                    }
                                }
                                else
                                {

                                    char buf[300];
                                    sprintf(buf, "%s %s", Mp3List.List[curmp3id].Name, Mp3List.List[curmp3id].mappeur);
                                    m_chat->addtext(buf, 2);

                                    mp3->stop();
                                    if (config.music_on)
                                        mp3->load(Mp3List.List[curmp3id].Name);
                                }
                            }

                        } // close connect

                    } // close REQUEST
                }
                else
                {
                }
            }
        }
        // end_orto();
    }
}
void Engine::download_task_old()
{

    if (downloading != INET_IDLE)
    {
        //		 begin_orto();
        glColor3f(0.6f, 0.5f, 0.5f);

        if (downloading == INET_ERR)
        {

            //	// o << "impossible de downloader la map" << endl;
            if (output_file)
            {
                fclose(output_file);
            }

            menu_state = -1;
            downloading = INET_IDLE;
        }

        if (downloading == INET_CONNECT)
        {

            //	// o << "inet_connect" << endl;

            LPCTSTR lpctstr[2] = {"*/*", NULL};

            //	cout << "url is : " << url<<endl;

            /******************/
            /* OPEN INTERNET */
            /******************/

            hOpen = WinHttpOpen(L"InetURL/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS, 0);

            //		'InternetOpen("HTTP-GET",LOCAL_INTERNET_ACCESS,NULL,0,0);

            if (!hOpen)
            {
                //	// o << GetLastError() << endl;

                //	m_xmlsession.ErrorOut ( GetLastError(), "InternetOpen");
                //	MessageBox (m_hwnd,GetLastError(),TEXT("InternetOpen"),MB_OK);
                downloading = INET_ERR;
            }
            else
            {

                /****************************/
                /* BREAK OUT URL COMPONENTS */
                /****************************/
                //	// o << "connect" << endl;
                port = 80;
                // CString str;
                // str.Format(_T("char %c ", c)

                // Install the status callback function.
                WCHAR lphost[100];
                DXUtil_ConvertAnsiStringToWide(lphost, m_cur_host);
                hConnect = WinHttpConnect(hOpen, (LPCWSTR)lphost, INTERNET_DEFAULT_HTTP_PORT, NULL); //(LPCWSTR) lphost
                // hConnect = InternetConnect(hOpen,m_cur_host,port,"","",
                //  INTERNET_SERVICE_HTTP,0,0);
                if (!hConnect)
                {
                    // ErrorOut (GetLastError(), "InternetConnect");
                    downloading = INET_ERR;
                }
                else
                {
                    //// o << "HTTP REQUEST" << endl;

                    /*********************/
                    /* OPEN HTTP REQUEST */
                    /*********************/

                    // flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
                    // if (! stricmp(url_scheme, "https"))
                    //     flags |= INTERNET_FLAG_SECURE;
                    WCHAR lp_distant_file[100];
                    DXUtil_ConvertAnsiStringToWide(lp_distant_file, distant_file);
                    //
                    hReq = WinHttpOpenRequest(hConnect, L"GET", (LPCWSTR)lp_distant_file, NULL, WINHTTP_NO_REFERER,
                                              WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

                    //	hReq = HttpOpenRequest(hConnect,"GET",distant_file,HTTP_VERSION,"",lpctstr,
                    //															flags,0);
                    if (!hReq)
                    {
                        //		// o << GetLastError() << endl;

                        //	ErrorOut (GetLastError(), "HttpOpenRequest");
                        downloading = INET_ERR;
                    }
                    else
                    {
                        //// o << "SEND REQUEST" << endl;

                        /****************/
                        /* SEND REQUEST */
                        /****************/
                        char *buff;
                        if (!WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0,
                                                0))
                        {
                            // ErrorOut (GetLastError(), "HttpSend");
                            downloading = INET_ERR;
                            //	// o << "HttpSend" << GetLastError() << endl;
                        }
                        else
                        {
                            //	// o << "CHECK HTTP RESPONSE" << endl;

                            /****************************************/
                            /* CHECK HTTP RESPONSE CODE FROM SERVER */
                            /****************************************/

                            BOOL bResults = FALSE;

                            bResults = WinHttpReceiveResponse(hReq, NULL);

                            /*******************/
                            /* OPEN LOCAL FILE */
                            /*******************/

                            output_file = fopen(local_file, "wb");

                            if (output_file == NULL)
                            {
                                //							cerr << "Open local save file error: " << GetLastError()
                                //<<endl;
                                downloading = INET_ERR;
                                envoi_msg("probleme de telechargement VISTA ? ");
                                //	o << "Open local save file error" << GetLastError() << endl;
                            }
                            else
                            {
                                // ok ouvert
                                total_down = 0;
                                m_dwTotalSize = 100;

                                downloading = INET_CHUNK; // on attaque le dwload
                                //	m_font->print(m_width/4, haut, "Connection Ok");
                            }

                        } // httpsend
                    } // hreq
                } // conect
            } // hopen
        }
        else
        {

            if (downloading == INET_CHUNK)
            {

                /*******************************************/
                /* LOOP READING CHUNKS OF FILE FROM SERVER */
                /*                                         */
                /* We can't rely on the CONTENTS_LENGTH    */
                /* header to find the size of the file,    */
                /* since this header may not exist.        */
                /* (it is optional for HTTP/1.0)           */
                /*******************************************/
                // Check for available data.
                DWORD dwSize = 0;
                DWORD dwDownloaded = 0;
                LPSTR pszOutBuffer;

                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hReq, &dwSize))
                {
                    char ttt[200];

                    sprintf(ttt, "Error %u in WinHttpQueryDataAvailable.", GetLastError());
                    envoi_msg(ttt);
                }

                // Allocate space for the buffer.
                pszOutBuffer = new char[dwSize + 1];
                if (!pszOutBuffer)
                {

                    dwSize = 0;
                    envoi_msg("Out of memory");
                }
                else
                {
                    // Read the data.
                    ZeroMemory(pszOutBuffer, dwSize + 1);

                    if (!WinHttpReadData(hReq, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                    {
                        char ttt[200];

                        sprintf(ttt, "Error %u in WinHttpReadData.\n", GetLastError());
                        envoi_msg(ttt);
                    }
                    else
                    {

                        //	write_count = fwrite(pszOutBuffer, 1, dwSize, output_file);
                    }

                    // envoi_msg("download");

                    // Free the memory allocated to the buffer.
                    delete[] pszOutBuffer;
                }
                if (!dwSize)
                {
                    downloading = INET_DISCONNECT;
                    // envoi_msg("Telechargement termine");
                    //	m_font->print(m_width/4, haut, "Decompression MAP");
                }
            }
            else
            {

                if (downloading == INET_DISCONNECT)
                {

                    /*********************/
                    /* CLOSE LOCAL FILE  */
                    /*********************/

                    fclose(output_file);

                    /*****************/
                    /* CLOSE REQUEST */
                    /*****************/

                    if (!WinHttpCloseHandle(hReq))
                    {
                        //	ErrorOut (GetLastError (), "CloseHandle on hReq");
                        downloading = INET_ERR;
                    }
                    else
                    {

                        /********************/
                        /* CLOSE CONNECTION */
                        /********************/

                        if (!WinHttpCloseHandle(hConnect))
                        {
                            //	ErrorOut (GetLastError (), "CloseHandle on hConnect");
                            downloading = INET_ERR;
                        }
                        else
                        {

                            /************************/
                            /* CLOSE HTTP INTERFACE */
                            /************************/

                            if (!WinHttpCloseHandle(hOpen))
                            {
                                //	ErrorOut(GetLastError(), "CloseHandle on hOpen");
                                downloading = INET_ERR;
                            }
                            else
                            {
                                downloading = INET_IDLE; // ok fini
                                //
                                if (type_download == 0)
                                {
                                    if (!dezip(local_file))
                                    {
                                        downloading = INET_ERR;
                                    }
                                }
                                else
                                {

                                    char buf[300];
                                    sprintf(buf, "%s %s", Mp3List.List[curmp3id].Name, Mp3List.List[curmp3id].mappeur);
                                    m_chat->addtext(buf, 2);

                                    mp3->stop();
                                    if (config.music_on)
                                        mp3->load(Mp3List.List[curmp3id].Name);
                                }
                            }

                        } // close connect

                    } // close REQUEST
                }
                else
                {
                }
            }
        }
        // end_orto();
    }
}

void Engine::donwload_frame()
{
    if (downloading != INET_IDLE)
    {
        int haut = m_height / 5;
        char down[200];
        int pct = 100;

        switch (downloading)
        {
        case INET_IDLE:

            break;
        case INET_CONNECT:
            sprintf(down, "CONNECTION");
            break;
        case INET_CHUNK:
            pct = ((100 * total_down) / m_dwTotalSize);
            if (type_download == 0)
                sprintf(down, "MAP: %s %i/100", m_cur_map, pct);
            else
                sprintf(down, "MP3: %s %i/100", Mp3List.List[curmp3id].Name, pct);
            break;
        case INET_DISCONNECT:
            sprintf(down, "DECONNECTION");
            break;
        case INET_ERR:
            sprintf(down, "ERREUR CONNECTION");
            break;
        }
        glColor4f(0.7f, 0.4f, 0.5f, 0.7f); //
        m_font->print(m_width / 4, haut, down);
        GlQuad(m_width / 4, haut - 16, m_width / 4 + 2 * (m_width / 4) * pct / 100, haut + 32);

        // m_font->print(m_width/4, haut, down);
    }
}

void Engine::reset_mouse()
{
    POINT p;
    p.x = m_xcenter, p.y = m_ycenter;
    if (!config.fullscreen)
    {
        ClientToScreen(m_hwnd, &p);
    }

    SetCursorPos(p.x, p.y);
}

DPNID Engine::prochain_a_suivre()
{
    DPNID prochain;
    int id_vivant = -1;
    int rndo = random_t::RandomRange(0, 2);
    if (rndo == 1)
        prochain = g_dpnidLocalPlayer;
    else
    {
        for (int yy = 0; yy < g_lNumberOfActivePlayers; yy++)
        {
            if (!lejoueur[yy]->mort && lejoueur[VRAI]->QuelTeam == lejoueur[yy]->QuelTeam)
                id_vivant = yy;
        }

        if (id_vivant > 0)
            prochain = lejoueur[id_vivant]->ID;
        else
            prochain = g_dpnidLocalPlayer;
    }
    return prochain;
}

void Engine::handle_input_main()
{
    // lockequipe();

    if (!mode_op)
        if ((m_input.keys[VK_F3]) && !(relkeys[VK_F3]))
        {
            if (m_playerfile)
            {
                //** CFT si la partie a pas commence on peut changer de skin
                if (((round == 0) && (cle_tournois == 0)) &&
                    ((CFT_ON == false) || (CFT_ON == TRUE && CFT_nb_gign == 0 && CFT_nb_terro == 0) ||
                     ((CFT_ON == TRUE || TEAM_ON == TRUE) && (CFT_eta < 2))))
                {
                    m_playerfile->player_model = m_playerfile->player_model + 1;
                    if (m_playerfile->player_model >= max_modele - 2)
                        m_playerfile->player_model = 0;
                    char buff[100];
                    model2str(m_playerfile->player_model, buff);

                    if (m_chat)
                        m_chat->addtext(buff, 2);

                    if (g_lNumberOfActivePlayers > 0)
                    {
                        lejoueur[VRAI]->affecte_modele(leshommes, m_playerfile->player_model, max_modele);

                        parts->SystemNew(new pSystemBoom_t, lejoueur[VRAI]->pos, 1);

                        //** CFT ici on replace le joueur qd il change de skin pdt les 5 secondes
                        if (CFT_ON || TEAM_ON)
                        {
                            lejoueur[VRAI]->pos = world.RenvoiePosition(est_gign(lejoueur[VRAI]->id_modele));
                            m_pivot.move(lejoueur[VRAI]->pos);
                        }
                    }
                }
            }
        }
    relkeys[VK_F3] = (m_input.keys[VK_F3]);

    if ((m_input.keys[VK_F4]) && !(relkeys[VK_F4]))

    {
        romuchat_on = !romuchat_on;
        if (romuchat_on)
            m_chat->addtext("RomuChat Active", 2);
        else
            m_chat->addtext("RomuChat Desactive", 2);
    }
    relkeys[VK_F4] = (m_input.keys[VK_F4]);

    if ((m_input.keys[VK_F2]) && !(relkeys[VK_F2]))
    {
        if (m_pings->tps > 1.0f)
            m_pings->tps = 0.0f;
        else
            m_pings->tps = 30.0f;
    }

    relkeys[VK_F2] = (m_input.keys[VK_F2]);

    if (menu_state != 1100 && !lan_mode && !config.isdebug && (m_input.keys[VK_F1]) && !(relkeys[VK_F1]) && (mp3))
    {

        if ((!config.music_on))
        {
            mp3->load(Mp3List.List[curmp3id].Name);

            char text[50];
            if (m_playerfile)
            {

                sprintf(text, "%s ON", m_playerfile->player_mp3);
                if (m_chat)
                    m_chat->addtext(text, 2);
            }
        }
        else
        {
            mp3->stop();
            if (m_playerfile)
            {

                char text[50];
                sprintf(text, "%s OFF", m_playerfile->player_mp3);
                if (m_chat)
                    m_chat->addtext(text, 2);
            }
        }
        config.music_on = !config.music_on;
        config.save();
        m_chat->addtext("Config sauvegardee", 2);
    }
    relkeys[VK_F1] = (m_input.keys[VK_F1]);

    if ((!config.isdebug && !lan_mode) && (m_input.keys[VK_F11]) && (downloading == INET_IDLE) && !(relkeys[VK_F11]))
    {
        curmp3id++;
        if (curmp3id > Mp3List.count - 1)
            curmp3id = 0;
        if (Verify_Mp3(Mp3List.List[curmp3id].Name))
        {
            char buf[300];
            sprintf(buf, "%s %s", Mp3List.List[curmp3id].Name, Mp3List.List[curmp3id].mappeur);
            m_chat->addtext(buf, 2);
            mp3->stop();
            if (config.music_on)
                mp3->load(Mp3List.List[curmp3id].Name);
        }
        else
        {
            sprintf(distant_file, "/romustrike/mp3/%s.mp3", Mp3List.List[curmp3id].Name);
            sprintf(local_file, "data/mp3/%s.mp3", Mp3List.List[curmp3id].Name);
            sprintf(m_cur_host, "%s", Mp3List.List[curmp3id].host);
            downloading = INET_CONNECT;
            type_download = 1;
        }
        m_xmlsession.set_mp3(m_playerfile, Mp3List.List[curmp3id].id);
    }
    relkeys[VK_F11] = (m_input.keys[VK_F11]); // VK_F12

    if (menu_state == 444 || menu_state == 555 || menu_state == 151 || menu_state == 110 || menu_state == 990 ||
        modechat)
        input_text_key();
}

void Engine::input_text_key()
{
    char cs[] = " ";

    if ((strlen(lesinput_box[active_input]->text.text) < lesinput_box[active_input]->max_len))
    {
        for (int c = 65; c < 91; c++)
        {
            if ((m_input.keys[c]) && !(relkeys[c]))
            {
                sprintf(cs, "%c", c);
                strcat(lesinput_box[active_input]->text.text, cs);
            }
            relkeys[c] = (m_input.keys[c]);
        }
        for (int c = 96; c < 106; c++)
        {
            if ((m_input.keys[c]) && !(relkeys[c]))
            {
                sprintf(cs, "%d", c - 96);
                strcat(lesinput_box[active_input]->text.text, cs);
            }
            relkeys[c] = (m_input.keys[c]);
        }
        if (m_input.keys[VK_SHIFT])
        {
            for (int c = 48; c < 59; c++)
            {
                if ((m_input.keys[c]) && !(relkeys[c]))
                {
                    sprintf(cs, "%d", c - 48);
                    strcat(lesinput_box[active_input]->text.text, cs);
                }
                relkeys[c] = (m_input.keys[c]);
            }
            if ((m_input.keys[190]) && !(relkeys[190]))
            {

                sprintf(cs, "%c", '.');
                strcat(lesinput_box[active_input]->text.text, cs);
            }
            relkeys[190] = (m_input.keys[190]);
        }
        else
        {
            relkeys[VK_SHIFT] = (m_input.keys[VK_SHIFT]);
            int acs[6] = {32, VK_DECIMAL, 56, 54, 48, 190};
            char car[6] = {' ', '.', '_', '-', '@', '.'};
            for (int cpt = 0; cpt < 6; cpt++)
            {

                if ((m_input.keys[acs[cpt]]) && !(relkeys[acs[cpt]]))
                {
                    sprintf(cs, "%c", car[cpt]);
                    strcat(lesinput_box[active_input]->text.text, cs);
                }
                relkeys[acs[cpt]] = (m_input.keys[acs[cpt]]);
            }
        }
    }
    if ((strlen(lesinput_box[active_input]->text.text) > 0) && (m_input.keys[VK_BACK]) && !(relkeys[VK_BACK]))
    {
        int l = strlen(lesinput_box[active_input]->text.text);
        memcpy(lesinput_box[active_input]->text.text, lesinput_box[active_input]->text.text, l - 1);
        lesinput_box[active_input]->text.text[l - 1] = '\0';
    }
    relkeys[VK_BACK] = (m_input.keys[VK_BACK]);
    if (!modechat)
    {
        if ((m_input.keys[9]) && !(relkeys[9]))
        {
            if (menu_state == 444)
                if (active_input == 0)
                    active_input = 1;
                else
                    active_input = 0;

            if (menu_state == 555)
                if (active_input == 2)
                    active_input = 3;
                else if (active_input == 3)
                    active_input = 4;
                else
                    active_input = 2;
        }
        relkeys[9] = (m_input.keys[9]);
    }
}

void Engine::handle_input(float delta)
{
    // lockequipe();

    m_input.keys[VK_LBUTTON] = m_input.left_button;
    m_input.keys[VK_RBUTTON] = m_input.right_button;

    // souris inversee
    if ((m_input.keys[config.keys[12]]) && !(relkeys[config.keys[12]]))
    {
        souris_inverse = souris_inverse * -1;
    }
    relkeys[config.keys[12]] = (m_input.keys[config.keys[12]]);

    if (!modechat)
    {
        for (int yi = 49; yi < 58; yi++)
        {
            if ((m_input.keys[yi]) && !(relkeys[yi]))
            {
                int id = yi - 49 + 42;
                Send_voix(id);
                lejoueur[VRAI]->Voix(lessons[id]);
            }
            relkeys[yi] = (m_input.keys[yi]);
        }
    }

    // GAMMA PATCH

    if ((m_input.keys[VK_SUBTRACT]) && !(relkeys[VK_SUBTRACT]))
    {
        DisplayGamma = true;
    }
    relkeys[VK_SUBTRACT] = (m_input.keys[VK_SUBTRACT]);

    if ((m_input.keys[VK_SUBTRACT]) && (relkeys[VK_SUBTRACT]) && config.GAMMA > 0)
    {
        gamadelta = gamadelta + delta;
        if (gamadelta > 0.5f)
        {
            config.GAMMA--;
            gamadelta = 0.0f;
        }
    }

    if ((m_input.keys[VK_ADD]) && !(relkeys[VK_ADD]))
    {
        DisplayGamma = true;
    }
    relkeys[VK_ADD] = (m_input.keys[VK_ADD]);
    if ((m_input.keys[VK_ADD]) && (relkeys[VK_ADD]) && config.GAMMA < 8)
    {
        gamadelta = gamadelta + delta;
        if (gamadelta > 0.5f)
        {
            config.GAMMA++;
            gamadelta = 0.0f;
        }
    }

    if (!(m_input.keys[VK_SUBTRACT]) && !(m_input.keys[VK_ADD]) && DisplayGamma == true)
    {

        world.AffecteGammaSurUneFace(config.GAMMA);
        DisplayGamma = false;
        config.save();
        m_chat->addtext("sauvegarde config", 2);
    }

    // fin GAMMAPATCH

    relkeys[VK_F7] = (m_input.keys[VK_F7]);

    if (lejoueur[VRAI]->is_car)
    {
        bool av = m_input.keys[config.keys[0]];
        bool ar = m_input.keys[config.keys[1]];
        bool ga = m_input.keys[config.keys[3]];
        bool dr = m_input.keys[config.keys[2]];
        bool fr = m_input.keys[VK_NUMPAD0];
    }

    lejoueur[VRAI]->tir = (!couvre_feu) && lejoueur[VRAI]->mort == false &&
                          (((m_input.keys[config.keys[6]]) && (mode_op || lejoueur[VRAI]->arme.munition > 0)));

    if ((m_input.keys[VK_RETURN]) && !(relkeys[VK_RETURN]))
    {
        if (modechat)
        {
            if (strlen(lesinput_box[9]->text.text) > 0)
            {
                kikage(lesinput_box[9]->text.text);
                envoi_chat(lesinput_box[9]->text.text);
                if (mode_ecoute)
                    if (le_socket != 0)
                        envoi_msg_ops(lesinput_box[9]->text.text);

                strcpy(lesinput_box[9]->text.text, "");
            }
            modechat = false;
        }
        else
        {
            modechat = true;
            active_input = 9;
        }
    }
    relkeys[VK_RETURN] = (m_input.keys[VK_RETURN]);

    if ((m_input.keys[VK_TAB]) && !(relkeys[VK_TAB]))
    {
        if (modechat)
        {
            modechat = false;
            kikage(lesinput_box[9]->text.text);

            if (le_socket != 0)
                envoi_msg(lesinput_box[9]->text.text);

            strcpy(lesinput_box[9]->text.text, "");
        }
        else
        {
            modechat = true;
        }
    }
    relkeys[VK_TAB] = (m_input.keys[VK_TAB]);

    if (!modechat)
    {

        if ((m_input.keys[82]) && !(relkeys[82]))
        {
            if (strcmp(lejoueur[VRAI]->arme.name, "grenade") && strcmp(lejoueur[VRAI]->arme.name, "c4") &&
                strcmp(lejoueur[VRAI]->arme.name, "fumigene") && strcmp(lejoueur[VRAI]->arme.name, "plasma") &&
                ((lejoueur[VRAI]->occupe == false)))
                lejoueur[VRAI]->reload_arme();
        }

        relkeys[82] = (m_input.keys[82]);
    }

    if (false) // config.isdebug
    {

        // blob
        vec3_t dir_blob = vec3_t(0.0f, 0.0f, 0.0f);

        if ((m_input.keys[VK_NUMPAD8]) && !(relkeys[VK_NUMPAD8]))
        {
            dir_blob[2] = 20.0f;
        }
        relkeys[VK_NUMPAD8] = (m_input.keys[VK_NUMPAD8]);

        if ((m_input.keys[VK_NUMPAD2]) && !(relkeys[VK_NUMPAD2]))
        {
            dir_blob[2] = -20.0f;
        }
        relkeys[VK_NUMPAD2] = (m_input.keys[VK_NUMPAD2]);

        if ((m_input.keys[VK_NUMPAD4]) && !(relkeys[VK_NUMPAD4]))
        {
            dir_blob[0] = 20.0f;
        }
        relkeys[VK_NUMPAD4] = (m_input.keys[VK_NUMPAD4]);

        if ((m_input.keys[VK_NUMPAD6]) && !(relkeys[VK_NUMPAD6]))
        {
            dir_blob[0] = -20.0f;
        }
        relkeys[VK_NUMPAD6] = (m_input.keys[VK_NUMPAD6]);

        if ((m_input.keys['1']) && !(relkeys['1']))
        {
            cycle = !cycle;
        }
        relkeys['1'] = (m_input.keys['1']);
        if ((m_input.keys['6']) && !(relkeys['6']))
        {
            gravite = !gravite;
        }
        relkeys['6'] = (m_input.keys['6']);
        if ((m_input.keys['7']) && !(relkeys['7']))
        {
            les_boites = !les_boites;
        }
        relkeys['7'] = (m_input.keys['7']);

        if ((m_input.keys['5']) && !(relkeys['5']))
        {
            boites_mdl = !boites_mdl;
        }
        relkeys['5'] = (m_input.keys['5']);

        if ((m_input.keys['9']) && !(relkeys['9']))
        {
            camera_libre = !camera_libre;
        }
        relkeys['9'] = (m_input.keys['9']);

        // pour le placement de la camera
        if ((m_input.keys['J']) && !(relkeys['J']))
        {
            switchcam = !switchcam;
        }
        relkeys['J'] = (m_input.keys['J']);

        if ((m_input.keys['T']) && (relkeys['T']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[0] = lejoueur[VRAI]->yeux[0] + 1;
            else
                lejoueur[VRAI]->flash[0] = lejoueur[VRAI]->flash[0] + 1;
        }
        relkeys['T'] = (m_input.keys['T']);

        if ((m_input.keys['F']) && (relkeys['F']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[0] = lejoueur[VRAI]->yeux[0] - 1;
            else
                lejoueur[VRAI]->flash[0] = lejoueur[VRAI]->flash[0] - 1;
        }
        relkeys['F'] = (m_input.keys['F']);

        if ((m_input.keys['Y']) && (relkeys['Y']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[1] = lejoueur[VRAI]->yeux[1] + 1;
            else
                lejoueur[VRAI]->flash[1] = lejoueur[VRAI]->flash[1] + 1;
        }
        relkeys['Y'] = (m_input.keys['Y']);

        if ((m_input.keys['G']) && (relkeys['G']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[1] = lejoueur[VRAI]->yeux[1] - 1;
            else
                lejoueur[VRAI]->flash[1] = lejoueur[VRAI]->flash[1] - 1;
        }
        relkeys['G'] = (m_input.keys['G']);

        if ((m_input.keys['U']) && (relkeys['U']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[2] = lejoueur[VRAI]->yeux[2] + 1;
            else
                lejoueur[VRAI]->flash[2] = lejoueur[VRAI]->flash[2] + 1;
        }
        relkeys['U'] = (m_input.keys['U']);

        if ((m_input.keys['H']) && (relkeys['H']))
        {
            if (switchcam)
                lejoueur[VRAI]->yeux[2] = lejoueur[VRAI]->yeux[2] - 1;
            else
                lejoueur[VRAI]->flash[2] = lejoueur[VRAI]->flash[2] - 1;
        }
        relkeys['H'] = (m_input.keys['H']);

        if ((m_input.keys['6']) && !(relkeys['6']))
        {
            les_boites = !les_boites;
        }
        relkeys['6'] = (m_input.keys['6']);

        if (m_input.keys['L'])
        {
            lejoueur[VRAI]->pos[0] = 324;
            lejoueur[VRAI]->pos[1] = 457;
            lejoueur[VRAI]->pos[2] = 40;
        }
    }

    float x = 0, y = 0, z = 0;
    float yaw, pitch, roll = 0;
    float x2 = 0, y2 = 0, z2 = 0;

    int str = 0, str2 = 0, str3 = 0;
    if (!modechat)
    {
        yaw = (m_input.xpos - m_xcenter) * msens;
        pitch = (m_input.ypos - m_ycenter) * (msens / 1.6f) * souris_inverse;
        reset_mouse();
    }
    else
    {
        yaw = 0.0f;
        pitch = 0.0f;
    }

    if (m_input.keys['E'])
    {
        z = +speed;
    }
    if (m_input.keys['D'])
    {
        z = -speed;
    }
    static const float piover180 = 0.0174532925f;

    if ((m_input.keys[config.keys[11]]) && !(relkeys[config.keys[11]]))
    {
        exterieur = !exterieur;
        if (exterieur)
        {

            lejoueur[VRAI]->yeux[0] = 0;
            lejoueur[VRAI]->yeux[1] = -89;
            lejoueur[VRAI]->yeux[2] = 36;
        }
    }
    relkeys[config.keys[11]] = (m_input.keys[config.keys[11]]);

    if (!lejoueur[VRAI]->is_car)
    {

        float d = m_pivot.yaw() + yaw;
        d *= (__PI / 180);

        if (m_input.keys[config.keys[3]])
        {
            str = -1;
            str3 = 1;
        }
        if (m_input.keys[config.keys[2]])
        {
            str = 1;
            str3 = 1;
        }
        if (m_input.keys[config.keys[0]])
        {
            str3 = 1;
            str2 = 1;
        }
        if (m_input.keys[config.keys[1]])
        {
            str3 = -1;
            str2 = 1;
        }

        lejoueur[VRAI]->sens = 1;
        if (str3 == -1)
        {
            lejoueur[VRAI]->sens = -1;
        }
        int max = 30 * str - str * 15 * (str2);
        int rot = 0;
        if (str3 == -1)
        {
            max = -max;
        }

        if (lejoueur[VRAI]->colonne < max)
        {
            lejoueur[VRAI]->colonne = lejoueur[VRAI]->colonne + 15;
            rot = -15;
        }
        else if (lejoueur[VRAI]->colonne > max)
        {
            lejoueur[VRAI]->colonne = lejoueur[VRAI]->colonne - 15;
            rot = 15;
        }

        if (cycle)
        {
            x = sinf(d - 80 + (3 * 0) * (__PI / 180)) * speed * str3;
            y = cosf(d - 80 + (3 * 0) * (__PI / 180)) * speed * str3;
        }
        else
        {
            x = sinf(d - 80 + (3 * rot) * (__PI / 180)) * speed * str3;
            y = cosf(d - 80 + (3 * rot) * (__PI / 180)) * speed * str3;
        }
        rotation[0] = pitch;
        rotation[1] = roll;
        rotation[2] = yaw + 3 * rot;

        rotation2[0] = pitch;
        rotation2[1] = 0;
        rotation2[2] = yaw - 3 * rot;

        char ch[20];
        strcpy(ch, TEXT("jump"));

        if (lejoueur[VRAI]->can_jump == true)
        {
            if ((m_input.keys[config.keys[5]]) || (force_crouch))
            {
                force_crouch = false;
                lejoueur[VRAI]->Offset_crouch = -6.0f;
                if (str3 == 0)
                {

                    strcpy(lejoueur[VRAI]->dep, TEXT("crouch_idle"));
                    // lejoueur[VRAI]->dep="idle1";
                }
                else
                {
                    strcpy(lejoueur[VRAI]->dep, TEXT("crouchrun"));
                    // lejoueur[VRAI]->dep="run";
                    x = x / 3.0f;
                    y = y / 3.0f;
                    z = z / 3.0f;
                }
            }
            else
            {

                lejoueur[VRAI]->Offset_crouch = 12.0f;
                if (str3 == 0)
                {

                    strcpy(lejoueur[VRAI]->dep, TEXT("idle1"));
                }
                else
                {
                    strcpy(lejoueur[VRAI]->dep, TEXT("run"));
                }

                if (m_input.keys[96])
                {
                    if (!est_fatigue)
                    {
                        fatigue += delta;
                        if (fatigue > 2.0f)
                        {
                            est_fatigue = true;
                        }
                        cours = 1.4f;
                    }
                    else
                        cours = 1.0f;
                }
                else
                {
                    cours = 1.0f;
                    if (fatigue > 0.0f)
                        fatigue -= delta;
                }

                if (fatigue < 0.0f)
                {
                    est_fatigue = false;
                }
            }
        }

        if ((m_input.keys[config.keys[4]]) && !relkeys[config.keys[4]])
        {
            if (lejoueur[VRAI]->can_jump == true)
            {
                strcpy(lejoueur[VRAI]->dep, TEXT("jump"));

                if ((int)random_t::RandomRange(0.0f, 10.0f) == 4)
                    JoueUnSon(son_jump, lejoueur[VRAI]->pos);
                lejoueur[VRAI]->pulse = 2000.0f * delta;
                lejoueur[VRAI]->pulse = 8.5f;
                lejoueur[VRAI]->hauteur = lejoueur[VRAI]->pos[2];
                lejoueur[VRAI]->saut = true;
                lejoueur[VRAI]->sens = 1;
                lejoueur[VRAI]->modele.fps1 = 0.0f;
            }
        }
        relkeys[config.keys[4]] = (m_input.keys[config.keys[4]]);
    }

    if ((m_input.keys['O']) && !(relkeys['O']))
    {
        m_overlay = !m_overlay;
    }
    relkeys['O'] = (m_input.keys['O']);

    if ((m_input.keys['F']) && !(relkeys['F']))
    {
        if (config.isdebug)
            world.wireframe_mode = !world.wireframe_mode;
    }
    relkeys['F'] = (m_input.keys['F']);

    if ((m_input.keys['S']) && !(relkeys['S']))
    {
        if (!CFT_ON && g_lNumberOfActivePlayers > 0 && lejoueur[VRAI]->mort == false)
        {

            mourrir();
        }
    }
    relkeys['S'] = (m_input.keys['S']);

    if ((m_input.keys[VK_ESCAPE]) && !(relkeys[VK_ESCAPE]))
    {
        strcpy(lejoueur[VRAI]->dep, TEXT("idle1"));
        // lejoueur[VRAI]->dep="idle1";
        menu_mode = true;
        if (m_romu)
            m_romu->show();
        ouvertureporte(); // 05/12/2002
    }
    relkeys[VK_ESCAPE] = (m_input.keys[VK_ESCAPE]);

    if ((m_input.right_button) && !(BTN_DROIT))
    {
        if ((lejoueur[VRAI]->arme.scope > 0))
        {
            SCOPE_ETA = !SCOPE_ETA;
        }
    }
    BTN_DROIT = m_input.right_button;

    if (lejoueur[VRAI]->arme.cur_scope < 2.0f)
        lejoueur[VRAI]->arme.cur_scope = 2.0f;
    if (lejoueur[VRAI]->arme.cur_scope > lejoueur[VRAI]->arme.scope)
        lejoueur[VRAI]->arme.cur_scope = lejoueur[VRAI]->arme.scope;

    static float save_msens = 0.0f;
    if ((SCOPE_ETA) && (lejoueur[VRAI]->arme.scope > 0) && (exterieur == false) && !lejoueur[VRAI]->mort)
    {

        if ((world.znear < lejoueur[VRAI]->arme.cur_scope) || (world.znear > lejoueur[VRAI]->arme.cur_scope))
        {
            if (save_msens == 0.0f)
                save_msens = msens;

            double ss = (lejoueur[VRAI]->arme.cur_scope - world.znear);

            world.znear = lejoueur[VRAI]->arme.cur_scope;
            world.set_znear();

            while (ss != 0.0f)
            {
                if (ss > 0.0f)
                {
                    ss -= 0.5;
                    msens *= 0.92f;
                }
                else if (ss < 0.0f)
                {
                    ss += 0.5;
                    msens /= 0.92f;
                }
            }
        }
        sniper = true;
    }
    else
    {
        if (save_msens != 0.0f)
        {
            world.znear = 1.0;
            world.set_znear();
            msens = save_msens;
            save_msens = 0.0f;
        }
        sniper = false;
    }

    dir[0] = x;
    dir[1] = y;
    dir[2] = z;

    // pour le scroll de la souris
    if ((m_input.keys[config.keys[9]]) && !(relkeys[config.keys[9]]))
    {
        m_cross->position++;
        if (m_cross->position >= 17)
        {
            m_cross->position = 1;
        }
        config.viseur = m_cross->position;
        config.save();
        m_chat->addtext("Config sauvegarder", 2);
    }
    relkeys[config.keys[9]] = (m_input.keys[config.keys[9]]);

    if ((m_input.keys[config.keys[8]]) && !(relkeys[config.keys[8]]))
    {
        m_cross->position--;
        if (m_cross->position <= 0)
        {
            m_cross->position = 16;
        }
        config.viseur = m_cross->position;
        config.save();
        m_chat->addtext("Config sauvegarder", 2);
    }
    relkeys[config.keys[8]] = (m_input.keys[config.keys[8]]);

    // pour l'arme
    if (m_playerfile)
    {
        int sc = m_playerfile->score;

        float coef = 1.4f;

        maximun_arme = floor(log((sc * coef * coef * coef) / 10) / log(coef) + 1);
        if (maximun_arme < 3)
            maximun_arme = 3;
    }
    else
    {
        maximun_arme = 3;
    }

    if (maximun_arme > max_arme)
    {
        maximun_arme = max_arme;
    }

    if (config.isdebug || lan_mode)
        maximun_arme = max_arme;

    if (!(lejoueur[VRAI]->tir) && (lejoueur[VRAI]->occupe == false) &&
        ((m_input.is_wheeling && m_input.wheel > 0) || (m_input.keys[config.keys[10]] && !relkeys[config.keys[10]])))
    {
        if (!SCOPE_ETA)
        {
            Change_arme(true);
        }
        else
        {
            lejoueur[VRAI]->arme.cur_scope += 1.0f;
        }
        m_input.is_wheeling = false;
    }

    relkeys[config.keys[10]] = (m_input.keys[config.keys[10]]);

    if (!(lejoueur[VRAI]->tir) && (lejoueur[VRAI]->occupe == false) &&
        ((m_input.is_wheeling && m_input.wheel <= 0) || (m_input.keys[config.keys[7]] && !relkeys[config.keys[7]])))
    {
        if (!SCOPE_ETA)
        {
            Change_arme(false);
        }
        else
        {
            lejoueur[VRAI]->arme.cur_scope -= 1.0f;
        }
        m_input.is_wheeling = false;
    }

    relkeys[config.keys[7]] = (m_input.keys[config.keys[7]]);

    if (strcmp(lejoueur[VRAI]->arme.name, "grenade") && strcmp(lejoueur[VRAI]->arme.name, "c4") &&
        strcmp(lejoueur[VRAI]->arme.name, "fumigene") && strcmp(lejoueur[VRAI]->arme.name, "plasma") &&
        (m_input.middle_button && (lejoueur[VRAI]->occupe == false)))
    {
        lejoueur[VRAI]->reload_arme();
    }
}

void Engine::Change_arme(bool plus)
{

    if (lejoueur[VRAI])
    {
        int next_arme = lejoueur[VRAI]->id_weapon;
        int untour;
        untour = 0;
        if (plus)
        {
            do
            {
                untour++;
                next_arme++;
                if (next_arme >= maximun_arme)
                    next_arme = 0;
            } while (untour < maximun_arme && les_armes_autorisees[next_arme] == '-');
        }
        else
        {
            do
            {
                untour++;
                next_arme--;
                if (next_arme < 0)
                    next_arme = maximun_arme - 1;
            } while (untour < maximun_arme && les_armes_autorisees[next_arme] == '-');
        }

        lejoueur[VRAI]->affecte_arme(lesarmes, next_arme, max_arme);
        lejoueur[VRAI]->affecte_sound(lessons[lejoueur[VRAI]->arme.id_son]);
        if (m_phrase)
        {
            char rr[100];
            sprintf(rr, "%s", lejoueur[VRAI]->arme.name);
            m_phrase->addtext(rr, 0);
        }
    }
}

void Engine::migratehost()
{
    if (m_playerfile && !lan_mode)
        m_serverid = m_xmlsession.DevenirServer(
            m_playerfile, m_cur_map, (CFT_ON | (TEAM_ON << 1) | (SNIPER_ON << 2)), m_dwMaxPlayers, cle_tournois, round,
            cur_md5, lesinput_box[6]->text.text, strlen(lesinput_box[5]->text.text) > 0, les_armes_autorisees); //
    m_bHostPlayer = TRUE;
    menu_state = 51;
    if (m_chat)
        m_chat->addtext("Tu est devenu le serveur", 2);
}

void Engine::overlay()

{
    static int frame = 0;
    static int fps = 0;
    static DWORD last_tick = 0;

    frame++;
    DWORD tick = GetTickCount();
    if (tick - last_tick > 1000)
    {
        fps = frame;
        frame = 0;
        last_tick = tick;
    }

    char tmp[64];

    if (!config.isdebug)
        sprintf(tmp, "Score %i pts   FPS: %d ", m_playerfile->score, fps);
    else
        sprintf(tmp, "FPS: %d FACES :%i", fps, world.render_count());
    glColor4f(0.5f, 0.9f, 0.8f, 1.0f);

    m_font->print(0, 0, tmp);

    //** CFT ici on affiche en overlay le compte a rebours
    if ((CFT_ON || TEAM_ON) && (CFT_eta == 1))
    {
        m_pings->visible = true;
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_FUNC);
}
void Engine::hit_Romu(void)
{
    m_panel->hit_Romu();
}

void Engine::AfficheCroix()
{
    if (exterieur)
    {
        return;
    }

    if (sniper)
    {
        m_cross->courante = 1;
    }
    else
    {
        m_cross->courante = 0;
    }

    if (!menu_mode && !lejoueur[VRAI]->mort)
    {
        m_cross->print(m_width, m_height);
    }
}
void Engine::HandleEvent(void)
{

    mp3->HandleEvent();
}

void Engine::AffichePanel(float delta)
{
    m_panel->print(m_width, m_height, lejoueur[VRAI]->arme.munition, lejoueur[VRAI]->vie, g_lNumberOfActivePlayers,
                   lejoueur[VRAI]->arme.name);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    char ma[5];
    sprintf(ma, "%d", lejoueur[VRAI]->arme.munition);
    int t = m_font->taille;
    m_font->taille = 40;
    glEnable(GL_BLEND);
    m_font->print(80 * m_width / 1200, 30 * m_height / 600, ma);
    m_font->taille = t;
    if (DisplayGamma)
    {
        char str[20];
        sprintf(str, "Gamma = %d", config.GAMMA);
        m_font->print(20, m_height / 2, str);
    }

    glDepthMask(1);

}

static float letick;

static void anim()
{

    static float curr = 0.0f, prev = 0.0f, direction = 1.0f;
    curr = GetTickCount() / 1000.0f;
    letick = direction * (curr - prev);
    prev = curr;
}
void Engine::frame()
{
    float offset_train = 0.0f;

    static bool HORS_DE_LA_MAP;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static vec3_t pos_apres_coll(0.0f, 0.0f, 0.0f);
    static vec3_t pos_leaf(0.0f, 0.0f, 0.0f);
    static vec3_t dir_apres_coll(0.0f, 0.0f, 0.0f);

    float delta = 0.0f;
    frameTime.Frame();
    delta = frameTime.GetDelta();

    if (delta > 5.1f)
        delta = 5.1f;

    delta = delta * time_acceleration;

    if (mode_storm)
    {
        int rnd_storm = random_t::RandomRange(0, floor(30 * 1 / delta));
        if (rnd_storm == 1 || rnd_storm == 3 || rnd_storm == 2)
        {
            time_storm = 1.0f;
            if (rnd_storm == 1)
                JoueUnSon(son_storm1, vec3_t(0.0f, 0.0f, 0.0f));
            if (rnd_storm == 2)
                JoueUnSon(son_storm2, vec3_t(0.0f, 0.0f, 0.0f));
            if (rnd_storm == 3)
                JoueUnSon(son_storm3, vec3_t(0.0f, 0.0f, 0.0f));
        }

        if (time_storm >= 1.0f)
        {
            time_storm = time_storm + delta * 10.0f;
            if (time_storm > 300.0f)
            {
                time_storm = 0.0f;
                storm_off = false;
            }

            storm_off = !((time_storm < eclaire[0]) || (time_storm > eclaire[1] && time_storm < eclaire[2]) ||
                          (time_storm > eclaire[3] && time_storm < eclaire[4]));
        }
    }

    vec3_t dst_cam;

    m_panel->frame(delta);
    marks->Frame(delta);
    m_pings->frame(delta);

    Earth_quakeframe(delta);

    if (m_tchat > 0.0f)
        m_tchat = m_tchat - delta;

    anim();

    speed = (GetTickCount() - old_tick) / 5.0f;
    if ((GetTickCount() - old_tick) > lagg_ms)
    {
        old_tick = GetTickCount();
        envoi_position(); // envoie DPLAY AUTRE JOUEUR
    }
    if ((GetTickCount() - old_tick_team) > 10000) // lagg_ms15000
    {
        if ((m_bHostPlayer && (menu_state == 51)))
        {
            if (TEAM_ON)
            {
                // y a t'il un survivant

                int nb_gign_vie = 0;
                int nb_terro_vie = 0;
                int nb_gign = 0;
                int nb_terro = 0;

                lockequipe(); //----------------------------------------------LOCK

                for (int i = 0; i < g_lNumberOfActivePlayers; i++)
                {
                    if (lejoueur[i]->QuelTeam == 0)
                    {
                        if (!lejoueur[i]->mort && lejoueur[i]->id_modele != (max_modele - 1)) // en vie et pas op
                            nb_gign_vie++;
                        nb_gign++;
                    }
                    else
                    {

                        if (!lejoueur[i]->mort && lejoueur[i]->id_modele != (max_modele - 1))
                            nb_terro_vie++;

                        nb_terro++;
                    }
                }
                unlockequipe(); //----------------------------------------------LOCK

                if (nb_terro != 0 || nb_gign != 0)
                {
                    if (nb_gign_vie != 0 || nb_terro_vie != 0)
                    {
                        if (nb_gign_vie == 0 && nb_gign > 0)
                            CFT_envoi_message(3);
                        if (nb_terro_vie == 0 && nb_terro > 0)
                            CFT_envoi_message(4);
                    }
                    else
                    {
                        if (nb_gign > 0 && nb_terro > 0)
                        {
                            CFT_envoi_message(3);
                            CFT_envoi_message(4);
                        }
                    }
                }
            }

            if (cle_tournois != 0)
            {
                if (TOURNOIS_ON == 0)
                {
                    // on est en tournois
                    //  on a pas commencé
                    // tous les monde est la si ekip 1 et 2 complete
                    bool complet[2];
                    complet[0] = true;
                    complet[1] = true;

                    lockequipe(); //----------------------------------------------LOCK
                    for (int be = 0; be < lesekip.size(); be++)
                    {

                        for (int bp = 0; bp < lesekip.at(be).list.size(); bp++)
                        {
                            bool trouve = false;
                            for (int i = 0; i < g_lNumberOfActivePlayers; i++)
                            {
                                if (strcmp(lesekip.at(be).list.at(bp).login, lejoueur.at(i)->playername))
                                {
                                    trouve = true;
                                }
                            }
                            complet[be] = complet[be] && trouve;
                        }
                    }
                    unlockequipe(); //----------------------------------------------LOCK
                    // y a ti l trop de monde
                    lockequipe(); //----------------------------------------------LOCK
                    for (int i = 0; i < g_lNumberOfActivePlayers; i++)
                    {
                        bool trouve = false;

                        for (int be = 0; be < lesekip.size(); be++)
                        {

                            for (int bp = 0; bp < lesekip.at(be).list.size(); bp++)
                            {

                                if (strcmp(lesekip.at(be).list.at(bp).login, lejoueur.at(i)->playername))
                                {
                                    trouve = true;
                                }
                            }
                        }
                        complet[0] = complet[0] && trouve;
                    }
                    unlockequipe(); //----------------------------------------------LOCK

                    if (complet[0] && complet[1])
                    {
                        envoi_chat("Les equipes sont au complet\0");
                        envoi_chat("Le tournois commence pour X min\0");
                        TOURNOIS_ON = 1;
                        TOURNOIS_count = GetTickCount() + tournoisv.at(curr_tournois).timeout * 1000;
                    }
                    else
                    {
                        m_chat->addtext("Mode tournois, en attente...\0", 2);
                    }
                }
                else
                {
                    // doit on finir
                    int remain = TOURNOIS_count - GetTickCount();
                    char buff[100];
                    sprintf(buff, "TOURNOIS EN COURS %d min %d sec\0", (remain / 1000) / 60, (remain / 1000) % 60);

                    envoi_chat(buff);

                    if (TOURNOIS_ON == 1 && (remain < 0))
                    {

                        TOURNOIS_ON = 2;

                        if (g_lNumberOfActivePlayers > 1)
                        {

                            // a faire : score par ekip ??? CYRIL
                            // a faire : BAT : mise a jour du noeud tournois sur mysql quand le tournois est terminer.
                            // a faire : friendly fire CYRIL
                            // a faire : quand un joueur entre dans la partie avant de faire 'LANCER' sur le menu , le
                            // rendu est tous pourri bare noir , surment du au envoi position qui envoie de la merde car
                            // pas init ??

                            int score1 = 0;
                            int score2 = 0;

                            for (int c = 0; c < lejoueur.size(); c++)
                            {
                                if (lejoueur.at(c)->QuelTeam == 0)
                                {
                                    score1 = score1 + lejoueur.at(c)->score;
                                }
                                else
                                {
                                    score2 = score2 + lejoueur.at(c)->score;
                                }
                            }
                            // mise a jour xml tounois
                            if (m_playerfile)
                                m_xmlsession.SetTournois(m_playerfile, cle_tournois, round, score1, score2);

                            // on vire tous le monde
                            for (int ij = 1; ij < g_lNumberOfActivePlayers; ij++)
                            {
                                GAMEMSG_KIKKED msgWave;
                                msgWave.dwType = GAME_MSGID_CFT_KIKKED;
                                msgWave.ID_du_joueur = lejoueur[ij]->ID;

                                DPN_BUFFER_DESC bufferDesc;
                                bufferDesc.dwBufferSize = sizeof(GAMEMSG_KIKKED);
                                bufferDesc.pBufferData = (BYTE *)&msgWave;

                                DPNHANDLE hAsync;

                                m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync,
                                              DPNSEND_GUARANTEED);
                            }

                            envoi_chat("Tournois termine\0");
                        }
                    }
                    else
                    {
                        if (TOURNOIS_ON == 2)
                        {
                            envoi_chat("Mise a jour des scores effectue\0");
                        }
                    }
                }
            }
        }
        old_tick_team = GetTickCount();
    }

    if ((GetTickCount() - old_tchat) > 20000) //
    {
        old_tchat = GetTickCount();
        envoi_who(); // envoie socket
    }

    if (speed > 10) // 05/12/2002
    {
        speed = 10; // 05/12/2002
    }

    speed = 200.0f * delta * cours;

    handle_input_main();
    download_task_new();

    if (CFT_ON || TEAM_ON)
    {
        //** CFT pour initialiser la partie
        if (CFT_eta == 0)
        {

            if (m_bHostPlayer == false)
                CFT_envoi_message(2); // a chaque nouvelle partie le joueur demande au host les recap

            CFT_count = 0;
            CFT_eta = 1;
            CFT_old_count = GetTickCount();
            m_overlay = true;
        }
        else
        {
            if (CFT_eta != -2)
            {
                m_overlay = true;

                long tmp = GetTickCount();

                if (CFT_count <= cft_timing)
                    CFT_count = tmp - CFT_old_count;
                // CFT_old_count=tmp;
                if (CFT_count > cft_timing)
                {

                    CFT_eta = 2;
                }
            }
        }
    }

    if (menu_mode)
    {

        m_camera.load();
        display_screen(delta);
    }
    else
    {

        bool world_is_loaded;
        world_is_loaded = (menu_state == 51 || menu_state == 64);

        vec3_t boi;
        vec3_t yeux;
        vec3_t flash;
        vec3_t fl, fl2;
        vec3_t old_pos;

        if (g_lNumberOfActivePlayers > 0)
            old_pos = lejoueur[VRAI]->pos;

        if (!menu_mode)
            handle_input(delta);

        //** CFT ici on bloque les moiuvements du joueur
        if ((CFT_ON || TEAM_ON) && CFT_eta < 2)
        {
            dir[0] = 0;
            dir[1] = 0;
            dir[2] = 0;
        }

        // if (exterieur)

        if (lejoueur[VRAI]->mort == false)
        {
            // on est pas mort pas en free camera

            if (world_is_loaded)
            {

                vec3_t pos = m_pivot.eye();
                // vec3_t pos=lejoueur[VRAI]->pos;

                if (pos[2] < -10000.f) //**ALTITUDE MODIFIE 06/01/2005
                {
                    vec3_t ret = world.RenvoiePosition(est_gign(lejoueur[VRAI]->id_modele));
                    m_pivot.move(ret);
                    pos = m_pivot.eye();
                    pos_apres_coll = pos;
                    lejoueur[VRAI]->pos = pos;
                    lejoueur[VRAI]->pulse = 0.0f;
                    //	lejoueur[VRAI]->m_PhysEnv->AxeG=pos ;
                }

                // si il est en l'air on reprend la velocity precedente
                if (lejoueur[VRAI]->can_jump == false)
                {
                    dir = dir;
                }
                dir_apres_coll = dir;
                world.process_visible_faces_collide(pos, dir);
                dir[2] = 1.0f;
                world.epsilon = 0.3f;
                world.radius = 23.0f;
                world.collide = false;
                world.normale_face_touchee[0] = 0.0f;
                world.normale_face_touchee[1] = 0.0f;
                world.normale_face_touchee[2] = 0.0f;

                vec3_t dst = world.check_collisions(pos, dir, 0, false);
                dst = collision_joueur(pos, dst - pos);

                world.trace = world.normale_face_touchee;
                // sers a recuperer la normale de la face toucher mais ca a pas l'air top

                static vec3_t repere;
                if (lejoueur[VRAI]->saut)
                    if ((lejoueur[VRAI]->pos[2] - lejoueur[VRAI]->hauteur) > 40.0f)
                        lejoueur[VRAI]->pulse = 0.0f;

                dir[2] = (-6.0f + lejoueur[VRAI]->pulse) * delta * 60.0f;

                bool JeSuisSurUneEchelle = world.DessineEntites(pos);

                if (JeSuisSurUneEchelle == true)
                {
                    world.radius = 23.0f;
                    if (m_input.keys[config.keys[0]])
                    {
                        dir[2] = (2) * delta * 60.0f;
                    }
                    if (m_input.keys[config.keys[1]])
                    {
                        dir[2] = (-2) * delta * 60.0f;
                    }
                }
                dir[0] = 0.0f;
                dir[1] = 0.0f;
                vec3_t e = dst;
                bool a_colide = world.collide;
                world.collide = false;
                dst = world.check_collisions(dst, dir, 4, false);
                world.trace = world.normale_face_touchee;

                if (JeSuisSurUneEchelle == false && world.trace[2] > 0.0f && world.trace[2] < 0.5f)
                {
                    dst = pos;
                }

                pos_apres_coll = dst;

                HORS_DE_LA_MAP = (world.find_leaf(dst) == 0);
                if (HORS_DE_LA_MAP == true)
                {

                    dir[2] = -70.0f;
                    dst = pos;
                    dst[2] = dst[2] + 60;
                    // cutoff=0;
                    dst = world.check_collisions(dst, dir, 0, false);

                    HORS_DE_LA_MAP = (world.find_leaf(dst) == 0);
                }
                lejoueur[VRAI]->pos[0] = dst[0];
                lejoueur[VRAI]->pos[1] = dst[1];
                lejoueur[VRAI]->pos[2] = dst[2] + lejoueur[VRAI]->Offset_crouch;
                if (dir.len() < 0.72f)
                {
                    lejoueur[VRAI]->pos[1] = dst[1];
                }

                if ((world.collide))
                {
                    lejoueur[VRAI]->can_jump = true;
                    lejoueur[VRAI]->pulse = 0.0f;
                    lejoueur[VRAI]->modele.AniActFini = true;
                }
                else
                {
                    // world.collide =false;
                    // vec3_t dst1 = world.check_collisions(e,dir,4);

                    if (dir.len() > 0.1f)
                        strcpy(lejoueur[VRAI]->dep, TEXT("jump"));
                    lejoueur[VRAI]->can_jump = false;
                    lejoueur[VRAI]->pulse = lejoueur[VRAI]->pulse - delta * 15.0f;
                }

                if (cycle)
                {
                    m_pivot.move(dst);
                    m_pivot.rotate_delta(rotation2);
                    m_pivot.transform();

                    m_pivot2.move(dst);
                    m_pivot2.rotate_delta(rotation);
                    m_pivot2.transform();
                }
                else
                {
                    m_pivot.move(dst);
                    m_pivot.rotate_delta(rotation2);
                    m_pivot.transform();

                    m_pivot2.move(dst);
                    m_pivot2.rotate_delta(rotation);
                    m_pivot2.transform();
                }

                blend = lejoueur[VRAI]->colonne;
                m_pivot2.pitch_bounds();
                lejoueur[VRAI]->modele.ang_dos = (-m_pivot2.pitch());
                lejoueur[VRAI]->rot[2] = lejoueur[VRAI]->colonne;
                lejoueur[VRAI]->rot[1] = m_pivot.yaw();
                lejoueur[VRAI]->fps = delta;

                m_camera.cam.load_identity();
                m_camera.load();
                vec3_t pos_tmp;
                yeux = lejoueur[VRAI]->pos + lejoueur[VRAI]->modele.tete;
                flash = lejoueur[VRAI]->pos + lejoueur[VRAI]->modele.flash;

                vec3_t yeux_tmp = yeux;
                lejoueur[VRAI]->flash[0] = -30; // lejoueur[VRAI]->flash[0]=0;
                lejoueur[VRAI]->flash[1] = 285;
                lejoueur[VRAI]->flash[2] = 0;

                lejoueur[VRAI]->yeux[0] = 3;
                lejoueur[VRAI]->yeux[1] = 0;
                lejoueur[VRAI]->yeux[2] = 21;
                float ang;
                if (cycle)
                    ang = m_pivot.yaw();
                else
                    ang = 0;
                float a = (ang + float(-blend * 3)) * float(__PI / 180);
                float b = 3 * m_pivot2.pitch() * (__PI / 180);
                // le flash
                flash = lejoueur[VRAI]->flash;

                vec3_t yyy = flash;

                yyy[2] = flash[2] * cosf(b) - flash[1] * sinf(b);
                yyy[1] = flash[2] * sinf(b) + flash[1] * cosf(b);

                flash = yyy;

                yyy[0] = flash[1] * cosf(a) + flash[0] * sinf(a);
                yyy[1] = -flash[1] * sinf(a) + flash[0] * cosf(a);

                flash[0] = lejoueur[VRAI]->pos[0] + yyy[0];
                flash[1] = lejoueur[VRAI]->pos[1] + yyy[1];
                flash[2] = lejoueur[VRAI]->pos[2] + lejoueur[VRAI]->modele.tete[2] + yyy[2];

                D3DVECTOR top;
                top.x = 0.0f;
                top.y = 1.0f;
                top.z = 0.0f;
                g_pDSListener->SetOrientation(-yyy[0], -yyy[1], 0, top.x, top.y, top.z, DS3D_IMMEDIATE);

                ///////////////////////////////////////////////

                // les yeux
                yeux = lejoueur[VRAI]->yeux;

                yyy = yeux;

                yyy[2] = yeux[2] * cosf(b) - yeux[1] * sinf(b);
                yyy[1] = yeux[2] * sinf(b) + yeux[1] * cosf(b);

                yeux = yyy;

                yyy[0] = yeux[1] * cosf(a) + yeux[0] * sinf(a);
                yyy[1] = -yeux[1] * sinf(a) + yeux[0] * cosf(a);

                yeux[0] = lejoueur[VRAI]->pos[0] + yyy[0];
                yeux[1] = lejoueur[VRAI]->pos[1] + yyy[1];
                yeux[2] = lejoueur[VRAI]->pos[2] + yyy[2];

                if (!world_is_loaded || (!exterieur && !lejoueur[VRAI]->mort))
                {

                }
                else
                {

                    vec3_t yeux_libres;
                    yeux_libres[0] = 0;
                    yeux_libres[1] = -89;
                    yeux_libres[2] = 36;

                    // les yeux libres

                    yyy = yeux_libres;

                    yyy[2] = yeux_libres[2] * cosf(b) - yeux_libres[1] * sinf(b);
                    yyy[1] = yeux_libres[2] * sinf(b) + yeux_libres[1] * cosf(b);

                    yeux_libres = yyy;

                    yyy[0] = yeux_libres[1] * cosf(a) + yeux_libres[0] * sinf(a);
                    yyy[1] = -yeux_libres[1] * sinf(a) + yeux_libres[0] * cosf(a);

                    yeux_libres[0] = lejoueur[VRAI]->pos[0] + yyy[0];
                    yeux_libres[1] = lejoueur[VRAI]->pos[1] + yyy[1];
                    yeux_libres[2] = lejoueur[VRAI]->pos[2] + yyy[2];

                    world.radius = 10.0f;

                    yeux = yeux_libres;
                    pos_tmp = lejoueur[VRAI]->pos;
                    yeux_tmp = yeux;
                    yeux = world.check_collisions(pos_tmp, yeux - pos_tmp, 4, false);
                }

                HORS_DE_LA_MAP = (world.find_leaf(yeux) == 0);
                float diff = yeux[2] - yeux_tmp[2];

                if (HORS_DE_LA_MAP)
                {
                    //||)	((diff<-0.5f) && (lejoueur[VRAI]->Offset_crouch!=-6.0f)
                    //	force_crouch=true; //forcé le crouch au prochain tour
                    lejoueur[VRAI]->Offset_crouch = -20.0f;
                    lejoueur[VRAI]->pos[2] = lejoueur[VRAI]->pos[2] + lejoueur[VRAI]->Offset_crouch;
                    yeux[2] = yeux[2] + lejoueur[VRAI]->Offset_crouch;
                    strcpy(lejoueur[VRAI]->dep, TEXT("crouchrun"));
                    // force_crouch=true;
                }

                yeux[2] = yeux[2] + earth_quake * 15.0f;
                flash[1] = flash[1] - earth_quake * 10.0f;

                gluLookAt(yeux[0], yeux[1], yeux[2], flash[0], flash[1], flash[2], 0, 0, 1);
                glGetFloatv(GL_MODELVIEW_MATRIX, m_camera.cam.m);

                m_camera.move(yeux[0], yeux[1], yeux[2]);
            } // pas voiture fin

        }
        else
        {
            if (lejoueur[VRAI]->mort == true)
            {

                dir[0] = 0;
                dir[1] = 0;
                dir[2] = 0;

                if (world_is_loaded && (respawn_time <= 0))
                { // on  relache lasouris respawn


                    init_player(VRAI);
                    pos_apres_coll = lejoueur[VRAI]->pos;
                    dir_apres_coll = dir;
                }
                else
                {
                    if (!TEAM_ON || (CFT_eta != -2 && TEAM_ON))
                        respawn_time = respawn_time - delta;
                }
            }
            // chasse=false;
            lockequipe(); //----------------------------------------------LOCK

            vec3_t dst2;
            vec3_t dst3;
            vec3_t dstf;
            int id_suivi;

            // chute du corps
            if (world_is_loaded)
            {
                lejoueur[VRAI]->velocity = lejoueur[VRAI]->velocity + vec3_t(0.0f, 0.0f, 1.0f) * delta * -9.30f; //
                vec3_t sp = lejoueur[VRAI]->pos;
                vec3_t np = sp + lejoueur[VRAI]->velocity;

                world.process_visible_faces_collide(sp, np - sp);
                world.epsilon = 0.3f;
                world.radius = 23.0f;
                world.collide = false;
                world.normale_face_touchee[0] = 0.0f;
                world.normale_face_touchee[1] = 0.0f;
                world.normale_face_touchee[2] = 0.0f;

                vec3_t dst = world.check_collisions(sp, np - sp, 2, false);
                if (world.collide)
                    ;
                lejoueur[VRAI]->velocity = lejoueur[VRAI]->velocity * 0.95f;

                lejoueur[VRAI]->pos = dst;
            }
            //------------------------- suivi camera

            if (g_camera_suivi == 0)
                g_camera_suivi = g_dpnidLocalPlayer;

            for (int j = 0; j < g_lNumberOfActivePlayers; j++)
            {
                if ((lejoueur[j]->ID == g_camera_suivi))
                {
                    dst2 = lejoueur[j]->pos - vec3_t(0.0f, 0.0f, -60.0f);

                    vec3_t rr = m_camera.eye();
                    dstf = lejoueur[j]->pos;
                    D3DVECTOR ee;
                    ee.x = rr[0];
                    ee.y = rr[1];
                    ee.z = rr[2];
                    id_suivi = j;
                }
            }

            //

            D3DVECTOR front;
            D3DVECTOR top;
            top.x = 0.0f;
            top.y = 1.0f;
            top.z = 0.0f;
            front.x = old_pos[0] - lejoueur[VRAI]->pos[0];
            front.y = old_pos[1] - lejoueur[VRAI]->pos[1];
            front.z = old_pos[2] - lejoueur[VRAI]->pos[2];
            g_pDSListener->SetOrientation(front.x, front.y, front.z, top.x, top.y, top.z, DS3D_IMMEDIATE);
            g_pDSListener->SetPosition(dst3[0], dst3[1], dst3[2], DS3D_IMMEDIATE);
            unlockequipe(); //----------------------------------------------LOCK
            dst3 = m_camera.eye();
            vec3_t dst5 = dst2 - dst3;
            vec3_t dst4;

            world.process_visible_faces_collide(dst2, dst5);

            collision_tir col_tir;

            col_tir = world.check_tirs(dst2, dst3 - dst2);
            // dst5=dst3-dst2;
            if (col_tir.found)
                dst3 = col_tir.pt;

            vec3_t dir_c = (dst2 - dst3);
            dst4 = (dst3 + dir_c * delta * 0.33f);

            vec3_t tmp = dst2 - dst4;
            if (tmp.len() > 150.0f)
                dst_cam = dst4;
            else
                dst_cam = m_camera.eye();

            vec3_t r = dstf - vue_cam;

            vue_cam = vue_cam + r / 1.5f;
            // vue_cam.normalize();
            // vue_cam[1]=0.0f;

            m_camera.cam.load_identity();
            m_camera.load();

            gluLookAt(dst_cam[0], dst_cam[1], dst_cam[2], vue_cam[0], vue_cam[1], vue_cam[2], 0, 0,
                      1); // orient ,pos,norm

            glGetFloatv(GL_MODELVIEW_MATRIX, m_camera.cam.m);
            m_camera.move(dst_cam);
            yeux = m_camera.eye();

            lejoueur[VRAI]->fps = letick;

            yeux = m_camera.eye();
        }

        if ((release_tir == 0) && (lejoueur[VRAI]->mort == true) && (lejoueur[VRAI]->tir)) //
            release_tir = 1;
        if ((release_tir == 1) && (((!lejoueur[VRAI]->tir) && (!TEAM_ON || (TEAM_ON && CFT_eta != -2)))))
            release_tir = 3;
        if ((release_tir == 3) && ((!lejoueur[VRAI]->tir)))
            release_tir = 4;

        //-------------------explosion grenade
        if (world_is_loaded)
        {
            lespos.clear();
            lesposjoueur.clear();
            lesid_joueur.clear();
            amoi.clear();

            lockequipe(); //----------------------------------------------LOCK
            for (int o = 0; o < g_lNumberOfActivePlayers; o++)
            {
                // if (o!=VRAI)
                lesposjoueur.push_back(lejoueur[o]->pos);
                lesid_joueur.push_back(lejoueur[o]->ID);
            }
            grenades->Frame(delta, &lespos, &amoi, &lesposjoueur, g_dpnidLocalPlayer, &lesid_joueur, &lejoueur);
            unlockequipe(); //----------------------------------------------LOCK
        }
        vec3_t tt;

        VectorCopy(m_camera.eye(), tt);

        if (!mode_storm && world_is_loaded && !config.isdebug)
            world.render_skyfaces(tt);
        if (world_is_loaded && !menu_mode)
        {
            world.process_visible_faces(m_camera);
            world.render_visible_faces(yeux, storm_off); // g_lNumberOfActivePlayers-1
            world.advance_frame(delta);
        }

        lockequipe(); //----------------------------------------------LOCK

        static Camera camera2; // MOUARF ENCORE UN GLOBAL DANS NOTRE FACE
        camera2 = (Camera)m_camera;
        int nb_vi = 0;

        for (int ga = 0; ga < lespos.size(); ga++)
        {
            if (amoi[ga] == g_dpnidLocalPlayer) // ca
            {

                vec3_t explode;
                //	vec3_t * pexplode;
                explode = lespos[ga];
            }
        }

        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if (lejoueur[j]->etat == true)
            {
                for (int g = 0; g < lespos.size(); g++) // mauvaise boucle sur le grenade explosion
                {
                    vec3_t explode;

                    explode = lespos[g];
                    if (VRAI == j)
                    {
                        JoueUnSon(son_explode, explode);
                        vec3_t d = lejoueur[VRAI]->pos - explode;
                        float dis = d.len();
                        if (dis < 900.0f)
                            tps_earthquake = 1.0f;
                        else if (dis < 200.f)
                            tps_earthquake = tps_earthquake - 7.0f;
                    }

                    if (world_is_loaded && amoi[g] == g_dpnidLocalPlayer) // c a  moi c ma grenade
                    {

                        world.process_visible_faces3(explode);

                        collision_tir col_tir;
                        col_tir = world.check_tirs(lejoueur[j]->pos, explode - lejoueur[j]->pos);
                        vec3_t col_j; // dist mur joueur
                        col_j = col_tir.pt - lejoueur[j]->pos;
                        vec3_t gre_j; // dist gre joueur
                        gre_j = explode - lejoueur[j]->pos;
                        bool collision_mur = col_tir.found && (col_tir.nearest > 2 && col_tir.nearest <= distance_gun);
                        bool joueur_avant_mur = col_j.len() > gre_j.len();
                        if ((collision_mur && joueur_avant_mur) || !collision_mur)
                        {
                            // le joueur avant lemurj
                            float force = floor(70.0f + gre_j.len() * -1.0f / 2.0f);

                            if (force > 5.0f)
                            {
                                if (j == VRAI)
                                {
                                    m_panel->hit();
                                    lejoueur[j]->vie = lejoueur[j]->vie - force;
                                    if (j == VRAI) // c moi
                                    {
                                        if (lejoueur[VRAI]->pulse < 5.0f)
                                        {
                                            lejoueur[VRAI]->pulse = 8.0f;
                                        }

                                        lejoueur[VRAI]->saut = true;
                                        if (lejoueur[j]->vie <= 0)
                                        {
                                            vec3_t nn = gre_j;
                                            nn.normalize();
                                            lejoueur[j]->velocity = nn * -force / 5.0f;
                                            //** CFT ici on simule le confirme tue pour replacer le flag si on s'est
                                            //fait sauter
                                            //** CFT ca ne suffit pas !! CFT_replace_flag(lejoueur[j]->ID);

                                            //** CFT A VOIR en fait il faudrait remettre le confirme tue mm pour au
                                            //moins avertir qu'un joueur a ete flingue
                                            //** CFT sinon on refait un message specifique
                                            //** dans score_plus je remet dans tous les cas le flag
                                            Confirme_Tue(g_dpnidLocalPlayer, 0.0f); // envoie confirmation au tueur
                                            mourrir();

                                            // exterieur=true;
                                        }
                                        else
                                        {
                                            if (force > 20.0f)
                                                JoueUnSon(son_breath, lejoueur[VRAI]->pos);
                                        }
                                    }
                                }
                                else
                                {

                                    envoi_Tir(lejoueur[j]->ID, explode, floor(force), lejoueur[j]->killed);
                                    lejoueur[j]->SpawnBlood();
                                }
                            }
                        }
                    }
                }

                // if (j=!VRAI)
                //{

                if ((lejoueur[j]->id_weapon != lejoueur[j]->id_arme_recu) && (j != VRAI))
                {
                    lejoueur[j]->affecte_arme(lesarmes, lejoueur[j]->id_arme_recu, max_arme);
                    lejoueur[j]->affecte_sound(lessons[lejoueur[j]->arme.id_son]);
                }
                if ((lejoueur[j]->id_modele != lejoueur[j]->id_modele_recu) && (j != VRAI))
                {
                    lejoueur[j]->affecte_modele(leshommes, lejoueur[j]->id_modele_recu, max_modele);
                }

                lejoueur[j]->modele.AfficheBoite = boites_mdl;
                lejoueur[j]->arme.AfficheBoite = boites_mdl;
                vec3_t r = m_camera.right_vector();
                lejoueur[j]->modele.g_vright[0] = r[0];
                lejoueur[j]->modele.g_vright[1] = r[1];
                lejoueur[j]->modele.g_vright[2] = r[2];

                vec3_t u = m_camera.up_vector();
                lejoueur[j]->modele.g_lightvec[0] = -u[1];
                lejoueur[j]->modele.g_lightvec[1] = -u[2];
                lejoueur[j]->modele.g_lightvec[2] = -u[3];
                int rr;
                if (j == VRAI)
                {
                    if ((exterieur == false) && !lejoueur[VRAI]->mort)
                    {
                        rr = 5;
                    }
                    else
                    {
                        rr = 2;
                    }
                }
                else
                {
                    rr = 2;
                }
                lejoueur[j]->calcul_rendu = rr;
                if (world_is_loaded && j != VRAI)
                {
                    vec3_t rr = lejoueur[j]->pos;
                    lejoueur[j]->Visible = world.LeJoueur_est_il_visible(camera2, rr);
                    if (lejoueur[j]->Visible)
                        nb_vi++;

                    lejoueur[j]->anim();
                }
                else
                {
                    lejoueur[j]->anim();
                }

                if ((HORS_DE_LA_MAP == false))
                {

                    lejoueur[j]->Render_particle(delta);
                }

                /// MUZZ ici on calcule le flash de l'arme
                // dans les deux cas
                if (j == VRAI)
                {
                    float aa = (-m_pivot.yaw()) * float(__PI / 180);
                    vec3_t yyy = lejoueur[VRAI]->arme.StudioCalcAttachments(true);
                    vec3_t yyy2 = lejoueur[VRAI]->arme.StudioCalcAttachments(false);
                    fl = yyy;
                    fl2 = yyy2;
                    yyy[0] = fl[1] * cosf(aa) + fl[0] * sinf(aa);
                    yyy[1] = -fl[1] * sinf(aa) + fl[0] * cosf(aa);
                    yyy2[0] = fl2[1] * cosf(aa) + fl2[0] * sinf(aa);
                    yyy2[1] = -fl2[1] * sinf(aa) + fl2[0] * cosf(aa);

                    fl[0] = lejoueur[VRAI]->pos[0] + yyy[1]; /// argh c pas ça FIXME
                    fl[1] = lejoueur[VRAI]->pos[1] + yyy[0];
                    fl[2] = lejoueur[VRAI]->pos[2] + yyy[2];   // fl devrai etre la position du bout du canon
                    fl2[0] = lejoueur[VRAI]->pos[0] + yyy2[1]; /// argh c pas ça FIXME
                    fl2[1] = lejoueur[VRAI]->pos[1] + yyy2[0];
                    fl2[2] = lejoueur[VRAI]->pos[2] + yyy2[2]; // fl devrai etre la position du bout du canon
                }
                if (!(lejoueur[VRAI]->mort) && (exterieur == false) && (j == VRAI))
                {

                    m_camera.transform();
                    r = m_camera.right_vector();
                    lejoueur[j]->vue.g_vright[0] = r[0];
                    lejoueur[j]->vue.g_vright[1] = r[1];
                    lejoueur[j]->vue.g_vright[2] = r[2];

                    u = m_camera.up_vector();
                    lejoueur[j]->vue.g_lightvec[0] = -u[1];
                    lejoueur[j]->vue.g_lightvec[1] = -u[2];
                    lejoueur[j]->vue.g_lightvec[2] = -u[3];

                    glPushMatrix();
                    glLoadMatrixf(mm);
                    // vec3_t rt=m_camera.eye ();
                    int multi;

                    char ch[20];
                    strcpy(ch, TEXT("run"));
                    if (strcmp(lejoueur[j]->dep, ch) == 0)
                        multi = 5;
                    else
                        multi = 1;

                    static double offset;
                    offset = offset + 50 * delta * multi;

                    lejoueur[j]->vue.m_origin[0] = cosf(offset * __PI / 180);
                    lejoueur[j]->vue.m_origin[2] = -fabs(sinf(offset * __PI / 180));
                    // lejoueur[j]->vue.m_origin[1]=cosf(offset*__PI/180);
                    if (lejoueur[j])

                        //*SCOPE
                        // ici si on est en snipper on ne rend pas le modele car avec le scroll l'arme est visible
                        if (!sniper)
                        {
                            if (true)
                                lejoueur[j]->vue.DrawIni();
                        }
                    glPopMatrix();

                    // fl=lejoueur[VRAI]->vue.StudioCalcAttachments ();
                    // world.drawBoxEx (fl);

                    glPushMatrix();
                    glLoadMatrixf(mm2);

                    parts2->Frame(delta);
                    parts2->Render();

                    glPopMatrix();
                }

                // m_camera.load();

                D3DVECTOR lapos;
                if (j != VRAI)
                {

                    lapos.x = lejoueur[j]->pos[0];
                    lapos.y = lejoueur[j]->pos[1];
                    lapos.z = lejoueur[j]->pos[2];

                    if (lejoueur[j]->Visible == true && (m_overlay == true) && (HORS_DE_LA_MAP == false))
                    {
                        vec3_t la_cam = m_camera.eye();
                        vec3_t v = lejoueur[j]->pos - la_cam;

                        if (v.len() < 700.0f)
                        {
                            if ((est_gign(lejoueur[j]->id_modele) && est_gign(lejoueur[VRAI]->id_modele)) ||
                                (!est_gign(lejoueur[j]->id_modele) && !est_gign(lejoueur[VRAI]->id_modele)))
                                glColor4f(0.5f, 0.5f, 0.8f, 0.4f); ////
                            else
                                glColor4f(0.8f, 0.5f, 0.5f, 0.4f);

                            glRasterPos3f(lejoueur[j]->pos[0], lejoueur[j]->pos[1], lejoueur[j]->pos[2] + 30);
#if defined(GLUT_BITMAP)
                            for (unsigned int i = 0; i < strlen(lejoueur[j]->playername); i++)
                                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *(lejoueur[j]->playername + i));
#endif
                        }
                    }
                }
                else
                {
                    lapos.x = flash[0];
                    lapos.y = flash[1];
                    lapos.z = flash[2];
                }
                if (((j != VRAI) && (lejoueur[j]->arme.id_son != lejoueur[VRAI]->arme.id_son)))
                {

                    D3DVECTOR v;
                    v.x = 0.0f;
                    v.y = 0.0f;
                    v.z = 0.0f;

                    lejoueur[j]->playersound->Positionne3Dbuffer(&lapos, &v);
                }
            }
        }

        if (!lejoueur[VRAI]->mort)
            lejoueur[VRAI]->velocity = (lejoueur[VRAI]->pos - old_pos) / lejoueur[VRAI]->fps;

        unlockequipe(); //----------------------------------------------LOCK

        if (CFT_ON)
        {
            FlagTR.dt = delta;
            FlagCS.dt = delta;
            CmpCS.dt = delta;
            CmpTR.dt = delta;
            CmpCS.animMDL();
            CmpTR.animMDL();
            FlagTR.animMDL();
            FlagCS.animMDL();
        }
        marks->Render();

        parts->Frame(delta);
        parts->Render();
        grenades->Render();

        //*******************LISTENER**********************************************
        D3DVECTOR lapos;
        vec3_t gg = m_camera.eye();

        vec3_t t = flash - yeux;
        vec3_t son = lejoueur[VRAI]->pos;

        lapos.x = son[0];
        lapos.y = son[1];
        lapos.z = son[2];

        D3DVECTOR Vel;
        Vel.x = 0.0f;
        Vel.y = 0.0f;
        Vel.z = 0.0f;

        if (!lejoueur[VRAI]->mort)
        {
            if (old_pos[0] != lejoueur[VRAI]->pos[0] || old_pos[1] != lejoueur[VRAI]->pos[1] ||
                old_pos[2] != lejoueur[VRAI]->pos[2])
            {

                g_pDSListener->SetPosition(lapos.x, lapos.y, lapos.z, DS3D_IMMEDIATE);
            }
            D3DVECTOR vOrientFront;
            D3DVECTOR vOrientTop;

            t.normalize();
        }

        //******************************************************************

        vec3_t flash2;
        flash2 = flash - yeux;
        flash2.normalize();

        boi = flash2 * distance_gun + yeux;

        if (((GetTickCount() - old_pas) > 300) && strcmp("run", lejoueur[VRAI]->dep) == 0)
        {

            int rnd_pas = random_t::RandomRange(0.0f, 3.0f);

            if (rnd_pas == 0)
            {
                JoueUnSon(son_pas1, lejoueur[VRAI]->pos);
            }
            else
            {
                if (rnd_pas == 1)
                {
                    JoueUnSon(son_pas2, lejoueur[VRAI]->pos);
                }
                else
                {
                    JoueUnSon(son_pas3, lejoueur[VRAI]->pos);
                }
            }
            old_pas = GetTickCount();
        }
        if ((lejoueur[VRAI]->tir && lejoueur[VRAI]->fire == true)) //
        {

            DecaleArme();

            if (!strcmp(lejoueur[VRAI]->arme.name, "grenade") || !strcmp(lejoueur[VRAI]->arme.name, "c4") ||
                !strcmp(lejoueur[VRAI]->arme.name, "fumigene") || !strcmp(lejoueur[VRAI]->arme.name, "plasma")) //
            {
                bool ok = false;

                vec3_t pos_gre = lejoueur[VRAI]->pos;
                vec3_t dir_gre = flash;

                if (world_is_loaded && !strcmp(lejoueur[VRAI]->arme.name, "c4"))
                {
                    collision_tir col_mur;
                    world.process_visible_faces_collide(lejoueur[VRAI]->pos, flash2);

                    col_mur = world.check_tirs_rec(lejoueur[VRAI]->pos, flash2 * 100.0f, 5);
                    if (col_mur.found)
                    {
                        collision_tir col_enface;
                        world.process_visible_faces_collide(col_mur.pt + col_mur.normal, col_mur.normal);

                        col_enface = world.check_tirs_rec(col_mur.pt, col_mur.normal * 2000.0f, 5);
                        if (col_enface.found)
                        {
                            grenades->SystemNew(new pGrenSystemLazer_t, col_mur.pt, (col_enface.pt - col_mur.pt), 1,
                                                g_dpnidLocalPlayer);
                            ok = true;
                            pos_gre = col_mur.pt;
                            dir_gre = (col_enface.pt - col_mur.pt);
                        }
                    }
                }

                else
                {
                    dir_gre = flash2;

                    if (!strcmp(lejoueur[VRAI]->arme.name, "grenade"))
                    {
                        grenades->SystemNew(new pGrenSystemSimple_t, lejoueur[VRAI]->pos, (flash2), 1,
                                            g_dpnidLocalPlayer);

                        ok = true;
                    }
                    else
                    {

                        if (!strcmp(lejoueur[VRAI]->arme.name, "fumigene"))
                        {
                            grenades->SystemNew(new pGrenSystemSmoke_t, lejoueur[VRAI]->pos, (flash2), 1,
                                                g_dpnidLocalPlayer);
                            ok = true;
                        }
                        else
                        {

                            flash2[2] = flash2[2] + 0.1f;
                            if (exterieur)
                            {
                                grenades->SystemNew(new pGrenSystemMissile_t, fl, (flash2), 1, g_dpnidLocalPlayer);
                                ok = true;
                            }
                            else
                            {
                                grenades->SystemNew(new pGrenSystemMissile_t, fl, (flash2), 1, g_dpnidLocalPlayer);
                                ok = true;
                            }
                        }
                    }
                }
                if (ok == true)
                {
                    int type_gre = 0;
                    if (!strcmp(lejoueur[VRAI]->arme.name, "grenade")) // lejoueur[VRAI]->id_weapon==11
                        type_gre = 1;
                    else if (!strcmp(lejoueur[VRAI]->arme.name, "plasma")) // 12
                        type_gre = 2;
                    else if (!strcmp(lejoueur[VRAI]->arme.name, "fumigene"))
                        type_gre = 3;

                    envoi_Grenade(pos_gre, dir_gre, type_gre);
                }
            }
            else
            {
                collision_tir col_tir;
                if (world_is_loaded)
                    col_tir = world.check_tirs(yeux, boi - yeux);

                vec3_t tmp = boi - yeux;
                tmp.normalize();
                tmp = tmp * 5.0f;
                if (col_tir.nearest > 2 && col_tir.nearest <= distance_gun)
                {

                    int id_joueur = check_impact(yeux, boi, col_tir.pt);

                    vec3_t n;
                    int type_mark;
                    if (id_joueur == -1)
                        type_mark = 0;
                    else
                        type_mark = 1;

                    //*FRIENDLY j'ai rajouté le vrai dans le test pour eviter de recalculer la normal du tir qui n'est
                    //plus bonne
                    if ((col_tir.found == true) && !(id_joueur == VRAI))
                    {
                        n = col_tir.normal * -1.00f;

                        if (type_mark == 1)
                            addblood(lejoueur[id_joueur]->pos, boi - yeux);

                        marks->AddMark(n, col_tir.pt, 0);
                        parts->SystemNew(new pSystemBoom_t, col_tir.pt, 1);
                    }
                    else
                    {
                        n = (boi - yeux) * -100.0f;

                        col_tir.pt[0] = 0.0f;
                        col_tir.pt[1] = 0.0f;
                        col_tir.pt[2] = 0.0f;
                    }
                    envoi_Mark(col_tir.pt, n, fl2, type_mark); // envoi de la mark et du tire au bout du cannon
                }

                if (exterieur)
                {
                    parts->SystemNew(new pSystemBurst_t, fl, 1); // particle canon

                    marks->AddMark(fl - col_tir.pt, col_tir.pt, 20);
                }
                else
                {
                    //*SCOPE on ne fais pas le rendu du canon duqnd snipper
                    if (!sniper)
                    {
                        parts2->SystemNew(new pSystemBurst_t, lejoueur[VRAI]->vue.StudioCalcAttachments(true),
                                          1); // particle canon
                    }
                }
            }
        }

        lejoueur[VRAI]->tir = false;

        //******** CFT FRAME c le plus gros
        if ((CFT_ON == true) && (CFT_eta == 2))

        {
            if (lejoueur[VRAI]->QuelTeam == 0 && !lejoueur[VRAI]->mort)
            {
                if (FlagTR.collision_Flg(lejoueur[VRAI]->ID, lejoueur[VRAI]->pos, lejoueur[VRAI]->QuelTeam))
                {
                    CFT_envoi_message(0);
                    // CFT_affiche_message(VRAI,TEXT("a capture le flag des TERRO"));
                }
            }
            else
            {
                if (FlagCS.collision_Flg(lejoueur[VRAI]->ID, lejoueur[VRAI]->pos, lejoueur[VRAI]->QuelTeam))
                {
                    CFT_envoi_message(0);
                    // CFT_affiche_message(VRAI,TEXT("a capture le flag des COUNTER"));
                }
            }

            lockequipe(); //----------------------------------------------LOCK

            for (int k = 0; k < g_lNumberOfActivePlayers; k++)
            {
                if (lejoueur[k]->etat == true)
                {

                    if (FlagTR.ID == lejoueur[k]->ID)
                    {
                        FlagTR.pos_cur = lejoueur[k]->pos;
                    }
                    if (FlagCS.ID == lejoueur[k]->ID)
                    {
                        FlagCS.pos_cur = lejoueur[k]->pos;
                    }
                }
            }

            unlockequipe(); //----------------------------------------------LOCK

            if (lejoueur[VRAI]->QuelTeam == 0)
            {

                if (CmpCS.collision_Cmp(FlagTR.ID, lejoueur[VRAI]->ID, lejoueur[VRAI]->pos, lejoueur[VRAI]->QuelTeam))
                {
                    CFT_envoi_message(1);
                    // CFT_affiche_message(VRAI,TEXT("MARQUE pour les COUNTER"));
                    FlagCS.eta_gagne();
                    CFT_eta = 0;
                    CFT_nouvelle_partie();

                }
            }
            else
            {
                if (CmpTR.collision_Cmp(FlagCS.ID, lejoueur[VRAI]->ID, lejoueur[VRAI]->pos, lejoueur[VRAI]->QuelTeam))
                {
                    CFT_envoi_message(1);
                    // CFT_affiche_message(VRAI,TEXT("MARQUE pour les TERRO"));
                    FlagTR.eta_gagne();
                    CFT_eta = 0;
                    CFT_nouvelle_partie();


                }
            }
        }
        //** CFT fin du gros morceau dans frame****************************************/

        if (world_is_loaded)
            world.render_entvarsInvi(m_camera);
    }

    begin_orto();

    glEnable(GL_DEPTH_FUNC);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    if (!storm_off && mode_storm)
        glClearColor(0.6f, 0.5f, 0.5f, 1.0f);
    else
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    if (g_lNumberOfActivePlayers > 0)
    {

        AfficheCroix();
    }

    if ((m_overlay) || (config.isdebug))
    {
        overlay();
    }

    if (modechat && !menu_mode)
    {

        lesinput_box[9]->frame(m_input.xpos, m_input.ypos, m_width, m_height, m_input.left_button, config.isdebug,
                               delta);
    }

    if (!menu_mode)
        AffichePanel(delta);

    if (g_lNumberOfActivePlayers > 0)
    {
        lockequipe(); //----------------------------------------------LOCK
        m_pings->print(m_width, m_height, &lejoueur, CFT_nb_gign, CFT_nb_terro, CFT_ON, TEAM_ON, CFT_eta, CFT_count,
                       cft_timing, FRIENDLY, SNIPER_ON);
        unlockequipe(); //----------------------------------------------LOCK
    }

    if (m_phrase)
        m_phrase->draw(delta, m_height, m_width);

    if (m_romu)
    {
        m_romu->print(0, 0, m_width, m_height, delta, menu_state); // fond
    }

    //----------------------------------------
    for (int da = 0; da < lesconsoles.size(); da++)
    {
        if (lesconsoles[da])
        {
            lesconsoles[da]->visible =
                menu_mode &&
                    (((da == 0 || da == 2) && menu_state == 1100) //|| da==1
                     || ((da == 0 || da == 2) &&
                         (menu_state == 0 || menu_state == 444 || menu_state == 555 || menu_state == 990)) ||
                     ((da == 0 || da == 2) && (menu_state == 64)) || ((da == 0 || da == 2) && (menu_state == 51)) ||
                     ((da == 0 || da == 3 || da == 15) && (menu_state == 2)) ||
                     ((da == 0 || da == 4) && (menu_state == 110)) || ((da == 16) && (menu_state == 0)) ||
                     ((da == 0 || da == 444 || da == 5 || da == 6 || da == 7 || da == 8 || da == 9) &&
                      (menu_state == 151)) //(da==10 && PASS_ON) ||
                     || ((da == 0) && (menu_state == 0)) || ((da == 12 || da == 2) && menu_state == 62) ||
                     ((da == 0 || da == 2) && menu_state == 2565) // confirmation logout
                     || ((da == 12) && (menu_state == 2052 || menu_state == 51 || menu_state == 64 ||
                                        menu_state == 63 || (menu_state == 351 && tournoisv.size() == 0)))) ||
                ((da == 13) && (m_tchat > 0.0f));

            if (lesconsoles[da]->visible)
                lesconsoles[da]->frame(m_width, m_height, config.isdebug, delta);
        }
    }
    //-----------------------------------------
    for (int da = 0; da < lesinput_box.size(); da++)
    {
        if (da != 9)
        {
            if (lesinput_box[da])
            {
                lesinput_box[da]->visible =
                    menu_mode && (((da == 0 || da == 1) && menu_state == 444) ||
                                  ((da == 2 || da == 3 || da == 4) && menu_state == 555) ||
                                  ((da == 6 || (da == 5 && PASS_ON)) && menu_state == 151) ||
                                  ((da == 5) && menu_state == 110) || ((da == 7 || da == 8) && menu_state == 990)

                                 );

                lesinput_box[da]->actif = active_input == da;

                if (lesinput_box[da]->fond.m_mouseclick)
                    active_input = da;

                if (lesinput_box[da]->visible)
                    lesinput_box[da]->frame(m_input.xpos, m_input.ypos, m_width, m_height, m_input.left_button,
                                            config.isdebug, delta);
            }
        }
    }

    //-----------------------------------------
    for (int dao = 0; dao < lesoptionsbouton.size(); dao++)
    {
        lesoptionsbouton[dao]->frame(m_input.xpos, m_input.ypos, m_width, m_height, m_input.left_button, config.isdebug,
                                     delta);
    }

    if (lesbtn[40]->visible) // 11 c l'image de la map pas ici plus loin
        lesbtn[40]->frame(m_input.xpos, m_input.ypos, m_width, m_height, m_input.left_button, config.isdebug, delta);

    if (m_chat)
        m_chat->draw(delta, m_height, m_width);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_FUNC);

    if (menu_mode)
    {
        float x = (float)m_input.xpos;
        float y = (float)m_input.ypos;
        // invert y coord
        y = (float)m_height - y + 15;

        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex3f(x + 2, y - 16, 0.1f);
        glVertex3f(x, y, 0.1f);
        glVertex3f(x + 15, y - 15, 0.1f);

        glEnd();
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    end_orto();

    flip();
}

void Engine::addblood(vec3_t pos, vec3_t dir)
{
    vec3_t rnd_v;
    if (menu_state == 51 || menu_state == 64)
    {
        // world.process_visible_faces3(pos);
        collision_tir col_tir;
        for (int y = 0; y < 3; y++)
        {
            col_tir.found = false;

            rnd_v = vec3_t(random_t::RandomRange(-1.0f, 1.0f), random_t::RandomRange(-1.0f, 1.0f),
                           random_t::RandomRange(-1.0f, 1.0f));

            col_tir = world.check_tirs(pos, dir + rnd_v);
            if (col_tir.found == true)
            {

                marks->AddMark(col_tir.normal * -1.00f, col_tir.pt, 1);
            }
        }
    }
}

HRESULT Engine::colle_position(DPNID idplayer, GAMEMSG_POSITION *ret)
{
    // reception
    lockequipe();

    for (int i = 0; i < g_lNumberOfActivePlayers; i++)
    {
        if (lejoueur[i]->etat == true)
        {

            if (lejoueur[i]->ID == idplayer)
            {

                lejoueur[i]->is_car = false;
                // (!lejoueur[i]->is_car)
                //

                vec3_t dst;
                vec3_t vel;

                dst[0] = shortint_to_float(ret->pos[0]);
                dst[1] = shortint_to_float(ret->pos[1]);
                dst[2] = shortint_to_float(ret->pos[2]);


                vel[0] = shortint_to_float(ret->velocity[0]) * lejoueur[VRAI]->fps;
                vel[1] = shortint_to_float(ret->velocity[1]) * lejoueur[VRAI]->fps;
                vel[2] = shortint_to_float(ret->velocity[2]) * lejoueur[VRAI]->fps;
                lejoueur[i]->pos = dst;

                lejoueur[i]->velocity = vel;
                lejoueur[i]->rot[0] = shortint_to_float(ret->rot[0]);
                lejoueur[i]->rot[1] = shortint_to_float(ret->rot[1]);
                lejoueur[i]->rot[2] = shortint_to_float(ret->rot[2]);

                lejoueur[i]->modele.ang_dos = ret->ang_dos / 10.0f;

                lejoueur[i]->fps = shortint_to_float(ret->fps);
                lejoueur[i]->id_arme_recu = (int)ret->id_arme;
                lejoueur[i]->id_modele_recu = (int)ret->id_model;
                lejoueur[i]->sens = (int)ret->sens;
                lejoueur[i]->id_seq1_recu = ret->seq_dep;
                lejoueur[i]->fps1_recu = shortint_to_float(ret->num_dep);
                lejoueur[i]->id_seq2_recu = ret->seq_act;
                lejoueur[i]->fps2_recu = shortint_to_float(ret->num_act);
                lejoueur[i]->killed = ret->killed;

                bool est_mort;
                est_mort = ret->is_dead & 1;
                if (est_mort != lejoueur[i]->mort)
                {
                    lejoueur[i]->SpawnSmoke();
                }
                lejoueur[i]->mort = est_mort;
                lejoueur[i]->is_car = ret->is_dead & 2;
            }
        }
    }
    unlockequipe();

    return S_OK;
}

void Engine::SetBackSoundProperties(D3DVECTOR *pvPosition, D3DVECTOR *pvVelocity)
{
    // Every change to 3-D sound buffer and listener settings causes
    // DirectSound to remix, at the expense of CPU cycles.
    // To minimize the performance impact of changing 3-D settings,
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings
    // method to execute all of the deferred commands at once.
    memcpy(&g_dsBufferParams.vPosition, pvPosition, sizeof(D3DVECTOR));
    memcpy(&g_dsBufferParams.vVelocity, pvVelocity, sizeof(D3DVECTOR));

    if (g_pDS3DBuffer)
        g_pDS3DBuffer->SetAllParameters(&g_dsBufferParams, DS3D_IMMEDIATE);
}

HRESULT Engine::envoi_position()
{
    if (g_lNumberOfActivePlayers > 0)
    {
        // Send a message to all of the players
        GAMEMSG_POSITION msgWave;
        msgWave.dwType = GAME_MSGID_POSITION;

        //	lescar[yy]->AxeHaut,lescar[yy]->AxeDevant

        if (!lejoueur[VRAI]->is_car)
        {
            msgWave.pos[0] = float_to_shortint(lejoueur[VRAI]->pos[0]);
            msgWave.pos[1] = float_to_shortint(lejoueur[VRAI]->pos[1]);
            msgWave.pos[2] = float_to_shortint(lejoueur[VRAI]->pos[2]);
            msgWave.rot[0] = float_to_shortint(lejoueur[VRAI]->rot[0]);
            msgWave.rot[1] = float_to_shortint(lejoueur[VRAI]->rot[1]);
            msgWave.rot[2] = float_to_shortint(lejoueur[VRAI]->rot[2]);

            msgWave.velocity[0] = float_to_shortint(lejoueur[VRAI]->velocity[0]);
            msgWave.velocity[1] = float_to_shortint(lejoueur[VRAI]->velocity[1]);
            msgWave.velocity[2] = float_to_shortint(lejoueur[VRAI]->velocity[2]);
        }

        msgWave.ang_dos = float_to_shortint(lejoueur[VRAI]->modele.ang_dos * 10.0f);
        msgWave.fps = float_to_shortint(letick);
        msgWave.id_model = lejoueur[VRAI]->id_modele;
        msgWave.id_arme = lejoueur[VRAI]->id_weapon;
        msgWave.seq_dep = lejoueur[VRAI]->modele.seq1;
        msgWave.seq_act = lejoueur[VRAI]->modele.seq2;

        msgWave.num_dep = float_to_shortint(lejoueur[VRAI]->modele.fps1);
        msgWave.num_act = float_to_shortint(lejoueur[VRAI]->modele.fps2);
        msgWave.killed = lejoueur[VRAI]->killed;
        msgWave.sens = lejoueur[VRAI]->sens;
        msgWave.is_dead = lejoueur[VRAI]->mort | (lejoueur[VRAI]->is_car << 1);

        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_POSITION);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;

        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 100, NULL, &hAsync,
                      DPNSEND_NOLOOPBACK | DPNSEND_NOCOMPLETE | DPNSEND_PRIORITY_LOW);
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MessageHandler
// Desc: Handler for DirectPlay messages.  This function is called by
//       the DirectPlay message handler pool of threads, so be careful of thread
//       synchronization problems with shared memory
//-----------------------------------------------------------------------------
void Engine::dpmsg(DWORD id)
{

    char t[1000];
    DWORD rr = id & 0x0000FFFF;
    sprintf(t, "dp:%i", rr);
    if (id != DPN_MSGID_RECEIVE)
        m_chat->addtext(t, 2);
    /*
    case	DPN_MSGID_OFFSET					0xFFFF0000
    case DPN_MSGID_ADD_PLAYER_TO_GROUP		( DPN_MSGID_OFFSET | 0x0001 )
    case DPN_MSGID_APPLICATION_DESC			( DPN_MSGID_OFFSET | 0x0002 )
    case DPN_MSGID_ASYNC_OP_COMPLETE			( DPN_MSGID_OFFSET | 0x0003 )
    case DPN_MSGID_CLIENT_INFO				( DPN_MSGID_OFFSET | 0x0004 )
    case DPN_MSGID_CONNECT_COMPLETE			( DPN_MSGID_OFFSET | 0x0005 )
    case DPN_MSGID_CREATE_GROUP				( DPN_MSGID_OFFSET | 0x0006 )
    case DPN_MSGID_CREATE_PLAYER				( DPN_MSGID_OFFSET | 0x0007 )
    case DPN_MSGID_DESTROY_GROUP				( DPN_MSGID_OFFSET | 0x0008 )
    case DPN_MSGID_DESTROY_PLAYER			( DPN_MSGID_OFFSET | 0x0009 )
    case DPN_MSGID_ENUM_HOSTS_QUERY			( DPN_MSGID_OFFSET | 0x000a )
    case DPN_MSGID_ENUM_HOSTS_RESPONSE		( DPN_MSGID_OFFSET | 0x000b ) reponse host
    case DPN_MSGID_GROUP_INFO				( DPN_MSGID_OFFSET | 0x000c )
    case DPN_MSGID_HOST_MIGRATE				( DPN_MSGID_OFFSET | 0x000d )
    case DPN_MSGID_INDICATE_CONNECT			( DPN_MSGID_OFFSET | 0x000e )
    case DPN_MSGID_INDICATED_CONNECT_ABORTED	( DPN_MSGID_OFFSET | 0x000f )
    case DPN_MSGID_PEER_INFO					( DPN_MSGID_OFFSET | 0x0010 )
    case DPN_MSGID_RECEIVE					( DPN_MSGID_OFFSET | 0x0011 )
    case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP	( DPN_MSGID_OFFSET | 0x0012 )
    case	DPN_MSGID_RETURN_BUFFER				( DPN_MSGID_OFFSET | 0x0013 )
    case DPN_MSGID_SEND_COMPLETE				( DPN_MSGID_OFFSET | 0x0014 )
    case DPN_MSGID_SERVER_INFO				( DPN_MSGID_OFFSET | 0x0015 )
    case	DPN_MSGID_TERMINATE_SESSION			( DPN_MSGID_OFFSET | 0x0016 )
    */
}

HRESULT WINAPI Engine::MessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
    // Try not to stay in this message handler for too long, otherwise
    // there will be a backlog of data.  The best solution is to
    // queue data as it comes in, and then handle it on other threads.

    // This function is called by the DirectPlay message handler pool of
    // threads, so be careful of thread synchronization problems with shared memory

    TServerConnectData *pServerConnectData;
    DPNMSG_RETURN_BUFFER *pdpMsgReturnBuffer;

    DPNMSG_INDICATE_CONNECT *pdpMsgIndicateConnect;

    switch (dwMessageId)
    {
    case DPN_MSGID_ENUM_HOSTS_RESPONSE: {
        PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg;
        pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE)pMsgBuffer;

        // Take note of the host response
        if (m_DPHostEnumHead.pNext)
            SessionsDlgNoteEnumResponse(pEnumHostsResponseMsg);
        break;
    }
    case DPN_MSGID_INDICATE_CONNECT: {

        pdpMsgIndicateConnect = (DPNMSG_INDICATE_CONNECT *)pMsgBuffer;

        pServerConnectData = (TServerConnectData *)HeapAlloc(GetProcessHeap(), 0, sizeof(TServerConnectData));
        strcpy(pServerConnectData->_cData, m_cur_map);
        strcpy(pServerConnectData->_Host, m_cur_host);

        pdpMsgIndicateConnect->dwReplyDataSize = sizeof(TServerConnectData);
        pdpMsgIndicateConnect->pvReplyData = pServerConnectData;

        break;
    }

    case DPN_MSGID_RETURN_BUFFER: {
        // Transtyper une fois pour toutes pour clarifier le code

        pdpMsgReturnBuffer = (DPNMSG_RETURN_BUFFER *)pMsgBuffer;

        // Libérer la mémoire qu'occupe le buffer

        HeapFree(GetProcessHeap(), 0, pdpMsgReturnBuffer->pvBuffer);

        break;
    }

    case DPN_MSGID_ASYNC_OP_COMPLETE: {
        PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
        pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

        if (pAsyncOpCompleteMsg->hAsyncOp == m_hEnumAsyncOp)
        {
            SessionsDlgEnumListCleanup();

            // The user canceled the DirectPlay connection dialog,
            // so stop the search
            // if m_bSearchingForSessions
            if (bConnectSuccess)
            {
                // menu_state=1;
                bConnectSuccess = FALSE;

                // m_bSearchingForSessions = FALSE;
            }
            // menu_state=51;

            m_hEnumAsyncOp = NULL;
        }
        break;
    }

    case DPN_MSGID_CONNECT_COMPLETE: {
        PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
        pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

        switch (pConnectCompleteMsg->hResultCode)
        {

        case DPNERR_ADDRESSING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ALREADYCLOSING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ALREADYCONNECTED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ALREADYDISCONNECTING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ALREADYINITIALIZED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ALREADYREGISTERED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_BUFFERTOOSMALL:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CANNOTCANCEL:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CANTCREATEGROUP:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CANTCREATEPLAYER:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CANTLAUNCHAPPLICATION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CONNECTIONLOST:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_CONVERSION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_DATATOOLARGE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_DOESNOTEXIST:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_DUPLICATECOMMAND:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ENDPOINTNOTRECEIVING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ENUMQUERYTOOLARGE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_ENUMRESPONSETOOLARGE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_EXCEPTION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_GENERIC:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_GROUPNOTEMPTY:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_HOSTING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_HOSTREJECTEDCONNECTION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_HOSTTERMINATEDSESSION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INCOMPLETEADDRESS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDADDRESSFORMAT:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDAPPLICATION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDCOMMAND:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDDEVICEADDRESS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDENDPOINT:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDFLAGS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDGROUP:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDHANDLE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDHOSTADDRESS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDINSTANCE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDINTERFACE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDOBJECT:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDPARAM:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDPASSWORD:
            m_chat->addtext("mot de passe invalide", 2);
            break;
        case DPNERR_INVALIDPLAYER:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDPOINTER:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDPRIORITY:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDSTRING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_INVALIDURL:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOCAPS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOCONNECTION:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOHOSTPLAYER:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOINTERFACE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOMOREADDRESSCOMPONENTS:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NORESPONSE:
            m_chat->addtext("DPNERR_NORESPONSE", 2);
            break;
        case DPNERR_NOTALLOWED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOTHOST:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOTREADY:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_NOTREGISTERED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_OUTOFMEMORY:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_PENDING:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_PLAYERALREADYINGROUP:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_PLAYERLOST:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_PLAYERNOTINGROUP:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_PLAYERNOTREACHABLE:
            m_chat->addtext("DPNERR_PLAYERNOTREACHABLE", 2);
            break;
        case DPNERR_SENDTOOLARGE:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_SESSIONFULL:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_TABLEFULL:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_TIMEDOUT:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_UNINITIALIZED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_UNSUPPORTED:
            m_chat->addtext("error dplay", 2);
            break;
        case DPNERR_USERCANCEL:
            m_chat->addtext("error dplay", 2);
            break;
        }

        PDPNMSG_CONNECT_COMPLETE pdpnmsgConnectComplete = reinterpret_cast<PDPNMSG_CONNECT_COMPLETE>(pMsgBuffer);


        m_hrConnectComplete = pConnectCompleteMsg->hResultCode;

        if (SUCCEEDED(pdpnmsgConnectComplete->hResultCode))
        {
            pServerConnectData = (TServerConnectData *)pConnectCompleteMsg->pvApplicationReplyData;

            // Set m_hrConnectComplete, then set an event letting
            // everyone know that the DPN_MSGID_CONNECT_COMPLETE msg
            // has been handled
            // SetEvent( m_hConnectCompleteEvent );

            sprintf(m_cur_map, "%s", pServerConnectData->_cData);
            sprintf(m_cur_host, "%s", pServerConnectData->_Host);
            if (strcmp(m_cur_host, "") == 0)
                sprintf(m_cur_host, "%s", pServerConnectData->_Host);

            menu_state = 63;
        }
        else
        {
            DXTRACE_ERR_NOMSGBOX(TEXT("DPN_MSGID_CONNECT_COMPLETE"), m_hrConnectComplete);

            if (m_hEnumAsyncOp)
            {
                m_pDP->CancelAsyncOperation(m_hEnumAsyncOp, 0);
            }
            if (!lan_mode)
                menu_state = 1;
            else
                menu_state = 990;
        }
        break;
    }
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgNoteEnumResponse()
// Desc: Stores them in the linked list, m_DPHostEnumHead.  This is
//       called from the DirectPlay message handler so it could be
//       called simultaneously from multiple threads.
//-----------------------------------------------------------------------------
HRESULT Engine::SessionsDlgNoteEnumResponse(PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg)
{
    HRESULT hr = S_OK;
    BOOL bFound;

    // This function is called from the DirectPlay message handler so it could be
    // called simultaneously from multiple threads, so enter a critical section
    // to assure that it we don't get race conditions.  Locking the entire
    // function is crude, and could be more optimal but is effective for this
    // simple sample
    EnterCriticalSection(&m_csHostEnum);

    DPHostEnumInfo *pDPHostEnum = m_DPHostEnumHead.pNext;
    DPHostEnumInfo *pDPHostEnumNext = NULL;
    const DPN_APPLICATION_DESC *pResponseMsgAppDesc = pEnumHostsResponseMsg->pApplicationDescription;

    // Look for a matching session instance GUID.
    bFound = FALSE;
    while (pDPHostEnum != &m_DPHostEnumHead)
    {
        if (pResponseMsgAppDesc->guidInstance == pDPHostEnum->pAppDesc->guidInstance)
        {
            bFound = TRUE;
            break;
        }

        pDPHostEnumNext = pDPHostEnum;
        pDPHostEnum = pDPHostEnum->pNext;
    }

    if (!bFound)
    {
        m_bEnumListChanged = TRUE;

        // If there's no match, then look for invalid session and use it
        pDPHostEnum = m_DPHostEnumHead.pNext;
        while (pDPHostEnum != &m_DPHostEnumHead)
        {
            if (!pDPHostEnum->bValid)
                break;

            pDPHostEnum = pDPHostEnum->pNext;
        }

        // If no invalid sessions are found then make a new one
        if (pDPHostEnum == &m_DPHostEnumHead)
        {
            // Found a new session, so create a new node
            pDPHostEnum = new DPHostEnumInfo;
            if (NULL == pDPHostEnum)
            {
                hr = E_OUTOFMEMORY;
                goto LCleanup;
            }

            ZeroMemory(pDPHostEnum, sizeof(DPHostEnumInfo));

            // Add pDPHostEnum to the circular linked list, m_DPHostEnumHead
            pDPHostEnum->pNext = m_DPHostEnumHead.pNext;
            m_DPHostEnumHead.pNext = pDPHostEnum;
        }
    }

    // Update the pDPHostEnum with new information
    TCHAR strName[MAX_PATH];
    TCHAR pstrName[MAX_PATH];

    if (pResponseMsgAppDesc->pwszSessionName)
    {
        DXUtil_ConvertWideStringToGeneric(strName, pResponseMsgAppDesc->pwszSessionName);
    }

    if (pResponseMsgAppDesc->dwFlags & DPNSESSION_REQUIREPASSWORD)
    {
        strcpy(pstrName, "privee");
    }

    else
    {
        strcpy(pstrName, "");
    }

    // Cleanup any old enum
    if (pDPHostEnum->pAppDesc)
    {
        SAFE_DELETE_ARRAY(pDPHostEnum->pAppDesc->pwszSessionName);
        SAFE_DELETE_ARRAY(pDPHostEnum->pAppDesc);
    }
    SAFE_RELEASE(pDPHostEnum->pHostAddr);
    SAFE_RELEASE(pDPHostEnum->pDeviceAddr);

    //
    // Duplicate pEnumHostsResponseMsg->pAddressSender in pDPHostEnum->pHostAddr.
    // Duplicate pEnumHostsResponseMsg->pAddressDevice in pDPHostEnum->pDeviceAddr.
    //
    if (FAILED(hr = pEnumHostsResponseMsg->pAddressSender->Duplicate(&pDPHostEnum->pHostAddr)))
    {
        DXTRACE_ERR(TEXT("Duplicate"), hr);
        goto LCleanup;
    }

    if (FAILED(hr = pEnumHostsResponseMsg->pAddressDevice->Duplicate(&pDPHostEnum->pDeviceAddr)))
    {
        DXTRACE_ERR(TEXT("Duplicate"), hr);
        goto LCleanup;
    }

    // Deep copy the DPN_APPLICATION_DESC from
    pDPHostEnum->pAppDesc = new DPN_APPLICATION_DESC;
    ZeroMemory(pDPHostEnum->pAppDesc, sizeof(DPN_APPLICATION_DESC));
    memcpy(pDPHostEnum->pAppDesc, pResponseMsgAppDesc, sizeof(DPN_APPLICATION_DESC));
    if (pResponseMsgAppDesc->pwszSessionName)
    {
        pDPHostEnum->pAppDesc->pwszSessionName = new WCHAR[wcslen(pResponseMsgAppDesc->pwszSessionName) + 1];
        wcscpy(pDPHostEnum->pAppDesc->pwszSessionName, pResponseMsgAppDesc->pwszSessionName);
    }

    // Update the time this was done, so that we can expire this host
    // if it doesn't refresh w/in a certain amount of time
    pDPHostEnum->dwLastPollTime = timeGetTime();

    // Check to see if the current number of players changed
    TCHAR szSessionTemp[MAX_PATH];
    if (pResponseMsgAppDesc->dwMaxPlayers > 0)
    {
        wsprintf(szSessionTemp, TEXT("%s (%d/%d) (%dms) %s"), strName, pResponseMsgAppDesc->dwCurrentPlayers,
                 pResponseMsgAppDesc->dwMaxPlayers, pEnumHostsResponseMsg->dwRoundTripLatencyMS, pstrName);
        //	lagg_ms=pEnumHostsResponseMsg->dwRoundTripLatencyMS; // update du lagg
    }
    else
    {
        wsprintf(szSessionTemp, TEXT("%s (%d) (%dms)  %s"), strName, pResponseMsgAppDesc->dwCurrentPlayers,
                 pEnumHostsResponseMsg->dwRoundTripLatencyMS, pstrName);
    }

    // if this node was previously invalidated, or the session name is now
    // different the session list in the dialog needs to be updated
    if ((pDPHostEnum->bValid == FALSE) || (_tcscmp(pDPHostEnum->szSession, szSessionTemp) != 0))
    {
        m_bEnumListChanged = TRUE;
    }
    _tcscpy(pDPHostEnum->szSession, szSessionTemp);

    // This host is now valid
    pDPHostEnum->bValid = TRUE;

LCleanup:
    LeaveCriticalSection(&m_csHostEnum);

    return hr;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgEnumListCleanup()
// Desc: Deletes the linked list, g_DPHostEnumInfoHead
//-----------------------------------------------------------------------------
VOID Engine::SessionsDlgEnumListCleanup()
{
    DPHostEnumInfo *pDPHostEnum = m_DPHostEnumHead.pNext;
    DPHostEnumInfo *pDPHostEnumDelete;

    while (pDPHostEnum != &m_DPHostEnumHead)
    {
        pDPHostEnumDelete = pDPHostEnum;
        pDPHostEnum = pDPHostEnum->pNext;

        if (pDPHostEnumDelete->pAppDesc)
        {
            SAFE_DELETE_ARRAY(pDPHostEnumDelete->pAppDesc->pwszSessionName);
            SAFE_DELETE_ARRAY(pDPHostEnumDelete->pAppDesc);
        }

        // Changed from array delete to Release
        SAFE_RELEASE(pDPHostEnumDelete->pHostAddr);
        SAFE_RELEASE(pDPHostEnumDelete->pDeviceAddr);
        SAFE_DELETE(pDPHostEnumDelete);
    }

    // Re-link the g_DPHostEnumInfoHead circular linked list
    m_DPHostEnumHead.pNext = &m_DPHostEnumHead;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgEnumHosts()
// Desc: Enumerates the DirectPlay sessions, and displays them in the listbox
//-----------------------------------------------------------------------------
HRESULT Engine::SessionsDlgEnumHosts()
{
    HRESULT hr;

    m_bEnumListChanged = TRUE;

    // Enumerate hosts
    DPN_APPLICATION_DESC dnAppDesc;
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = m_guidApp;

    // Enumerate all the active DirectPlay games on the selected connection
    hr = m_pDP->EnumHosts(&dnAppDesc,                         // application description
                          m_pHostAddress,                     // host address
                          m_pDeviceAddress,                   // device address
                          NULL,                               // pointer to user data
                          0,                                  // user data size
                          INFINITE,                           // retry count (forever)
                          0,                                  // retry interval (0=default)
                          INFINITE,                           // time out (forever)
                          NULL,                               // user context
                          &m_hEnumAsyncOp,                    // async handle
                          DPNENUMHOSTS_OKTOQUERYFORADDRESSING // flags
    );
    if (FAILED(hr))
        return DXTRACE_ERR(TEXT("EnumHosts"), hr);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgInitListbox()
// Desc: Initializes the listbox
//-----------------------------------------------------------------------------
VOID Engine::SessionsDlgInitListbox()
{
    // HWND hWndListBox = GetDlgItem( hDlg, IDC_GAMES_LIST );

    // Clear the contents from the list box, and
    // display "Looking for games" text in listbox
    /*SendMessage( hWndListBox, LB_RESETCONTENT, 0, 0 );
    if( m_bSearchingForSessions )
    {
        SendMessage( hWndListBox, LB_ADDSTRING, 0,
                     (LPARAM) TEXT("Looking for games...") );
    }
    else
    {
        SendMessage( hWndListBox, LB_ADDSTRING, 0,
                     (LPARAM) TEXT("Click Start Search to see a list of games.  ")
                              TEXT("Click Create to start a new game.") );
    }

    SendMessage( hWndListBox, LB_SETITEMDATA,  0, NULL );
    SendMessage( hWndListBox, LB_SETCURSEL,    0, 0 );

    // Disable the join button until sessions are found
    EnableWindow( GetDlgItem( hDlg, IDC_JOIN ), FALSE ); */
}

//-----------------------------------------------------------------------------
// Name: ConnectionsDlgOnOK()
// Desc: Stores the player name m_strPlayerName, and in creates a IDirectPlay
//       object based on the connection type the user selected.
//-----------------------------------------------------------------------------
HRESULT Engine::ConnectionsDlgOnOK()
{
    HRESULT hr;

    DWORD dwItems = 0;
    DPN_SERVICE_PROVIDER_INFO *pdnSPInfo = NULL;
    DWORD dwSize = 0;

    // Query for the enum host timeout for this SP
    DPN_SP_CAPS dpspCaps;
    ZeroMemory(&dpspCaps, sizeof(DPN_SP_CAPS));
    dpspCaps.dwSize = sizeof(DPN_SP_CAPS);

    if (FAILED(hr = m_pDP->GetSPCaps(&ptcpGuid, &dpspCaps, 0)))
        return DXTRACE_ERR(TEXT("GetSPCaps"), hr);

    // Set the host expire time to around 3 times
    // length of the dwDefaultEnumRetryInterval
    m_dwEnumHostExpireInterval = dpspCaps.dwDefaultEnumRetryInterval * 3;

    // Create a device address

    if (FAILED(hr = m_pDeviceAddress->SetSP(&ptcpGuid)))
        return DXTRACE_ERR(TEXT("SetSP"), hr);

    if (FAILED(hr = m_pHostAddress->SetSP(&ptcpGuid)))
        return DXTRACE_ERR(TEXT("SetSP"), hr);

    // The SP has been chosen, so move forward in the wizard
    m_hrDialog = NCW_S_FORWARD;
    //   EndDialog( hDlg, 0 );
    // delete(pGuid);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SessionsDlgCreateGame()
// Desc: Asks the user the session name, and creates a new DirectPlay session
//-----------------------------------------------------------------------------
HRESULT Engine::SessionsDlgCreateGame()
{
    HRESULT hr;
    // Display a modal multiplayer connect dialog box.
    //    EnableWindow( hDlg, FALSE );
    //  nResult = (int)DialogBox( m_hInst, MAKEINTRESOURCE(IDD_MULTIPLAYER_CREATE),
    //          hDlg, (DLGPROC) StaticCreateSessionDlgProc );
    // EnableWindow( hDlg, TRUE );

    // Stop the search if we are about to connect
    if (m_bSearchingForSessions)
    {
        //    CheckDlgButton( m_hDlg, IDC_SEARCH_CHECK, BST_UNCHECKED );
        //  SendMessage( m_hDlg, WM_COMMAND, IDC_SEARCH_CHECK, 0 );
    }

    m_bHostPlayer = TRUE;

    // Set peer info name
    WCHAR wszPeerName[MAX_PATH];
    if (m_playerfile)
        DXUtil_ConvertGenericStringToWide(wszPeerName, m_playerfile->player_name);
    else
        DXUtil_ConvertGenericStringToWide(wszPeerName, "toto");
    //	 _tcscpy( m_strLocalPlayerName, TEXT("Unknown") );
    // DXUtil_ConvertGenericStringToWide( wszPeerName, m_strLocalPlayerName  );

    // mbstowcs(wszPeerName,m_playerfile->player_name,30);
    //_tcscpy( wszPeerName, TEXT("MYAPP") );

    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = wszPeerName;

    // Set the peer info, and use the DPNOP_SYNC since by default this
    // is an async call.  If it is not DPNOP_SYNC, then the peer info may not
    // be set by the time we call Host() below.
    if (FAILED(hr = m_pDP->SetPeerInfo(&dpPlayerInfo, NULL, NULL, DPNOP_SYNC)))
    {

        return DXTRACE_ERR(TEXT("SetPeerInfo"), hr);
    }

    WCHAR wszSessionName[MAX_PATH];
    WCHAR pass[MAX_PATH];
    // mbstowcs(wszSessionName,m_playerfile->player_name,30);
    //_tcscpy( wszSessionName, TEXT("MYAPP") );

    if (m_playerfile)
        DXUtil_ConvertGenericStringToWide(wszSessionName, m_playerfile->player_name); // m_strSessionName
    else
        DXUtil_ConvertGenericStringToWide(wszSessionName, "DEBUG"); // m_strSessionName

    //	_tcscpy( m_strSessionName, TEXT("ssessUnknown") );
    // DXUtil_ConvertGenericStringToWide( wszSessionName, m_strSessionName );
    //	WCHAR wszMap[MAX_PATH];

    //	DXUtil_ConvertGenericStringToWide( wszMap, m_cur_map); //m_strSessionName

    // Setup the application desc
    DPN_APPLICATION_DESC dnAppDesc;
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = m_guidApp;
    dnAppDesc.pwszSessionName = wszSessionName;
    dnAppDesc.dwMaxPlayers = m_dwMaxPlayers;

    if (PASS_ON)
    {
        DXUtil_ConvertGenericStringToWide(pass, lesinput_box[5]->text.text); // m_strSessionName
        dnAppDesc.pwszPassword = pass;
        dnAppDesc.dwFlags = DPNSESSION_REQUIREPASSWORD;
    }
    else
        dnAppDesc.dwFlags = 0;

    char buff[100];
    char modej[5];
    strcpy(buff, "");
    strcpy(modej, "");

    if (CFT_ON)
        strcpy(modej, "CFT");
    else if (TEAM_ON)
        strcpy(modej, "TEA");
    else if (SNIPER_ON)
        strcpy(modej, "SNI");
    else
        strcpy(modej, "...");

    for (int dae = 0; dae < lesoptionsbouton.size(); dae++)
    {
        if (lesoptionsbouton[dae])
        {
            if ((lesoptionsbouton[dae]->le_btnON.visible))
            {
                les_armes_autorisees[dae] = '*';
            }
            else
            {
                les_armes_autorisees[dae] = '-';
            }
        }
    }
    sprintf(buff, "%s %i %s", modej, FRIENDLY, les_armes_autorisees);
    dnAppDesc.pvApplicationReservedData = buff;
    dnAppDesc.dwApplicationReservedDataSize = sizeof(buff);

    if (TRUE) // m_bMigrateHost
        dnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST;

    // Host a game on m_pDeviceAddress as described by dnAppDesc
    // DPNHOST_OKTOQUERYFORADDRESSING allows DirectPlay to prompt the user
    // using a dialog box for any device address information that is missing
    if (FAILED(hr =
                   m_pDP->Host(&dnAppDesc,        // the application desc
                               &m_pDeviceAddress, // array of addresses of the local devices used to connect to the host
                               1,                 // number in array
                               NULL, NULL,        // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
                               NULL,              // player context
                               DPNHOST_OKTOQUERYFORADDRESSING))) // flags
        return DXTRACE_ERR(TEXT("Host"), hr);

    // DirectPlay connect successful, so end dialog
    m_hrDialog = NCW_S_FORWARD;
    //  EndDialog( hDlg, 0 );
    // bConnectSuccess=TRUE;

    menu_state = 51;
    return S_OK;
}

HRESULT Engine::SessionsDlgDisplayEnumList(long xd, long yd, int x, int y)
{
    DPHostEnumInfo *pDPHostEnumSelected = NULL;
    GUID guidSelectedInstance;
    BOOL bFindSelectedGUID;
    BOOL bFoundSelectedGUID;

    int numb;

    // This is called from the dialog UI thread, SessionsDlgNoteEnumResponse
    // is called from the DirectPlay message handler threads so
    // they may also be inside it at this time, so we need to go into the
    // critical section first
    EnterCriticalSection(&m_csHostEnum);

    // Only update the display list if it has changed since last time
    m_bEnumListChanged = FALSE;

    bFindSelectedGUID = FALSE;
    bFoundSelectedGUID = FALSE;

    /*// Test to see if any sessions exist in the linked list
     */
    DPHostEnumInfo *pDPHostEnum = m_DPHostEnumHead.pNext;

    // If there are any sessions in list,
    // then add them to the listbox
    if (pDPHostEnum != &m_DPHostEnumHead)
    {
        // Clear the contents from the list box and enable the join button
        pDPHostEnum = m_DPHostEnumHead.pNext;
        numb = 0;

        while (pDPHostEnum != &m_DPHostEnumHead)
        {

            // Add host to list box if it is valid
            if (pDPHostEnum->bValid)
            {

                numb++;

                TCHAR szSessionTemp[MAX_PATH];

                wsprintf(szSessionTemp, TEXT("%s"), (LPARAM)pDPHostEnum->szSession);

                if (m_input.xpos > xd && m_input.xpos < xd * 3 && m_input.ypos > yd * 3 - numb * 20 - 100 &&
                    m_input.ypos < yd * 3 - (numb - 1) * 20 - 100)
                { // quit
                    glColor4f(0.0f, 0.3f, 1.0f, 0.5f);

                    GlQuad(xd, numb * 20 + yd + 100, xd * 3, numb * 20 + yd + 120);

                    if (m_input.left_button)
                    {
                        menu_state = 62;
                        char temp[4];
                        char temp1[100];

                        sprintf(temp1, "%s", pDPHostEnum->pAppDesc->pvApplicationReservedData);
                        sscanf(temp1, "%s %d %s", temp, &FRIENDLY, les_armes_autorisees);

                        if (SessionsDlgJoinGame(pDPHostEnum) == S_OK)
                        {

                            if (strcmp(temp, "CFT") == 0)
                            {
                                CFT_ON = true;
                                TEAM_ON = false;
                                //							SNIPER_ON=false;
                                m_chat->addtext("Mode capture de drapeau", 2);
                            }
                            else
                            {
                                if (strcmp(temp, "TEA") == 0)
                                {
                                    m_chat->addtext("Mode equipe", 2);

                                    CFT_ON = false;
                                    TEAM_ON = true;
                                    //								SNIPER_ON=false;
                                }
                                else
                                {
                                    if (strcmp(temp, "SNI") == 0)
                                    {
                                        m_chat->addtext("Mode S.N.I.P.E.R ", 2);

                                        CFT_ON = false;
                                        TEAM_ON = false;
                                        //									SNIPER_ON=true;
                                    }
                                    else
                                    {
                                        CFT_ON = false;
                                        TEAM_ON = false;
                                        //									SNIPER_ON=false;
                                    }
                                }
                            }

                            if (FRIENDLY == 1)
                                m_chat->addtext("Friendly fire", 2);

                            if (FRIENDLY == 2)
                                m_chat->addtext("Self fire", 2);

                            killed = 0;
                            killer = 0;

                            break;
                        }
                        else
                        {
                            m_bConnecting = FALSE;
                            menu_state = 2;
                        }
                    }
                }
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColor3f(1.0f, 1.0f, 0.0f);
                m_font->print(xd, yd + 100 + numb * 20, szSessionTemp);

                if (bFindSelectedGUID)
                {
                    // Look for the session the was selected before
                    if (pDPHostEnum->pAppDesc->guidInstance == guidSelectedInstance)
                    {
                        /*   SendMessage( hWndListBox, LB_SETCURSEL, nIndex, 0 );*/
                        bFoundSelectedGUID = TRUE;
                    }
                }
            }

            pDPHostEnum = pDPHostEnum->pNext;
        }
    }

    LeaveCriticalSection(&m_csHostEnum);

    return S_OK;
}

VOID Engine::SessionsDlgExpireOldHostEnums()
{
    DWORD dwCurrentTime = timeGetTime();

    // This is called from the dialog UI thread, SessionsDlgNoteEnumResponse
    // is called from the DirectPlay message handler threads so
    // they may also be inside it at this time, so we need to go into the
    // critical section first
    EnterCriticalSection(&m_csHostEnum);

    DPHostEnumInfo *pDPHostEnum = m_DPHostEnumHead.pNext;
    while (pDPHostEnum != &m_DPHostEnumHead)
    {
        // Check the poll time to expire stale entries.  Also check to see if
        // the entry is already invalid.  If so, don't note that the enum list
        // changed because that causes the list in the dialog to constantly redraw.
        if ((pDPHostEnum->bValid != FALSE) &&
            (pDPHostEnum->dwLastPollTime < dwCurrentTime - m_dwEnumHostExpireInterval))
        {
            // This node has expired, so invalidate it.
            pDPHostEnum->bValid = FALSE;
            m_bEnumListChanged = TRUE;
        }

        pDPHostEnum = pDPHostEnum->pNext;
    }

    LeaveCriticalSection(&m_csHostEnum);
}

HRESULT Engine::SessionsDlgJoinGame(DPHostEnumInfo *pDPHostEnumSelected)
{
    HRESULT hr;

    DWORD dwPort = 0;

    m_bHostPlayer = FALSE;

    // Add status text in list box
    // nItemSelected = (int)SendMessage( hWndListBox, LB_GETCURSEL, 0, 0 );

    EnterCriticalSection(&m_csHostEnum);

    if (NULL == pDPHostEnumSelected)
    {
        LeaveCriticalSection(&m_csHostEnum);
        //        MessageBox( hDlg, TEXT("There are no games to join."),
        //                  TEXT("DirectPlay Sample"), MB_OK );
        return S_FALSE;
    }

    m_bConnecting = TRUE;

    // Set the peer info
    WCHAR wszPeerName[MAX_PATH];
    if (m_playerfile)
        DXUtil_ConvertGenericStringToWide(wszPeerName, m_playerfile->player_name);
    else
        DXUtil_ConvertGenericStringToWide(wszPeerName, "110");

    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = wszPeerName;

    // Set the peer info, and use the DPNOP_SYNC since by default this
    // is an async call.  If it is not DPNOP_SYNC, then the peer info may not
    // be set by the time we call Connect() below.
    if (FAILED(hr = m_pDP->SetPeerInfo(&dpPlayerInfo, NULL, NULL, DPNOP_SYNC)))
        return DXTRACE_ERR(TEXT("SetPeerInfo"), hr);

    ResetEvent(m_hConnectCompleteEvent);

    // Connect to an existing session. DPNCONNECT_OKTOQUERYFORADDRESSING allows
    // DirectPlay to prompt the user using a dialog box for any device address
    // or host address information that is missing
    // We also pass in copies of the app desc and host addr, since pDPHostEnumSelected
    // might be deleted from another thread that calls SessionsDlgExpireOldHostEnums().
    // This process could also be done using reference counting instead.
    WCHAR pass[MAX_PATH];
    if (strlen(lesinput_box[5]->text.text) != 0)
    {
        DXUtil_ConvertGenericStringToWide(pass, lesinput_box[5]->text.text); // m_strSessionName
        pDPHostEnumSelected->pAppDesc->pwszPassword = pass;
    }
    hr = m_pDP->Connect(
        pDPHostEnumSelected->pAppDesc,      // the application desc
        pDPHostEnumSelected->pHostAddr,     // address of the host of the session
        pDPHostEnumSelected->pDeviceAddr,   // address of the local device the enum responses were received on
        NULL, NULL,                         // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
        NULL, 0,                            // user data, user data size
        NULL,                               // player context,
        NULL, &m_hConnectAsyncOp,           // async context, async handle,
        DPNCONNECT_OKTOQUERYFORADDRESSING); // flags
    if (hr != E_PENDING && FAILED(hr))
        return DXTRACE_ERR(TEXT("Connect"), hr);

    LeaveCriticalSection(&m_csHostEnum);

    // Set a timer to wait for m_hConnectCompleteEvent to be signaled.
    // This will tell us when DPN_MSGID_CONNECT_COMPLETE has been processed
    // which lets us know if the connect was successful or not.
    // SetTimer( hDlg, TIMERID_CONNECT_COMPLETE, 100, NULL );

    // Disable the create/join buttons until connect succeeds or fails
    // EnableWindow( GetDlgItem( hDlg, IDC_JOIN ), FALSE );
    // EnableWindow( GetDlgItem( hDlg, IDC_CREATE ), FALSE );

    return S_OK;
}

void Engine::Joueur_Creation(char *playername, DPNID ID, long cle_joueur)
{
    Aplayer *nouveau = new Aplayer;
    // lejoueur.
    lejoueur.push_back(nouveau);
    int i = g_lNumberOfActivePlayers;

    lejoueur[i]->ID = ID;
    lejoueur[i]->id_modele = 0;
    lejoueur[i]->id_weapon = 0;

    lejoueur[i]->id_modele_recu = 0;
    lejoueur[i]->id_arme_recu = 0;
    lejoueur[i]->mort = false;
    lejoueur[i]->occupe = false;

    strcpy(lejoueur[i]->dep, TEXT("idle1"));
    strcpy(lejoueur[i]->action, TEXT("aim"));
    strcpy(lejoueur[i]->str_dep, "jump");
    strcpy(lejoueur[i]->str_act, "");
    strcpy(lejoueur[i]->playername, playername);
    // cyril ajout du initparticles ici et non pas dans engine.init
    lejoueur[i]->init_particle(&world);
    lejoueur[i]->is_car = false;

    if (g_dpnidLocalPlayer == ID)
    { // c moi

        //*SCOPE
        //	ETA_SCOPE=false;

        if (mode_op)
            lejoueur[i]->affecte_modele(leshommes, max_modele - 1, max_modele);
        else

            if ((round != 0) && (cle_tournois != 0))
        {
            if (m_playerfile)
                lejoueur[i]->affecte_modele(leshommes, m_playerfile->ekip_tournois, max_modele);
        }
        else
            lejoueur[i]->affecte_modele(leshommes, m_playerfile->player_model, max_modele);
    }
    else
    {
        lejoueur[i]->affecte_modele(leshommes, 0, max_modele);
    }

    lejoueur[i]->affecte_arme(lesarmes, 0, max_arme);

    // lejoueur[i]->affecte_arme (lesarmes,0,max_arme);
    lejoueur[i]->affecte_sound(lessons[lejoueur[i]->arme.id_son]);

    lejoueur[i]->affecte_effet((CSound *)lessons[10], 0); // reload
    lejoueur[i]->affecte_effet((CSound *)lessons[11], 1); // meur
    // lejoueur[i]->affecte_effet ((CSound*)lessons[0],2);//marche

    if (g_dpnidLocalPlayer == ID)
    {
        lejoueur[i]->local = true;
        VRAI = i;
        Change_arme(true);
    }
    else
    {
        lejoueur[i]->local = false;
    }

    lejoueur[i]->etat = true;

    char text[50];
    sprintf(text, "%s entre dans le jeu", playername);
    if (m_chat)
        m_chat->addtext(text, 2);

    //	o << i << endl;
    //	o << NB_MAX << endl;
    //	o << VRAI << endl;
    //}

    if (ID == g_dpnidLocalPlayer)
    {

        killed = 0;
        killer = 0;
        is_kikked = false;

        if ((m_bHostPlayer == TRUE))
        {
            TOURNOIS_ON = 0;

            if (config.isdebug == false && !lan_mode)
            {
                m_serverid = m_xmlsession.DevenirServer(
                    m_playerfile, m_cur_map, (CFT_ON | (TEAM_ON << 1) | (SNIPER_ON << 2)), m_dwMaxPlayers, cle_tournois,
                    round, cur_md5, lesinput_box[6]->text.text, PASS_ON, les_armes_autorisees); //
                idpartie = m_xmlsession.JoinServer(m_playerfile, m_serverid);
            }
            menu_state = 51;
        }
        else
        {
            // vec3_t ret= world.RenvoiePosition(lejoueur[VRAI]->id_modele);
            // m_pivot.move( ret);

            if (m_playerfile && (config.isdebug == false && !lan_mode))
                idpartie = m_xmlsession.JoinServer(m_playerfile, m_serverid);
            // menu_state=64;
        }

        // m_pivot.move( ret);

        init_player(i); // bc le 15/05/2008  avant false , probleme de killed a 1 au départ
    }
    else
    {

        // lejoueur[i]->etat=false;
        lejoueur[i]->rot[0] = 0.0f;
        lejoueur[i]->rot[1] = -43.0f;
        lejoueur[i]->rot[2] = 0.0f;

        lejoueur[i]->velocity[0] = 0.0f;
        lejoueur[i]->velocity[1] = 0.0f;
        lejoueur[i]->velocity[2] = 0.0f;
        lejoueur[i]->modele.ang_dos = 49.2f;

        lejoueur[i]->fps = 1;
        lejoueur[i]->sens = 1;
        lejoueur[i]->id_seq1_recu = 0;
        lejoueur[i]->fps1_recu = 15;
        lejoueur[i]->id_seq2_recu = 18;
        lejoueur[i]->fps2_recu = 24;
        // lejoueur[i]->Last_pos();
        lejoueur[i]->pos = world.RenvoiePosition(lejoueur[i]->id_modele);
        lejoueur[i]->anim();
        // lejoueur[i]->SpawnSmoke();
    }

    init_player(i); // bc le 15/05/2008  avant false , probleme de killed a 1 au départ

    JoueUnSon(19, vec3_t(0.0f, 0.0f, 0.0f));
}

void Engine::lockequipe()
{
    EnterCriticalSection(&m_csTeam);
}
void Engine::unlockequipe()
{
    LeaveCriticalSection(&m_csTeam);
}
void 

Engine::Joueur_Destroy(char * playername,DPNID ID)
{
    int trouve = -1;

    for (int i = 0; i < g_lNumberOfActivePlayers; i++)
    {
        if (lejoueur[i]->ID == ID)
        {

            char text[50];
            sprintf(text, "%s quitte le jeu", playername);
            if (m_chat)
                m_chat->addtext(text, (lejoueur[i]->QuelTeam == lejoueur[VRAI]->QuelTeam ? 0 : 1));
            trouve = i;
            /*lejoueur[i]->SpawnSmoke();
            lejoueur[i]->ID=0;
            lejoueur[i]->etat=false;*/

            // NB_MAX=lejoueur.size(); //sema
            JoueUnSon(18, vec3_t(0.0f, 0.0f, 0.0f));
            if (g_camera_suivi == ID)
                g_camera_suivi = prochain_a_suivre();
            break;
        }
    }
    if (trouve >= 0)
    {
        delete lejoueur[trouve];
        // lejoueur.erase (&lejoueur[trouve]);
        lejoueur.erase(lejoueur.begin() + trouve, lejoueur.begin() + trouve + 1);
    }

    //** CFT un joueur c casse on verifie qu'il avait pas le flag
    CFT_replace_flag(ID);
}

bool Engine::dezip(char *file)
{
    //	int processid=ShellExecute(m_hwnd, "open", file, NULL, NULL, SW_SHOWNORMAL);
    SECURITY_ATTRIBUTES sec;
    STARTUPINFO start;
    PROCESS_INFORMATION pinfo;
    memset(&start, 0, sizeof(STARTUPINFO));
    start.cb = sizeof(start);
    start.wShowWindow = SW_SHOWDEFAULT;
    sec.nLength = sizeof(sec);
    sec.lpSecurityDescriptor = NULL;
    sec.bInheritHandle = TRUE;
    if (CreateProcess(file, NULL, &sec, &sec, TRUE, 0, NULL, NULL, &start, &pinfo))
    {
        WaitForSingleObject(pinfo.hProcess, INFINITE);
        CloseHandle(pinfo.hThread);
        CloseHandle(pinfo.hProcess);
        return true;
    }
    else
    {
        return false;
    }
}

bool Engine::Verify_Map(char *file)
{
    bool existe;

    FILE *output_file;

    char chemin[100];
    sprintf(chemin, "data/map/%s.wad", file);

    // verifie lexistence
    // si ok exit
    output_file = fopen(chemin, "rb");
    if (output_file == NULL)
    {

        existe = false;
    }
    else
    {
        fclose(output_file);
        existe = true;
    }

    return existe;
}

bool Engine::Verify_Bmp(char *file)
{
    bool existe;

    FILE *output_file;

    char chemin[100];
    sprintf(chemin, "data/map/%s.bmp", file);

    // verifie lexistence
    // si ok exit
    output_file = fopen(chemin, "rb");
    if (output_file == NULL)
    {

        existe = false;
    }
    else
    {
        fclose(output_file);
        existe = true;
    }

    return existe;
}

bool Engine::Verify_Mp3(char *file)
{
    if (file)
    {

        bool existe = false;

        FILE *output_file;

        char chemin[100];
        sprintf(chemin, "data/mp3/%s.mp3", file);

        // verifie lexistence
        // si ok exit
        output_file = fopen(chemin, "rb");
        if (output_file == NULL)
        {
            existe = false;

            //	MessageBox (m_hwnd,TEXT("Attention download de la map. Cela va prendre quelques secondes
            //..."),TEXT("ROMUSTRIKE"),MB_OK); sinon dwlolad dezip exit
        }
        else
        {
            fclose(output_file);
            existe = true;
        }

        return existe;
    }
    else
    {
        return true;
    }
}

void Engine::load_map(char *map)
{
    m_chat->addtext("load map", 2);

    char cbufferwad[50];
    char cbufferbsp[50];

    sprintf(cbufferwad, "data/map/%s.wad", map);
    sprintf(cbufferbsp, "data/map/%s.bsp", map);
    world.wad(cbufferwad);
    world.load(cbufferbsp, config.GAMMA);

    //** CFT init des flags a voir ou il faut le faire
    // CFT_ON=true;
    CFT_eta = 0;
    CFT_count = 0;
    CFT_old_count = 0;
    CFT_init_les_flags();

    if (CFT_ON)
    {
        // CFT_init_les_flags();
        FlagTR.eta_depart();
        FlagCS.eta_depart();
        CmpTR.pos_cur = CmpTR.pos_ini;
        CmpCS.pos_cur = CmpCS.pos_ini;
    }

    CFT_nb_gign = 0;
    CFT_nb_terro = 0;
}

void Engine::LoadLesSons()
{
    float min = 250.0f;
    float max = 550.1f;

    ChargeUnSon("data/weapons/deagle.wav", 0, min, max); //
    ChargeUnSon("data/weapons/mac10.wav", 1, min, max);
    ChargeUnSon("data/weapons/p228.wav", 2, min, max);
    ChargeUnSon("data/weapons/mp5.wav", 3, min, max);
    ChargeUnSon("data/weapons/ak47.wav", 4, min, max);
    ChargeUnSon("data/weapons/m4a1.wav", 5, min, max);
    ChargeUnSon("data/weapons/aug.wav", 6, min, max);
    ChargeUnSon("data/weapons/awp1.wav", 7, min, max);
    ChargeUnSon("data/weapons/pompe.wav", 8, min, max);
    ChargeUnSon("data/weapons/m249.wav", 9, min, max);

    // effet
    ChargeUnSon("data/sound/reload.wav", 10, min, max);      //
    ChargeUnSon("data/sound/mort.wav", 11, min, max);        //
    ChargeUnSon("data/sound/pl_dirt1.wav", 12, min, max);    //
    ChargeUnSon("data/sound/pl_dirt2.wav", 13, min, max);    //
    ChargeUnSon("data/sound/pl_dirt3.wav", 14, min, max);    //
    ChargeUnSon("data/sound/pl_dirt4.wav", 15, min, max);    //
    ChargeUnSon("data/sound/headshot.wav", 16, min, max);    //
    ChargeUnSon("data/sound/humiliation.wav", 17, min, max); //
    ChargeUnSon("data/sound/outgame.wav", 18, min, max);     //
    ChargeUnSon("data/sound/ingame.wav", 19, min, max);      //
    ChargeUnSon("data/sound/dk_FACE01.wav", 20, min, max);   //
    ChargeUnSon("data/sound/dk_YIPPIE.wav", 21, min, max);   //
    ChargeUnSon("data/sound/dk_EATSHT.wav", 22, min, max);   //
    ChargeUnSon("data/sound/dk_WANSOMe.wav", 23, min, max);  //
    ChargeUnSon("data/sound/ricochet.wav", 24, min, max);    //
    ChargeUnSon("data/sound/gre_explode.wav", 25, min, max);
    ChargeUnSon("data/sound/grenade.wav", 26, min, max);
    ChargeUnSon("data/sound/mis_explode.wav", 27, min, max);
    ChargeUnSon("data/sound/missile.wav", 28, min, max);
    ChargeUnSon("data/sound/breath.wav", 29, min, max);

    ChargeUnSon("data/sound/pl_dirt1.wav", 30, min, max);
    ChargeUnSon("data/sound/pl_dirt2.wav", 31, min, max);
    ChargeUnSon("data/sound/pl_dirt3.wav", 32, min, max);

    ChargeUnSon("data/sound/chat.wav", 33, min, max);
    ChargeUnSon("data/sound/jump.wav", 34, min, max);

    //** CFT load des sons
    ChargeUnSon("data/sound/flag_depart.wav", 35, min, max);
    ChargeUnSon("data/sound/flag_attrape.wav", 36, min, max);
    ChargeUnSon("data/sound/flag_gagne.wav", 37, min, max);

    ChargeUnSon("data/weapons/c4.wav", 38, min, max);
    ChargeUnSon("data/weapons/famas.wav", 39, min, max);
    ChargeUnSon("data/weapons/ump45.wav", 40, min, max);
    ChargeUnSon("data/weapons/awp2.wav", 41, min, max);
    // voix
    ChargeUnSon("data/sound/radio/suivezmoi.wav", 42, min, max);
    ChargeUnSon("data/sound/radio/bougez.wav", 43, min, max);
    ChargeUnSon("data/sound/radio/position.wav", 44, min, max);
    ChargeUnSon("data/sound/radio/roger.wav", 45, min, max);
    ChargeUnSon("data/sound/radio/couvrezmoi.wav", 46, min, max);
    ChargeUnSon("data/sound/radio/daccord.wav", 47, min, max);
    ChargeUnSon("data/sound/radio/infiltrez.wav", 48, min, max);
    ChargeUnSon("data/sound/radio/negatif.wav", 49, min, max);
    ChargeUnSon("data/sound/radio/renfort.wav", 50, min, max);
    ChargeUnSon("data/sound/radio/repere.wav", 51, min, max);
    ChargeUnSon("data/sound/radio/repli.wav", 52, min, max);
    ChargeUnSon("data/sound/radio/restezgroupe.wav", 53, min, max);

    ChargeUnSon("data/sound/damage2.wav", 54, min, max);
    ChargeUnSon("data/sound/damage1.wav", 55, min, max);
    ChargeUnSon("data/sound/impactmur1.wav", 56, min, max);
    ChargeUnSon("data/sound/impactmur2.wav", 57, min, max);
    ChargeUnSon("data/sound/impactmur3.wav", 58, min, max);
    ChargeUnSon("data/sound/impactmur4.wav", 59, min, max);
    ChargeUnSon("data/sound/storm1.wav", 60, min, max);
    ChargeUnSon("data/sound/storm2.wav", 61, min, max);
    ChargeUnSon("data/sound/storm3.wav", 62, min, max);
    ChargeUnSon("data/weapons/m4a2.wav", 63, min, max);

    // my_blob.SetSound(lessons[59]);
}
void Engine::JoueUnSon(int id, vec3_t lapos)
{

    if (lessons[id])
    {
        if (lessons[id]->IsSoundPlaying() == TRUE)
        {
            lessons[id]->Reset();
        }
        HRESULT hr;
        if (FAILED(hr = lessons[id]->Get3DBufferInterface(0, &lessons[id]->g_pDS3DBuffer)))
        {
            DXTRACE_ERR_NOMSGBOX(TEXT("Get3DBufferInterface"), hr);
            return;
        }

        lessons[id]->g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
        lessons[id]->g_pDS3DBuffer->GetAllParameters(&lessons[id]->g_dsBufferParams);

        // Set new 3D buffer parameters

        if ((lapos[0] == 0.0f) && (lapos[1] == 0.0f))
            lessons[id]->g_dsBufferParams.dwMode = DS3DMODE_DISABLE;
        else
            lessons[id]->g_dsBufferParams.dwMode = DS3DMODE_NORMAL;

        D3DVECTOR ee;
        ee.x = lapos[0];
        ee.y = lapos[1];
        ee.z = lapos[2];
        D3DVECTOR vv;
        vv.x = 0.0f;
        vv.z = 0.0f;
        vv.y = 1.0f;

        // lessons[id]->g_dsBufferParams.vPosition=lapos;
        lessons[id]->g_pDS3DBuffer->SetAllParameters(&lessons[id]->g_dsBufferParams, DS3D_IMMEDIATE);
        lessons[id]->Positionne3Dbuffer(&ee, &vv);

        lessons[id]->Play(0, 0);
    }
}
void Engine::load3d()
{

    // std::ofstream o("log/load3d.log");
    //  o << "start load" << endl;
    //  o << "cft load" << endl;

    //**CFT load des objets dans engine.load3d
    lesobjets[0].Load("data/model/flag.mdl");
    lesobjets[1].Load("data/model/flag.mdl");
    lesobjets[2].Load("data/model/base.mdl");
    lesobjets[3].Load("data/model/base.mdl");
    leshommes[0].Load("data/model/rosetti.mdl");
    leshommes[0].lie = 0;
    leshommes[1].Load("data/model/spaceboy.mdl");
    leshommes[1].lie = 0;

    leshommes[2].Load("data/model/kakashi.mdl");
    leshommes[2].lie = 0;

    leshommes[3].Load("data/model/assassin.mdl");
    leshommes[3].lie = 0;

    leshommes[4].Load("data/model/skater.mdl");
    leshommes[4].lie = 0;

    leshommes[5].Load("data/model/pamela.mdl");
    leshommes[5].lie = 0;

    leshommes[6].Load("data/model/sandman.mdl");
    leshommes[6].lie = 0;

    leshommes[7].Load("data/model/agtx.mdl");
    leshommes[7].lie = 0;
    // 134
    leshommes[8].Load("data/model/sgtjones.mdl");
    leshommes[8].lie = 0;

    leshommes[9].Load("data/model/wolfgang.mdl");
    leshommes[9].lie = 0;

    leshommes[10].Load("data/model/forest.mdl");
    leshommes[10].lie = 0;

    leshommes[11].Load("data/model/mrblack.mdl");
    leshommes[11].lie = 0;

    leshommes[12].Load("data/model/ratman.mdl"); // ratman.mdl
    leshommes[12].lie = 0;

    leshommes[13].Load("data/model/operateur.mdl"); // operateur.mdl
    leshommes[13].lie = 0;

    max_modele = 14;

    T_armes liste_armes[max_arme] = {
        {"deagle", 1, "_onehanded", 0, 10, 10, 0, -1.0f, -1.0f, 1.0f, true},
        {"mp5", 1, "_mp5", 0, 20, 10, 3, 30.0f, 10.0f, 2.0f, true},
        {"fumigene", 1, "_hegrenade", 0, 5, 0, 26, -1.0f, 8.0f, 1.5f, true},
        {"p228", 1, "_onehanded", 0, 10, 10, 2, -1.0f, -1.0f, 1.5f, true},
        {"pompe", 1, "_m249", 0, 5, 80, 8, -1.0f, -1.0f, 1.0f, true},
        {"mac10", 1, "_onehanded", 0, 20, 20, 1, 40.0f, 10.0f, 2.5f, true},
        {"grenade", 1, "_hegrenade", 0, 5, 0, 26, -1.0f, 8.0f, 1.5f, false},
        {"m249", 1, "_m249", 0, 40, 10, 9, 30.0f, 10.0f, 2.0f, false},
        {"plasma", 1, "_m249", 0, 3, 0, 28, 22.0f, 22.0f, 3.0f, true},
        {"ak47", 1, "_mp5", 0, 15, 15, 4, 30.0f, 5.0f, 1.5f, true},
        {"c4", 1, "_c4", 0, 5, 0, 38, -1.0f, 8.0f, 1.5f, true},
        {"famas", 1, "_carbine", 0, 30, 20, 39, 22.0f, 8.0f, 1.5f, true},
        {"aug", 1, "_rifle", 10, 20, 20, 6, 30.0f, 10.0f, 1.5f, true},
        {"m4a1", 1, "_rifle", 10, 20, 20, 5, 30.0f, 5.0f, 1.5f, true},
        {"awp1", 1, "_rifle", 20, 10, 100, 7, -1.0f, -1.0f, 1.0f, true},
        {"ump45", 1, "_carbine", 0, 30, 20, 40, 30.0f, 8.0f, 2.0f, false},
        {"m4a2", 1, "_rifle", 10, 20, 25, 63, 30.0f, 5.0f, 1.0f, false},
        {"awp2", 1, "_rifle", 20, 10, 110, 41, -1.0f, -1.0f, 1.0f, false},

    };

    int offs;
    int offsy;
    offsy = 0;
    offs = 0;

    for (int ar = 0; ar < max_arme; ar++)
    {
        if (offsy > 10)
        {
            offsy = 0;
            offs++;
        }
        option_bouton *armes1 = new option_bouton;

        armes1->SetXY(10 + offs * 80, 900 - offs * 23 - offsy * 55, 475, 100);
        armes1->SetText(liste_armes[ar].file);
        armes1->le_texte.tps_vie = ar;
        armes1->SetFont(m_font);
        lesoptionsbouton.push_back(armes1);

        char files[100];
        sprintf(files, "data/weapons/p_%s.mdl", liste_armes[ar].file);

        // o << liste_armes[ar].file << endl;

        lesarmes[ar].Load(files);
        lesarmes[ar].lie = liste_armes[ar].lie;
        strcpy(lesarmes[ar].type, liste_armes[ar].type);
        sprintf(lesarmes[ar].name, liste_armes[ar].file);
        lesarmes[ar].scope = liste_armes[ar].scope;
        lesarmes[ar].balles = liste_armes[ar].balles;
        lesarmes[ar].puissance = liste_armes[ar].puissance;
        lesarmes[ar].id_son = liste_armes[ar].id_son;
        lesarmes[ar].fps_weapon = liste_armes[ar].fps_weapon_ext;
        lesarmes[ar].droitier = liste_armes[ar].droitier;
        lesarmes[ar].multi = 0;

        sprintf(files, "data/weapons/v_%s.mdl", liste_armes[ar].file);
        lesarmes[ar + max_arme].Load(files);
        lesarmes[ar + max_arme].lie = liste_armes[ar].lie;
        strcpy(lesarmes[ar + max_arme].type, liste_armes[ar].type);
        sprintf(lesarmes[ar + max_arme].name, liste_armes[ar].file);
        lesarmes[ar + max_arme].scope = liste_armes[ar].scope;
        lesarmes[ar + max_arme].balles = liste_armes[ar].balles;
        lesarmes[ar + max_arme].puissance = liste_armes[ar].puissance;
        lesarmes[ar + max_arme].id_son = liste_armes[ar].id_son;
        lesarmes[ar + max_arme].fps_weapon = liste_armes[ar].fps_weapon_vue;
        lesarmes[ar + max_arme].droitier = liste_armes[ar].droitier;
        lesarmes[ar + max_arme].multi = liste_armes[ar].multi;
        offsy++;
    }
}

//-----------------------------------------------------------------------------
// Name: GetPosition()
// Desc: Main collision detection function. This is what you call to get
//       a position.
//-----------------------------------------------------------------------------
vec3_t world_t::GetPosition(const vec3_t &position, const vec3_t &velocity)
{

    vec3_t scaledPosition, scaledVelocity;
    vec3_t finalPosition;

    // the first thing we do is scale the player and his velocity to
    // ellipsoid space
    scaledPosition = position / ellipsoidRadius;
    scaledVelocity = velocity / ellipsoidRadius;

    // call the recursive collision response function
    finalPosition = collideWithWorld(scaledPosition, scaledVelocity);

    // when the function returns the result is still in ellipsoid space, so
    // we have to scale it back to R3 before we return it
    finalPosition = finalPosition * ellipsoidRadius;

    return finalPosition;
}

//-----------------------------------------------------------------------------
// Name: collideWithWorld()
// Desc: Recursive part of the collision response. This function is the
//       one who actually calls the collision check on the meshes
//-----------------------------------------------------------------------------
vec3_t world_t::collideWithWorld(const vec3_t &position, const vec3_t &velocity)
{

    vec3_t pos;

    // do we need to worry ?
    if (velocity.len() < EPSILONN)
        return position;

    collision.foundCollision = FALSE;
    collision.stuck = FALSE;
    collision.nearestDistance = -1;

    vec3_t destinationPoint = position + velocity;

    // reset the collision package we send to the mesh
    collision.velocity = velocity;
    collision.sourcePoint = position;
    collision.eRadius = ellipsoidRadius;

    // list de face

    // Check all meshes

    CheckCollision();

    // check return value here, and possibly call recursively

    if (collision.foundCollision == FALSE)
    {
        // if no collision move very close to the desired destination.
        float l = velocity.len();
        vec3_t V = velocity;
        setLength(V, l - EPSILONN);

        // update the last safe position for future error recovery
        collision.lastSafePosition = position;

        // return the final position
        return position + V;
    }
    else
    { // There was a collision

        // drawBox(&collision.sourcePoint);

        // If we are stuck, we just back up to last safe position
        if (collision.stuck)
            return collision.lastSafePosition;

        // OK, first task is to move close to where we hit something :
        vec3_t newSourcePoint;

        // only update if we are not already very close
        if (collision.nearestDistance >= EPSILONN)
        {

            vec3_t V = velocity;
            setLength(V, collision.nearestDistance - EPSILONN);
            newSourcePoint = collision.sourcePoint + V;
        }
        else
            newSourcePoint = collision.sourcePoint;

        // Now we must calculate the sliding plane
        vec3_t slidePlaneOrigin = collision.nearestPolygonIntersectionPoint;
        vec3_t slidePlaneNormal = newSourcePoint - collision.nearestPolygonIntersectionPoint;

        // We now project the destination point onto the sliding plane
        double l = intersectRayPlane(destinationPoint, slidePlaneNormal, slidePlaneOrigin, slidePlaneNormal);

        // We can now calculate a new destination point on the sliding plane
        vec3_t newDestinationPoint;
        newDestinationPoint[0] = destinationPoint[0] + l * slidePlaneNormal[0];
        newDestinationPoint[1] = destinationPoint[1] + l * slidePlaneNormal[1];
        newDestinationPoint[2] = destinationPoint[2] + l * slidePlaneNormal[2];

        // Generate the slide vector, which will become our new velocity vector
        // for the next iteration
        vec3_t newVelocityVector = newDestinationPoint - collision.nearestPolygonIntersectionPoint;

        // now we recursively call the function with the new position and velocity
        collision.lastSafePosition = position;
        return collideWithWorld(newSourcePoint, newVelocityVector);
    }
}

//---------------------- BOUNDING BOX------------------------------------
bool world_t::isect_const_xyz(const int quoi, const float x, const vec3_t &p1, const vec3_t &p2, vec3_t &out)
{
    float rr = p2[quoi] - p1[quoi];

    if (rr != 0.0f)
    {
        float t = (x - p1[quoi]) / rr;
        if ((t >= 0.0f) && (t <= 1.0f))
        {
            // b + m*t);
            out[0] = p1[0] + (p2[0] - p1[0]) * t;
            out[1] = p1[1] + (p2[1] - p1[1]) * t;
            out[2] = p1[2] + (p2[2] - p1[2]) * t;

            return true;
        }
    }
    return false;
}

bool world_t::pip_const_x(const vec3_t &p, const short *mins, const short *maxs)
{
    if ((p[1] >= mins[1]) && (p[1] <= maxs[1]) && (p[2] >= mins[2]) && (p[2] <= maxs[2]))
        return true;
    else
        return false;
}

bool world_t::pip_const_y(const vec3_t &p, const short *mins, const short *maxs)
{
    if ((p[0] >= mins[0]) && (p[0] <= maxs[0]) && (p[2] >= mins[2]) && (p[2] <= maxs[2]))
        return true;
    else
        return false;
}

bool world_t::pip_const_z(const vec3_t &p, const short *mins, const short *maxs)
{
    if ((p[0] >= mins[0]) && (p[0] <= maxs[0]) && (p[1] >= mins[1]) && (p[1] <= maxs[1]))
        return true;
    else
        return false;
}

bool world_t::intersectbox(const vec3_t &p1, const vec3_t &p2, const short *vmin, const short *vmax)
{
    vec3_t s(p1);
    vec3_t e(p2);
    if (((s[0] >= vmin[0]) && (s[1] >= vmin[1]) && (s[2] >= vmin[2]) && (s[0] <= vmax[0]) && (s[1] <= vmax[1]) &&
         (s[2] <= vmax[2])) ||
        ((e[0] >= vmin[0]) && (e[1] >= vmin[1]) && (e[2] >= vmin[2]) && (e[0] <= vmax[0]) && (e[1] <= vmax[1]) &&
         (e[2] <= vmax[2])))
    {
        return true;
    }
    else
    {
        // otherwise do intersection check
        int i;
        vec3_t out;
        for (i = 0; i < 6; i++)
        {
            switch (i)
            {
            // left side, vmin.x is constant
            case 0:
                if (isect_const_xyz(0, vmin[0], s, e, out) && pip_const_x(out, vmin, vmax))
                    return true;
                break;
            case 1:
                if (isect_const_xyz(0, vmax[0], s, e, out) && pip_const_x(out, vmin, vmax))
                    return true;
                break;
            case 2:
                if (isect_const_xyz(1, vmin[1], s, e, out) && pip_const_y(out, vmin, vmax))
                    return true;
                break;
            case 3:
                if (isect_const_xyz(1, vmax[1], s, e, out) && pip_const_y(out, vmin, vmax))
                    return true;
                break;
            case 4:
                if (isect_const_xyz(2, vmin[2], s, e, out) && pip_const_z(out, vmin, vmax))
                    return true;
                break;
            case 5:
                if (isect_const_xyz(2, vmax[2], s, e, out) && pip_const_z(out, vmin, vmax))
                    return true;
                break;
            }
        }
    }
    return false;
}

int world_t::line_test(float v0, float v1, float w0, float w1)
{
    // quick rejection test
    if ((v1 < w0) || (v0 > w1))
        return OUTSIDE;
    else if ((v0 == w0) && (v1 == w1))
        return ISEQUAL;
    else if ((v0 >= w0) && (v1 <= w1))
        return ISCONTAINED;
    else if ((v0 <= w0) && (v1 >= w1))
        return CONTAINS;
    else
        return CLIPS;
}

int world_t::BoxIntersectBox(const short *vmin1, const short *vmax1, const short *vmin2, const short *vmax2)
{
    int and_code = 0xffff;
    int or_code = 0;
    int cx, cy, cz;
    cx = line_test(vmin1[0], vmax1[0], vmin2[0], vmax2[0]);
    and_code &= cx;
    or_code |= cx;
    cy = line_test(vmin1[1], vmax1[1], vmin2[1], vmax2[1]);
    and_code &= cy;
    or_code |= cy;
    cz = line_test(vmin1[2], vmax1[2], vmin2[2], vmax2[2]);
    and_code &= cz;
    or_code |= cz;
    if (or_code == 0)
        return OUTSIDE;
    else if (and_code != 0)
    {
        return and_code;
    }
    else
    {
        // only if all test produced a non-outside result,
        // an intersection has occured
        if (cx && cy && cz)
            return CLIPS;
        else
            return OUTSIDE;
    }
}

HRESULT Engine::envoi_Mark(vec3_t pos, vec3_t normal, vec3_t flash, int type_mark)
{
    if (g_lNumberOfActivePlayers > 1)
    {
        // Send a message to all of the players
        GAMEMSG_MARK msgWave;
        msgWave.dwType = GAME_MSGID_MARK;

        msgWave.pos = pos;
        msgWave.flash = flash;
        msgWave.rot = normal;
        msgWave.type_gre = type_mark;

        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_MARK);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;
        const DWORD timeout = 100;

        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 100, NULL, &hAsync,
                      DPNSEND_NOLOOPBACK | DPNSEND_NOCOMPLETE | DPNSEND_PRIORITY_LOW);
    }

    return S_OK;
}

HRESULT Engine::envoi_Grenade(vec3_t pos, vec3_t normal, int type_gre)
{
    if (g_lNumberOfActivePlayers > 1)
    {
        // Send a message to all of the players
        GAMEMSG_MARK msgWave;
        msgWave.dwType = GAME_MSGID_GRENADE;

        msgWave.pos = pos;
        msgWave.type_gre = type_gre;
        msgWave.rot = normal;
        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_MARK);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;
        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 100, NULL, &hAsync,
                      DPNSEND_NOLOOPBACK | DPNSEND_NOCOMPLETE | DPNSEND_PRIORITY_LOW);
    }

    return S_OK;
}

HRESULT Engine::colle_Mark(DPNID idplayer, GAMEMSG_MARK *ret)
{

    if (ret->pos[0] != 0.0f)
    {
        marks->AddMark(ret->rot, ret->pos, ret->type_gre);
        parts->SystemNew(new pSystemBoom_t, ret->pos, 1);
        parts->SystemNew(new pSystemSmoke_t, ret->pos, 1);
        lockequipe(); //----------------------------------------------LOCK

        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if (lejoueur[j]->ID == idplayer && j != VRAI)
            {
                D3DVECTOR lapos;
                // on poitonne le son de l'arme sur le joueur
                if (lejoueur[j]->playersound->IsSoundPlaying() == TRUE)
                {
                    lejoueur[j]->playersound->Reset();
                }
                lapos.x = lejoueur[j]->pos[0];
                lapos.y = lejoueur[j]->pos[1];
                lapos.z = lejoueur[j]->pos[2];
                if (lejoueur[j]->id_arme_recu == 1 || lejoueur[j]->id_arme_recu == 4 ||
                    lejoueur[j]->id_arme_recu == 6 || lejoueur[j]->id_arme_recu == 7 || lejoueur[j]->id_arme_recu == 8)
                {
                    lejoueur[j]->SpawnSmoke();
                }
                //	if (ret->type_gre==1)
                //		lejoueur[j]->SpawnBlood();
                //(lejoueur[j]->pos,(ret->pos-lejoueur[j]->pos)*2.0f);
                D3DVECTOR dd;
                dd.x = 0.0f;
                dd.y = 0.0f;
                dd.z = 0.0f;

                lejoueur[j]->playersound->Positionne3Dbuffer(&lapos, &dd);
                lejoueur[j]->playersound->Play(0, 0);
            }
        }
        unlockequipe(); //----------------------------------------------LOCK

        vec3_t e = (ret->flash - ret->pos) * -1.0f;
        e.normalize();
        marks->AddMark(ret->flash - ret->pos, ret->pos, 20); // creation de la balle tracante type mark==20

        vec3_t dis = lejoueur[VRAI]->pos - ret->pos;

        if (dis.len() < 150.0f)
        {
            int rnds = random_t::RandomRange(0, 3);
            JoueUnSon(56 + rnds, ret->pos);
        }
    }

    parts->SystemNew(new pSystemBurst_t, ret->flash, 1);

    return S_OK;
}

HRESULT Engine::colle_Grenade(DPNID idplayer, GAMEMSG_MARK *ret)
{

    lockequipe(); //----------------------------------------------LOCK
    for (int j = 0; j < g_lNumberOfActivePlayers; j++)
    {
        if (lejoueur[j]->ID == idplayer && j != VRAI)
        {
            D3DVECTOR lapos;

            lapos.x = lejoueur[j]->pos[0];
            lapos.y = lejoueur[j]->pos[1];
            lapos.z = lejoueur[j]->pos[2];

            D3DVECTOR dd;
            dd.x = 0.0f;
            dd.y = 0.0f;
            dd.z = 0.0f;

            lejoueur[j]->playersound->Positionne3Dbuffer(&lapos, &dd);
            lejoueur[j]->playersound->Play(0, 0);
        }
    }

    unlockequipe(); //----------------------------------------------LOCK

    if (ret->type_gre == 1)
    {

        // JoueUnSon (son_grenade, ret->pos);
        grenades->SystemNew(new pGrenSystemSimple_t, ret->pos, ret->rot, 0, idplayer);
    }
    else
    {
        if (ret->type_gre == 3)
        {
            //	JoueUnSon (son_grenade, ret->pos);
            grenades->SystemNew(new pGrenSystemSmoke_t, ret->pos, ret->rot, 0, idplayer);
        }
        else
        {
            if (ret->type_gre == 2)
            {
                //	JoueUnSon (son_missile, ret->pos);
                grenades->SystemNew(new pGrenSystemMissile_t, ret->pos, ret->rot, 0, idplayer);
            }
            else
            {
                //	JoueUnSon (son_missile, ret->pos);
                //
                grenades->SystemNew(new pGrenSystemLazer_t, ret->pos, ret->rot, 0, idplayer);
            }
        }
    }

    return S_OK;
}

HRESULT Engine::newchat(DPNID idplayer, char *playername, char *txt)
{
    lockequipe(); //----------------------------------------------LOCK
    int lid = VRAI;

    for (int j = 0; j < g_lNumberOfActivePlayers; j++)
    {
        if (j != VRAI)
        {
            if (lejoueur[j]->ID == idplayer)
            {
                lid = j;
            }
        }
    }

    char tsxt[150];
    sprintf(tsxt, "%s > %s", playername, txt);
    if (m_chat)
        m_chat->addtext(tsxt, (lejoueur[lid]->QuelTeam == lejoueur[VRAI]->QuelTeam ? 0 : 1));

    unlockequipe(); //----------------------------------------------LOCK

    return S_OK;
}

HRESULT Engine::envoi_Tir(DPNID dpnidPlayer, vec3_t pos, int force, int killed)
{
    if (g_lNumberOfActivePlayers > 1)
    {
        // Send a message to all of the players
        GAMEMSG_TIR msgWave;
        msgWave.dwType = GAME_MSGID_TIR;
        msgWave.pos = pos;
        msgWave.fps = force;
        msgWave.killed = killed;

        msgWave.ID_du_joueur_touche = dpnidPlayer;
        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_TIR);
        bufferDesc.pBufferData = (BYTE *)&msgWave;

        DPNHANDLE hAsync;
        m_pDP->SendTo(dpnidPlayer, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED);
    }

    return S_OK;
}
HRESULT Engine::envoi_chat(char *text)
{
    if (g_lNumberOfActivePlayers > 1)
    {
        GAMEMSG_CHAT msgChat;
        msgChat.dwType = GAME_MSGID_WAVE;

        wsprintf(msgChat.strChatString, TEXT("%s"), text);
        // strcpy(,"rere");//text

        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(msgChat.dwType) + (strlen(msgChat.strChatString) + 1) * sizeof(TCHAR);
        bufferDesc.pBufferData = (BYTE *)&msgChat;

        // Send it to all of the players include the local client
        // DirectPlay will tell via the message handler
        // if there are any severe errors, so ignore any errors
        DPNHANDLE hAsync;
        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED);
    }

    return S_OK;
}

HRESULT Engine::Confirme_Tue(DPNID dpnidPlayer, float dans_la_tete)
{
    //** CFT ici on le joueur a la confirme d'etre tue donc on replace le flag si besoin
    CFT_replace_flag(lejoueur[VRAI]->ID);
    if (g_lNumberOfActivePlayers > 0)
    {

        // Send a message to all of the players
        GAMEMSG_CONF_TUE msgWave;
        msgWave.dwType = GAME_MSGID_CONFIRME;
        msgWave.ID_du_joueur_touche = dpnidPlayer; // id du tueur

        msgWave.fps = dans_la_tete;
        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_CONF_TUE);
        bufferDesc.pBufferData = (BYTE *)&msgWave;
        DPNHANDLE hAsync;
        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync,
                      DPNSEND_GUARANTEED | DPNSEND_PRIORITY_HIGH);
    }
    return S_OK;
}

HRESULT Engine::Send_score(int score)
{ // envois son score a tous le monde
    if (g_lNumberOfActivePlayers > 0)
    {
        GAMEMSG_SCORE msgWave;
        msgWave.dwType = GAME_MSGID_SCORE;
        msgWave.score = score;
        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_SCORE);
        bufferDesc.pBufferData = (BYTE *)&msgWave;
        DPNHANDLE hAsync;
        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync,
                      DPNSEND_GUARANTEED | DPNSEND_PRIORITY_HIGH);
    }
    return S_OK;
}

HRESULT Engine::Send_voix(int id_voix)
{ // envois son score a tous le monde
    if (g_lNumberOfActivePlayers > 0)
    {
        GAMEMSG_VOIX msgWave;
        msgWave.dwType = GAME_MSGID_VOIX;
        msgWave.id_voix = id_voix;
        DPN_BUFFER_DESC bufferDesc;
        bufferDesc.dwBufferSize = sizeof(GAMEMSG_VOIX);
        bufferDesc.pBufferData = (BYTE *)&msgWave;
        DPNHANDLE hAsync;
        m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_PRIORITY_LOW);
    }
    return S_OK;
}

HRESULT Engine::Receive_score(DPNID idplayer, GAMEMSG_SCORE *ret)
{ // envois son score a tous le monde
    if (g_lNumberOfActivePlayers > 0)
    {
        lockequipe(); //----------------------------------------------LOCK

        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if (j != VRAI)
            {
                if (lejoueur[j]->ID == idplayer)
                {
                    lejoueur[j]->score = ret->score;
                }
            }
        }

        unlockequipe(); //----------------------------------------------LOCK
    }
    return S_OK;
}

HRESULT Engine::Receive_voix(DPNID idplayer, GAMEMSG_VOIX *ret)
{ // envois son score a tous le monde
    if (g_lNumberOfActivePlayers > 0)
    {
        lockequipe(); //----------------------------------------------LOCK

        for (int j = 0; j < g_lNumberOfActivePlayers; j++)
        {
            if (j != VRAI)
            {
                if (lejoueur[j]->ID == idplayer)
                {
                    if ((est_gign(lejoueur[j]->id_modele) && est_gign(lejoueur[VRAI]->id_modele)) ||
                        (!est_gign(lejoueur[j]->id_modele) && !est_gign(lejoueur[VRAI]->id_modele)))
                    {
                        lejoueur[j]->Voix(lessons[ret->id_voix]);
                        char eer[100];
                        sprintf(eer, "%s parle a son equipe", lejoueur[j]->playername);
                        m_chat->addtext(eer, 2);
                    }
                }
            }
        }

        unlockequipe(); //----------------------------------------------LOCK
    }
    return S_OK;
}

HRESULT Engine::score_plus(DPNID idplayer, GAMEMSG_CONF_TUE *ret)
{
    char head[20];

    if (lejoueur[VRAI]->ID == ret->ID_du_joueur_touche)
    {
        // on recoit confirmation ++ score
        if (ret->fps == 1000.0f)
        {
            JoueUnSon(son_head_shot, lejoueur[VRAI]->pos);
        }

        if (idplayer != lejoueur[VRAI]->ID) // on recoit son porpe messgae
        {
            m_playerfile->score++;
            killer++;
            lejoueur[VRAI]->score = killer;
            Send_score(killer);
            if (!lan_mode && !config.isdebug)
                m_xmlsession.scoreplus(m_playerfile, idpartie, killer, true); //,killed
        }

        static int son_precedent = 0;
        son_precedent++;
        if (son_precedent > 3)
            son_precedent = 0;

        JoueUnSon(20 + son_precedent, lejoueur[VRAI]->pos);
    }
    if (ret->fps == 1000.0f)
    {
        strcpy(head, "(headshot)");
    }
    else
    {
        strcpy(head, "");
    }

    int id_tueur = -1;
    int id_tuer = -1;
    lockequipe(); //----------------------------------------------LOCK

    for (int r = 0; r < lejoueur.size(); r++)
    {
        if (lejoueur[r]->ID == idplayer)
        {
            id_tuer = r;
        }
        if (lejoueur[r]->ID == ret->ID_du_joueur_touche)
        {
            id_tueur = r;
        }
    }

    char tmp[150];
    if (id_tueur != -1 && id_tuer != -1)
    {
        sprintf(tmp, "%s a tue %s %s", lejoueur[id_tueur]->playername, lejoueur[id_tuer]->playername, head);
        if (m_phrase)
            m_phrase->addtext(tmp, lejoueur[id_tueur]->QuelTeam);
        if (g_camera_suivi == lejoueur[id_tuer]->ID)
            g_camera_suivi = prochain_a_suivre();
    }
    else if (id_tuer != -1)
    {
        sprintf(tmp, "%s est mort", lejoueur[id_tuer]->playername, head);
        if (m_phrase)
            m_phrase->addtext(tmp, 0);
    }
    else
    {
        sprintf(tmp, "%s a tue %s", lejoueur[id_tueur]->playername, head);
        if (m_phrase)
            m_phrase->addtext(tmp, lejoueur[id_tueur]->QuelTeam);
    }

    lejoueur[id_tuer]->mort = true;
    unlockequipe(); //----------------------------------------------LOCK

    //** CFT ici on a recu l'info qu'un joueur est mort on verifie si il avait le flag
    if (CFT_ON)
        CFT_replace_flag(idplayer);

    return S_OK;
}

HRESULT Engine::recoit_Tir(DPNID idplayer, GAMEMSG_TIR *ret)
{

    lockequipe(); //----------------------------------------------LOCK
    for (int j = 0; j < g_lNumberOfActivePlayers; j++)
    {
        if (lejoueur[j]->etat == true && lejoueur[j]->killed == ret->killed)
        {

            if (lejoueur[j]->ID == ret->ID_du_joueur_touche)
            {
                lejoueur[j]->SpawnBlood();
                // JoueUnSon (son_touche,lejoueur[j]->pos);
                int rnds = random_t::RandomRange(0, 1);
                JoueUnSon(54 + rnds, lejoueur[j]->pos);

                if (j == VRAI && !mode_op && !(CFT_ON && CFT_eta == 1)) // c moi
                {
                    lejoueur[j]->vie = lejoueur[j]->vie - ret->fps;

                    m_panel->hit();
                    if (lejoueur[VRAI]->pulse < 5.0f)
                    {
                        lejoueur[VRAI]->pulse = 8.0f;
                    }

                    lejoueur[VRAI]->saut = true;
                    if (lejoueur[j]->vie <= 0)
                    {

                        // qui ma tué
                        vec3_t recul = vec3_t(0.0f, 0.0f, 0.0f);

                        if (ret->pos[0] == 0.0f && ret->pos[1] == 0.0f)
                        {

                            for (int qui = 0; qui < g_lNumberOfActivePlayers; qui++)
                            {
                                if (idplayer == lejoueur[qui]->ID)
                                    recul = lejoueur[VRAI]->pos - lejoueur[qui]->pos;
                            }
                            recul.normalize();
                            recul = recul * 2.0f;
                            recul[2] = 1.0f;
                            if (lejoueur[j]->mort == false && ret->fps == 1000.0f)
                                JoueUnSon(son_head_shot, lejoueur[j]->pos);
                        }
                        else
                        { // grenade
                            recul = lejoueur[j]->pos - ret->pos;
                            recul.normalize();
                            recul[2] = fabs(recul[2]);
                            recul = recul * ret->fps / 5.0f;
                        }
                        if (lejoueur[j]->mort == false)
                        {
                            Confirme_Tue(idplayer, ret->fps); // envoie confirmation au tueur
                            mourrir();
                            if (!lan_mode && !config.isdebug)
                                m_xmlsession.scoreplus(m_playerfile, idpartie, lejoueur[VRAI]->killed, false); //,killed
                        }
                        lejoueur[j]->velocity = recul;
                    }
                    else
                    {
                        if (lejoueur[j]->mort == false && ret->fps > 10.0f)
                            JoueUnSon(son_breath, lejoueur[j]->pos);
                    }
                }
            }
        }
    }
    unlockequipe(); //----------------------------------------------LOCK

    return S_OK;
}

vec3_t Engine::collision_joueur(vec3_t src, vec3_t dir)
{
    float dirlen = dir.len();
    //	float ledot;
    vec3_t dirn = dir;
    dirn.normalize();
    vec3_t retv = src + dir;
    lockequipe(); //----------------------------------------------LOCK
    for (int j = 0; j < g_lNumberOfActivePlayers; j++)
    {
        vec3_t moi_joueur = (lejoueur[j]->pos - (src + dir));

        // if  ((j!=VRAI) &&(moi_joueur.len()-30.0f<dirlen))
        if ((moi_joueur.len() < 30.0f) && (j != VRAI))
        {

            if (moi_joueur.len() < 25.0f)
                retv = src + vec3_t(random_t::RandomRange(-5.0f, 5.0f), random_t::RandomRange(-5.0f, 5.0f), 0.0f);
            else
                retv = src;
        }
    }
    unlockequipe(); //----------------------------------------------LOCK

    return (retv);
}

int Engine::check_impact(vec3_t src, vec3_t dest, vec3_t pos_mur)
{
    // premier test en gros pour voir qui peut etre touché entre les deux point
    vec3_t mini = pos_mur;
    int id_mini = -1;
    float dist_mur = 0.0f;
    float dist_joueur = 0.0f;
    // dist_mur= abs(sqrt(pow(dest[0]-pos_mur[0],2)+pow(dest[1]-pos_mur[1],2)+pow(dest[2]-pos_mur[2],2)));
    dist_mur = fabs(sqrt(pow(src[0] - pos_mur[0], 2) + pow(src[1] - pos_mur[1], 2)));
    lockequipe(); //----------------------------------------------LOCK

    for (int j = 0; j < g_lNumberOfActivePlayers; j++)
    {
        if (lejoueur[j]->etat == true && j != VRAI && lejoueur[j]->Visible == true)
        {
            vec3_t normal = dest - src;
            normal.normalize();

            float t = intersect_sphere(src, normal, lejoueur[j]->pos, 30);
            if (t > 0)
            {
                // on tape dans la sphere

                last_id = lejoueur[j]->modele.calculeboite(src, normal);
                if (last_id != -1)
                {
                    dist_joueur = fabs(sqrt(pow(src[0] - lejoueur[j]->modele.pos_touche[0], 2) +
                                            pow(src[1] - lejoueur[j]->modele.pos_touche[1], 2)));

                    if (dist_joueur < dist_mur)
                    {
                        id_mini = j;
                        dist_mur = dist_joueur;
                    }
                }
            }
        }
    }

    if (id_mini != -1)
    {
        int force;
        if (lejoueur[id_mini]->modele.idgroupe_touche == 1)
        {
            force = 1000.0f;
        }
        else
        {
            // force=lejoueur[VRAI]->arme.puissance/dist_mur;
            int puis = lejoueur[VRAI]->arme.puissance;

            force = floor(100.0f * puis / dist_mur);
            if (force < 5.0f)
                force = 5.0f;
        }

        //*FRIENDLY
        if ((FRIENDLY == 0) || !(lejoueur[id_mini]->QuelTeam == lejoueur[VRAI]->QuelTeam))
        {
            envoi_Tir(lejoueur[id_mini]->ID, vec3_t(0.0f, 0.0f, 0.0f), force, lejoueur[id_mini]->killed);
            lejoueur[id_mini]->SpawnBlood();
        }
        else
        {
            // on verifie si le lapin est un collegue
            if ((FRIENDLY == 2) && (lejoueur[id_mini]->QuelTeam == lejoueur[VRAI]->QuelTeam))
            {
                // ca te revient dans la gueule
                lejoueur[VRAI]->modele.pos_touche = lejoueur[id_mini]->modele.pos_touche;
                id_mini = VRAI;
                envoi_Tir(lejoueur[id_mini]->ID, vec3_t(0.0f, 0.0f, 0.0f), force, lejoueur[id_mini]->killed);
                lejoueur[id_mini]->SpawnBlood();
            }
            else
            {
                id_mini = -1;
            }
        }
    }
    unlockequipe(); //----------------------------------------------LOCK

    return id_mini;
}

void world_t::InitSkyBox()
{
    char *k;
    SkyRange = bsp->FloatForKey(&bsp->Lesentities[0], "MaxRange");

    k = bsp->ValueForKey(&bsp->Lesentities[0], "skyname");

    char ligne[30];
    Image img, img2, img3, img4, img5, img6;
    // devant
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("ft.tga"));
    // o << ligne << endl;
    img.load(ligne);
    TexturesSky[0] = new Texture(img.data(), img.width(), img.height(), GL_RGB);

    // droite
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("bk.tga"));
    // o << ligne << endl;
    img2.load(ligne);
    TexturesSky[1] = new Texture(img2.data(), img2.width(), img2.height(), GL_RGB);

    // bvack
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("lf.tga"));
    // o << ligne << endl;
    img3.load(ligne);
    TexturesSky[2] = new Texture(img3.data(), img3.width(), img3.height(), GL_RGB);

    // left
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("rt.tga"));
    // o << ligne << endl;
    img4.load(ligne);
    TexturesSky[3] = new Texture(img4.data(), img4.width(), img4.height(), GL_RGB);

    // top
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("up.tga"));
    // o << ligne << endl;
    img5.load(ligne);
    TexturesSky[4] = new Texture(img5.data(), img5.width(), img5.height(), GL_RGB);

    // dwn
    sprintf(ligne, "%s%s%s\0", TEXT("data\\env\\"), k, TEXT("dn.tga"));
    // o << ligne << endl;
    img6.load(ligne);
    TexturesSky[5] = new Texture(img6.data(), img6.width(), img6.height(), GL_RGB);

    // calcule de la boite englobante du bsp

    BspMin[0] = BspMin[1] = BspMin[2] = 100.0f;
    BspMax[0] = BspMax[1] = BspMax[2] = -100.0f;
    for (int c = 0; c < bsp->vertex_count; c++)
    {
        if (BspMin[0] > bsp->vertices[c].point[0])
            BspMin[0] = bsp->vertices[c].point[0];

        if (BspMin[1] > bsp->vertices[c].point[1])
            BspMin[1] = bsp->vertices[c].point[1];

        if (BspMin[2] > bsp->vertices[c].point[2])
            BspMin[2] = bsp->vertices[c].point[2];

        if (BspMax[0] < bsp->vertices[c].point[0])
            BspMax[0] = bsp->vertices[c].point[0];

        if (BspMax[1] < bsp->vertices[c].point[1])
            BspMax[1] = bsp->vertices[c].point[1];

        if (BspMax[2] < bsp->vertices[c].point[2])
            BspMax[2] = bsp->vertices[c].point[2];
    }

    vec3_t dist;
    // BspMax=BspMax+1000;
    // BspMin=BspMin-1000;
    dist[0] = fabs(BspMax[0] - BspMin[0]);
    dist[1] = fabs(BspMax[1] - BspMin[1]);
    dist[2] = fabs(BspMax[2] - BspMin[2]);
    SkyOri[0] = BspMax[0] - dist[0] / 2.0f;
    SkyOri[1] = BspMax[1] - dist[1] / 2.0f;
    SkyOri[2] = BspMax[2] - dist[2] / 2.0f;

    if ((dist[0] >= dist[1]) && (dist[0] >= dist[2]))
    {
        SkyTaille = dist[0];
    }
    else if ((dist[1] >= dist[0]) && (dist[1] >= dist[2]))
    {
        SkyTaille = dist[1];
    }
    else
    {
        SkyTaille = dist[2];
    }
}

void world_t::DessineSkyBox(vec3_t pos)
{

    // glDepthMask(0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
    glPushMatrix();
    glTranslatef(SkyOri[0], SkyOri[1], SkyOri[2]);
    glPushMatrix();
    glRotatef(0, 0, 0, 1);
    glRotatef(0, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    int i = 0;
    float x, y, z;
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    int SKYtaille = SkyTaille + 1000;

    // glDepthMask(1);
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);

    // face arriere
    glTexCoord2i(-1, 0);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, 1);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(-1, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glEnd();

    i++;
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    // face droite
    glTexCoord2i(0, 1);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(1, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(1, 1);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glEnd();

    i++;
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    // face avant
    glTexCoord2i(0, 1);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(1, 0);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(1, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glEnd();

    i++;
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    // face gauche
    glTexCoord2i(-1, 0);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(-1, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glEnd();

    i++;
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    // face dessus
    glTexCoord2i(0, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(1, 0);
    glVertex3f(x - SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(1, 1);
    glVertex3f(x + SKYtaille / 2.0f, y + SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glEnd();

    i++;
    TexturesSky[i]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    // face dessous
    glTexCoord2i(1, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glTexCoord2i(0, 0);
    glVertex3f(x - SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(0, -1);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z - SKYtaille / 2.0f);
    glTexCoord2i(1, -1);
    glVertex3f(x + SKYtaille / 2.0f, y - SKYtaille / 2.0f, z + SKYtaille / 2.0f);
    glEnd();
    glPopMatrix();
    glPopMatrix();
}

void world_t::InitPositions()
{

    // std::ofstream o("log/position.log");
    pos_gign.clear();
    pos_terro.clear();
    pos_armor.clear();
    pos_ladder.clear();

    for (int xx = 0; xx < bsp->model_count; xx++)
    {
        bsp->models[xx].unused = 0;
    }

    vec3_t ori;
    int v1, v2, v3;
    char *k;

    for (int x = 0; x < bsp->num_entities; x++)
    {
        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "flag_gign") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            flag_gign = ori;
            got_flag = true;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "flag_terro") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            flag_terro = ori;
            got_flag = true;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "flag_gign_rec") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            flag_gign_rec = ori;
            got_flag = true;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "flag_terro_rec") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            flag_terro_rec = ori;
            got_flag = true;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "info_player_start") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            pos_gign.push_back(ori);
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "info_player_deathmatch") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            pos_terro.push_back(ori);
            // o << k << "terro" <<endl;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "armoury_entity") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "origin");
            v1 = v2 = v3 = 0;
            sscanf(k, "%d %d %d", &v1, &v2, &v3);
            ori[0] = v1;
            ori[1] = v2;
            ori[2] = v3;
            pos_armor.push_back(ori);
        }
        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "func_ladder") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
            int kk = 0;
            sscanf(k, "*%d", &kk);
            pos_ladder.push_back(kk);
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "func_breakable") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
            int kk = 0;
            sscanf(k, "*%d", &kk);
            bsp->models[kk].unused = -1;
        }
        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "func_door") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
            // bsp->GetVectorForKey(&bsp->Lesentities[x],"origin",ori);
            // o << k << endl;
            int kk = 0;
            sscanf(k, "*%d", &kk);
            bsp->models[kk].unused = -1;
        }
        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "func_door_rotating") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
            // bsp->GetVectorForKey(&bsp->Lesentities[x],"origin",ori);
            // o << k << endl;
            int kk = 0;
            sscanf(k, "*%d", &kk);
            bsp->models[kk].unused = -1;
        }
        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "trigger_multiple") == 0))
        {
            k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
            // bsp->GetVectorForKey(&bsp->Lesentities[x],"origin",ori);
            // o << k << endl;
            int kk = 0;
            sscanf(k, "*%d", &kk);
            bsp->models[kk].unused = -1;
        }

        if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "classname"), "func_illusionary") == 0))
        {
            if ((strcmp(bsp->ValueForKey(&bsp->Lesentities[x], "skin"), "-1") == 0))
            {
                k = bsp->ValueForKey(&bsp->Lesentities[x], "model");
                int kk = 0;
                sscanf(k, "*%d", &kk);
                bsp->models[kk].unused = -2;
            }
        }
    }
    if (!got_flag)
    {
        if (pos_gign.size() > 0)
            flag_gign = pos_gign[0];
        if (pos_terro.size() > 0)
            flag_terro = pos_terro[0];
    }
    if (flag_terro_rec[0] == 0.0f && flag_gign_rec[0] == 0.0f)
    {
        flag_gign_rec = flag_gign;
        flag_terro_rec = flag_terro;
    }
}

vec3_t world_t::RenvoiePosition(bool gign)
{
    vec3_t ret = vec3_t(0.0f, 0.0f, 0.0f);

    int rnd;
    if (gign)
    {
        if (pos_gign.size() > 0)
        {
            rnd = floor(random_t::RandomRange(0.0f, pos_gign.size()));
            ret = pos_gign[rnd];
        }
    }
    else
    {
        if (pos_terro.size() > 0)
        {
            rnd = floor(random_t::RandomRange(0.0f, pos_terro.size()));
            ret = pos_terro[rnd];
        }
    }

    return (ret);
}
bool Engine::est_gign(int mod)
{
    return ((mod == 0) || (mod == 3) || (mod == 4) || (mod == 7) || (mod == 9) || (mod == 11));
}

void Engine::init_player(int i)
{

    lejoueur[i]->velocity = vec3_t(0.0f, 0.0f, 0.0f);
    strcpy(lejoueur[i]->str_dep, "");
    strcpy(lejoueur[i]->str_act, "");

    vec3_t ret = world.RenvoiePosition(est_gign(lejoueur[i]->id_modele));
    if (i == VRAI)
    {
        respawn_time = 0.0f;
        m_pivot.move(ret);
        // lejoueur[VRAI]->arme.munition =0;
        lejoueur[i]->vie = 100;

        lejoueur[i]->occupe = false;
        for (int r = 0; r < max_arme; r++)
        {
            int balles = lesarmes[r].balles;
            lesarmes[r].munition = balles;
            if (r == lejoueur[VRAI]->id_weapon)
                lejoueur[VRAI]->arme.munition = balles;
        }
    }

    lejoueur[i]->pos = ret;

    lejoueur[i]->mort = false;

    strcpy(lejoueur[i]->action, TEXT("aim"));
    strcpy(lejoueur[i]->dep, TEXT("idle1"));

    // release_tir=0;
}

DWORD Engine::get_lagg(DPNID id)
{
    DWORD ms;
    DPN_CONNECTION_INFO dpnConnectionInfo;

    // set the correct size
    ZeroMemory(&dpnConnectionInfo, sizeof(DPN_CONNECTION_INFO));
    dpnConnectionInfo.dwSize = sizeof(DPN_CONNECTION_INFO);

    // call GetConnectionInfo off of your interface

    HRESULT hr = m_pDP->GetConnectionInfo(id, &dpnConnectionInfo, 0);

    ms = dpnConnectionInfo.dwRoundTripLatencyMS;
    return ms;
}
void Engine::xmlsocket(char *xml)
{
    if (strlen(xml) != 0)
    {

        int type = m_xmlsession.GetType_tchat(xml);
        if ((type == GAME_TCHAT_MSG) && (romuchat_on))
        {
            JoueUnSon(son_chat, vec3_t(0.0f, 0.0f, 0.0f));

            char out_pseudo[SIZE_CHAINE];
            strcpy(out_pseudo, "");
            char out_msg[SIZE_CHAINE];
            strcpy(out_msg, "");
            char buff[SIZE_CHAINE];

            m_xmlsession.GetMsg_tchat(xml, out_pseudo, out_msg);
            sprintf(buff, "%s:%s", out_pseudo, out_msg);

            if (m_chat)
                m_chat->addtext(buff, 2);
        }
        else
        {
            if (type == GAME_TEAM_STOP)
            {
                if (!couvre_feu)
                {
                    old_model = lejoueur[VRAI]->id_modele;
                    lejoueur[VRAI]->affecte_modele(leshommes, max_modele - 2, max_modele);
                }
                couvre_feu = true;
                if (g_lNumberOfActivePlayers > 0)
                    JoueUnSon(son_breath, lejoueur[VRAI]->pos);
            }
            if (type == GAME_TEAM_START)
            {
                if (couvre_feu)
                    lejoueur[VRAI]->affecte_modele(leshommes, old_model, max_modele);

                couvre_feu = false;
                if (g_lNumberOfActivePlayers > 0)
                    JoueUnSon(son_breath, lejoueur[VRAI]->pos);
            }

            if (type == GAME_MODE_ECOUTE_ON)
            {

                mode_ecoute = true;
            }
            if (type == GAME_MODE_ECOUTE_OFF)
            {
                mode_ecoute = false;
            }

            if (type == GAME_TCHAT_SCROLL)
            {
                char out_msg[SIZE_CHAINE];
                strcpy(out_msg, "");

                m_xmlsession.GetScroll_tchat(xml, out_msg);
                m_romu->tchat_scroll(out_msg);
            }

            if (type == GAME_TCHAT_PINGS)
            {
                strcpy(m_txt_tchat, "");
            }
            if (type == GAME_TCHAT_KILL && !mode_op)
            {
                strcpy(m_txt_tchat, "");

                menu_mode = true;
                is_kikked = true;
                m_chat->addtext("tu a ete vire par un admin", 2);
            }
            if (type == GAME_STORM_START)
            {
                mode_storm = true;
                storm_off = mode_storm;
            }
            if (type == GAME_STORM_STOP)
            {
                mode_storm = false;
                storm_off = mode_storm;
            }
        }
    }
}

HRESULT Engine::envoi_who(void)
{
    if (m_playerfile && !lan_mode)
    {

        if ((le_socket != 0) && (!(strlen(m_playerfile->player_name) == 0)))
        {
            char chaine[SIZE_CHAINE];
            int vie = 100;
            if (g_lNumberOfActivePlayers > 0)
            {
                vie = lejoueur[VRAI]->vie;
            }

            if (g_lNumberOfActivePlayers > 0)
            {
                if (menu_state == 62)
                {
                    sprintf(chaine, "<who><pseudo>%s</pseudo><site>CNX JOUEUR %i</site><score>%i</score></who>\0",
                            m_playerfile->player_name, g_lNumberOfActivePlayers, m_playerfile->score);
                }
                else
                {
                    int x = lejoueur[VRAI]->pos[0];
                    int y = lejoueur[VRAI]->pos[1];
                    char rr[100];
                    strcpy(rr, "");
                    if (cle_tournois != 0)
                    {
                        sprintf(rr, "T-");
                    }
                    char sur_ecoute[5];
                    char desarme[5];
                    char sto[5];
                    char mode_jeu[20];
                    if (CFT_ON)
                        sprintf(mode_jeu, "CTF #%d", lejoueur[0]->QuelTeam + 1);
                    else if (TEAM_ON)
                        sprintf(mode_jeu, "TEAM #%d", lejoueur[0]->QuelTeam + 1);
                    else
                        sprintf(mode_jeu, "");

                    if (mode_ecoute)
                        sprintf(sur_ecoute, "1");
                    else
                        sprintf(sur_ecoute, "0");

                    if (couvre_feu)
                        sprintf(desarme, "1");
                    else
                        sprintf(desarme, "0");

                    if (storm_off)
                        sprintf(sto, "0");
                    else
                        sprintf(sto, "1");
                    int lescore;
                    if ((CFT_ON || TEAM_ON) && g_lNumberOfActivePlayers > 0)
                    {
                        if (lejoueur[0]->QuelTeam == 0)
                            lescore = CFT_nb_gign;
                        else
                            lescore = CFT_nb_terro;
                    }
                    else
                    {
                        lescore = m_playerfile->score;
                    }
                    sprintf(chaine,
                            "<who><pseudo>%s</pseudo><site>Partie en cours v%d</site><map>%s "
                            "%s</map><score>%i</score><life>%i</life><id_player>%i</id_player><host>%i</host><MODE>%i</"
                            "MODE><DESARME>%s</DESARME><ECOUTE>%s</ECOUTE><STORM>%s</STORM></who>\0",
                            m_playerfile->player_name, m_xmlsession.Version_Soft, m_cur_map, mode_jeu, lescore, vie,
                            m_playerfile->player_id, m_bHostPlayer, CFT_ON | (TEAM_ON << 1), desarme, sur_ecoute, sto);
                }
            }

            else
            {
                char title[30];
                switch (menu_state)
                {
                case 0:
                    sprintf(title, "Connection v%d", m_xmlsession.Version_Soft);
                    break;
                case 4:
                    sprintf(title, "Recherche session");
                    break;
                case 2:
                    sprintf(title, "Liste des parties");
                    break;
                case 151:
                    sprintf(title, "Creation %s", MapList.List[curmapid].Name);

                    break;
                case 110:
                    sprintf(title, "Rejoindre  %d", m_serverid);

                    break;
                default:
                    sprintf(title, "Etat %i", menu_state);
                    break;
                }

                sprintf(chaine, "<who><pseudo>%s</pseudo><site>%s</site><score>%i</score></who>\0",
                        m_playerfile->player_name, title, m_playerfile->score);
                if (downloading == INET_CHUNK)
                {
                    int pct = ((100 * total_down) / m_dwTotalSize);

                    sprintf(chaine,
                            "<who><pseudo>%s</pseudo><site>Dans le jeu</site><map>%s "
                            "%i/100</map><score>%i</score><life>0</life></who>\0",
                            m_playerfile->player_name, m_cur_map, pct, m_playerfile->score);
                }
            }

            send(le_socket, (LPCTSTR)chaine, strlen(chaine) + 1, NULL);
        }
    }
    return 1;
}

void Engine::model2str(int id_model, char *msg)
{
    switch (id_model)
    {
    case 0:
        strcpy(msg, "rosetti\0");
        break;
    case 1:
        strcpy(msg, "spaceboy\0");
        break;
    case 2:
        strcpy(msg, "kakashi\0");
        break;
    case 3:
        strcpy(msg, "assassin\0");
        break;
    case 4:
        strcpy(msg, "skaterboy\0");
        break;
    case 5:
        strcpy(msg, "pamela\0");
        break;
    case 6:
        strcpy(msg, "sandman\0");
        break;
    case 7:
        strcpy(msg, "agtx\0");
        break;
    case 8:
        strcpy(msg, "sgtjones\0");
        break;
    case 9:
        strcpy(msg, "wolfgang\0");
        break;
    case 10:
        strcpy(msg, "forest\0");
        break;
    case 11:
        strcpy(msg, "mr black\0");
        break;
    case 12:
        strcpy(msg, "ratman\0");
        break;
    case 13:
        strcpy(msg, "ops skin\0");
        break;
    }
}

HRESULT Engine::envoi_msg(char *msg)
{
    if (m_playerfile)
    {
        if ((strlen(msg) != 0) && ((le_socket != 0) && (!(strlen(m_playerfile->player_name) == 0))))
        {
            char chaine[SIZE_CHAINE];
            sprintf(chaine, "<racine><pseudo>%s</pseudo><msg>%s</msg></racine>\0", m_playerfile->player_name, msg);
            send(le_socket, (LPCTSTR)chaine, strlen(chaine) + 1, NULL);
        }
    }

    return 1;
}

HRESULT Engine::envoi_msg_ops(char *msg)
{
    if (m_playerfile)
    {
        if ((strlen(msg) != 0) && ((le_socket != 0) && (!(strlen(m_playerfile->player_name) == 0))))
        {
            char chaine[SIZE_CHAINE];
            sprintf(chaine, "<racine><pseudo>%s</pseudo><msg>%s</msg><priv>oui</priv></racine>\0",
                    m_playerfile->player_name, msg);
            send(le_socket, (LPCTSTR)chaine, strlen(chaine) + 1, NULL);
        }
    }

    return 1;
}

void Engine::DecaleArme()
{
    POINT p;
    p.x = m_xcenter, p.y = m_ycenter;
    if (!config.fullscreen)
    {
        ClientToScreen(m_hwnd, &p);
    }

    static int ping = 1;
    ping = ping * -1;
    SetCursorPos(p.x + 5 * ping, p.y - 5);
}

void Engine::CFT_init_les_flags()
{
    //** CFT init des classes flags declares dans engine.h
    FlagCS.affecte_modele(lesobjets, 0, 4);
    FlagCS.Team = 0; // gign
    FlagCS.pos_ini = world.flag_gign;
    FlagCS.affecte_son_init(lessons[35]);
    FlagCS.affecte_son_attrape(lessons[36]);
    FlagCS.affecte_son_gagne(lessons[37]);
    FlagCS.type = 1;
    FlagCS.modele.SetSkin(1);

    CmpCS.type = 0;
    CmpCS.affecte_modele(lesobjets, 2, 4);
    CmpCS.Team = 0; // gign
    CmpCS.pos_ini = world.flag_gign_rec;
    CmpCS.modele.SetSkin(2);

    FlagTR.affecte_modele(lesobjets, 1, 4);
    FlagTR.Team = 1; // terro
    FlagTR.pos_ini = world.flag_terro;

    FlagTR.affecte_son_init(lessons[35]);
    FlagTR.affecte_son_attrape(lessons[36]);
    FlagTR.affecte_son_gagne(lessons[37]);
    FlagTR.type = 1;
    FlagTR.modele.SetSkin(2);

    CmpTR.affecte_modele(lesobjets, 3, 4);
    CmpTR.Team = 1; // terro
    CmpTR.pos_ini = world.flag_terro_rec;
    CmpTR.type = 0;
    CmpTR.modele.SetSkin(1);
}

void Engine::CFT_nouvelle_partie()
{
    //** CFT init la partie
    if (m_chat)
    {

        m_chat->addtext("NOUVELLE PARTIE", 2);
    }

    if (CFT_ON)
    {
        FlagTR.eta_depart();
        FlagCS.eta_depart();
        CmpTR.pos_cur = CmpTR.pos_ini;
        CmpCS.pos_cur = CmpCS.pos_ini;
    }

    init_player(VRAI);
}

void Engine::CFT_renvoie_lib_team(int lateam, char *msg)
{

    if (lateam == 0)
        strcpy(msg, "GIGN\0");
    else
        strcpy(msg, "TERRO\0");
}

void Engine::CFT_replace_flag(DPNID idplayer)
{
    //** CFT replace le flag apres la mort du joueur

    if (FlagTR.ID == idplayer)
    {
        if (m_chat)
        {

            m_chat->addtext("-->Le Flag des TERRO est replace", 2);
        }
        FlagTR.eta_depart();
    }
    else
    {
        if (FlagCS.ID == idplayer)
        {
            m_chat->addtext("-->Le Flag des COUNTER est replace", 2);
            FlagCS.eta_depart();
        }
    }
}

void Engine::RESET()
{
    if (le_socket != 0)
        sockclose(le_socket);
    if (m_tex)
    {
        m_tex->Release();
        delete (m_tex);
    }
    if (m_tex_impact)
    {
        delete m_tex_impact;
        m_tex_impact = 0;
    }

    if (marks)
    {
        delete marks;
        marks = 0;
    }

    if (mp3)
        delete (mp3);
    if (parts)
    {
        delete parts;
        parts = 0;
    }

    if (parts2)
    {
        delete parts2;
        parts2 = 0;
    }

    if (grenades)
    {
        delete grenades;
        grenades = 0;
    }
    if (m_cross)
        SAFE_DELETE(m_cross)
    if (m_panel)
        SAFE_DELETE(m_panel);
    if (m_pings)
        SAFE_DELETE(m_pings);

    for (int d = 0; d <= max_son; d++)
    {
        if (lessons[d])
            SAFE_DELETE(lessons[d]);
    }
    for (int da = 0; da < lesbtn.size(); da++)
    {
        if (lesbtn[da])
            SAFE_DELETE(lesbtn[da]);
    }
    for (int da = 0; da < lesconsoles.size(); da++)
    {
        if (lesconsoles[da])
            SAFE_DELETE(lesconsoles[da]);
    }

    for (int da = 0; da < lesinput_box.size(); da++)
    {
        if (lesinput_box[da])
            SAFE_DELETE(lesinput_box[da]);
    }
    for (int dae = 0; dae < lesoptionsbouton.size(); dae++)
    {
        if (lesoptionsbouton[dae])
            SAFE_DELETE(lesoptionsbouton[dae]);
    }

    SAFE_DELETE(g_pSporte);

    SAFE_RELEASE(g_pDSListener);
    SAFE_RELEASE(g_pDS3DBuffer);

    SAFE_DELETE(g_pSoundManager);
    // fin sound bat
    DeleteCriticalSection(&m_csHostEnum);
    DeleteCriticalSection(&m_csTeam);
    CloseHandle(m_hConnectCompleteEvent);
    CloseHandle(m_hLobbyConnectionEvent);

    SAFE_RELEASE(m_pDeviceAddress);
    SAFE_RELEASE(m_pHostAddress);

    if (m_playerfile)
    {
        delete m_playerfile;
        m_playerfile = 0;
    }
    if (m_font)
    {
        delete m_font;
        m_font = 0;
    }

    if (m_romu)
    {
        delete m_romu;
        m_romu = 0;
    }

    if (m_chat)
    {
        delete m_chat;
        m_chat = 0;
    }
    if (m_phrase)
    {
        delete m_phrase;
        m_phrase = 0;
    }

    if (m_cross)
    {
        delete m_cross;
        m_cross = 0;
    }

    lejoueur.clear();

    for (int j = 0; j < max_modele; j++)
        leshommes[j].Cleanup();
    int a = 0;
    a = a;

    for (int i = 0; i < max_arme * 2; i++)
        lesarmes[i].Cleanup();
    for (int xxx = 0; xxx < 4; xxx++)
        lesobjets[xxx].Cleanup();
}

void Engine::kikage(char *str)
{

    lockequipe(); //----------------------------------------------LOCK
    char text[200] = "";
    char num[200] = "";
    int id_player = 0;
    sscanf(str, "%4s%1s%d", text, num, &id_player);
    if (!strcmp("KILL", text))
    {
        if (id_player > 0 && id_player < g_lNumberOfActivePlayers + 1)
        {
            if (m_bHostPlayer == TRUE)
            {
                GAMEMSG_KIKKED msgWave;
                msgWave.dwType = GAME_MSGID_CFT_KIKKED;
                msgWave.ID_du_joueur = lejoueur[id_player - 1]->ID;

                DPN_BUFFER_DESC bufferDesc;
                bufferDesc.dwBufferSize = sizeof(GAMEMSG_KIKKED);
                bufferDesc.pBufferData = (BYTE *)&msgWave;

                DPNHANDLE hAsync;

                m_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED);
            }
        }
    }
    unlockequipe(); //----------------------------------------------LOCK
}

void Engine::kikked(GAMEMSG_KIKKED *ret)
{
    char levire[200];

    // reception kiking
    // virer les concerné;
    lockequipe(); //----------------------------------------------LOCK

    for (int i = 0; i < g_lNumberOfActivePlayers; i++)
    {
        if (lejoueur[i]->ID == ret->ID_du_joueur)
        {
            sprintf(levire, "%s c est fait virer de la partie", lejoueur[i]->playername);
            m_chat->addtext(levire, 2);
        }
    }
    if (ret->ID_du_joueur == g_dpnidLocalPlayer && !mode_op)
    {
        menu_mode = true;
        is_kikked = true;
    }
    unlockequipe(); //----------------------------------------------LOCK
}
void Engine::Earth_quakeframe(float delta)
{
    if (tps_earthquake < 7.0f)
    {
        earth_quake = -1.0f * sinf(tps_earthquake * -50.0f) * exp(1.0f - tps_earthquake) / 5.0f; // de 4-10
        tps_earthquake = tps_earthquake + delta;
    }
    else
    {
        earth_quake = 0.0f;
    }
}

void __stdcall Engine::Juggler(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus,
                               LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
}

void Engine::mourrir(void)
{
    lejoueur[VRAI]->velocity = vec3_t(0.0f, 0.0f, 0.0f);
    lejoueur[VRAI]->mort = true;
    lejoueur[VRAI]->killed++;
    respawn_time = 5.0f;
    static int son_precedent = 0;
    son_precedent++;
    if (son_precedent > 3)
        son_precedent = 0;
    JoueUnSon(20 + son_precedent, lejoueur[VRAI]->pos);
    int rndo = random_t::RandomRange(0, 2);
    g_camera_suivi = prochain_a_suivre();
    if (CFT_ON)
    {
        CFT_eta = 0;
        CFT_count = 0;
        CFT_old_count = GetTickCount();
    }
    else if (TEAM_ON)
        CFT_eta = -2;
}
