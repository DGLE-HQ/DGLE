/**
\author		Korotkov Andrey aka DRON
\date		30.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _RESOURCEMANAGER_H
#define _RESOURCEMANAGER_H

#include "Common.h"
//#include "LightAttribs.h"

class CBObjDummy;
class CBitmapFontDummy;
class CSSampleDummy;
class CMusicDummy;

struct TFileFormat
{
	std::string ext;
	std::string discr;
	E_ENG_OBJ_TYPE type;
	void *pParametr;
	bool (DGLE2_API *pLoadProc)(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
};

struct TDefaultRes
{
	E_ENG_OBJ_TYPE type;
	IEngBaseObj *pBaseObj;
	TDefaultRes(E_ENG_OBJ_TYPE _type, IEngBaseObj *_pObj):type(_type), pBaseObj(_pObj){}
};

struct TResource
{
	uint32 nameHash;
	IEngBaseObj *pObj;
	
	TResource(const char *pFullName, IEngBaseObj *pObject):
	pObj(pObject)
	{
		nameHash = GetCRC32((uint8*)pFullName, (uint32)strlen(pFullName)*sizeof(char));
	}
};

//class CBasePfpMaterial;

class CResourceManager : public CInstancedObj, public IResourceManager
{
	TWinHandle				_stWnd;
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

	CBObjDummy				*_pBObjDummy;
	CBitmapFontDummy		*_pDefBmFntDummy;
	CSSampleDummy			*_pDefSSmpDummy;
	CMusicDummy				*_pDefMusicDummy;
	
	inline HRESULT _Load(const char *pcFileName, IFile *pFile, uint uiFFIdx, IEngBaseObj *&prObj, uint uiLoadFlags);
	inline uint _GetFFIdx(const char *pcFileName, E_ENG_OBJ_TYPE eObjType, IEngBaseObj *&prObj);
	inline uint8 _GetBytesPerPixel(E_TEXTURE_DATA_FORMAT &format);
	uint _GenerateDecompressedTextureData(const uint8 *pDataIn, uint8 *&prDataOut, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_TEXTURE_CREATION_FLAGS &eCreationFlags);
	bool _SwabRB(uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT &format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);
	uint _GenerateScaleImage(const uint8 *pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, uint uiNewWidth, uint uiNewHeight, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);
	uint _GenerateMipMapData(const uint8 *pDataIn, uint uiWidth, uint uiHeight, uint8 *&prDataOut, E_TEXTURE_DATA_FORMAT format, E_CORE_RENDERER_DATA_ALIGNMENT eAlignment);

	bool _CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags);
	bool _LoadTextureBMP(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);
	bool _LoadTextureTGA(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);
	bool _LoadTextureDTX(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eFlags);

	bool _LoadFontDFT(IFile *pFile, IBitmapFont *&prFnt);
	bool _LoadMusicMCI(IFile *pFile, IMusic *&prMusic);

	bool _CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags);
	bool _LoadDMDFile(IFile *pFile, IEngBaseObj *&prObj, E_MESH_LOAD_FLAGS eLoadFlags);

	void _ProfilerEventHandler() const;

	static void DGLE2_API _s_ConListFileFormats(void *pParametr, const char *pcParam);
	static bool DGLE2_API _s_LoadTextureBMP(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static bool DGLE2_API _s_LoadTextureTGA(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr); 
	static bool DGLE2_API _s_LoadTextureDTX(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr); 
	static bool DGLE2_API _s_LoadFontDFT(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static bool DGLE2_API _s_LoadMusicMCI(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static bool DGLE2_API _s_LoadDMDFile(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static void DGLE2_API _s_ProfilerEventHandler(void *pParametr, IBaseEvent *pEvent);

public:

	CResourceManager(uint uiInstIdx);
	~CResourceManager();

	inline IBitmapFont* pISystemFont(){return (IBitmapFont*)_pDefBmpFnt;}
	void FreeAllResources();

	HRESULT DGLE2_API CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResourse);
	HRESULT DGLE2_API CreateMaterial(IMaterial *&prMaterial, const char *pcName, bool bAddResourse);
	HRESULT DGLE2_API CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags, const char *pcName, bool bAddResourse);
	
	HRESULT DGLE2_API RegisterFileFormat(const char* pcExtension, E_ENG_OBJ_TYPE eObjType, const char *pcDiscription, bool (DGLE2_API *pLoadProc)(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr), void *pParametr);
	HRESULT DGLE2_API UnregisterFileFormat(const char* pcExtension);
	HRESULT DGLE2_API RegisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj);
	HRESULT DGLE2_API UnregisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj);
	HRESULT DGLE2_API GetRegisteredExtensions(char* pcTxt, uint &uiCharsCount);
	HRESULT DGLE2_API GetExtensionDescription(const char *pcExtension, char *pcTxt, uint &uiCharsCount);
	HRESULT DGLE2_API GetExtensionType(const char *pcExtension, E_ENG_OBJ_TYPE &eType);
	
	HRESULT DGLE2_API GetResourceByFileName(const char *pcFileName, IEngBaseObj *&prObj);
	HRESULT DGLE2_API GetDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *&prObj);
	
	HRESULT DGLE2_API Load(const char *pcFileName, IEngBaseObj *&prObj, uint uiLoadFlags);
	HRESULT DGLE2_API Load2(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags);
	
	HRESULT DGLE2_API FreeResource(IEngBaseObj *&prObj);
	HRESULT DGLE2_API AddResource(const char *pcName, IEngBaseObj *pObj);
	HRESULT DGLE2_API RemoveResource(IEngBaseObj *pObj, bool &bCanDelete);

	HRESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubSysType);

	IDGLE2_BASE_IMPLEMENTATION1(IResourceManager, IEngineSubSystem)
};
#endif //_RESOURCEMANAGER_H
