/**
\author		Korotkov Andrey aka DRON
\date		19.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "PluginCore.h"
#include <locale>
CPluginCore::CPluginCore(IEngineCore *pEngineCore):
_pEngineCore(pEngineCore), _iDrawProfiler(0)
{
	_pEngineCore->GetInstanceIndex(_uiInstIdx);

	_pEngineCore->AddProcedure(EPT_RENDER, &_s_Render, (void*)this);
	_pEngineCore->AddProcedure(EPT_UPDATE, &_s_Update, (void*)this);
	_pEngineCore->AddProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->AddProcedure(EPT_FREE, &_s_Free, (void*)this);
	_pEngineCore->AddEventListener(ET_ON_WINDOW_MESSAGE, &_s_EventHandler, (void*)this);
	_pEngineCore->AddEventListener(ET_ON_PROFILER_DRAW, &_s_EventHandler, (void*)this);

	_pEngineCore->ConsoleRegisterVariable("tmpl_profiler", "Displays Plugin Template profiler.", &_iDrawProfiler, 0, 1);
}

CPluginCore::~CPluginCore()
{
	_pEngineCore->RemoveProcedure(EPT_RENDER, &_s_Render, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_UPDATE, &_s_Update, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_FREE, &_s_Free, (void*)this);
	_pEngineCore->RemoveEventListener(ET_ON_WINDOW_MESSAGE, &_s_EventHandler, (void*)this);
	_pEngineCore->AddEventListener(ET_ON_PROFILER_DRAW, &_s_EventHandler, (void*)this);

	_pEngineCore->ConsoleUnregister("tmpl_profiler");
}

void CPluginCore::_Render()
{
	//ToDo: Put your code here.
}

void CPluginCore::_Update(uint64 ui64DeltaTime)
{
	//ToDo: Put your code here.
}

void CPluginCore::_Init()
{
	//ToDo: Put your code here.
}

void CPluginCore::_Free()
{
	//ToDo: Put your code here.
}

void CPluginCore::_MsgProc(const TWindowMessage &stMsg)
{
	//ToDo: Put your code here.
}

void CPluginCore::_ProfilerDraw()
{
	if (_iDrawProfiler == 0)
		return;

	//ToDo: Put your code here.

	_pEngineCore->RenderProfilerText("======Plugin Template=======");
	_pEngineCore->RenderProfilerText("Everything is Ok!");
	_pEngineCore->RenderProfilerText("============================");
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInfo(TPluginInfo &stInfo)
{
	strcpy(stInfo.cName, PLUGIN_NAME);
	strcpy(stInfo.cVersion, PLUGIN_VERSION);
	strcpy(stInfo.cVendor, PLUGIN_VENDOR);
	strcpy(stInfo.cDescription, PLUGIN_DESCRIPTION);
	stInfo.ui8PluginSDKVersion = _DGLE_PLUGIN_SDK_VER_;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInterfaceName(char* pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount <= (uint)strlen(PLUGIN_INTERFACE_NAME))
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		strcpy(pcName, "");
		return E_INVALIDARG;
	}

	strcpy(pcName, PLUGIN_INTERFACE_NAME);
	
	return S_OK;
}

void DGLE_API CPluginCore::_s_Render(void *pParametr)
{
   ((CPluginCore *)pParametr)->_Render();
}

void DGLE_API CPluginCore::_s_Update(void *pParametr)
{
	uint64 dt;
	((CPluginCore *)pParametr)->_pEngineCore->GetLastUpdateDeltaTime(dt);
	((CPluginCore *)pParametr)->_Update(dt);
}

void DGLE_API CPluginCore::_s_Init(void *pParametr)
{
	((CPluginCore *)pParametr)->_Init();
}

void DGLE_API CPluginCore::_s_Free(void *pParametr)
{
	((CPluginCore *)pParametr)->_Free();
}

void DGLE_API CPluginCore::_s_EventHandler(void *pParametr, IBaseEvent *pEvent)
{
	E_EVENT_TYPE ev_type;
	TWindowMessage msg;

	pEvent->GetEventType(ev_type);

	switch(ev_type)
	{
	case ET_ON_WINDOW_MESSAGE:
		IEvWindowMessage *p_ev_msg;
		p_ev_msg = (IEvWindowMessage *)pEvent;
		p_ev_msg->GetMessage(msg);
		((CPluginCore *)pParametr)->_MsgProc(msg);
		break;

	case ET_ON_PROFILER_DRAW:
		((CPluginCore *)pParametr)->_ProfilerDraw();
		break;
	}
}