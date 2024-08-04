
#pragma once

#include "font.h"
#include "texture.h"
#include "bouton.h"
#include "console.h"
class option_bouton {
public:
    option_bouton();
    void SetTexture();
    void draw();
    void frame(int mouseX, int mouseY, int m_width, int m_height, bool mousekik, bool isdebug, float delta);
    void SetFont(Font* in_font);
    void SetXY(int in_X, int in_Y, int in_W, int in_H);
    void SetText(char* in_text);
    bouton le_btnON;
    bouton le_btnOFF;

    console le_texte;
    int lengthc;
    ~option_bouton();
    bool visible;
};
