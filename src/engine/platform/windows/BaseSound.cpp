/**
\author		Andrey Korotkov aka DRON
\date		03.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
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
	Console()->UnRegCom("snd_list_devices");
	Console()->UnRegCom("snd_device_id");
	Console()->UnRegCom("snd_force_device");

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
		p_this->_pStreamToDeviceCallback(p_this->_pParameter, (uint8 *)pWaveHdr->lpData);
	
	waveOutPrepareHeader(p_this->_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	
	waveOutWrite(p_this->_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	
	LeaveCriticalSection(&p_this->_cs);
}

uint CBaseSound::_FindDevice(const WAVEFORMATEX &stFormat)
{
	uint res = -1;
	_vecDevices.clear();
	
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

		string name(caps.szPname);

		string::size_type bracket_pos_1 = name.find_last_of('('), bracket_pos_2 = name.find_last_of(')');
		if (bracket_pos_1 != string::npos && (bracket_pos_2 == string::npos || bracket_pos_2 < bracket_pos_1))
			name += "...)";

		_vecDevices.push_back((flag ? string("Compatible") : string("Incompatible")) + " audio device with id: " + UIntToStr(i) + " name: \"" + name + "\"");
	}

	return res;
}

bool CBaseSound::_InitDevice(uint id)
{
	if (_hWaveOut)
		return false;

	if (EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_SINGLE_THREAD)
	{
		LOG("This sound realisation works only in multithreading mode.", LT_WARNING);
		return false;
	}

	if (MMSYSERR_NOERROR != waveOutOpen(&_hWaveOut, id, &_stWaveFormat, (DWORD_PTR)&_s_WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION))
	{
		LOG("Failed to open audio device.", LT_ERROR);
		return false;
	}
	
	InitializeCriticalSection(&_cs);
	
	_bDeviceClosingFlag = false;

	_uiBufferSize = _sc_ui8Latency * (_stWaveFormat.nSamplesPerSec / 1000) * (_stWaveFormat.wBitsPerSample / 8) * _stWaveFormat.nChannels;

	memset(_stWaveBuffers, 0, sizeof(WAVEHDR) * 2);
	
	_pBuffersData = new uint8[_uiBufferSize * 2];
	memset(_pBuffersData, 0, _uiBufferSize * 2);


	_stWaveBuffers[0].dwBufferLength = _uiBufferSize;
	_stWaveBuffers[0].lpData = (LPSTR)_pBuffersData;
	
	_s_WaveCallback(_hWaveOut, WOM_DONE, (DWORD)this, &_stWaveBuffers[0], 0);
	
	_stWaveBuffers[1].dwBufferLength = _uiBufferSize;
	_stWaveBuffers[1].lpData = (LPSTR)&_pBuffersData[_uiBufferSize];

	_s_WaveCallback(_hWaveOut, WOM_DONE, (DWORD)this, &_stWaveBuffers[1], 0);

	LOG("Audio device opened correctly.", LT_INFO);

	return true;
}

bool CBaseSound::OpenDevice(uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint &uiBufferSize, void (DGLE_API *pStreamToDeviceCallback)(void *pParameter, uint8 *pBufferData), void *pParameter)
{
	if (_hWaveOut || !pStreamToDeviceCallback)
		return false;

	_stWaveFormat.nChannels = bStereo ? 2 : 1;
	_stWaveFormat.nSamplesPerSec = uiSamplesPerSec;
	_stWaveFormat.wBitsPerSample = uiBitsPerSample;
	_stWaveFormat.nBlockAlign = _stWaveFormat.wBitsPerSample / 8 * _stWaveFormat.nChannels;
	_stWaveFormat.nAvgBytesPerSec = _stWaveFormat.nSamplesPerSec * _stWaveFormat.nBlockAlign;
	_stWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	_stWaveFormat.cbSize = 0;

	UINT dev_id = -1;

	dev_id = _FindDevice(_stWaveFormat);

	LOG("Found " + UIntToStr(_vecDevices.size()) + " output audio devices.", LT_INFO);

	for (size_t i = 0; i < _vecDevices.size(); ++i)
		LOG(_vecDevices[i], LT_INFO);

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
	_pParameter = pParameter;
	uiBufferSize = _uiBufferSize;

	return true;
}

void CBaseSound::CloseDevice()
{
	if (!_hWaveOut)
		return;

	_bDeviceClosingFlag = true;
		
	EnterCriticalSection(&_cs);
	
	DWORD tick = GetTickCount();
	while (!(_stWaveBuffers[0].dwFlags & WHDR_DONE && _stWaveBuffers[1].dwFlags & WHDR_DONE) && GetTickCount() - tick < 500) Sleep(0);

	bool flag = _stWaveBuffers[0].dwFlags & WHDR_DONE && _stWaveBuffers[1].dwFlags & WHDR_DONE;

	waveOutUnprepareHeader(_hWaveOut, &_stWaveBuffers[0], sizeof(WAVEHDR));
	waveOutUnprepareHeader(_hWaveOut, &_stWaveBuffers[1], sizeof(WAVEHDR));

	if (flag)
		waveOutReset(_hWaveOut);
	
	LeaveCriticalSection(&_cs);

	DeleteCriticalSection(&_cs);

	if (MMSYSERR_NOERROR != waveOutClose(_hWaveOut))
		LOG("Failed to close audio device properly.", LT_ERROR);
	else
		LOG("Audio device closed properly.", LT_INFO);
	
	delete[] _pBuffersData;
	
	_hWaveOut = NULL;
	_pBuffersData = NULL;
}

void CBaseSound::EnterThreadSafeSection()
{
	if (_hWaveOut)
		EnterCriticalSection(&_cs);
}

void CBaseSound::LeaveThreadSafeSection()
{
	if (_hWaveOut)
		LeaveCriticalSection(&_cs);
}

void CBaseSound::_PrintDevList()
{
	string output("There is no audio output devices in system.");

	if (!_vecDevices.empty())
	{
		output = "--------- Audio Devices ---------\n";

		for (size_t i = 0; i < _vecDevices.size(); ++i)
			output += _vecDevices[i] + "\n";

		output += "---------------------------------\n";
	}
		
	Console()->Write(output.c_str());
}

bool DGLE_API CBaseSound::_s_PrintDevList(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CBaseSound, "No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CBaseSound)->_PrintDevList();
		return true;
	}
}

bool DGLE_API CBaseSound::_s_PrintDevId(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CBaseSound, "No parameters expected.");
		return false;
	}
	else
	{
		UINT id;
		waveOutGetID(PTHIS(CBaseSound)->_hWaveOut, &id);
		CON(CBaseSound, ("Using audio device with id " + UIntToStr(id) + ".").c_str());
		return true;
	}
}

bool DGLE_API CBaseSound::_s_ForceDevice(void *pParameter, const char *pcParam)
{
	string param(pcParam);

	if (param.find(' ') != string::npos)
	{
		CON(CBaseSound, "Only one parameter expected.");
		return false;
	}
	else
	{
		PTHIS(CBaseSound)->CloseDevice();
		PTHIS(CBaseSound)->_InitDevice(StrToUInt(param));
		return true;
	}
}

#endif