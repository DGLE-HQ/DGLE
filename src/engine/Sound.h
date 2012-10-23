/**
\author		Korotkov Andrey aka DRON
\date		23.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_SOUND

#ifdef PLATFORM_WINDOWS
#	include "platform\windows\BaseSound.h"
#endif

struct TSoundFrame
{
	int16 i16L, i16R;

	TSoundFrame(int16 l, int16 r, float vol)
	{
		i16L = int16((float)l * vol);
		i16R = int16((float)r * vol);
	}
	
	TSoundFrame(int16 mono, float pan, float vol) 
	{
		i16L = (int16)((float)mono * min(1.f, 1.f - pan) * vol);
		i16R = (int16)((float)mono * min(1.f, pan + 1.f) * vol);
	}

	inline int16 Clamp(int val)
	{
		return (val < -0x8000 ? -0x8000 : (val > 0x7FFF ? 0x7FFF : (int16)val));
	}

	inline TSoundFrame& operator += (const TSoundFrame &s)
	{
		i16L = Clamp((int)i16L + (int)s.i16L);
		i16R = Clamp((int)i16R + (int)s.i16R);
		return *this;
	}
};

enum E_SOUND_CHANNEL_STATE
{
	SCS_STOPPED = 0,
	SCS_PLAYING,
	SCS_PAUSED
};

class CChannel : public ISoundChannel
{
	CBaseSound *_pSnd;

	uint _uiSamplesPerSec, _uiBitsPerSample;
	bool _bStereo, _bStreamable;
	const uint8 *_c_pData;
	uint32 _ui32DataSize, _ui32ReaderPos, _ui32FrameCnt;
	uint _uiLength;
	bool _bAquired;
	float _fVol, _fPan, _fSpeed;
	bool _bLooped;
	E_SOUND_CHANNEL_STATE _eState;

	void (DGLE_API *_pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize);
	void *_pParametr;
	uint _uiBufferSize;

	TSoundFrame _frame;

	inline uint _DataPosToMsec(uint32 pos);

public:

	// Thread unsafe methods.

	CChannel();
	CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize);
	CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, uint uiBufferSize, void (DGLE_API *pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParametr);
	~CChannel();

	inline bool IsActive();
	inline bool IsDone();
	inline bool IsLooped();
	inline bool IsAquired();
	inline bool CmprDataPtr(const uint8 *pData);
	inline uint MsecLeftToPlay();
	inline void StreamData();
	__forceinline const TSoundFrame & NextFrame(float masterVol);

	// Thread safe methods.
	
	DGLE_RESULT DGLE_API Play(bool bLooped);
	DGLE_RESULT DGLE_API Pause();
	DGLE_RESULT DGLE_API Stop();
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying);
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume);
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume);
	DGLE_RESULT DGLE_API SetPan(int iPan);
	DGLE_RESULT DGLE_API GetPan(int &iPan);
	DGLE_RESULT DGLE_API SetSpeed(int iSpeed);
	DGLE_RESULT DGLE_API GetSpeed(int &iSpeed);
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos);
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos);
	DGLE_RESULT DGLE_API GetLength(uint &uiLength);
	DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable);
	DGLE_RESULT DGLE_API Unaquire();

	IDGLE_BASE_IMPLEMENTATION(ISoundChannel)
};

class CSound: private CBaseSound, public ISound
{
	// Attention! This sound realization will work correct only with stereo 16 bit output and 44100 frequency.
	static const uint _sc_uiBitsPerSample = 16;
	static const uint _sc_uiSamplesPerSec = 44100;

	static const uint _sc_uiMaxChannels = 64;

	bool _bInited, _bPaused;
	uint _uiBufferSize;
	float _fMasterVolume;
	int _iProfilerState;
	volatile uint64 _ui64MixDelay;

	CChannel _clChannels[_sc_uiMaxChannels];

	static void DGLE_API _s_StreamToDeviceCallback(void *pParametr, uint8 *pBufferData);
	void _MixSoundChannels(TSoundFrame *frames, uint uiFramesCount);
	uint _ReleaseChannel();

	void _ProfilerDraw();
	static void DGLE_API _s_EventProfilerDraw(void *pParametr, IBaseEvent *pEvent);

public:

	CSound(uint uiInstIdx);
	~CSound();

	DGLE_RESULT DGLE_API SetMasterVolume(uint uiVolume);
	DGLE_RESULT DGLE_API MasterPause(bool bPaused);
	DGLE_RESULT DGLE_API StopAllChannels();
	DGLE_RESULT DGLE_API GetMaxChannelsCount(uint &uiCount);
	DGLE_RESULT DGLE_API GetFreeChannelsCount(uint &uiCount);
	DGLE_RESULT DGLE_API ReleaseChannelsByData(const uint8 *pData);
	DGLE_RESULT DGLE_API CreateChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize);
	DGLE_RESULT DGLE_API CreateStreamableChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, void (DGLE_API *pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParametr);
	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubsysType);

	IDGLE_BASE_IMPLEMENTATION1(ISound, IEngineSubSystem)
};

#endif