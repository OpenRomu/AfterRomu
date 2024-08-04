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
#include "stdafx.h"
#include "wad.h"
#include "exception.h"

// #include <fstream>
using namespace std;

wad_file_t::wad_file_t(const char* filename)
    : lumps(0)
    , lump_count(0)
    , file(0)
    , _name(filename)
{
    file = fopen(filename, "rb");
    if (!file) {
        string s = "wad cannot be read:";
        s += filename;
        throw basic_exception(s.c_str());
    }

    wadinfo_t w;
    if (fread(&w, 1, sizeof(w), file) != sizeof(w)) {
        throw basic_exception("wad header");
    }

    fseek(file, w.infotableofs, SEEK_SET);
    lump_count = w.numlumps;
    lumps = new lumpinfo_t[w.numlumps];
    if (fread(lumps, sizeof(lumpinfo_t), w.numlumps, file) != w.numlumps) {
        throw basic_exception("wad lumps");
    }
}

wad_file_t::~wad_file_t()
{
    if (lumps)
        delete lumps;
    if (file)
        fclose(file);
}

#define MIPLEVELS 4
typedef struct miptex_s {
    char name[16];
    unsigned width, height;
    unsigned offsets[MIPLEVELS]; // four mip maps stored
} miptex_t;

static unsigned char* fill_rgb(unsigned char* pal, unsigned char* idx, int size)
{
    unsigned char* data = new unsigned char[size * 3];
    int c = 0;
    for (int i = 0; i < size; i++) {
        data[c++] = pal[idx[i] * 3];
        data[c++] = pal[idx[i] * 3 + 1];
        data[c++] = pal[idx[i] * 3 + 2];
    }

    return data;
}
static unsigned char* fill_rgbA(unsigned char* pal, unsigned char* idx, int size)
{
    unsigned char* data = new unsigned char[size * 4];
    int c = 0;
    for (int i = 0; i < size; i++) {
        if (pal[idx[i] * 3] == 0 && pal[idx[i] * 3 + 1] == 0 && pal[idx[i] * 3 + 2] == 255) {
            data[c++] = 0;
            data[c++] = 0;
            data[c++] = 0;
            data[c++] = 0;
        } else {
            data[c++] = pal[idx[i] * 3];
            data[c++] = pal[idx[i] * 3 + 1];
            data[c++] = pal[idx[i] * 3 + 2];
            data[c++] = 255;
        }
    }

    return data;
}
void wad_file_t::get(const char* name, rgb_image_t* r)
{
    for (int i = 0; i < lump_count; i++) {
        // o << "lump[" << i << "]" << lumps[i].name << endl;
        if (!strcmpi(lumps[i].name, name)) {
            unsigned char* tmp = new unsigned char[lumps[i].disksize];
            fseek(file, lumps[i].filepos, SEEK_SET);
            if (fread(tmp, 1, lumps[i].disksize, file) != lumps[i].disksize)
                ; // fixme raise error

            miptex_t* t = (miptex_t*)tmp;
            r->width = t->width;
            r->height = t->height;

            // palette starts after the fourth mipmap (1/8 size)
            unsigned char* pal = tmp + t->offsets[3] + ((r->width / 8) * (r->height / 8)) + 2;
            unsigned char* indices = tmp + t->offsets[0];
            int size = r->width * r->height;

            if (strcspn(name, "{") == 0) {
                r->data = fill_rgbA(pal, indices, size);
            } else {
                r->data = fill_rgb(pal, indices, size);
            }

#if _MIPMAPS
            size = (r->width / 2) * (r->height / 2);
            indices = tmp + t->offsets[1];
            r->data2 = fill_rgb(pal, indices, size);

            size = (r->width / 4) * (r->height / 4);
            indices = tmp + t->offsets[2];
            r->data4 = fill_rgb(pal, indices, size);

            size = (r->width / 8) * (r->height / 8);
            indices = tmp + t->offsets[3];
            r->data8 = fill_rgb(pal, indices, size);
#endif

            delete tmp;
            return;
        }
    }
}
