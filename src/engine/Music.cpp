/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "Music.h"

CMusic::CMusic(uint uiInstIdx, ISoundSample *pSndSample):
CInstancedObj(uiInstIdx), _pSndSample(pSndSample), _pSndChan(NULL), _bLooped(false)
{
	_pSndSample->PlayEx(_pSndChan);
	_pSndChan->Stop();
}

CMusic::~CMusic()
{
	_pSndChan->Stop();
	_pSndChan->Unaquire();
	_pSndSample->Free();
}

DGLE_RESULT DGLE_API CMusic::Play(bool bLooped)
{
	_bLooped = bLooped;
	return _pSndChan->Play(_bLooped);
}

DGLE_RESULT DGLE_API CMusic::Pause(bool bPaused)
{
	if (bPaused)
		return _pSndChan->Pause();
	else
		return _pSndChan->Play(_bLooped);
}

DGLE_RESULT DGLE_API CMusic::Stop()
{
	return _pSndChan->Stop();
}

DGLE_RESULT DGLE_API CMusic::IsPlaying(bool &bIsPlaying)
{
	return _pSndChan->IsPlaying(bIsPlaying);
}

DGLE_RESULT DGLE_API CMusic::SetVolume(uint uiVolume)
{
	return _pSndChan->SetVolume(uiVolume);
}

DGLE_RESULT DGLE_API CMusic::GetVolume(uint &uiVolume)
{
	return _pSndChan->GetVolume(uiVolume);
}

DGLE_RESULT DGLE_API CMusic::SetCurrentPosition(uint uiPos)
{
	return _pSndChan->SetCurrentPosition(uiPos);
}

DGLE_RESULT DGLE_API CMusic::GetCurrentPosition(uint &uiPos)
{
	return _pSndChan->GetCurrentPosition(uiPos);
}

DGLE_RESULT DGLE_API CMusic::GetLength(uint &uiLength)
{
	return _pSndChan->GetLength(uiLength);
}