#include "stdafx.h"
#include "exception.h"
#include "Pings.h"
#include "image.h"
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "fastmath.h"
Pings::Pings(IDirectPlay8Peer* dp)
    : tps(0.0f)
    , visible(false)

{
    m_dp = dp;

    m_font = new Font("data/env/font.tga");
    Image img, img2, img3, img4, img5, img6;
    img.load("data/env/pings.bmp");
    m_tex[0] = new Texture(img.data(), img.width(), img.height(), GL_RGBA);
    W[0] = img.width();
    H[0] = img.height();

    float inc = 1.0f;

    m_coords.resize(1);
    m_coords[0] = new Coords;
    m_coords[0]->c00[0] = 0;
    m_coords[0]->c00[1] = 0;
    m_coords[0]->c10[0] = 0 + inc;
    m_coords[0]->c10[1] = 0;
    m_coords[0]->c11[0] = 0 + inc, m_coords[0]->c11[1] = 0 + inc;
    m_coords[0]->c01[0] = 0;
    m_coords[0]->c01[1] = 0 + inc;
}

Pings::~Pings()
{

    if (m_tex[0]) {
        m_tex[0]->destroy();
        delete m_tex[0];
        m_tex[0] = 0;
    }

    for (int c = 0; c < m_coords.size(); c++) {
        if (m_coords[c]) {
            delete m_coords[c];
        }
    }
    if (m_font) {
        delete (m_font);
        m_font = 0;
    }
}

void Pings::frame(float delta)
{
    if (tps < 10.0f) {
        tps = tps + delta;
    }
}

void Pings::print(int m_width, int m_height, vector<Aplayer*>* joueur_list, int nb_gign, int nb_terro, bool CFT_ON,
    bool TEAM_ON, int CFT_eta, long CFT_count, long cft_timing, int FRIENDLY, bool SNIPER_ON)
{
    if ((CFT_eta != 1 && tps < 5.0f) || CFT_eta == 1) {
        int i;

        glDisable(GL_DEPTH_FUNC);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.5f, 1.0f, 1.0f, 0.5f);
        glDepthMask(0);
        glEnable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);

        GLfloat ratio = m_height / H[0];
        GLfloat WW = m_width;
        GLfloat HH = m_height;
        int id;

        i = 0;
        int offx = 50;
        int offy = 100;

        m_tex[0]->bind();
        id = 0;
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(50, HH / 5, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(WW - 50, HH / 5, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(WW - 50, HH / 1.1, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(50, HH / 1.1, 0.0f); // Top Left
        glEnd();

        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glColor4f(0.0f, 1.0f, 1.0f, 1.0f );//

        char str_friendly[100];
        sprintf(str_friendly, "DEGATS NORMAUX");

        switch (FRIENDLY) {
        case 1:
            sprintf(str_friendly, "DEGATS enemmi");
            break;
        case 2:
            sprintf(str_friendly, "DEGATS self");
            break;
        }

        char str[100];
        // m_font->print(offx+16*11+0*250+16*3 ,2*m_height/3  ,"TERRO");
        sprintf(str, "Mode NORMAL");
        if (CFT_ON)
            sprintf(str, "Mode capture de drapeau");
        if (TEAM_ON)
            sprintf(str, "Mode equipe");
        if (SNIPER_ON)
            sprintf(str, "Mode SNIPER");

        m_font->print(offx + 12 * 16, m_height + 16 - 2 * m_height / 3, str);
        m_font->print(offx + 12 * 16, m_height - 2 * m_height / 3, str_friendly);
        char tmp[100];

        if (CFT_ON || TEAM_ON) {
            sprintf(tmp, "%d secondes", (cft_timing / 1000) - (CFT_count / 1000));
            m_font->print(offx + 15 * 16, m_height + 2 * 16 - 2 * m_height / 3, tmp);
        }
        if (CFT_ON) {
            sprintf(tmp, "F3 pour changer de Team");
            m_font->print(offx + 15 * 16, m_height + 3 * 16 - 2 * m_height / 3, tmp);
        }
        int score;

        for (int ekip = 0; ekip < 2; ekip++) {

            int level = 0;
            score = 0;
            for (int c = 0; c < joueur_list->size(); c++) {
                char name[100];
                char lagg[100];
                char point[10];
                DWORD ms = get_lagg(joueur_list->at(c)->ID);
                char eez[30];
                sprintf(eez, "%s", joueur_list->at(c)->playername);
                eez[9] = '\0';
                sprintf(name, "%d-%s", c + 1, eez);
                sprintf(lagg, "%d", ms);
                sprintf(point, "%d", joueur_list->at(c)->score);

                if (joueur_list->at(c)->QuelTeam == ekip) {
                    level++;
                    if (joueur_list->at(c)->mort)
                        glColor4f(0.4f, 0.4f, 0.4f, 1.0f); //
                    else
                        glColor4f(0.7f, 0.8f, 0.7f, 1.0f); //
                    m_font->print(offx + ekip * 300, 2 * m_height / 3 - level * 16, name);
                    glColor4f(1.0f, 0.9f, 0.5f, 1.0f);
                    m_font->print(offx + 16 * 11 + ekip * 300, 2 * m_height / 3 - level * 16, point);
                    glColor4f(0.5f, 0.5f, 0.5f, 0.7f);
                    m_font->print(offx + 16 * 14 + ekip * 300, 2 * m_height / 3 - level * 16, lagg);
                    score = score + joueur_list->at(c)->score;

                    //	m_font->print(offx+16*15+ekip*250 ,2*m_height/3-level*16  ,"|1");
                }
            }
            glColor4f(0.7f, 0.7f, 0.4f, 1.0f); //

            sprintf(str, "Equipe %d", ekip + 1);
            m_font->print(offx + ekip * 250 + 16 * 3, 2 * m_height / 3 + 16, str);

            if (0 == ekip)
                sprintf(str, "%d pts", nb_gign);
            else
                sprintf(str, "%d pts", nb_terro);

            m_font->print(offx + ekip * 250 + 16 * 3, 2 * m_height / 3, str);
        }
        // glRasterPos2i(15+(-strlen(players)*5)+XX-H[2]/2+H[2]/4,-5+YY-W[2]/2);
        // for (unsigned int tt=0;tt<strlen(players);tt++)
        // glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*(players+tt));

        glDepthMask(1);
        // glDisable(GL_BLEND);
        glEnable(GL_DEPTH_FUNC);
    }
}

DWORD Pings::get_lagg(DPNID id)
{
    DWORD ms;
    DPN_CONNECTION_INFO dpnConnectionInfo;

    // set the correct size
    ZeroMemory(&dpnConnectionInfo, sizeof(DPN_CONNECTION_INFO));
    dpnConnectionInfo.dwSize = sizeof(DPN_CONNECTION_INFO);

    // call GetConnectionInfo off of your interface

    HRESULT hr = m_dp->GetConnectionInfo(id, &dpnConnectionInfo, 0);

    ms = dpnConnectionInfo.dwRoundTripLatencyMS;
    return ms;
}