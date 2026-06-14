// console.h: interface for the console class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_console_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_)
#define AFX_console_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "font.h"
#include "texture.h"

class console
{
  public:
    class Coords
    {
      public:
        float c00[2];
        float c10[2];
        float c11[2];
        float c01[2];
    };
    console();
    void GlQuad(float x1, float y1, float x2, float y2);
    void SetTexture();
    void draw();
    void frame(int m_width, int m_height, bool isdebug, float delta);
    void SetFont(Font *in_font);
    void SetXY(int in_X, int in_Y, int in_W, int in_H);
    void SetText(char *in_text);

    int lengthc;
    ~console();
    char text[500];
    bool m_mousemove;
    bool m_mouseclick;
    bool m_release;
    bool visible;
    float tps_vie;

  private:
    Coords *m_coords;

    Font *m_font;
    int X;
    int Y;
    // Texture* m_tex ;
    int H;
    int W;
    int WW;
    int HH;
};

#endif // !defined(AFX_console_H__0BACE443_B7B8_4AA4_8872_2B67A7643A6C__INCLUDED_)
