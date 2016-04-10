/**
\author		Korotkov Andrey aka DRON
\date		10.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"

using namespace std;

bool bTerminate = false;

vector<TEngInstance> vecEngineInstances;

TEngInstance *EngineInstance(uint uiInstIdx)
{
	if (uiInstIdx >= vecEngineInstances.size())
		return NULL;

	return &vecEngineInstances[uiInstIdx];
}

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	if (uiInstIdx == -1)
	{
		for (const auto &engine_instance : vecEngineInstances)
			if (engine_instance.pclCore)
				engine_instance.pclCore->WriteToLogEx(("**Broadcast**"s + pcTxt).c_str(), eType, pcSrcFileName, iSrcLineNumber);
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
	for (auto &engine_instance : vecEngineInstances)
	{
		bool stop = false;
		if (!pEngineCore || (stop = pEngineCore == engine_instance.pclCore))
		{
			delete engine_instance.pclCore;
			engine_instance.pclCore = NULL;

			delete engine_instance.pclConsole;
			engine_instance.pclConsole = NULL;
		}
		if (stop)
			return true;
	}

	if (!pEngineCore)
	{
		vecEngineInstances.clear();
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
		
		if (bTerminate)
			TerminateProcess(GetCurrentProcess(), 1);
		
		break;
	}

	return TRUE;
}

#endif