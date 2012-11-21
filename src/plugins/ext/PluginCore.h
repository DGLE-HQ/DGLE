/**
\author		Korotkov Andrey aka DRON
\date		21.11.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

extern "C" {
#include "png.h"
#include "pngstruct.h"
#include "jpeglib.h"
}

class CPluginCore : public IPlugin
{
	friend void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

private:
	
	uint _uiInstIdx;
	IEngineCore *_pEngineCore;
	IResourceManager *_pResMan;

	void _Init();
	void _Free();

	bool _LoadTexturePNG(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams);
	bool _LoadTextureJPG(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams);
	bool _LoadTextureDDS(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams);

	static void DGLE_API _s_Init(void *pParametr);
	static void DGLE_API _s_Free(void *pParametr);

	static bool DGLE_API _s_LoadTexturePNG(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static bool DGLE_API _s_LoadTextureJPG(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);
	static bool DGLE_API _s_LoadTextureDDS(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr);

	static void _s_PNGError(png_structp ptr, png_const_charp msg);
	static void PNGAPI _s_PNGReadCallback(png_structp ptr, png_bytep data, png_size_t length);
	
	static void _s_JPGInitSrc(j_decompress_ptr cinfo);
	static boolean _s_FillInputBuffer(j_decompress_ptr cinfo);
	static void _s_SkipInputData(j_decompress_ptr cinfo, long count);
	static void _s_TermSource(j_decompress_ptr cinfo);
	static void _s_ErrorExit(j_common_ptr cinfo);
	static void _s_OutputMessage(j_common_ptr cinfo);

public:

	CPluginCore(IEngineCore *pEngineCore);
	~CPluginCore();

	DGLE_RESULT DGLE_API GetPluginInfo(TPluginInfo &stInfo);
	DGLE_RESULT DGLE_API GetPluginInterfaceName(char* pcName, uint &uiCharsCount);

	IDGLE_BASE_IMPLEMENTATION(IPlugin, INTERFACE_IMPL_END)
};