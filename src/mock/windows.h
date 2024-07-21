#pragma once
#ifndef __FAKE_WINDOWS__
#define  __FAKE_WINDOWS__
#endif

typedef struct WNDCLASSEX {

} WNDCLASSEX;


#define TRUE 1
#define FALSE 0
#define CP_ACP 1

typedef int BOOL;
typedef void VOID;
typedef char TCHAR;
typedef char CHAR;
typedef int GUID;
typedef int HRESULT;
typedef int DWORD;
typedef int BYTE;
typedef int HKEY;
typedef short WCHAR;
typedef int HWND;
typedef float FLOAT;
typedef unsigned int UINT;
typedef int DPNID;
typedef char* LPTSTR;
typedef unsigned long ULONG;
typedef unsigned long LARGE_INTEGER;
typedef unsigned long long LONGLONG;

#define ShowCursor(x)
#define SetForegroundWindow(x)
#define UpdateWindow(x)
#define SetFocus(x)
#define ChangeDisplaySettings(x)
#define UnregisterClass(x)
#define ChangeDisplaySettings(x)
#define ChoosePixelFormat(x) 1
#define timeGetTime() 1
#define unlink(a)
#define OutputDebugString(a)
#define UNREFERENCED_PARAMETER(a) (void)a
#define WideCharToMultiByte(a, b, c, d, e, f, g, h)
#define MultiByteToWideChar(a, b, c, d, e, f)
#define ZeroMemory(a, b)
#define _T(a) a
#define wcslen(a) strlen(a)

#define ReleaseDC(x, y) TRUE

#define WS_EX_APPWINDOW 0
#define WS_EX_TOPMOST 0
#define CO_E_NOTINITIALIZED 1

#define TEXT(a) a
#define strcpy_s(a, b) strcpy(a, b)