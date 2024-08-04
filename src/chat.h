// chat.h: interface for the Cchat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAT_H__57860D0D_29C9_463F_9E96_437F5B17B4BB__INCLUDED_)
#define AFX_CHAT_H__57860D0D_29C9_463F_9E96_437F5B17B4BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "texture.h"
#include <vector>
#include "dsutil.h"
#include "font.h"
#include "bitset.h"

#include <string>

struct msg_chat {
    string msg;
    int ekip;
    float t0;
};

class Cchat {
public:
    Font* m_font;
    float tps;
    float tps_phrase;
    void draw(float fps, float height, float width);
    char phrase[100];
    bool ekip;
    vector<msg_chat> TAB;
    void addtext(char* txt, int ekip);
    Cchat(float x, float y, float buffer);
    virtual ~Cchat();
    float pos_x;
    float pos_y;
    float nb_buffer;
};

#endif // !defined(AFX_CHAT_H__57860D0D_29C9_463F_9E96_437F5B17B4BB__INCLUDED_)
