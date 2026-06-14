/*
 * HL rendering engine
 * Copyright (c) 2000,2001 Bart Sekura
 *
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and
 * this permission notice appear in all copies of the software,
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * camera, core engine class
 */

#pragma warning(disable : 4996)
#pragma warning(disable : 4101)

#ifndef __engine_h__
#define __engine_h__
#ifndef _WINSOCKAPI_
#include <winsock.h>

#endif

#include "Model_MDL.h"
#include "glwin.h"
#include "font.h"
// #include "world.h"
#include "config.h"
#include "romu.h"  //bat
#include "chat.h"  //bat
#include "cross.h" //bat
#include "panel.h" //bat
#include "pings.h" //bat
#include "mp3.h"
// #include "NetConnect.h"
#include "playerfile.h" //bat
#include "geom.h"

#include "matrix.h"
#include <dplay8.h>
#include <dpaddr.h>
#include <tchar.h>
#include "player.h"
#include "flag.h"
// sound bat
#include "resource.h"
#include "dsound.h"
#include "dsutil.h"
#include "dxutil.h"
#include "xmlmessaging.h" //bat
#include <dshow.h>

#include "grenadeMan.h"

#include "ticker.h"
#include "texman.h"
#include "mark.h"

// #include "physics.h"
#include "bouton.h"
#include "console.h"
#include "input_box.h"
#include "option_bouton.h"
// #include <iostream>

// #include "rigid.h"
#include "MD5.h"
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
#include "blob.h"
#define GAME_MSGID_WAVE 1
#define GAME_MSGID_POSITION 2
#define GAME_MSGID_MARK 3
#define GAME_MSGID_TIR 4
#define GAME_MSGID_CONFIRME 5
#define GAME_MSGID_GRENADE 6
#define GAME_MSGID_CFT 7
#define GAME_MSGID_CFT_TOTALE 8
#define GAME_MSGID_CFT_KIKKED 9
#define GAME_MSGID_SCORE 10
#define GAME_MSGID_VOIX 11
#define cft_timing 3000
#define INET_IDLE 0
#define INET_CONNECT 1
#define INET_CHUNK 2
#define INET_DISCONNECT 3
#define INET_ERR 4

//-----------------------------------------------------------------------------
// Player context locking defines
//-----------------------------------------------------------------------------
//
#define max_arme 18
#define max_son 64 //	44

struct GAMEMSG_GENERIC
{
    BYTE dwType;
};

//** CFT
struct GAMEMSG_CFT : public GAMEMSG_GENERIC
{
    int type_du_message; // 0 = capture 1=gagne
};

struct GAMEMSG_CFT_TOTALE : public GAMEMSG_GENERIC
{
    DPNID Id_Flag_Cs; // id du joueur qui a le flag des CS
    DPNID Id_Flag_Tr; // id du joueur qui a le flag des TR
    int Nb_Cs;        // victoires
    int Nb_Tr;
};

struct GAMEMSG_CHAT : public GAMEMSG_GENERIC
{
    TCHAR strChatString[100];
};

struct GAMEMSG_POS : public GAMEMSG_GENERIC
{
    vec3_t pos;
    vec3_t rot;
    float ang_dos;
    float fps;
    int id_model;
    int id_arme;
    int seq_dep;
    float num_dep;
    int seq_act;
    float num_act;
    int sens;
    DPNID ID_du_joueur_touche;
};

struct GAMEMSG_CONF_TUE : public GAMEMSG_GENERIC
{
    float fps;
    DPNID ID_du_joueur_touche;
};

struct GAMEMSG_SCORE : public GAMEMSG_GENERIC
{
    int score;
};

struct GAMEMSG_VOIX : public GAMEMSG_GENERIC
{
    int id_voix;
};

struct GAMEMSG_TIR : public GAMEMSG_GENERIC
{
    vec3_t pos;
    int fps;
    BYTE killed;
    DPNID ID_du_joueur_touche;
};

struct GAMEMSG_MARK : public GAMEMSG_GENERIC
{
    vec3_t pos;
    vec3_t flash;
    vec3_t rot;
    int type_gre;
};
/*
struct GAMEMSG_POSITION : public GAMEMSG_GENERIC
{
    vec3_t pos; // 4*3
    vec3_t rot; // 4*3
    vec3_t velocity; // 4*3
    float ang_dos; // 4
    float fps; // 4
    BYTE id_model; // 1
    BYTE id_arme;// 1
    int seq_dep; // 4
    float num_dep; // 4
    int seq_act; // 4
    float num_act; // 4
    BYTE sens; // 1
    BYTE killed; // 1
    //total message = 12 +12 +12+ 4+ 4 +1+1+4+4+4+1+1
    // 64 Octets !!!!


};
*/

struct GAMEMSG_POSITION : public GAMEMSG_GENERIC
{
    float pos[3];      // 2*3 float  >>  pos *float_precision
    float rot[3];      // 2*3 float  >>  rot *float_precision
    float velocity[3]; // 2*3			*float_precision
                       //	float car_devant[3]; // 2*3			*float_precision
                       //	float car_haut[3]; // 2*3			*float_precision
    float ang_dos;     // 1        de -25.0f  a 25.0f float    >> ang_dos*10
    float fps;         // 2 fps_type positif float *float_precision
    BYTE id_model;     // 1 entier
    BYTE id_arme;      // 1 entier
    BYTE seq_dep;      // 1 entier
    float num_dep;     // 2     fps_type float *float_precision
    BYTE seq_act;      // 1 entier
    float num_act;     // 2    fps_type float *float_precision
    BYTE sens;         // 1 entier
    BYTE killed;       // 1 entier
    BYTE is_dead;
};
struct GAMEMSG_KIKKED : public GAMEMSG_GENERIC
{
    DPNID ID_du_joueur;
};

struct APP_PLAYER_INFO
{
    LONG lRefCount;          // Ref count so we can cleanup when all threads
                             // are done w/ this object
    DPNID dpnidPlayer;       // DPNID of player
    TCHAR strPlayerName[30]; // Player name
    LONG CLE_JOUEUR;         // Player name
};

//-----------------------------------------------------------------------------
// Defines, structures, and error codes
//-----------------------------------------------------------------------------
#define DISPLAY_REFRESH_RATE 250
#define TIMERID_DISPLAY_HOSTS 1
#define TIMERID_CONNECT_COMPLETE 2

#define NCW_S_FORWARD 0x01000001      // Dialog success, so go forward
#define NCW_S_BACKUP 0x01000002       // Dialog canceled, show previous dialog
#define NCW_S_QUIT 0x01000003         // Dialog quit, close app
#define NCW_S_LOBBYCONNECT 0x01000004 // Dialog connected from lobby, connect success

// #define NB_MAX           4

#define son_head_shot 16
#define son_humiliation 17
#define son_perfect 18
#define son_ingame 19
#define son_yourface 20
#define son_yippie 21
#define son_eatshit 22
#define son_wantsome 23
#define son_touche 24
#define son_explode 25
#define son_grenade 26
#define son_mis_explode 27
#define son_missile 28
#define son_breath 29
#define son_pas1 30
#define son_pas2 31
#define son_pas3 32
#define son_chat 33
#define son_jump 34
#define son_storm1 60
#define son_storm2 61
#define son_storm3 62

class DispList
{
  public:
    DispList()
    {
        m_id = glGenLists(1);
    }
    ~DispList()
    {
        glDeleteLists(m_id, 1);
    }

    void begin() const
    {
        glNewList(m_id, GL_COMPILE);
    }
    void end() const
    {
        glEndList();
    }
    void render() const
    {
        glCallList(m_id);
    }
    GLuint id() const
    {
        return m_id;
    }

  private:
    GLuint m_id;
};

///////////////////////////////////////////////////

// static int cutoff = 0;
static int geomNumMarks = 40;
static float float_precision = 100.0f;

//////////////////////////////////////////////////////
//
class Engine : public GLWindow
{ //

  public:
    PFNDPNMESSAGEHANDLER dphdl;
    int max_modele;
    bool is_op;
    int active_input;
    bool mode_op;
    int max_joueur;
    bool exterieur;
    bool phys;
    vector<tournois> tournoisv;
    int curr_tournois;
    int cle_tournois;
    int round;
    float msens;
    int maximun_arme;
    //*SCOPE ajout des variables
    bool SCOPE_ETA;
    bool BTN_DROIT;

    bool DisplayGamma;
    float gamadelta;
    char cur_md5[200];
    CMD5 md5;
    // Aplayer* nouveau;
    bool romuchat_on; // pour stoper le scroll des post depuis le site
    float tps_menu;
    bool release_tir_car;
    bool release_mouse;
    // simulation_world *pWorld ;
    SOCKET le_socket;
    SOCKET so;
    vector<ekip> lesekip;
    char m_password[100];
    char local_file[100];
    char distant_file[100];
    long m_dwTotalSize;
    long total_down;
    char *write_count;
    HINTERNET hOpen, hFile;
    WORD port;
    DWORD flags;
    DWORD dwSize, dwCode;
    FILE *output_file;
    HINTERNET hConnect;
    HINTERNET hReq;
    char inet_buffer[4097];
    IGraphBuilder *g_pGraphBuilder;
    Cmp3 *mp3;
    texMan_t *m_tex_impact;
    mMarkManager_t *marks;
    float m_tchat;
    char m_txt_tchat[500];
    vector<vec3_t> lespos;
    vector<vec3_t> lesposjoueur;
    vector<DPNID> lesid_joueur;
    // simulation_world *pWorld ;

    vector<DPNID> amoi;
    texMan_t *m_tex;
    pParticleManager_t *parts;
    pParticleManager_t *parts2;
    pGrenadeManager_t *grenades;

    config_t config;
    frameTimer_t frameTime;
    IDirectPlay8Peer *m_pDP;
    IGraphBuilder *m_pgraph;

    IDirectPlay8LobbiedApplication *m_pLB;
    DPNID g_dpnidLocalPlayer;
    LONG g_lNumberOfActivePlayers;
    DPNID g_camera_suivi_old;
    DPNID g_camera_suivi;
    float respawn_time;
    vec3_t vue_cam;
    float earth_quake;
    float tps_earthquake;
    // sound bat
    CSoundManager *g_pSoundManager;
    int offset;

    CSound *g_pSound;
    CSound *g_pSporte;
    LPDIRECTSOUND3DBUFFER g_pDS3DBuffer;   // 3D sound buffer
    LPDIRECTSOUND3DLISTENER g_pDSListener; // 3D listener object
    DS3DBUFFER g_dsBufferParams;           // 3D buffer properties
    DS3DLISTENER g_dsListenerParams;       // Listener properties
    blob my_blob;
    // sound bat
    bool lan_mode;
    bool storm_off;
    bool mode_storm;
    float time_storm;
    float eclaire[5];

    SOCKET Socket;
    Engine();
    // Engine();
    ~Engine();
    void SetpGuidApp(GUID *pGuidApp)
    {
        m_guidApp = *pGuidApp;
    }
    void Settcpguid(GUID *tcpguid)
    {
        ptcpGuid = *tcpguid;
    };
    void SetpDP(IDirectPlay8Peer *pDP)
    {
        m_pDP = pDP;
    };
    void SetpLB(IDirectPlay8LobbiedApplication *pLobbiedApp)
    {
        m_pLobbiedApp = pLobbiedApp;
    };
    void Sethinstance(HINSTANCE hinstance)
    {
        m_hInst = hinstance;
    };

    void Setlebuild(IGraphBuilder *lebuild)
    {
        m_pgraph = lebuild;
    };
    void Setpfn(const PFNDPNMESSAGEHANDLER pfn)
    {
        dphdl = pfn;
    };

    float float_to_shortint(float f)
    {

        return (f);
    };

    float shortint_to_float(float f)
    {
        return (f);
    };

    void JoueUnSon(int id, vec3_t lapos);
    void hit_Romu(void);
    void Joueur_Creation(char *playername, DPNID ID, long cle_joueur);
    void Joueur_Destroy(char *playername, DPNID ID);
    void lockequipe();
    void unlockequipe();
    void frame();
    void xmlsocket(char *xml);
    void ChargeUnSon(char *strFileName, int id, float min, float max);
    void init_les_sons();
    void RESET();
    void LoadLesSons();
    void init_sound();
    void init_porte();
    void donwload_frame();
    void Change_arme(bool plus);
    void DecaleArme();
    void Earth_quakeframe(float delta);
    void mourrir(void);
    void __stdcall Juggler(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation,
                           DWORD dwStatusInformationLength);
    int getAdresseMac(int numeroInterface, char *adrMAC);

    bool est_gign(int mod);
    void init_player(int id);
    void input_text_key();

    //** CFT
    void CFT_init_les_flags();
    void CFT_nouvelle_partie();
    void CFT_affiche_message(int r, char *mess);
    void CFT_renvoie_lib_team(int lateam, char *msg);
    HRESULT CFT_recoit_message(DPNID idplayer, GAMEMSG_CFT *ret);
    HRESULT CFT_envoi_message(int quoi);
    HRESULT CFT_HOST_envoi_recapitulatif();
    HRESULT CFT_HOST_recoit_recapitulatif(DPNID idplayer, GAMEMSG_CFT_TOTALE *ret);

    void CFT_replace_flag(DPNID idplayer);
    void kikage(char *str);

    HRESULT playbackground(DWORD flags);
    HRESULT ouvertureporte();
    void SetBackSoundProperties(D3DVECTOR *pvPosition, D3DVECTOR *pvVelocity);
    int killer;
    int killed;
    int menu_state; // bat 06062002
    int old_menu_state;
    int downloading; // bat 06062002
    int last_id;     // a viré debug

    HRESULT WINAPI MessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);
    void dpmsg(DWORD id);
    HRESULT envoi_position();
    // HRESULT colle_position(DPNID idplayer,vec3_t  pos,vec3_t  rot,float  ang_dos,float fps,int id_model,int
    // id_arme,char * dep ,char * act,int sens);
    HRESULT colle_position(DPNID idplayer, GAMEMSG_POSITION *ret);
    void addblood(vec3_t pos, vec3_t dir);
    void kikked(GAMEMSG_KIKKED *ret);

    BOOL bConnectSuccess;
    void migratehost();
    void load_map(char *map);
    void load3d();
    float blend;
    int type_download;
    char m_cur_map[100]; // nom map en cours
    char m_cur_host[50]; // serveur de download de la map
    float prev;
    int VRAI;
    bool relkeys[MAX_KEYS];
    unsigned long old_tick;  // pour temporisé les envois de position
    unsigned long old_tchat; // pour temporisé les envois de position
    unsigned long old_tick_team;
    unsigned long old_pas; // pour temporisé les envois de position
    unsigned int old_son;
    float speed; // movement speed//05/12/2002
    float cours; // movement speed//05/12/2002
    float fatigue;
    bool est_fatigue;
    float time_acceleration;
    HRESULT envoi_Mark(vec3_t pos, vec3_t normal, vec3_t flash, int type_mark);

    HRESULT envoi_Grenade(vec3_t pos, vec3_t normal, int type_gre);
    HRESULT envoi_chat(char *text);
    HRESULT colle_Mark(DPNID idplayer, GAMEMSG_MARK *ret);
    HRESULT colle_Grenade(DPNID idplayer, GAMEMSG_MARK *ret);
    HRESULT newchat(DPNID idplayer, char *playername, char *txt);
    HRESULT envoi_Tir(DPNID dpnidPlayer, vec3_t pos, int force, int killed);
    HRESULT recoit_Tir(DPNID idplayer, GAMEMSG_TIR *ret);
    HRESULT Confirme_Tue(DPNID dpnidPlayer, float dans_la_tete);
    HRESULT score_plus(DPNID idplayer, GAMEMSG_CONF_TUE *ret);
    HRESULT Receive_score(DPNID idplayer, GAMEMSG_SCORE *ret);
    HRESULT Receive_voix(DPNID idplayer, GAMEMSG_VOIX *ret);
    HRESULT Send_score(int score);
    HRESULT Send_voix(int id_voix);
    DPNID prochain_a_suivre(void);
    int check_impact(vec3_t src, vec3_t dest, vec3_t pos_mur);
    vec3_t collision_joueur(vec3_t src, vec3_t dir);

    DWORD get_lagg(DPNID id);
    Cchat *m_chat;
    Cchat *m_phrase;
    bool couvre_feu;
    bool mode_ecoute;
    int old_model;
    char MAC[100];

    bool dezip(char *file);

  protected:
    ////////////////////////////////////////////////////////////

    // Model_MDL model;
    // Model_MDL arme;
    int le_rendu;

    int souris_inverse;
    DWORD lagg_ms;
    long idpartie;
    Model_MDL leshommes[14];
    Model_MDL lesarmes[max_arme * 2];

    //** CFT declarations des donnees
    Model_MDL lesobjets[4];

    AFlag FlagCS;
    AFlag FlagTR;
    AFlag CmpCS;
    AFlag CmpTR;
    bool PASS_ON;
    int FRIENDLY;
    bool CFT_ON;    // gros booleen qui determine si jeu en cft
    bool SNIPER_ON; // gros booleen qui determine si jeu en cft
    bool TEAM_ON;
    int TOURNOIS_ON;
    long TOURNOIS_count; // pour le compteur de debut

    int CFT_eta;        // eta =0 init; =1 debut du compte; =2 apres 5 secondes pour changer les skins
    long CFT_count;     // pour le compteur de debut
    long CFT_old_count; // pour le compteur de debut
    int CFT_nb_terro;   // compte les victoires
    int CFT_nb_gign;    // compte les victoires
    //**

    CSound *lessons[max_son];
    // CSound * leseffets [20];

    /*typedef struct joueur {
        int id_modele;
        int id_weapon;
        int seq1;
        int seq2;
        float fps1;
        float fps2;
    } joueur;

    joueur jj1;
    joueur jj2;
    joueur jj3;*/
    // Aplayer *lejoueur;
    vector<Aplayer *> lejoueur;
    int NB_MAX;
    //////////////////////////////////////////
    struct DPHostEnumInfo
    {
        DWORD dwRef;
        DPN_APPLICATION_DESC *pAppDesc;
        IDirectPlay8Address *pHostAddr;
        IDirectPlay8Address *pDeviceAddr;
        TCHAR szSession[MAX_PATH];
        DWORD dwLastPollTime;
        BOOL bValid;
        DPHostEnumInfo *pNext;
    };

    virtual void init();
    virtual void resize();
    virtual void HandleEvent();
    void Run();
    void begin_orto();
    void end_orto();
    void download_task_new();
    char *Left(char *Texte, int Nb_Caract);
    void download_task();
    void download_task_old();
    void download_task_wininet();
    void handle_input(float delta);
    void handle_input_main();
    void reset_mouse();
    void overlay();
    void AfficheCroix();
    void AffichePanel(float delta);
    void display_screen(float delta);
    HRESULT SessionsDlgNoteEnumResponse(PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponse);
    VOID SessionsDlgEnumListCleanup();
    HRESULT SessionsDlgEnumHosts();
    VOID SessionsDlgExpireOldHostEnums();
    HRESULT SessionsDlgJoinGame(DPHostEnumInfo *pDPHostEnumSelected);
    VOID SessionsDlgInitListbox();
    HRESULT ConnectionsDlgOnOK();

    HRESULT SessionsDlgCreateGame();

  private:
    HRESULT envoi_who(void);
    HRESULT envoi_msg(char *msg);
    HRESULT envoi_msg_ops(char *msg);
    void model2str(int id_model, char *msg);
    bool Verify_Map(char *file);
    bool Verify_Bmp(char *file);
    // void Download_Map(char* host,char * file );
    void Verify_Env(char *file);
    bool Verify_Mp3(char *file);
    // void Download_Mp3(char* host,char * file );

    HRESULT SessionsDlgDisplayEnumList(long xd, long yd, int x, int y);
    playerfile_t *m_playerfile;
    CXmlMessaging m_xmlsession;
    Camera m_camera;
    int release_tir; // pour temporiser la mort
    Camera m_pivot2;
    Camera m_pivot;
    bool chasse;
    bool force_crouch;
    bool les_boites;
    bool m_overlay;
    int is_colliding;
    server_t lists;
    maplist_t MapList;
    maplist_t Mp3List;
    int curmapid;
    int curmp3id;

    long m_serverid;
    Font *m_font;
    Romu *m_romu;
    Cross *m_cross;
    Panel *m_panel;
    Pings *m_pings;
    // screen mid coords
    int m_xcenter;
    int m_ycenter;

    bool menu_mode;
    bool modechat;
    // movement and rotation vectors
    vec3_t dir;
    vec3_t rotation;
    vec3_t dir2;
    vec3_t rotation2;
    vector<bouton *> lesbtn;

    vector<console *> lesconsoles;
    vector<input_box *> lesinput_box;
    vector<option_bouton *> lesoptionsbouton;

    // dplay bat
    GUID ptcpGuid;

    IDirectPlay8LobbiedApplication *m_pLobbiedApp;
    CRITICAL_SECTION m_csHostEnum;
    CRITICAL_SECTION m_csTeam;

    GUID m_guidApp;
    HRESULT m_hrDialog;
    HWND m_hDlg;
    HINSTANCE m_hInst;
    HWND m_hWndParent;
    DWORD m_dwMaxPlayers;
    TCHAR m_strAppName[MAX_PATH];
    TCHAR m_strPreferredProvider[MAX_PATH];
    TCHAR m_strSessionName[MAX_PATH];
    TCHAR m_strLocalPlayerName[MAX_PATH];
    BOOL m_bSearchingForSessions;
    BOOL m_bMigrateHost;
    IDirectPlay8Address *m_pDeviceAddress;
    IDirectPlay8Address *m_pHostAddress;
    DPHostEnumInfo m_DPHostEnumHead;
    BOOL m_bEnumListChanged;
    DPNHANDLE m_hEnumAsyncOp;
    BOOL m_bHostPlayer;
    DWORD m_dwEnumHostExpireInterval;
    BOOL m_bConnecting;
    BOOL is_kikked;
    DPNHANDLE m_hConnectAsyncOp;
    HANDLE m_hConnectCompleteEvent;
    HANDLE m_hLobbyConnectionEvent;
    HRESULT m_hrConnectComplete;
    BOOL m_bHaveConnectionSettingsFromLobby;
    DPNHANDLE m_hLobbyClient;
    BOOL g_bWasLobbyLaunched;
    HINSTANCE g_hInst;
    HWND g_hDlg;
    TCHAR g_strAppName[256];
    HRESULT g_hrDialog;
    char les_armes_autorisees[max_arme + 1];
    // std::ofstream  o;
};

#endif // __engine_h__
