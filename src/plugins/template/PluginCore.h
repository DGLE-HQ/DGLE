/**
\author		Korotkov Andrey aka DRON
\date		19.10.2012 (c)Korotkov Andrey

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
	
	uint				 _uiInstIdx;
	IEngineCore			*_pEngineCore;

	int					 _iDrawProfiler;

	void _Render();
	void _Update(uint64 ui64DeltaTime);
	void _Init();
	void _Free();
	void _MsgProc(const TWinMessage &stMsg);
	void _ProfilerDraw();

	static void DGLE_API _s_EventHandler(void *pParametr, IBaseEvent *pEvent);
	static void DGLE_API _s_Render(void *pParametr);
	static void DGLE_API _s_Update(void *pParametr);
	static void DGLE_API _s_Init(void *pParametr);
	static void DGLE_API _s_Free(void *pParametr);

public:

	CPluginCore(IEngineCore *pEngineCore);
	~CPluginCore();

	DGLE_RESULT DGLE_API GetPluginInfo(TPluginInfo &stInfo);
	DGLE_RESULT DGLE_API GetPluginInterfaceName(char* pcName, uint &uiCharsCount);

	IDGLE_BASE_IMPLEMENTATION(IPlugin)
};