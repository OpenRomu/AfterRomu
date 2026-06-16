////////////////////////////////////////////////////////////////////////
//
//	Nom:		BubSock.cpp
//	Auteur:		BUILS Marc (Bub325)
//	E_Mail:		bub325@ifrance.com
//	Fonction:	Faciliter l'utilisation des sockets
//	Methode:	Creation de fonction simple
//
////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "stdafx.h"
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")

#include "resource.h"
#include "bubsock.h"

////////////////////////////////////////////////////////////////////////
//	SOCKOPEN:
//		Fonction d'initialisation et d'ouverture du socket
////////////////////////////////////////////////////////////////////////

int sockopen(SOCKET *Client, char *host, int port)
{
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
        return FALSE_WINSOCK;
    }
    SOCKADDR_IN ClientSock;
    *Client = INVALID_SOCKET;
    HOSTENT *ServerInfos = NULL;
    if (!(ServerInfos = gethostbyname(host)))
    {
        WSACleanup();
        return FALSE_HOST;
    }
    memset(&ClientSock, 0, sizeof(SOCKADDR_IN));
    memcpy(&ClientSock.sin_addr.s_addr, ServerInfos->h_addr, ServerInfos->h_length);
    ClientSock.sin_port = htons(port);
    ClientSock.sin_family = AF_INET;
    if (!(*Client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        WSACleanup();
        return FALSE_SOCKET;
    }
    if (connect(*Client, (SOCKADDR *)&ClientSock, sizeof(SOCKADDR_IN)))
    {
        WSACleanup();
        return FALSE_CONNECT;
    }

    // fonction de reception:
    DWORD RecvThreadID = 0;
    HANDLE RecvThread = NULL;

    if (!(RecvThread = CreateThread(NULL, 0, &ClientRecv, Client, 0, &RecvThreadID)))
    {
        WSACleanup();
        return FALSE_THREAD;
    }
    CloseHandle(RecvThread);

    return BUBSOCK_OK;
}

////////////////////////////////////////////////////////////////////////
//	SOCKREAD:
//		Fonction de reception des données
////////////////////////////////////////////////////////////////////////

DWORD WINAPI ClientRecv(LPVOID arg)
{
    char chaine[SIZE_CHAINE];
    int size_chaine = 0;

    SOCKET Client = *(SOCKET *)arg;

    while (recv(Client, &chaine[size_chaine++], 1, 0) > 0)
    {

        if (chaine[size_chaine - 1] == '\0') //\n
        {
            chaine[size_chaine] = '\0';
            SOCKREAD(Client, chaine);
            size_chaine = 0;
        }
    }
    WSACleanup();
    /*
    #ifdef USE_SOCKCLOSE
        SOCKCLOSE(Client);
    #endif
    */
    return 0;
}

////////////////////////////////////////////////////////////////////////
//	SOCKCLOSE:
//		Fonction de fermeture de socket
////////////////////////////////////////////////////////////////////////

void sockclose(SOCKET Client)
{
    closesocket(Client);
}

////////////////////////////////////////////////////////////////////////
//	SOCKWRITE:
//		Fonction d'envoie de donnée
////////////////////////////////////////////////////////////////////////

void sockwrite(SOCKET _Client, char *chaine)
{
    send(_Client, chaine, strlen(chaine), NULL);
}

////////////////////////////////////////////////////////////////////////
//	SOCKLISTEN:
//		Fonction de mise en écoute d'un port
////////////////////////////////////////////////////////////////////////

int socklisten(SOCKET *Client, int port)
{
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
        return FALSE_WINSOCK;
    }
    SOCKADDR_IN ClientSock;
    *Client = INVALID_SOCKET;

    ClientSock.sin_addr.s_addr = INADDR_ANY;
    ClientSock.sin_port = htons(port);
    ClientSock.sin_family = AF_INET;
    if (!(*Client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        WSACleanup();
        return FALSE_SOCKET;
    }

    // bind the socket
    if ((bind(*Client, (LPSOCKADDR)&ClientSock, sizeof(ClientSock))) == SOCKET_ERROR)
    {
        WSACleanup();
        return FALSE_BIND;
    }

    // get socket to listen
    if ((listen(*Client, 2)) == SOCKET_ERROR)
    {
        WSACleanup();
        return FALSE_LISTEN;
    }

    // fonction de reception:
    DWORD RecvThreadID = 0;
    HANDLE RecvThread = NULL;

    if (!(RecvThread = CreateThread(NULL, 0, &ServListen, Client, 0, &RecvThreadID)))
    {
        WSACleanup();
        return FALSE_THREAD;
    }
    CloseHandle(RecvThread);

    return BUBSOCK_OK;
}

////////////////////////////////////////////////////////////////////////
//	SOCKLISTEN:
//		Fonction de lecture du port
////////////////////////////////////////////////////////////////////////

DWORD WINAPI ServListen(LPVOID arg)
{
    SOCKET Serv = *(SOCKET *)arg;

#ifdef USE_SOCKLISTEN
    SOCKET Client;
    while ((Client = accept(Serv, NULL, NULL)) > 0)
    {
        // fonction de reception:
        DWORD RecvThreadID = 0;
        HANDLE RecvThread = NULL;

        if (!(RecvThread = CreateThread(NULL, 0, &ClientRecv, &Client, 0, &RecvThreadID)))
        {
            WSACleanup();
            return FALSE_THREAD;
        }
        CloseHandle(RecvThread);

        SOCKLISTEN(Client);
    }
#endif

    return 0;
}