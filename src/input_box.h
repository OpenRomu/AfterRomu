// input_box.h: interface for the input_box class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_BOX_H__C747028C_F7D1_437D_8072_DF8A59B5424F__INCLUDED_)
#define AFX_INPUT_BOX_H__C747028C_F7D1_437D_8072_DF8A59B5424F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "font.h"
#include "texture.h"
#include "console.h"
#include "bouton.h"

class input_box {
public:
    input_box();
    virtual ~input_box();
    void draw();
    void SetLibelle(char* t);
    void SetText(char* t);
    void frame(int mouseX, int mouseY, int m_width, int m_height, bool mousekik, bool isdebug, float delta);
    void SetFont(Font* in_font);
    void SetXY(int in_X, int in_Y, int in_W, int in_H);
    bool visible;
    bool actif;
    console libelle;
    console text;
    bouton fond;
    bouton activ_fond;
    int max_len;
};

#endif // !defined(AFX_INPUT_BOX_H__C747028C_F7D1_437D_8072_DF8A59B5424F__INCLUDED_)
