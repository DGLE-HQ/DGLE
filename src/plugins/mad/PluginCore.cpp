/**
\author		Korotkov Andrey aka DRON
\date		13.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "PluginCore.h"

// Samples per frame for MPEG Layer III format.
#define MP3_SAMPLES_PER_FRAME 1152

//CMusic//

class CMusic: public IMusic
{
	static const uint _sc_uiSamplesPerSec = 44100;
	static const uint _sc_uiBitsPerSample = 16;

	uint _uiInstIdx;

	IResourceManager *_pResMan;
	ISound *_pSound;
	ISoundChannel *_pSndChan;
	bool _bLooped;
	uint _uiLength;

	const uint8 *_cpData;
	uint32 _ui32DataSize;

	uint32 _ui32PrevDataPos,
		_ui32DataRemaining;

	int16 _i16RemainigBuffer[MP3_SAMPLES_PER_FRAME * 2];
	uint _uiRemainingBufferSize;

	mad_stream _stream;
	mad_frame _frame;
	mad_synth _synth;
	mad_timer_t _timer;

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

	void _Stream(uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)
	{
		uint seek_pos = -1;

		if (ui32DataPos != _ui32PrevDataPos + uiBufferSize)
		{
			seek_pos = (uint)(((float)ui32DataPos / 2) / (_sc_uiSamplesPerSec * (_sc_uiBitsPerSample / 8)) * 1000);

			if (seek_pos == 0 || seek_pos < (uint)mad_timer_count(_timer, MAD_UNITS_MILLISECONDS))
			{
				_stream.next_frame = _cpData;
				_ui32DataRemaining = _ui32DataSize;
				
				mad_timer_reset(&_timer);

				if (seek_pos == 0)
					seek_pos = -1;
			}
		}

		uint buffer_pos = 0;

		if (_uiRemainingBufferSize != 0)
		{
			const uint rem_buff_size = _uiRemainingBufferSize * sizeof(int16);
			memcpy(pBufferData, _i16RemainigBuffer, rem_buff_size);
			buffer_pos += rem_buff_size;
			_uiRemainingBufferSize = 0;
		}

		while (buffer_pos < uiBufferSize || seek_pos != -1)
		{
			if (_ui32DataRemaining <= MAD_BUFFER_GUARD)
				break;

			mad_stream_buffer(&_stream, _stream.next_frame, _ui32DataRemaining);

			const bool decoded = mad_frame_decode(&_frame, &_stream) == 0;

			mad_timer_add(&_timer, _frame.header.duration);
			
			_ui32DataRemaining = _stream.bufend - _stream.next_frame;

			if (decoded && seek_pos == -1)
			{
				mad_synth_frame(&_synth, &_frame);
		
				uint nsamples = _synth.pcm.length;
				int16 *buffer = reinterpret_cast<int16 *>(pBufferData);

				uint rem_buff_origin = 0;

				while (nsamples--)
				{
					if (buffer != _i16RemainigBuffer && buffer_pos + _synth.pcm.channels * sizeof(int16) > uiBufferSize)
					{
						_uiRemainingBufferSize = nsamples * _synth.pcm.channels;
						buffer = _i16RemainigBuffer;
						rem_buff_origin = buffer_pos;
					}
					
					const uint pos = _synth.pcm.length - nsamples - 1,
						buff_offset = (buffer_pos - rem_buff_origin) / sizeof(int16);

					buffer[buff_offset] = _s_Scale(_synth.pcm.samples[0][pos]);

					if (_synth.pcm.channels == 2)
						buffer[buff_offset + 1] = _s_Scale(_synth.pcm.samples[1][pos]);
				
					buffer_pos += _synth.pcm.channels * sizeof(int16);
				}
			}

				if (seek_pos != -1 && seek_pos < (uint)mad_timer_count(_timer, MAD_UNITS_MILLISECONDS))
					seek_pos = -1;
		}

		if (buffer_pos < uiBufferSize)
			memset(pBufferData + buffer_pos, 0, uiBufferSize - buffer_pos);

		_ui32PrevDataPos = ui32DataPos;
	}

	static void DGLE_API _s_StreamCallback(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)
	{
		((CMusic*)pParameter)->_Stream(ui32DataPos, pBufferData, uiBufferSize);
	}

public:

	CMusic(uint uiInstIdx, IResourceManager *pResMan, ISound *pSound, bool bStereo, uint uiLength, const uint8 *pData, uint32 ui32DataSize):
	_uiInstIdx(uiInstIdx), _pResMan(pResMan), _pSound(pSound), _pSndChan(NULL), _bLooped(false), _uiLength(uiLength),
	_ui32PrevDataPos(-1), _ui32DataRemaining(0), _cpData(pData), _ui32DataSize(ui32DataSize), _uiRemainingBufferSize(0)
	{
		memset(_i16RemainigBuffer, 0, MP3_SAMPLES_PER_FRAME * sizeof(int16));

		if (FAILED(_pSound->CreateStreamableChannel(_pSndChan, _sc_uiSamplesPerSec, _sc_uiBitsPerSample, bStereo,
			(uint)((float)_uiLength / 1000 * _sc_uiSamplesPerSec * (_sc_uiBitsPerSample / 8) * (bStereo ? 2 : 1)), &_s_StreamCallback, this)))
			_pSndChan = NULL;
		else
		{
			mad_stream_init(&_stream);
			mad_frame_init(&_frame);
			mad_synth_init(&_synth);
		}
	}

	~CMusic()
	{
		if (_pSndChan)
		{
			_pSndChan->Stop();
			_pSndChan->Unaquire();

			mad_synth_finish(&_synth);
			mad_frame_finish(&_frame);
			mad_stream_finish(&_stream);
		}
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
		_pSndChan->Stop();
		return S_OK;
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
		uiLength = _uiLength;
		return S_OK;
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
		_pResMan->RegisterFileFormat("mp3", EOT_MUSIC, "MPEG Audio Layer III (MP3) compatible music files.", &_s_LoadMP3, (void *)this);
}

void CPluginCore::_Free()
{
	if (_pSound)
		_pResMan->UnregisterFileFormat("mp3");
}

bool CPluginCore::_LoadMP3(IFile *pFile, IMusic *&prMusic)
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

	uint8 *data = new uint8[size + MAD_BUFFER_GUARD];
	
	pFile->Read(data, size, read);

	if (read != size)
	{
		delete[] data;
		LOG("Failed to read audio frames data.", LT_ERROR);
		return false;
	}

	memset(data + size, 0, MAD_BUFFER_GUARD);
	size += MAD_BUFFER_GUARD;

	mad_stream stream;
	mad_stream_init(&stream);
	
	mad_frame frame;
	mad_frame_init(&frame);

	mad_stream_buffer(&stream, data, size);

	mad_header header;

	if (0 != mad_stream_sync(&stream) || 0 != mad_header_decode(&header, &stream) || header.layer != MAD_LAYER_III)
	{
		LOG("Wrong MPEG Layer III audio file.", LT_ERROR);
		goto clean;
	}

	if (header.samplerate != 44100)
	{
		LOG("MPEG Layer III decoder supports only frequency equal to 44100 but got " + std::to_string(header.samplerate) + ".", LT_ERROR);
		goto clean;
	}

	mad_timer_t timer;
	mad_timer_reset(&timer);

	stream.next_frame = data;
	uint32 data_remainind = size;

	while (data_remainind != MAD_BUFFER_GUARD && stream.next_frame < data + size)
	{
		mad_stream_buffer(&stream, stream.next_frame, data_remainind);

		if (0 != mad_frame_decode(&frame, &stream) && !MAD_RECOVERABLE(stream.error))
		{
			LOG("Can't decode MPEG Layer III audio frames.", LT_ERROR);
			goto clean;
		}

		mad_timer_add(&timer, frame.header.duration);

		data_remainind = stream.bufend - stream.next_frame;
	}

	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

	prMusic = new CMusic(_uiInstIdx, _pResMan, _pSound, header.mode > MAD_MODE_SINGLE_CHANNEL, mad_timer_count(timer, MAD_UNITS_MILLISECONDS), data, size);

	return true;

clean:
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);
	delete[] data;
	return false;
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
	IMusic *p_music = NULL;

	const bool ret = ((CPluginCore*)pParameter)->_LoadMP3(pFile, p_music);

	if (ret) prObj = (IEngineBaseObject *&)p_music;

	return ret;
}
