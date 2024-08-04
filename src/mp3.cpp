// mp3.cpp: implementation of the Cmp3 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <malloc.h>
#include "glwin.h"

#include "mp3.h"
#define SAFE_RELEASE(p)                                                                                                \
    {                                                                                                                  \
        if (p) {                                                                                                       \
            (p)->Release();                                                                                            \
            (p) = NULL;                                                                                                \
        }                                                                                                              \
    }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cmp3::Cmp3(IGraphBuilder* builder)
    : g_iNumFiles(3)
    , g_iNextFile(0)
    , g_pGraphBuilder(NULL)
    , g_pMediaControl(NULL)
    , g_pMediaSeeking(NULL)
    , g_pSourceCurrent(NULL)
    , g_pSourceNext(NULL)
    , pEvent(NULL)
{
    // Initialize COM
    g_pGraphBuilder = builder;

    g_iNextFile = 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return NULL; }
/*
LRESULT Cmp3::OnGraphNotify(WPARAM wParam, LPARAM lParam)
{

}
*/
HRESULT
Cmp3::init(HWND hinstance)
{
    HRESULT hr = S_FALSE;

    // Get the IMediaControl Interface
    if (FAILED(g_pGraphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&g_pMediaControl))))
        return hr;

    // Get the IMediaControl Interface
    if (FAILED(g_pGraphBuilder->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&g_pMediaSeeking))))
        return hr;

    if (FAILED(g_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&pEvent)))
        return hr;

    pEvent->SetNotifyWindow((OAHWND)hinstance, WM_GRAPHEVENT, 0);

    pEvent->SetNotifyFlags(0); // turn on notifications

    // Create Source Filter for first file
    // Create the intial graph

    return S_OK;
}
void Cmp3::HandleEvent(void)
{
    long evCode, param1, param2;
    HRESULT hr;
    while (hr = pEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr)) {
        hr = pEvent->FreeEventParams(evCode, param1, param2);
        if ((EC_COMPLETE == evCode)) {
            // stop();
            start();
            break;
        }
    }
}

HRESULT Cmp3::start()
{
    HRESULT hr = g_pMediaControl->Stop();

    LONGLONG llPos = 0;
    hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, &llPos, AM_SEEKING_NoPositioning);

    return g_pMediaControl->Run();
}
HRESULT Cmp3::stop()
{
    HRESULT hr = g_pMediaControl->Stop();

    return hr;
}

HRESULT
Cmp3::load(char* mp3_file)
{

    LPCTSTR pstrFiles;

    HRESULT hr = S_OK;
    IPin* pPin = NULL;
    TCHAR szFilename[MAX_PATH];
    WCHAR wFileName[MAX_PATH];

    // Determine the file to load based on DirectX Media path (from SDK)
    // strcpy( szFilename, DXUtil_GetDXSDKMediaPath() );
    //    _tcscpy( szFilename, pstrFiles[1]);
    //  _tcscpy( g_szCurrentFile, pstrFiles[1]);

    wsprintf(szFilename, TEXT("data\\mp3\\%s.mp3"), mp3_file);
    wsprintf(g_szCurrentFile, TEXT("data\\mp3\\%s.mp3"), mp3_file);
    // wsprintf(szFilename,TEXT("data\\sound\\dk_EATSHT.wav"));

    DWORD dwAttr = GetFileAttributes(szFilename); //
    if (dwAttr == (DWORD)-1)
        return ERROR_FILE_NOT_FOUND;

#ifndef UNICODE
    MultiByteToWideChar(CP_ACP, 0, szFilename, -1, wFileName, MAX_PATH);
#else
    lstrcpy(wFileName, szFilename);
#endif

    // OPTIMIZATION OPPORTUNITY
    // This will open the file, which is expensive. To optimize, this
    // should be done earlier, ideally as soon as we knew this was the
    // next file to ensure that the file load doesn't add to the
    // filter swapping time & cause a hiccup.
    //
    // Add the new source filter to the graph. (Graph can still be running)
    hr = g_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &g_pSourceNext);

    // Get the first output pin of the new source filter. Audio sources
    // typically have only one output pin, so for most audio cases finding
    // any output pin is sufficient.
    if (SUCCEEDED(hr)) {
        hr = g_pSourceNext->FindPin(L"Output", &pPin);
    }

    // Stop the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Stop();
    }

    // Break all connections on the filters. You can do this by adding
    // and removing each filter in the graph
    if (SUCCEEDED(hr)) {
        IEnumFilters* pFilterEnum = NULL;

        if (SUCCEEDED(hr = g_pGraphBuilder->EnumFilters(&pFilterEnum))) {
            int iFiltCount = 0;
            int iPos = 0;

            // Need to know how many filters. If we add/remove filters during the
            // enumeration we'll invalidate the enumerator
            while (S_OK == pFilterEnum->Skip(1)) {
                iFiltCount++;
            }

            // Allocate space, then pull out all of the
            IBaseFilter** ppFilters = reinterpret_cast<IBaseFilter**>(_alloca(sizeof(IBaseFilter*) * iFiltCount));
            pFilterEnum->Reset();

            while (S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL))
                ;
            SAFE_RELEASE(pFilterEnum);

            for (iPos = 0; iPos < iFiltCount; iPos++) {
                g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
                // Put the filter back, unless it is the old source
                if (ppFilters[iPos] != g_pSourceCurrent) {
                    g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
                }
                SAFE_RELEASE(ppFilters[iPos]);
            }
        }
    }

    // We have the new ouput pin. Render it
    if (SUCCEEDED(hr)) {
        hr = g_pGraphBuilder->Render(pPin);
        g_pSourceCurrent = g_pSourceNext;
        g_pSourceNext = NULL;
    }

    SAFE_RELEASE(pPin);
    SAFE_RELEASE(g_pSourceNext); // In case of errors

    // Re-seek the graph to the beginning
    if (SUCCEEDED(hr)) {
        LONGLONG llPos = 0;
        hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, &llPos, AM_SEEKING_NoPositioning);
    }

    // Start the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Run();
    }

    // Release the old source filter.
    SAFE_RELEASE(g_pSourceCurrent);
    return S_OK;
}

Cmp3::~Cmp3()
{
    if (g_pMediaControl)
        g_pMediaControl->Stop();

    pEvent->SetNotifyWindow(NULL, 0, 0);

    // Release all remaining pointers
    SAFE_RELEASE(pEvent);
    pEvent = NULL;

    SAFE_RELEASE(g_pSourceNext);
    SAFE_RELEASE(g_pSourceCurrent);
    SAFE_RELEASE(g_pMediaSeeking);
    SAFE_RELEASE(g_pMediaControl);
}
