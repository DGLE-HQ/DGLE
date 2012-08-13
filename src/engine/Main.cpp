/**
\author		Korotkov Andrey aka DRON
\date		21.02.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Common.h"

bool bTerminate = false;

std::vector<TEngInstance> EngineInstances;

TEngInstance* EngineInstance(uint uiInstIdx)
{
	if (uiInstIdx >= EngineInstances.size())
		return NULL;

	return &EngineInstances[uiInstIdx];
}

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	if (uiInstIdx == -1)
	{
		for (std::size_t i = 0; i < EngineInstances.size(); i++)
			if (EngineInstances[i].pclCore)
				EngineInstances[i].pclCore->AddToLogEx(("**Broadcast**" + std::string(pcTxt)).c_str(), eType, pcSrcFileName, iSrcLineNumber);
		return;
	}
	
	if (uiInstIdx >= EngineInstances.size())
		return;

	EngineInstances[uiInstIdx].pclCore->AddToLogEx(pcTxt, eType, pcSrcFileName, iSrcLineNumber);
}

extern bool CALLBACK CreateEngine(IEngineCore *&pEngineCore, E_GET_ENGINE_FLAGS eFlags, uint8 ubtSDKVer)
{
	if (ubtSDKVer != _DGLE2_SDK_VER_)
	{
		ShowModalUserAlert("Engine library version differs from header version!", "DGLE2");
		return false;
	}

	EngineInstances.push_back(TEngInstance());

	size_t cur_id = EngineInstances.size() - 1;

	EngineInstances[cur_id].eGetEngFlags = eFlags;
	EngineInstances[cur_id].pclConsole = new CConsole(!(eFlags & GEF_FORCE_SINGLE_THREAD));
	EngineInstances[cur_id].pclCore = new CCore((uint)cur_id, eFlags & GEF_FORCE_SINGLE_THREAD);

	if (eFlags & GEF_FORCE_QUIT)
		bTerminate = true;

	pEngineCore = (IEngineCore*)(EngineInstances[cur_id].pclCore);

	return true;
}

extern bool CALLBACK FreeEngine(DGLE2::IEngineCore *pEngineCore)
{
	if (!pEngineCore)
		return false;

	for (std::size_t i = 0; i < EngineInstances.size(); i++)
		if (pEngineCore == EngineInstances[i].pclCore)
		{
			delete EngineInstances[i].pclCore;
			EngineInstances[i].pclCore = NULL;
			
			delete EngineInstances[i].pclConsole;
			EngineInstances[i].pclConsole = NULL;
			
			return true;
		}

	return false;
}

#ifdef PLATFORM_WINDOWS

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
		
		for(size_t i = 0; i < EngineInstances.size(); i++)
		{
			if(EngineInstances[i].pclCore)
				delete EngineInstances[i].pclCore;

			if(EngineInstances[i].pclConsole)
				delete EngineInstances[i].pclConsole;
		}

		EngineInstances.clear();

		if(bTerminate)
			TerminateProcess(GetCurrentProcess(), 1);
		
		break;
	}

	return TRUE;
}

#endif