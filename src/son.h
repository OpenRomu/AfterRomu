#ifndef __SONH__
#define __SONH__

#include <windows.h>
#include <dxerr8.h>
#include "dsutil.h"
#include "DXUtil.h"

#include <dsound.h>
#include <mmsystem.h>

//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSon
{
  protected:
    LPDIRECTSOUNDBUFFER *m_apDSBuffer;
    DWORD m_dwDSBufferSize;
    CWaveFile *m_pWaveFile;
    DWORD m_dwNumBuffers;

    HRESULT RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL *pbWasRestored);

  public:
    CSon();
    void LoadSon(LPDIRECTSOUNDBUFFER *apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile *pWaveFile);

    virtual ~CSon();

    HRESULT Get3DBufferInterface(DWORD dwIndex, LPDIRECTSOUND3DBUFFER *ppDS3DBuffer);
    HRESULT FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger);
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer(DWORD dwIndex);
    HRESULT Play(DWORD dwPriority = 0, DWORD dwFlags = 0);
    HRESULT Stop();
    HRESULT Reset();
    BOOL IsSoundPlaying();
};
#endif
