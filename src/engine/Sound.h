/**
\author		Korotkov Andrey aka DRON
\date		23.09.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_SOUND

#ifdef PLATFORM_WINDOWS
#	include "platform\win\BaseSound.h"
#endif

class CSound: public CBaseSound, public ISound
{

public:

	CSound(uint uiInstIdx);
	~CSound();

	HRESULT DGLE2_API SetMasterVolume(uint uiVolume);
	HRESULT DGLE2_API PauseAllChannels(bool bPaused);
	HRESULT DGLE2_API StopAllChannels();
	HRESULT DGLE2_API SetMaxChannelsCount(uint uiCount);
	HRESULT DGLE2_API SetListnerPosition(const TPoint3 &stCoords);
	HRESULT DGLE2_API GetListnerPosition(TPoint3 &stCoords);
	HRESULT DGLE2_API SetListnerOrientation(const TVector3 &stDir, const TVector3 &stUp);
	HRESULT DGLE2_API GetListnerOrientation(TVector3 &stDir, TVector3 &stUp);
	HRESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubsysType);

	IDGLE2_BASE_IMPLEMENTATION1(ISound, IEngineSubSystem)
};

#endif