/**
\author		Korotkov Andrey aka DRON
\date		28.09.2014 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "PluginCore.h"

//CSoundSample//

class CSoundSample: public ISoundSample
{
	IResourceManager *_pResMan;
	ISound *_pSound;

	uint _uiSamplesPerSec;
	uint _uiBitsPerSample;
	bool _bStereo;
	const uint8 *_pData;
	uint32 _ui32DataSize;

public:

	CSoundSample(IResourceManager *pResMan, ISound *pSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize):
	_pResMan(pResMan), _pSound(pSound), _uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(uiBitsPerSample),
	_bStereo(bStereo), _pData(pData), _ui32DataSize(ui32DataSize)
	{}

	~CSoundSample()
	{
		_pSound->ReleaseChannelsByData(_pData);
		//delete[] _pData;
	}

	DGLE_RESULT DGLE_API Play()
	{
		ISoundChannel *p_chnl;
		
		DGLE_RESULT res = _pSound->CreateChannel(p_chnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _pData, _ui32DataSize);

		if (SUCCEEDED(res))
		{
			p_chnl->Play(false);
			p_chnl->Unaquire();
		}

		return res;
	}
	
	DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags)
	{
		DGLE_RESULT res = _pSound->CreateChannel(pSndChnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _pData, _ui32DataSize);

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

std::vector<int16> sdata;

struct mad_buffer
{
  unsigned char const *start;
  unsigned long length;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */
mad_flow input(void *data, mad_stream *stream)
{
  mad_buffer *buffer = (mad_buffer*)data;

  if (!buffer->length)
	return MAD_FLOW_STOP;

  mad_stream_buffer(stream, buffer->start, buffer->length);

  buffer->length = 0;

  return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */
inline int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
	sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
	sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */
mad_flow output(void *data, mad_header const *header, mad_pcm *pcm)
{
  unsigned int nchannels, nsamples;
  mad_fixed_t const *left_ch, *right_ch;

  /* pcm->samplerate contains the sampling frequency */

  nchannels = pcm->channels;
  nsamples  = pcm->length;
  left_ch   = pcm->samples[0];
  right_ch  = pcm->samples[1];

  while (nsamples--)
  {
	int16 sample;

	/* output sample(s) in 16-bit signed little-endian PCM */

	sample = scale(*left_ch++);

	sdata.push_back(sample);

	if (nchannels == 2)
	{
		sample = scale(*right_ch++);
		sdata.push_back(sample);
	}
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */
mad_flow error(void *data, mad_stream *stream, mad_frame *frame)
{
  mad_buffer *buffer = (mad_buffer*)data;

  uint _uiInstIdx = 0; // need to be removed
  LOG("MPEG decoding error " + UIntToStrX(stream->error) + " (" +  mad_stream_errorstr(stream) + ") at byte offset " + UIntToStr(stream->this_frame - buffer->start) + ".", LT_ERROR);
 
  /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */
int decode(unsigned char const *start, unsigned long length)
{
  mad_buffer buffer;
  struct mad_decoder decoder;
  int result;

  /* initialize our private message structure */

  buffer.start  = start;
  buffer.length = length;

  /* configure input, output, and error functions */

  mad_decoder_init(&decoder, &buffer, input, 0 /* header */, 0 /* filter */, output, error, 0 /* message */);

  /* start decoding */

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  /* release the decoder */

  mad_decoder_finish(&decoder);

  return result;
}

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
	// 2. Read header to check if supported http://mpgedit.org/mpgedit/mpeg_format/MP3Format.html .
	byte *data = new byte[size];
	uint read;
	pFile->Read(data, size, read);

	decode(data, size);

	prSSample = new CSoundSample(_pResMan, _pSound, 44100, 16, true,  reinterpret_cast<const uint8 *>(&sdata[0]), sdata.size() * sizeof(int16));

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

void DGLE_API CPluginCore::_s_Init(void *pParametr)
{
	((CPluginCore *)pParametr)->_Init();
}

void DGLE_API CPluginCore::_s_Free(void *pParametr)
{
	((CPluginCore *)pParametr)->_Free();
}

bool DGLE_API CPluginCore::_s_LoadMP3(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParametr)
{
	ISoundSample *ps = NULL;
	
	const bool ret = ((CPluginCore*)pParametr)->_LoadMP3(pFile, ps);
	
	if (ret)
	{
		if (uiLoadFlags & SSLF_LOAD_AS_MUSIC)
			prObj = new CMusic(((CPluginCore*)pParametr)->_pResMan, ps);
		else
			prObj = (IEngineBaseObject *&)ps;
	}

	return ret;
}
