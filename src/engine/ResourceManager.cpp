/**
\author		Korotkov Andrey aka DRON
\date		18.04.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "ResourceManager.h"
#include "Material.h"
#include "Light.h"
#include "MainFS.h"
#include "Texture.h"
#include "Font.h"
#include "Music.h"
#include "Mesh.h"
#include "Model.h"

#ifdef PLATFORM_WINDOWS
#	include "ResFile.h"
#	include "..\..\build\windows\engine\resource.h"
#endif

using namespace std;

//Dummy Classes//

class CBObjDummy: public IEngineBaseObject
{
public:
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType){eObjType = EOT_UNKNOWN; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType){uiObjUnknownType = 0; return S_OK;}

	IDGLE_BASE_IMPLEMENTATION(IEngineBaseObject, INTERFACE_IMPL_END)
};

class CSoundChannelDummy : public ISoundChannel
{
public:
	DGLE_RESULT DGLE_API Play(bool bLooped){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Pause(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Stop(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying){bIsPlaying = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume){uiVolume = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetPan(int iPan){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetPan(int &iPan){iPan = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetSpeed(uint uiSpeed){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetSpeed(uint &uiSpeed){uiSpeed = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos){uiPos = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetLength(uint &uiLength){uiLength = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable){bStreamable = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Unaquire(){delete this; return S_OK;}

	IDGLE_BASE_IMPLEMENTATION(ISoundChannel, INTERFACE_IMPL_END)
};

class CSSampleDummy: public ISoundSample
{
public:
	DGLE_RESULT DGLE_API Play(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags){pSndChnl = (ISoundChannel*)new CSoundChannelDummy; return E_NOTIMPL;}
	
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType){eObjType = EOT_SOUND_SAMPLE; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType){uiObjUnknownType = -1; return S_FALSE;}

	IDGLE_BASE_IMPLEMENTATION(ISoundSample, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};

class CMusicDummy: public IMusic
{
public:
	DGLE_RESULT DGLE_API Play(bool bLooped){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Pause(bool bPaused){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Stop(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying){bIsPlaying = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetVolume(uint uiVolume){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetVolume(uint &uiVolume){uiVolume = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos){uiPos = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetLength(uint &uiLength){uiLength = 0; return E_NOTIMPL;}
	
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType){eObjType = EOT_MUSIC; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType) {uiObjUnknownType = -1; return S_FALSE;}

	IDGLE_BASE_IMPLEMENTATION(IMusic, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};

class CBitmapFontDummy: public IBitmapFont
{
public:
	DGLE_RESULT DGLE_API GetTexture(ITexture *&prTexture){prTexture = NULL; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetScale(float fScale){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetScale(float &fScale){fScale = 1.f; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight){uiWidth = 0; uiHeight = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2DRect(const TRectF &stRect, const char *pcTxt, const TColor4 &stColor){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw3D(const char *pcTxt){return E_NOTIMPL;}
	
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType){eObjType = EOT_BITMAP_FONT; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType) {uiObjUnknownType = -1; return S_FALSE;}

	IDGLE_BASE_IMPLEMENTATION(IBitmapFont, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};

//CSoundSample//

class CSoundSample: public CInstancedObj, public ISoundSample
{
	uint _uiSamplesPerSec;
	uint _uiBitsPerSample;
	bool _bStereo;
	const uint8 *_pData;
	uint32 _ui32DataSize;

public:

	CSoundSample(uint uiInstIdx, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize):
	CInstancedObj(uiInstIdx), _uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(uiBitsPerSample),
	_bStereo(bStereo), _pData(pData), _ui32DataSize(ui32DataSize)
	{}

	~CSoundSample()
	{
		Core()->pSound()->ReleaseChannelsByData(_pData);
		delete[] _pData;
	}

	DGLE_RESULT DGLE_API Play()
	{
		if (!Core()->pSound())
			return E_ABORT;

		ISoundChannel *p_chnl;
		
		DGLE_RESULT res = Core()->pSound()->CreateChannel(p_chnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _pData, _ui32DataSize);

		if (SUCCEEDED(res))
		{
			p_chnl->Play(false);
			p_chnl->Unaquire();
		}

		return res;
	}
	
	DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags)
	{
		if (!Core()->pSound())
		{
			pSndChnl = NULL;
			return E_ABORT;
		}

		DGLE_RESULT res = Core()->pSound()->CreateChannel(pSndChnl, _uiSamplesPerSec, _uiBitsPerSample, _bStereo, _pData, _ui32DataSize);

		if (SUCCEEDED(res))
			pSndChnl->Play((bool)(eFlags & SSP_LOOPED));

		return res;
	}
	
	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_SOUND_SAMPLE)
	IDGLE_BASE_IMPLEMENTATION(ISoundSample, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};


//CResourceManager//

const char CResourceManager::sc_acFormatString[] = "DMD 3.1 BIN";

CResourceManager::CResourceManager(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_iProfilerState(0), _uiResIdxCounter(0)
{
	// If this assertion occures than you must see _s_GetObjTypeName method if all E_ENGINE_OBJECT_TYPE enum elements are implemented there.
	// After it change this assertion expression as well.
	assert(EOT_MUSIC == EOT_EMPTY - 1);

	Core()->pMainWindow()->GetWindowHandle(_stWnd);
	
	_pCoreRenderer = Core()->pCoreRenderer();

	Console()->RegComVar("rman_stats", "Displays resource manager subsystems statistic.", &_iProfilerState, 0, 2);
	Console()->RegComProc("rman_list_file_formats", "Lists all file formats registered in the Resource Manager.", &_s_ConListFileFormats, (void*)this);
	Console()->RegComProc("rman_list_resources", "Lists all loaded resources.", &_s_ConListResources, (void*)this);

	RegisterFileFormat("bmp", EOT_TEXTURE, "BitMaP images.", &_s_LoadTextureBMP, (void*)this);
	RegisterFileFormat("tga", EOT_TEXTURE, "truevision TarGA images.", &_s_LoadTextureTGA, (void*)this);
	RegisterFileFormat("dtx", EOT_TEXTURE, "Dgle TeXtures images.", &_s_LoadTextureDTX, (void*)this);
	RegisterFileFormat("dmd", EOT_MODEL, "Dgle MoDel is a bunch of meshes with additionall attributes and material references.", &_s_LoadDMDFile, (void*)this);
	RegisterFileFormat("dft", EOT_BITMAP_FONT, "Dgle FonT bitmap 2D fonts.", &_s_LoadFontDFT, (void*)this);
	
	if (Core()->SoundEnabled())
		RegisterFileFormat("wav", EOT_SOUND_SAMPLE, "WAVe (PCM) uncompressed sound files.", &_s_LoadSoundWAV, (void*)this);

	//Create dummies

	_pBObjDummy	= new CBObjDummy();
	_pDefBmFntDummy	= new CBitmapFontDummy();
	_pDefSSmpDummy	= new CSSampleDummy();
	_pDefMusicDummy = new CMusicDummy();

	RegisterDefaultResource(EOT_SOUND_SAMPLE, (IEngineBaseObject*)_pDefSSmpDummy);
	RegisterDefaultResource(EOT_MUSIC, (IEngineBaseObject*)_pDefMusicDummy);

	//Create default texture

	const uint8 ubt_def_tex_dat[] = {
		231,120,23, 56 ,52 ,49, 0,0,
		56 ,52 ,49, 231,120,23, 0,0
	};

	if (!_CreateTexture(_pDefTex, &ubt_def_tex_dat[0], 2, 2, TDF_RGB8, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_NONE | TLF_COORDS_REPEAT)))
		LOG("Can't create default texture.", LT_FATAL);

	RegisterDefaultResource(EOT_TEXTURE, (IEngineBaseObject*)_pDefTex);

	//Create default material

	_pDefMaterial = new CMaterial(uiInstIdx);
	_pDefMaterial->SetDiffuseTexture(_pDefTex);

	RegisterDefaultResource(EOT_MATERIAL, (IEngineBaseObject*)_pDefMaterial);

	//Create default mesh
	
	const float def_mesh_vtx[] = {
	//vertices
	-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,
	0.5,-0.5,-0.5,-0.5,-0.5,0.5,0.5,-0.5,0.5,
	0.5,0.5,0.5,-0.5,0.5,0.5,-0.5,-0.5,-0.5,
	0.5,-0.5,-0.5,0.5,-0.5,0.5,-0.5,-0.5,0.5,
	0.5,-0.5,-0.5,0.5,0.5,-0.5,0.5,0.5,0.5,
	0.5,-0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,-0.5,
	-0.5,0.5,0.5,0.5,0.5,0.5,-0.5,0.5,-0.5,
	-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,
	//normals
	0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,
	0,0,1,0,0,1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,
	1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,
	0,1,0,0,1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,
	//textures
	1,1,1,0,0,0,0,1,0,1,1,1,1,0,
	0,0,0,1,1,1,1,0,0,0,0,1,1,1,
	1,0,0,0,0,1,1,1,1,0,0,0,0,1,
	1,1,1,0,0,0
	};

	const uint16 def_mesh_fs[] = {
	0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,
	10,11,8,12,13,14,14,15,12,16,17,
	18,18,19,16,20,21,22,22,23,20
	};

	uint8 ubt_mesh_data[sizeof(def_mesh_vtx) + sizeof(def_mesh_fs)]; 

	memcpy(ubt_mesh_data, def_mesh_vtx, sizeof(def_mesh_vtx));
	memcpy(ubt_mesh_data + sizeof(def_mesh_vtx), def_mesh_fs, sizeof(def_mesh_fs));

	if (!_CreateMesh(_pDefMesh, ubt_mesh_data, sizeof(ubt_mesh_data), _countof(def_mesh_vtx) / 8, _countof(def_mesh_fs) / 3, TPoint3(0.f, 0.f, 0.f), TVector3(0.5, 0.5, 0.5), (E_MESH_CREATION_FLAGS)(MCF_TEXTURE_COORDS_PRESENTED | MCF_NORMALS_PRESENTED), (E_MESH_MODEL_LOAD_FLAGS)RES_LOAD_DEFAULT))
		LOG("Can't create default mesh.", LT_FATAL);

	RegisterDefaultResource(EOT_MESH, (IEngineBaseObject*)_pDefMesh);

	_pDefModel = new CModel(InstIdx());
	_pDefModel->AddMesh(_pDefMesh);

	RegisterDefaultResource(EOT_MODEL, (IEngineBaseObject*)_pDefModel);

	//Create default font
	
#ifdef PLATFORM_WINDOWS
	CResFile *p_rfile = NULL;
	bool b_rfopened;

	_pDefBmpFnt = NULL;

	p_rfile = new CResFile(InstIdx(), IDR_FONT1);

	p_rfile->IsOpen(b_rfopened);

	if (!b_rfopened || !_LoadFontDFT((IFile*)p_rfile, _pDefBmpFnt))
		LOG("Can't create default font.", LT_FATAL);

	delete p_rfile;
#else
	_pDefBmpFnt = (IBitmapFont*)_pDefBmFntDummy;
#endif

	_pSysBmpFnt = _pDefBmpFnt;

	RegisterDefaultResource(EOT_BITMAP_FONT, (IEngineBaseObject*)_pDefBmpFnt);

	Core()->AddEventListener(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	LOG("Resource Manager Subsystem initialized.",LT_INFO);
}

CResourceManager::~CResourceManager()
{
	Console()->UnRegCom("rman_stats");
	Console()->UnRegCom("rman_list_file_formats");
	Console()->UnRegCom("rman_list_resources");
	
	Core()->RemoveEventListener(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	LOG("Resource Manager Subsystem finalized.",LT_INFO);
}

void CResourceManager::FreeAllResources()
{
	_vecFileFormats.clear();

	while (!_vecList.empty())
	{
		delete[] _vecList.begin()->pcName;
		_vecList.begin()->pObj->Free();
	}

	delete (CMesh*)_pDefMesh;
	delete (CModel*)_pDefModel;
	delete (CMaterial*)_pDefMaterial;
	delete (CTexture*)_pDefTex;
	delete (CBitmapFont*)_pDefBmpFnt;

	delete _pDefBmFntDummy;
	delete _pDefSSmpDummy;
	delete _pDefMusicDummy;
	delete _pBObjDummy;
}

DGLE_RESULT DGLE_API CResourceManager::RegisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj)
{
	if (eObjType == EOT_UNKNOWN)
		return E_INVALIDARG;

	_vecDefRes.push_back(TDefaultRes(eObjType, pObj));
	
	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::UnregisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj)
{
	for (size_t i = 0; i < _vecDefRes.size(); ++i)
		if (_vecDefRes[i].type == eObjType && _vecDefRes[i].pBaseObj == pObj)
		{
			_vecDefRes.erase(_vecDefRes.begin() + i);
			return S_OK;
		}

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CResourceManager::UnregisterFileFormat(const char* pcExtension)
{
	for (size_t i = 0; i < _vecFileFormats.size(); ++i)
		if (_vecFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			_vecFileFormats.erase(_vecFileFormats.begin()+i);
			return S_OK;
		}

	return E_INVALIDARG;
}

bool DGLE_API CResourceManager::_s_ConListFileFormats(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CResourceManager, "No parameters expected.");
		return false;
	}
	else
	{
		CON(CResourceManager, string("---Supported File Formats---\n" + PTHIS(CResourceManager)->_strFileFormatsDescs + "----------------------------").c_str());
		return true;
	}
}

bool DGLE_API CResourceManager::_s_ConListResources(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CResourceManager, "No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CResourceManager)->_ListResources();
		return true;
	}
}

DGLE_RESULT DGLE_API CResourceManager::GetRegisteredExtensions(char* pcTxt, uint &uiCharsCount)
{
	string exts;

	for (size_t i = 0; i < _vecFileFormats.size(); ++i)
		exts += _vecFileFormats[i].ext + ";";

	if (!pcTxt)
	{
		uiCharsCount = exts.size();
		return S_OK;
	}

	if (exts.size() >= uiCharsCount)
	{
		uiCharsCount = exts.size();
		if (uiCharsCount > 0)
			strcpy(pcTxt, "");
		return E_INVALIDARG;
	}

	strcpy(pcTxt, exts.c_str());

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::GetDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *&prObj)
{
	for (int i = (int)_vecDefRes.size() - 1; i > -1; --i)
		if (eObjType == _vecDefRes[i].type)
		{
			prObj = _vecDefRes[i].pBaseObj;
			return S_OK;
		}

	prObj = (IEngineBaseObject*)_pBObjDummy;

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CResourceManager::GetResourcesCount(uint &uiCount)
{
	uiCount = _vecList.size();
	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::GetResourceByName(const char *pcName, IEngineBaseObject *&prObj)
{
	prObj = NULL;

	if (!pcName || strlen(pcName) == 0)
		return E_INVALIDARG;

	uint32 hash = GetCRC32((uint8*)pcName, (uint32)strlen(pcName) * sizeof(char));

	for (size_t i = 0; i < _vecList.size(); ++i)
		if (_vecList[i].nameHash == hash)
		{
			prObj = _vecList[i].pObj;
			return S_OK;
		}

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CResourceManager::GetResourceByIndex(uint uiIdx, IEngineBaseObject *&prObj)
{
	prObj = NULL;

	if (uiIdx >= _vecList.size())
		return E_INVALIDARG;

	prObj = _vecList[uiIdx].pObj;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::GetResourceName(IEngineBaseObject *pObj, char *pcName, uint &uiCharsCount)
{
	for (size_t i = 0; i < _vecList.size(); ++i)
		if (_vecList[i].pObj == pObj)
			if (!pcName || uiCharsCount == 0)
			{
				uiCharsCount = strlen(_vecList[i].pcName) + 1;
				return S_OK;
			}
			else
			{
				if (uiCharsCount <= strlen(_vecList[i].pcName))
				{
					uiCharsCount = strlen(_vecList[i].pcName) + 1;
					strcpy(pcName, "");
					return E_INVALIDARG;
				}

				strcpy(pcName, _vecList[i].pcName);
				
				return S_OK;
			}

	return E_INVALIDARG;	
}

void CResourceManager::_AddResource(const char *pcName, IEngineBaseObject *pObj)
{
	if (pcName == NULL || strlen(pcName) == 0)
	{
		_vecList.push_back(TResource(("Res_" + IntToStr(_uiResIdxCounter)).c_str(), pObj));
		++_uiResIdxCounter;
	}
	else
		_vecList.push_back(TResource(pcName, pObj));
}

inline uint8 CResourceManager::_GetBytesPerPixel(E_TEXTURE_DATA_FORMAT &format)
{
	switch (format)
	{
	case TDF_BGR8:
	case TDF_RGB8:
	case TDF_DEPTH_COMPONENT24:
		return 3;
	case TDF_BGRA8: 
	case TDF_RGBA8:
	case TDF_DEPTH_COMPONENT32:
		return 4;
	case TDF_ALPHA8:
		return 1;
	case TDF_DXT1:
		return 8; //per block
	case TDF_DXT5:
		return 16; //per block
	default : return 0;
	}
}

#include "bcunpacker_incl.h"

uint CResourceManager::_GenerateDecompressedTextureData(const uint8 *pDataIn, uint8 *&prDataOut, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_TEXTURE_CREATION_FLAGS &eCreationFlags)
{
	const bool mips = (eCreationFlags & TCF_MIPMAPS_PRESENTED) != 0;
	uint data_size = 0, mip_width = uiWidth, mip_height = uiHeight;
	register bool widht_ready, height_ready;
	do
	{
		data_size += mip_width * mip_height;
		if (!(widht_ready = !mips || mip_width <= 1))
			mip_width >>= 1;
		if (!(height_ready = !mips || mip_height <= 1))
			mip_height >>= 1;
	}
	while (!(widht_ready && height_ready));

	if (format == TDF_DXT1)
	{
		data_size *= 3;
		prDataOut = new uint8[data_size];
	}
	else
		if (format == TDF_DXT5)
		{
			data_size *= 4;
			prDataOut = new uint8[data_size];
		}
		else
			return -1;

	uint data_offset = 0;
	mip_width = uiWidth, mip_height = uiHeight;
	do
	{
		uint data_size = mip_width * mip_height, compressed_data_size = ((mip_width + 3) / 4) * ((mip_height + 3) / 4);
		if (format == TDF_DXT1)
		{
			data_size *= 3;
			compressed_data_size *= 8;
			UnpackBC1(mip_width, mip_height, pDataIn, prDataOut + data_offset);
		}
		else
			if (format == TDF_DXT5)
			{
				data_size *= 4;
				compressed_data_size *= 16;
				UnpackBC3(mip_width, mip_height, pDataIn, prDataOut + data_offset);
			}
		data_offset += data_size;
		pDataIn += compressed_data_size;
		if (!(widht_ready = !mips || mip_width <= 1))
			mip_width >>= 1;
		if (!(height_ready = !mips || mip_height <= 1))
			mip_height >>= 1;
	}
	while (!(widht_ready && height_ready));

	if (format == TDF_DXT1)
	{
		format = TDF_RGB8;
		(int &)eCreationFlags |= TCF_PIXEL_ALIGNMENT_1;
	}
	else
		if(format == TDF_DXT5)
			format = TDF_RGBA8;

	return data_size;
}

bool CResourceManager::_SwabRB(uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment)
{
	if (format == TDF_BGR8)
	{
		format = TDF_RGB8;

		int align = 0;

		if (eAlignment == CRDA_ALIGNED_BY_4)
			align = GetDataAlignmentIncrement(uiWidth, 3, 4);

		uint ui_line_w = uiWidth * 3 + align;
			
		for(uint j = 0; j < uiHeight; ++j)
			for(uint i = 0; i < ui_line_w; ++i)
				if (i%3 == 0)
				{
					uint8
					ubt_tmp	= pData[ui_line_w * j + i];
					pData[ui_line_w * j + i] = pData[ui_line_w * j + i + 2];
					pData[ui_line_w * j + i + 2] = ubt_tmp;
				}
		return true;
	}
	else
		if (format == TDF_BGRA8)
		{
			format = TDF_RGBA8;

			for (uint i = 0; i < uiWidth*uiHeight; ++i)
			{
				uint8
				ubt_tmp = pData[i * 4];
				pData[i * 4] = pData[i * 4 + 2];
				pData[i * 4 + 2] = ubt_tmp;
			}

			return true;
		}
		else
			return false;
}

uint CResourceManager::_GenerateScaleImage(const uint8 * const pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, uint uiNewWidth, uint uiNewHeight, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment)
{
	if (format > TDF_BGRA8)
	{
		prDataOut = const_cast<uint8*>(pDataIn);
		return -1;
	}

	uint8 bytes_per_pix = _GetBytesPerPixel(format);

	int cur_align = 0, new_align = 0;

	if (eAlignment == CRDA_ALIGNED_BY_4)
	{
		cur_align = GetDataAlignmentIncrement(uiWidth, bytes_per_pix, 4);
		new_align = GetDataAlignmentIncrement(uiNewWidth, bytes_per_pix, 4);
	}

	uint data_size = uiNewHeight * (uiNewWidth * bytes_per_pix + new_align);

	if (prDataOut == NULL)
		prDataOut = new uint8[data_size];

	float x_factor = (float)uiNewWidth / (float)uiWidth, y_factor = (float)uiNewHeight / (float)uiHeight;
	
	for (int cy = 0; cy < (int)uiNewHeight; ++cy)
		for (int cx = 0; cx < (int)uiNewWidth; ++cx)
		{
			int pixel = cy * (uiNewWidth * bytes_per_pix + new_align) + cx * bytes_per_pix;
			int nearest_match = (int)(cy / y_factor) * (uiWidth * bytes_per_pix + cur_align) + (int)(cx / x_factor) * bytes_per_pix;
			for (int i = 0; i < bytes_per_pix; ++i)
				prDataOut[pixel + i] = pDataIn[nearest_match + i];
		}

	return data_size;
}

uint CResourceManager::_GenerateMipMapData(const uint8 * const pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment)
{
	if (format > TDF_BGRA8)
		return -1;

	int i_mipmaps = 0, max_side = max(uiWidth, uiHeight) / 2;

	//This loop is more correct because of NPOT textures than calculation like this: (int)(log((float)max(uiWidth, uiHeight))/log(2.f))
	while (max_side > 0)
	{
		max_side /= 2;
		++i_mipmaps;
	}

	uint8 bytes_per_pix = _GetBytesPerPixel(format);

	int i_cur_w = (int)uiWidth * 2, i_cur_h = (int)uiHeight * 2;
	uint data_size = 0, dat_offset = 0, dat_last_offset, cur_align = 0;

	for (int i = 0; i <= i_mipmaps; ++i)
	{
		i_cur_w /= 2; if (i_cur_w == 0) i_cur_w = 1;
		i_cur_h /= 2; if (i_cur_h == 0) i_cur_h = 1;

		if (eAlignment == CRDA_ALIGNED_BY_4)
			cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pix, 4);

		data_size += i_cur_h * (i_cur_w * bytes_per_pix + cur_align);
	}

	i_cur_w = (int)uiWidth, i_cur_h = (int)uiHeight;

	prDataOut = new uint8[data_size];

	if (eAlignment == CRDA_ALIGNED_BY_4)
		cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pix, 4);

	dat_last_offset = uiHeight * (uiWidth * bytes_per_pix + cur_align);

	memcpy(prDataOut, pDataIn, dat_last_offset);

	for (int i = 1; i <= i_mipmaps; ++i)
	{
		int prev_w = i_cur_w, prev_h = i_cur_h;

		i_cur_w /= 2; if (i_cur_w == 0) i_cur_w = 1;
		i_cur_h /= 2; if (i_cur_h == 0) i_cur_h = 1;

		uint8 *p_out = &prDataOut[dat_offset + dat_last_offset];
		
		uint new_dat_size = _GenerateScaleImage(&prDataOut[dat_offset], prev_w, prev_h, p_out, i_cur_w, i_cur_h, format, eAlignment);

		dat_offset += dat_last_offset;
		dat_last_offset = new_dat_size;
	}

	return data_size;
}

bool CResourceManager::_CreateTexture(ITexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags)
{
	if (eLoadFlags == RES_LOAD_DEFAULT)
		eLoadFlags = (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_BILINEAR | TLF_COORDS_REPEAT);

	uint8 *p_data_in = const_cast<uint8 *>(pData);

	bool b_is_compressed =	eDataFormat == TDF_DXT1 || eDataFormat == TDF_DXT5;

	bool need_delete_data_in = false;

	if (p_data_in == NULL)
	{
		if (b_is_compressed)
			return false;
		else
		{
			uint data_size = uiWidth * uiHeight * _GetBytesPerPixel(eDataFormat);
			p_data_in = new uint8 [data_size];
			memset(p_data_in, 0, data_size);
			need_delete_data_in = true;
		}
	}

	bool b_feature_supported;
	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_COMPRESSION, b_feature_supported);

	if (!b_feature_supported && b_is_compressed)
	{
		_GenerateDecompressedTextureData(pData, p_data_in, uiWidth, uiHeight, eDataFormat, eCreationFlags);
		b_is_compressed = false;
		need_delete_data_in = true;
	}

	bool b_been_swaped = false;

	_pCoreRenderer->IsFeatureSupported(CRFT_BGRA_DATA_FORMAT, b_feature_supported);

	if (!b_feature_supported && (eDataFormat == TDF_BGR8 || eDataFormat == TDF_BGRA8))
	{
		b_been_swaped = true;
		_SwabRB(p_data_in, uiWidth, uiHeight, eDataFormat,  eCreationFlags & TCF_PIXEL_ALIGNMENT_1 ? CRDA_ALIGNED_BY_1 : CRDA_ALIGNED_BY_4);
	}

	int i_new_w = uiWidth, i_new_h = uiHeight;

	if (!(eCreationFlags & TCF_MIPMAPS_PRESENTED) && !(b_is_compressed || eDataFormat == TDF_DEPTH_COMPONENT24 || eDataFormat == TDF_DEPTH_COMPONENT32))
	{
		if (eLoadFlags & TLF_DECREASE_QUALITY_MEDIUM)
		{
			i_new_w /= 2;
			i_new_h	/= 2;
		}
		else
			if (eLoadFlags & TLF_DECREASE_QUALITY_HIGH)
			{
				i_new_w /= 4;
				i_new_h	/= 4;
			}
	}

	int i_max_tex_res;

	_pCoreRenderer->GetDeviceMetric(CRMT_MAX_TEXTURE_RESOLUTION, i_max_tex_res);

	_pCoreRenderer->IsFeatureSupported(CRFT_NON_POWER_OF_TWO_TEXTURES, b_feature_supported);

	if (uiWidth > (uint)i_max_tex_res)
		i_new_w = i_max_tex_res;
	else
		if (!b_feature_supported)
			i_new_w = 1 << (int)floor((logf((float)uiWidth) / logf(2.f)) + 0.5f);

	if (uiHeight > (uint)i_max_tex_res)
		i_new_h = i_max_tex_res;
	else
		if (!b_feature_supported)
			i_new_h = 1 << (int)floor((logf((float)uiHeight) / logf(2.f)) + 0.5f);

	bool b_need_scale = (i_new_w != uiWidth || i_new_h != uiHeight);

	if (eCreationFlags & TCF_MIPMAPS_PRESENTED)
	{
		if (eCreationFlags & TLF_GENERATE_MIPMAPS)
			(int &)eCreationFlags &= ~TLF_GENERATE_MIPMAPS;

		if (b_need_scale || b_been_swaped)
		{
			(int &)eCreationFlags &= ~TCF_MIPMAPS_PRESENTED;
			(int &)eLoadFlags |= TLF_GENERATE_MIPMAPS;			
		}
	}

	if (b_need_scale)
	{
		if (eDataFormat == TDF_DEPTH_COMPONENT24 || eDataFormat == TDF_DEPTH_COMPONENT32)
		{
			if (need_delete_data_in)
				delete[] p_data_in;
			return false;
		}

		if (b_is_compressed)
		{
			_GenerateDecompressedTextureData(pData, p_data_in, uiWidth, uiHeight, eDataFormat, eCreationFlags);
			b_is_compressed = false;
			need_delete_data_in = true;
		}

		uint8 *p_out_dat = NULL;
		
		uint ret = _GenerateScaleImage(p_data_in, uiWidth, uiHeight, p_out_dat, i_new_w, i_new_h, eDataFormat, eCreationFlags & TCF_PIXEL_ALIGNMENT_1 ? CRDA_ALIGNED_BY_1 : CRDA_ALIGNED_BY_4);

		if (need_delete_data_in)
			delete[] p_data_in;
		else
			need_delete_data_in = true;
		
		if (ret == -1)
			return false;

		p_data_in = p_out_dat;
	}

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_MIPMAP_GENERATION, b_feature_supported);

	if (!b_feature_supported && eLoadFlags & TLF_GENERATE_MIPMAPS && !(eCreationFlags & TCF_MIPMAPS_PRESENTED))
	{
		uint8 *p_out_dat = NULL;
		
		if (b_is_compressed)
		{
			_GenerateDecompressedTextureData(pData, p_data_in, uiWidth, uiHeight, eDataFormat, eCreationFlags);
			b_is_compressed = false;
			need_delete_data_in = true;
		}

		int ret = _GenerateMipMapData(p_data_in, uiWidth, uiHeight, p_out_dat, eDataFormat, eCreationFlags & TCF_PIXEL_ALIGNMENT_1 ? CRDA_ALIGNED_BY_1 : CRDA_ALIGNED_BY_4);

		(int &)eCreationFlags |= TCF_MIPMAPS_PRESENTED;

		if (need_delete_data_in)
			delete[] p_data_in;
		else
			need_delete_data_in = true;
		
		if (ret == -1)
			return false;

		p_data_in = p_out_dat;
	}

	ICoreTexture *p_tex;
	
	if (FAILED(_pCoreRenderer->CreateTexture(p_tex, p_data_in, (uint)i_new_w, (uint)i_new_h, (eCreationFlags & TCF_MIPMAPS_PRESENTED) != 0, (eCreationFlags & TCF_PIXEL_ALIGNMENT_1) != 0 ? CRDA_ALIGNED_BY_1 : CRDA_ALIGNED_BY_4, eDataFormat, eLoadFlags)))
	{
		if (need_delete_data_in)
			delete[] p_data_in;

		return false;
	}

	if (need_delete_data_in)
			delete[] p_data_in;

	prTex = new CTexture(InstIdx(), p_tex, uiWidth, uiHeight);

	return true;
}

DGLE_RESULT DGLE_API CResourceManager::RegisterFileFormat(const char* pcExtension, E_ENGINE_OBJECT_TYPE eObjType, const char *pcDiscription, bool (DGLE_API *pLoadProc)(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter), void *pParameter)
{
	DGLE_RESULT res = S_OK;

	for (size_t i = 0; i<_vecFileFormats.size(); ++i)
		if (_vecFileFormats[i].ext == string(pcExtension) && _vecFileFormats[i].type == eObjType)
		{
			LOG("File format with extension \"" + string(pcExtension) + "\" was overrided.", LT_WARNING);
			res = S_FALSE;
		}

	TFileFormat tff;

	tff.ext = ToUpperCase(string(pcExtension));
	tff.type = eObjType;
	tff.discr = string(pcDiscription);
	tff.pLoadProc = pLoadProc;
	tff.pParameter = pParameter;

	_vecFileFormats.push_back(tff);

	_strFileFormatsDescs += string("- " + ToUpperCase(string(pcExtension)) + " " + string(pcDiscription) + "\n");

	return res;
}

bool CResourceManager::_LoadTextureTGA(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags)
{
#pragma pack(push, 1)
	 struct TTGAHeader
	 {
		uint8	ui8IdLength;
		uint8	ui8ColorMapType;
		uint8	ui8ImageType;
		uint8	ui8FirstEntryIndex[2];
		uint16	ui16ColorMapLength;
		uint8	ui8ColorMapEntrySize;
		uint8	ui8XOrigin[2];
		uint8	ui8YOrigin[2];
		uint16	ui16ImageWidth;
		uint16	ui16ImageHeight;
		uint8	ui8PixelDepth;
		uint8	ui8ImageDescriptor;
	};
#pragma pack(pop)

	TTGAHeader st_header;

	uint32 ui32_pos = 0;
	uint ui_read = 0;
	
	pFile->Seek(0, FSSF_BEGIN, ui32_pos);
	
	pFile->Read((void*)&st_header, sizeof(TTGAHeader), ui_read);

	if (st_header.ui8IdLength)
		pFile->Seek(st_header.ui8IdLength, FSSF_CURRENT, ui32_pos);

	if (st_header.ui8ColorMapType != 0)
		pFile->Seek((st_header.ui8ColorMapEntrySize / 8 * st_header.ui16ColorMapLength), FSSF_CURRENT, ui32_pos);

	int i_bytes_per_pixel = st_header.ui8PixelDepth / 8;
	
	int i_image_size = st_header.ui16ImageHeight * st_header.ui16ImageWidth * i_bytes_per_pixel;
	
	uint8 *p_data = new uint8[i_image_size];

	if (st_header.ui8ImageType == 2)
		pFile->Read(p_data, i_image_size, ui_read);
	else
		if (st_header.ui8ImageType == 10)
		{
			int i_current_byte = 0;

			while (i_current_byte < i_image_size)
			{
				uint8 ui8_chunkheader = 0;
					
				pFile->Read(&ui8_chunkheader, sizeof(uint8), ui_read);

				if (ui8_chunkheader < 128)
				{
					++ui8_chunkheader;
					pFile->Read(&p_data[i_current_byte], i_bytes_per_pixel * ui8_chunkheader, ui_read);
					i_current_byte += i_bytes_per_pixel * ui8_chunkheader;
				}
				else
				{
					ui8_chunkheader -= 127;
					int i_data_offset = i_current_byte;
					pFile->Read(&p_data[i_data_offset], i_bytes_per_pixel, ui_read);
					i_current_byte += i_bytes_per_pixel;

					for (int counter = 1; counter < ui8_chunkheader; ++counter)
					{
						for (int elementCounter = 0; elementCounter < i_bytes_per_pixel; ++elementCounter)
							p_data[i_current_byte + elementCounter] = p_data[i_data_offset + elementCounter];

						i_current_byte += i_bytes_per_pixel;
					}
				}
			}
		}
		else
		{
			LOG("Unsupported TGA file type.", LT_ERROR);
			return false; 
		}

	bool b_result = false;
	
	uint8 *p_out = new uint8[i_image_size];
	
	if (i_bytes_per_pixel == 3)
	{
		const uint8 *in = p_data; uint8 *out = p_out;
		const int line_w = 3 * st_header.ui16ImageWidth;
		bool flip = (st_header.ui8ImageDescriptor & 0x20) == 0;
		
		if (flip) p_out += line_w * st_header.ui16ImageHeight;

		for (int y = 0; y < st_header.ui16ImageHeight; ++y)
		{
			if (flip) out -= line_w;
		
			for (int x = 0; x < line_w; x += 3)
			{
				out[x] = in[x + 2];
				out[x + 1] = in[x + 1];
				out[x + 2] = in[x];
			}

			if (!flip) out += line_w;

			in += line_w;
		}
		
		E_TEXTURE_CREATION_FLAGS e_create_params = TCF_DEFAULT;
		
		if (st_header.ui16ImageWidth % 4 != 0)
			(int&)e_create_params |= TCF_PIXEL_ALIGNMENT_1;
		
		b_result = _CreateTexture(prTex, p_out, st_header.ui16ImageWidth, st_header.ui16ImageHeight, TDF_RGB8, e_create_params, eFlags);
	}
	else
		if (i_bytes_per_pixel == 4)
		{
			const int *in = (int*)p_data; int *out = (int*)p_out;
			const int *p = in;
			out += st_header.ui16ImageHeight * st_header.ui16ImageWidth;

			for (int y = 0; y < st_header.ui16ImageHeight; ++y)
			{
				for (int x = 0; x < st_header.ui16ImageWidth; ++x)
				{
					p = in + ((int)st_header.ui16ImageWidth - x - 1);
					--out;
					*out = *p;
				}
				
				in += (int)st_header.ui16ImageWidth;
			}

			b_result = _CreateTexture(prTex, p_out, st_header.ui16ImageWidth, st_header.ui16ImageHeight, TDF_BGRA8, TCF_DEFAULT, eFlags);
		}
		else
			LOG("Unsupported TGA format.", LT_ERROR);

	delete [] p_data;
	delete [] p_out;

	return b_result;
}

bool CResourceManager::_LoadTextureBMP(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags)
{
#pragma pack(push, 1)
	struct TBitmapFileHeader
	{
		uint16	ui16Type;
		uint32	ui32Size;
		uint16	ui16Reserved1;
		uint16	ui16Reserved2;
		uint32	ui32OffBits;
	};

	struct TBitmapInfoHeader
	{
		uint32	ui32Size;
		int32	i32Width;
		int32	i32Height;
		uint16	ui16Planes;
		uint16	ui16BitCount;
		uint32	ui32Compression;
		uint32	ui32SizeImage;
		int32	i32XPelsPerMeter;
		int32	i32YPelsPerMeter;
		uint32	ui32ClrUsed;
		uint32	ui32ClrImportant;
	};
#pragma pack(pop)

	TBitmapFileHeader st_file_header;
	TBitmapInfoHeader st_info_header;

	uint ui_read;

	pFile->Read((void*)&st_file_header, sizeof(TBitmapFileHeader), ui_read);
	pFile->Read((void*)&st_info_header, sizeof(TBitmapInfoHeader), ui_read);

	if (st_info_header.ui16BitCount != 24)
	{
		LOG("Only 24 bit BMP supported.", LT_ERROR);
		return false;
	}

	if (st_info_header.ui32ClrUsed > 0)
	{
		uint32 pos;
		pFile->Seek(st_info_header.ui32ClrUsed, FSSF_CURRENT, pos);
		
		if (ui_read != st_info_header.ui32ClrUsed)
		{
			LOG("Can't read BMP palette.", LT_ERROR);
			return false;
		}
	}

	uint ui_bitmap_length = st_info_header.ui32SizeImage;

	if (ui_bitmap_length == 0)
	{
		uint8 ui8_add_bts = GetDataAlignmentIncrement(st_info_header.i32Width, 3, 4);
		ui_bitmap_length = st_info_header.i32Height * (st_info_header.i32Width * 3 + ui8_add_bts);
	}

	uint8 *p_data = new uint8[ui_bitmap_length];

	pFile->Read((void*)p_data, ui_bitmap_length, ui_read);

	if (ui_read != ui_bitmap_length)
	{
		LOG("Error reading BMP data.", LT_ERROR);
		delete[] p_data;
		return false;
	}
	
	const int i_line_w = ui_bitmap_length / st_info_header.i32Height;
	
	uint8 *p_out = new uint8[ui_bitmap_length];
	
	for (int i = 0; i < st_info_header.i32Height; ++i)
		memcpy((void*)&p_out[i * i_line_w], (void*)&p_data[ui_bitmap_length - (i + 1) * i_line_w], i_line_w);

	delete[] p_data;

	const bool ret = _CreateTexture(prTex, p_out, st_info_header.i32Width, st_info_header.i32Height, TDF_BGR8, TCF_DEFAULT, eFlags);

	delete[] p_out;

	return ret;
}

bool CResourceManager::_LoadTextureDTX(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags)
{
	uint ui_read;

	char ftype[7];

	pFile->Read(ftype, 7, ui_read);

	if (ftype[0] != 'D' || ftype[1] != 'T' || ftype[2] != 'X')
	{
		LOG("Wrong DTX texture file.", LT_ERROR);
		return false;
	}

	if (ftype[4] != '1' || ftype[5] != '.' || ftype[6] != '0')
	{
		LOG("Incorrect DTX texture version.", LT_ERROR);
		return false;
	}

#pragma pack(push, 1)
	struct TDTXHeader
	{
		uint32	ui32Count;
		uint8	ui8Format;
		uint32	ui32Width;
		uint32	ui32Height;
		bool	bMip;
		uint32	ui32DataSize;
	};
#pragma pack(pop)

	TDTXHeader header;

	pFile->Read(&header, sizeof(TDTXHeader), ui_read);

	E_TEXTURE_DATA_FORMAT format;

	switch (header.ui8Format)
	{
	case 0: format = TDF_BGR8; break;
	case 1: format = TDF_BGRA8; break;
	case 2: format = TDF_DXT1; break;
	case 3: format = TDF_DXT5; break;
	default:
		LOG("Incorrect DTX data format.", LT_ERROR);
		return false;
	}

	E_TEXTURE_CREATION_FLAGS flags = TCF_DEFAULT;

	if (header.bMip)
		flags = TCF_MIPMAPS_PRESENTED;

	uint8 *data_in = new uint8[header.ui32DataSize];

	pFile->Read(data_in, header.ui32DataSize, ui_read);

	if (ui_read != header.ui32DataSize)
	{
		LOG("Error(s) while reading DTX texture data.", LT_ERROR);
		delete[] data_in;
		return false;
	}

	const bool ret = _CreateTexture(prTex, data_in, header.ui32Width, header.ui32Height, format, flags, eFlags);

	delete[] data_in;

	return ret;
}

bool CResourceManager::_CreateMesh(IMesh *&prMesh, const uint8 * const pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, const TPoint3 &stCenter, const TVector3 &stExtents, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags)
{
	if (!pData || uiNumVerts == 0)
	{
		prMesh = new CMesh(InstIdx(), NULL, stCenter, stExtents);
		return true;
	}

	bool normals = (eCreationFlags & MCF_NORMALS_PRESENTED) != 0,
		 textured = (eCreationFlags & MCF_TEXTURE_COORDS_PRESENTED) != 0,
		 tangents = (eCreationFlags & MCF_TANGENT_SPACE_PRESENTED) != 0;

	const uint8 face_size = uiNumVerts > 65535 ? sizeof(uint32) : sizeof(uint16);

	const uint vdata_size = (((1 + (int)normals) * uiNumVerts) * 3 + (textured * 2 + tangents * 6) * uiNumVerts) * sizeof(float),
		 idata_size = uiNumFaces * face_size * 3,
		 data_size = vdata_size + idata_size;

	if (uiDataSize != data_size || (uiNumFaces == 0 && uiNumVerts % 3 != 0))
	{
		LOG("Can't create mesh input data is incorrect.", LT_ERROR);
		return false;
	}

	ICoreGeometryBuffer *p_buff;
	TDrawDataDesc desc;

	desc.pData = const_cast<uint8 *>(pData);
	desc.bIndexBuffer32 = face_size == sizeof(uint32);
	
	if (uiNumFaces != 0)
		desc.pIndexBuffer = const_cast<uint8 *>(&pData[vdata_size]);
	
	if (eCreationFlags & MCF_VERTEX_DATA_INTERLEAVED)
	{
		uint offset = 3 * sizeof(float), interleave = (3 + (normals ? 3 : 0) + (textured ? 2 : 0) + (tangents ? 6 : 0)) * sizeof(float);
		
		desc.uiVertexStride = interleave;

		if (normals)
		{
			desc.uiNormalOffset = offset;
			desc.uiNormalStride = interleave;
			offset *= 2;
		}

		if (textured)
		{
			desc.uiTextureVertexOffset = offset;
			desc.uiTextureVertexStride = interleave;
			offset += 2 * sizeof(float);
		}

		if (tangents)
		{
			desc.uiTangentOffset = offset;
			desc.uiTangentStride = interleave;
			offset += 3 * sizeof(float);			

			desc.uiBinormalOffset = offset;
			desc.uiBinormalStride = interleave;
			offset += 3 * sizeof(float);
		}
	}
	else
	{
		uint offset = 3 * uiNumVerts * sizeof(float);

		if (normals)
		{
			desc.uiNormalOffset = offset;
			offset *= 2;
		}

		if (textured)
		{
			desc.uiTextureVertexOffset = offset;
			offset += 2 * uiNumVerts * sizeof(float);
		}

		if (tangents)
		{
			desc.uiTangentOffset = offset;
			offset += 3 * uiNumVerts * sizeof(float);
		
			desc.uiBinormalOffset = offset;
			offset += 3 * uiNumVerts * sizeof(float);
		}
	}

	if (FAILED(_pCoreRenderer->CreateGeometryBuffer(p_buff, desc, uiNumVerts, uiNumFaces * 3, CRDM_TRIANGLES,
		(eLoadFlags & MMLF_FORCE_SOFTWARE_BUFFER) ? CRBT_SOFTWARE : (eLoadFlags & MMLF_DYNAMIC_BUFFER ? CRBT_HARDWARE_DYNAMIC : CRBT_HARDWARE_STATIC))))
	{
		p_buff->Free();
		LOG("Can't create mesh geometry buffer.", LT_ERROR);
		return false;
	}

	prMesh = new CMesh(InstIdx(), p_buff, stCenter, stExtents);
	
	return true;
}

bool CResourceManager::_LoadDMDFile(IFile *pFile, IEngineBaseObject *&prObj, E_MESH_MODEL_LOAD_FLAGS eLoadFlags)
{
	uint ui_read;

	char ftype[11];

	pFile->Read(ftype, 11, ui_read);

	if (ftype[0] != 'D' || ftype[1] != 'M' || ftype[2] != 'D')
	{
		LOG("Wrong DMD file.", LT_ERROR);
		return false;
	}

	if (ftype[4] != '3' || ftype[5] != '.' || ftype[6] != '1')
	{
		LOG("Incorrect DMD file version.", LT_ERROR);
		return false;
	}

	E_DMD_HEADER_FLAGS ext_flags;
	pFile->Read(&ext_flags, sizeof(E_DMD_HEADER_FLAGS), ui_read);

	uint meshes_count;
	pFile->Read(&meshes_count, sizeof(uint), ui_read);

	if (meshes_count == 0)
	{
		LOG("Zero meshes in DMD file.", LT_ERROR);
		return false;
	}

	TPoint3 model_center;
	TVector3 model_extents;

	if (ext_flags & DHF_MODEL_AABB)
	{
		pFile->Read(&model_center.xyz, sizeof(TPoint3), ui_read);
		pFile->Read(&model_extents.xyz, sizeof(TVector3), ui_read);
	}

	bool ret = false;

	CModel *p_model = new CModel(InstIdx());

	for (uint i = 0; i < meshes_count; ++i)
	{

#pragma pack(push, 1)
		struct TMeshHeader
		{
			bool isTextured, haveTangentSpace;
			uint32 ui32VertsCount, ui32IdxsCount;
			TPoint3 stCenter;
			TVector3 stExtents;
		};
#pragma pack(pop)

		TMeshHeader header;

		pFile->Read(&header, sizeof(TMeshHeader), ui_read);

		const uint8 idx_size = header.ui32VertsCount > 65535 ? sizeof(uint32) : sizeof(uint16);
		const uint datasize = header.ui32VertsCount * sizeof(float) * 2 * 3 + header.ui32IdxsCount * idx_size + header.isTextured * header.ui32VertsCount * sizeof(float) * 2;

		uint8 *data_in = new uint8[datasize];

		pFile->Read(data_in, datasize, ui_read);

		if (ui_read != datasize)
		{
			LOG("Error(s) while reading DMD mesh data.", LT_ERROR);
			delete[] data_in;
			delete p_model;
			return false;
		}

		E_MESH_CREATION_FLAGS cr_flags = MCF_NORMALS_PRESENTED;

		if (header.isTextured)
			(int&)cr_flags |= MCF_TEXTURE_COORDS_PRESENTED;

		IMesh *p_mesh;

		ret = _CreateMesh(p_mesh, data_in, datasize, header.ui32VertsCount, header.ui32IdxsCount / 3, header.stCenter, header.stExtents, cr_flags, eLoadFlags);

		delete[] data_in;

		if (eLoadFlags & MMLF_FORCE_MODEL_TO_MESH)
		{
			delete p_model;
			prObj = p_mesh;
			return ret;
		}
		else
			if (!(ext_flags & DHF_MODEL_AABB) && i == meshes_count - 1)
				p_model->AddMesh(p_mesh);
			else
				p_model->AddMesh(p_mesh, model_center, model_extents);
	}

	prObj = p_model;

	return ret;
}

void CResourceManager::_s_GetObjTypeName(E_ENGINE_OBJECT_TYPE type, string &name)
{
	switch (type)
	{
	case EOT_TEXTURE: name = "Texture"; break;
	case EOT_MATERIAL: name = "Material"; break;
	case EOT_LIGHT: name = "Light"; break;
	case EOT_MESH: name = "Mesh"; break;
	case EOT_MODEL: name = "Model"; break;
	case EOT_BITMAP_FONT: name = "Bitmap Font"; break;
	case EOT_SOUND_SAMPLE: name = "Sound Sample"; break;
	case EOT_MUSIC: name = "Music"; break;
	default: name = "Other/Unknown";
	}
}

void CResourceManager::_ProfilerEventHandler() const
{
	if (_iProfilerState == 0)
		return;

	TColor4 color;
	Core()->RenderProfilerText("== Resource Man. Profiler ==", color);

	if (_iProfilerState > 0)
		Core()->RenderProfilerText(("Resources loaded: " + UIntToStr((uint)_vecList.size())).c_str(), color);

	if (_iProfilerState > 1)
	{
		uint cnt[_sc_EngObjTypeCount];
		memset(cnt, 0, _sc_EngObjTypeCount * sizeof(uint));

		for (size_t i = 0; i < _vecList.size(); ++i)
		{
			E_ENGINE_OBJECT_TYPE type;
			_vecList[i].pObj->GetType(type);
			++cnt[type];
		}

		Core()->RenderProfilerText("---- Resource Statistic ----", color);

		for (int i = 0; i < _sc_EngObjTypeCount; ++i)
		{
			string s;
			
			_s_GetObjTypeName((E_ENGINE_OBJECT_TYPE)i, s);

			s += ':' + UIntToStr(cnt[i]);

			Core()->RenderProfilerText(s.c_str(), color);
		}

		Core()->RenderProfilerText("----------------------------", color);
	}

}

void CResourceManager::_ListResources() const
{
	string res;

	for (size_t i = 0; i < _vecList.size(); ++i)
	{
		E_ENGINE_OBJECT_TYPE type;
		_vecList[i].pObj->GetType(type);
		string name;
		_s_GetObjTypeName(type, name);
		res += string(_vecList[i].pcName) + "[" + name + "]\n";
	}

	if(!res.empty())
		res.erase(res.size() - 1, res.size() - 1);

	Console()->Write(res);
}

bool CResourceManager::_LoadFontDFT(IFile *pFile, IBitmapFont *&prFnt)
{
	uint ui_read;

	char ftype[7];

	pFile->Read(&ftype[0], 7, ui_read);

	if (ftype[0] != 'D' || ftype[1] != 'F' || ftype[2] != 'T')
	{
		LOG("Wrong DFT font file.", LT_ERROR);
		return false;
	}

	if(ftype[4] != '1' || ftype[5] != '.' || ftype[6] != '1')
	{
		LOG("Incorrect font version.", LT_ERROR);
		return false;
	}

	CBitmapFont::TFontHeader header;

	pFile->Read(&header, sizeof(CBitmapFont::TFontHeader), ui_read);

	uint ui_data_size = header.texWidth * header.texHeight * header.bitDepth;
	
	uint8 *p_data = new uint8[ui_data_size];

	CBitmapFont::TCharBox chars[224];

	pFile->Read(chars, 224 * sizeof(CBitmapFont::TCharBox), ui_read);
	
	pFile->Read(p_data, ui_data_size, ui_read);

	if (ui_read != ui_data_size)
	{
		LOG("Error(s) while reading font data.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	E_TEXTURE_DATA_FORMAT format;

	switch (header.bitDepth)
	{
	case 1: format = TDF_ALPHA8; break;
	case 4: format = TDF_RGBA8; break;
	default:
		LOG("Wrong font bitdepth.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	ITexture *p_tex;

	if (!_CreateTexture((ITexture*&)p_tex, p_data, header.texWidth, header.texHeight, format, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_GENERATE_MIPMAPS | TLF_FILTERING_ANISOTROPIC | TLF_ANISOTROPY_4X | TLF_COORDS_REPEAT)))
	{
		LOG("Error(s) while loading font texture.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	prFnt = (IBitmapFont*) new CBitmapFont(InstIdx(), p_tex, header, chars);

	if (_pDefBmpFnt == _pDefBmFntDummy)
		_pDefBmpFnt = (IBitmapFont*)prFnt;

	delete[] p_data;

	return true;
}

bool CResourceManager::_CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 * const pData, uint32 ui32DataSize)
{
	if (!pData || ui32DataSize == 0 || (uiBitsPerSample != 8 && uiBitsPerSample != 16) || (uiSamplesPerSec != 11025 && uiSamplesPerSec != 22050 && uiSamplesPerSec != 44100))
		return false;

	uint8 * p_data = new uint8[ui32DataSize];
	memcpy(p_data, pData, ui32DataSize);

	prSndSample = (ISoundSample*)(new CSoundSample(InstIdx(), uiSamplesPerSec, uiBitsPerSample, bStereo, p_data, ui32DataSize));
	
	return true;
}

DGLE_RESULT DGLE_API CResourceManager::CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize, const char *pcName, bool bAddResource)
{
	DGLE_RESULT result = _CreateSound(prSndSample, uiSamplesPerSec, uiBitsPerSample, bStereo, pData, ui32DataSize) ? S_OK : E_FAIL;

	if (bAddResource)
	{
		if (result == S_OK) 
			_AddResource(pcName, (IEngineBaseObject*)prSndSample);
		else
			LOG("Error creating sound with name \"" + string(pcName) + "\".", LT_ERROR);
	}

	return result;
}

bool CResourceManager::_LoadSoundWAV(IFile *pFile, ISoundSample *&prSSample)
{
	const uint16 c_wave_format_pcm = 1;

#pragma pack(push, 1)
	struct TWaveFormatEx
	{
		uint16	ui16FormatTag;
		uint16	ui16Channels;
		uint32	ui32SamplesPerSec;
		uint32	ui32AvgBytesPerSec;
		uint16	ui16BlockAlign;
		uint16	ui16BitsPerSample;
	};
#pragma pack(pop)

	uint ui_read;
	uint32 ui32_tmp, ui32_pos;		

	pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);

	if (ui32_tmp != MAKEFOURCC('R','I','F','F'))
	{
		LOG("Can't find chunk \"RIFF\".", LT_ERROR);
		return false;
	}

	pFile->Seek(4, FSSF_CURRENT, ui32_pos);

	pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);

	if (ui32_tmp != MAKEFOURCC('W','A','V','E'))
	{
		LOG("Can't find chunk \"WAVE\".", LT_ERROR);
		return false;
	}

	pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);

	if (ui32_tmp != MAKEFOURCC('f','m','t',' '))
	{
		LOG("Can't find chunk \"fmt\".", LT_ERROR);
		return false;
	}

	int32 i32_l_size_fmt;

	pFile->Read(&i32_l_size_fmt, sizeof(int32), ui_read);

	if (i32_l_size_fmt == 0)
	{
		LOG("Can't find size of audio-information.", LT_ERROR);
		return false;
	}

	TWaveFormatEx st_format;

	pFile->Read(&st_format, sizeof(TWaveFormatEx), ui_read);
	
	if ((st_format.ui16Channels != 1 || st_format.ui16Channels != 2) &&
		(st_format.ui16BitsPerSample != 8 && st_format.ui16BitsPerSample != 16) || (st_format.ui32SamplesPerSec != 11025 && st_format.ui32SamplesPerSec != 22050 && st_format.ui32SamplesPerSec != 44100))
	{
		LOG("Unsupported sound data format. Only 8 or 16 bits per sample with 11025, 22050 or 44100 frequency formats are supported.", LT_ERROR);
		return false;
	}

	pFile->Seek(i32_l_size_fmt - sizeof(TWaveFormatEx), FSSF_CURRENT, ui32_pos);
	
	if (st_format.ui16FormatTag != c_wave_format_pcm)
	{
		LOG("Compressed files are not supported.", LT_ERROR);
		return false;
	}

	pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);

	if (ui32_tmp == MAKEFOURCC('f','a','c','t'))
	{
		uint32 ui32_dw_size_fact;
		pFile->Read(&ui32_dw_size_fact, sizeof(uint32), ui_read);
		pFile->Seek(ui32_dw_size_fact, FSSF_CURRENT, ui32_pos);
		pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);
	}												

	if (ui32_tmp != MAKEFOURCC('d','a','t','a'))		
	{
		LOG("Can't find chunk \"data\".", LT_ERROR);
		return false;
	}

	pFile->Read(&ui32_tmp, sizeof(uint32), ui_read);			
	
	uint8 *p_data= new uint8[ui32_tmp];
	
	pFile->Read(p_data, ui32_tmp, ui_read);

	if (ui_read != ui32_tmp)
	{
		LOG("Failed to read \"data\" chunk.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	prSSample = (ISoundSample*)(new CSoundSample(InstIdx(), st_format.ui32SamplesPerSec, st_format.ui16BitsPerSample, st_format.ui16Channels == 2, p_data, ui32_tmp));

	return true;
}

bool DGLE_API CResourceManager::_s_LoadTextureBMP(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	ITexture *ptex = NULL;
	const bool ret = PTHIS(CResourceManager)->_LoadTextureBMP(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if (ret) prObj = (IEngineBaseObject *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadTextureTGA(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	ITexture *ptex = NULL;
	const bool ret = PTHIS(CResourceManager)->_LoadTextureTGA(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if (ret) prObj = (IEngineBaseObject *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadTextureDTX(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	ITexture *ptex = NULL;
	const bool ret = PTHIS(CResourceManager)->_LoadTextureDTX(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if (ret) prObj = (IEngineBaseObject *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadDMDFile(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	IEngineBaseObject *pobj = NULL;
	const bool ret = PTHIS(CResourceManager)->_LoadDMDFile(pFile, pobj, (E_MESH_MODEL_LOAD_FLAGS)uiLoadFlags);
	if (ret) prObj = pobj;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadFontDFT(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	IBitmapFont *pfnt = NULL;
	const bool ret = PTHIS(CResourceManager)->_LoadFontDFT(pFile, pfnt);
	if (ret) prObj = (IEngineBaseObject *&)pfnt;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadSoundWAV(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter)
{
	ISoundSample *ps = NULL;
	
	const bool ret = PTHIS(CResourceManager)->_LoadSoundWAV(pFile, ps);
	
	if (ret)
	{
		if (uiLoadFlags & SSLF_LOAD_AS_MUSIC)
			prObj = new CMusic(PTHIS(CResourceManager)->InstIdx(), ps);
		else
			prObj = (IEngineBaseObject *&)ps;
	}

	return ret;
}

void DGLE_API CResourceManager::_s_ProfilerEventHandler(void *pParameter, IBaseEvent *pEvent)
{
	PTHIS(CResourceManager)->_ProfilerEventHandler();
}

DGLE_RESULT DGLE_API CResourceManager::CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResource)
{
	const DGLE_RESULT result = _CreateTexture(prTex, pData, uiWidth, uiHeight, eDataFormat, eCreationFlags, eLoadFlags) ? S_OK : S_FALSE;
	
	if (bAddResource)
	{
		if (result == S_OK) 
			_AddResource(pcName, (IEngineBaseObject*)prTex);
		else
			LOG("Error creating texture with name \"" + string(pcName) + "\".", LT_ERROR);
	}

	return result;
}

DGLE_RESULT DGLE_API CResourceManager::CreateMaterial(IMaterial *&prMaterial, const char *pcName, bool bAddResource)
{
	prMaterial = new CMaterial(InstIdx());

	if (bAddResource)
		_AddResource(pcName, (IEngineBaseObject*)prMaterial);

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::CreateLight(ILight *&prLight, const char *pcName, bool bAddResource)
{
	prLight = new CLight(InstIdx());

	if (bAddResource)
		_AddResource(pcName, (IEngineBaseObject*)prLight);

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResource)
{
	DGLE_RESULT result;

	if (!pData || uiNumVerts == 0 || uiDataSize == 0)
		result = _CreateMesh(prMesh, NULL, 0, 0, 0, TPoint3(), TVector3(), eCreationFlags, eLoadFlags) ? S_OK : E_FAIL;
	else
	{
		if (uiNumFaces == 0 && uiNumVerts % 3 != 0)
			return E_INVALIDARG;

		TPoint3 min_dem((numeric_limits<float>::infinity)(), (numeric_limits<float>::infinity)(), (numeric_limits<float>::infinity)()),
				max_dem(-(numeric_limits<float>::infinity)(), -(numeric_limits<float>::infinity)(), -(numeric_limits<float>::infinity)());
	
		uint stride = 3 * sizeof(float);

		if (eCreationFlags & MCF_VERTEX_DATA_INTERLEAVED)
			stride = (3 + 3 * (eCreationFlags & MCF_NORMALS_PRESENTED) + 2 * (eCreationFlags & MCF_TEXTURE_COORDS_PRESENTED) + 6 * (eCreationFlags & MCF_TANGENT_SPACE_PRESENTED)) * sizeof(float);

		for (uint i = 0; i < uiNumVerts; ++i)
		{
			const TPoint3 * const p = reinterpret_cast<TPoint3 *>(const_cast<uint8 *>(&pData[stride * i]));

			max_dem.x = max(p->x, max_dem.x);
			min_dem.x = min(p->x, min_dem.x);

			max_dem.y = max(p->y, max_dem.y);
			min_dem.y = min(p->y, min_dem.y);

			max_dem.z = max(p->z, max_dem.z);
			min_dem.z = min(p->z, min_dem.z);
		}

		result = _CreateMesh(prMesh, pData, uiDataSize, uiNumVerts, uiNumFaces, min_dem + (max_dem - min_dem) / 2.f, (max_dem - min_dem) / 2.f, eCreationFlags, eLoadFlags) ? S_OK : E_FAIL;
	}

	if (bAddResource)
	{
		if (result == S_OK)
			_AddResource(pcName, (IEngineBaseObject*)prMesh);
		else
			LOG("Error creating mesh with name \"" + string(pcName) + "\".", LT_ERROR);
	}

	return result;
}

DGLE_RESULT DGLE_API CResourceManager::CreateModel(IModel *&prModel, const char *pcName, bool bAddResource)
{
	prModel = new CModel(InstIdx());

	if (bAddResource)
		_AddResource(pcName, (IEngineBaseObject*)prModel);
	
	return S_OK;
}

inline uint CResourceManager::_GetFileFormatLoaderIdx(const char *pcFileName, E_ENGINE_OBJECT_TYPE eObjType, uint uiLoadFlags, IEngineBaseObject *&prObj)
{
	uint ret = -1;

	if (pcFileName == NULL)
	{
		prObj = (IEngineBaseObject*&)_pBObjDummy;
		return ret;
	}

	string file_ext = ToUpperCase(GetFileExt(pcFileName));

	for (size_t i = 0; i<_vecFileFormats.size(); ++i)
		if ((eObjType == EOT_UNKNOWN || _vecFileFormats[i].type == eObjType) && _vecFileFormats[i].ext == file_ext)
		{
			ret = (uint)i;
			break;
		}

	if (ret == -1)
		prObj = (IEngineBaseObject*&)_pBObjDummy;
	else
		switch (_vecFileFormats[ret].type)
		{
		case EOT_SOUND_SAMPLE:
			GetDefaultResource(uiLoadFlags & SSLF_LOAD_AS_MUSIC ? EOT_MUSIC : EOT_SOUND_SAMPLE, prObj);
			break;

		case EOT_MODEL:
			GetDefaultResource(uiLoadFlags & MMLF_FORCE_MODEL_TO_MESH ? EOT_MESH : EOT_MODEL, prObj);
			break;

		default:
			GetDefaultResource(_vecFileFormats[ret].type, prObj);
		}		

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::GetExtensionType(const char *pcExtension, E_ENGINE_OBJECT_TYPE &eType)
{
	for (size_t i = 0; i < _vecFileFormats.size(); ++i)
		if (_vecFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			eType = _vecFileFormats[i].type;
			return S_OK;
		}

	eType = EOT_UNKNOWN;

	return S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::GetExtensionDescription(const char *pcExtension, char *pcTxt, uint &uiCharsCount)
{
	for (size_t i = 0; i < _vecFileFormats.size(); ++i)
		if (_vecFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			if (!pcTxt)
			{
				uiCharsCount = _vecFileFormats[i].discr.size() + 1;
				return S_OK;
			}

			if (_vecFileFormats[i].discr.size() >= uiCharsCount)
			{
				uiCharsCount = _vecFileFormats[i].discr.size() + 1;
				if (uiCharsCount > 0)
					strcpy(pcTxt, "");
				return E_INVALIDARG;
			}

			strcpy(pcTxt, _vecFileFormats[i].discr.c_str());

			return S_OK;
		}

	if (uiCharsCount > 0)
		strcpy(pcTxt, "");

	return S_FALSE;
}

inline DGLE_RESULT CResourceManager::_Load(const char *pcFileName, IFile *pFile, uint uiFFIdx, IEngineBaseObject *&prObj, uint uiLoadFlags, const char *pcName)
{
	if (uiFFIdx == -1)
	{
		Console()->Write("Use \"rman_list_file_formats\" console command to list supported file formats.");
		LOG("Resource Manager can't load file \"" + string(pcFileName) + "\" because of unknown file extension \"" + ToUpperCase(GetFileExt(pcFileName)) + "\".", LT_FATAL);
		return E_FAIL;
	}

	bool opened;
	
	pFile->IsOpen(opened);
	
	if (!opened)
	{
		LOG("Can't open file \"" + string(pcFileName) + "\", probably file doesn't exists.", LT_WARNING);
		return S_FALSE;
	}

	uint32 size = 0;
	
	pFile->GetSize(size);
	
	if (size <= 0)
	{
		LOG("File \"" + string(pcFileName) + "\" size is zero, nothing to read.", LT_WARNING);
		return S_FALSE;
	}

	bool ret = (*_vecFileFormats[uiFFIdx].pLoadProc)(pFile, prObj, uiLoadFlags, _vecFileFormats[uiFFIdx].pParameter);

	if (!ret)
		LOG("Error(s) while loading file \"" + string(pcFileName) + "\".", LT_ERROR);
	else
		_vecList.push_back(TResource(pcName == NULL || strlen(pcName) == 0 ? pcFileName : pcName, prObj));

	return ret ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::Load(const char *pcFileName, IEngineBaseObject *&prObj, uint uiLoadFlags, const char *pcName)
{
	if (pcFileName == NULL || strlen(pcFileName) == 0)
	{
		prObj = NULL;
		return E_INVALIDARG;
	}

	uint ff_idx = _GetFileFormatLoaderIdx(pcFileName, EOT_UNKNOWN, uiLoadFlags, prObj);

	IFile *p_file = NULL;

	if (Core()->pMainFS()->LoadFile(pcFileName, p_file) != S_OK)
		return S_FALSE;

	if (!p_file)
	{
		LOG("Error openning file \"" + string(pcFileName) + "\".", LT_ERROR);
		return E_ABORT;
	}

	DGLE_RESULT ret = _Load(pcFileName, p_file, ff_idx, prObj, uiLoadFlags, pcName);

	p_file->Free();

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::LoadEx(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, const char *pcName)
{
	if (pFile == NULL)
	{
		prObj = NULL;
		return E_INVALIDARG;
	}

	char *name_path;
	uint name_length, path_length;

	if (FAILED(pFile->GetName(NULL, name_length)))
	{
		prObj = (IEngineBaseObject*&)_pBObjDummy;
		LOG("Can't get file name length.", LT_ERROR);
		return E_ABORT;
	}
	
	if (FAILED(pFile->GetPath(NULL, path_length)))
	{
		prObj = (IEngineBaseObject*&)_pBObjDummy;
		LOG("Can't get file path length.", LT_ERROR);
		return E_ABORT;
	}

	name_path = new char [name_length + path_length];

	char * const name = name_path, * const path = name_path + name_length;

	if (FAILED(pFile->GetName(name, name_length)) || FAILED(pFile->GetPath(path, path_length)))
	{
		prObj = (IEngineBaseObject*&)_pBObjDummy;
		LOG("Can't get filename of IFile.", LT_ERROR);
		return E_ABORT;
	}

	string file_name(path);

	if (file_name.length() > 0 && file_name[file_name.length() - 1] != '\\')
		file_name += '\\';

	file_name += string(name);

	DGLE_RESULT ret = _Load(file_name.c_str(), pFile, _GetFileFormatLoaderIdx(name, EOT_UNKNOWN, uiLoadFlags, prObj), prObj, uiLoadFlags, pcName);

	delete[] name_path;

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::FreeResource(IEngineBaseObject *&prObj)
{
	E_ENGINE_OBJECT_TYPE obj_type;
	prObj->GetType(obj_type);
	prObj->Free();
	return GetDefaultResource(obj_type, prObj);
}

DGLE_RESULT DGLE_API CResourceManager::AddResource(const char *pcName, IEngineBaseObject *pObj)
{
	for (size_t i = 0; i < _vecList.size(); ++i)
		if (_vecList[i].pObj == pObj)
			return E_INVALIDARG;

	_AddResource(pcName, pObj);

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::RemoveResource(IEngineBaseObject *pObj, bool &bCanDelete)
{
	for (size_t i = 0; i < _vecList.size(); ++i)
		if (_vecList[i].pObj == pObj)
		{
			bCanDelete = true;
			_vecList.erase(_vecList.begin() + i);
			return S_OK;
		}

	bCanDelete = true;

	for (size_t i = 0; i < _vecDefRes.size(); ++i)
		if (pObj == _vecDefRes[i].pBaseObj)
			bCanDelete = false;
	
	return S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::GetType(E_ENGINE_SUB_SYSTEM &eSubSysType)
{
	eSubSysType = ESS_RESOURCE_MANAGER;
	return S_OK;
}