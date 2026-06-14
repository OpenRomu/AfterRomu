#include "stdafx.h"
#include "playerfile.h"
#include "exception.h"

playerfile_t::playerfile_t()
    : player_id(0), is_op(false), score(0), player_model(0), ekip_tournois(0), is_autorise(false)
{
    strcpy(player_name, "");
    strcpy(playermail, "");
    strcpy(player_mp3, "vito_1");
    strcpy(msg1, "");
    strcpy(msg2, "");
    strcpy(msg3, "");
    strcpy(msg4, "");

    strcpy(player_pwd, "");
    // scroll=new char[600];

    strcpy(scroll, "********************************************************dEBUGMODE**********************************"
                   "**********************");
    strcpy(panel_joueur, "**");
}

playerfile_t::~playerfile_t()
{
    // delete [] scroll;
}

bool playerfile_t::load()
{
    try
    {

        FILE *f = fopen("data/player.ini", "r");
        vector<string> tokens;
        if (f)
        {
            char tmp[256];
            while (fgets(tmp, sizeof(tmp), f))
            {
                if (tmp[0] == '/' && tmp[1] == '/')
                    continue;
                static char seps[] = " ,=\n\r";
                char *token = strtok(tmp, seps);
                while (token)
                {
                    tokens.push_back(token);
                    token = strtok(0, seps);
                }
            }

            fclose(f);
        }

        int tok = 0;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (tokens[i] == "[playerid]")
            {
                tok = 1;
                continue;
            }
            if (tokens[i] == "[playerpwd]")
            {
                tok = 2;
                continue;
            }

            switch (tok)
            {
            case 1:
                player_id = atoi(tokens[i].c_str());
                tok = 0;
                break;
            case 2:
                sprintf(player_pwd, "%s", tokens[i].c_str());
                tok = 0;
                break;
            }
        }
    }
    catch (basic_exception &e)
    {
        return false;
    }
    return true;
}
bool playerfile_t::unset()
{
    unlink("data/player.ini");
    return true;
}
bool playerfile_t::save()
{
    try
    {

        FILE *f = fopen("data/player.ini", "w");
        vector<string> tokens;

        if (f)
        {
            char tmp[100];
            sprintf(tmp, "[playerid]\n%i\n[playerpwd]\n%s\n", player_id, player_pwd);

            fputs(tmp, f);

            fclose(f);
        }
    }
    catch (basic_exception &e)
    {
        return false;
    }
    return true;
}
