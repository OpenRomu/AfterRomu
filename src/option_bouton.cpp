#include "stdafx.h"
#include "bouton.h"
#include "console.h"

#include "option_bouton.h"

option_bouton::option_bouton(void)
{
    visible = false;

    le_texte.SetText("ARME1");
    le_texte.visible = true;

    le_btnON.SetTexture("nb_plus.bmp");
    le_btnON.visible = true;
    le_btnOFF.SetTexture("nb_moins.bmp");
    le_btnOFF.visible = false;
}

option_bouton::~option_bouton(void)
{
}

void option_bouton::SetFont(Font *in_font)
{
    le_btnON.SetFont(in_font);
    le_btnOFF.SetFont(in_font);
    le_texte.SetFont(in_font);
}

void option_bouton::SetXY(int in_X, int in_Y, int in_W, int in_H)
{
    le_btnON.SetXY(in_X, in_Y);
    le_btnOFF.SetXY(in_X, in_Y);
    le_texte.SetXY(in_X + 45, in_Y - 15, in_W, in_H);
}
void option_bouton::frame(int mouseX, int mouseY, int m_width, int m_height, bool mousekik, bool isdebug, float delta)
{
    if (visible)
    {
        le_btnON.frame(mouseX, mouseY, m_width, m_height, mousekik, isdebug, delta);
        le_btnOFF.frame(mouseX, mouseY, m_width, m_height, mousekik, isdebug, delta);

        le_texte.frame(m_width, m_height, isdebug, delta * 2.0f);
        if (le_btnON.m_mouseclick && !le_btnOFF.m_mouseclick)
        {
            le_btnON.visible = false;
            le_btnOFF.visible = true;
            le_btnON.m_mouseclick = false;
            le_texte.tps_vie = 19.5f;
        }

        if (le_btnOFF.m_mouseclick && !le_btnON.m_mouseclick)
        {
            le_btnOFF.visible = false;
            le_btnON.visible = true;
            le_btnOFF.m_mouseclick = false;
            le_texte.tps_vie = 19.5f;
        }
    }
}

void option_bouton::SetText(char *in_text)
{
    le_texte.SetText(in_text);
}
