
// if !defined(AFX_XMLDOMFROMVCDLG_H__DA4EAB43_6DF3_11D4_ABD3_000102378429__INCLUDED_)
// #define AFX_XMLDOMFROMVCDLG_H__DA4EAB43_6DF3_11D4_ABD3_000102378429__INCLUDED_
#pragma warning(disable : 4996)
#pragma warning(disable : 4101)

#include <stdio.h>
#import <msxml4.dll>
#include "geom.h"

#include "playerfile.h"
// #import "msxml.dll"
// using namespace MSXML;
#include "PhysEnv.h"
/*#import "msxml4.dll"
using namespace MSXML2;
 */
#include <vector>
// #include "wininet.h"
#include "winhttp.h"
#include "MD5.h"
using namespace std;
typedef struct
{
    char file[100];
    char MD5[100];

} md5_file;

typedef struct
{
    char ip[20]; // LPTSTR
    long playerid;
    long serverid;
    char playername[100];
    char MD5[100];
    char comment[100];
    char version[50];
    char regles[500];
    char la_map[50];

} server;

typedef struct
{
    server list[150];
    int count;

} server_t;

typedef struct
{
    char Name[50];
    char mappeur[201];
    char host[50];
    char bsp_md5[33];
    char wad_md5[33];
    int id;
} map_t;

typedef struct
{
    map_t List[100];
    int count;

} maplist_t;

typedef struct
{
    char desc[100];
    char map[100];
    int cle_noeud;
    int cle_tournois;
    int timeout;

} tournois;

typedef struct
{
    char login[100];
    int cle_joueur;
} player;

typedef struct
{
    char nom[100];
    vector<player> list;
} ekip;

class CXmlMessaging
{

  public:
    CXmlMessaging();
    ~CXmlMessaging();

    void CXmlMessaging::affiche();

    // int download(char * host,char *url, char *filename_for_output);

    void CXmlMessaging::Crypt(char *result, const char *method, const char *src);
    HRESULT GetMap(maplist_t *MapList, playerfile_t *playerconfig);
    HRESULT DeleteServer(int serverid, playerfile_t *playerconfig);
    HRESULT QuitServer(playerfile_t *playerconfig, long idpartie, int killer, int killed);
    HRESULT scoreplus(playerfile_t *playerconfig, long idpartie, int point, bool is_killer);
    HRESULT set_mp3(playerfile_t *playerconfig, int idmp3);
    long DevenirServer(playerfile_t *playerconfig, char *version, short int bcft, DWORD MaxPlayers, int cle_tournois,
                       int round, char *md5, char *desc, bool privee, char *armes_autoriser);

    int NewPlayer(char *playername, char *playermail, char *pwd_player, char *error_msg);
    HRESULT QuitteServer();
    HRESULT ServerEnCours(server_t *ServerList, playerfile_t *playerconfig, int cle_tournois, int round);
    HRESULT libererListServer(server_t *ServerList);
    HRESULT InfoJoueur(playerfile_t *playerconfig, char *serverchat, char *MAC);
    HRESULT Get_Mp3(maplist_t *MapList, playerfile_t *playerconfig);
    HRESULT GetTournois(vector<tournois> *vtournois, playerfile_t *playerconfig);

    HRESULT GetTournoisDetail(vector<ekip> *lesekip, playerfile_t *playerconfig, int round, int cle_tournois);
    HRESULT SetTournois(playerfile_t *playerconfig, int cle_tournois, int round, int score_ekip1, int score_ekip2);
    HRESULT setobjet(playerfile_t *playerconfig, vec3_t pos, vec3_t dir, vec3_t haut, int type_objet, char *lamap);
    HRESULT getobjet(vector<CPhysEnv *> *lescar, playerfile_t *playerconfig, char *lamap);
    int Get_Id(char *login, char *pwd);

    int GetScroll_tchat(char *xml, char *out_msg);
    int GetPings_tchat(char *xml, char *out_msg);

    long JoinServer(playerfile_t *playerconfig, long serverid);
    int GetType_tchat(char *xml);
    void mode(bool isdebug);
    // int chartoint(char *);
    // long chartolong(char *);
    int len(char *);
    bool modedebug;
    int exp10(int);
    long exp10long(long);

    int GetMsg_tchat(char *xml, char *out_pseudo, char *out_msg);
    //	void dump_com_error(_com_error &e);
    bool XMLHttpRequest(char *url);
    void ErrorOut(DWORD dError, char *CallFunc);
    int Version_Soft;
    char GServerName[100];

  private:
    unsigned long Gmagic_key;

    int num_partie;

  protected:
    MSXML2::IXMLDOMDocumentPtr pDoc;
    // IXMLDOMDocumentPtr dom;
    LPDISPATCH *xmlDoc;
    CMD5 md5;
};

// #endif