
#ifndef __playerfile_h__
#define __playerfile_h__

#pragma warning(disable:4786)

#include <vector>
#include <string>
using namespace std;

class playerfile_t {
public:
    playerfile_t();
	~playerfile_t();
	bool playerfile_t::load();
    bool playerfile_t::save();
	bool playerfile_t::unset() ;
    int player_id;
	int score;
	

	char player_name[30];
	char player_mp3[100];
	int player_mp3id;
	int player_model;
	char playermail[30];


	char player_pwd[20];
	char msg1[50];
	char msg2[50];
	char msg3[50];
	char msg4[50];
	char scroll[600];
	char panel_joueur[1000];
	char panel_stats[1000];
	int ekip_tournois;
	bool is_op;
	bool is_autorise;
    
};

#endif // __playerfile_h__
