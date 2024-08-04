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
 * config file
 */
#include "stdafx.h"
#include "exception.h"
#include "config.h"

// #include <fstream>

config_t::config_t(const char* filename)
    : xres(640)
    , yres(480)
    , msens(120)
    , bpp(16)
    , hz(60)
    , vsync_off(false)
    , fullscreen(false)
    , isdebug(false)
    , GAMMA(5)
    , music_on(true)
    , viseur(10)
{

    sprintf(server_xml, "romu.soft.free.fr");
    sprintf(server_tchat, "romu.soft.free.fr");

    FILE* f = fopen(filename, "r");
    vector<string> tokens;
    if (f) {
        char tmp[256];
        while (fgets(tmp, sizeof(tmp), f)) {
            if (tmp[0] == '/' && tmp[1] == '/')
                continue;
            static char seps[] = " ,=\n\r";
            char* token = strtok(tmp, seps);
            while (token) {
                tokens.push_back(token);
                token = strtok(0, seps);
            }
        }

        fclose(f);
    }

    int disp_count = 0;
    int key_count = 0;
    int cf = 0;
    keys[0] = (DWORD)VK_UP;
    keys[1] = (DWORD)VK_DOWN;
    keys[2] = (DWORD)VK_RIGHT;
    keys[3] = (DWORD)VK_LEFT;
    keys[4] = (DWORD)VK_SHIFT;
    keys[5] = (DWORD)VK_CONTROL;
    keys[6] = (DWORD)VK_LBUTTON;
    keys[7] = (DWORD)VK_NEXT;
    keys[8] = (DWORD)VK_END;
    keys[9] = (DWORD)VK_HOME;
    keys[10] = (DWORD)VK_PRIOR;
    keys[11] = (DWORD)VK_INSERT;
    keys[12] = (DWORD)VK_DELETE;

    int tok = 0;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "[display]") {
            tok = 1;
            continue;
        } else if (tokens[i] == "[keys]") {
            tok = 3;
            continue;
        } else if (tokens[i] == "[debug]") {
            tok = 2;
            continue;
        } else if (tokens[i] == "[gamma]") {
            tok = 4;
            continue;
        } else if (tokens[i] == "[server_xml]") {
            tok = 6;
            continue;
        } else if (tokens[i] == "[server_tchat]") {
            tok = 7;
            continue;
        } else if (tokens[i] == "[mouse]") {
            tok = 8;
            continue;
        } else if (tokens[i] == "[music_on]") {
            tok = 9;
            continue;
        } else if (tokens[i] == "[viseur]") {
            tok = 10;
            continue;
        }

        switch (tok) {
        case 1: {
            switch (disp_count++) {
            case 0:
                xres = atoi(tokens[i].c_str());
                break;
            case 1:
                yres = atoi(tokens[i].c_str());
                break;
            case 2:
                bpp = atoi(tokens[i].c_str());
                break;
            case 3:
                hz = atoi(tokens[i].c_str());
                break;
            case 4:
                if (tokens[i] == "true" || tokens[i] == "TRUE") {
                    vsync_off = true;
                }
                break;
            case 5:
                if (tokens[i] == "true" || tokens[i] == "TRUE") {
                    fullscreen = true;
                }
                break;
            }
        } break;
        case 3: {

            keys[key_count++] = (DWORD)atoi(tokens[i].c_str());
        }

        break;
        case 2:
            if (tokens[i] == "true" || tokens[i] == "TRUE") {
                isdebug = true;
            }

            break;
        case 4:
            GAMMA = atoi(tokens[i].c_str());
            if (GAMMA > 8)
                GAMMA = 8;
            if (GAMMA < 0)
                GAMMA = 0;

            break;
        case 6:
            strcpy(server_xml, tokens[i].c_str());

            break;
        case 7:
            strcpy(server_tchat, tokens[i].c_str());

            break;
        case 8:
            msens = atoi(tokens[i].c_str());

            break;
        case 9:
            if (tokens[i] == "false" || tokens[i] == "FALSE") {
                music_on = false;
            }
            break;
        case 10:
            viseur = atoi(tokens[i].c_str());
            break;
        }
    }
}

void config_t::save()
{
    try {

        FILE* f = fopen("data/config.ini", "w");
        vector<string> tokens;

        if (f) {

            char faux[] = "false";
            char vrai[] = "true";
            char kdebug[10];
            char kfull[10];
            char kvsync[10];
            char kmusic_on[10];
            if (isdebug) {
                sprintf(kdebug, "%s", vrai);
            } else {
                sprintf(kdebug, "%s", faux);
            }

            if (fullscreen) {
                sprintf(kfull, "%s", vrai);
            } else {
                sprintf(kfull, "%s", faux);
            }

            if (vsync_off) {
                sprintf(kvsync, "%s", vrai);
            } else {
                sprintf(kvsync, "%s", faux);
            }

            if (music_on) {
                sprintf(kmusic_on, "%s", vrai);
            } else {
                sprintf(kmusic_on, "%s", faux);
            }

            char touche[1000];

            strcpy(touche, "");
            for (int a = 0; a < 13; a++) {
                char key[6];
                if (a != 0) {
                    sprintf(key, ",%d", keys[a]);

                } else {
                    sprintf(key, "%d", keys[a]);
                }

                strcat(touche, key);
            }

            char tmp[1000];

            sprintf(tmp,
                "[debug]\n%s\n[gamma]\n%d\n[display]\n%d,%d,%d,%d,%s,%s\n[keys]\n%s\n[server_xml]\n%s\n[server_tchat]"
                "\n%s\n[mouse]\n%d\n[music_on]\n%s\n[viseur]\n%d\n",
                kdebug, GAMMA, xres, yres, bpp, hz, kvsync, kfull, touche, server_xml, server_tchat, msens, kmusic_on,
                viseur);

            fputs(tmp, f);

            fclose(f);
        }

    } catch (basic_exception& e) {
        ;
    }
}
