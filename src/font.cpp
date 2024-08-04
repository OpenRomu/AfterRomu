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
 * font
 */
#include "stdafx.h"
#include "exception.h"
#include "font.h"
#include "image.h"

Font::Font(const char* filename)
    : m_tex(0)
    , taille(16)
{
    Image img;
    img.load(filename);
    m_tex = new Texture(img.data(), img.width(), img.height(), GL_ALPHA);
    m_coords.resize(256);
    float inc = 1.0f / 16.0f; // 0.0625f;
    int c = 0;
    for (float y = 1 - inc; y >= 0; y -= inc) {
        for (float x = 0; x < 1.0f; x += inc) {
            m_coords[c] = new Coords;
            if (!m_coords[0])
                throw out_of_memory();
            m_coords[c]->c00[0] = x;
            m_coords[c]->c00[1] = y;
            m_coords[c]->c10[0] = x + inc;
            m_coords[c]->c10[1] = y;
            m_coords[c]->c11[0] = x + inc, m_coords[c]->c11[1] = y + inc;
            m_coords[c]->c01[0] = x;
            m_coords[c]->c01[1] = y + inc;
            c++;
        }
    }
}

Font::~Font()
{
    if (m_tex) {
        m_tex->destroy();
        delete m_tex;
        m_tex = 0;
    }

    for (int c = 0; c < m_coords.size(); c++) {
        if (m_coords[c]) {
            delete m_coords[c];
        }
    }
}

void Font::print(int x, int y, const char* s)
{

    glEnable(GL_TEXTURE_2D);
    m_tex->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);
    char c;
    while (c = *s++) {
        if (c >= 0 && c <= 256) // tres curieux dans certain cas on sort du range
        {
            Coords* z = m_coords[(int)c];
            glTexCoord2fv(z->c00);
            glVertex2s(x, y);
            glTexCoord2fv(z->c10);
            glVertex2s(x + taille, y);
            glTexCoord2fv(z->c11);
            glVertex2s(x + taille, y + taille);
            glTexCoord2fv(z->c01);
            glVertex2s(x, y + taille);
        } else {

            int y = 0;
        }

        x += taille;
    }
    glEnd();
}
