/**
\author		Korotkov Andrey aka DRON
\date		23.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Sound.h"

using namespace std;

#ifndef NO_BUILTIN_SOUND

// CChannelDummy //

class CChannelDummy : public ISoundChannel
{
public:
	DGLE_RESULT DGLE_API Play(bool bLooped){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Pause(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Stop(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying){bIsPlaying = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume){uiVolume = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetPan(int iPan){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetPan(int &iPan){iPan = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetSpeed(int iSpeed){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetSpeed(int &iSpeed){iSpeed = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos){uiPos = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetLength(uint &uiLength){uiLength = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable){bStreamable = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Unaquire(){delete this; return S_OK;}

	IDGLE_BASE_IMPLEMENTATION(ISoundChannel)
};

// CChannel //

CChannel::CChannel():
_pSnd(NULL),
_uiSamplesPerSec(44100), _uiBitsPerSample(16), _bStereo(false), _bStreamable(false),
_c_pData(NULL), _ui32DataSize(0), _ui32ReaderPos(0), _ui32FrameCnt(0), _uiLength(0),
_bAquired(false), _fVol(1.f), _fPan(0.f), _fSpeed(0.f), _bLooped(false), _eState(SCS_STOPPED),
_frame(0, 0.f, 1.f), _pStreamCallback(NULL), _pParametr(NULL), _uiBufferSize(0)
{}

CChannel::CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize):
_pSnd(pBaseSound),
_uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(uiBitsPerSample), _bStereo(bStereo), _bStreamable(false),
_c_pData(pData), _ui32DataSize(ui32DataSize), _ui32ReaderPos(0), _ui32FrameCnt(0), _uiLength(0),
_bAquired(true), _fVol(1.f), _fPan(0.f), _fSpeed(0.f), _bLooped(false), _eState(SCS_STOPPED),
_frame(0, 0.f, 1.f), _pStreamCallback(NULL), _pParametr(NULL), _uiBufferSize(0)
{
	_uiLength = _DataPosToMsec(_ui32DataSize);
}

CChannel::CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, uint uiBufferSize, void (DGLE_API *pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParametr):
_pSnd(pBaseSound),
_uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(uiBitsPerSample), _bStereo(bStereo), _bStreamable(true),
_c_pData(NULL), _ui32DataSize(ui32DataSize), _ui32ReaderPos(0), _ui32FrameCnt(0), _uiLength(0),
_bAquired(true), _fVol(1.f), _fPan(0.f), _fSpeed(0.f), _bLooped(false), _eState(SCS_STOPPED),
_frame(0, 0.f, 1.f), _pStreamCallback(pStreamCallback), _pParametr(pParametr), _uiBufferSize(uiBufferSize)
{
	_uiLength = _DataPosToMsec(_ui32DataSize);
	_c_pData = new uint8[_uiBufferSize];
}

CChannel::~CChannel()
{
	if (_bStreamable)
		delete[] _c_pData;
}

inline uint CChannel::_DataPosToMsec(uint32 pos)
{
	return (pos / (_bStereo ? 2 : 1)) / (_uiSamplesPerSec * (_uiBitsPerSample / 8)) * 1000;
}

inline bool CChannel::IsActive()
{
	return _eState == SCS_PLAYING;
}

inline bool CChannel::IsDone()
{
	return !_bAquired && _eState == SCS_STOPPED;
}

inline bool CChannel::IsLooped()
{
	return _bLooped;
}

inline bool CChannel::IsAquired()
{
	return _bAquired;
}

inline uint CChannel::MsecLeftToPlay()
{
	return _uiLength - _DataPosToMsec(_ui32ReaderPos);
}

inline bool CChannel::CmprDataPtr(const uint8 *pData)
{
	return pData == _c_pData;
}

inline void CChannel::StreamData()
{
	if (!_bStreamable)
		return;

	_pStreamCallback(_pParametr, _ui32ReaderPos, const_cast<uint8 *>(_c_pData), _uiBufferSize);
}

__forceinline const TSoundFrame & CChannel::NextFrame(float masterVol)
{
	if (_eState != SCS_PLAYING)
		new(&_frame)TSoundFrame(0, 0.f, 1.f);
	else
	{
		int16 out[2];

		if (_uiBitsPerSample == 8)
		{
			out[0] = ((int16)_c_pData[_ui32ReaderPos] - 128) * 256;

			if (_bStereo)
				out[1] = ((int16)_c_pData[_ui32ReaderPos + 1] - 128) * 256;
		}
		else // 16 bit
		{
			const int16 *pi16_dat = reinterpret_cast<const int16 *>(&_c_pData[_ui32ReaderPos]);

			out[0] = pi16_dat[0];

			if (_bStereo)
				out[1] = pi16_dat[1];
		}

		if (_uiSamplesPerSec == 44100 || _ui32FrameCnt % 2 == 0)
			_ui32ReaderPos += (_uiBitsPerSample / 8) * (_bStereo ? 2 : 1);

		++_ui32FrameCnt;

		if (_ui32ReaderPos >= _ui32DataSize)
		{
			if (!_bLooped)
				_eState = SCS_STOPPED;
		
			_ui32ReaderPos = 0;
			_ui32FrameCnt = 0;
		}

		if (_bStereo)
			new(&_frame)TSoundFrame(out[0], out[1], _fVol * masterVol);
		else
			new(&_frame)TSoundFrame(out[0], _fPan, _fVol * masterVol);
	}

	return _frame;
}

DGLE_RESULT DGLE_API CChannel::Play(bool bLooped)
{
	_pSnd->EnterThreadSafeSection();
	
	_bLooped = bLooped;
	_eState = SCS_PLAYING;
	
	_pSnd->LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::Pause()
{
	_pSnd->EnterThreadSafeSection();

	_eState = SCS_PAUSED;
	
	_pSnd->LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::Stop()
{
	_pSnd->EnterThreadSafeSection();
	
	_eState = SCS_STOPPED;
	_ui32ReaderPos = 0;
	_ui32FrameCnt = 0;
	
	_pSnd->LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::IsPlaying(bool &bIsPlaying)
{
	_pSnd->EnterThreadSafeSection();

	bIsPlaying = _eState == SCS_PLAYING;
	
	_pSnd->LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::SetVolume(uint uiVolume)
{
	DGLE_RESULT res = S_OK;

	if (uiVolume > 100)
	{
		uiVolume = 100;
		res = S_FALSE;
	}

	_pSnd->EnterThreadSafeSection();

	_fVol = (float)uiVolume / 100.f;
	
	_pSnd->LeaveThreadSafeSection();

	return res;
}

DGLE_RESULT DGLE_API CChannel::GetVolume(uint &uiVolume)
{
	uiVolume = (uint)(100.f * _fVol);
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::SetPan(int iPan)
{
	if (_bStereo)
		return S_FALSE;

	DGLE_RESULT res = S_OK;

	if (iPan < -100) 
	{
		iPan = -100;
		res = S_FALSE;
	}

	if (iPan > 100)
	{
		iPan = 100;
		res = S_FALSE;
	}

	_pSnd->EnterThreadSafeSection();

	_fPan = (float)iPan / 100.f;

	_pSnd->LeaveThreadSafeSection();

	return res;
}

DGLE_RESULT DGLE_API CChannel::GetPan(int &iPan)
{
	iPan = (int)(100.f * _fPan);
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::SetSpeed(int iSpeed)
{
	_pSnd->EnterThreadSafeSection();

	_fSpeed = (float)iSpeed / 100.f;
	
	_pSnd->LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::GetSpeed(int &iSpeed)
{
	iSpeed = (int)(100.f * _fSpeed);
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::SetCurrentPosition(uint uiPos)
{
	DGLE_RESULT res = S_OK;

	if (uiPos > _uiLength)
	{
		uiPos = _uiLength;
		res = S_FALSE;
	}

	_pSnd->EnterThreadSafeSection();

	_ui32ReaderPos = (uint32)((float(uiPos / 1000.f) * _uiSamplesPerSec * (_uiBitsPerSample / 8)) * (_bStereo ? 2 : 1));

	_pSnd->LeaveThreadSafeSection();

	return res;
}

DGLE_RESULT DGLE_API CChannel::GetCurrentPosition(uint &uiPos)
{
	_pSnd->EnterThreadSafeSection();

	uiPos = _DataPosToMsec(_ui32ReaderPos);
	
	_pSnd->LeaveThreadSafeSection();
	
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::GetLength(uint &uiLength)
{
	uiLength = _uiLength;
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::IsStreamable(bool &bStreamable)
{
	bStreamable = _bStreamable;
	return S_OK;
}

DGLE_RESULT DGLE_API CChannel::Unaquire()
{
	if (!_bAquired)
		return S_FALSE;

	_bAquired = false;
	return S_OK;
}

// CSound //

CSound::CSound(uint uiInstIdx):
CBaseSound(uiInstIdx), _bPaused(false), _fMasterVolume(1.f), _iProfilerState(0)
{
	_bInited = OpenDevice(_sc_uiSamplesPerSec, _sc_uiBitsPerSample, true, _uiBufferSize, &_s_StreamToDeviceCallback, this);

	Console()->RegComValue("snd_profiler", "Displays sound subsystems profiler.", &_iProfilerState, 0, 1);
	
	Core()->AddEventListner(ET_ON_PROFILER_DRAW, &_s_EventProfilerDraw, (void*)this);

	if (_bInited)
		LOG("Sound Subsystem initialized.", LT_INFO);
	else
		LOG("Failed to initialize Sound Subsystem.", LT_ERROR);
}

CSound::~CSound()
{
	Console()->UnRegCom("snd_profiler");

	Core()->RemoveEventListner(ET_ON_PROFILER_DRAW, &_s_EventProfilerDraw, (void*)this);

	if (_bInited)
	{
		CloseDevice();
		LOG("Sound Subsystem finalized.", LT_INFO);
	}
	else
		LOG("Sound Subsystem destroyed.", LT_INFO);
}

void CSound::_ProfilerDraw()
{
	if (_iProfilerState == 0)
		return;

	Core()->RenderProfilerTxt("=======Sound Profiler=======", TColor4());
	
	if (!_bInited)
		Core()->RenderProfilerTxt("Initialization failed!", TColor4(255, 0, 0, 255));
	else
	{
		Core()->RenderProfilerTxt(("Bits per sample :" + UIntToStr(_sc_uiBitsPerSample)).c_str(), TColor4());
		Core()->RenderProfilerTxt(("Samples per sec.:" + UIntToStr(_sc_uiSamplesPerSec)).c_str(), TColor4());

		EnterThreadSafeSection();

		Core()->RenderProfilerTxt(("Mixer delay     :" + UInt64ToStr(_ui64MixDelay / 1000) + "." + UIntToStr(_ui64MixDelay % 1000) + " ms.").c_str(), TColor4());

		uint cnt = 0;

		for (uint i = 0; i < _sc_uiMaxChannels; ++i)
			if (!_clChannels[i].IsDone())
				++cnt;

		LeaveThreadSafeSection();

		Core()->RenderProfilerTxt(("Opened channels :" + UIntToStr(cnt) + "/" + UIntToStr(_sc_uiMaxChannels)).c_str(), TColor4());
	}
	
	Core()->RenderProfilerTxt("============================", TColor4());

}

void DGLE_API CSound::_s_EventProfilerDraw(void *pParametr, IBaseEvent *pEvent)
{
	PTHIS(CSound)->_ProfilerDraw();
}

// This method is called from separate thread.
void DGLE_API CSound::_s_StreamToDeviceCallback(void *pParametr, uint8 *pBufferData)
{
	PTHIS(CSound)->_MixSoundChannels(reinterpret_cast<TSoundFrame *>(pBufferData), (PTHIS(CSound)->_uiBufferSize / sizeof(int16)) / 2);
}

void CSound::_MixSoundChannels(TSoundFrame *frames, uint uiFramesCount)
{
	_ui64MixDelay = GetPerfTimer();

	memset(frames, 0, sizeof(TSoundFrame) * uiFramesCount);

	if (!_bPaused)
		for (uint i = 0; i < _sc_uiMaxChannels; ++i)
			if (_clChannels[i].IsActive())
			{
				_clChannels[i].StreamData();

				for (uint j = 0; j < uiFramesCount; ++j)
					frames[j] += _clChannels[i].NextFrame(_fMasterVolume);
			}

	_ui64MixDelay = GetPerfTimer() - _ui64MixDelay;
}

uint CSound::_ReleaseChannel()
{
	uint idx = -1, msecs_left = -1;

	EnterThreadSafeSection();

	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		if (!_clChannels[i].IsAquired() && !_clChannels[i].IsLooped() && _clChannels[i].MsecLeftToPlay() < msecs_left)
		{
			idx = i;
			msecs_left = _clChannels[i].MsecLeftToPlay();
		}

	LeaveThreadSafeSection();

	return idx;
}

DGLE_RESULT DGLE_API CSound::SetMasterVolume(uint uiVolume)
{
	DGLE_RESULT res = S_OK;

	if (uiVolume > 100)
	{
		uiVolume = 100;
		res = S_FALSE;
	}

	EnterThreadSafeSection();
	
	_fMasterVolume = (float)uiVolume / 100.f;
	
	LeaveThreadSafeSection();
	
	return res;
}

DGLE_RESULT DGLE_API CSound::MasterPause(bool bPaused)
{
	_bPaused = bPaused;
	return S_OK;
}

DGLE_RESULT DGLE_API CSound::StopAllChannels()
{
	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		_clChannels[i].Stop();

	return S_OK;
}

DGLE_RESULT DGLE_API CSound::GetMaxChannelsCount(uint &uiCount)
{
	uiCount = _sc_uiMaxChannels;
	return S_OK;
}

DGLE_RESULT DGLE_API CSound::GetFreeChannelsCount(uint &uiCount)
{
	uiCount = 0;

	EnterThreadSafeSection();

	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		if (_clChannels[i].IsDone())
			++uiCount;

	LeaveThreadSafeSection();

	return S_OK;
}

DGLE_RESULT DGLE_API CSound::ReleaseChannelsByData(const uint8 *pData)
{
	DGLE_RESULT res = S_OK;

	EnterThreadSafeSection();

	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		if (_clChannels[i].CmprDataPtr(pData))
		{
			if (_clChannels[i].IsAquired())
			{
				res = S_FALSE;
				_clChannels[i].Unaquire();
			}

			_clChannels[i].Stop();
		}

	LeaveThreadSafeSection();

	if (res != S_OK)
		LOG("Releasing aquired sound channel.", LT_WARNING);

	return res;
}

DGLE_RESULT DGLE_API CSound::CreateChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize)
{
	if (!_bInited || !pData || ui32DataSize == 0 || (uiBitsPerSample != 8 && uiBitsPerSample != 16) || (uiSamplesPerSec != 22050 && uiSamplesPerSec != 44100))
	{
		prSndChnl = new CChannelDummy();
		return E_INVALIDARG;
	}

	uint idx = -1;

	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		if (_clChannels[i].IsDone())
		{
			idx = i;
			break;
		}

	if (idx == -1 && (idx = _ReleaseChannel(), idx == -1))
	{
		prSndChnl = new CChannelDummy();
		return E_ABORT;
	}

	EnterThreadSafeSection();

	_clChannels[idx].~CChannel();
	new(&_clChannels[idx])CChannel(this, uiSamplesPerSec, uiBitsPerSample, bStereo, pData, ui32DataSize);

	LeaveThreadSafeSection();

	prSndChnl = &_clChannels[idx];

	return S_OK;
}

DGLE_RESULT DGLE_API CSound::CreateStreamableChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, void (DGLE_API *pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParametr)
{
	if (!_bInited || !pStreamCallback || ui32DataSize == 0 || (uiBitsPerSample != 8 && uiBitsPerSample != 16) || (uiSamplesPerSec != 22050 && uiSamplesPerSec != 44100))
	{
		prSndChnl = new CChannelDummy();
		return E_INVALIDARG;
	}

	uint idx = -1;

	for (uint i = 0; i < _sc_uiMaxChannels; ++i)
		if (_clChannels[i].IsDone())
		{
			idx = i;
			break;
		}

	if (idx == -1 && (idx = _ReleaseChannel(), idx == -1))
	{
		prSndChnl = new CChannelDummy();
		return E_ABORT;
	}

	EnterThreadSafeSection();

	_clChannels[idx].~CChannel();
	new(&_clChannels[idx])CChannel(this, uiSamplesPerSec, uiBitsPerSample, bStereo, ui32DataSize, _uiBufferSize, pStreamCallback, pParametr);

	LeaveThreadSafeSection();

	prSndChnl = &_clChannels[idx];

	return S_OK;
}

DGLE_RESULT DGLE_API CSound::GetType(E_ENGINE_SUB_SYSTEM &eSubsysType)
{
	eSubsysType = ESS_SOUND;
	return S_OK;
}

#endif