/**
\author		Korotkov Andrey aka DRON
\date		18.02.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CMusic: public CInstancedObj, public IMusic
{

	ISoundSample *_pSndSample;
	ISoundChannel *_pSndChan;
	bool _bLooped;

public:

	CMusic(uint uiInstIdx, ISoundSample *pSndSample);
	~CMusic();

	DGLE_RESULT DGLE_API Play(bool bLooped);
	DGLE_RESULT DGLE_API Pause(bool bPaused);
	DGLE_RESULT DGLE_API Stop();
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying);
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume);
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume);
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos);
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos);
	DGLE_RESULT DGLE_API GetLength(uint &uiLength);

	IENGBASEOBJ_IMPLEMENTATION(EOT_MUSIC)
	IDGLE_BASE_IMPLEMENTATION(IMusic, INTERFACE_IMPL(IEngBaseObj, INTERFACE_IMPL_END))
};