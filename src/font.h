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
 * font (duh)
 */

#ifndef __font_h__
#define __font_h__

#include "texture.h"
#include <vector>
using namespace std;

class Font {
public:
	int taille;
   
    class Coords {
    public:
        float   c00[2];
        float   c10[2];
        float   c11[2];
        float   c01[2];
    };
public:
    Font(const char* filename);
    ~Font();

   void print(int x, int y, const char* s);

private:
    Texture* m_tex;
    vector<Coords*> m_coords;
};

#endif // __font_h__
