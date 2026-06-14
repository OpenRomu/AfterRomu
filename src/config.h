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

#ifndef __config_h__
#define __config_h__

#pragma warning(disable : 4786)

#include <vector>
#include <string>
using namespace std;

class config_t
{
  public:
    config_t(const char *filename);
    void save();
    // [display] section
    int xres;
    int yres;
    int bpp;
    int hz;
    int GAMMA;
    int viseur;

    char server_tchat[100];
    char server_xml[100];

    int msens;
    bool vsync_off;
    bool fullscreen;
    bool isdebug;
    unsigned long keys[100];
    bool music_on;
};

#endif // __config_h__
