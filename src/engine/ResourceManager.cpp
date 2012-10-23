/**
\author		Korotkov Andrey aka DRON
\date		30.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "ResourceManager.h"
//#include "Material.h"
//#include "shader.h"
//#include "Core.h"
#include "MainFS.h"
#include "ResFile.h"
//#include "RenderGL.h"
#include "Texture.h"
#include "Font.h"
//#include "MusicMCI.h"
//#include "Mesh.h"
//#include "MultiMesh.h"

#ifdef PLATFORM_WINDOWS
#include "..\..\build\windows\engine\resource.h"
#endif

using namespace std;

//#define _CRTDBG_MAP_ALLOC
//#include <debag.h>

//Dummy Classes//

class CBObjDummy: public IEngBaseObj
{
public:
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENG_OBJ_TYPE &eObjType){eObjType = EOT_UNKNOWN; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType){uiObjUnknownType = 0; return S_OK;}

	IDGLE_BASE_IMPLEMENTATION(IEngBaseObj)
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
	DGLE_RESULT DGLE_API SetSpeed(int iSpeed){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetSpeed(int &iSpeed){iSpeed = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos){uiPos = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetLength(uint &uiLength){uiLength = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable){bStreamable = false; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Unaquire(){delete this; return S_OK;}

	IDGLE_BASE_IMPLEMENTATION(ISoundChannel)
};

class CSSampleDummy: public ISoundSample
{
public:
	DGLE_RESULT DGLE_API Play(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags){pSndChnl = (ISoundChannel*)new CSoundChannelDummy; return E_NOTIMPL;}
	
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENG_OBJ_TYPE &eObjType){eObjType = EOT_SOUND_SAMPLE; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType){uiObjUnknownType = -1; return S_FALSE;}

	IDGLE_BASE_IMPLEMENTATION1(ISoundSample, IEngBaseObj)
};

class CBitmapFontDummy: public IBitmapFont
{
public:
	DGLE_RESULT DGLE_API GetTexture(ITexture *&prTexture){prTexture = NULL; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API SetScale(const float &fScale){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetScale(float &fScale){fScale = 1.f; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight){uiWidth = 0; uiHeight = 0; return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2DRect(const TRectF &stRect, const char *pcTxt, const TColor4 &stColor){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Draw3D(const char *pcTxt){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API Free(){return E_NOTIMPL;}
	DGLE_RESULT DGLE_API GetType(E_ENG_OBJ_TYPE &eObjType){eObjType = EOT_BITMAP_FONT; return S_OK;}
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType) {uiObjUnknownType = -1; return S_FALSE;}

	IDGLE_BASE_IMPLEMENTATION1(IBitmapFont, IEngBaseObj)
};

//CSoundSample//

class CSoundSample: public ISoundSample
{
	CResourceManager *_pResMan;
	ISound *_pSound;
	uint _uiSamplesPerSec;
	uint _uiBitsPerSample;
	bool _bStereo;
	const uint8 *_pData;
	uint32 _ui32DataSize;

public:

	CSoundSample(CResourceManager *pResMan, ISound *pSound, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize):
	_pResMan(pResMan), _pSound(pSound), _uiSamplesPerSec(uiSamplesPerSec), _uiBitsPerSample(uiBitsPerSample),
	_bStereo(bStereo), _pData(pData), _ui32DataSize(ui32DataSize)
	{}

	~CSoundSample()
	{
		_pSound->ReleaseChannelsByData(_pData);
		delete[] _pData;
	}

	DGLE_RESULT DGLE_API Play()
	{
		if (!_pSound)
			return E_ABORT;

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
		if (!_pSound)
		{
			pSndChnl = NULL;
			return E_ABORT;
		}

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
	
	DGLE_RESULT DGLE_API GetType(E_ENG_OBJ_TYPE &eObjType)
	{
		eObjType = EOT_SOUND_SAMPLE;
		return S_OK;
	}
	
	DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType)
	{
		uiObjUnknownType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION1(ISoundSample, IEngBaseObj)
};


//CResource Manager//

CResourceManager::CResourceManager(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_iProfilerState(0)
{
	Core()->pMainWindow()->GetWindowHandle(_stWnd);
	
	_pCoreRenderer = Core()->pCoreRenderer();

	Console()->RegComValue("rman_stats", "Displays resource manager subsystems statistic.", &_iProfilerState, 0, 2);
	Console()->RegComProc("rman_list_file_formats", "Lists all file formats registered in the Resource Manager.", &_s_ConListFileFormats, (void*)this);

	RegisterFileFormat("bmp", EOT_TEXTURE, "BitMaP images.", &_s_LoadTextureBMP, (void*)this);
	RegisterFileFormat("tga", EOT_TEXTURE, "truevision TarGA images.", &_s_LoadTextureTGA, (void*)this);
	RegisterFileFormat("dtx", EOT_TEXTURE, "Dgle TeXtures images.", &_s_LoadTextureDTX, (void*)this);
	RegisterFileFormat("dmd", EOT_MESH, "Dgle MoDel static and animated meshes.", &_s_LoadDMDFile, (void*)this);
	RegisterFileFormat("dft", EOT_BITMAP_FONT, "Dgle FonT bitmap 2D fonts.", &_s_LoadFontDFT, (void*)this);
	
	if (Core()->SoundEnabled())
		RegisterFileFormat("wav", EOT_SOUND_SAMPLE, "WAVe (PCM) uncompressed sound files.", &_s_LoadSoundWAV, (void*)this);

	//Create dummies

	_pBObjDummy	= new CBObjDummy();
	_pDefBmFntDummy	= new CBitmapFontDummy();
	_pDefSSmpDummy	= new CSSampleDummy();

	RegisterDefaultResource(EOT_SOUND_SAMPLE, (IEngBaseObj*)_pDefSSmpDummy);

	//Create default texture

	const uint8 ubt_def_tex_dat[] = {
		231,120,23, 56 ,52 ,49, 0,0,
		56 ,52 ,49, 231,120,23, 0,0
	};

	if (!_CreateTexture(_pDefTex, &ubt_def_tex_dat[0], 2, 2, TDF_RGB8, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_NONE | TLF_COORDS_REPEAT)))
		LOG("Can't create default texture.", LT_FATAL);

	RegisterDefaultResource(EOT_TEXTURE, (IEngBaseObj*)_pDefTex);

	//Create default material

//	if (FAILED(CreateMaterial(_pDefMaterial, "", false)) || FAILED(_pDefMaterial->SetDiffuseTexture(_pDefTex)))
//		LOG("Can't create default material.", LT_FATAL);

	RegisterDefaultResource(EOT_MATERIAL, (IEngBaseObj*)_pDefMaterial);

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

	if (!_CreateMesh(_pDefMesh, ubt_mesh_data, sizeof(ubt_mesh_data), _countof(def_mesh_vtx)/8, _countof(def_mesh_fs)/3, (E_MESH_CREATION_FLAGS)(MCF_TEXTURE_COORDS_PRESENTED | MCF_NORMALS_PRESENTED), (E_MESH_LOAD_FLAGS)RES_LOAD_DEFAULT))
		LOG("Can't create default mesh.", LT_FATAL);

//	((CMesh*)_pDefMesh)->_stExtents = TPoint3(0.5,0.5,0.5);

	RegisterDefaultResource(EOT_MESH, (IEngBaseObj*)_pDefMesh);

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

	RegisterDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj*)_pDefBmpFnt);

	Core()->AddEventListner(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	LOG("Resource Manager Subsystem initialized.",LT_INFO);
}

CResourceManager::~CResourceManager()
{
	Console()->UnRegCom("rman_stats");
	Console()->UnRegCom("rman_list_file_formats");

	Core()->RemoveEventListner(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	LOG("Resource Manager Subsystem finalized.",LT_INFO);
}

void CResourceManager::FreeAllResources()
{
	_clFileFormats.clear();

	while (!_resList.empty())
		_resList.begin()->pObj->Free();

	//delete ((CMesh*)_pDefMesh);
	//delete (CBaseMaterial*)_pDefMaterial;
	delete ((CTexture*)_pDefTex);
	delete ((CBitmapFont*)_pDefBmpFnt);

	delete _pDefBmFntDummy;
	delete _pDefSSmpDummy;
	delete _pBObjDummy;
}

DGLE_RESULT DGLE_API CResourceManager::RegisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj)
{
	if (eObjType == EOT_UNKNOWN)
		return E_INVALIDARG;

	_defRes.push_back(TDefaultRes(eObjType, pObj));
	
	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::UnregisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj)
{
	for (size_t i = 0; i < _defRes.size(); ++i)
		if (_defRes[i].type == eObjType && _defRes[i].pBaseObj == pObj)
		{
			_defRes.erase(_defRes.begin() + i);
			return S_OK;
		}

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CResourceManager::UnregisterFileFormat(const char* pcExtension)
{
	for (size_t i = 0; i < _clFileFormats.size(); ++i)
		if (_clFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			_clFileFormats.erase(_clFileFormats.begin()+i);
			return S_OK;
		}

	return E_INVALIDARG;
}

void DGLE_API CResourceManager::_s_ConListFileFormats(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		CON(CResourceManager, "No parametrs expected.");
	else 
		CON(CResourceManager, string("---Supported File Formats---\n" + PTHIS(CResourceManager)->_strFileFormatsDescs + "----------------------------").c_str());
}

DGLE_RESULT DGLE_API CResourceManager::GetRegisteredExtensions(char* pcTxt, uint &uiCharsCount)
{
	string exts;

	for (size_t i = 0; i < _clFileFormats.size(); ++i)
		exts += _clFileFormats[i].ext + ";";

	if (!pcTxt)
	{
		uiCharsCount = exts.size();
		return S_OK;
	}

	if (exts.size() >= uiCharsCount)
	{
		uiCharsCount = exts.size();
		strcpy(pcTxt, "");
		return E_INVALIDARG;
	}

	strcpy(pcTxt, exts.c_str());

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::GetDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *&prObj)
{
	for (int i = (int)_defRes.size() - 1; i > -1; --i)
		if (eObjType == _defRes[i].type)
		{
			prObj = _defRes[i].pBaseObj;
			return S_OK;
		}

	prObj = (IEngBaseObj*)_pBObjDummy;

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CResourceManager::GetResourceByFileName(const char *pcFileName, IEngBaseObj *&prObj)
{
	prObj = NULL;

	if (!pcFileName || strlen(pcFileName) == 0)
		return E_INVALIDARG;

	uint32 hash = GetCRC32((uint8*)pcFileName, (uint32)strlen(pcFileName)*sizeof(char));

	for (size_t i = 0; i < _resList.size(); ++i)
		if (_resList[i].nameHash == hash)
		{
			prObj = _resList[i].pObj;
			return S_OK;
		}

	return E_INVALIDARG;
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

		uint ui_line_w = uiWidth*3 + align;
			
		for(uint j = 0; j < uiHeight; ++j)
			for(uint i = 0; i < ui_line_w; ++i)
				if (i%3 == 0)
				{
					uint8
					ubt_tmp	= pData[ui_line_w*j + i];
					pData[ui_line_w*j + i] = pData[ui_line_w*j + i + 2];
					pData[ui_line_w*j + i + 2] = ubt_tmp;
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
				ubt_tmp = pData[i*4];
				pData[i*4] = pData[i*4 + 2];
				pData[i*4 + 2] = ubt_tmp;
			}

			return true;
		}
		else
			return false;
}

uint CResourceManager::_GenerateScaleImage(const uint8 *pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, uint uiNewWidth, uint uiNewHeight, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment)
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

	uint data_size = uiNewHeight*(uiNewWidth*bytes_per_pix + new_align);

	if (prDataOut == NULL)
		prDataOut = new uint8[data_size];

	float x_factor = (float)uiNewWidth/(float)uiWidth, y_factor = (float)uiNewHeight/(float)uiHeight;
	
	for (int cy = 0; cy < (int)uiNewHeight; ++cy)
		for (int cx = 0; cx < (int)uiNewWidth; ++cx)
		{
			int pixel = cy*(uiNewWidth*bytes_per_pix + new_align) + cx*bytes_per_pix;
			int nearest_match = (int)(cy/y_factor)*(uiWidth*bytes_per_pix + cur_align) + (int)(cx/x_factor)*bytes_per_pix;
			for (int i = 0; i < bytes_per_pix; ++i)
				prDataOut[pixel + i] = pDataIn[nearest_match + i];
		}

	return data_size;
}

uint CResourceManager::_GenerateMipMapData(const uint8 *pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment)
{
	if (format > TDF_BGRA8)
		return -1;

	int i_mipmaps = 0, max_side = max(uiWidth, uiHeight)/2;

	//This loop is more correct because of NPOT textures than calculation like this: (int)(log((float)max(uiWidth, uiHeight))/log(2.f))
	while (max_side > 0)
	{
		max_side /= 2;
		++i_mipmaps;
	}

	uint8 bytes_per_pix = _GetBytesPerPixel(format);

	int i_cur_w = (int)uiWidth*2, i_cur_h = (int)uiHeight*2;
	uint data_size = 0, dat_offset = 0, dat_last_offset, cur_align = 0;

	for (int i = 0; i <= i_mipmaps; ++i)
	{
		i_cur_w /= 2; if (i_cur_w == 0) i_cur_w = 1;
		i_cur_h /= 2; if (i_cur_h == 0) i_cur_h = 1;

		if (eAlignment == CRDA_ALIGNED_BY_4)
			cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pix, 4);

		data_size += i_cur_h*(i_cur_w*bytes_per_pix + cur_align);
	}

	i_cur_w = (int)uiWidth, i_cur_h = (int)uiHeight;

	prDataOut = new uint8[data_size];

	if (eAlignment == CRDA_ALIGNED_BY_4)
		cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pix, 4);

	dat_last_offset = uiHeight*(uiWidth*bytes_per_pix + cur_align);

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

bool CResourceManager::_CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags)
{
	if (FAILED(_pCoreRenderer->MakeCurrent()))
		return false;

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
			uint data_size = uiWidth*uiHeight*_GetBytesPerPixel(eDataFormat);
			p_data_in = new uint8 [data_size];
			memset(p_data_in, 0, data_size);
			need_delete_data_in = true;
		}
	}

	bool b_feature_supported;
	_pCoreRenderer->IsFeatureSupported(CRSF_TEXTURE_COMPRESSION, b_feature_supported);

	if (!b_feature_supported && b_is_compressed)
	{
		_GenerateDecompressedTextureData(pData, p_data_in, uiWidth, uiHeight, eDataFormat, eCreationFlags);
		b_is_compressed = false;
		need_delete_data_in = true;
	}

	bool b_been_swaped = false;

	_pCoreRenderer->IsFeatureSupported(CRSF_BGRA_DATA_FORMAT, b_feature_supported);

	if (!b_feature_supported && (eDataFormat == TDF_BGR8 || eDataFormat == TDF_BGRA8))
	{
		b_been_swaped = true;
		_SwabRB(p_data_in, uiWidth, uiHeight, eDataFormat,  eCreationFlags & TCF_PIXEL_ALIGNMENT_1 ? CRDA_ALIGNED_BY_1 : CRDA_ALIGNED_BY_4);
	}

	int i_new_w = uiWidth, i_new_h = uiHeight;

	if (!(eCreationFlags & TCF_MIPMAPS_PRESENTED) && !(b_is_compressed || eDataFormat == TDF_DEPTH_COMPONENT24))
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

	_pCoreRenderer->IsFeatureSupported(CRSF_NON_POWER_OF_TWO_TEXTURES, b_feature_supported);

	if (uiWidth > (uint)i_max_tex_res)
		i_new_w = i_max_tex_res;
	else
		if (!b_feature_supported)
			i_new_w = 1 << (int)floor((log((double)uiWidth)/log(2.f)) + 0.5f);

	if (uiHeight > (uint)i_max_tex_res)
		i_new_h = i_max_tex_res;
	else
		if (!b_feature_supported)
			i_new_h = 1 << (int)floor((log((double)uiHeight)/log(2.f)) + 0.5f);

	bool b_need_scale = (i_new_w != uiWidth || i_new_h != uiHeight);

	if (eCreationFlags & TCF_MIPMAPS_PRESENTED && (b_need_scale || b_been_swaped))
	{
		if (!(eCreationFlags & TLF_GENERATE_MIPMAPS))
			(int &)eLoadFlags |= TLF_GENERATE_MIPMAPS;
					
		(int &)eCreationFlags &= ~TCF_MIPMAPS_PRESENTED;
	}

	if (b_need_scale)
	{
		if (eDataFormat == TDF_DEPTH_COMPONENT24)
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

	_pCoreRenderer->IsFeatureSupported(CRSF_TEXTURE_MIPMAP_GENERATION, b_feature_supported);

	if (!b_feature_supported && eLoadFlags & TLF_GENERATE_MIPMAPS)
	{
		uint8 *p_out_dat = NULL;
		
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

DGLE_RESULT DGLE_API CResourceManager::RegisterFileFormat(const char* pcExtension, E_ENG_OBJ_TYPE eObjType, const char *pcDiscription, bool (DGLE_API *pLoadProc)(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr), void *pParametr)
{
	for (size_t i = 0; i<_clFileFormats.size(); ++i)
		if (_clFileFormats[i].ext == string(pcExtension) && _clFileFormats[i].type == eObjType)
			LOG("File format with extension \"" + string(pcExtension) + "\" was overrided.", LT_WARNING);

	TFileFormat tff;

	tff.ext = ToUpperCase(string(pcExtension));
	tff.type = eObjType;
	tff.discr = string(pcDiscription);
	tff.pLoadProc = pLoadProc;
	tff.pParametr = pParametr;

	_clFileFormats.push_back(tff);

	_strFileFormatsDescs += string("- " + ToUpperCase(string(pcExtension)) + " " + string(pcDiscription) + "\n");

	return S_OK;
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
		pFile->Seek((st_header.ui8ColorMapEntrySize/8 * st_header.ui16ColorMapLength), FSSF_CURRENT, ui32_pos);

	int i_bytes_per_pixel = st_header.ui8PixelDepth/8;
	
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
						for (int elementCounter=0; elementCounter < i_bytes_per_pixel; ++elementCounter)
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
				out[x]		= in[x + 2];
				out[x + 1]	= in[x + 1];
				out[x + 2]	= in[x];
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
		ui_bitmap_length = st_info_header.i32Height*(st_info_header.i32Width*3 + ui8_add_bts);
	}

	uint8 *p_data = new uint8[ui_bitmap_length];

	pFile->Read((void*)p_data, ui_bitmap_length, ui_read);

	if (ui_read != ui_bitmap_length)
	{
		LOG("Error reading BMP data.", LT_ERROR);
		delete[] p_data;
		return false;
	}
	
	int i_line_w = ui_bitmap_length/st_info_header.i32Height;
	
	uint8 *p_out = new uint8[ui_bitmap_length];
	
	for (int i = 0; i < st_info_header.i32Height; ++i)
		memcpy((void*)&p_out[i*i_line_w], (void*)&p_data[ui_bitmap_length - (i + 1)*i_line_w], i_line_w);

	delete[] p_data;

	bool ret = _CreateTexture(prTex, p_out, st_info_header.i32Width, st_info_header.i32Height, TDF_BGR8, TCF_DEFAULT, eFlags);

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
		uint32	uiCount;
		uint8	ui8Format;
		uint32	uiWidth;
		uint32	uiHeight;
		bool	bMip;
		uint32	uiDataSize;
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

	uint8 *data_in = new uint8[header.uiDataSize];

	pFile->Read(data_in, header.uiDataSize, ui_read);

	if (ui_read != header.uiDataSize)
	{
		LOG("Error(s) while reading DTX texture data.", LT_ERROR);
		delete[] data_in;
		return false;
	}

	bool ret = _CreateTexture(prTex, data_in, header.uiWidth, header.uiHeight, format, flags, eFlags);

	delete[] data_in;

	return ret;
}

bool CResourceManager::_CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags)
{
	if (FAILED(_pCoreRenderer->MakeCurrent()))
		return false;

	/*
	CMesh *p_mesh = new CMesh(InstIdx());

	p_mesh->_bTextureCoords = (eCreationFlags & MCF_TEXTURE_COORDS_PRESENTED) != 0;
	p_mesh->_bTangentSpace	= (eCreationFlags & MCF_TANGENT_SPACE_PRESENTED) != 0;
	p_mesh->_uiNumVerts		= uiNumVerts;
	p_mesh->_uiNumFaces		= uiNumFaces;

	ubyte face_size = uiNumVerts > 65535 ? sizeof(uint32) : sizeof(uint16);

	uint data_size = (p_mesh->_uiNumVerts + (eCreationFlags & MCF_NORMALS_PRESENTED)*p_mesh->_uiNumVerts)*sizeof(float)*3 +
					  p_mesh->_uiNumFaces*face_size*3 +
					 (p_mesh->_bTextureCoords*2 + p_mesh->_bTangentSpace*6)*p_mesh->_uiNumVerts*sizeof(float);

	if(uiDataSize != data_size)
	{
		p_mesh->Free();
		LOG("Can't create mesh input data is incorrect.",LT_ERROR);
		return false;
	}

	if(eCreationFlags & MCF_NORMALS_PRESENTED)
	{
		p_mesh->_pData = new ubyte[data_size];
		memcpy(p_mesh->_pData, pubtData, data_size);
	}
	else
	{
		uint vertices_size = p_mesh->_uiNumVerts*sizeof(float)*3;

		p_mesh->_pData = new ubyte[data_size + vertices_size];

		memcpy(p_mesh->_pData, pubtData, vertices_size);
		ZeroMemory((void*)&p_mesh->_pData[vertices_size], vertices_size);
		memcpy((void*)&p_mesh->_pData[vertices_size*2], (void*)&pubtData[vertices_size], uiDataSize - vertices_size);

		eCreationFlags = (E_MESH_CREATION_FLAGS)(eCreationFlags | MCF_NORMALS_PRESENTED);
	}

	p_mesh->_uiAllDataSize	= data_size;
	p_mesh->_uiFaceDataSize	= face_size*p_mesh->_uiNumFaces*3;
	p_mesh->_eMeshCrFlags	= eCreationFlags;
	p_mesh->_eMeshLdFlags	= eLoadFlags;

	if(eLoadFlags & MLF_CALCULATE_NORMALS)
		p_mesh->_CalculateNormals();

	if(p_mesh->_bTextureCoords && eLoadFlags & MLF_CALCULATE_TANGENT_SPACE)
		p_mesh->_CalculateTangentSpace();

	if(!(eLoadFlags & MLF_FORCE_NO_VBO))
		p_mesh->WriteToVBO();

	prMesh = (IMesh*)p_mesh;
	*/
	return true;
}

bool CResourceManager::_LoadDMDFile(IFile *pFile, IEngBaseObj *&prObj, E_MESH_LOAD_FLAGS eLoadFlags)
{
	uint ui_read;

	char ftype[11];

	pFile->Read(ftype, 11, ui_read);

	if (ftype[0] != 'D' || ftype[1] != 'M' || ftype[2] != 'D')
	{
		LOG("Wrong DMD mesh file.", LT_ERROR);
		return false;
	}

	if (ftype[4] != '3' || ftype[5] != '.' || ftype[6] != '0')
	{
		LOG("Incorrect DMD mesh version.", LT_ERROR);
		return false;
	}

#pragma pack(push, 1)
	struct TDMDHeader
	{
		uint32	uiFlags;
		bool	bTexCoords;
		uint32	uiNumVerts;
		uint32	uiNumFaces;
	};
#pragma pack(pop)

	TDMDHeader header;

	pFile->Read(&header, sizeof(TDMDHeader), ui_read);

	uint8 face_size = header.uiNumVerts > 65535 ? sizeof(uint32) : sizeof(uint16);

	uint datasize = (header.uiNumVerts*sizeof(float)*2 + header.uiNumFaces*face_size)*3 + header.bTexCoords*header.uiNumVerts*sizeof(float)*2;

	bool b_frame_animated = (header.uiFlags & 0x00000001/*MULTI_MESH*/);

	uint ui_mcount = 1;
/*
	CMesh *p_cur_mesh = NULL;
	CMultiMesh *p_cur_multi_mesh = NULL;

	if (b_frame_animated)
	{
		p_cur_multi_mesh = new CMultiMesh(InstIdx());
		pFile->Read(&ui_mcount, sizeof(uint), ui_read);
	}

	bool ret = false;

	for (uint i = 0; i < ui_mcount; ++i)
	{
		float points[6];
		pFile->Read(&points, sizeof(float)*6, ui_read);

		uint8 *data_in = new uint8[datasize];

		pFile->Read(data_in, datasize, ui_read);

		if (ui_read != datasize)
		{
			LOG("Error(s) while reading DMD mesh data.", LT_ERROR);
			delete[] data_in;
			return false;
		}

		E_MESH_CREATION_FLAGS cr_flags = MCF_NORMALS_PRESENTED;

		if (header.bTexCoords)
			(int&)cr_flags |= MCF_TEXTURE_COORDS_PRESENTED;

		ret = _CreateMesh((IMesh *&)p_cur_mesh, data_in, datasize, header.uiNumVerts, header.uiNumFaces, cr_flags, eLoadFlags);

		if (ret)
		{
			p_cur_mesh->_stCenter	= TPoint3(points[0], points[1], points[2]);
			p_cur_mesh->_stExtents	= TPoint3(points[3], points[4], points[5]);
		}

		delete[] data_in;

		if (b_frame_animated)
			p_cur_multi_mesh->_clMeshs.push_back(p_cur_mesh);
	}

	if (b_frame_animated)
	{
		if (ret)
			prObj = (IEngBaseObj*)p_cur_multi_mesh;
		else
			delete p_cur_multi_mesh;
	}
	else
	{
		if (ret)
			prObj = (IEngBaseObj*)p_cur_mesh;
		else
			delete p_cur_mesh;
	}

	return ret;*/return true;
}

void CResourceManager::_ProfilerEventHandler() const
{
	if (_iProfilerState == 0)
		return;

	TColor4 color;
	Core()->RenderProfilerTxt("== Resource Man. Profiler ==", color);

	if (_iProfilerState > 0)
		Core()->RenderProfilerTxt(("Resources loaded: " + UIntToStr((uint)_resList.size())).c_str(), color);

	if (_iProfilerState > 1)
	{
		const uint c_size = EOT_GUI_FORMS + 1;
		uint cnt[c_size];
		memset(cnt, 0, c_size*sizeof(uint));

		for (size_t i = 0; i < _resList.size(); ++i)
		{
			E_ENG_OBJ_TYPE type;
			_resList[i].pObj->GetType(type);
			++cnt[type];
		}

		Core()->RenderProfilerTxt("---- Resource Statistic ----", color);

		for (int i = 0; i < c_size; ++i)
		{
			string s;
			
			switch (i)
			{
			case EOT_UNKNOWN: s = "Unknown:"; break;
			case EOT_TEXTURE: s = "Texture:"; break;
			case EOT_MATERIAL: s = "Material:"; break;
			case EOT_MESH: s = "Mesh:"; break;
			case EOT_MODEL: s = "Model:"; break;
			case EOT_BITMAP_FONT: s = "Bitmap Font:"; break;
			case EOT_PARTICLE_EFFECT: s = "Particle Effect:"; break;
			case EOT_SOUND_SAMPLE: s = "Sound Sample:"; break;
			case EOT_SPRITE: s = "Sprite:"; break;
			case EOT_GUI_FORMS: s = "GUI Forms:"; break;
			default: s = "Other/Unknown:";
			}

			s += UIntToStr(cnt[i]);

			Core()->RenderProfilerTxt(s.c_str(), color);
		}

		Core()->RenderProfilerTxt("----------------------------", color);
	}

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

	uint ui_data_size = header.iTexWidth*header.iTexHeight*header.ubBitdepth;
	
	uint8 *p_data = new uint8[ui_data_size];

	CBitmapFont::TCharBox chars[224];

	pFile->Read(chars, 224*sizeof(CBitmapFont::TCharBox), ui_read);
	
	pFile->Read(p_data, ui_data_size, ui_read);

	if (ui_read != ui_data_size)
	{
		LOG("Error(s) while reading font data.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	E_TEXTURE_DATA_FORMAT format;

	switch (header.ubBitdepth)
	{
	case 1: format = TDF_ALPHA8; break;
	case 4: format = TDF_RGBA8; break;
	default:
		LOG("Wrong font bitdepth.", LT_ERROR);
		delete[] p_data;
		return false;
	}

	ITexture *p_tex;

	if (!_CreateTexture((ITexture*&)p_tex, p_data, header.iTexWidth, header.iTexHeight, format, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_GENERATE_MIPMAPS | TLF_FILTERING_ANISOTROPIC | TLF_ANISOTROPY_4X | TLF_COORDS_REPEAT)))
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

bool CResourceManager::_CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize)
{
	if (!pData || ui32DataSize == 0 || (uiBitsPerSample != 8 && uiBitsPerSample != 16) || (uiSamplesPerSec != 22050 && uiSamplesPerSec != 44100))
		return false;

	uint8 * p_data = new uint8[ui32DataSize];
	memcpy(p_data, pData, ui32DataSize);

	prSndSample = (ISoundSample*)(new CSoundSample(this, Core()->pSound(), uiSamplesPerSec, uiBitsPerSample, bStereo, p_data, ui32DataSize));
	
	return true;
}

DGLE_RESULT DGLE_API CResourceManager::CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize, const char *pcName, bool bAddResourse)
{
	DGLE_RESULT result = _CreateSound(prSndSample, uiSamplesPerSec, uiBitsPerSample, bStereo, pData, ui32DataSize) ? S_OK : E_FAIL;

	if (bAddResourse)
	{
		if (result == S_OK) 
			_resList.push_back(TResource(pcName, (IEngBaseObj*)prSndSample));
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
		uint	uiSamplesPerSec;
		uint	uiAvgBytesPerSec;
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
	
	if ((st_format.ui16Channels != 1 || st_format.ui16Channels != 2) && (st_format.ui16BitsPerSample != 8 && st_format.ui16BitsPerSample != 16) || (st_format.uiSamplesPerSec != 22050 && st_format.uiSamplesPerSec != 44100))
	{
		LOG("Unsupported sound data format. Only 8 or 16 bits per sample with 22050 or 44100 frequency formats are supported.", LT_ERROR);
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

	prSSample = (ISoundSample*)(new CSoundSample(this, Core()->pSound(), st_format.uiSamplesPerSec, st_format.ui16BitsPerSample, st_format.ui16Channels == 2, p_data, ui32_tmp));

	return true;
}

bool DGLE_API CResourceManager::_s_LoadTextureBMP(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadTextureBMP(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if(ret) prObj = (IEngBaseObj *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadTextureTGA(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadTextureTGA(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if(ret) prObj = (IEngBaseObj *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadTextureDTX(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadTextureDTX(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);
	if(ret) prObj = (IEngBaseObj *&)ptex;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadDMDFile(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	IEngBaseObj *pobj = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadDMDFile(pFile, pobj, (E_MESH_LOAD_FLAGS)uiLoadFlags);
	if(ret) prObj = pobj;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadFontDFT(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	IBitmapFont *pfnt = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadFontDFT(pFile, pfnt);
	if(ret) prObj = (IEngBaseObj *&)pfnt;
	return ret;
}

bool DGLE_API CResourceManager::_s_LoadSoundWAV(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr)
{
	ISoundSample *ps = NULL;
	bool ret = PTHIS(CResourceManager)->_LoadSoundWAV(pFile, ps);
	if(ret) prObj = (IEngBaseObj *&)ps;
	return ret;
}

void DGLE_API CResourceManager::_s_ProfilerEventHandler(void *pParametr, IBaseEvent *pEvent)
{
	PTHIS(CResourceManager)->_ProfilerEventHandler();
}

DGLE_RESULT DGLE_API CResourceManager::CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResourse)
{
	DGLE_RESULT result = _CreateTexture(prTex, pData, uiWidth, uiHeight, eDataFormat, eCreationFlags, eLoadFlags) ? S_OK : S_FALSE;
	
	if (bAddResourse)
	{
		if (result == S_OK) 
			_resList.push_back(TResource(pcName, (IEngBaseObj*)prTex));
		else
			LOG("Error creating texture with name \"" + string(pcName) + "\".", LT_ERROR);
	}

	return result;
}

DGLE_RESULT DGLE_API CResourceManager::CreateMaterial(IMaterial *&prMaterial, const char *pcName, bool bAddResourse)
{
	//ToDo
	return E_FAIL;
}

DGLE_RESULT DGLE_API CResourceManager::CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResourse)
{
	DGLE_RESULT result = _CreateMesh(prMesh, pData, uiDataSize, uiNumVerts, uiNumFaces, eCreationFlags, eLoadFlags) ? S_OK : E_FAIL;

	if (bAddResourse)
	{
		if (result == S_OK) 
			_resList.push_back(TResource(pcName, (IEngBaseObj*)prMesh));
		else
			LOG("Error creating mesh with name \"" + string(pcName) + "\".", LT_ERROR);
	}

	return result;
}

inline uint CResourceManager::_GetFFIdx(const char *pcFileName, E_ENG_OBJ_TYPE eObjType, IEngBaseObj *&prObj)
{
	uint ret = -1;

	if (pcFileName == NULL)
	{
		prObj = (IEngBaseObj*&)_pBObjDummy;
		return ret;
	}

	string file_ext = ToUpperCase(GetFileExt(pcFileName));

	for (size_t i = 0; i<_clFileFormats.size(); ++i)
		if ((eObjType == EOT_UNKNOWN || _clFileFormats[i].type == eObjType) && _clFileFormats[i].ext == file_ext)
		{
			ret = (uint)i;
			break;
		}

	if (ret==-1)
		prObj = (IEngBaseObj*&)_pBObjDummy;
	else
		GetDefaultResource(_clFileFormats[ret].type, prObj);

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::GetExtensionType(const char *pcExtension, E_ENG_OBJ_TYPE &eType)
{
	for (size_t i = 0; i < _clFileFormats.size(); ++i)
		if (_clFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			eType = _clFileFormats[i].type;
			return S_OK;
		}

	eType = EOT_UNKNOWN;

	return S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::GetExtensionDescription(const char *pcExtension, char *pcTxt, uint &uiCharsCount)
{
	for (size_t i = 0; i < _clFileFormats.size(); ++i)
		if (_clFileFormats[i].ext == ToUpperCase(string(pcExtension)))
		{
			if (!pcTxt)
			{
				uiCharsCount = _clFileFormats[i].discr.size() + 1;
				return S_OK;
			}

			if (_clFileFormats[i].discr.size() >= uiCharsCount)
			{
				uiCharsCount = _clFileFormats[i].discr.size() + 1;
				strcpy(pcTxt, "");
				return E_INVALIDARG;
			}

			strcpy(pcTxt, _clFileFormats[i].discr.c_str());

			return S_OK;
		}

	strcpy(pcTxt, "");

	return S_FALSE;
}

inline DGLE_RESULT CResourceManager::_Load(const char *pcFileName, IFile *pFile, uint uiFFIdx, IEngBaseObj *&prObj, uint uiLoadFlags)
{
	if (uiFFIdx == -1)
	{
		Console()->Write("Use \"rman_list_reged_fformats\" console command to list supported file formats.");
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

	bool ret = (*_clFileFormats[uiFFIdx].pLoadProc)(pFile, prObj, uiLoadFlags, _clFileFormats[uiFFIdx].pParametr);

	if (!ret)
		LOG("Error(s) while loading file \"" + string(pcFileName) + "\".", LT_ERROR);
	else
		_resList.push_back(TResource(pcFileName, prObj));

	return ret ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::Load(const char *pcFileName, IEngBaseObj *&prObj, uint uiLoadFlags)
{
	if (pcFileName == NULL || strlen(pcFileName) == 0)
	{
		prObj = NULL;
		return E_INVALIDARG;
	}

	uint ff_id = _GetFFIdx(pcFileName, EOT_UNKNOWN, prObj);

	IFile *p_file = NULL;

	if (Core()->pMainFS()->LoadFile(pcFileName, p_file) != S_OK)
		return S_FALSE;

	if (!p_file)
	{
		LOG("Error openning file \"" + string(pcFileName) + "\".", LT_ERROR);
		return E_ABORT;
	}

	DGLE_RESULT ret = _Load(pcFileName, p_file, ff_id, prObj, uiLoadFlags);

	p_file->Free();

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::Load2(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags)
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
		prObj = (IEngBaseObj*&)_pBObjDummy;
		LOG("Can't get file name length.", LT_ERROR);
		return E_ABORT;
	}
	
	if (FAILED(pFile->GetPath(NULL, path_length)))
	{
		prObj = (IEngBaseObj*&)_pBObjDummy;
		LOG("Can't get file path length.", LT_ERROR);
		return E_ABORT;
	}

	name_path = new char [name_length + path_length];

	char * const name = name_path, * const path = name_path + name_length;

	if (FAILED(pFile->GetName(name, name_length)) || FAILED(pFile->GetPath(path, path_length)))
	{
		prObj = (IEngBaseObj*&)_pBObjDummy;
		LOG("Can't get filename of IFile.", LT_ERROR);
		return E_ABORT;
	}

	string file_name(path);

	if (file_name.length() > 0 && file_name[file_name.length() - 1] != '\\')
		file_name += '\\';

	file_name += string(name);

	DGLE_RESULT ret = _Load(file_name.c_str(), pFile, _GetFFIdx(name, EOT_UNKNOWN, prObj), prObj, uiLoadFlags);

	delete[] name_path;

	return ret;
}

DGLE_RESULT DGLE_API CResourceManager::FreeResource(IEngBaseObj *&prObj)
{
	E_ENG_OBJ_TYPE obj_type;
	prObj->GetType(obj_type);
	prObj->Free();
	return GetDefaultResource(obj_type, prObj);
}

DGLE_RESULT DGLE_API CResourceManager::AddResource(const char *pcName, IEngBaseObj *pObj)
{
	for (size_t i = 0; i < _resList.size(); ++i)
		if (_resList[i].pObj == pObj)
			return E_INVALIDARG;

	_resList.push_back(TResource(pcName, pObj));

	return S_OK;
}

DGLE_RESULT DGLE_API CResourceManager::RemoveResource(IEngBaseObj *pObj, bool &bCanDelete)
{
	for (size_t i = 0; i < _resList.size(); ++i)
		if (_resList[i].pObj == pObj)
		{
			bCanDelete = true;
			_resList.erase(_resList.begin() + i);
			return S_OK;
		}

	bCanDelete = true;

	for (size_t i = 0; i < _defRes.size(); ++i)
		if (pObj == _defRes[i].pBaseObj)
			bCanDelete = false;
	
	return S_FALSE;
}

DGLE_RESULT DGLE_API CResourceManager::GetType(E_ENGINE_SUB_SYSTEM &eSubSysType)
{
	eSubSysType = ESS_RESOURCE_MANAGER;
	return S_OK;
}