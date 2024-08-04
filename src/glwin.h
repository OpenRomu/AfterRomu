/*
 * HL rendering engine
 * Copyright (c) 2000,2001 Bart Sekura
 *
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and
 * this permission notice appear in all copies of the software,
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * OpenGL window
 */

#ifndef __glwin_h__
#define __glwin_h__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

// OpenGL stuff
#include <gl/gl.h>
#include "glext.h"

// stc++
#pragma warning(disable : 4786)
#include <vector>
#include <string>
using namespace std;
#define WM_GRAPHEVENT WM_USER // define a custom window message for graph events

class GLWindow {
public:
#define MAX_KEYS (256)
    class InputStatus {
    public:
        InputStatus()
            : left_button(false)
            , right_button(false)
            , xpos(0)
            , ypos(0)
            , wheel(0)
            , is_wheeling(false)
            , middle_button(false)
        {
            for (int c = 0; c < MAX_KEYS; keys[c++] = false)
                ;
        }

        bool keys[MAX_KEYS];
        bool left_button;
        bool right_button;
        bool middle_button;
        short wheel;
        bool is_wheeling;
        int xpos;
        int ypos;
    };

    class GLInfo {
    public:
        GLInfo() { }
        void init();

        const char* vendor() const { return m_vendor.c_str(); }
        const char* version() const { return m_version.c_str(); }
        const char* renderer() const { return m_renderer.c_str(); }

        const vector<string>& extensions() const { return m_extensions; }

        bool has_extension(const char* name) const
        {
            for (int i = 0; i < m_extensions.size(); i++) {
                if (m_extensions[i] == name) {
                    return true;
                }
            }
            return false;
        }

    private:
        string m_vendor;
        string m_version;
        string m_renderer;
        vector<string> m_extensions;
    };

public:
    GLWindow()
        : m_width(0)
        , m_height(0)
        , m_bpp(0)
        , m_fullscreen(0)
        , m_hinst(0)
        , m_hwnd(0)
        , m_hdc(0)
        , m_hrc(0)
        , m_active(false)
    {
    }
    ~GLWindow()
    {
        m_instance = 0;
        destroy();
    }

    void create(int width, int height, int bpp, int hz = 0, bool fullscreen = false);
    void destroy();

    void flip() const { SwapBuffers(m_hdc); }

protected:
    // window proc
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (m_instance) {
            return m_instance->window_proc(msg, wparam, lparam);
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    LRESULT window_proc(UINT msg, WPARAM wparam, LPARAM lparam);

public:
    static const char* className;
    static void enum_display();
    static vector<DEVMODE> m_modes;
    static GLInfo gl_info;

    // extensions
    static bool enable_multitexture();

protected:
    virtual void init() { }
    virtual void resize() { }
    virtual void HandleEvent() { }

protected:
    string m_title;
    int m_width;
    int m_height;
    int m_bpp;
    bool m_fullscreen;
    bool m_active;
    InputStatus m_input;

protected:
    HINSTANCE m_hinst;
    HWND m_hwnd;
    HDC m_hdc;
    HGLRC m_hrc;

    static GLWindow* m_instance;
};

///////////////////////////////
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB0;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

#endif // __glwin_h__
