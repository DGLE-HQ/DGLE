/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CBObjDummy;
class CBitmapFontDummy;
class CSSampleDummy;
class CMusicDummy;

struct TFileFormat
{
	std::string ext;
	std::string discr;
	E_ENGINE_OBJECT_TYPE type;
	void *pParameter;
	bool (DGLE_API *pLoadProc)(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);
};

struct TDefaultRes
{
	E_ENGINE_OBJECT_TYPE type;
	IEngineBaseObject *pBaseObj;
	TDefaultRes(E_ENGINE_OBJECT_TYPE _type, IEngineBaseObject *_pObj):type(_type), pBaseObj(_pObj){}
};

struct TResource
{
	char *pcName;
	uint32 nameHash;
	IEngineBaseObject *pObj;
	
	TResource(const char *pcFullName, IEngineBaseObject *pObject):
	pObj(pObject)
	{
		if (strlen(pcFullName) == 0)
		{
			pcName = NULL;
			nameHash = 0;
		}
		else
		{
			pcName = new char[strlen(pcFullName) + 1];
			strcpy(pcName, pcFullName);
			nameHash = GetCRC32((uint8*)pcFullName, (uint32)strlen(pcFullName) * sizeof(char));
		}
	}
};

class CResourceManager : public CInstancedObj, public IResourceManager
{
	static const uint _sc_EngObjTypeCount = EOT_EMPTY + 1;
	static void _s_GetObjTypeName(E_ENGINE_OBJECT_TYPE type, std::string &name);

	TWindowHandle			_stWnd;
	int						_iProfilerState;

	ICoreRenderer			*_pCoreRenderer;

	std::string				_strFileFormatsDescs;
	std::vector<TFileFormat>_clFileFormats;

	std::vector<TResource>	_resList;
	std::vector<TDefaultRes>_defRes;

	ITexture				*_pDefTex;
	IMaterial				*_pDefMaterial;
	IBitmapFont				*_pDefBmpFnt;
	IMesh					*_pDefMesh;
	IModel					*_pDefModel;

	CBObjDummy				*_pBObjDummy;
	CBitmapFontDummy		*_pDefBmFntDummy;
	CSSampleDummy			*_pDefSSmpDummy;
	CMusicDummy				*_pDefMusicDummy;

	inline DGLE_RESULT _Load(const char *pcFileName, IFile *pFile, uint uiFFIdx, IEngineBaseObject *&prObj, uint uiLoadFlags);
	inline uint _GetFileFormatLoaderIdx(const char *pcFileName, E_ENGINE_OBJECT_TYPE eObjType, uint uiLoadFlags, IEngineBaseObject *&prObj);
	inline uint8 _GetBytesPerPixel(E_TEXTURE_DATA_FORMAT &format);
	uint _GenerateDecompressedTextureData(const uint8 * pDataIn, uint8 *&prDataOut, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_TEXTURE_CREATION_FLAGS &eCreationFlags);
	bool _SwabRB(uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);
	uint _GenerateScaleImage(const uint8 * const pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, uint uiNewWidth, uint uiNewHeight, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);
	uint _GenerateMipMapData(const uint8 * const pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);

	bool _CreateTexture(ITexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags);
	bool _LoadTextureBMP(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);
	bool _LoadTextureTGA(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);
	bool _LoadTextureDTX(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);

	bool _LoadFontDFT(IFile *pFile, IBitmapFont *&prFnt);
	
	bool _CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 * const pData, uint32 ui32DataSize);
	bool _LoadSoundWAV(IFile *pFile, ISoundSample *&prSSample);

	bool _CreateMesh(IMesh *&prMesh, const uint8 * const pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, const TPoint3 &stCenter, const TVector3 &stExtents, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags);
	bool _LoadDMDFile(IFile *pFile, IEngineBaseObject *&prObj, E_MESH_MODEL_LOAD_FLAGS eLoadFlags);

	void _ProfilerEventHandler() const;
	void _ListResources() const;

	static void DGLE_API _s_ConListFileFormats(void *pParameter, const char *pcParam);
	static void DGLE_API _s_ConListResources(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_LoadTextureBMP(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);
	static bool DGLE_API _s_LoadTextureTGA(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter); 
	static bool DGLE_API _s_LoadTextureDTX(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter); 
	static bool DGLE_API _s_LoadFontDFT(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);
	static bool DGLE_API _s_LoadDMDFile(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);
	static bool DGLE_API _s_LoadSoundWAV(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);
	static void DGLE_API _s_ProfilerEventHandler(void *pParameter, IBaseEvent *pEvent);

public:

	CResourceManager(uint uiInstIdx);
	~CResourceManager();

	inline IBitmapFont* pISystemFont(){return (IBitmapFont*)_pDefBmpFnt;}
	void FreeAllResources();

	DGLE_RESULT DGLE_API CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResource);
	DGLE_RESULT DGLE_API CreateMaterial(IMaterial *&prMaterial, const char *pcName, bool bAddResource);
	DGLE_RESULT DGLE_API CreateLight(ILight *&prLight, const char *pcName, bool bAddResource);
	DGLE_RESULT DGLE_API CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResource);
	DGLE_RESULT DGLE_API CreateModel(IModel *&prModel, const char *pcName, bool bAddResource);
	DGLE_RESULT DGLE_API CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize, const char *pcName, bool bAddResource);

	DGLE_RESULT DGLE_API RegisterFileFormat(const char* pcExtension, E_ENGINE_OBJECT_TYPE eObjType, const char *pcDiscription, bool (DGLE_API *pLoadProc)(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter), void *pParameter);
	DGLE_RESULT DGLE_API UnregisterFileFormat(const char* pcExtension);
	DGLE_RESULT DGLE_API RegisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj);
	DGLE_RESULT DGLE_API UnregisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj);
	DGLE_RESULT DGLE_API GetRegisteredExtensions(char* pcTxt, uint &uiCharsCount);
	DGLE_RESULT DGLE_API GetExtensionDescription(const char *pcExtension, char *pcTxt, uint &uiCharsCount);
	DGLE_RESULT DGLE_API GetExtensionType(const char *pcExtension, E_ENGINE_OBJECT_TYPE &eType);
	
	DGLE_RESULT DGLE_API GetResourceByName(const char *pcName, IEngineBaseObject *&prObj);
	DGLE_RESULT DGLE_API GetDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *&prObj);
	
	DGLE_RESULT DGLE_API Load(const char *pcFileName, IEngineBaseObject *&prObj, uint uiLoadFlags);
	DGLE_RESULT DGLE_API LoadEx(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags);
	
	DGLE_RESULT DGLE_API FreeResource(IEngineBaseObject *&prObj);
	DGLE_RESULT DGLE_API AddResource(const char *pcName, IEngineBaseObject *pObj);
	DGLE_RESULT DGLE_API RemoveResource(IEngineBaseObject *pObj, bool &bCanDelete);

	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubSysType);

	IDGLE_BASE_IMPLEMENTATION(IResourceManager, INTERFACE_IMPL(IEngineSubSystem, INTERFACE_IMPL_END))
};