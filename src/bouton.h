// bouton.h: interface for the bouton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUTON_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_)
#define AFX_BOUTON_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "font.h"
#include "texture.h"

class bouton {
public:
    class Coords {
    public:
        float c00[2];
        float c10[2];
        float c11[2];
        float c01[2];
    };
    bouton();
    void GlQuad(float x1, float y1, float x2, float y2);
    void SetTexture();
    void draw();
    void frame(int mouseX, int mouseY, int m_width, int m_height, bool mousekik, bool isdebug, float delta);
    void SetFont(Font* in_font);
    void SetXY(int in_X, int in_Y);
    void SetText(char* in_text);
    void SetTexture(char* in_texture);
    void SetMap(char* in_texture);

    ~bouton();
    char text[100];
    bool m_mousemove;
    bool m_mouseclick;
    int m_release;
    bool visible;
    bool enabled;
    float tps_vie;
    char file[100];

private:
    Coords* m_coords;

    Font* m_font;
    int X;
    int Y;
    Texture* m_tex;
    int H;
    int W;
    int WW;
    int HH;
};

#endif // !defined(AFX_BOUTON_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_)
