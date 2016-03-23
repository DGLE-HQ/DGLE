/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

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

	TSoundFrame() : i16L(0), i16R(0) {}

	inline void SetMono(int16 mono, float pan, float vol) 
	{
		i16L = (int16)((float)mono * fmin(1.f, 1.f - pan) * vol);
		i16R = (int16)((float)mono * fmin(1.f, pan + 1.f) * vol);
	}

	inline void SetStereo(const int16 *data, float vol) 
	{
		i16L = (int16)((float)data[0] * vol);
		i16R = (int16)((float)data[1] * vol);
	}

	inline int16 Clamp(int val) const
	{
		return (val < -0x8000 ? -0x8000 : (val > 0x7FFF ? 0x7FFF : (int16)val));
	}

	inline TSoundFrame &operator += (const TSoundFrame &s)
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

class CChannel final : public ISoundChannel
{
	CBaseSound *_pSnd;

	const uint _c_uiSamplesPerSec, _c_uiBitsPerSample;
	const bool _c_bStereo, _c_bStreamable;
	const uint8 *_c_pData;
	const uint32 _c_ui32DataSize;
	const uint _c_uiDataPerFrame;
	uint32 _ui32ReaderPos;
	float _fFrameCnt;
	const uint _c_uiLength;
	bool _bAquired;
	float _fVol, _fPan, _fSpeed;
	const float _c_fBaseSpeed;
	bool _bLooped;
	E_SOUND_CHANNEL_STATE _eState;

	void (DGLE_API *_pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize);
	void *_pParameter;
	const uint _c_uiBufferSize;
	uint32 _ui32BufferDataOffset;

	TSoundFrame _frame;

	inline uint _DataPosToMsec(uint32 pos) const;

public:

	// Thread unsafe methods.

	CChannel();
	CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize);
	CChannel(CBaseSound *pBaseSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, uint uiBufferSize, void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParameter);
	~CChannel();

	inline bool IsActive() const;
	inline bool IsDone() const;
	inline bool IsLooped() const;
	inline bool IsAquired() const;
	inline bool CmprDataPtr(const uint8 *pData) const;
	inline bool CmprCallbackPtr(void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)) const;
	inline uint MsecLeftToPlay() const;
	inline bool IsStreamable() const;
	inline void StreamData();
	FORCEINLINE const TSoundFrame & NextFrame(float masterVol);

	// Thread safe methods.
	
	DGLE_RESULT DGLE_API Play(bool bLooped) override;
	DGLE_RESULT DGLE_API Pause() override;
	DGLE_RESULT DGLE_API Stop() override;
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying) override;
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume) override;
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume) override;
	DGLE_RESULT DGLE_API SetPan(int iPan) override;
	DGLE_RESULT DGLE_API GetPan(int &iPan) override;
	DGLE_RESULT DGLE_API SetSpeed(uint uiSpeed) override;
	DGLE_RESULT DGLE_API GetSpeed(uint &uiSpeed) override;
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos) override;
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos) override;
	DGLE_RESULT DGLE_API GetLength(uint &uiLength) override;
	DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable) override;
	DGLE_RESULT DGLE_API Unaquire() override;

	IDGLE_BASE_IMPLEMENTATION(ISoundChannel, INTERFACE_IMPL_END)
};

class CSound final : private CBaseSound, public ISound
{
	// Attention! This sound realization will work correct only with stereo 16 bit output and 44100 frequency.
	static const uint _sc_uiBitsPerSample = 16;
	static const uint _sc_uiSamplesPerSec = 44100;

	static const uint _sc_uiMaxChannels = 64;

	bool _bInited, _bPaused;
	uint _uiBufferSize;
	float _fMasterVolume;
	int _iProfilerState, _iMuteState;
	volatile uint64 _ui64MixDelay;

	CChannel _clChannels[_sc_uiMaxChannels];

	static void DGLE_API _s_StreamToDeviceCallback(void *pParameter, uint8 *pBufferData);
	void _MixSoundChannels(TSoundFrame *frames, uint uiFramesCount);
	uint _ReleaseChannel();

	void _ProfilerDraw();
	static void DGLE_API _s_EventProfilerDraw(void *pParameter, IBaseEvent *pEvent);

public:

	CSound(uint uiInstIdx);
	~CSound();

	DGLE_RESULT DGLE_API SetMasterVolume(uint uiVolume) override;
	DGLE_RESULT DGLE_API MasterPause(bool bPaused) override;
	DGLE_RESULT DGLE_API StopAllChannels() override;
	DGLE_RESULT DGLE_API GetMaxChannelsCount(uint &uiCount) override;
	DGLE_RESULT DGLE_API GetFreeChannelsCount(uint &uiCount) override;
	DGLE_RESULT DGLE_API ReleaseChannelsByData(const uint8 *pData) override;
	DGLE_RESULT DGLE_API ReleaseChannelsByCallback(void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)) override;
	DGLE_RESULT DGLE_API CreateChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize) override;
	DGLE_RESULT DGLE_API CreateStreamableChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParameter) override;
	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubsysType) override;

	IDGLE_BASE_IMPLEMENTATION(ISound, INTERFACE_IMPL(IEngineSubSystem, INTERFACE_IMPL_END))
};

#endif