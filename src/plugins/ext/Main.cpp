/**
\author		Korotkov Andrey aka DRON
\date		19.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "PluginCore.h"

HMODULE hModule	= NULL;

std::vector<CPluginCore*> PlCores;

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	if (uiInstIdx == -1)
	{
		for (size_t i = 0; i < PlCores.size(); ++i)
			PlCores[i]->_pEngineCore->AddToLogEx(("**Broadcast**" + std::string(pcTxt)).c_str(), eType, pcSrcFileName, iSrcLineNumber);
		
		return;
	}

	if (uiInstIdx >= (uint)PlCores.size())
		return;

	PlCores[uiInstIdx]->_pEngineCore->AddToLogEx(pcTxt, eType, pcSrcFileName, iSrcLineNumber);
}

void CALLBACK InitPlugin(IEngineCore *engineCore, IPlugin *&plugin)
{
	PlCores.push_back(NULL);
	
	uint cur_idx = (uint)PlCores.size() - 1;

	PlCores[cur_idx] = new CPluginCore(engineCore);

	plugin = (IPlugin*)(PlCores[cur_idx]);
}

void CALLBACK FreePlugin(IPlugin *plugin)
{
	for (size_t i = 0; i < PlCores.size(); ++i)
		if(plugin == PlCores[i])
		{
			delete PlCores[i];
			PlCores.erase(PlCores.begin() + i);
			return;
		}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		::hModule = hModule;
		break;
	case DLL_PROCESS_DETACH:
		for(size_t i = 0; i < PlCores.size(); i++)
			delete PlCores[i];
		PlCores.clear();
		break;
	}
	return TRUE;
}