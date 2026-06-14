// chat.cpp: implementation of the Cchat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chat.h"
#include "exception.h"
#include "romu.h"
#include "image.h"
#include "math.h"
#include "fastmath.h"
#include <string>
// #include "glut.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cchat::Cchat(float x, float y, float buffer) : tps(0), TAB(NULL), tps_phrase(0.0f)
{

    phrase[0] = '\0';
    m_font = new Font("data/env/font.tga");
    // zero_dwords((void*) &TAB[0], TAB.size());
    TAB.clear();
    m_font->taille = 12;

    pos_x = x;
    pos_y = y;
    nb_buffer = buffer;
}

Cchat::~Cchat()
{

    if (m_font)
    {
        delete (m_font);
        m_font = 0;
    }

    int o = TAB.size();

    for (int i = 0; i < o; i++)
        TAB.erase(TAB.begin());

    TAB.clear();
}

void Cchat::addtext(char *txt, int ekip) // 2 neutre 0 moi 1 eux
{
    string toto(txt);
    toto = toto.substr(0, 60);

    msg_chat lemsg;
    lemsg.msg = toto;
    lemsg.ekip = ekip;

    lemsg.t0 = 0.0f;

    if (TAB.size() > 0)
    {
        if (TAB[TAB.size() - 1].t0 < 1.0f)
        {
            for (int i = 0; i != TAB.size(); i++)
            {

                TAB[i].t0 = TAB[i].t0 + 1.0f;
            }
        }
    }

    TAB.push_back(lemsg);
    if (TAB.size() > 20)
    {
        TAB.erase(TAB.begin());
    }
}

void Cchat::draw(float fps, float height, float width)
{
    float temps = 5.0f;

    // glEnable(GL_DEPTH_FUNC);
    //	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glDepthMask(0);

    glEnable(GL_BLEND);

    tps = tps + fps * 1.0f;

    if (TAB.size() > 0)
    {
        if (TAB[0].t0 > nb_buffer)
        {
            TAB.erase(TAB.begin());
        }
    }

    for (int i = 0; i < TAB.size(); i++)
    {
        float offset = pos_y; // 120.0f
        float t = height - offset + (TAB[i].t0) * 10.0f;
        TAB[i].t0 = TAB[i].t0 + fps;

        float alpha;

        if (t > (height - 60))
        {
            alpha = (1.0f - (t - (height - 60)) / 30.0f);
        }
        else
        {
            alpha = 1.0f;
        }
        if (TAB[i].ekip == 1)
            glColor4f(1.0f, 0.f, 0.2f, 1.0f);
        else if (TAB[i].ekip == 0)
            glColor4f(0.0f, 1.0f, 0.2f, 1.0f);
        else
            glColor4f(1.0f, 1.f, 1.0f, 1.0f);

        m_font->print(pos_x, t, TAB[i].msg.c_str());

        /*		char c[200];
                strcpy(c,TAB[i].c_str());


                glRasterPos2i(10,height-50.0f-(4.0f*temps)*i+(tps*4.0f));
                for (unsigned int h=0;h<strlen(c);h++)
                    glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*(c+h));
        */
    }

    if (tps_phrase > 0.0f)
    {
        tps_phrase = tps_phrase - (fps * 20.0f);
        float cap = 40.0f;
        float amis;
        if (ekip == true)
        {
            amis = 1.0f;
        }
        else
        {
            amis = 0.0f;
        }

        if (tps_phrase > cap)
        {
            glColor4f(1.0f - amis, 1.0f - (1.0f - amis), 0.1f, 1.0f);
            m_font->print(width - 400.0f, height - 50.0f, phrase);
        }
        else
        {
            if (tps_phrase > 5.0f)
            {

                glColor4f(1.0f - amis, 1.0f - (1.0f - amis), 0.1f, (1.0f - (cap - tps_phrase) / cap));
                m_font->print(width - 400.0f, height - 50.0f, phrase);
            }
        }
    }

    glDisable(GL_BLEND);
}
