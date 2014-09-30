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

	uint _uiSamplesPerSec,
		_uiBitsPerSample,
		_uiBitRate;
	bool _bStereo;

	const uint8 *_pData;
	uint32 _ui32DataOffset, _ui32DataSize;

	static inline int16 _s_Scale(mad_fixed_t sample)
	{
		sample += (1L << (MAD_F_FRACBITS - 16));

		if (sample >= MAD_F_ONE)
			sample = MAD_F_ONE - 1;
		else
			if (sample < -MAD_F_ONE)
				sample = -MAD_F_ONE;

		return (int16)(sample >> (MAD_F_FRACBITS + 1 - 16));
	}

	uint32 _FindNextMP3Header(const uint32 ui32Offset)
	{
		const uint32 offset = _ui32DataOffset + ui32Offset;
		uint cnt = 0;
		uint32 pos = offset;

		while (offset + cnt < _ui32DataSize)
		{
			pos = cnt;
		
			byte frame = _pData[offset + cnt];
			++cnt;

			if (0xFF == frame)
			{
				frame = _pData[offset + cnt];
				++cnt;

				if(0xFA == frame || 0xFB == frame)
					return pos;
			}
		};

		return -1;
	}

	uint32 GetRoughMP3DataPosFromPCMDataPos(uint32 pos) const
	{
		const uint pos_msec = (pos / (_bStereo ? 2 : 1)) / (_uiSamplesPerSec * (_uiBitsPerSample / 8)) * 1000,
		avg_frame_size = (uint)(((1152 /*samples per frame for MPEG-1 Layer III format*/ / 8) * _uiBitRate / _uiSamplesPerSec) + 4 /*header size*/);
		return pos_msec / 26 /*length in msecs of single frame*/ * avg_frame_size;
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
		LOG("MPEG-1 Audio Layer III decoding error " + UIntToStrX(stream->error) + " (" +  mad_stream_errorstr(stream) + ") at byte offset " + UIntToStr(stream->this_frame - buffer->pData) + ".", LT_ERROR);
		
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

	CSoundSample(uint uiInstIdx, IResourceManager *pResMan, ISound *pSound, uint uiSamplesPerSec, uint uiBitRate, bool bStereo, const uint8 *pData, uint32 ui32DataOffset, uint32 ui32DataSize):
	_uiInstIdx(uiInstIdx), _pResMan(pResMan), _pSound(pSound), _uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(16),
	_uiBitRate(uiBitRate), _bStereo(bStereo), _pData(pData), _ui32DataOffset(ui32DataOffset), _ui32DataSize(ui32DataSize)
	{
		_MadDecode(&_pData[ui32DataOffset], _ui32DataSize - ui32DataOffset);
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
		_pResMan->RegisterFileFormat("mp3", EOT_SOUND_SAMPLE, "MPEG-1 Audio Layer III (MP3) compatible sound files.", &_s_LoadMP3, (void*)this);
}

void CPluginCore::_Free()
{
	if (_pSound)
		_pResMan->UnregisterFileFormat("mp3");
}

bool CPluginCore::_LoadMP3(IFile *pFile, ISoundSample *&prSSample)
{	
	uint read;

	char tag[3];
	uint32 file_pos;
	
	pFile->Seek(128, FSSF_END, file_pos);
	pFile->Read(tag, sizeof(tag), read);

	if (read != sizeof(tag))
	{
		LOG("Error checking ID3v1 tag.", LT_ERROR);
		return false;
	}
	
	const bool id3v1 = tag[0] == 'T' && tag[1] == 'A' && tag[2] == 'G';
	
	pFile->Seek(0, FSSF_BEGIN, file_pos);
	pFile->Read(tag, sizeof(tag), read);
	
	if (read != sizeof(tag))
	{
		LOG("Error checking ID3v2 tag.", LT_ERROR);
		return false;
	}

	const bool id3v2 = tag[0] == 'I' && tag[1] == 'D' && tag[2] == '3';

	if (id3v2)
	{
		byte size_bytes[4];
		pFile->Seek(3, FSSF_CURRENT, file_pos);
		pFile->Read(size_bytes, sizeof(size_bytes), read);
		const uint size = size_bytes[0] << 21 | size_bytes[1] << 14 | size_bytes[2] << 7 | size_bytes[3];
		pFile->Seek(size, FSSF_CURRENT, file_pos);
	}
	else
		pFile->Seek(0, FSSF_BEGIN, file_pos);

	uint32 size;
	pFile->GetSize(size);

	if (id3v1)
		size -= 128;

	if (id3v2)
		size -= file_pos;

	uint8 *data = new uint8[size];
	pFile->Read(data, size, read);

	if (read != size)
	{
		delete[] data;
		LOG("Failed to read audio frames data.", LT_ERROR);
		return false;
	}

	bool found = false;
	uint cnt = 0;
	uint32 pos = 0;
	byte frame;

	while (cnt < size)
	{
		pos = cnt;
		
		frame = data[cnt];
		++cnt;

		if (0xFF == frame)
		{
			frame = data[cnt];
			++cnt;

			if(0xFA == frame || 0xFB == frame)
			{
				found = true;
				break;
			}
		}
	};

	if ((frame & 0x08) != 0x08 && (frame & 0x06) != 0x02)
	{
		delete[] data;
		LOG("Not MPEG-1 Layer III audio file or unsupported format.", LT_ERROR);
		return false;
	}

	frame = data[cnt];
	++cnt;

	const uint bitrate_table[16] = {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 0},
	freq_table[4] = {44100, 48000, 32000, 0};

	const uint brate = bitrate_table[(frame & 0xF0) >> 4],
		freq = freq_table[(frame & 0x0C) >> 2];
	
	const bool padding = (frame & 0x02) == 0x02;

	frame = data[cnt];
	++cnt;

	const bool stereo = (frame & 0xC0) != 0xC0;

	//const uint frame_len = (uint)(((1152 /*samples per frame for MPEG-1 Layer III format*/ / 8) * brate / freq) + (padding ? 1 : 0));
	//
	//if (frame_len == 156 /*size of first header if VBR*/)
	//{
	//	delete[] data;
	//	LOG("Variable Bit Rate (VBR) is not supported by decoder for MPEG-1 Layer III audio files.", LT_ERROR);
	//	return false;
	//}

	prSSample = new CSoundSample(_uiInstIdx, _pResMan, _pSound, freq, brate, stereo, data, pos, size);

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
