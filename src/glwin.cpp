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
#include "stdafx.h"
#include "glwin.h"
#include "exception.h"
#include "Resource.h"

#define WM_MOUSEWHEEL 0x020A
/////////////////////////////////////////////////////////////////

void GLWindow::GLInfo::init()
{
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *renderer = (const char *)glGetString(GL_RENDERER);

    const char *error = "error";
    m_vendor = vendor ? vendor : error;
    m_version = version ? version : error;
    m_renderer = renderer ? renderer : error;

    const char *ext = (const char *)glGetString(GL_EXTENSIONS);
    if (ext)
    {
        // parse extensions
        char *s = new char[strlen(ext) + 1];
        if (!s)
            throw out_of_memory();
        strcpy(s, ext);
        char seps[] = " ";
        char *token = strtok(s, seps);
        while (token)
        {
            m_extensions.push_back(token);
            token = strtok(0, seps);
        }
        delete s;
    }
}

///////////////////////////////////////////////////////////////////

const char *GLWindow::className = "GL Engine";
GLWindow *GLWindow::m_instance = 0;
GLWindow::GLInfo GLWindow::gl_info;
vector<DEVMODE> GLWindow::m_modes;

void GLWindow::create(int width, int height, int bpp, int hz, bool fullscreen)
{

    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL); // hInstance;
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ROMUSTRIKE));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    /*
        WNDCLASSEX wc = { sizeof(WNDCLASSEX),
                        CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                          WindowProc,
                          0L,
                          0L,
                          GetModuleHandle(NULL),
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          className,
                          NULL };

    */
    RegisterClassEx(&wc);
    m_hinst = wc.hInstance;

    // if window exists, kill it
    if (m_instance)
    {
        m_instance->destroy();
        m_instance = 0;
    }
    // go fullscreen in requested
    if (fullscreen)
    {
        DEVMODE dm;
        memset(&dm, 0, sizeof(dm));
        dm.dmSize = sizeof(dm);
        dm.dmPelsWidth = width;
        dm.dmPelsHeight = height;
        dm.dmBitsPerPel = bpp;
        dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (hz)
        {
            dm.dmDisplayFrequency = hz;
            dm.dmFields |= DM_DISPLAYFREQUENCY;
        }

        if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            throw basic_exception("fullscreen failed");
        }
    }

    // Create a window
    DWORD style;
    DWORD exStyle;

    if (fullscreen)
    {
        exStyle = WS_EX_TOPMOST | WS_EX_APPWINDOW;
        style = WS_POPUP;
    }
    else
    {
        exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        style = WS_OVERLAPPEDWINDOW;
    }

    ShowCursor(FALSE);

    m_hwnd = CreateWindowEx(exStyle, className, "ROMUSTRIKE\0", style, 0, 0, width, height, 0, 0, wc.hInstance, 0);

    if (!m_hwnd)
    {
        throw basic_exception("CreateWindowEx() failed");
    }

    m_hdc = GetDC(m_hwnd);
    if (!m_hdc)
    {
        throw basic_exception("GetDC() failed");
    }

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                       // Version Number
        PFD_DRAW_TO_WINDOW |     // Format Must Support Window
            PFD_SUPPORT_OPENGL | // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,    // Must Support Double Buffering
        PFD_TYPE_RGBA,           // Request An RGBA Format
        bpp,                     // bpp,                                // Select Our Color Depth
        0,
        0,
        0,
        0,
        0,
        0, // Color Bits Ignored
        0, // No Alpha Buffer
        0, // Shift Bit Ignored
        0, // No Accumulation Buffer
        0,
        0,
        0,
        0,              // Accumulation Bits Ignored
        24,             // Z-Buffer (Depth Buffer) bits
        0,              // No Stencil Buffer
        0,              // No Auxiliary Buffer
        PFD_MAIN_PLANE, // Main Drawing Layer
        0,              // Reserved
        0,
        0,
        0 // Layer Masks Ignored
    };

    GLuint pixFmt = 0;

    if (!(pixFmt = ChoosePixelFormat(m_hdc, &pfd)))
    {
        throw basic_exception("ChoosePixelFormat() failed");
    }

    if (!SetPixelFormat(m_hdc, pixFmt, &pfd))
    {
        throw basic_exception("SetPixelFormat() failed");
    }

    if (!(m_hrc = wglCreateContext(m_hdc)))
    {
        throw basic_exception("wglCreateContext() failed");
    }

    if (!wglMakeCurrent(m_hdc, m_hrc))
    {
        throw basic_exception("wglMakeCurrent() failed");
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    SetForegroundWindow(m_hwnd);

    SetFocus(m_hwnd);

    m_instance = this;
    m_width = width;
    m_height = height;
    m_bpp = bpp;
    m_fullscreen = fullscreen;

    enum_display();
    char toto[200];

    for (int y = 0; y < m_modes.size(); y++)
    {
        sprintf(toto, "bpp %i,height %i, width %i", m_modes[y].dmBitsPerPel, m_modes[y].dmPelsHeight,
                m_modes[y].dmPelsWidth);
    }
    gl_info.init();

    resize();

    init();
}

void GLWindow::destroy()
{
    if (m_fullscreen)
    {
        ChangeDisplaySettings(0, 0);
        ShowCursor(TRUE);
    }

    if (m_hrc)
    {
        if (!wglMakeCurrent(0, 0))
        {
        }

        if (!wglDeleteContext(m_hrc))
        {
        }

        m_hrc = 0;
    }

    if (m_hdc)
    {
        if (!ReleaseDC(m_hwnd, m_hdc))
        {
        }
    }

    if (m_hwnd)
    {
        if (!DestroyWindow(m_hwnd))
        {
        }
    }

    UnregisterClass(className, m_hinst);
    m_hinst = 0;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_fullscreen = false;
    m_active = false;
}

LRESULT
GLWindow::window_proc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        if (!HIWORD(wparam))
        {
            m_active = true;
        }
        else
        {
            m_active = false;
        }

        return 0;

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (wparam == SIZE_MINIMIZED)
        {
            m_active = false;
            return 0;
        }
        else if (wparam == SIZE_RESTORED)
        {
            m_active = true;
        }
        m_width = LOWORD(lparam);
        m_height = HIWORD(lparam);
        resize();
        return 0;

    case WM_KEYDOWN:
        m_input.keys[wparam] = true;
        if (wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        return 0;

    case WM_KEYUP:
        m_input.keys[wparam] = false;
        return 0;

    case WM_MOUSEMOVE: {
        POINT p;
        GetCursorPos(&p);
        if (!m_fullscreen)
        {
            ScreenToClient(m_hwnd, &p);
        }
        m_input.xpos = (float)p.x;
        m_input.ypos = (float)p.y;
    }
        // m_input.xpos = GET_X_LPARAM(lparam);
        // m_input.ypos = GET_Y_LPARAM(lparam);
        return 0;
    case WM_LBUTTONDOWN:
        m_input.left_button = true;
        return 0;
    case WM_LBUTTONUP:
        m_input.left_button = false;
        return 0;
    case WM_MBUTTONDOWN:
        m_input.middle_button = true;
        return 0;
    case WM_MBUTTONUP:
        m_input.middle_button = false;
        return 0;

    case WM_RBUTTONDOWN:
        m_input.right_button = true;
        return 0;
    case WM_RBUTTONUP:
        m_input.right_button = false;
        return 0;
    case WM_MOUSEWHEEL:
        m_input.wheel = (short)HIWORD(wparam);
        m_input.is_wheeling = true;
        return 0;
    case WM_GRAPHEVENT:
        HandleEvent(); /// bat mp3 loops
        return 0;
    }

    return DefWindowProc(m_hwnd, msg, wparam, lparam);
}

void GLWindow::enum_display()
{
    if (m_modes.size())
    {
        return;
    }

    int c = 0;
    DEVMODE mode;
    while (EnumDisplaySettings(0, c++, &mode))
    {
        if (mode.dmBitsPerPel >= 16 && // only 16bpp or more
            mode.dmPelsWidth >= 640 && // at least 640x...
            mode.dmDisplayFlags == 0)  // color and noninterlaced
        {
            m_modes.push_back(mode);
        }
    }
}

///////////////////////////////////////////
// extensions

////////////////////////////////////////////////////////////
// multi texture

PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = 0;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB = 0;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = 0;

bool GLWindow::enable_multitexture()
{
    if (!gl_info.has_extension("GL_ARB_multitexture"))
    {
        return false;
    }

    glActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fvARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");

    // excrements hit the air conditioning
    if (!glActiveTextureARB || !glMultiTexCoord2fvARB || !glClientActiveTextureARB)
    {
        return false;
    }

    return true;
}
