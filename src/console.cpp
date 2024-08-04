// console.cpp: implementation of the console class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "console.h"
#include "image.h"
#include "math.h"
#include "fastmath.h"
//////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

console::console()
    : lengthc(35)
    , tps_vie(0.0f)
    , visible(false)
    , m_mousemove(false)
    , m_mouseclick(false)
    , m_release(false)
    , m_coords(NULL)
    , m_font(NULL)
{
    //	m_font = new Font("data/env/font.tga");

    strcpy(text, "");
    X = 0;
    Y = 0;
}

console::~console()
{

    if (m_coords)
        delete m_coords;
}

void console::SetFont(Font* in_font) { m_font = in_font; }

void console::SetXY(int in_X, int in_Y, int in_W, int in_H)
{
    X = in_X;
    Y = in_Y;
    HH = in_H;
    WW = in_W;
}

void console::SetText(char* in_text) { strcpy(text, in_text); }

void console::frame(int m_width, int m_height, bool isdebug, float delta)
{

    float taille = 1.5f;
    float scalex = (m_width / 1000.0f);
    float scaley = (m_height / 1000.0f);
    int x = X * scalex;
    int y = Y * scaley;
    int w = (W * scalex) * taille;
    int h = (H * scaley) * taille;
    int ww = (WW * scalex);
    int hh = (HH * scaley);

    m_font->taille = (((ww / lengthc)) + ((ww / lengthc)) % 2); // FIXME (int) conversion
    if (visible) {
        tps_vie = tps_vie + delta;
        if (tps_vie > 20.0f)
            tps_vie = 0.0f;

        glEnable(GL_DEPTH_FUNC);
        glDepthMask(0);
        glEnable(GL_BLEND);
        glColor4f(0.0f, 0.3f, 1.0f, 0.5f);

        // m_tex->bind();

        float ratioc = 0.0f;

        if (tps_vie < 7.0f) {
            ratioc = 1.0f + (fsin(tps_vie * -10.0f) * exp(1.0f - tps_vie)) / 80.0f; // de 4-10
            // ratio= exp(1.0f-tps_players);//de 4-10
        } else {
            ratioc = 1.0f;
        }

        /*glColor4f(0.5f,0.5f,0.5f,1.0f);
        glBegin(GL_QUADS);
        float oofx=w-w/ratioc;
        float oofy=h-h/ratioc;
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                                                                                // Start Drawing A Quad
        glTexCoord2fv(m_coords->c00); glVertex3f(x-oofx,m_height-y,0.0f);	// Bottom Left
        glTexCoord2fv(m_coords->c10); glVertex3f(x+w/ratioc,m_height-y,0.0f);	// Bottom Right
        glTexCoord2fv(m_coords->c11); glVertex3f(x+w/ratioc,m_height-y+h,0.0f);	// Top Right
        glTexCoord2fv(m_coords->c01); glVertex3f(x-oofx,m_height-y+h,0.0f);	// Top Left
        glEnd();

        if (m_mousemove)
        glColor4f(1.0f,0.0f,0.0f,1.0f);
        else
        glColor4f(1.0f,1.0f,1.0f,1.0f);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);

        glBegin(GL_QUADS);											// Start
        Drawing A Quad glTexCoord2fv(m_coords->c00); glVertex3f(x,m_height-y,0.0f);	// Bottom Left
        glTexCoord2fv(m_coords->c10); glVertex3f(x+w,m_height-y,0.0f);	// Bottom Right
        glTexCoord2fv(m_coords->c11); glVertex3f(x+w,m_height-y+h,0.0f);	// Top Right
        glTexCoord2fv(m_coords->c01); glVertex3f(x,m_height-y+h,0.0f);	// Top Left
        glEnd();

        */

        char matrice[30][100];
        int nb_ligne = 0;
        int pos_car = 0;
        char toto[500];
        int mxd = floor(tps_vie * 20);
        if (mxd > strlen(text))
            mxd = strlen(text);
        memcpy(&toto, &text, mxd);
        memcpy(&toto[mxd], "\0", 1);

        for (int r = 0; r < strlen(toto) + 1; r++) {
            char cur_car[1];
            memcpy(&cur_car, &toto[r], 1);
            if (cur_car[0] == 42 || cur_car[0] == 0) // *
            {
                strcpy(&matrice[nb_ligne][pos_car], "\0");
                pos_car = 0;
                nb_ligne++;
            } else {

                memcpy(&matrice[nb_ligne][pos_car], cur_car, 1);
                pos_car++;
            }
        }

        strcpy(&matrice[nb_ligne][pos_car + 1], "\0");

        char currt[100];

        if (m_font) {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            int saut = 0;

            for (int r = 0; r < nb_ligne; r++) {
                int maxs = 0;
                int lenf = 0;
                do {
                    maxs = 0;

                    if (strlen(matrice[r]) - lenf >= lengthc)

                    {
                        maxs = lengthc;
                    } else
                        maxs = (strlen(matrice[r]) - lenf) % lengthc;

                    memcpy(&currt, &matrice[r][lenf], maxs);
                    currt[lenf % lengthc + maxs] = 0;
                    char final[100];
                    sprintf(final, ">%s", currt);

                    m_font->print(x, m_height - y - m_font->taille * (r + saut), final);
                    lenf = lenf + maxs;
                    if (maxs == lengthc)
                        saut++;
                } while (maxs == lengthc);
            }
        }

        glDisable(GL_DEPTH_FUNC);
        glDepthMask(1);
        glDisable(GL_BLEND);

        // GlQuad( x, m_height-y+16,x+ww,m_height-y-hh);
    }
    glDisable(GL_BLEND);
}

void console::GlQuad(float x1, float y1, float x2, float y2)
{

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_COLOR);

    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.0f, 0.1f, 0.0f);

    glBegin(GL_QUADS);
    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y1, 0.0f);
    glVertex3f(x2, y2, 0.0f);
    glVertex3f(x1, y2, 0.0f);
    glEnd();
}
