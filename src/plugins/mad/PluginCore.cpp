/**
\author		Korotkov Andrey aka DRON
\date		30.09.2014 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "PluginCore.h"

//CSoundSample//

std::vector<uint16> sdata;

class CSoundSample: public ISoundSample
{
	struct TMadBuffer
	{
		CSoundSample *pSndSample;
		const uint8 *pData;
		uint32 ui32Size;

		TMadBuffer(CSoundSample *sample, const uint8 *data, uint32 size):
		pSndSample(sample), pData(data), ui32Size(size)
		{}
	};

	uint _uiInstIdx;

	IResourceManager *_pResMan;
	ISound *_pSound;

	uint _uiSamplesPerSec;
	uint _uiBitsPerSample;
	bool _bStereo;
	const uint8 *_pData;
	uint32 _ui32DataSize;

	static inline int16 _s_Scale(mad_fixed_t sample)
	{
		sample += (1L << (MAD_F_FRACBITS - 16));

		if (sample >= MAD_F_ONE)
			sample = MAD_F_ONE - 1;
		else
			if (sample < -MAD_F_ONE)
				sample = -MAD_F_ONE;

		return sample >> (MAD_F_FRACBITS + 1 - 16);
	}

	static mad_flow _s_MadInput(void *pParameter, mad_stream *stream)
	{
		TMadBuffer *buffer = (TMadBuffer*)pParameter;

		if (buffer->ui32Size == 0)
			return MAD_FLOW_STOP;

		mad_stream_buffer(stream, buffer->pData, buffer->ui32Size);

		buffer->ui32Size = 0;

		return MAD_FLOW_CONTINUE;
	}

	static mad_flow _s_MadOutput(void *pParameter, mad_header const *header, mad_pcm *pcm)
	{
		TMadBuffer *buffer = (TMadBuffer*)pParameter;

		//pcm->samplerate
		const bool stereo = pcm->channels == 2;
		uint nsamples = pcm->length;
		mad_fixed_t const *left_ch = pcm->samples[0],
			*right_ch = pcm->samples[1];

		sdata.reserve(sdata.size() + nsamples * (stereo ? 2 : 1));

		while (nsamples--)
		{
			int16 sample = _s_Scale(*left_ch++);

			sdata.push_back(sample);

			if (stereo)
			{
				sample = _s_Scale(*right_ch++);
				sdata.push_back(sample);
			}
		}

		return MAD_FLOW_CONTINUE;
	}

	static mad_flow _s_MadError(void *pParameter, mad_stream *stream, mad_frame *frame)
	{
		TMadBuffer *buffer = ((TMadBuffer*)pParameter);
		
		const uint _uiInstIdx = buffer->pSndSample->_uiInstIdx;
		LOG("MPEG Audio Layer III decoding error " + UIntToStrX(stream->error) + " (" +  mad_stream_errorstr(stream) + ") at byte offset " + UIntToStr(stream->this_frame - buffer->pData) + ".", LT_ERROR);
		
		return MAD_FLOW_BREAK;
	}

	bool _MadDecode(const uint8 *pData, uint32 ui32Size)
	{
		TMadBuffer buffer(this, pData, ui32Size);

		mad_decoder decoder;
		mad_decoder_init(&decoder, &buffer, _s_MadInput, 0 /* header */, 0 /* filter */, _s_MadOutput, _s_MadError, 0 /* message */);

		int result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

		mad_decoder_finish(&decoder);

		return result == 0;
	}

	void _StreamCallback(uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)
	{
	}

	static void DGLE_API _s_StreamCallback(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)
	{
		((CSoundSample*)pParameter)->_StreamCallback(ui32DataPos, pBufferData, uiBufferSize);
	}

public:

	CSoundSample(uint uiInstIdx, IResourceManager *pResMan, ISound *pSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize):
	_uiInstIdx(uiInstIdx), _pResMan(pResMan), _pSound(pSound), _uiSamplesPerSec(uiSamplesPerSec),
	_uiBitsPerSample(uiBitsPerSample), _bStereo(bStereo), _pData(pData), _ui32DataSize(ui32DataSize)
	{
		_MadDecode(_pData, _ui32DataSize);
	}

	~CSoundSample()
	{
		_pSound->ReleaseChannelsByData(_pData);
		delete[] _pData;
	}

	DGLE_RESULT DGLE_API Play()
	{
		ISoundChannel *p_chnl;
		
		DGLE_RESULT res = _pSound->CreateChannel(p_chnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, reinterpret_cast<const uint8 *>(&sdata[0]), sdata.size() * sizeof(int16));
		//DGLE_RESULT res = _pSound->CreateStreamableChannel(p_chnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _ui32DataSize, &_s_StreamCallback, this);

		if (SUCCEEDED(res))
		{
			p_chnl->Play(false);
			p_chnl->Unaquire();
		}

		return res;
	}
	
	DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags)
	{
		DGLE_RESULT res = _pSound->CreateStreamableChannel(pSndChnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _ui32DataSize, &_s_StreamCallback, this);

		if (SUCCEEDED(res))
			pSndChnl->Play((bool)(eFlags & SSP_LOOPED));

		return res;
	}
	
	DGLE_RESULT DGLE_API Free()
	{
		bool can_delete;	
		_pResMan->RemoveResource(this, can_delete);
		if (can_delete)
		{
			delete this;
			return S_OK;
		}
		else
			return S_FALSE;
	}

	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType)
	{
		eObjType = EOT_SOUND_SAMPLE;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType)
	{
		uiObjUnknownType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(ISoundSample, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};

//CMusic//

class CMusic: public IMusic
{
	IResourceManager *_pResMan;
	ISoundSample *_pSndSample;
	ISoundChannel *_pSndChan;
	bool _bLooped;

public:

	CMusic(IResourceManager *pResMan, ISoundSample *pSndSample):
	_pResMan(pResMan), _pSndSample(pSndSample), _pSndChan(NULL), _bLooped(false)
	{
		_pSndSample->PlayEx(_pSndChan);
		_pSndChan->Stop();
	}

	~CMusic()
	{
		_pSndChan->Stop();
		_pSndChan->Unaquire();
		_pSndSample->Free();
	}

	DGLE_RESULT DGLE_API Play(bool bLooped)
	{
		_bLooped = bLooped;
		return _pSndChan->Play(_bLooped);
	}

	DGLE_RESULT DGLE_API Pause(bool bPaused)
	{
		if (bPaused)
			return _pSndChan->Pause();
		else
			return _pSndChan->Play(_bLooped);
	}

	DGLE_RESULT DGLE_API Stop()
	{
		return _pSndChan->Stop();
	}

	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying)
	{
		return _pSndChan->IsPlaying(bIsPlaying);
	}

	DGLE_RESULT DGLE_API SetVolume(uint uiVolume)
	{
		return _pSndChan->SetVolume(uiVolume);
	}

	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume)
	{
		return _pSndChan->GetVolume(uiVolume);
	}

	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos)
	{
		return _pSndChan->SetCurrentPosition(uiPos);
	}

	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos)
	{
		return _pSndChan->GetCurrentPosition(uiPos);
	}

	DGLE_RESULT DGLE_API GetLength(uint &uiLength)
	{
		return _pSndChan->GetLength(uiLength);
	}

	DGLE_RESULT DGLE_API Free()
	{
		bool can_delete;	
		_pResMan->RemoveResource(this, can_delete);
		if (can_delete)
		{
			delete this;
			return S_OK;
		}
		else
			return S_FALSE;
	}

	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType)
	{
		eObjType = EOT_MUSIC;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType)
	{
		uiObjUnknownType = -1;
		return S_FALSE;
	}
	
	IDGLE_BASE_IMPLEMENTATION(IMusic, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};

//CPluginCore//

CPluginCore::CPluginCore(IEngineCore *pEngineCore):
_pEngineCore(pEngineCore)
{
	_pEngineCore->GetInstanceIndex(_uiInstIdx);

	_pEngineCore->AddProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->AddProcedure(EPT_FREE, &_s_Free, (void*)this);
}

CPluginCore::~CPluginCore()
{
	_pEngineCore->RemoveProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_FREE, &_s_Free, (void*)this);
}

void CPluginCore::_Init()
{
	_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)_pResMan);
	
	if (SUCCEEDED(_pEngineCore->GetSubSystem(ESS_SOUND, (IEngineSubSystem *&)_pSound)))
		_pResMan->RegisterFileFormat("mp3", EOT_SOUND_SAMPLE, "MPEG-1 or MPEG-2 Audio Layer III sound files.", &_s_LoadMP3, (void*)this);
}

void CPluginCore::_Free()
{
	if (_pSound)
		_pResMan->UnregisterFileFormat("mp3");
}

bool CPluginCore::_LoadMP3(IFile *pFile, ISoundSample *&prSSample)
{
	uint32 size;
	pFile->GetSize(size);

	//ToDo:
	// 1. Skip ID3v1 and ID3v2 tags.
	// ID3V1
	/*struct TID3v1
	{
		char tag[3];
		char title[30];
		char artist[30];
		char album[30];
		char year[4];
		char comment[30];
		unsigned char genre;
	};*/
	// http://www.ulduzsoft.com/2012/07/parsing-id3v2-tags-in-the-mp3-files/
	// 2. Read header to check if supported http://mpgedit.org/mpgedit/mpeg_format/MP3Format.html http://www.cplusplus.com/forum/general/105054/ .
	byte *data = new byte[size];
	uint read;
	pFile->Read(data, size, read);

	prSSample = new CSoundSample(_uiInstIdx, _pResMan, _pSound, 44100, 16, true, data, size);

	return true;
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInfo(TPluginInfo &stInfo)
{
	strcpy(stInfo.cName, PLUGIN_NAME);
	strcpy(stInfo.cVersion, PLUGIN_VERSION);
	strcpy(stInfo.cVendor, PLUGIN_VENDOR);
	strcpy(stInfo.cDescription, PLUGIN_DESCRIPTION);
	stInfo.ui8PluginSDKVersion = _DGLE_PLUGIN_SDK_VER_;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInterfaceName(char* pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount <= (uint)strlen(PLUGIN_INTERFACE_NAME))
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		if (uiCharsCount > 0)
			strcpy(pcName, "");
		return E_INVALIDARG;
	}

	strcpy(pcName, PLUGIN_INTERFACE_NAME);
	
	return S_OK;
}

void DGLE_API CPluginCore::_s_Init(void *pParameter)
{
	((CPluginCore *)pParameter)->_Init();
}

void DGLE_API CPluginCore::_s_Free(void *pParameter)
{
	((CPluginCore *)pParameter)->_Free();
}

bool DGLE_API CPluginCore::_s_LoadMP3(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	ISoundSample *ps = NULL;
	
	const bool ret = ((CPluginCore*)pParameter)->_LoadMP3(pFile, ps);
	
	if (ret)
	{
		if (uiLoadFlags & SSLF_LOAD_AS_MUSIC)
			prObj = new CMusic(((CPluginCore*)pParameter)->_pResMan, ps);
		else
			prObj = (IEngineBaseObject *&)ps;
	}

	return ret;
}
