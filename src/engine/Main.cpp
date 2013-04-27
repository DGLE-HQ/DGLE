/**
\author		Korotkov Andrey aka DRON
\date		21.02.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"

bool bTerminate = false;

std::vector<TEngInstance> vecEngineInstances;

TEngInstance* EngineInstance(uint uiInstIdx)
{
	if (uiInstIdx >= vecEngineInstances.size())
		return NULL;

	return &vecEngineInstances[uiInstIdx];
}

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	if (uiInstIdx == -1)
	{
		for (std::size_t i = 0; i < vecEngineInstances.size(); ++i)
			if (vecEngineInstances[i].pclCore)
				vecEngineInstances[i].pclCore->WriteToLogEx(("**Broadcast**" + std::string(pcTxt)).c_str(), eType, pcSrcFileName, iSrcLineNumber);
		return;
	}
	
	if (uiInstIdx >= vecEngineInstances.size())
		return;

	vecEngineInstances[uiInstIdx].pclCore->WriteToLogEx(pcTxt, eType, pcSrcFileName, iSrcLineNumber);
}

bool CALLBACK CreateEngine(IEngineCore *&pEngineCore, E_GET_ENGINE_FLAGS eFlags, uint8 ubtSDKVer)
{
	if (ubtSDKVer != _DGLE_SDK_VER_)
	{
		ShowModalUserAlert("Engine library version differs from header version!", "DGLE");
		return false;
	}

	vecEngineInstances.push_back(TEngInstance());

	size_t cur_id = vecEngineInstances.size() - 1;

	vecEngineInstances[cur_id].eGetEngFlags = eFlags;
	vecEngineInstances[cur_id].pclConsole = new CConsole(cur_id);
	vecEngineInstances[cur_id].pclCore = new CCore(cur_id);

	if (eFlags & GEF_FORCE_QUIT)
		bTerminate = true;

	pEngineCore = (IEngineCore*)(vecEngineInstances[cur_id].pclCore);

	return true;
}

bool CALLBACK FreeEngine(DGLE::IEngineCore *pEngineCore)
{
	if (!pEngineCore)
		return false;

	for (std::size_t i = 0; i < vecEngineInstances.size(); ++i)
		if (pEngineCore == vecEngineInstances[i].pclCore)
		{
			delete vecEngineInstances[i].pclCore;
			vecEngineInstances[i].pclCore = NULL;
			
			delete vecEngineInstances[i].pclConsole;
			vecEngineInstances[i].pclConsole = NULL;
			
			return true;
		}

	return false;
}

#ifdef PLATFORM_WINDOWS

//Break on specific alloc by number
#define CRT_BREAK_ON_ALLOC 0

HMODULE	hModule = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

#if defined(_DEBUG) && CRT_BREAK_ON_ALLOC >= 0
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetBreakAlloc(CRT_BREAK_ON_ALLOC);
#endif
		
		::hModule = hModule;
		
		break;

	case DLL_PROCESS_DETACH:
		
		for(size_t i = 0; i < vecEngineInstances.size(); ++i)
		{
			if(vecEngineInstances[i].pclCore)
			{
				delete vecEngineInstances[i].pclCore;
				vecEngineInstances[i].pclCore = NULL;
			}

			if(vecEngineInstances[i].pclConsole)
				delete vecEngineInstances[i].pclConsole;
		}

		vecEngineInstances.clear();

		if(bTerminate)
			TerminateProcess(GetCurrentProcess(), 1);
		
		break;
	}

	return TRUE;
}

#endif