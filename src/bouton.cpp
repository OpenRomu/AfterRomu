// bouton.cpp: implementation of the bouton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bouton.h"
#include "image.h"
#include "math.h"
#include "fastmath.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bouton::bouton()
    : tps_vie(0.0f)
    , enabled(true)
    , visible(false)
    , m_mousemove(false)
    , m_mouseclick(false)
    , m_release(0)
    , m_tex(NULL)
    , m_coords(NULL)
    , m_font(NULL)
{

    strcpy(file, "");
    strcpy(text, "");
    X = 0;
    Y = 0;
}

bouton::~bouton()
{

    if (m_tex)
        delete m_tex;

    if (m_coords)
        delete m_coords;
}

void bouton::SetFont(Font* in_font) { m_font = in_font; }

void bouton::SetXY(int in_X, int in_Y)
{
    X = in_X;
    Y = in_Y;
}

void bouton::SetText(char* in_text) { strcpy(text, in_text); }

void bouton::SetTexture(char* in_texture)
{
    strcpy(text, in_texture);
    Image img;
    char t[100];
    sprintf(file, "%s", in_texture);
    sprintf(t, "data/env/btn/%s", in_texture);
    img.load(t);
    m_tex = new Texture(img.data(), img.width(), img.height(), GL_RGBA);
    W = img.width();
    H = img.height();
    m_coords = new Coords;
    m_coords->c00[0] = 0;
    m_coords->c00[1] = 0;
    m_coords->c10[0] = 0 + 1;
    m_coords->c10[1] = 0;
    m_coords->c11[0] = 0 + 1, m_coords->c11[1] = 0 + 1;
    m_coords->c01[0] = 0;
    m_coords->c01[1] = 0 + 1;
}

void bouton::SetMap(char* in_texture)
{
    if (m_tex)
        delete (m_tex);
    if (m_coords)
        delete m_coords;
    m_tex = NULL;
    m_coords = NULL;

    if (strcmp(in_texture, "")) {
        strcpy(file, in_texture);
        strcpy(text, in_texture);
        Image img;
        char t[100];
        sprintf(t, "data/map/%s", in_texture);
        img.load(t);
        m_tex = new Texture(img.data(), img.width(), img.height(), GL_RGB);
        W = img.width();
        H = img.height() + 20;
        m_coords = new Coords;
        m_coords->c00[0] = 0;
        m_coords->c00[1] = 0;
        m_coords->c10[0] = 0 + 1;
        m_coords->c10[1] = 0;
        m_coords->c11[0] = 0 + 1, m_coords->c11[1] = 0 + 1;
        m_coords->c01[0] = 0;
        m_coords->c01[1] = 0 + 1;
    } else {
        sprintf(file, "");

        sprintf(text, "PAS D IMAGE");
    }
}
void bouton::frame(int mouseX, int mouseY, int m_width, int m_height, bool mousekik, bool isdebug, float delta)
{

    float taille = 1.5f;
    float scalex = (m_width / 1000.0f);
    float scaley = (m_height / 1000.0f);
    int x = X * scalex;
    int y = Y * scaley;
    int w = (W * scalex) * taille;
    int h = (H * scaley) * taille;

    if (tps_vie < 30.0f) {
        tps_vie = tps_vie + delta;
    }
    float my = (mouseY) / scaley;
    float mx = (mouseX) / scalex;
    if (visible && m_tex) {

        if (enabled && (mx > X) && (my > Y - H * taille / 2) && (mx < X + W * taille) && (my < Y + H * taille / 2)) {
            if (!m_mousemove)
                tps_vie = 0.0f;
            m_mousemove = true;

        } else
            m_mousemove = false;

        switch (m_release) {
        case 0:
            // rien
            if (visible && m_mousemove && mousekik) {
                m_release = -1;
                m_mouseclick = false;
            }
            break;
        case -1:
            // enfonce
            if (visible && m_mousemove && !mousekik) {
                m_release = 2;
                m_mouseclick = true;
            } else
                m_mouseclick = false;
            break;
        case 2: // lacher
        {
            m_mouseclick = false;
            m_release = 0;
        } break;
        }

        float ratioc = 0.0f;

        if (tps_vie < 7.0f) {
            ratioc = 1.0f + (fsin(tps_vie * -10.0f) * exp(1.0f - tps_vie)) / 80.0f; // de 4-10
            // ratio= exp(1.0f-tps_players);//de 4-10
        } else {
            ratioc = 1.0f;
        }

        float oofx = w - w / ratioc;
        float oofy = h - h / ratioc;

        /*
        glEnable(GL_DEPTH_FUNC);


        */
        glEnable(GL_BLEND);
        m_tex->bind();
        glColor4f(1.0f, 0.8f, 0.9f, 1.0f);
        glBegin(GL_QUADS);
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Start Drawing A Quad
        glTexCoord2fv(m_coords->c00);
        glVertex3f(x - oofx, m_height - oofx - y, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords->c10);
        glVertex3f(x + w / ratioc, m_height - y, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords->c11);
        glVertex3f(x + w / ratioc, m_height - y + h, 0.0f); // Top Right
        glTexCoord2fv(m_coords->c01);
        glVertex3f(x - oofx, m_height + oofx - y + h, 0.0f); // Top Left
        glEnd();

        if (m_mousemove)
            if (m_release != -1)
                glColor4f(1.0f, 1.0f, 0.0f, 0.9f);
            else
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        else
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords->c00);
        glVertex3f(x, m_height - y, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords->c10);
        glVertex3f(x + w, m_height - y, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords->c11);
        glVertex3f(x + w, m_height - y + h, 0.0f); // Top Right
        glTexCoord2fv(m_coords->c01);
        glVertex3f(x, m_height - y + h, 0.0f); // Top Left
        glEnd();

        if (m_font && isdebug) {
            //	glColor4f(1.0f,1.0f,1.0f,1.0f);
            m_font->print(x, m_height - y - 20, text);
        }
        // glColor4f(1.0f,0.5f,0.5f,0.8f);
        //  glDepthMask(1);
    }
}

void bouton::GlQuad(float x1, float y1, float x2, float y2)
{

    // glBlendFunc( GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.3f, 0.3f, 1.0f);

    glBegin(GL_QUADS);
    glVertex3f(x1, y1, 0.0f);
    glVertex3f(x2, y1, 0.0f);
    glVertex3f(x2, y2, 0.0f);
    glVertex3f(x1, y2, 0.0f);
    glEnd();

    glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(x1 + 5, y1 + 5, 0.0f);
    glVertex3f(x2 - 5, y1 + 5, 0.0f);
    glVertex3f(x2 - 5, y2 - 5, 0.0f);
    glVertex3f(x1 + 5, y2 - 5, 0.0f);
    glEnd();
}
