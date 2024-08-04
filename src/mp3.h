// mp3.h: interface for the Cmp3 class.
//
//////////////////////////////////////////////////////////////////////

#define WM_GRAPHNOTIFY WM_APP + 1
#include <windows.h>
#include <initguid.h>
#include <Dshow.h>

class Cmp3 {
public:
    Cmp3(IGraphBuilder* builder);
    virtual ~Cmp3();

    HRESULT init(HWND hinstance);
    // LRESULT OnGraphNotify(WPARAM wParam, LPARAM lParam);
    HRESULT load(char* mp3_file);
    HRESULT start();
    HRESULT stop();
    void HandleEvent(void);

private:
    IGraphBuilder* g_pGraphBuilder;
    IMediaControl* g_pMediaControl;
    IMediaSeeking* g_pMediaSeeking;
    IBaseFilter* g_pSourceCurrent;
    IBaseFilter* g_pSourceNext;
    IMediaEventEx* pEvent;

    TCHAR g_szCurrentFile[128];
    int g_iNumFiles;
    int g_iNextFile;
};
