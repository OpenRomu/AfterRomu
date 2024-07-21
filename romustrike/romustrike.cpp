// romustrike.cpp : Defines the entry point for the application.
//
#define USE_SOCKCLOSE
#define USE_SOCKREAD
#define _CRTDBG_MAP_ALLOC
#define  _CRTDBG_MAP_ALLOC_NEW
#include <stdlib.h>
#include <crtdbg.h>

#include "stdafx.h"
#include <windows.h>

#include <winhttp.h> 


#include "romustrike.h"

#include <dplay8.h>
#include <dplobby8.h>
#include <dxerr8.h>
#include <mmsystem.h>
#include <dsound.h>
#include "engine.h"

#import <msxml4.dll>
using namespace MSXML2;
#include "Nb30.h"



// les define



typedef struct _ASTAT_
{
 
 ADAPTER_STATUS adapt;
 NAME_BUFFER NameBuff [30];
 
}ASTAT, * PASTAT;
ASTAT Adapter;
typedef struct {
    HWND        hWindow;        // Handle for the dialog box
    HINTERNET   hConnect;       // Connection handle
    HINTERNET   hRequest;       // Resource request handle
    int         nURL;           // ID of the URL edit box
    int         nHeader;        // ID of the header output box
    int         nResource;      // ID of the resource output box
    DWORD       dwSize;         // Size of the latest data block
    DWORD       dwTotalSize;    // Size of the total data
    LPSTR       lpBuffer;       // Buffer for storing read data
    WCHAR       szMemo[512];    // String providing state information
} REQUEST_CONTEXT;
static REQUEST_CONTEXT  rcContext1,
                        rcContext2;

CRITICAL_SECTION g_CallBackCritSec;



#define MAX_LOADSTRING 100
CRITICAL_SECTION g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) SAFE_DELETE( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );
#define MAX_PLAYER_NAME         14

GUID g_guidApp = { 0x2ae835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 } };

GUID TCPguid = { 3959323552,25229, 4562, { 174, 15, 0, 96,151, 176, 20,17 } };

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
IDirectPlay8Peer*  g_pDP                         = NULL;    // DirectPlay peer objec
IDirectPlay8LobbiedApplication* g_pLobbiedApp    = NULL;    // DirectPlay lobbied app 
IGraphBuilder *g_pGraphBuilder =NULL;
Engine*  g_e                         = NULL;    // DirectPlay peer object
config_t config("data/config.ini");

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HRESULT  init_dplay();
HRESULT WINAPI   DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );


int getAdresseMac(int numeroInterface, char * adrMAC)
{
 NCB Ncb;
 LANA_ENUM lenum;
 
 memset ( &Ncb, 0, sizeof(Ncb) );
 Ncb.ncb_command = NCBENUM;
 Ncb.ncb_buffer = (UCHAR *)&lenum;
 Ncb.ncb_length = sizeof(lenum);
 Netbios( &Ncb);
 
 if (numeroInterface <= lenum.length)
 {
  memset (&Ncb, 0, sizeof(Ncb));
  Ncb.ncb_command = NCBRESET;
  Ncb.ncb_lana_num = lenum.lana[numeroInterface];
 
  Netbios(&Ncb);
   
  memset (&Ncb, 0,sizeof(Ncb));
  Ncb.ncb_command = NCBASTAT;
  Ncb.ncb_lana_num = lenum.lana[numeroInterface];
 
  strcpy ( (char*)Ncb.ncb_callname, "*     " );
  Ncb.ncb_buffer = (PUCHAR ) &Adapter;
  Ncb.ncb_length = sizeof(Adapter);
 
  if (Netbios(&Ncb) ==0)
  {
   sprintf(adrMAC, "%02x%02x%02x%02x%02x%02x",
    Adapter.adapt.adapter_address[0],
    Adapter.adapt.adapter_address[1],
    Adapter.adapt.adapter_address[2],
    Adapter.adapt.adapter_address[3],
    Adapter.adapt.adapter_address[4],
    Adapter.adapt.adapter_address[5]);
   return 1;
  }
  else
   return 0;
 }
 else
  return 0;
}
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF); 
//_CrtSetBreakAlloc(19937);
	// Initialize global strings
	/*LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ROMUSTRIKE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	*/
	
//	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ROMUSTRIKE));

	// Main message loop:

	/*while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	*/

	
    try {
			CoInitialize( NULL );

InitializeCriticalSection( &g_csPlayerContext );



		if (S_OK==init_dplay())// 
		{	
			g_e=new Engine();
			g_e->SetpGuidApp(&g_guidApp);
			g_e->Settcpguid(&TCPguid);
			g_e->SetpDP(g_pDP);
			g_e->SetpLB(g_pLobbiedApp);
			g_e->Sethinstance (hInstance);
			g_e->Setlebuild( g_pGraphBuilder);
			g_e->Setpfn(DirectPlayMessageHandler);
			int te=getAdresseMac(0,g_e->MAC);
			//g_e->dezip("afghan.zip");
			
			if (g_e)//
			{
				g_e->create(config.xres, config.yres, config.bpp, config.hz,  config.fullscreen);
 				if (true)
				{
					if (g_e->config.isdebug)
					{
							g_e->le_socket=0;
							g_e->m_chat->addtext("Mode debug RomuChat OFF",2);
					}
					else
					{
						g_e->le_socket=0;
						if (false)
						{
							g_e->le_socket=0;
						}
						else
						{
							if (g_e->m_chat)
							{
								g_e->m_chat->TAB.clear();
								g_e->m_phrase->TAB.clear();
							}
						
						}

					}
				//		g_e->m_chat->addtext(g_e->MAC,2);
		
					g_e->ouvertureporte();//05/12/2002
					g_e->frameTime.Frame();
					//g_e->load_map("sniper_2");
					MSG msg;    
					ZeroMemory(&msg, sizeof(msg));
					int yy=0;
					while((g_e->menu_state !=-1)) 
					{
						if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						} 
						else 
						{
							  g_e->frame();
						}
					}
					
				}	
				g_e->RESET();
				}
			 
			SAFE_DELETE ( g_e );
			g_e=NULL;
			}
			
		if( g_pGraphBuilder )
		{
	 
			g_pGraphBuilder->Release();
			g_pGraphBuilder = NULL;
		}	

		if( g_pDP )
		{
			HRESULT hr = g_pDP->Close(0); 

			g_pDP->Release();
			g_pDP = NULL;
		}

		if( g_pLobbiedApp )
		{
			g_pLobbiedApp->Close( 0 );
			SAFE_RELEASE( g_pLobbiedApp );
		}    

		DeleteCriticalSection( &g_csPlayerContext );
		CoUninitialize();
	 


			}	
			catch(_com_error &e)
			{	
				_bstr_t bstrSource(e.Source());
				_bstr_t bstrDescription(e.Description());
				char buf[200];
				sprintf(buf,"Code = %08lx , Code meaning = %s tSource = %s tDescription = %s\n\nMSXML4.dll non enregistré",  e.Error(),e.ErrorMessage(),(LPCSTR) bstrSource,(LPCSTR) bstrDescription);
				MessageBox(NULL,buf,"erreur COM",0);
			}
	return 0; // (int) msg.wParam
}

//-----------------------------------------------------------------------------
// Name: DirectPlayMessageHandler
// Desc: Handler for DirectPlay messages.  This function is called by
//       the DirectPlay message handler pool of threads, so be careful of thread
//       synchronization problems with shared memory
//-----------------------------------------------------------------------------
HRESULT init_dplay()
{
	HRESULT hr;


		 hr = CoCreateInstance( CLSID_DirectPlay8Peer, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8Peer, 
                                       (LPVOID*) &g_pDP ) ;
  
		hr = CoCreateInstance( CLSID_DirectPlay8LobbiedApplication, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8LobbiedApplication, 
                                       (LPVOID*) &g_pLobbiedApp );

        if (FAILED (  hr = CoCreateInstance(CLSID_FilterGraph, NULL,
                                      CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
                                      reinterpret_cast<void **>(&g_pGraphBuilder))) )
     return hr;

//		g_pDP->Initialize( NULL,DirectPlayMessageHandler, 0 );
		
return  S_OK;
}
     

HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, 
                                         DWORD dwMessageId, 
                                         PVOID pMsgBuffer )
{
    // Try not to stay in this message handler for too long, otherwise
    // there will be a backlog of data.  The best solution is to 
    // queue data as it comes in, and then handle it on other threads.
    
    // This function is called by the DirectPlay message handler pool of 
    // threads, so be careful of thread synchronization problems with shared memory

    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER://
        {

		
			
            HRESULT hr;
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
            pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->lRefCount   = 1;
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

            // Get the peer info and extract its name
            DWORD dwSize = 0;
            DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
            hr = DPNERR_CONNECTING;
            
            // GetPeerInfo might return DPNERR_CONNECTING when connecting, 
            // so just keep calling it if it does
            while( hr == DPNERR_CONNECTING ) 
                hr = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );                                
                
            if( hr == DPNERR_BUFFERTOOSMALL )
            {
                pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
                ZeroMemory( pdpPlayerInfo, dwSize );
                pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
                
                hr = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
                if( SUCCEEDED(hr) )
                {
                    // This stores a extra TCHAR copy of the player name for 
                    // easier access.  This will be redundent copy since DPlay 
                    // also keeps a copy of the player name in GetPeerInfo()
                    DXUtil_ConvertWideStringToGeneric( pPlayerInfo->strPlayerName, 
                                                       pdpPlayerInfo->pwszName, MAX_PLAYER_NAME );    
                    
                                   
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
						g_e->g_dpnidLocalPlayer  = pCreatePlayerMsg->dpnidPlayer;  
						
				g_e->lockequipe();
				g_e->Joueur_Creation( pPlayerInfo->strPlayerName,pCreatePlayerMsg->dpnidPlayer,pPlayerInfo->CLE_JOUEUR);
			  InterlockedIncrement( &g_e->g_lNumberOfActivePlayers );
          	g_e->unlockequipe();
			

						
					}
                
                SAFE_DELETE_ARRAY( pdpPlayerInfo );
            }

            // Tell DirectPlay to store this pPlayerInfo 
            // pointer in the pvPlayerContext.
            pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            //if( g_hDlg != NULL )
                //PostMessage( g_hDlg, WM_APP_UPDATE_STATS, 0, 0 );
			g_e->hit_Romu();

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
            pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;
				g_e->lockequipe();
		if (g_e)
			{
			g_e->Joueur_Destroy(pPlayerInfo->strPlayerName ,pDestroyPlayerMsg->dpnidPlayer);
          	g_e->hit_Romu();

			}
			PLAYER_LOCK();                  // enter player context CS
            PLAYER_RELEASE( pPlayerInfo );  // Release player and cleanup if needed
            PLAYER_UNLOCK();                // leave player context CS

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedDecrement( &g_e->g_lNumberOfActivePlayers );
			
 			g_e->unlockequipe();
		
             break;
        }

        case DPN_MSGID_HOST_MIGRATE:
        {
            PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
            pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;

            // Check to see if we are the new host
            if( pHostMigrateMsg->dpnidNewHost == g_e->g_dpnidLocalPlayer )
                //SeWindowText( g_hDlg, TEXT("SimplePeer (Host)") );
			
				g_e->migratehost();

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            HRESULT g_hrDialog = DPNERR_CONNECTIONLOST;
           //EndDialog( g_hDlg, 0 );
            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;
            if( NULL == pPlayerInfo )
                break;

            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            if( pMsg->dwType == GAME_MSGID_POSITION )
            {
				//g_e->colle_position(pPlayerInfo->dpnidPlayer,pMsg->pos,pMsg->rot,pMsg->ang_dos,pMsg->fps,pMsg->id_model ,pMsg->id_arme ,pMsg->dep,pMsg->act,pMsg->sens);
				
				//ofstream o("bbbb.log");
				//o << "debut" << endl;
  			 GAMEMSG_POSITION* pposMessage = (GAMEMSG_POSITION*) pMsg;
				
				g_e->colle_position(pPlayerInfo->dpnidPlayer,pposMessage);
				//o << "fin" << endl;
                // This message is sent when a player has waved to us, so 
                // post a message to the dialog thread to update the UI.  
                // This keeps the DirectPlay threads from blocking, and also
                // serializes the recieves since DirectPlayMessageHandler can
                // be called simultaneously from a pool of DirectPlay threads.
                //PostMessage( g_hDlg, WM_APP_DISPLAY_WAVE, , 0 );
				//break;
			}
			
			if( pMsg->dwType == GAME_MSGID_MARK )
            {  			 
				GAMEMSG_MARK* pposMessage = (GAMEMSG_MARK*) pMsg;

				g_e->colle_Mark(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_GRENADE )
            {  			 
				GAMEMSG_MARK* pposMessage = (GAMEMSG_MARK*) pMsg;

				g_e->colle_Grenade(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_TIR )
            {
					GAMEMSG_TIR* pposMessage = (GAMEMSG_TIR*) pMsg;

				g_e->recoit_Tir(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_CONFIRME )
            {
					GAMEMSG_CONF_TUE* pposMessage = (GAMEMSG_CONF_TUE*) pMsg;
			
				g_e->score_plus(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_WAVE )
            {

				 GAMEMSG_CHAT* pChatMessage = (GAMEMSG_CHAT*) pMsg;

				g_e->newchat(pPlayerInfo->dpnidPlayer,pPlayerInfo->strPlayerName ,pChatMessage->strChatString);
			//	break;
            }

			//** CFT ici les messages 
			if( pMsg->dwType == GAME_MSGID_CFT )
            {

				GAMEMSG_CFT* CftMessage = (GAMEMSG_CFT*) pMsg;
				g_e->CFT_recoit_message(pPlayerInfo->dpnidPlayer,CftMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_CFT_TOTALE )
            {

				GAMEMSG_CFT_TOTALE* CftMessage = (GAMEMSG_CFT_TOTALE*) pMsg;
				g_e->CFT_HOST_recoit_recapitulatif(pPlayerInfo->dpnidPlayer,CftMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_CFT_KIKKED )
            {  			 
				GAMEMSG_KIKKED* pposMessage = (GAMEMSG_KIKKED*) pMsg;

				g_e->kikked(pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_SCORE )
            {
					GAMEMSG_SCORE* pposMessage = (GAMEMSG_SCORE*) pMsg;

				g_e->Receive_score(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			if( pMsg->dwType == GAME_MSGID_VOIX )
            {
					GAMEMSG_VOIX* pposMessage = (GAMEMSG_VOIX*) pMsg;

				g_e->Receive_voix(pPlayerInfo->dpnidPlayer,pposMessage);
			//	break;
            }
			
            break;
        }
    }

    // Make sure the DirectPlay MessageHandler calls the CNetConnectWizard handler, 
    // so it can be informed of messages such as DPN_MSGID_ENUM_HOSTS_RESPONSE.
      if (dwMessageId!=DPN_MSGID_RECEIVE)
	{
		  if(dwMessageId==DPN_MSGID_DESTROY_PLAYER)
	g_e->menu_state=	g_e->menu_state;

	}

        return g_e->MessageHandler( pvUserContext, dwMessageId, 
                                                    pMsgBuffer );
   

    return S_OK;
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROMUSTRIKE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ROMUSTRIKE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void unescape(char*src,char*dst)
{
int i=0;
	for (i=0;i<strlen(src);i++)
	{
	 
		if ( src[i]==32 || src[i]==60 || (src[i]>=47) && (src[i] < 91)  || (src[i]>96) && (src[i] < 123) )
		{
			dst[i]=src[i];

		}
		else
		{
			if ((src[i]==10) || (src[i]==13))
			{
			dst[i]=' ';
			}
			else
			{
			dst[i]=' ';
			}


		}


	}
	
	dst[i]='\0';

}


void SOCKREAD(SOCKET _Client,char* chaine)
{
	char correct_str[SIZE_CHAINE];
	strcpy(correct_str,"");

if (strlen(chaine)!=0)
{
	unescape(chaine,correct_str);
	g_e->xmlsocket(correct_str);
}

	
}

void SOCKCLOSE(SOCKET _Client)
{
 
	
int o;
o=0;



}

/*

void __stdcall Juggler  (HINTERNET hInternet, DWORD dwContext,
                                                         DWORD dwInternetStatus,
                                                         LPVOID lpvStatusInformation,
                                                         DWORD dwStatusInformationLength)
{
         

}
*/

