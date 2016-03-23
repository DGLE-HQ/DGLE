/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

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

	DGLE_RESULT DGLE_API Play(bool bLooped) override;
	DGLE_RESULT DGLE_API Pause(bool bPaused) override;
	DGLE_RESULT DGLE_API Stop() override;
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying) override;
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume) override;
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume) override;
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos) override;
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos) override;
	DGLE_RESULT DGLE_API GetLength(uint &uiLength) override;

	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_MUSIC)
	IDGLE_BASE_IMPLEMENTATION(IMusic, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};