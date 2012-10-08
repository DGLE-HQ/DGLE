/**
\author		Andrey Korotkov aka DRON
\date		03.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "BaseSound.h"

using namespace std;

#ifndef NO_BUILTIN_SOUND

#pragma message("Linking with \"Winmm.lib\".")
#pragma comment(linker, "/defaultlib:Winmm.lib")

CBaseSound::CBaseSound(uint uiInstIdx):
CInstancedObj(uiInstIdx), _hWaveOut(NULL),
_pBuffersData(NULL), _pStreamToDeviceCallback(NULL)
{
	Console()->RegComProc("snd_list_devices", "Prints the list of the output audio devices.", &_s_PrintDevList, (void*)this);
	Console()->RegComProc("snd_device_id", "Prints id of the current audio device.", &_s_PrintDevId, (void*)this);
	Console()->RegComProc("snd_force_device", "Forces sound system to use specific audio device.\nUsage: \"sound_force_device <device id>", &_s_ForceDevice, (void*)this);
}

CBaseSound::~CBaseSound()
{
	if (_hWaveOut)
			LOG("Audio device hasn't been closed before quit.", LT_ERROR);
}

void CALLBACK CBaseSound::_s_WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, PWAVEHDR pWaveHdr, DWORD dwParam2)
{
	CBaseSound *p_this = reinterpret_cast<CBaseSound *>(dwInstance);

	if (uMsg != WOM_DONE || p_this->_bDeviceClosingFlag)
		return;
	
	EnterCriticalSection(&p_this->_cs);

	waveOutUnprepareHeader(p_this->_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	
	if (p_this->_pStreamToDeviceCallback)
		p_this->_pStreamToDeviceCallback(p_this->_pParametr, (uint8 *)pWaveHdr->lpData);
	
	waveOutPrepareHeader(p_this->_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	
	waveOutWrite(p_this->_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	
	LeaveCriticalSection(&p_this->_cs);
}

uint CBaseSound::_FindDevice(const WAVEFORMATEX &stFormat)
{
	uint res = -1;
	_devices.clear();
	
	UINT count = waveOutGetNumDevs();

	for (UINT i = 0; i < count; ++i)
	{
		WAVEOUTCAPS caps;
		waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS));

		bool flag = false;

		if (MMSYSERR_NOERROR == waveOutOpen(NULL, i, &_stWaveFormat, 0, 0, WAVE_FORMAT_QUERY))
		{
			flag = true;
			if (res == -1) res = i;
		}

		_devices.push_back((flag ? string("Compatible") : string("Incompatible")) + " audio device with id: " + UIntToStr(i) + " name: \"" + string(caps.szPname) + "\"");
	}

	return res;
}

bool CBaseSound::_InitDevice(uint id)
{
	if (_hWaveOut)
		return false;

	if (MMSYSERR_NOERROR != waveOutOpen(&_hWaveOut, id, &_stWaveFormat, (DWORD_PTR)&_s_WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION))
	{
		LOG("Failed to open audio device.", LT_ERROR);
		return false;
	}
	
	InitializeCriticalSection(&_cs);
	
	_bDeviceClosingFlag = false;

	_ui32BufferSize = (_sc_ui8Latency * _stWaveFormat.nSamplesPerSec * (_stWaveFormat.wBitsPerSample / 8) / 1000) * _stWaveFormat.nChannels;
	
	memset(_stWaveBuffers, 0, sizeof(WAVEHDR) * 2);
	
	_pBuffersData = new uint8[_ui32BufferSize * 2];
	memset(_pBuffersData, 0, _ui32BufferSize * 2);


	_stWaveBuffers[0].dwBufferLength = _ui32BufferSize;
	_stWaveBuffers[0].lpData = (LPSTR)_pBuffersData;
	
	_s_WaveCallback(_hWaveOut, WOM_DONE, (DWORD)this, &_stWaveBuffers[0], 0);
	
	_stWaveBuffers[1].dwBufferLength = _ui32BufferSize;
	_stWaveBuffers[1].lpData = (LPSTR)&_pBuffersData[_ui32BufferSize];

	_s_WaveCallback(_hWaveOut, WOM_DONE, (DWORD)this, &_stWaveBuffers[1], 0);

	LOG("Audio device opened correctly.", LT_INFO);

	return true;
}

bool CBaseSound::OpenDevice(uint uiFrequency, uint uiBitsPerSample, bool bStereo, uint32 &ui32BufferSize, void (DGLE2_API *pStreamToDeviceCallback)(void *pParametr, uint8 *pBufferData), void *pParametr)
{
	if (_hWaveOut || !pStreamToDeviceCallback)
		return false;

	_stWaveFormat.nChannels = bStereo ? 2 : 1;
	_stWaveFormat.nSamplesPerSec = uiFrequency;
	_stWaveFormat.wBitsPerSample = uiBitsPerSample;
	_stWaveFormat.nBlockAlign = _stWaveFormat.wBitsPerSample / 8 * _stWaveFormat.nChannels;
	_stWaveFormat.nAvgBytesPerSec = _stWaveFormat.nSamplesPerSec * _stWaveFormat.nBlockAlign;
	_stWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	_stWaveFormat.cbSize = 0;

	UINT dev_id = -1;

	dev_id = _FindDevice(_stWaveFormat);

	LOG("Found " + UIntToStr(_devices.size()) + " output audio devices.", LT_INFO);

	for (size_t i = 0; i < _devices.size(); ++i)
		LOG(_devices[i], LT_INFO);

	if (dev_id == -1)
	{
		LOG("No any compatible audio device found.", LT_ERROR);
		return false;
	}

	LOG("Using compatible audio device with id " + UIntToStr(dev_id) + ".", LT_INFO);

	if (!_InitDevice(dev_id))
	{
		_hWaveOut = NULL;
		return false;
	}

	_pStreamToDeviceCallback = pStreamToDeviceCallback;
	_pParametr = pParametr;
	ui32BufferSize = _ui32BufferSize;

	return true;
}

void CBaseSound::CloseDevice()
{
	if (_hWaveOut)
	{
		_bDeviceClosingFlag = true;
		
		EnterCriticalSection(&_cs);
	
		DWORD tick = GetTickCount();
		while (!(_stWaveBuffers[0].dwFlags & WHDR_DONE && _stWaveBuffers[1].dwFlags & WHDR_DONE) && GetTickCount() - tick < 500) Sleep(0);

		bool flag = _stWaveBuffers[0].dwFlags & WHDR_DONE && _stWaveBuffers[1].dwFlags & WHDR_DONE;

		waveOutUnprepareHeader(_hWaveOut, &_stWaveBuffers[0], sizeof(WAVEHDR));
		waveOutUnprepareHeader(_hWaveOut, &_stWaveBuffers[1], sizeof(WAVEHDR));

		if (flag)
			waveOutReset(_hWaveOut);
	
		if (MMSYSERR_NOERROR != waveOutClose(_hWaveOut))
			LOG("Failed to close audio device properly.", LT_ERROR);
		else
			LOG("Audio device closed properly.", LT_INFO);
	
		delete[] _pBuffersData;
	
		_hWaveOut = NULL;
		_pBuffersData = NULL;

		LeaveCriticalSection(&_cs);
	}

	DeleteCriticalSection(&_cs);
}

void CBaseSound::_PrintDevList()
{
	string output("There is no audio output devices in system.");

	if (!_devices.empty())
	{
		output = "--------- Audio Devices ---------\n";

		for (size_t i = 0; i < _devices.size(); ++i)
			output += _devices[i] + "\n";

		output += "---------------------------------\n";
	}
		
	Console()->Write(output.c_str());
}

void DGLE2_API CBaseSound::_s_PrintDevList(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		CON(CBaseSound, "No parametrs expected.");
	else
		PTHIS(CBaseSound)->_PrintDevList();

}

void DGLE2_API CBaseSound::_s_PrintDevId(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		CON(CBaseSound, "No parametrs expected.");
	else
	{
		UINT id;
		waveOutGetID(PTHIS(CBaseSound)->_hWaveOut, &id);
		CON(CBaseSound, ("Using audio device with id " + UIntToStr(id) + ".").c_str());
	}
}

void DGLE2_API CBaseSound::_s_ForceDevice(void *pParametr, const char *pcParam)
{
	string param(pcParam);

	if (param.find(' ') != string::npos)
		CON(CBaseSound, "Only one parametr expected.");
	else
	{
		PTHIS(CBaseSound)->CloseDevice();
		PTHIS(CBaseSound)->_InitDevice(StrToUInt(param));
	}
}

#endif
