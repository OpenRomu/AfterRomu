

//-----------------------------------------------------------------------------
// Name:   XMLCLASS
// Desc:
//-----------------------------------------------------------------------------

#include <windows.h>
#include "stdafx.h"
/*
//#include <atlbase.h>     // Text conversion macros -- W2A, W2T, etc
 #include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
//#include "AMMimeUtils.h"		// MFC support for Internet Explorer 4 Common Controls
*/
#include "resource.h"
// #include <afxinet.h>
#include <winhttp.h>
// #include <iostream>

#include <stdlib.h>

// #include <fstream>
#include <stdio.h>
#import <msxml4.dll>
#include "xmlmessaging.h"
// #import "c:\windows\system\MSXML.DLL" named_guids
/*
#import "msxml4.dll"
using namespace MSXML2;
*/
#include "playerfile.h"

CXmlMessaging::CXmlMessaging()
    : pDoc(__uuidof(MSXML2::DOMDocument40)), modedebug(false), Gmagic_key(1), Version_Soft(140), num_partie(0)
{
    pDoc = NULL;

    HRESULT hr = pDoc.CreateInstance("Msxml2.DOMDocument.4.0");

    // hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
    //        IID_IXMLDOMDocument2, (void**)&pDoc); // Check the return value, hr
    SUCCEEDED(hr) ? 0 : throw hr;

    pDoc->async = VARIANT_FALSE; // default - true,
}

void CXmlMessaging::mode(bool isdebug)
{
    modedebug = isdebug;
}

void CXmlMessaging::affiche()
{
    MSXML2::IXMLDOMElementPtr ElementPtr;
    ElementPtr = pDoc->documentElement;
    MessageBox(NULL, ElementPtr->xml, "Document from its root", MB_OK);
}
void CXmlMessaging::Crypt(char *result, const char *method, const char *src)
{
    strcpy(result, "");
    char resultat[1500];
    strcpy(resultat, "1");
    char buffer[1500];
    int len;
    len = sprintf(buffer, "method=%s&%s&LAVERSION=%i", method, src, Version_Soft);

    if (true)
    {
        unsigned long key = Gmagic_key;
        char les_octet[4];
        for (int i = 0; i <= 3; i++)
        {
            les_octet[i] = (key % 256);
            key = key >> 8;
        }
        len = strlen(buffer);
        for (int i = 0; i < len; i++)
        {
            char car = buffer[i];
            char xor = car ^ les_octet[i % 4];
            char lhex[3] = "  ";
            sprintf(lhex, "%02X", xor);
            strcat(resultat, lhex);
        }
    }
    else
    {
        strcpy(resultat, "<REDACTED BY OPENRS AUTHORS>");
    }
    sprintf(result, "%s/script/romustrike/xml_layer.php?crypt=%s", GServerName, resultat);
}

CXmlMessaging::~CXmlMessaging()
{
    //	pDoc->Release ();

    // pDoc->Release ();

    //	CoUninitialize();
}

int CXmlMessaging::NewPlayer(char *playername, char *playermail, char *pwd_player, char *error_msg)
{
    char lbuffer[1000];
    BSTR bstrNodeNameplayer = SysAllocString(L"IDPLAYER");
    BSTR bstrNodePwdplayer = SysAllocString(L"PWDPLAYER");
    BSTR bstrNodeError = SysAllocString(L"ERROR");

    BSTR bstrNodeName;
    BSTR bstrNodeText;
    char buffer[500];
    int num_player;

    num_player = 0;

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        long i = 0;

        long j = 0;

        char url[400];
        sprintf(url, "LENUM=0&LESOFT=2&LENOM=%s&LEVERSION=100&LEMAIL=%s&LEPASS=%s", playername, playermail, pwd_player);
        Crypt(buffer, "nouveaujoueur", url);

        // sprintf(  buffer,"%s/script/romustrike/nouveaujoueur.php3);

        if (XMLHttpRequest(buffer) == true)
        {
            Roots = pDoc->GetdocumentElement();

            // Roots->get_text();

            HRESULT hr = Roots->get_childNodes(&NodeList);

            for (i = 0; i < NodeList->length; i++)
            {
                HRESULT hr = NodeList->get_item(i, &Node);

                if (Node)
                {
                    Nodechild = Node->GetchildNodes();
                    hr = Node->get_nodeName(&bstrNodeName);
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameplayer)) == 0)
                    {
                        hr = Node->get_text(&bstrNodeText);

                        num_player = atoi((char *)_bstr_t(bstrNodeText));
                    }
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodePwdplayer)) == 0)
                    {
                        hr = Node->get_text(&bstrNodeText);

                        sprintf(pwd_player, "%s", (char *)_bstr_t(bstrNodeText));
                    }
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeError)) == 0)
                    {
                        hr = Node->get_text(&bstrNodeText);

                        sprintf(error_msg, "Erreur : %s", (char *)_bstr_t(bstrNodeText));
                    }
                }
            }
        }
        else
        {
            //	pError->GeterrorCode(), &buffer, pError->Getline(), static_cast<const wchar_t*>(pError->Getreason())
            //MSXML2::IXMLDOMParseErrorPtr pError = (LPCSTR)pDoc->parseError->Getreason();;
            /*	sprintf(lbuffer, "erreur XML %s\n",(char *)pDoc->parseError->Getreason());
                MessageBox (NULL,lbuffer ,TEXT("ROMUSTRIKE"),MB_OK);
                */
            return 0;
        }
    }
    catch (_com_error &e)
    {

        /*	WCHAR  * wszAddress;
            sprintf(lbuffer,"%s erreur xml %s",e.ErrorMessage ,e.Description );
            wcstombs(lbuffer,wszAddress,100 );

            MessageBox (NULL,lbuffer ,TEXT("ROMUSTRIKE"),MB_OK);
            */
        return 0;
    }

    // CoUninitialize();
    return num_player;
}

HRESULT CXmlMessaging::GetMap(maplist_t *MapList, playerfile_t *playerconfig)
{

    BSTR bstrAttributeNameIp = SysAllocString(L"IP");
    BSTR bstrAttributeNameServerId = SysAllocString(L"SERVERID");
    BSTR bstrAttributeNamePlayerId = SysAllocString(L"PLAYERID");
    BSTR bstrNodeName;
    BSTR bstrNodeNameComment = SysAllocString(L"COMMENT");
    BSTR bstrNodeMappeur = SysAllocString(L"MAPPEUR");
    BSTR bstrNodeWADMD5 = SysAllocString(L"WADMD5");
    BSTR bstrNodeBSPMD5 = SysAllocString(L"BSPMD5");

    BSTR bstrNodeHost = SysAllocString(L"HOST");
    BSTR bstrNodeNameName = SysAllocString(L"NAME");
    BSTR bstrNodeText;
    char buffer[1000];
    long i = 0;

    long j = 0;

    MapList->count = 0;

    if (modedebug)
    {

        WIN32_FIND_DATA File;
        HANDLE hSearch;
        BOOL re;

        hSearch = FindFirstFile("data/map/*.wad", &File);
        if (hSearch == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }
        re = TRUE;
        MapList->count = 0;

        do
        {
            /* Traitement */
            if (re)
            {
                //	MapList->List[MapList->count].Name = (char*)malloc(strlen(File.cFileName)+1);
                strncpy(MapList->List[MapList->count].Name, File.cFileName, strlen(File.cFileName) - 4);
                MapList->List[MapList->count].Name[(strlen(File.cFileName) - 4)] = '\0';
                MapList->List[MapList->count].host[0] = '\0';
                MapList->List[MapList->count].mappeur[0] = '\0';
                MapList->List[MapList->count].bsp_md5[0] = '\0';
                MapList->List[MapList->count].wad_md5[0] = '\0';

                MapList->count++;
                re = FindNextFile(hSearch, &File);
            }
        } while (re);
        FindClose(hSearch);
        /*return TRUE;


            MapList->List[0].Name = (char*)malloc(6);
            strcpy(MapList->List[0].Name,TEXT("italy"));
            MapList->count++;

            MapList->List[1].Name = (char*)malloc(10);
            strcpy(MapList->List[1].Name,TEXT("chateau"));
            MapList->count++;

            MapList->List[2].Name = (char*)malloc(7);
            strcpy(MapList->List[2].Name,TEXT("2rooms"));
            MapList->count++;

            MapList->List[3].Name = (char*)malloc(10);
            strcpy(MapList->List[3].Name,TEXT("aztec"));
            MapList->count++;

            MapList->List[4].Name = (char*)malloc(10);
            strcpy(MapList->List[4].Name,TEXT("train"));
            MapList->count++;
        */
    }
    else
    {

        try
        {

            MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
            MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
            MSXML2::IXMLDOMElementPtr Roots = NULL;
            MSXML2::IXMLDOMNodePtr Node = NULL;
            MSXML2::IXMLDOMNodePtr Nodec = NULL;
            MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

            MSXML2::IXMLDOMNamedNodeMapPtr MAP;
            MSXML2::IXMLDOMAttributePtr ATTR;

            char url[400];
            sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2", playerconfig->player_id, playerconfig->player_pwd);
            Crypt(buffer, "get_map", url);

            // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );

            if (XMLHttpRequest(buffer) == true)
            {
                Roots = pDoc->GetdocumentElement();

                HRESULT hr = Roots->get_childNodes(&NodeList);
                for (i = 0; i < NodeList->length; i++)
                {
                    strcpy(MapList->List[i].mappeur, "");
                    strcpy(MapList->List[i].host, "");

                    hr = NodeList->get_item(i, &Node);
                    if (Node)
                    {
                        Nodechild = Node->GetchildNodes();
                        for (j = 0; j < Nodechild->length; j++)
                        {
                            hr = Nodechild->get_item(j, &Nodec);
                            if (Nodec)
                            {
                                hr = Nodec->get_nodeName(&bstrNodeName);
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameName)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].Name, (char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeMappeur)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].mappeur, (char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeWADMD5)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].wad_md5, (char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeBSPMD5)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].bsp_md5, (char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeHost)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].host, (char *)_bstr_t(bstrNodeText));
                                }
                            }
                        }

                        MapList->count++;
                    }
                }
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
        }

        // CoUninitialize();
    }

    return S_OK;
}

HRESULT CXmlMessaging::Get_Mp3(maplist_t *MapList, playerfile_t *playerconfig)
{

    BSTR bstrAttributeNameIp = SysAllocString(L"IP");
    BSTR bstrAttributeNameServerId = SysAllocString(L"SERVERID");
    BSTR bstrAttributeNamePlayerId = SysAllocString(L"PLAYERID");
    BSTR bstrNodeName;
    BSTR bstrNodeNameComment = SysAllocString(L"COMMENT");
    BSTR bstrNodeMappeur = SysAllocString(L"COMMENTAIRE");
    BSTR bstrNodeID = SysAllocString(L"ID");

    BSTR bstrNodeHost = SysAllocString(L"HOST");
    BSTR bstrNodeNameName = SysAllocString(L"NAME");
    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;

    long j = 0;

    MapList->count = 0;

    if (modedebug)
    {

        WIN32_FIND_DATA File;
        HANDLE hSearch;
        BOOL re;

        hSearch = FindFirstFile("data/map/*.mp3", &File);
        if (hSearch == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }
        re = TRUE;
        MapList->count = 0;

        do
        {
            /* Traitement */
            if (re)
            {
                //	MapList->List[MapList->count].Name = (char*)malloc(strlen(File.cFileName)+1);
                strncpy(MapList->List[MapList->count].Name, File.cFileName, strlen(File.cFileName) - 4);
                MapList->List[MapList->count].Name[(strlen(File.cFileName) - 4)] = '\0';
                MapList->List[MapList->count].host[0] = '\0';
                MapList->List[MapList->count].mappeur[0] = '\0';

                MapList->count++;
                re = FindNextFile(hSearch, &File);
            }
        } while (re);
        FindClose(hSearch);
        /*return TRUE;


            MapList->List[0].Name = (char*)malloc(6);
            strcpy(MapList->List[0].Name,TEXT("italy"));
            MapList->count++;

            MapList->List[1].Name = (char*)malloc(10);
            strcpy(MapList->List[1].Name,TEXT("chateau"));
            MapList->count++;

            MapList->List[2].Name = (char*)malloc(7);
            strcpy(MapList->List[2].Name,TEXT("2rooms"));
            MapList->count++;

            MapList->List[3].Name = (char*)malloc(10);
            strcpy(MapList->List[3].Name,TEXT("aztec"));
            MapList->count++;

            MapList->List[4].Name = (char*)malloc(10);
            strcpy(MapList->List[4].Name,TEXT("train"));
            MapList->count++;
        */
    }
    else
    {

        try
        {

            MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
            MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
            MSXML2::IXMLDOMElementPtr Roots = NULL;
            MSXML2::IXMLDOMNodePtr Node = NULL;
            MSXML2::IXMLDOMNodePtr Nodec = NULL;
            MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

            MSXML2::IXMLDOMNamedNodeMapPtr MAP;
            MSXML2::IXMLDOMAttributePtr ATTR;

            char url[400];
            sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2", playerconfig->player_id, playerconfig->player_pwd);
            Crypt(buffer, "get_mp3", url);

            // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );

            if (XMLHttpRequest(buffer) == true)
            {
                Roots = pDoc->GetdocumentElement();

                HRESULT hr = Roots->get_childNodes(&NodeList);
                for (i = 0; i < NodeList->length; i++)
                {
                    strcpy(MapList->List[i].mappeur, "");
                    strcpy(MapList->List[i].host, "");

                    hr = NodeList->get_item(i, &Node);
                    if (Node)
                    {
                        Nodechild = Node->GetchildNodes();
                        for (j = 0; j < Nodechild->length; j++)
                        {
                            hr = Nodechild->get_item(j, &Nodec);
                            if (Nodec)
                            {
                                hr = Nodec->get_nodeName(&bstrNodeName);
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameName)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].Name, (char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeMappeur)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].mappeur, (char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeHost)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    strcpy(MapList->List[i].host, (char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeID)) == 0)
                                {
                                    hr = Nodec->get_text(&bstrNodeText);
                                    MapList->List[i].id = atoi((char *)_bstr_t(bstrNodeText));
                                }
                            }
                        }

                        MapList->count++;
                    }
                }
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
        }

        // CoUninitialize();
    }

    return S_OK;
}

HRESULT CXmlMessaging::DeleteServer(int serverid, playerfile_t *playerconfig)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {
        char buffer[500];
        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&CLE_SERVEUR=%i", playerconfig->player_id, playerconfig->player_pwd, serverid);
        Crypt(buffer, "delete_server", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
    }
}

HRESULT CXmlMessaging::QuitServer(playerfile_t *playerconfig, long idpartie, int killer, int killed)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {
        char buffer[500];
        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&LESCORE=%i&LAPARTIE=%i&KILLER=%i&KILLED=%i", playerconfig->player_id,
                playerconfig->player_pwd, playerconfig->score, idpartie, killer, killed);
        Crypt(buffer, "quitter_server", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
    }
}

HRESULT CXmlMessaging::SetTournois(playerfile_t *playerconfig, int cle_tournois, int round, int score_ekip1,
                                   int score_ekip2)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {
        char buffer[500];
        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&CLE_TOURNOIS=%i&ROUND=%i&SCORE1=%i&SCORE2=%i", playerconfig->player_id,
                playerconfig->player_pwd, cle_tournois, round, score_ekip1, score_ekip2);
        Crypt(buffer, "set_tournois", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
    }
}

HRESULT CXmlMessaging::scoreplus(playerfile_t *playerconfig, long idpartie, int point, bool is_killer)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {
        char buffer[500];
        char url[400];
        char str_kil[50];

        if (is_killer)
            sprintf(str_kil, "&KILLER=%i", point);
        else
            sprintf(str_kil, "&KILLED=%i", point);

        sprintf(url, "LENUM=%i&LEPASS=%s&LESCORE=%i&LAPARTIE=%i%s", playerconfig->player_id, playerconfig->player_pwd,
                playerconfig->score, idpartie, str_kil);
        Crypt(buffer, "score_plus", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        pDoc->put_async(VARIANT_TRUE);

        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
        pDoc->put_async(VARIANT_FALSE);
    }
}

HRESULT CXmlMessaging::set_mp3(playerfile_t *playerconfig, int idmp3)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {

        char buffer[500];
        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&IDMP3=%i", playerconfig->player_id, playerconfig->player_pwd, idmp3);
        Crypt(buffer, "set_mp3", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        pDoc->put_async(VARIANT_TRUE);

        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
        pDoc->put_async(VARIANT_FALSE);
    }
}

long CXmlMessaging::DevenirServer(playerfile_t *playerconfig, char *map, short int bcft, DWORD MaxPlayers,
                                  int cle_tournois, int round, char *md5, char *desc, bool privee,
                                  char *armes_autoriser)
{
    MSXML2::IXMLDOMDocument2Ptr pDocl;
    if (modedebug)
    {
        return 1;
    }
    else
    {

        try
        {

            MSXML2::IXMLDOMElementPtr Roots = NULL;
            BSTR bstrNodeText;
            char buffer[1000];
            char url[600];
            char yy[4];
            if (privee)
                sprintf(yy, "%s", "1");
            else
                sprintf(yy, "%s", "0");
            sprintf(url,
                    "LENUM=%i&LEPASS=%s&LESOFT=2&LECOMMENT=%s&MAX_PLAYERS=%i&CFT=%i&CLE_TOURNOIS=%i&ROUND=%i&MD5=%s&"
                    "DESC=%s&PRIVEE=%s&ARMES=%s",
                    playerconfig->player_id, playerconfig->player_pwd, map, MaxPlayers, bcft, cle_tournois, round, md5,
                    desc, yy, armes_autoriser);
            Crypt(buffer, "set_server", url);

            // sprintf(
            // buffer,"%s/script/romustrike/set_server.php3?LESOFT=2&LEJOUEUR=%i&LAVERSION=%s&LECOMMENT=AVAILABLE",GServerName,playerid,version);
            if ((XMLHttpRequest(buffer) == true))
            {
                Roots = pDoc->GetdocumentElement();
                HRESULT hr = Roots->get_text(&bstrNodeText);
                return atol((char *)_bstr_t(bstrNodeText));
            }
            else
            {
                return 0;
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
            return 0;
        }
    }
}

HRESULT CXmlMessaging::ServerEnCours(server_t *ServerList, playerfile_t *playerconfig, int cle_tournois, int round)
{

    BSTR bstrAttributeNameIp = SysAllocString(L"IP");
    BSTR bstrAttributeNameServerId = SysAllocString(L"SERVERID");
    BSTR bstrAttributeNamePlayerId = SysAllocString(L"PLAYERID");
    BSTR bstrNodeName;
    BSTR bstrNodeNameComment = SysAllocString(L"COMMENT");

    BSTR bstrNodeNamedesc = SysAllocString(L"DESC");
    BSTR bstrNodeNamemap = SysAllocString(L"MAP");

    BSTR bstrNodeNameMD5 = SysAllocString(L"MD5");
    BSTR bstrNodeNameVersion = SysAllocString(L"VERSION");
    BSTR bstrNodeNameName = SysAllocString(L"NOM");
    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;

    long j = 0;
    ServerList->count = 0;

    long ir;
    // CoInitialize(NULL);
    if (modedebug)
    {
        // ServerList->list[i].ip=(char*)malloc(12);

        strcpy(ServerList->list[i].ip, TEXT("10.0.0.12"));
        ServerList->list[i].serverid = 1;
        ServerList->list[i].playerid = 1;
        // ServerList->list[i].comment=(char*)malloc(12);
        strcpy(ServerList->list[i].comment, TEXT("10.0.0.12"));
        // ServerList->list[i].version=(char*)malloc(2);
        strcpy(ServerList->list[i].version, TEXT("1"));
        // ServerList->list[i].playername=(char*)malloc(12);
        strcpy(ServerList->list[i].playername, TEXT("LOCALPLAYER"));
        strcpy(ServerList->list[i].MD5, "");
        strcpy(ServerList->list[i].regles, "NO RULES !");

        ServerList->count++;
        i++;

        // ServerList->list[i].ip=(char*)malloc(12);

        strcpy(ServerList->list[i].ip, TEXT("10.0.0.11"));
        ServerList->list[i].serverid = 1;
        ServerList->list[i].playerid = 1;
        // ServerList->list[i].comment=(char*)malloc(12);
        strcpy(ServerList->list[i].comment, TEXT("10.0.0.11"));
        // ServerList->list[i].version=(char*)malloc(2);
        strcpy(ServerList->list[i].version, TEXT("1"));
        // ServerList->list[i].playername=(char*)malloc(12);
        strcpy(ServerList->list[i].playername, TEXT("LOCALPLAYER"));
        strcpy(ServerList->list[i].MD5, "");
        strcpy(ServerList->list[i].regles, "NO RULES !");

        ServerList->count++;
        i++;

        // ServerList->list[i].ip=(char*)malloc(12);

        strcpy(ServerList->list[i].ip, TEXT("192.168.0.2"));
        ServerList->list[i].serverid = 1;
        ServerList->list[i].playerid = 1;
        // ServerList->list[i].comment=(char*)malloc(12);
        strcpy(ServerList->list[i].comment, TEXT("192.168.0.2"));
        // ServerList->list[i].version=(char*)malloc(2);
        strcpy(ServerList->list[i].version, TEXT("1"));
        // ServerList->list[i].playername=(char*)malloc(12);
        strcpy(ServerList->list[i].playername, TEXT("LOCALPLAYER"));
        strcpy(ServerList->list[i].MD5, "");
        strcpy(ServerList->list[i].regles, "NO RULES !");
        ServerList->count++;
    }
    else
    {

        try
        {
            //	IXMLDOMDocumentPtr pDoc = NULL;
            MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
            MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
            MSXML2::IXMLDOMElementPtr Roots = NULL;
            MSXML2::IXMLDOMNodePtr Node = NULL;
            MSXML2::IXMLDOMNodePtr Nodec = NULL;
            MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

            MSXML2::IXMLDOMNamedNodeMapPtr MAP;
            MSXML2::IXMLDOMAttributePtr ATTR;
            VARIANT vValue;

            VariantInit(&vValue);

            V_VT(&vValue) = VT_BSTR;
            vValue.bstrVal = SysAllocString(L"do it");

            char url[400];
            sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2&CLE_TOURNOIS=%i&ROUND=%i", playerconfig->player_id,
                    playerconfig->player_pwd, cle_tournois, round);
            Crypt(buffer, "get_server", url);
            //		sprintf(  buffer,"%s/script/romustrike/get_server.php3?LESOFT=2",GServerName );

            //	ServerList->count=0;

            if ((XMLHttpRequest(buffer) == true))
            {
                Roots = pDoc->GetdocumentElement();
                if (Roots)
                {

                    if (Roots->hasChildNodes())
                    {

                        HRESULT hr = Roots->get_childNodes(&NodeList);
                        for (i = 0; ((i < NodeList->length) && (i < 100)); i++)
                        {
                            hr = NodeList->get_item(i, &Node);
                            if (Node)
                            {
                                // traitment des attribut
                                MAP = Node->Getattributes();
                                hr = MAP->get_length(&ir);
                                //------------------
                                ATTR = MAP->getNamedItem(bstrAttributeNameIp);
                                hr = ATTR->get_value(&vValue);
                                // ServerList->list[i].ip = (char*)malloc(_bstr_t(vValue.bstrVal).length());

                                strcpy(ServerList->list[i].ip, (char *)_bstr_t(vValue.bstrVal));
                                // strcpy (ServerList->list[i].ip,  "");

                                //------------------
                                ATTR = MAP->getNamedItem(bstrAttributeNameServerId);
                                hr = ATTR->get_value(&vValue);
                                ServerList->list[i].serverid = atol((char *)_bstr_t(vValue.bstrVal));
                                //------------------
                                ATTR = MAP->getNamedItem(bstrAttributeNamePlayerId);
                                hr = ATTR->get_value(&vValue);
                                ServerList->list[i].playerid = atol((char *)_bstr_t(vValue.bstrVal));
                                // traitment des sous noeud nom ,version,et comment
                                Nodechild = Node->GetchildNodes();
                                for (j = 0; j < Nodechild->length; j++)
                                {
                                    hr = Nodechild->get_item(j, &Nodec);
                                    if (Nodec)
                                    {
                                        hr = Nodec->get_nodeName(&bstrNodeName);
                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameName)) ==
                                            0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);

                                            //							ServerList->list[i].playername =
                                            //(char*)malloc(_bstr_t(bstrNodeText).length());
                                            strcpy(ServerList->list[i].playername, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName),
                                                   (char *)_bstr_t(bstrNodeNameVersion)) == 0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);

                                            // ServerList->list[i].version =
                                            // (char*)malloc(_bstr_t(bstrNodeText).length());
                                            strcpy(ServerList->list[i].version, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName),
                                                   (char *)_bstr_t(bstrNodeNameComment)) == 0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);

                                            //	ServerList->list[i].comment =
                                            //(char*)malloc(_bstr_t(bstrNodeText).length());
                                            strcpy(ServerList->list[i].comment, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameMD5)) ==
                                            0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);
                                            strcpy(ServerList->list[i].MD5, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamedesc)) ==
                                            0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);
                                            strcpy(ServerList->list[i].regles, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemap)) ==
                                            0)
                                        {
                                            hr = Nodec->get_text(&bstrNodeText);
                                            strcpy(ServerList->list[i].la_map, (char *)_bstr_t(bstrNodeText));
                                        }
                                    }
                                }

                                ServerList->count++;
                            }
                        }
                    }
                }
                else
                {
                    return S_FALSE;
                }
            }
            else
            {
                return S_FALSE;
            }

            // Note   If these functions are passed NULL pointers then there will be an access violation and the program
            // will crash. It is your responsibility to protect these functions against NULL pointers.

            if (&vValue)
            {
                VariantClear(&vValue);
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
        }
    }
    // CoUninitialize();

    return S_OK;
}
int CXmlMessaging::Get_Id(char *login, char *pwd)
{

    BSTR bstrNodeName;
    BSTR bstrNodecle = SysAllocString(L"ID_PLAYER");
    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;
    int cle;
    long j = 0;

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        char url[400];
        sprintf(url, "LELOGIN=%s&LEPASS=%s&LESOFT=2", login, pwd);
        Crypt(buffer, "get_id", url);

        // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );
        // tournois letournois;

        if (XMLHttpRequest(buffer) == true)
        {
            Roots = pDoc->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            if (NodeList->length > 0)
            {
                for (i = 0; i < NodeList->length; i++)
                {

                    hr = NodeList->get_item(i, &Node);
                    if (Node)
                    {

                        hr = Node->get_nodeName(&bstrNodeName);

                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodecle)) == 0)
                        {
                            hr = Node->get_text(&bstrNodeText);
                            //	strcpy (letournois.desc ,  (char*)_bstr_t(bstrNodeText));
                            cle = atoi((char *)_bstr_t(bstrNodeText));
                        }

                        // vtournois->push_back(letournois);

                        //	MapList->count++;
                    }
                }
            }
            else
            {
                cle = 0;
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }
    // CoUninitialize();

    return cle;
}

HRESULT CXmlMessaging::InfoJoueur(playerfile_t *playerconfig, char *serverchat, char *MAC)
{

    BSTR bstrAttributeNameIp = SysAllocString(L"IP");
    BSTR bstrAttributeNameServerId = SysAllocString(L"SERVERID");
    BSTR bstrAttributeNamePlayerId = SysAllocString(L"PLAYERID");
    BSTR bstrAttributeNameMp3Id = SysAllocString(L"ID");
    BSTR bstrAttributeNameKey = SysAllocString(L"KEY");
    BSTR bstrNodeName;
    BSTR bstrNodeNamea;
    BSTR bstrNodeNameMessage = SysAllocString(L"MESSAGE");
    BSTR bstrNodeNameNames = SysAllocString(L"NAME");
    BSTR bstrNodeError = SysAllocString(L"ERROR");
    BSTR bstrNodeNameModel = SysAllocString(L"MODEL");
    BSTR bstrNodeNameMp3 = SysAllocString(L"MP3");
    BSTR bstrNodeNameScore = SysAllocString(L"SCORE");
    BSTR bstrNodeMd5 = SysAllocString(L"MD5");

    BSTR bstrNodeNamemsg1 = SysAllocString(L"MSG1");
    BSTR bstrNodeNameis_op = SysAllocString(L"IS_OP");
    BSTR bstrNodeNameis_autorise = SysAllocString(L"VALIDE");

    BSTR bstrNodeNamemsg2 = SysAllocString(L"MSG2");
    BSTR bstrNodeNamescroll = SysAllocString(L"SCROLL");
    BSTR bstrNodeNamestats = SysAllocString(L"STATS");
    BSTR bstrNodecontrole = SysAllocString(L"CONTROLE");
    BSTR bstrNodeNamemsg3 = SysAllocString(L"MSG3");
    BSTR bstrNodeNamemsg4 = SysAllocString(L"MSG4");
    BSTR bstrNodepanel = SysAllocString(L"PANEL");
    BSTR bstrNodeNamemsgsock = SysAllocString(L"ROMUCHAT");
    BSTR bstrAttributeNameFile = SysAllocString(L"ID");

    VARIANT vValue;
    VariantInit(&vValue);
    BSTR bstrNodeText;
    BSTR bstrNodeTexta;
    char buffer[500];
    //----------------------------------------------------------liste fichier
    /*
    HANDLE hSearch;
    WIN32_FIND_DATA File;
    bool re;

    char rep[4][100];
    sprintf(rep[0],"data/model/");
    sprintf(rep[1],"data/weapons/");
    sprintf(rep[2],"data/env/");
    char fic[4][100];
    sprintf(fic[0],"mdl");
    sprintf(fic[1],"mdl");
    sprintf(fic[2],"tga");

    vector<md5_file> locaux;
    vector<md5_file> distant;

     char err_md5[900000];

     //err_md5 << "start" << endl;

    char f[200];

    for (int o=1;o<2;o++)
    {
        char t[150];
        sprintf(t,"%s/*.%s",rep[o],fic[o]);
        hSearch=FindFirstFile(t, &File);

        if (hSearch == INVALID_HANDLE_VALUE){
            return FALSE;
        }
        re=TRUE;


        do {
            if (re)
            {
                md5_file tmps;

        //	MapList->List[MapList->count].Name = (char*)malloc(strlen(File.cFileName)+1);
            strcpy (tmps.file,File.cFileName);
            sprintf(t,"%s%s",rep[o],tmps.file);
        strcpy(tmps.MD5,"");
    char tmpsmd5[100];


            md5.GetMD5_new(t,tmpsmd5);

                strcpy(tmps.MD5,tmpsmd5);


            locaux.push_back(tmps);
            sprintf(f,"insert into anti_cheat (file,md5) values ('%s','%s');\r\n",t,tmps.MD5);
    //sprintf(f,"update map set bsp_md5='%s' where map='%s';\r\n",tmps.MD5,tmps.file);


    strcat(err_md5,f);
         re = FindNextFile(hSearch, &File);
            }
        } while (re);
        FindClose(hSearch);
        FILE *out=fopen("sql.txt","w");
        fwrite(err_md5,sizeof(err_md5),1,out);
        fclose(out);

    }
     */
    //----------------------------------------------------------liste fichier

    strcpy(playerconfig->scroll,
           "*****************************Romustrike***Serveur Hors Service ou en Maintenance ou inactif a jamais.. "
           "essayez plus tard   *********************************************************");
    bool tricheur = false;

    if (modedebug)
    {
        strcpy(playerconfig->player_name, TEXT("BETA-TESTEUR"));
        strcpy(playerconfig->msg1, TEXT("DEBUG"));
        strcpy(playerconfig->msg2, TEXT("MODE"));
        strcpy(playerconfig->msg3, TEXT("ON"));
        strcpy(playerconfig->msg4, TEXT("-*--*-"));
        strcpy(playerconfig->panel_joueur, TEXT("-*--*-"));

        strcpy(playerconfig->panel_stats, TEXT("-*--*-"));
        playerconfig->player_model = 0;
        strcpy(playerconfig->player_mp3, TEXT("vito_1"));
        playerconfig->player_mp3id = 1;
        playerconfig->is_op = true;
        playerconfig->is_autorise = true;
    }
    else
    {

        // CoInitialize(NULL);

        try
        {
            //	IXMLDOMDocumentPtr pDoc = NULL;
            MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
            MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
            MSXML2::IXMLDOMElementPtr Roots = NULL;
            MSXML2::IXMLDOMNodePtr Node = NULL;
            MSXML2::IXMLDOMNodePtr Nodec = NULL;
            MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

            MSXML2::IXMLDOMNamedNodeMapPtr MAP;
            MSXML2::IXMLDOMAttributePtr ATTR;

            long i = 0;

            long j = 0;
            char url[400];
            sprintf(url, "LENUM=%i&LEPASS=%s&LAMAC=%s", playerconfig->player_id, playerconfig->player_pwd, MAC);
            Crypt(buffer, "info_joueur", url);
            pDoc->put_async(VARIANT_FALSE);

            if (XMLHttpRequest(buffer) == true)
            {
                Roots = pDoc->GetdocumentElement();
                if (Roots)
                {

                    HRESULT s_error = S_OK;

                    HRESULT hr = Roots->get_childNodes(&NodeList);
                    for (i = 0; i < NodeList->length; i++)
                    {
                        hr = NodeList->get_item(i, &Node);
                        if (Node)
                        {

                            hr = Node->get_nodeName(&bstrNodeName);

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameNames)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->player_name, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeError)) == 0)
                            {
                                char tt[10];
                                hr = Node->get_text(&bstrNodeText);

                                strcpy(tt, (char *)_bstr_t(bstrNodeText));
                                if (strcmp(tt, "0") != 0)
                                    s_error = S_FALSE;
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameMp3)) == 0)
                            {
                                MAP = Node->Getattributes();
                                //	hr=MAP->get_length (&ir);
                                //------------------
                                ATTR = MAP->getNamedItem(bstrAttributeNameMp3Id);
                                hr = ATTR->get_value(&vValue);
                                //
                                playerconfig->player_mp3id = atoi((char *)_bstr_t(&vValue));

                                hr = Node->get_text(&bstrNodeText);
                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->player_mp3, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameModel)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);
                                playerconfig->player_model = atoi((char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameis_op)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);
                                int c = atoi((char *)_bstr_t(bstrNodeText));

                                if (c == 1)
                                    playerconfig->is_op = true;
                                else
                                    playerconfig->is_op = false;
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameis_autorise)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);
                                int c = atoi((char *)_bstr_t(bstrNodeText));

                                if (c == 1)
                                    playerconfig->is_autorise = true;
                                else
                                    playerconfig->is_autorise = false;
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg1)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->msg1, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg2)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->msg2, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg3)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->msg3, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg4)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                // playerconfig->player_name   = (char*)malloc(_bstr_t(bstrNodeText).length());
                                strcpy(playerconfig->msg4, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameScore)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);
                                playerconfig->score = atoi((char *)_bstr_t(bstrNodeText));

                                // msg = (char*)malloc(_bstr_t(bstrNodeText).length());
                                // strcpy (msg,  (char*)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamescroll)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                char toto[52];
                                sprintf(toto, "%s", TEXT("*************************************************"));
                                strcpy(playerconfig->scroll, "");

                                strcat(playerconfig->scroll, toto);
                                strcat(playerconfig->scroll, (char *)_bstr_t(bstrNodeText));
                                strcat(playerconfig->scroll, toto);

                                // msg = (char*)malloc(_bstr_t(bstrNodeText).length());
                                // strcpy (msg,  (char*)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamestats)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                sprintf(playerconfig->panel_stats, "%s", (char *)_bstr_t(bstrNodeText));
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodepanel)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                strcpy(playerconfig->panel_joueur, (char *)_bstr_t(bstrNodeText));
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsgsock)) == 0)
                            {
                                hr = Node->get_text(&bstrNodeText);

                                strcpy(serverchat, (char *)_bstr_t(bstrNodeText));
                            }

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodecontrole)) == 0)
                            {
                                MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;
                                Nodechild = Node->GetchildNodes();
                                for (int aj = 0; aj < Nodechild->length; aj++)
                                {
                                    hr = Nodechild->get_item(aj, &Nodec);
                                    if (Nodec)
                                    {
                                        hr = Nodec->get_nodeName(&bstrNodeNamea);
                                        if (strcmp((char *)_bstr_t(bstrNodeNamea), (char *)_bstr_t(bstrNodeMd5)) == 0)
                                        {
                                            md5_file tmps;

                                            MAP = Nodec->Getattributes();
                                            ATTR = MAP->getNamedItem(bstrAttributeNameFile);
                                            hr = ATTR->get_value(&vValue);

                                            // strcpy (tmps.file,  (char*)_bstr_t(vValue.bstrVal));
                                            sprintf(tmps.file, "%s", (char *)_bstr_t(vValue.bstrVal));

                                            hr = Nodec->get_text(&bstrNodeTexta);

                                            strcpy(tmps.MD5, (char *)_bstr_t(bstrNodeTexta));
                                            char tmpsmd5[32 + 1];

                                            md5.GetMD5_new(tmps.file, tmpsmd5);
                                            if (strcmp(tmpsmd5, tmps.MD5))
                                            {
                                                //	err_md5 << tmps.file << endl;
                                                tricheur = true;
                                                s_error = S_FALSE;
                                            }

                                            //	distant.push_back(tmps);
                                        }
                                    }
                                }
                            }

                        } // if
                    } // for
                    return s_error;
                }
                else
                {
                    return S_FALSE;
                }
            }
            else
            {
                return S_FALSE;
            }

            if (&vValue)
            {
                VariantClear(&vValue);
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
            return S_FALSE;
        }
    }

    // CoUninitialize();

    return S_OK;
}

long CXmlMessaging::JoinServer(playerfile_t *playerconfig, long serverid)
{
    if (modedebug)
    {
        return 1;
    }
    else
    {

        try
        {
            MSXML2::IXMLDOMElementPtr Roots = NULL;
            BSTR bstrNodeText;
            char buffer[500];
            char url[400];
            sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2&SERVERID=%i", playerconfig->player_id, playerconfig->player_pwd,
                    serverid);
            Crypt(buffer, "joinserver", url);

            // sprintf(
            // buffer,"%s/script/romustrike/set_server.php3?LESOFT=2&LEJOUEUR=%i&LAVERSION=%s&LECOMMENT=AVAILABLE",GServerName,playerid,version);
            if ((XMLHttpRequest(buffer) == true))
            {
                Roots = pDoc->GetdocumentElement();
                HRESULT hr = Roots->get_text(&bstrNodeText);
                return atol((char *)_bstr_t(bstrNodeText));
            }
            else
            {
                return 0;
            }
        }
        catch (_com_error &e)
        {
            MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
            return 0;
        }
    }
}

int CXmlMessaging::len(char *nome)
{
    int i = 0;
    while (nome[i] != 0)
        i++;
    return i;
}

int CXmlMessaging::exp10(int num)
{
    int ris = 1;
    for (int i = 0; i < num - 1; i++)
        ris *= 10;
    return ris;
}

long CXmlMessaging::exp10long(long num)
{
    long ris = 1;
    for (int i = 0; i < num - 1; i++)
        ris *= 10;
    return ris;
}

/*
int CXmlMessaging::chartoint(char *dato)
{
int ris=0,cont=0;
int i=len(dato);
int ci=i;
int molt[10];
for(cont;cont<i;cont++)
{
molt[cont]=exp10(ci);
ci--;
}
cont=0;
for(cont;cont<i;cont++)
{
ris+=(dato[cont]-'0')*molt[cont];
}
return ris;
}

long CXmlMessaging::chartolong(char *dato)
{
long ris=0,cont=0;
int i=len(dato);
int ci=i;
long molt[10];
for(cont;cont<i;cont++)
{
molt[cont]=exp10long(ci);
ci--;
}
cont=0;
for(cont;cont<i;cont++)
{
ris+=(dato[cont]-'0')*molt[cont];
}
return ris;
}

*/
/*
void CXmlMessaging::dump_com_error(_com_error &e)
{
    //printf("Error\n");
//	printf("\a\tCode = %08lx\n", e.Error());
//	printf("\a\tCode meaning = %s", e.ErrorMessage());
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
//	printf("\a\tSource = %s\n", (LPCSTR) bstrSource);
    //printf("\a\tDescription = %s\n", (LPCSTR) bstrDescription);

char buf[200];
sprintf(buf,"Code = %08lx , Code meaning = %s tSource = %s tDescription = %s\n",  e.Error(),e.ErrorMessage(),(LPCSTR)
bstrSource,(LPCSTR) bstrDescription);
//if (e.Error()!=E_Fail)
 //MessageBox(NULL, buf, "xml Exception occurred", MB_OK | MB_ICONERROR);
static ofstream err_acces("log/access.log");

err_acces << "ERREUR XML" << endl;
err_acces << buf << endl;


//
}
*/
bool CXmlMessaging::XMLHttpRequest(char *url)
{
    MSXML2::IXMLHTTPRequestPtr pIXMLHTTPRequest = NULL;

    BSTR bstrString = NULL;
    HRESULT hr;

    try
    {
        // hr=pIXMLHTTPRequest.CreateInstance("Msxml2.XMLHTTP.4.0");  //CreateInstance("Msxml2.XMLHTTP") marche pas ac
        // esey

        hr = pIXMLHTTPRequest.CreateInstance(
            "Msxml2.XMLHTTP.4.0"); // CreateInstance("Msxml2.XMLHTTP") marche pas ac esey
        SUCCEEDED(hr) ? 0 : throw hr;

        hr = pIXMLHTTPRequest->open("GET", _bstr_t(url), false);
        SUCCEEDED(hr) ? 0 : throw hr;

        hr = pIXMLHTTPRequest->send();
        SUCCEEDED(hr) ? 0 : throw hr;

        bstrString = pIXMLHTTPRequest->responseText;

        // MessageBox(NULL, _bstr_t(bstrString), _T("Results"), MB_OK);
        hr = pDoc->loadXML(_bstr_t(bstrString));

        // affiche();

        if (bstrString)
        {
            ::SysFreeString(bstrString);
            bstrString = NULL;
        }

        return true;
    }
    catch (_com_error &e)
    {
        // MessageBox(NULL, _T("Exception occurred"), _T("Error"), MB_OK);
        // dump_com_error(e);
        //("XML Http Request pointer creation failed\n");
        if (bstrString)
            ::SysFreeString(bstrString);
        return false;
    }
}

/*********************************************************/
/* This function does error checking, and prints out the */
/* error causing the problem.
 */
/*********************************************************/

void CXmlMessaging::ErrorOut(DWORD dError, char *CallFunc)
{
    //    cerr << CallFunc << " error " << dError << " ";

    switch (dError)
    {
    case 12001:
        //      cerr << " (Out of handles)" << endl;
        break;
    case 12002:
        //      cerr << " (Timeout)" << endl;
        break;
    case 12004:
        //      cerr << " (Internal Error)" << endl;
        break;
    case 12005:
        //   cerr << " (Invalid URL)" << endl;
        break;
    case 12006:
        // cerr << " (Unrecognized Scheme : Please check the URL name in 'TVFilmAlerter.ini'.)" << endl;
        break;
    case 12007:
        //     cerr << " (Service Name Not Resolved)" << endl;
        break;
    case 12008:
        //     cerr << " (Protocol Not Found)" << endl;
        break;
    case 12013:
        //    cerr << " (Incorrect User Name)" << endl;
        break;
    case 12014:
        //    cerr << " (Incorrect Password)" << endl;
        break;
    case 12015:
        //    cerr << " (Login Failure)" << endl;
        break;
    case 12016:
        //   cerr << " (Invalid Operation)" << endl;
        break;
    case 12017:
        //   cerr << " (Operation Canceled)" << endl;
        break;
    case 12020:
        //     cerr << " (Not Proxy Request)" << endl;
        break;
    case 12023:
        //    cerr << " (No Direct Access)" << endl;
        break;
    case 12026:
        //    cerr << " (Request Pending)" << endl;
        break;
    case 12027:
        //    cerr << " (Incorrect Format)" << endl;
        break;
    case 12028:
        //   cerr << " (Item not found)" << endl;
        break;
    case 12029:
        //  cerr << " (Cannot connect)" << endl;
        break;
    case 12030:
        //   cerr << " (Connection Aborted)" << endl;
        break;
    case 12031:
        //   cerr << " (Connection Reset)" << endl;
        break;
    case 12033:
        //   cerr << " (Invalid Proxy Request)" << endl;
        break;
    case 12034:
        //   cerr << " (Need UI)" << endl;
        break;
    case 12035:
        //   cerr << " (Sec Cert Date Invalid)" << endl;
        break;
    case 12038:
        // cerr << " (Sec Cert CN Invalid)" << endl;
        break;
    case 12044:
        // cerr << " (Client Auth Cert Needed)" << endl;
        break;
    case 12045:
        // cerr << " (Invalid CA Cert)" << endl;
        break;
    case 12046:
        //   cerr << " (Client Auth Not Setup)" << endl;
        break;
    case 12150:
        //    cerr << " (HTTP Header Not Found)" << endl;
        break;
    case 12152:
        //   cerr << " (Invalid HTTP Server Response)" << endl;
        break;
    case 12153:
        //  cerr << " (Invalid HTTP Header)" << endl;
        break;
    case 120154:
        //   cerr << " (Invalid Query Request)" << endl;
        break;
    case 120156:
        //   cerr << " (Redirect Failed)" << endl;
        break;
    case 120159:
        //   cerr << " (TCP/IP not installed)" << endl;
        break;
    default:
        //   cerr << endl;
        break;
    }
}

int CXmlMessaging::GetType_tchat(char *xml)
{
    MSXML2::IXMLDOMDocument2Ptr pDocChat;
    HRESULT hr = pDocChat.CreateInstance(__uuidof(MSXML2::DOMDocument40));
    // HRESULT hr=pDocChat.CreateInstance("Msxml2.DOMDocument.4.0");
    //	(__uuidof(MSXML2::DOMDocument40));

    // hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
    //        IID_IXMLDOMDocument2, (void**)&pDoc); // Check the return value, hr
    SUCCEEDED(hr) ? 0 : throw hr;

    pDocChat->async = VARIANT_FALSE; // default - true,

    // BSTR bstrAttributeNameIp = SysAllocString(L"WHO");
    BSTR bstrNodeName;
    BSTR bstrNodeText;

    try
    {

        VARIANT vValue;

        VariantInit(&vValue);

        V_VT(&vValue) = VT_BSTR;
        vValue.bstrVal = SysAllocString(L"do it");

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        BSTR bstrAttributeNametype = SysAllocString(L"TYPE");
        BSTR bstrAttributeNamevalue = SysAllocString(L"VALUE");

        int i = 0;
        int j = 0;

        if (pDocChat->loadXML((xml)))
        {
            Roots = pDocChat->GetdocumentElement();
            char buff[100];
            hr = Roots->get_nodeName(&bstrNodeName);
            strcpy(buff, (char *)_bstr_t(bstrNodeName));
            if (!(strcmp(buff, "who")))
            {
                hr = GAME_TCHAT_WHO;
            }
            else
            {
                if (!(strcmp(buff, "racine")))
                {
                    hr = GAME_TCHAT_MSG;
                }
                else
                {
                    if (!(strcmp(buff, "NIGHT")))
                    {
                        hr = Roots->get_text(&bstrNodeText);
                        char out[10];
                        strcpy(out, (char *)_bstr_t(bstrNodeText));
                        if (!(strcmp(out, "0")))
                        {
                            hr = GAME_TEAM_START;
                        }
                        else
                        {
                            hr = GAME_TEAM_STOP;
                        }
                    }
                    else
                    {
                        if (!(strcmp(buff, "SCROLL")))
                        {
                            hr = Roots->get_text(&bstrNodeText);

                            hr = GAME_TCHAT_SCROLL;
                        }
                        else
                        {
                            if (!(strcmp(buff, "m")))
                            {
                                hr = Roots->get_text(&bstrNodeText);

                                hr = GAME_TCHAT_PINGS;
                            }
                            else
                            {
                                if (!(strcmp(buff, "KILL")))
                                {
                                    hr = Roots->get_text(&bstrNodeText);

                                    hr = GAME_TCHAT_KILL;
                                }
                                else
                                { // mise sur ecoute des conversations
                                    if (!(strcmp(buff, "ECOUTEON")))
                                    {
                                        // hr=Roots->get_text (&bstrNodeText);

                                        hr = GAME_MODE_ECOUTE_ON;
                                    }
                                    else
                                    {
                                        if (!(strcmp(buff, "ECOUTEOFF")))
                                        {
                                            // hr=Roots->get_text (&bstrNodeText);

                                            hr = GAME_MODE_ECOUTE_OFF;
                                        }
                                        else
                                        {
                                            if (!(strcmp(buff, "STORM")))
                                            {
                                                hr = Roots->get_text(&bstrNodeText);
                                                char out[10];
                                                strcpy(out, (char *)_bstr_t(bstrNodeText));
                                                if (!(strcmp(out, "0")))
                                                {
                                                    hr = GAME_STORM_START;
                                                }
                                                else
                                                {
                                                    hr = GAME_STORM_STOP;
                                                }
                                            }
                                            else
                                            {
                                                hr = GAME_TCHAT_ERR;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return hr;
}

int CXmlMessaging::GetMsg_tchat(char *xml, char *out_pseudo, char *out_msg)
{
    MSXML2::IXMLDOMDocument2Ptr pDocChat;
    HRESULT hr = pDocChat.CreateInstance("Msxml2.DOMDocument.4.0");

    // hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
    //        IID_IXMLDOMDocument2, (void**)&pDoc); // Check the return value, hr
    SUCCEEDED(hr) ? 0 : throw hr;

    pDocChat->async = VARIANT_FALSE; // default - true,

    BSTR bstrNodeNamemsg = SysAllocString(L"msg");
    BSTR bstrNodeNamepseudo = SysAllocString(L"pseudo");

    // BSTR bstrAttributeNameIp = SysAllocString(L"WHO");
    BSTR bstrNodeName;
    BSTR bstrNodeText;

    //		char buffer[500];

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        int i = 0;
        int j = 0;

        if (pDocChat->loadXML(_bstr_t(xml)))
        {
            Roots = pDocChat->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            char buff[100];
            hr = Roots->get_nodeName(&bstrNodeName);
            strcpy(buff, (char *)_bstr_t(bstrNodeName));
            if (!(strcmp(buff, "racine")))
            {

                for (i = 0; i < NodeList->length; i++)
                {
                    // strcpy(MapList->List[i].mappeur,"");
                    // strcpy(MapList->List[i].host,"");

                    hr = NodeList->get_item(i, &Node);

                    if (Node)
                    {

                        hr = Node->get_nodeName(&bstrNodeName);
                        strcpy(buff, (char *)_bstr_t(bstrNodeName));

                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamepseudo)) == 0)
                        {
                            hr = Node->get_text(&bstrNodeText);
                            strcpy(out_pseudo, (char *)_bstr_t(bstrNodeText));
                        }
                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg)) == 0)
                        {
                            hr = Node->get_text(&bstrNodeText);
                            strcpy(out_msg, (char *)_bstr_t(bstrNodeText));
                        }
                    }
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return hr;
}

int CXmlMessaging::GetScroll_tchat(char *xml, char *out_msg)
{
    MSXML2::IXMLDOMDocument2Ptr pDocChat;
    HRESULT hr = pDocChat.CreateInstance("Msxml2.DOMDocument.4.0");

    // hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
    //        IID_IXMLDOMDocument2, (void**)&pDoc); // Check the return value, hr
    SUCCEEDED(hr) ? 0 : throw hr;

    pDocChat->async = VARIANT_FALSE; // default - true,

    BSTR bstrNodeNamemsg = SysAllocString(L"LINE");
    // BSTR bstrNodeNamepseudo = SysAllocString(L"pseudo");

    // BSTR bstrAttributeNameIp = SysAllocString(L"WHO");
    BSTR bstrNodeName;
    BSTR bstrNodeText;

    //		char buffer[500];

    strcpy(out_msg, "");

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        int i = 0;
        int j = 0;

        if (pDocChat->loadXML(_bstr_t(xml)))
        {
            Roots = pDocChat->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            char buff[100];
            hr = Roots->get_nodeName(&bstrNodeName);
            strcpy(buff, (char *)_bstr_t(bstrNodeName));
            if (!(strcmp(buff, "SCROLL")))
            {

                for (i = 0; i < NodeList->length; i++)
                {
                    // strcpy(MapList->List[i].mappeur,"");
                    // strcpy(MapList->List[i].host,"");

                    hr = NodeList->get_item(i, &Node);

                    if (Node)
                    {

                        hr = Node->get_nodeName(&bstrNodeName);
                        strcpy(buff, (char *)_bstr_t(bstrNodeName));

                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNamemsg)) == 0)
                        {
                            hr = Node->get_text(&bstrNodeText);
                            char tmp[200];

                            strcpy(tmp, (char *)_bstr_t(bstrNodeText));
                            char ee[200];
                            sprintf(ee, "%s ", tmp);

                            strcat(out_msg, ee);
                        }
                    }
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return hr;
}

int CXmlMessaging::GetPings_tchat(char *xml, char *out_msg)
{
    MSXML2::IXMLDOMDocument2Ptr pDocChat;
    HRESULT hr = pDocChat.CreateInstance("Msxml2.DOMDocument.4.0");

    // hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
    //        IID_IXMLDOMDocument2, (void**)&pDoc); // Check the return value, hr
    SUCCEEDED(hr) ? 0 : throw hr;

    pDocChat->async = VARIANT_FALSE; // default - true,

    BSTR bstrNodeNamemsg = SysAllocString(L"m");
    BSTR bstrNodeNameclient = SysAllocString(L"c");
    BSTR bstrNodeNamepseudo = SysAllocString(L"p");
    BSTR bstrNodeNamesite = SysAllocString(L"ma");
    // BSTR bstrNodeNamepseudo = SysAllocString(L"pseudo");

    // BSTR bstrAttributeNameIp = SysAllocString(L"WHO");
    BSTR bstrNodeName;
    BSTR bstrNodeNamea;
    BSTR bstrNodeText;

    //		char buffer[500];

    strcpy(out_msg, "");

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        int i = 0;
        int j = 0;

        if (pDocChat->loadXML(_bstr_t(xml)))
        {
            Roots = pDocChat->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            char buff[100];
            hr = Roots->get_nodeName(&bstrNodeName);
            strcpy(buff, (char *)_bstr_t(bstrNodeName));
            if (!(strcmp(buff, "m")))
            {

                for (i = 0; i < NodeList->length; i++)
                {
                    // strcpy(MapList->List[i].mappeur,"");
                    // strcpy(MapList->List[i].host,"");

                    hr = NodeList->get_item(i, &Node);

                    if (Node)
                    {

                        hr = Node->get_nodeName(&bstrNodeName);
                        strcpy(buff, (char *)_bstr_t(bstrNodeName));

                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameclient)) == 0)
                        {
                            MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;
                            Nodechild = Node->GetchildNodes();

                            char map[200];
                            char pseudo[200];
                            for (int aj = 0; aj < Nodechild->length; aj++)
                            {
                                hr = Nodechild->get_item(aj, &Nodec);
                                if (Nodec)
                                {
                                    hr = Nodec->get_nodeName(&bstrNodeNamea);
                                    if (strcmp((char *)_bstr_t(bstrNodeNamea), (char *)_bstr_t(bstrNodeNamepseudo)) ==
                                        0)
                                    {
                                        hr = Nodec->get_text(&bstrNodeText);

                                        strcpy(pseudo, (char *)_bstr_t(bstrNodeText));
                                    }
                                    if (strcmp((char *)_bstr_t(bstrNodeNamea), (char *)_bstr_t(bstrNodeNamesite)) == 0)
                                    {
                                        hr = Nodec->get_text(&bstrNodeText);

                                        strcpy(map, (char *)_bstr_t(bstrNodeText));
                                    }
                                }
                            }
                            char ee[200];
                            sprintf(ee, "%s %s*", pseudo, map);

                            strcat(out_msg, ee);
                        }
                    }
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return hr;
}

HRESULT CXmlMessaging::GetTournois(vector<tournois> *vtournois, playerfile_t *playerconfig)
{

    BSTR bstrNodeName;
    BSTR bstrNodeDesc = SysAllocString(L"DESC");
    BSTR bstrNodeMap = SysAllocString(L"MAP");
    BSTR bstrNodeId = SysAllocString(L"ID");
    BSTR bstrNodeRound = SysAllocString(L"ROUND");
    BSTR bstrNodeTimeout = SysAllocString(L"TIMEOUT");
    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;

    long j = 0;

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2", playerconfig->player_id, playerconfig->player_pwd);
        Crypt(buffer, "get_tournois", url);

        // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );
        tournois letournois;

        if (XMLHttpRequest(buffer) == true)
        {
            Roots = pDoc->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            for (i = 0; i < NodeList->length; i++)
            {

                hr = NodeList->get_item(i, &Node);
                if (Node)
                {

                    Nodechild = Node->GetchildNodes();
                    for (j = 0; j < Nodechild->length; j++)
                    {

                        hr = Nodechild->get_item(j, &Nodec);
                        if (Nodec)
                        {
                            hr = Nodec->get_nodeName(&bstrNodeName);

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeDesc)) == 0)
                            {
                                hr = Nodec->get_text(&bstrNodeText);
                                strcpy(letournois.desc, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeMap)) == 0)
                            {
                                hr = Nodec->get_text(&bstrNodeText);
                                strcpy(letournois.map, (char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeId)) == 0)
                            {
                                hr = Nodec->get_text(&bstrNodeText);
                                letournois.cle_tournois = atoi((char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeRound)) == 0)
                            {
                                hr = Nodec->get_text(&bstrNodeText);
                                letournois.cle_noeud = atoi((char *)_bstr_t(bstrNodeText));
                            }
                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeTimeout)) == 0)
                            {
                                hr = Nodec->get_text(&bstrNodeText);
                                letournois.timeout = atoi((char *)_bstr_t(bstrNodeText));
                            }
                        }
                    }
                    vtournois->push_back(letournois);

                    //	MapList->count++;
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return S_OK;
}

HRESULT CXmlMessaging::GetTournoisDetail(vector<ekip> *lesekip, playerfile_t *playerconfig, int round, int cle_tournois)
{

    BSTR bstrNodeName;
    BSTR bstrNodeTeam = SysAllocString(L"TEAM");
    BSTR bstrNodeDetail = SysAllocString(L"DETAIL");
    BSTR bstrNodeEkip = SysAllocString(L"EKIP");
    BSTR bstrNodeNameName = SysAllocString(L"NAME");
    BSTR bstrNodeJoueur = SysAllocString(L"JOUEUR");
    BSTR bstrNodePlayer = SysAllocString(L"PLAYER");
    BSTR bstrNodeCamp = SysAllocString(L"CAMP");

    BSTR bstrNodeDesc = SysAllocString(L"DESC");
    BSTR bstrNodeMap = SysAllocString(L"MAP");
    BSTR bstrNodeId = SysAllocString(L"ID");
    BSTR bstrNodeRound = SysAllocString(L"ROUND");

    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;

    long j = 0;

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodePtr Nodece = NULL;
        MSXML2::IXMLDOMNodePtr Nodej = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechilde = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechildj = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&LESOFT=2&ROUND=%i&CLE_TOURNOIS=%i", playerconfig->player_id,
                playerconfig->player_pwd, round, cle_tournois);
        Crypt(buffer, "info_tournois", url);

        // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );
        ekip lekip;

        if (XMLHttpRequest(buffer) == true)
        {
            Roots = pDoc->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            for (i = 0; i < NodeList->length; i++)
            {

                hr = NodeList->get_item(i, &Node);
                if (Node)
                {
                    hr = Node->get_nodeName(&bstrNodeName);
                    char ttt[100];
                    strcpy(ttt, (char *)_bstr_t(bstrNodeName));
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeDetail)) == 0)
                    {
                        Nodechild = Node->GetchildNodes();
                        for (int ja = 0; ja < Nodechild->length; ja++)
                        {
                            hr = Nodechild->get_item(ja, &Nodece);
                            hr = Nodece->get_nodeName(&bstrNodeName);

                            if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeCamp)) == 0)
                            {
                                hr = Nodece->get_text(&bstrNodeText);
                                int y = atoi((char *)_bstr_t(bstrNodeText));
                                playerconfig->ekip_tournois = y;
                            }
                        }
                    }
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeTeam)) == 0)
                    {

                        Nodechild = Node->GetchildNodes();
                        for (j = 0; j < Nodechild->length; j++)
                        {

                            hr = Nodechild->get_item(j, &Nodec);
                            if (Nodec)
                            {
                                hr = Nodec->get_nodeName(&bstrNodeName);

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeEkip)) == 0)
                                {
                                    ekip cekip;
                                    Nodechilde = Nodec->GetchildNodes();
                                    for (int ja = 0; ja < Nodechilde->length; ja++)
                                    {
                                        hr = Nodechilde->get_item(ja, &Nodece);
                                        hr = Nodece->get_nodeName(&bstrNodeName);

                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeNameName)) ==
                                            0)
                                        {
                                            hr = Nodece->get_text(&bstrNodeText);
                                            strcpy(cekip.nom, (char *)_bstr_t(bstrNodeText));
                                        }
                                        if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeJoueur)) == 0)
                                        {
                                            player joueur;
                                            Nodechildj = Nodece->GetchildNodes();
                                            for (int jac = 0; jac < Nodechildj->length; jac++)
                                            {
                                                hr = Nodechildj->get_item(jac, &Nodej);
                                                hr = Nodej->get_nodeName(&bstrNodeName);

                                                if (strcmp((char *)_bstr_t(bstrNodeName),
                                                           (char *)_bstr_t(bstrNodePlayer)) == 0)
                                                {
                                                    hr = Nodej->get_text(&bstrNodeText);
                                                    strcpy(joueur.login, (char *)_bstr_t(bstrNodeText));
                                                }
                                                joueur.cle_joueur = 0;
                                                cekip.list.push_back(joueur);
                                            }
                                        }
                                    }
                                    lesekip->push_back(cekip);

                                    //	strcpy (lekip.desc ,  (char*)_bstr_t(bstrNodeText));
                                }
                            }
                        }
                        // vtournois->push_back(letournois);
                    }
                    //	MapList->count++;
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return S_OK;
}

//------------------------------------------------------------------------------
HRESULT CXmlMessaging::setobjet(playerfile_t *playerconfig, vec3_t pos, vec3_t dir, vec3_t haut, int type_objet,
                                char *lamap)
{
    if (modedebug)
    {
        return S_OK;
    }
    else
    {
        char buffer[500];
        char url[400];
        sprintf(url,
                "LENUM=%i&LEPASS=%s&P[x]=%d&P[y]=%d&P[z]=%d&D[x]=%d&D[y]=%d&D[z]=%d&U[x]=%d&U[y]=%d&U[z]=%d&T=%i&M=%s",
                playerconfig->player_id, playerconfig->player_pwd, int(pos[0]), int(pos[1]), int(pos[2]), int(dir[0]),
                int(dir[1]), int(dir[2]), int(haut[0]), int(haut[1]), int(haut[2]), type_objet, lamap);
        Crypt(buffer, "set_objet", url);

        // sprintf(  buffer,"%s/script/romustrike/delete_server.php3?CLE_SERVEUR=%i",GServerName, serverid );
        if (XMLHttpRequest(buffer) == true)
        {
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
    }
}

HRESULT CXmlMessaging::getobjet(vector<CPhysEnv *> *lescar, playerfile_t *playerconfig, char *lamap)
{
    BSTR bstrNodeName;
    BSTR bstrNodeTeam = SysAllocString(L"OBJ");
    BSTR bstrNodex = SysAllocString(L"X");
    BSTR bstrNodey = SysAllocString(L"Y");
    BSTR bstrNodez = SysAllocString(L"Z");

    BSTR bstrNodedx = SysAllocString(L"DX");
    BSTR bstrNodedy = SysAllocString(L"DY");
    BSTR bstrNodedz = SysAllocString(L"DZ");

    BSTR bstrNodehx = SysAllocString(L"HX");
    BSTR bstrNodehy = SysAllocString(L"HY");
    BSTR bstrNodehz = SysAllocString(L"HZ");

    BSTR bstrNodetype = SysAllocString(L"TYPE");
    BSTR bstrNodeid = SysAllocString(L"ID");

    BSTR bstrNodeText;
    char buffer[500];
    long i = 0;

    long j = 0;

    try
    {

        MSXML2::IXMLDOMNodePtr spXMLNode = NULL;
        MSXML2::IXMLDOMNodeListPtr NodeList = NULL;
        MSXML2::IXMLDOMElementPtr Roots = NULL;
        MSXML2::IXMLDOMNodePtr Node = NULL;
        MSXML2::IXMLDOMNodePtr Nodec = NULL;
        MSXML2::IXMLDOMNodePtr Nodece = NULL;
        MSXML2::IXMLDOMNodePtr Nodej = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechild = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechilde = NULL;
        MSXML2::IXMLDOMNodeListPtr Nodechildj = NULL;

        MSXML2::IXMLDOMNamedNodeMapPtr MAP;
        MSXML2::IXMLDOMAttributePtr ATTR;

        char url[400];
        sprintf(url, "LENUM=%i&LEPASS=%s&LAMAP=%s", playerconfig->player_id, playerconfig->player_pwd, lamap);
        Crypt(buffer, "get_objet", url);

        // sprintf(  buffer,"%s/script/romustrike/get_map.php3?LESOFT=2",GServerName );
        //	ekip lekip;

        if (XMLHttpRequest(buffer) == true)
        {
            Roots = pDoc->GetdocumentElement();

            HRESULT hr = Roots->get_childNodes(&NodeList);
            for (i = 0; i < NodeList->length; i++)
            {

                hr = NodeList->get_item(i, &Node);
                if (Node)
                {
                    hr = Node->get_nodeName(&bstrNodeName);
                    char ttt[100];
                    strcpy(ttt, (char *)_bstr_t(bstrNodeName));
                    if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeTeam)) == 0)
                    {

                        vec3_t pos, d, h;
                        int type = 0;

                        Nodechild = Node->GetchildNodes();
                        for (j = 0; j < Nodechild->length; j++)
                        {

                            hr = Nodechild->get_item(j, &Nodec);
                            if (Nodec)
                            {

                                hr = Nodechild->get_item(j, &Nodece);
                                hr = Nodece->get_nodeName(&bstrNodeName);

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodeid)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    int id = atoi((char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodex)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    pos[0] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodey)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    pos[1] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodez)) == 0)
                                {
                                    //	player joueur;
                                    hr = Nodece->get_text(&bstrNodeText);
                                    pos[2] = atof((char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodedx)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    d[0] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodedy)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    d[1] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodedz)) == 0)
                                {
                                    //	player joueur;
                                    hr = Nodece->get_text(&bstrNodeText);
                                    d[2] = atof((char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodehx)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    h[0] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodehy)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    h[1] = atof((char *)_bstr_t(bstrNodeText));
                                }
                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodehz)) == 0)
                                {
                                    //	player joueur;
                                    hr = Nodece->get_text(&bstrNodeText);
                                    h[2] = atof((char *)_bstr_t(bstrNodeText));
                                }

                                if (strcmp((char *)_bstr_t(bstrNodeName), (char *)_bstr_t(bstrNodetype)) == 0)
                                {
                                    hr = Nodece->get_text(&bstrNodeText);
                                    type = atoi((char *)_bstr_t(bstrNodeText));
                                }

                                //	lesekip->push_back(cekip);

                                //	strcpy (lekip.desc ,  (char*)_bstr_t(bstrNodeText));
                            }
                            if (type > 0)
                            {
                                CPhysEnv *nouveau = new CPhysEnv;

                                // nouveau->AxeG =lejoueur[VRAI]->voiture->AxeG;
                                nouveau->LoadData(pos);

                                nouveau->AxeDevant = d;
                                nouveau->AxeHaut = h;

                                nouveau->SetPos(pos, vec3_t(0.0f, 0.0f, 0.0f));

                                lescar->push_back(nouveau);
                            }
                        }
                        // vtournois->push_back(letournois);
                    }
                    //	MapList->count++;
                }
            }
        }
    }
    catch (_com_error &e)
    {
        MessageBox(NULL, TEXT("erreur accés xml"), TEXT("ROMUSTRIKE"), MB_OK);
    }

    // CoUninitialize();

    return S_OK;
}
