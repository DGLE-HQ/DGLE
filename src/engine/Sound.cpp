/**
\author		Korotkov Andrey aka DRON
\date		04.10.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Sound.h"

using namespace std;

#ifndef NO_BUILTIN_SOUND

CSound::CSound(uint uiInstIdx):
CBaseSound(uiInstIdx)
{
	_bInited = OpenDevice(_sc_uiFrequency, _sc_uiBitsPerSample, true, _ui32BufferSize, &_s_StreamToDeviceCallback, this);

	if (_bInited)
		LOG("Sound Subsystem initialized.", LT_INFO);
	else
		LOG("Failed to initialize Sound Subsystem.", LT_ERROR);

}

CSound::~CSound()
{
	if (_bInited)
	{
		CloseDevice();
		LOG("Sound Subsystem finalized.", LT_INFO);
	}
	else
		LOG("Sound Subsystem destroyed.", LT_INFO);
}

void DGLE2_API CSound::_s_StreamToDeviceCallback(void *pParametr, uint8 *pBufferData)
{
	PTHIS(CSound)->_Render(reinterpret_cast<TSoundFrame *>(pBufferData), (PTHIS(CSound)->_ui32BufferSize / sizeof(int16)) / 2);
}

void CSound::_Render(TSoundFrame *frames, uint uiFramesCount)
{

}

DGLE2_RESULT DGLE2_API CSound::SetMasterVolume(uint uiVolume)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::PauseAllChannels(bool bPaused)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::StopAllChannels()
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::SetMaxChannelsCount(uint uiCount)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::SetListnerPosition(const TPoint3 &stCoords)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::GetListnerPosition(TPoint3 &stCoords)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::SetListnerOrientation(const TVector3 &stDir, const TVector3 &stUp)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::GetListnerOrientation(TVector3 &stDir, TVector3 &stUp)
{
	return E_NOTIMPL;
}

DGLE2_RESULT DGLE2_API CSound::GetType(E_ENGINE_SUB_SYSTEM &eSubsysType)
{
	eSubsysType = ESS_SOUND;
	return S_OK;
}

#endif