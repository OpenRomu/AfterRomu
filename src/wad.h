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
 * wad file
 */

#ifndef __wad_h__
#define __wad_h__

#include "texture.h"

#include <stdio.h>
#include <vector>
using namespace std;

typedef struct
{
    char        identification[4];        // should be WAD2 or 2DAW
    int            numlumps;
    int            infotableofs;
} wadinfo_t;


typedef struct
{
    int            filepos;
    int            disksize;
    int            size;                    // uncompressed
    char        type;
    char        compression;
    char        pad1, pad2;
    char        name[16];                // must be null terminated
} lumpinfo_t;

class wad_file_t {
public:
    wad_file_t(const char* filename);
    ~wad_file_t();

    void get(const char* name, rgb_image_t* r);

    const char* name() const { return _name.c_str(); }

private:
    string _name;
    FILE*    file;
    int lump_count; 
    lumpinfo_t* lumps;
};

class wad_man_t {
public:
    wad_man_t() {}
    ~wad_man_t() { 
        for(int i = 0; i < wads.size(); i++) {
            if(wads[i]) delete wads[i];
        }
    }

    void add(const char* wad) {
        wads.push_back(new wad_file_t(wad));
    }

    void get(const char* file, rgb_image_t* r) {
        for(int i = 0; i < wads.size(); i++) {
            wads[i]->get(file,r);
            if(r->data) break;
        }
    }

private:
    vector<wad_file_t*> wads;
};

#endif // __wad_h__
