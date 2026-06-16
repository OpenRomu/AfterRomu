////////////////////////////////////////////////////////////////////////
//
//	Nom:		BubSock.h
//	Auteur:		BUILS Marc (Bub325)
//	E_Mail:		bub325@ifrance.com
//	Fonction:	Faciliter l'utilisation des sockets
//	Methode:	Creation de fonction simple
//
////////////////////////////////////////////////////////////////////////

// #define SIZE_CHAINE 0xFFFF		// Taille des chaine de reception

// Utilise <winsock.h>
#ifndef _WINSOCKAPI_
#include <winsock.h>
#endif
#pragma comment(lib, "wsock32.lib")

#include "resource.h"

// Sane defaults
#define USE_SOCKCLOSE
#define USE_SOCKREAD

////////////////////////////////////////////////////////////////////////
//	SOCKOPEN:
//		Fonction d'initialisation et d'ouverture du socket
////////////////////////////////////////////////////////////////////////

#define BUBSOCK_OK 0    // La connection est faite
#define FALSE_WINSOCK 1 // Winsock ne s'est pas chargé
#define FALSE_HOST 2    // L'host n'est pas valide
#define FALSE_SOCKET 3  // La création du socket a échoué
#define FALSE_CONNECT 4 // La connection a échouée
#define FALSE_THREAD 5  // Le thread de reception n'a pas été créé

// Déclaration de la fonction de reception
DWORD WINAPI ClientRecv(LPVOID arg);

// Déclaration de sockopen
int sockopen(SOCKET *Client, char *host, int port);

////////////////////////////////////////////////////////////////////////
//	SOCKREAD:
//		Fonction de reception des données
////////////////////////////////////////////////////////////////////////

// declaration de la fonction Recv
void SOCKREAD(SOCKET, char *);
void SOCKCLOSE(SOCKET);

////////////////////////////////////////////////////////////////////////
//	SOCKCLOSE:
//		Fonction de fermeture de socket
////////////////////////////////////////////////////////////////////////

void sockclose(SOCKET Client);

////////////////////////////////////////////////////////////////////////
//	SOCKWRITE:
//		Fonction d'envoie de donnée
////////////////////////////////////////////////////////////////////////

void sockwrite(SOCKET _Client, char *chaine);

////////////////////////////////////////////////////////////////////////
//	SOCKLISTEN:
//		Fonction de mise en écoute d'un port
////////////////////////////////////////////////////////////////////////

#define FALSE_BIND 4   // Le bind du socket a échoué
#define FALSE_LISTEN 6 // La mise en écoute du port a échoué

// Déclaration de la fonction de reception
DWORD WINAPI ServListen(LPVOID arg);

int socklisten(SOCKET *Client, int port);

////////////////////////////////////////////////////////////////////////
//	SOCKLISTEN:
//		Fonction de lecture du port
////////////////////////////////////////////////////////////////////////

// declaration de la fonction SOCKLISTEN
void SOCKLISTEN(SOCKET);
