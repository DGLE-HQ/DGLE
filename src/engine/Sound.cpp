/**
\author		Korotkov Andrey aka DRON
\date		23.09.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Sound.h"

using namespace std;

#ifndef NO_BUILTIN_SOUND

CSound::CSound(uint uiInstIdx):
CBaseSound(uiInstIdx)
{}

CSound::~CSound()
{}

HRESULT DGLE2_API CSound::SetMasterVolume(uint uiVolume)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::PauseAllChannels(bool bPaused)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::StopAllChannels()
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::SetMaxChannelsCount(uint uiCount)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::SetListnerPosition(const TPoint3 &stCoords)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::GetListnerPosition(TPoint3 &stCoords)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::SetListnerOrientation(const TVector3 &stDir, const TVector3 &stUp)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::GetListnerOrientation(TVector3 &stDir, TVector3 &stUp)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CSound::GetType(E_ENGINE_SUB_SYSTEM &eSubsysType)
{
	eSubsysType = ESS_SOUND;
	return S_OK;
}

#endif