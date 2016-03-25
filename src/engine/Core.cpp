/**
\author		Korotkov Andrey aka DRON
\date		25.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "Core.h"
#include "MainWindow.h"
#include "HookedWindow.h"
#include "DummyWindow.h"
#include "SplashWindow.h"
#include "MainFileSystem.h"
#include "ResourceManager.h"
#include "Input.h"
#include "Sound.h"
#include "CoreRendererGL.h"
#include "Render.h"
#include "Render2D.h"

using namespace std;

extern bool bUnhandledFilterEnabled;

//Event Classes//

class CBaseEvent : public IBaseEvent
{
	E_EVENT_TYPE _eEvType;

public:
	
	CBaseEvent(E_EVENT_TYPE eEvType):_eEvType(eEvType){}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = _eEvType;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IBaseEvent, INTERFACE_IMPL_END)
};

class CEvBeforeInit : public IEvBeforeInitialization
{
	 TEngineWindow *_pstWindowParam;
	 E_ENGINE_INIT_FLAGS *_peInitFlags; 

public:

	CEvBeforeInit(TEngineWindow *pstWindowParam, E_ENGINE_INIT_FLAGS *peInitFlags)
	{
		_pstWindowParam = pstWindowParam;
		_peInitFlags = peInitFlags;
	}

	DGLE_RESULT DGLE_API SetParameters(const TEngineWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags)
	{
		(*_pstWindowParam)= stWindowParam;
		(*_peInitFlags)	= eInitFlags;
		return S_OK;
	}
	
	DGLE_RESULT DGLE_API GetParameters(TEngineWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags)
	{
		stWindowParam = (*_pstWindowParam);
		eInitFlags = (*_peInitFlags); 	
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_BEFORE_INITIALIZATION;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvBeforeInitialization, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

class CEvWinMessage : public IEvWindowMessage
{
	TWindowMessage _stMessage;

public:

	CEvWinMessage(const TWindowMessage &stMessage):_stMessage(stMessage){}

	DGLE_RESULT DGLE_API GetMessage(TWindowMessage &stWinMsg)
	{
		stWinMsg = _stMessage;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_WINDOW_MESSAGE;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvWindowMessage, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

class CEvGetSubSystem : public IEvGetSubSystem
{

	E_ENGINE_SUB_SYSTEM _eSSType;
	IEngineSubSystem **_ppEngSS;

public:

	CEvGetSubSystem(E_ENGINE_SUB_SYSTEM eSSType, IEngineSubSystem *pEngSS)
	{
		_eSSType = eSSType;
		_ppEngSS = &pEngSS;
	}

	DGLE_RESULT DGLE_API GetSubSystemType(E_ENGINE_SUB_SYSTEM &eSubSystem)
	{
		eSubSystem = _eSSType;
		return S_OK;
	}

	DGLE_RESULT DGLE_API OverrideSubSystem(IEngineSubSystem *prSubSystem)
	{
		_ppEngSS = &prSubSystem;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_GET_SUBSYSTEM;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvGetSubSystem, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

class CEvFatalMessage : public IEvFatalMessage
{
	
	CCore *_pCore;
	string _txt;
	bool _bNoMessage, _bNoHalt;

public:

	CEvFatalMessage(string txt, CCore *pCore):_txt(txt), _pCore(pCore), _bNoMessage(false), _bNoHalt(false){}
	
	bool DoHalt() {return !_bNoHalt;}
	bool DoShowMessage() {return !_bNoMessage;}

	DGLE_RESULT DGLE_API GetMessageText(char *pcTxt, uint &uiCharsCount)
	{
		if (!pcTxt)
		{
			uiCharsCount = _txt.length() + 1;
			return S_OK;
		}

		if (uiCharsCount <= _txt.length())
		{
			uiCharsCount = _txt.length() + 1;
			strcpy(pcTxt, "");
			return E_INVALIDARG;
		}

		strcpy(pcTxt, _txt.c_str());

		return S_OK;
	}
	
	DGLE_RESULT DGLE_API FreezeEngine(bool bFreeze)
	{
		_pCore->ToogleSuspendEngine(bFreeze);
		return S_OK;
	}

	DGLE_RESULT DGLE_API ForceNoMessage()
	{
		_bNoMessage = true;
		return S_OK;
	}
		
	DGLE_RESULT DGLE_API ForceIgnoreError()
	{
		_bNoHalt = true;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_ENGINE_FATAL_MESSAGE;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvFatalMessage, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

class CEvGoFullScreen : public IEvGoFullScreen
{
	
	bool _bIsFullScreen;
	uint &_uiWidth, &_uiHeight;

public:

	CEvGoFullScreen(uint &uiResWidth, uint &uiResHeight, bool bFScreen) :
	  _uiWidth(uiResWidth), _uiHeight(uiResHeight), _bIsFullScreen(bFScreen)
	{}

	DGLE_RESULT DGLE_API GetResolution(uint &uiScreenWidth, uint &uiScreenHeight, bool &bGoFullScreen)
	{
		uiScreenWidth = _uiWidth;
		uiScreenHeight = _uiHeight;
		bGoFullScreen = _bIsFullScreen;

		return S_OK;
	}

	DGLE_RESULT DGLE_API SetResolution(uint uiScreenWidth, uint uiScreenHeight)
	{
		_uiWidth = uiScreenWidth;
		_uiHeight = uiScreenHeight;

		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_FULLSCREEN;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvGoFullScreen, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

//Engine Core//

CCore::CCore(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_pMainWindow(NULL), _uiFPSTimer(0),
_uiLogWarningsCount(0), _uiLogErrorsCount(0),
_pRender(NULL), _pMainFS(NULL), _pResMan(NULL), _pInput(NULL), _pSound(NULL), _pCoreRenderer(NULL),
_bBuiltInSound(true), _bBuiltInRenderer(true), _bBuiltInInput(true),
_pSplashWindow(NULL), _bCmdKeyIsPressed(false), _bFScreenKeyIsPressed(false),
_bDoExit(false), _bInDrawProfilers(false), _bWasFScreen(false), _bRendering(false),
_bStartedFlag(false), _bInitedFlag(false), _bQuitFlag(false),
_iAllowPause(1), _iFPSToCaption(0), _iAllowDrawProfilers(1), _iDrawProfiler(0),
_bNeedApplyNewWnd(false), _uiLastUpdateDeltaTime(0), _uiLastMemUsage(0),
_ui64StartTime(0), _ui64PauseTime(0),
//Delegates initialization
_clDelUpdate(uiInstIdx),
_clDelRender(uiInstIdx),
_clDelInit(uiInstIdx),
_clDelFree(uiInstIdx),
_clDelMLoop(uiInstIdx),
_clDelMProc(uiInstIdx),
_clDelOnFPSTimer(uiInstIdx)
{
	_pcCustomSplash = new char [1];
	_pcCustomSplash[0] = '\0';

	if (!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_NO_LOG_FILE))
	{
		_clLogFile.setf(ios_base::right, ios_base::adjustfield);
		_clLogFile.open(("log" + (InstIdx() != 0 ? to_string(InstIdx()) : "") + ".txt").c_str(), ios::out | ios::trunc);

		TSysTimeAndDate time;
		GetLocalTimaAndDate(time);

		_clLogFile << "DGLEngine Log File" << endl;
		_clLogFile << "Version: "<< DGLE_VERSION << endl;
		_clLogFile << "Visit http://dglengine.org/ for more information." << endl;
		_clLogFile << "Log Started at " << time.ui16Day << '.' << time.ui16Month << '.' << time.ui16Year << '.' << endl;
	}
}

CCore::~CCore()
{
	if (!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_NO_LOG_FILE))
	{
		Console()->UnRegCom("core_allow_pause");
		Console()->UnRegCom("core_fps_in_caption");
		Console()->UnRegCom("core_profiler");
		Console()->UnRegCom("core_allow_profilers");
		Console()->UnRegCom("core_version");
		Console()->UnRegCom("core_features");
		Console()->UnRegCom("core_list_plugins");
		Console()->UnRegCom("core_instidx");
		Console()->UnRegCom("core_set_mode");

		if (!_bDoExit)
			_LogWrite("***Abnormal Engine Termination***");

		_LogWrite("Engine Finalized.");
		_clLogFile << "----" << endl;
		_clLogFile << "Warning(s): " << _uiLogWarningsCount << " Error(s): " << _uiLogErrorsCount << endl;
		
		if (_ui64CyclesCount == 0) _ui64CyclesCount = 1;
		_clLogFile << "Average FPS: " << (uint64)(_ui64FPSSumm / _ui64CyclesCount) << endl;		

		_clLogFile << "Log Closed.";
		_clLogFile.close();
	}

	delete[] _pcCustomSplash;
}

void CCore::_LogWriteEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	string msg;
	
	switch (eType)
	{
	case LT_WARNING:
		msg = "[WARNING] "s + pcTxt;
		++_uiLogWarningsCount;
		break;
	
	case LT_ERROR:
		msg = "[ERROR] "s + pcTxt + " (File: \"" + pcSrcFileName + "\", Line: " + to_string(iSrcLineNumber) + ')';
		++_uiLogErrorsCount;
		break;
	
	case LT_FATAL:
		msg = "[FATAL] "s + pcTxt + " (File: \"" + pcSrcFileName + "\", Line: " + to_string(iSrcLineNumber) + ')';
		++_uiLogErrorsCount;
		
		_LogWrite(msg.c_str(), true);		 
							
		CEvFatalMessage *ev_fatal_msg;
							
		ev_fatal_msg = new CEvFatalMessage(msg, this);

		CastEvent(ET_ON_ENGINE_FATAL_MESSAGE, (IBaseEvent*)ev_fatal_msg);

		if (((CEvFatalMessage*)ev_fatal_msg)->DoShowMessage())
		{
			Console()->Visible(true);
								
			if (_pSplashWindow)
			{
				_pSplashWindow->Free();
				_pSplashWindow = NULL;
			}

			ShowModalUserAlert(pcTxt, "DGLE Fatal Error!");
		}

		if (((CEvFatalMessage*)ev_fatal_msg)->DoHalt())
		{
			delete ev_fatal_msg;
			delete this;
			TerminateProcess(GetCurrentProcess(), 2);
		}
		else
			delete ev_fatal_msg;

		break;

	default:
		msg = pcTxt;
		break;
	}
	
	if (eType != LT_FATAL)
		_LogWrite(msg.c_str(), eType > 1/*immediately flushs only errors and fatals*/);

	assert(eType == LT_INFO);
}

void CCore::_LogWrite(const char *pcTxt, bool bFlush)
{
	if (_clLogFile.is_open())
	{
		TSysTimeAndDate time;
		GetLocalTimaAndDate(time);
		_clLogFile.fill('0');
		_clLogFile.width(2);
		_clLogFile << time.ui16Hour << ':';
		_clLogFile.width(2);
		_clLogFile << time.ui16Minute << ':';
		_clLogFile.width(2);
		_clLogFile << time.ui16Second << '.';	
		_clLogFile.width(3);
		_clLogFile << time.ui16Milliseconds;
		_clLogFile.width(0);
		_clLogFile << " - ";

		_clLogFile << pcTxt << endl;

		if (bFlush)
			_clLogFile.flush();
	}

#ifndef NDEBUG
	OutputDebugTxt(pcTxt);
#endif

	Console()->Write(pcTxt);
}

void CCore::_MessageProc(const TWindowMessage &stMsg)
{
	{
		CEvWinMessage event(stMsg);
		CastEvent(ET_ON_WINDOW_MESSAGE, &event);
	}
	
	size_t i;
	
	switch (stMsg.eMessage)
	{
	case WMT_REDRAW:

		_clDelMLoop.Invoke();

		break;

	case WMT_CLOSE:

		_bDoExit = true;

		break;

	case WMT_DESTROY:

		LOG("Finalizing Engine...", LT_INFO);

		_pCoreRenderer->MakeCurrent();

		if (!_clDelFree.IsNull() || !_vecEngineCallbacks.empty()) 
		{
			LOG("Calling user finalization procedure...", LT_INFO);
			
			_InvokeUserCallback(EPT_FREE);

			_clDelFree.Invoke();	
			
			LOG("Done.", LT_INFO);
		}

		_pResMan->FreeAllResources();
		_pMainFS->UnregisterAndFreeAll();

		i = 0;

		while (i < _vecPlugins.size())
		{
			char *p_name;
			uint chars_cnt;

			_vecPlugins[i].pPlugin->GetPluginInterfaceName(NULL, chars_cnt);

			p_name = new char[chars_cnt];

			_vecPlugins[i].pPlugin->GetPluginInterfaceName(p_name, chars_cnt);
					
			if (strcmp(p_name, "ISubSystemPlugin") != 0)
				_UnloadPlugin(_vecPlugins[i].pPlugin);
			else
				++i;

			delete[] p_name;
		}

		if (!ReleaseTimer(_uiFPSTimer))
			LOG("Can't release FPS timer.", LT_ERROR);

		break;

	case WMT_RELEASED:
		
		delete _pResMan;

		delete _pRender;

		_pCoreRenderer->Finalize();

		while (!_vecPlugins.empty())
			_UnloadPlugin(_vecPlugins.begin()->pPlugin);

#ifndef NO_BUILTIN_INPUT
		if (_bBuiltInInput)
			delete (CInput*)_pInput;
#endif

#ifndef NO_BUILTIN_SOUND
		if (_bSndEnabled && _bBuiltInSound)
			delete (CSound*)_pSound;
#endif

#ifndef NO_BUILTIN_RENDERER
		if (_bBuiltInRenderer)
			delete (CCoreRendererGL*)_pCoreRenderer;
#endif

		delete _pMainFS;

		_pMainWindow->Free();

		for (i = 0; i < _vecEvents.size(); ++i)
			delete _vecEvents[i].pDEvent;

		_vecEvents.clear();

		break;

	case WMT_ACTIVATED:

		if (stMsg.ui32Param1 == 1)
			break;

		_bPause = false;	

		if (_iAllowPause)
		{
			_pMainWindow->SetCaption(_pcApplicationCaption);
			if (_pSound != NULL) _pSound->MasterPause(false);
			_ui64PauseTime += GetPerfTimer() - _ui64PauseStartTime;
		}

		break;

	case WMT_DEACTIVATED:

		if (stMsg.ui32Param1 == 1)
			break;

		_bPause = true;

		if (_iAllowPause)
		{
			_pMainWindow->SetCaption((_pcApplicationCaption + " [Paused]"s).c_str());
			if (_pSound != NULL) _pSound->MasterPause(true);
			_ui64PauseStartTime = GetPerfTimer();
		}

		if ((_stWin.bFullScreen && !_bNeedApplyNewWnd))
		{
			if (_bDoExit)
				_pMainWindow->ExitFullScreen();
			else
			{
				_bWasFScreen = true;
				_stWndToApply = _stWin;
				_stWndToApply.bFullScreen = false;
				_pMainWindow->Minimize();
				_ChangeWinMode(_stWndToApply, true);
			}
		}

		break;

	case WMT_RESTORED:

		_bCmdKeyIsPressed = false;

		if (_bWasFScreen && !_bNeedApplyNewWnd)
		{
			_bWasFScreen = false;
			_stWndToApply.bFullScreen = true;
			_ChangeWinMode(_stWndToApply, true);
			break;
		}

	case WMT_SIZE:

		_stWin.uiWidth = stMsg.ui32Param1;
		_stWin.uiHeight = stMsg.ui32Param2;
		_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);

		break;

	case WMT_KEY_UP:

		if (!(_stWin.uiFlags & EWF_RESTRICT_FULLSCREEN_HOTKEY))
		{
			if (stMsg.ui32Param1 == c_eFScreenKeyFirst[0] || stMsg.ui32Param1 == c_eFScreenKeyFirst[1])
				_bCmdKeyIsPressed = false;

			if (stMsg.ui32Param1 == c_eFScreenKeySecond)
				_bFScreenKeyIsPressed = false;
		}

		if (stMsg.ui32Param1 == KEY_GRAVE && !(_stWin.uiFlags & EWF_RESTRICT_CONSOLE_HOTKEY))
			Console()->Visible(true);

		break;

	case WMT_KEY_DOWN:

		if (_stWin.uiFlags & EWF_RESTRICT_FULLSCREEN_HOTKEY)
			break;

		if (stMsg.ui32Param1 == c_eFScreenKeySecond && _bCmdKeyIsPressed && !_bFScreenKeyIsPressed && !_bNeedApplyNewWnd)
		{
			_bFScreenKeyIsPressed = true;
			_stWndToApply = _stWin;
			_stWndToApply.bFullScreen = !_stWndToApply.bFullScreen;
			_ChangeWinMode(_stWndToApply, false);
		}

		if (stMsg.ui32Param1 == c_eFScreenKeyFirst[0] || stMsg.ui32Param1 == c_eFScreenKeyFirst[1])
			_bCmdKeyIsPressed = true;

		break;
	}
}

void CCore::_OnTimer()
{
	if (_bNeedApplyNewWnd)
	{
		_ChangeWinMode(_stWndToApply, false);
		_bNeedApplyNewWnd = false;
	}

	{
		CBaseEvent event(ET_ON_PER_SECOND_TIMER);
		CastEvent(ET_ON_PER_SECOND_TIMER, &event);
	}

	if (_iDrawProfiler == 3)
		_uiLastMemUsage = GetProcessMemoryUsage();

	_uiLastFPS = _uiFPSCount;
	_uiLastUPS = _uiUPSCount;

	if (!_bPause)
	{
		_ui64FPSSumm  += _uiLastFPS;
		++_ui64CyclesCount;

		if (_iFPSToCaption == 1)
			_pMainWindow->SetCaption((_pcApplicationCaption + " FPS:"s + to_string(_uiLastFPS)).c_str());
		else
			_pMainWindow->SetCaption(_pcApplicationCaption);
	}

	_uiFPSCount = 0;
	_uiUPSCount = 0;
}

void CCore::_MainLoop()
{
	if (_bDoExit)
	{
		_pMainWindow->KillWindow();
		return;
	}

	if (FAILED(_pCoreRenderer->MakeCurrent()))
		return;

	uint64 time = GetPerfTimer();
	uint time_delta = (uint)(time - _ui64TimeOld);

	bool flag = false;

	Console()->EnterThreadSafeSection();

	uint cycles_cnt;

	if (_eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
		cycles_cnt = 1;
	else
		if (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE)
			cycles_cnt = time_delta >= _uiUpdateInterval * 1000;
		else
		{
			cycles_cnt = time_delta / (_uiUpdateInterval * 1000);

			if (cycles_cnt > _sc_MaxUpdateCycles)
				cycles_cnt = _sc_MaxUpdateCycles;
		}

	if (cycles_cnt > 0)
	{
		if (_eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
			_uiLastUpdateDeltaTime = _stWin.bVSync && _uiLastFPS >= 5 ? 200 / (_uiLastFPS / 5) : time_delta;
		else
			_uiLastUpdateDeltaTime = (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE) ? time_delta : _uiUpdateInterval;
		
		_ui64UpdateDelay = GetPerfTimer();
	}
	
	for (uint i = 0; i < cycles_cnt; ++i)
	{
		if (((!_bPause && _iAllowPause) || !_iAllowPause) && (!_clDelUpdate.IsNull() || !_vecEngineCallbacks.empty()) && !_bQuitFlag) 
		{
			if (i == cycles_cnt - 1)
				_pRender->RefreshBatchData();

			_clDelUpdate.Invoke();
			
			_InvokeUserCallback(EPT_UPDATE);

			++_uiUPSCount;
		}

		flag = true;
	}

	if (flag)
	{
		_ui64UpdateDelay = GetPerfTimer() - _ui64UpdateDelay;
		
		if (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE || _eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
			_ui64TimeOld = time;
		else
			_ui64TimeOld = time - time_delta % (_uiUpdateInterval * 1000);
	}

	_RenderFrame();

	Console()->LeaveThreadSafeSection();

	const uint sleep = (int)((_eInitFlags & EIF_FORCE_LIMIT_FPS) && ((_uiLastFPS > _uiUpdateInterval && _uiLastFPS > 120) || _bPause)) * 8 +
				 (int)(_bPause && _iAllowPause) * 15 + (int)(_stSysInfo.uiCPUCount < 2 && cycles_cnt < 2) * 6;

	if (sleep > 0)
		Suspend(sleep);
}

void CCore::_RenderFrame()
{
	_bRendering = true;

	_pRender->BeginRender();

	_ui64RenderDelay = GetPerfTimer();

	_pRender->BeginFrame();

	{
		CBaseEvent event(ET_BEFORE_RENDER);
		CastEvent(ET_BEFORE_RENDER, &event);
	}

	_clDelRender.Invoke();
	
	_InvokeUserCallback(EPT_RENDER);

	{
		CBaseEvent event(ET_AFTER_RENDER);
		CastEvent(ET_AFTER_RENDER, &event);
	}

	_pRender->EndFrame();
	
	_ui64RenderDelay = GetPerfTimer() - _ui64RenderDelay;

	if (_iAllowDrawProfilers == 1)
	{
		_pRender->pRender2D()->BeginProfiler2D();

		_bInDrawProfilers = true;

		_uiProfilerCurTxtXOffset = 0;
		_uiProfilerCurTxtYOffset = 0;
		_uiProfilerCurTxtMaxLength = 0;
				
		if (_iDrawProfiler > 0)
		{
			RenderProfilerText(("FPS:" + to_string(_uiLastFPS)).c_str(), _uiLastFPS < 60 ? ColorRed() : ColorWhite());
			RenderProfilerText(("UPS:" + to_string(_uiLastUPS)).c_str(), _uiLastUPS < 1000 / _uiUpdateInterval ? ColorRed() : ColorWhite());

			if (_iDrawProfiler > 1)
			{
				RenderProfilerText(("Render delay:" + to_string(_ui64RenderDelay / 1000) + '.' + to_string(_ui64RenderDelay % 1000) + " ms").c_str(), ColorWhite());
				RenderProfilerText(("Update delay:" + to_string(_ui64UpdateDelay / 1000) + '.' + to_string(_ui64RenderDelay % 1000) + " ms").c_str(), ColorWhite());
			}

			if (_iDrawProfiler == 3)
				RenderProfilerText(("Memory usage:" + to_string((uint)ceil(_uiLastMemUsage / 1024.f)) + " KiB").c_str(), ColorWhite());
		}
				
		_pRender->DrawProfiler();

		{
			CBaseEvent event(ET_ON_PROFILER_DRAW);
			CastEvent(ET_ON_PROFILER_DRAW, &event);
		}

		_bInDrawProfilers = false;

		_pRender->pRender2D()->EndProfiler2D();
	}

	_pRender->EndRender();

	++_uiFPSCount;

	_bRendering = false;
}

DGLE_RESULT DGLE_API CCore::RenderFrame()
{
	if (_bRendering)
		return E_FAIL;

	_RenderFrame();

	return S_OK;
}

void CCore::_InvokeUserCallback(E_ENGINE_PROCEDURE_TYPE eProcType)
{
	if (_vecEngineCallbacks.empty())
		return;

	CATCH_ALL_EXCEPTIONS(_eInitFlags & EIF_CATCH_UNHANDLED, InstIdx(), 
	for (size_t i = 0; i < _vecEngineCallbacks.size(); ++i)
		switch(eProcType)
		{
		case EPT_UPDATE: _vecEngineCallbacks[i]->Update(_uiLastUpdateDeltaTime); break;
		case EPT_RENDER: _vecEngineCallbacks[i]->Render(); break;
		case EPT_INIT: _vecEngineCallbacks[i]->Initialize(); break;
		case EPT_FREE: _vecEngineCallbacks[i]->Free(); break;
		}
	)
}

DGLE_RESULT DGLE_API CCore::RenderProfilerText(const char *pcTxt, const TColor4 &stColor)
{
	if (!_bInDrawProfilers)
		return S_FALSE;

	uint tw, th;
	
	_pResMan->pISystemFont()->GetTextDimensions(pcTxt, tw, th);
	_pResMan->pISystemFont()->Draw2D((float)_uiProfilerCurTxtXOffset, (float)_uiProfilerCurTxtYOffset, pcTxt, stColor);
	
	_uiProfilerCurTxtYOffset += th;
	_uiProfilerCurTxtMaxLength = max(_uiProfilerCurTxtMaxLength, tw);

	uint x,y, w, h;
	_pCoreRenderer->GetViewport(x, y, w, h);

	if (_uiProfilerCurTxtYOffset + th > h) 
	{
		_uiProfilerCurTxtXOffset += _uiProfilerCurTxtMaxLength;
		_uiProfilerCurTxtMaxLength = 0;
		_uiProfilerCurTxtYOffset = 0;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin)
{
	error_code error;
	if (fs::exists(pcFileName, error))
		return _LoadPlugin(pcFileName, prPlugin) ? S_OK : E_ABORT;
	else if (error)
	{
		LOG("Fail to connect plugin\""s + pcFileName + "\": " + error.message() + '.', LT_ERROR);
		return E_FAIL;
	}
	else
	{
		prPlugin = NULL;
		LOG("Plugin file \""s + pcFileName + "\" doesn't exists.", LT_ERROR);
		return S_FALSE;
	}
}

DGLE_RESULT DGLE_API CCore::DisconnectPlugin(IPlugin *pPlugin)
{
	return _UnloadPlugin(pPlugin) ? S_OK : E_INVALIDARG;
}

DGLE_RESULT DGLE_API CCore::GetPlugin(const char *pcPluginName, IPlugin *&prPlugin)
{
	for (size_t i = 0; i < _vecPlugins.size(); ++i)
	{
		TPluginInfo info;
		_vecPlugins[i].pPlugin->GetPluginInfo(info);
		if (strcmp(pcPluginName,info.cName) == 0)
		{
			prPlugin = _vecPlugins[i].pPlugin;
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

bool CCore::_UnloadPlugin(IPlugin *pPlugin)
{
	for (size_t i = 0; i < _vecPlugins.size(); ++i)
		if (_vecPlugins[i].pPlugin == pPlugin)
		{
			TPluginInfo info;
			_vecPlugins[i].pPlugin->GetPluginInfo(info);

			void (DGLE_API *pFreePlugin)(IPlugin *plugin) = NULL;

			pFreePlugin = reinterpret_cast<void (DGLE_API *)(IPlugin *)>(GetProcAddress(_vecPlugins[i].tLib,("FreePlugin")));

			if (pFreePlugin)
				(*pFreePlugin)(_vecPlugins[i].pPlugin);
			
			if (!ReleaseDynamicLib(_vecPlugins[i].tLib))
				LOG("Can't free \""s + info.cName + "\" plugin library.", LT_ERROR);
			else
				LOG("Plugin \""s + info.cName + "\" disconnected succesfully.", LT_INFO);
			
			_vecPlugins.erase(_vecPlugins.begin() + i);
			return true;
		}

	return false;
}

bool CCore::_LoadPlugin(const string &strFileName, IPlugin *&prPlugin)
{
	prPlugin = NULL;
	TPlugin tmp;
	tmp.tLib = NULL;
	tmp.pPlugin = NULL;
	
	tmp.tLib = LoadDynamicLib(strFileName.c_str());
	
	if (!tmp.tLib)
	{
		LOG("Can't load plugin from library \"" + strFileName + "\".", LT_ERROR);
		return false;
	}

	void (DGLE_API *pInitPlugin)(IEngineCore *engineCore, IPlugin *&plugin) = NULL;
	pInitPlugin = reinterpret_cast<void (DGLE_API *)(IEngineCore *, IPlugin *&)>(GetFuncAddress(tmp.tLib, "InitPlugin"));

	if (!pInitPlugin)
	{
		LOG("Library \"" + strFileName + "\" is not a valid DGLE plugin.", LT_ERROR);
		ReleaseDynamicLib(tmp.tLib);
		return false;
	}

	(*pInitPlugin)((IEngineCore*)this, tmp.pPlugin);

	if (!tmp.pPlugin)
	{
		LOG("Failed to retrieve plugin interface from library \"" + strFileName + "\".", LT_ERROR);
		ReleaseDynamicLib(tmp.tLib);
		return false;
	}

	TPluginInfo info;
	tmp.pPlugin->GetPluginInfo(info);

	if (info.ui8PluginSDKVersion != _DGLE_PLUGIN_SDK_VER_)
	{
		LOG("Plugin \"" + strFileName + "\" SDK version differs from engine version.", LT_ERROR);
		ReleaseDynamicLib(tmp.tLib);
		return false;
	}

	_vecPlugins.push_back(tmp);

	prPlugin = tmp.pPlugin;

	LOG("Plugin \""s + info.cName + "\" " + info.cVersion +" by \"" + info.cVendor + "\" connected succesfully.", LT_INFO);
	LOG("Plugin description: \""s + info.cDescription + '\"', LT_INFO);

	return true;
}

void CCore::_PrintPluginsInfo()
{
	string tmp = "------Connected Plugins------\n";
	for (size_t i = 0; i < _vecPlugins.size(); ++i)
	{
		TPluginInfo info;
		_vecPlugins[i].pPlugin->GetPluginInfo(info);
		tmp += "- "s + info.cName + ' ' + info.cVersion + " by " + info.cVendor + '\n';
	}
	tmp += "-----------------------------";
	Console()->Write(tmp.c_str());
}

DGLE_RESULT DGLE_API CCore::LoadSplashPicture(const char *pcBmpFileName)
{
	if (_bInitedFlag)
		return S_FALSE;

	delete[] _pcCustomSplash;
	_pcCustomSplash = new char [strlen(pcBmpFileName) + 1];
	strcpy(_pcCustomSplash, pcBmpFileName);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::AddPluginToInitializationList(const char *pcFileName)
{
	if (_bInitedFlag)
		return S_FALSE;

	_vecPluginInitList.push_back(pcFileName);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::InitializeEngine(TWindowHandle tHandle, const char* pcApplicationName, const TEngineWindow &stWindowParam, uint uiUpdateInterval, E_ENGINE_INIT_FLAGS eInitFlags)
{
	if (!_bInitedFlag)
	{
		_eInitFlags = eInitFlags;

		LOG("Initializing Engine...", LT_INFO);

		if (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE && _eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
		{
			LOG("Setting both EIF_ENABLE_FLOATING_UPDATE and EIF_ENABLE_PER_FRAME_UPDATE is not allowed.", LT_FATAL);
			return E_ABORT;
		}

		if (!PlatformInit())
		{
			LOG("Can't initialize platform API.", LT_FATAL);
			return E_ABORT;
		}

		LOG("Current locale: "s + setlocale(LC_ALL, ""), LT_INFO);
		
		setlocale(LC_NUMERIC, "C");

		_stWin = stWindowParam;
		_uiUpdateInterval = uiUpdateInterval;
		_bPause = false;

		{
			CEvBeforeInit event(&_stWin, &_eInitFlags);
			CastEvent(ET_BEFORE_INITIALIZATION, &event);
		}

		bool do_spl = !((uint)_eInitFlags & EIF_NO_SPLASH) && InstIdx() == 0;

		if (do_spl)
		{
			_pSplashWindow = new CSplashWindow(InstIdx());
			_pSplashWindow->InitWindow(_pcCustomSplash);
		}

		if (_eInitFlags & EIF_CATCH_UNHANDLED) 
			InitDbgHelp(InstIdx());

		const string eng_path = GetEngineFilePath(), working_path = fs::current_path().string();

		if (eng_path == working_path)
			LOG("Working directory: \"" + working_path + '\"', LT_INFO);
		else
			LOG("Engine working directory: \"" + eng_path + "\"\nApplication working directory: \"" + working_path + '\"', LT_INFO);

		string system_info;
		GetSystemInformation(system_info, _stSysInfo);
		LOG(system_info.c_str(), LT_INFO);

		_bSndEnabled = !(_eInitFlags & EIF_FORCE_NO_SOUND);

		if (_eInitFlags & EIF_LOAD_ALL_PLUGINS)
		{
			if (!FindFilesInDir((eng_path + "plugins\\*" PLUGIN_FILE_EXTENSION).c_str(), _vecPluginInitList))
				LOG("Plugin search routine fails.", LT_ERROR);
		}
		else
		{
			string ext_fnames[] = { eng_path + "Ext" PLUGIN_FILE_EXTENSION, eng_path + "plugins\\Ext" PLUGIN_FILE_EXTENSION };

			for (int i = 0; i < _countof(ext_fnames); ++i)
			{
				error_code error;
				if (fs::exists(ext_fnames[i], error))
				{
					_vecPluginInitList.push_back(move(ext_fnames[i]));
					break;
				}
				else if (error)
					LOG("Ext plugin file error: " + error.message() + '.', LT_ERROR);
			}
		}

		_clDelUpdate.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelRender.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelInit.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelFree.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);

		Console()->RegComVar("core_allow_pause", "Pauses main process rutine when window losts focus.", &_iAllowPause, 0, 1, &_s_ConAutoPause, this);
		Console()->RegComVar("core_fps_in_caption", "Displays current fps value in window caption.", &_iFPSToCaption, 0, 1, NULL, this);
		Console()->RegComVar("core_profiler", "Displays engine core profiler.\r\0 - hide profiler.\n1 - simple draw FPS and UPS.\n2 - additionally draw performance metrics.\n3 - additionally draw memory usage metrics.", &_iDrawProfiler, 0, 3, NULL, this);
		Console()->RegComVar("core_allow_profilers", "Allow or not rendering various engine profilers.", &_iAllowDrawProfilers, 0, 1, NULL, this);
		Console()->RegComProc("core_version", "Prints engine version.", &_s_ConPrintVersion, this);
		Console()->RegComProc("core_features", "Prints list of features with which engine was build.\nw - write to logfile.", &_s_ConFeatures, this);
		Console()->RegComProc("core_list_plugins", "Prints list of connected plugins.", &_s_ConListPlugs, this);
		Console()->RegComProc("core_instidx", "Prints Instance Index of current engine unit.", &_s_InstIdx, this);
		Console()->RegComProc("core_set_mode", "Changes display mode.\nUsage: \"core_set_mode [ScrWidth] [ScrHeight] [Fullscreen(0 or 1)] [VSync(0 or 1)] [MSAA(from 1 to 8)]\"\nExample:\"core_set_mode 800 600 1 1 4\"", &_s_ConChangeMode, this);

		if (!_pMainWindow)
			if (_eInitFlags & EIF_FORCE_NO_WINDOW)
				_pMainWindow = new CDummyWindow(InstIdx());
			else
			{
				if (tHandle == NULL)
					_pMainWindow = new CMainWindow(InstIdx());
				else
				{
					_pMainWindow = new CHookedWindow(InstIdx());
					_iAllowPause = 0;
				}
			}

		_clDelOnFPSTimer.Add(&_s_OnTimer, this);
		_clDelMLoop.Add(&_s_MainLoop, this);
		_clDelMProc.Add(&_s_MessageProc, this);

		if (!_vecPluginInitList.empty())
		{
			for (size_t i = 0; i < _vecPluginInitList.size(); ++i)
				for (size_t j = i + 1; j < _vecPluginInitList.size(); ++j)
					if (ToUpperCase(fs::path(_vecPluginInitList[i]).replace_extension().string()) == ToUpperCase(fs::path(_vecPluginInitList[j]).replace_extension().string()))
					{
						_vecPluginInitList.erase(_vecPluginInitList.begin() + j);
						LOG("Found duplicated plugin \"" + fs::path(_vecPluginInitList[i]).string() + "\" in plugins initialization list.", LT_WARNING);
					}

			for (size_t i = 0; i < _vecPluginInitList.size(); ++i)
			{
				IPlugin *plugin;
				if (S_OK == ConnectPlugin(_vecPluginInitList[i].c_str(), plugin))
				{
					char *p_name;
					uint chars_cnt;

					if (FAILED(plugin->GetPluginInterfaceName(NULL, chars_cnt)))
					{
						LOG("Failed to get plugin interface name length.", LT_ERROR);
						_UnloadPlugin(plugin);
						continue;
					}

					p_name = new char[chars_cnt];

					if (FAILED(plugin->GetPluginInterfaceName(p_name, chars_cnt)))
					{
						LOG("Failed to get plugin interface name.", LT_ERROR);
						_UnloadPlugin(plugin);
						continue;
					}
				
					if (strcmp(p_name, "ISubSystemPlugin") == 0)
					{
						IEngineSubSystem *pss;
						((ISubSystemPlugin*)plugin)->GetSubSystemInterface(pss);
						
						E_ENGINE_SUB_SYSTEM sst;
						pss->GetType(sst);
						
						switch (sst)
						{
						case ESS_CORE_RENDERER:
							if (!_bBuiltInRenderer)
							{
								LOG("Only one CoreRenderer plugin could be connected at the same time.", LT_WARNING);
								_UnloadPlugin(plugin);
							}
							else
							{
								_pCoreRenderer = (ICoreRenderer*)pss;
								_bBuiltInRenderer = false;
							}
							break;
						case ESS_INPUT:
							if (!_bBuiltInInput)
							{
								LOG("Only one Input plugin could be connected at the same time.", LT_WARNING);
								_UnloadPlugin(plugin);
							}
							else
							{
								_pInput = (IInput*)pss;
								_bBuiltInInput = false;
							}
							break;
						case ESS_SOUND:
							if (!_bBuiltInSound)
							{
								LOG("Only one Sound plugin could be connected at the same time.", LT_WARNING);
								_UnloadPlugin(plugin);
								break;
							}
							else
								if (_bSndEnabled)
								{ 
									_pSound = (ISound*)pss;
									_bBuiltInSound = false;
									break;
								}
						default: 
							LOG("The subsystem which plugin tries to override is not overridable.", LT_ERROR);
							_UnloadPlugin(plugin);
						}
					}

					delete[] p_name;

				}			
			}
		}

		if (_bBuiltInRenderer)
		{
#ifndef NO_BUILTIN_RENDERER
			LOG("Using builtin OpenGL Legacy renderer.", LT_INFO);
			_pCoreRenderer = new CCoreRendererGL(InstIdx());
#else
			LOG("No any Core Renderer plugin connected!", LT_FATAL);
			return E_INVALIDARG;
#endif
		}

		TCrRndrInitResults rnd_init_res;

		if (FAILED(_pCoreRenderer->Prepare(rnd_init_res)))
			return E_ABORT;

		if (FAILED(_pMainWindow->InitWindow(tHandle, rnd_init_res, &_clDelMLoop, &_clDelMProc)))
			return E_ABORT;

		if (do_spl)
		{
			TWindowHandle t_h;
			_pMainWindow->GetWindowHandle(t_h);
			_pSplashWindow->SetOwnerWindow(t_h);
		}

		if (strlen(pcApplicationName) < _sc_AppCaptionMaxLength)
			strcpy(_pcApplicationCaption, pcApplicationName);
		else
		{
			memcpy(_pcApplicationCaption, pcApplicationName, _sc_AppCaptionMaxLength - 1);
			_pcApplicationCaption[_sc_AppCaptionMaxLength - 1] = '\0';
			LOG("Application name is too long.", LT_WARNING);	   
		}

		_pMainWindow->SetCaption(_pcApplicationCaption);

		if (_uiFPSTimer = CreateTimer(1000, &_clDelOnFPSTimer), _uiFPSTimer == -1) 
			LOG("Can't set fps timer.", LT_FATAL);	   

		_pMainFS = new CMainFileSystem(InstIdx());

		if (FAILED(_pCoreRenderer->Initialize(rnd_init_res, _stWin, _eInitFlags)))
			return E_ABORT;

		_pRender = new CRender(InstIdx());

		_pResMan = new CResourceManager(InstIdx());

		if (_bSndEnabled && _bBuiltInSound)
		{
#ifndef NO_BUILTIN_SOUND
			LOG("Using builtin core audio based subsystem.", LT_INFO);
			_pSound = new CSound(InstIdx());
#else
			LOG("No any Sound plugin connected!", LT_FATAL);
			return E_INVALIDARG;
#endif
		}

		if (_bBuiltInInput)
		{
#ifndef NO_BUILTIN_INPUT
			LOG("Using builtin messages based input subsystem.", LT_INFO);
			_pInput = new CInput(InstIdx());
#else
			LOG("No any Input plugin connected!", LT_FATAL);
			return E_INVALIDARG;
#endif
		}

		_bInitedFlag = true;

		LOG("Engine Initialized.", LT_INFO);
		return S_OK;
	}
	else
	{
		LOG("Trying to reinit already inited engine.", LT_ERROR);
		return E_FAIL;
	}
	
	return S_OK;
}

void CCore::ToogleSuspendEngine(bool bSuspend)
{
	_clDelMLoop.AllowInvoke(!bSuspend);
	_clDelMProc.AllowInvoke(!bSuspend);
}

DGLE_RESULT DGLE_API CCore::SetUpdateInterval(uint uiUpdateInterval)
{
	_uiUpdateInterval = uiUpdateInterval;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetSystemInfo(TSystemInfo &stSysInfo)
{
	stSysInfo = _stSysInfo;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetCurrentWindow(TEngineWindow &stWin)
{
	stWin = _stWin;
	return S_OK;
}

void CCore::_LogWinMode(const TEngineWindow &stWin)
{
	E_WINDOW_ACCESS_TYPE e_actype;
	_pMainWindow->GetWindowAccessType(e_actype);	
	switch (e_actype)
	{
		case WAT_RESTRICTED_ACCESS:
			LOG("Restricted window access.", LT_INFO);
			break;

		case WAT_NO_ACCESS:
			LOG("Isolated window access.", LT_INFO);
			break;
	}

	LOG("Setting window mode: " + to_string(stWin.uiWidth) + 'X' + to_string(stWin.uiHeight) + (_eInitFlags & EIF_FORCE_16_BIT_COLOR ? " 16bit" : "") +
		(stWin.bFullScreen ? " Fullscreen" : " Windowed") + (stWin.bVSync ? " VSync" : "") +
		(stWin.eMultisampling != MM_NONE ? to_string(stWin.eMultisampling * 2) + "X MSAA" : "") + "...", LT_INFO);
}

DGLE_RESULT CCore::_ChangeWinMode(const TEngineWindow &stNewWin, bool bForceNoEvents)
{
	TEngineWindow wnd = stNewWin;

	if (!bForceNoEvents && wnd.bFullScreen != _stWin.bFullScreen)
	{
		CEvGoFullScreen event(wnd.uiWidth, wnd.uiHeight, wnd.bFullScreen);
		CastEvent(ET_ON_FULLSCREEN, &event);
	}

	_LogWinMode(wnd);

	if (SUCCEEDED(_pMainWindow->ConfigureWindow(wnd, !_bWasFScreen && !_bFScreenKeyIsPressed)) && SUCCEEDED(_pCoreRenderer->AdjustMode(wnd)))
	{
		_stWin = wnd;
		_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);
		Console()->ResetWinPos();
		LOG("Window mode has been set.", LT_INFO);
	}
	else
	{
		LOG("Required window mode has not been set due to some errors.", LT_ERROR);
		return E_ABORT;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ChangeWindowMode(const TEngineWindow &stNewWin)
{
	return _ChangeWinMode(stNewWin, false);
}

DGLE_RESULT DGLE_API CCore::GetDesktopResolution(uint &uiWidth, uint &uiHeight)
{
	GetDisplaySize(uiWidth, uiHeight);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::StartEngine()
{
	if (_bStartedFlag)
		return S_FALSE;

	if (FAILED(_pCoreRenderer->MakeCurrent()))
		return E_ABORT;

	_pRender->SetDefaultStates();

	if (!_clDelInit.IsNull() || !_vecEngineCallbacks.empty()) 
	{
		LOG("Calling user initialization procedure...", LT_INFO);
		
		_clDelInit.Invoke();
		
		_InvokeUserCallback(EPT_INIT);
		
		LOG("Done.", LT_INFO);
	}

	if (_pSplashWindow) 
		_pSplashWindow->Free();

	_LogWinMode(_stWin);

	if (FAILED(_pMainWindow->ConfigureWindow(_stWin, true)) || FAILED(_pCoreRenderer->AdjustMode(_stWin)))
		return E_ABORT;

	LOG("Window mode has been set.", LT_INFO);

	_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);

	_uiLastUPS = _uiUPSCount = _uiLastFPS = _uiFPSCount	= 0;
	_ui64FPSSumm = _ui64CyclesCount = 0;
	_ui64StartTime = GetPerfTimer();
	_ui64TimeOld = _ui64StartTime - _uiUpdateInterval * 1000;
	
	DGLE_RESULT hr = _pMainWindow->BeginMainLoop();

	_bStartedFlag = SUCCEEDED(hr);

	return hr;
}

DGLE_RESULT DGLE_API CCore::QuitEngine()
{
	if (_bQuitFlag)
	{
		LOG("Trying to reinit already started quit query.", LT_ERROR);
		return E_FAIL;
	}

	E_WINDOW_ACCESS_TYPE e_access;
	_pMainWindow->GetWindowAccessType(e_access);

	if (e_access != WAT_FULL_ACCESS)
	{
		LOG("Direct calling of \"QuitEngine()\" rootine is note allowed when engine doesn't own window, close the root window instead.", LT_WARNING);
		return S_FALSE;
	}

	_bQuitFlag = true;

	_pMainWindow->SendMessage(TWindowMessage(WMT_CLOSE));

	if (_eInitFlags & EIF_FORCE_NO_WINDOW)
		_pMainWindow->KillWindow();

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::AllowPause(bool bAllow)
{
	if (!_bPause)
	{
		_iAllowPause = bAllow ? 1 : 0;
		return S_OK;
	}
	else
		return S_FALSE;
}

DGLE_RESULT DGLE_API CCore::GetFPS(uint &uiFPS)
{
	uiFPS = _uiLastFPS;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetLastUpdateDeltaTime(uint &uiDeltaTime)
{
	uiDeltaTime = _uiLastUpdateDeltaTime;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetElapsedTime(uint64 &ui64ElapsedTime)
{
	const uint64 tick = GetPerfTimer();
	ui64ElapsedTime = (tick - _ui64StartTime - _ui64PauseTime - (_iAllowPause && _bPause ? tick - _ui64PauseStartTime : 0)) / 1000;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetInstanceIndex(uint &uiIdx)
{
	uiIdx = InstIdx();
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetTimer(uint64 &ui64Tick)
{
	ui64Tick = GetPerfTimer() / 1000;
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent)
{
	for (size_t i = 0; i < _vecEvents.size(); ++i)
		if (eEventType == _vecEvents[i].eType)
		{
			if (!_vecEvents[i].pDEvent->IsNull())
				_vecEvents[i].pDEvent->Invoke(pEvent);
			
			break;
		}

	CATCH_ALL_EXCEPTIONS(_eInitFlags & EIF_CATCH_UNHANDLED, InstIdx(), 
	for (size_t i = 0; i < _vecEngineCallbacks.size(); ++i)
		_vecEngineCallbacks[i]->OnEvent(eEventType, pEvent);
	)
	
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::AddEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter)
{
	if (eEventType == ET_BEFORE_INITIALIZATION && _bInitedFlag) // Means that engine is already inited and event will never happen.
		return S_FALSE;
	
	for (size_t i = 0; i < _vecEvents.size(); ++i)
		if (eEventType == _vecEvents[i].eType)
		{
			_vecEvents[i].pDEvent->Add(pListnerProc, pParameter);
			return S_OK;
		}

	TEvent new_event;
	new_event.eType = eEventType;
	new_event.pDEvent = new TEventProcDelegate(InstIdx());
	new_event.pDEvent->Add(pListnerProc, pParameter);
	new_event.pDEvent->CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);

	_vecEvents.push_back(new_event);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::RemoveEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter)
{
	for (size_t i = 0; i < _vecEvents.size(); ++i)
		if (eEventType == _vecEvents[i].eType)
		{
			_vecEvents[i].pDEvent->Remove(pListnerProc, pParameter);
			return S_OK;
		}

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CCore::AddEngineCallback(IEngineCallback *pEngineCallback)
{
	for (size_t i = 0; i < _vecEngineCallbacks.size(); ++i)
		if (_vecEngineCallbacks[i] == pEngineCallback)
			return S_FALSE;

	_vecEngineCallbacks.push_back(pEngineCallback);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::RemoveEngineCallback(IEngineCallback *pEngineCallback)
{
	for (size_t i = 0; i < _vecEngineCallbacks.size(); ++i)
		if (_vecEngineCallbacks[i] == pEngineCallback)
		{
			_vecEngineCallbacks.erase(_vecEngineCallbacks.begin() + i);
			return S_OK;
		}

	return S_FALSE;
}

DGLE_RESULT DGLE_API CCore::AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter)
{
	switch(eProcType)
	{
	case EPT_UPDATE:
		_clDelUpdate.Add(pProc, pParameter);
		break;
	case EPT_RENDER:
		_clDelRender.Add(pProc, pParameter);
		break;
	case EPT_INIT:
		_clDelInit.Add(pProc, pParameter);
		if(_bStartedFlag)
		{
			(*pProc)(pParameter);
			return S_FALSE;
		}
		break;
	case EPT_FREE:
		_clDelFree.Add(pProc, pParameter);
		break;
	default: return E_INVALIDARG;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter)
{
	switch(eProcType)
	{
	case EPT_UPDATE:
		_clDelUpdate.Remove(pProc, pParameter);
		break;
	case EPT_RENDER:
		_clDelRender.Remove(pProc, pParameter);
		break;
	case EPT_INIT:
		_clDelInit.Remove(pProc, pParameter);
		break;
	case EPT_FREE:
		_clDelFree.Remove(pProc, pParameter);
		break;
	default: return E_INVALIDARG;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::WriteToLog(const char *pcTxt)
{
	_LogWrite(pcTxt);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::WriteToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	_LogWriteEx(pcTxt, eType, pcSrcFileName, iSrcLineNumber);
	return S_OK;
}

bool DGLE_API CCore::_s_InstIdx(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CCore, "No parameters expected.");
		return false;
	}
	else
	{
		CON(CCore, ("Instance Index is " + to_string(PTHIS(CCore)->InstIdx()) + '.').c_str());
		return true;
	}
}

bool DGLE_API CCore::_s_ConFeatures(void *pParameter, const char *pcParam)
{
	bool write = strlen(pcParam) != 0 && pcParam[0] == 'w';

	const string res = "Engine was build with:\n"s +
#ifdef NDEBUG
		"* Release build.\n"
#else
		"* DEBUG build.\n"
#endif		
		+
		"* Engine SDK version: " + to_string(_DGLE_SDK_VER_) + ".\n" +
		"* Plugin SDK version: " + to_string(_DGLE_PLUGIN_SDK_VER_) + ".\n" +
#ifdef DGLE_STATIC
		"* Static library build.\n"
#endif
#ifdef PLATFORM_WINDOWS
#ifdef _WIN64
		"* Target platform Win64.\n"
#elif _WIN32
		"* Target platform Win32.\n"
#endif
#endif
#ifdef STRUCT_ALIGNMENT_1
		"* All engine structures are aligned by 1 byte.\n"
#endif
#ifdef PDB_DEBUG
		"* Unhandled Exceptions Filter with support of PDB files.\n"
#endif
#ifdef DGLE_USE_COM
		"* Support of Microsoft COM technology for engine interfaces.\n"
#endif
#ifdef PLATFORM_WINDOWS
#	ifdef NO_DIRECTX
		"* No Microsoft DirectX build.\n"
#	elif defined(DXDIAG_VIDEO_INFO)
		"* Uses Microsoft DirectX diagnostic information.\n"
#	endif
#endif
#ifdef NO_BUILTIN_RENDERER
		"* No builtin Renderer.\n"
#else
		"* Builtin OpenGL Legacy Renderer.\n"
#endif
#ifdef NO_BUILTIN_SOUND
		"* No builtin Sound Subsystem.\n"
#else
		"* Builtin Sound Subsystem.\n"
#endif
#ifdef NO_BUILTIN_INPUT
		"* No builtin Input Subsystem.\n"
#else
		"* Builtin Input Subsystem.\n"
#endif
		;

	if (write)
		LOG2(CCore, res, LT_INFO);
	else
		CON(CCore, res.c_str());

	return true;
}

bool DGLE_API CCore::_s_ConPrintVersion(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CCore, "No parameters expected.");
		return false;
	}
	else 
	{
		CON(CCore, ("Engine version: "s + DGLE_VERSION).c_str());
		return true;
	}
}

bool DGLE_API CCore::_s_ConAutoPause(void *pParameter, const char *pcParam)
{
	if (PTHIS(CCore)->_bPause)
	{
		CON(CCore, "Value can not be changed while in engine is paused.");
		return false;
	}
	else
		return true;
}

bool DGLE_API CCore::_s_ConListPlugs(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CCore, "No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CCore)->_PrintPluginsInfo();
		return true;
	}
}

bool DGLE_API CCore::_s_ConChangeMode(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) == 0)
	{
		CON(CCore, "Parameters expected.");
		return false;
	}
	else
	{
		TEngineWindow wnd; int samples;
		istrstream params_str(pcParam);
		params_str >> wnd.uiWidth >> wnd.uiHeight >> wnd.bFullScreen >> wnd.bVSync >> samples;

		switch (samples)
		{
		case 2: wnd.eMultisampling = MM_2X; break;
		case 4: wnd.eMultisampling = MM_4X; break;
		case 8: wnd.eMultisampling = MM_8X; break;
		case 16: wnd.eMultisampling = MM_16X; break;
		default: wnd.eMultisampling = MM_NONE;
		}

		wnd.uiFlags = PTHIS(CCore)->_stWin.uiFlags;
		PTHIS(CCore)->_stWndToApply = wnd;
		PTHIS(CCore)->_bNeedApplyNewWnd = true;

		return true;
	}
}

DGLE_RESULT DGLE_API CCore::GetWindowHandle(TWindowHandle &tHandle)
{
	_pMainWindow->GetWindowHandle(tHandle);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleVisible(bool bIsVisible)
{
	Console()->Visible(bIsVisible);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleWrite(const char *pcTxt, bool bWriteToPreviousLine)
{
	Console()->Write(pcTxt, bWriteToPreviousLine);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleExecute(const char *pcCommandTxt)
{
	Console()->Exec(pcCommandTxt);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleRegisterCommand(const char *pcCommandName, const char *pcCommandHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter)
{
	Console()->RegComProc(pcCommandName, pcCommandHelp, pProc, pParameter);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleRegisterVariable(const char *pcCommandName, const char *pcCommandHelp, int *piVar, int iMinValue, int iMaxValue, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter)
{
	Console()->RegComVar(pcCommandName, pcCommandHelp, piVar, iMinValue, iMaxValue, pProc, pParameter);
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::ConsoleUnregister(const char *pcCommandName)
{
	if (Console()->UnRegCom(pcCommandName))
		return S_OK;
	else
		return S_FALSE;
}

DGLE_RESULT DGLE_API CCore::GetVersion(char *pcBuffer, uint &uiBufferSize)
{
	if (!pcBuffer)
	{
		uiBufferSize = strlen(DGLE_VERSION) + 1;
		return S_OK;
	}

	if (uiBufferSize < strlen(DGLE_VERSION))
		return E_INVALIDARG;

	strcpy(pcBuffer, DGLE_VERSION);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem)
{
	switch(eSubSystem)
	{
		case ESS_CORE_RENDERER:
			prSubSystem = (IEngineSubSystem*)_pCoreRenderer;
			break;

		case ESS_RENDER:
			prSubSystem = (IEngineSubSystem*)_pRender;
			break;

		case ESS_RESOURCE_MANAGER:
			prSubSystem = (IEngineSubSystem*)_pResMan;
			break;		

		case ESS_FILE_SYSTEM:
			prSubSystem = (IEngineSubSystem*)_pMainFS;
			break;		

		case ESS_INPUT:
			if(_pInput == NULL)
			{
				prSubSystem = NULL;
				return E_NOTIMPL;
			}
			else
				prSubSystem = (IEngineSubSystem*)_pInput;
			break;	

		case ESS_SOUND:
			if(_pSound == NULL)
			{
				prSubSystem = NULL;
				return E_NOTIMPL;
			}
			else
				prSubSystem = (IEngineSubSystem*)_pSound;
			break;

		default:
			return E_INVALIDARG;
	}

	{
		CEvGetSubSystem event(eSubSystem, prSubSystem);
		CastEvent(ET_ON_GET_SUBSYSTEM, &event);
	}

	return S_OK;
}

void DGLE_API CCore::_s_OnTimer(void *pParameter)
{
	PTHIS(CCore)->_OnTimer();
}

void DGLE_API CCore::_s_MainLoop(void *pParameter)
{
	PTHIS(CCore)->_MainLoop();
}

void DGLE_API CCore::_s_MessageProc(void *pParameter, const TWindowMessage &stMsg)
{
	PTHIS(CCore)->_MessageProc(stMsg);
}
