/**
\author		Andrey Korotkov aka DRON
\date		03.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\..\Common.h"

#ifndef NO_BUILTIN_SOUND

class CBaseSound : public CInstancedObj, public CPlatformBaseSound
{
	static const uint8 _sc_ui8MaxDevicesCount = 8;
	static const uint8 _sc_ui8Latency = 50;

	HWAVEOUT _hWaveOut;
	WAVEFORMATEX _stWaveFormat;
	WAVEHDR _stWaveBuffers[2];
	uint _uiBufferSize;
	uint8 *_pBuffersData;
	void (DGLE_API *_pStreamToDeviceCallback)(void *pParametr, uint8 *pBufferData);
	void *_pParametr;
	CRITICAL_SECTION _cs;
	bool _bDeviceClosingFlag;
	std::vector<std::string> _devices;

	uint _FindDevice(const WAVEFORMATEX &stFormat);
	bool _InitDevice(uint id);
	void _PrintDevList();

	static void DGLE_API _s_PrintDevList(void *pParametr, const char *pcParam);
	static void DGLE_API _s_PrintDevId(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ForceDevice(void *pParametr, const char *pcParam);
	
	static void CALLBACK _s_WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, PWAVEHDR pWaveHdr, DWORD dwParam2);

public:

	bool OpenDevice(uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint &uiBufferSize, void (DGLE_API *pStreamToDeviceCallback)(void *pParametr, uint8 *pBufferData), void *pParametr);
	void CloseDevice();
	void EnterThreadSafeSection();
	void LeaveThreadSafeSection();

	CBaseSound(uint uiInstIdx);
	~CBaseSound();

};

#endif
