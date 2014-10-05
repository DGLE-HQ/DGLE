/**
\author		Korotkov Andrey aka DRON
\date		04.10.2014 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CPluginCore : public IPlugin
{
	friend void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

private:
	
	uint _uiInstIdx;
	IEngineCore *_pEngineCore;
	IResourceManager *_pResMan;
	ISound *_pSound;

	void _Init();
	void _Free();
	bool _LoadMP3(IFile *pFile, IMusic *&prMusic);

	static void DGLE_API _s_Init(void *pParameter);
	static void DGLE_API _s_Free(void *pParameter);
	static bool DGLE_API _s_LoadMP3(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter);

public:

	CPluginCore(IEngineCore *pEngineCore);
	~CPluginCore();

	DGLE_RESULT DGLE_API GetPluginInfo(TPluginInfo &stInfo);
	DGLE_RESULT DGLE_API GetPluginInterfaceName(char* pcName, uint &uiCharsCount);

	IDGLE_BASE_IMPLEMENTATION(IPlugin, INTERFACE_IMPL_END)
};