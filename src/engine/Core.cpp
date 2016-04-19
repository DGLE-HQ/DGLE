/**
\author		Korotkov Andrey aka DRON
\date		19.04.2016 (c)Korotkov Andrey

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
using namespace placeholders;
using namespace chrono;
using namespace fs;

extern bool bUnhandledFilterEnabled;

namespace
{
	template <class Compare>
	class PathCompare
	{
		Compare cmp;

	public:
		PathCompare(const Compare &compare) : cmp(compare) {}

	public:
		bool operator ()(path left, path right) const
		{
			return cmp(ToUpperCase(canonical(left.replace_extension()).native()), ToUpperCase(canonical(right.replace_extension()).native()));
		}
	};

	template <class Compare>
	inline PathCompare<Compare> MakePathCompare(const Compare &compare)
	{
		return{ compare };
	}
}

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

inline CCore::CConnectionsTracker::GenericConnection::GenericConnection(Signals::ScopedConnection<IBaseEvent *> &&eventConnection) noexcept :
eventConnection(move(eventConnection)), _type(ConnectionType::Event) {}

inline CCore::CConnectionsTracker::GenericConnection::GenericConnection(Signals::ScopedConnection<> &&procConnection) noexcept :
procConnection(move(procConnection)), _type(ConnectionType::Proc) {}

CCore::CConnectionsTracker::GenericConnection::GenericConnection(GenericConnection &&connection) noexcept : _type(connection._type)
{
	switch (_type)
	{
	case ConnectionType::Event:
		new(&eventConnection) decltype(eventConnection)(move(connection.eventConnection));
		break;
	case ConnectionType::Proc:
		new(&procConnection) decltype(procConnection)(move(connection.procConnection));
		break;
	default:
		assert(false);
		__assume(false);
	}
}

auto CCore::CConnectionsTracker::GenericConnection::operator =(GenericConnection &&connection) noexcept -> GenericConnection &
{
	switch (_type = connection._type)
	{
	case ConnectionType::Event:
		eventConnection = move(connection.eventConnection);
		break;
	case ConnectionType::Proc:
		procConnection = move(connection.procConnection);
		break;
	default:
		assert(false);
		__assume(false);
	}
	return *this;
}

CCore::CConnectionsTracker::GenericConnection::~GenericConnection() noexcept
{
	switch (_type)
	{
	case ConnectionType::Event:
		eventConnection.~ScopedConnection();
		break;
	case ConnectionType::Proc:
		procConnection.~ScopedConnection();
		break;
	default:
		assert(false);
		__assume(false);
	}
}

inline void CCore::CConnectionsTracker::Add(Slot slot, Signals::ScopedConnection<IBaseEvent *> &&eventConnection)
{
	Add(slot, GenericConnection(move(eventConnection)));
}

inline void CCore::CConnectionsTracker::Add(Slot slot, Signals::ScopedConnection<> &&procConnection)
{
	Add(slot, GenericConnection(move(procConnection)));
}

void CCore::CConnectionsTracker::Add(Slot slot, GenericConnection &&connection)
{
	_connections.emplace(slot, move(connection));
}

void CCore::CConnectionsTracker::Remove(Slot slot)
{
	_connections.erase(slot);
}

CCore::CCore(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_pMainWindow(), _uiFPSTimer(),
_uiLogWarningsCount(), _uiLogErrorsCount(),
_pRender(), _pMainFS(), _pResMan(), _pInput(), _pSound(), _pCoreRenderer(),
_bBuiltInSound(true), _bBuiltInRenderer(true), _bBuiltInInput(true),
_pSplashWindow(), _bCmdKeyIsPressed(false), _bFScreenKeyIsPressed(false),
_bDoExit(false), _bInDrawProfilers(false), _bWasFScreen(false), _bRendering(false),
_bStartedFlag(false), _bInitedFlag(false), _bQuitFlag(false),
_iAllowPause(1), _iFPSToCaption(0), _iAllowDrawProfilers(1), _iDrawProfiler(0),
_bNeedApplyNewWnd(false), _lastUpdateDeltaTime(), _uiLastMemUsage(),
_startTime(), _pauseTime(),
//Delegates initialization
_clDelUpdate(piecewise_construct, make_tuple(), make_tuple(uiInstIdx)),
_clDelRender(piecewise_construct, make_tuple(), make_tuple(uiInstIdx)),
_clDelInit(piecewise_construct, make_tuple(), make_tuple(uiInstIdx)),
_clDelFree(piecewise_construct, make_tuple(), make_tuple(uiInstIdx)),
_clDelMLoop(piecewise_construct, make_tuple(), make_tuple(uiInstIdx)),
_clDelMProc(uiInstIdx),
_clDelOnFPSTimer(piecewise_construct, make_tuple(), make_tuple(uiInstIdx))
{
	_pcCustomSplash = new char [1];
	_pcCustomSplash[0] = '\0';

	if (!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_NO_LOG_FILE))
	{
		_clLogFile.setf(ios_base::right, ios_base::adjustfield);
		_clLogFile.open(("log" + (InstIdx() != 0 ? to_string(InstIdx()) : "") + ".txt").c_str(), ios::out | ios::trunc);

		time_t cur_time;
		time(&cur_time);

		_clLogFile << "DGLEngine Log File" << endl;
		_clLogFile << "Version: "<< DGLE_VERSION << endl;
		_clLogFile << "Visit http://dglengine.org/ for more information." << endl;
		_clLogFile << "Log Started at " << put_time(localtime(&cur_time), "%d.%m.%Y.") << endl;
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

		CastEvent(ET_ON_ENGINE_FATAL_MESSAGE, (IBaseEvent *)ev_fatal_msg);

		if (((CEvFatalMessage *)ev_fatal_msg)->DoShowMessage())
		{
			Console()->Visible(true);
								
			if (_pSplashWindow)
			{
				_pSplashWindow->Free();
				_pSplashWindow = NULL;
			}

			ShowModalUserAlert(pcTxt, "DGLE Fatal Error!");
		}

		if (((CEvFatalMessage *)ev_fatal_msg)->DoHalt())
		{
			delete ev_fatal_msg;
			delete this;
			quick_exit(2);
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
		using namespace chrono;
		const auto now = system_clock::now();
		const time_t cur_time = system_clock::to_time_t(now);
		const milliseconds ms = duration_cast<milliseconds>(now - floor<seconds>(now));
		_clLogFile.fill('0');
		_clLogFile << put_time(localtime(&cur_time), "%T.") << setw(3) << ms.count() << setw(0) << " - " << pcTxt << '\n';

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

		_clDelMLoop.second();

		break;

	case WMT_CLOSE:

		_bDoExit = true;

		break;

	case WMT_DESTROY:

		LOG("Finalizing Engine...", LT_INFO);

		_pCoreRenderer->MakeCurrent();

		if (!_clDelFree.second.IsNull() || !_vecEngineCallbacks.empty()) 
		{
			LOG("Calling user finalization procedure...", LT_INFO);
			
			_InvokeUserCallback(EPT_FREE);

			_clDelFree.second();	
			
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
			delete (CInput *)_pInput;
#endif

#ifndef NO_BUILTIN_SOUND
		if (_bSndEnabled && _bBuiltInSound)
			delete (CSound *)_pSound;
#endif

#ifndef NO_BUILTIN_RENDERER
		if (_bBuiltInRenderer)
			delete (CCoreRendererGL *)_pCoreRenderer;
#endif

		delete _pMainFS;

		_pMainWindow->Free();

		fill(begin(_events), end(_events), nullptr);

		break;

	case WMT_ACTIVATED:

		if (stMsg.ui32Param1 == 1)
			break;

		_bPause = false;	

		if (_iAllowPause)
		{
			_pMainWindow->SetCaption(_pcApplicationCaption);
			if (_pSound != NULL) _pSound->MasterPause(false);
			_pauseTime += GetPerfTimer<microseconds>() - _pauseStartTime;
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
			_pauseStartTime = GetPerfTimer<microseconds>();
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

	const microseconds time = GetPerfTimer<microseconds>();
	const auto time_delta = time - _timeOld;

	bool flag = false;

	Console()->EnterThreadSafeSection();

	uint cycles_cnt;

	if (_eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
		cycles_cnt = 1;
	else
		if (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE)
			cycles_cnt = time_delta >= _updateInterval;
		else
		{
			cycles_cnt = time_delta / _updateInterval;

			if (cycles_cnt > _sc_MaxUpdateCycles)
				cycles_cnt = _sc_MaxUpdateCycles;
		}

	if (cycles_cnt > 0)
	{
		if (_eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE)
			_lastUpdateDeltaTime = _stWin.bVSync && _uiLastFPS >= 5 ? 200ms / (_uiLastFPS / 5) : duration_cast<milliseconds>(time_delta);
		else
			_lastUpdateDeltaTime = (_eInitFlags & EIF_ENABLE_FLOATING_UPDATE) ? duration_cast<milliseconds>(time_delta) : _updateInterval;
		
		_updateDelay = GetPerfTimer<microseconds>();
	}
	
	for (uint i = 0; i < cycles_cnt; ++i)
	{
		if (((!_bPause && _iAllowPause) || !_iAllowPause) && (!_clDelUpdate.second.IsNull() || !_vecEngineCallbacks.empty()) && !_bQuitFlag) 
		{
			if (i == cycles_cnt - 1)
				_pRender->RefreshBatchData();

			_clDelUpdate.second();
			
			_InvokeUserCallback(EPT_UPDATE);

			++_uiUPSCount;
		}

		flag = true;
	}

	if (flag)
	{
		_updateDelay = GetPerfTimer<microseconds>() - _updateDelay;
		
		_timeOld = time;
		if (!(_eInitFlags & EIF_ENABLE_FLOATING_UPDATE || _eInitFlags & EIF_ENABLE_PER_FRAME_UPDATE))
			_timeOld -= time_delta % _updateInterval;
	}

	_RenderFrame();

	Console()->LeaveThreadSafeSection();

	const milliseconds sleep = (int)((_eInitFlags & EIF_FORCE_LIMIT_FPS) && ((_uiLastFPS > _updateInterval.count() && _uiLastFPS > 120) || _bPause)) * 8ms +
		(int)(_bPause && _iAllowPause) * 15ms + (int)(_stSysInfo.uiCPUCount < 2 && cycles_cnt < 2) * 6ms;

	if (sleep > sleep.zero())
		this_thread::sleep_for(sleep);
}

void CCore::_RenderFrame()
{
	_bRendering = true;

	_pRender->BeginRender();

	_renderDelay = GetPerfTimer<microseconds>();

	_pRender->BeginFrame();

	{
		CBaseEvent event(ET_BEFORE_RENDER);
		CastEvent(ET_BEFORE_RENDER, &event);
	}

	_clDelRender.second();
	
	_InvokeUserCallback(EPT_RENDER);

	{
		CBaseEvent event(ET_AFTER_RENDER);
		CastEvent(ET_AFTER_RENDER, &event);
	}

	_pRender->EndFrame();
	
	_renderDelay = GetPerfTimer<microseconds>() - _renderDelay;

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
			RenderProfilerText(("UPS:" + to_string(_uiLastUPS)).c_str(), _uiLastUPS < 1s / _updateInterval ? ColorRed() : ColorWhite());

			if (_iDrawProfiler > 1)
			{
				RenderProfilerText(GetProfilerDelayText("Render delay:", _renderDelay).c_str(), ColorWhite());
				RenderProfilerText(GetProfilerDelayText("Update delay:", _updateDelay).c_str(), ColorWhite());
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
	const auto invoke = [=]
	{
		for (const auto callback : _vecEngineCallbacks)
			switch (eProcType)
			{
			case EPT_UPDATE:	callback->Update(_lastUpdateDeltaTime.count());	break;
			case EPT_RENDER:	callback->Render();								break;
			case EPT_INIT:		callback->Initialize();							break;
			case EPT_FREE:		callback->Free();								break;
			}
	};

	CATCH_ALL_EXCEPTIONS(_eInitFlags & EIF_CATCH_UNHANDLED, InstIdx(), invoke();)
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
	if (exists(pcFileName, error))
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
	for (const auto &plugin : _vecPlugins)
	{
		TPluginInfo info;
		plugin.pPlugin->GetPluginInfo(info);
		if (strcmp(pcPluginName, info.cName) == 0)
		{
			prPlugin = plugin.pPlugin;
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

bool CCore::_UnloadPlugin(IPlugin *pPlugin)
{
	const auto found = find_if(_vecPlugins.cbegin(), _vecPlugins.cend(), [pPlugin](decltype(_vecPlugins)::const_reference plugin) { return plugin.pPlugin == pPlugin; });
	if (found != _vecPlugins.cend())
	{
		TPluginInfo info;
		found->pPlugin->GetPluginInfo(info);

		void (DGLE_API *pFreePlugin)(IPlugin *plugin) = NULL;

		pFreePlugin = reinterpret_cast<void (DGLE_API *)(IPlugin *)>(GetProcAddress(found->tLib, "FreePlugin"));

		if (pFreePlugin)
			pFreePlugin(found->pPlugin);
			
		if (!ReleaseDynamicLib(found->tLib))
			LOG("Can't free \""s + info.cName + "\" plugin library.", LT_ERROR);
		else
			LOG("Plugin \""s + info.cName + "\" disconnected succesfully.", LT_INFO);
			
		_vecPlugins.erase(found);
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

	(*pInitPlugin)((IEngineCore *)this, tmp.pPlugin);

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
	for (const auto &plugin : _vecPlugins)
	{
		TPluginInfo info;
		plugin.pPlugin->GetPluginInfo(info);
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

DGLE_RESULT DGLE_API CCore::InitializeEngine(TWindowHandle tHandle, const char *pcApplicationName, const TEngineWindow &stWindowParam, uint uiUpdateInterval, E_ENGINE_INIT_FLAGS eInitFlags)
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
		_updateInterval = milliseconds(uiUpdateInterval);
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

		const string eng_path = GetEngineFilePath(), working_path = current_path().string() + '\\';

		error_code error;
		if (canonical(eng_path) == canonical(working_path) || equivalent(eng_path, working_path, error) && error)
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

			for (string &name : ext_fnames)
			{
				error_code error;
				if (exists(name, error))
				{
					_vecPluginInitList.push_back(move(name));
					break;
				}
				else if (error)
					LOG("Ext plugin file error: " + error.message() + '.', LT_ERROR);
			}
		}

		_clDelUpdate.second.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelRender.second.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelInit.second.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelFree.second.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);

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

		_clDelOnFPSTimer.second.Add(bind(&CCore::_OnTimer, this));
		_clDelMLoop.second.Add(bind(&CCore::_MainLoop, this));
		_clDelMProc.Add(bind(&CCore::_MessageProc, this, _1));

		sort(_vecPluginInitList.begin(), _vecPluginInitList.end(), MakePathCompare(less<>()));
		// std::unique is more appropriate here and is more efficiend but prevents logging
		for (auto dup = _vecPluginInitList.cbegin(); (dup = adjacent_find(dup, _vecPluginInitList.cend(), MakePathCompare(equal_to<>()))) != _vecPluginInitList.cend();)
		{
			LOG("Found duplicated plugin \"" + *dup + "\" in plugins initialization list.", LT_WARNING);
			dup = _vecPluginInitList.erase(dup);
		}

		for (const auto &filename : _vecPluginInitList)
		{
			IPlugin *plugin;
			if (S_OK == ConnectPlugin(filename.c_str(), plugin))
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
					((ISubSystemPlugin *)plugin)->GetSubSystemInterface(pss);
						
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
							_pCoreRenderer = (ICoreRenderer *)pss;
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
							_pInput = (IInput *)pss;
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
								_pSound = (ISound *)pss;
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

		_vecPluginInitList.clear();

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

		if (FAILED(_pMainWindow->InitWindow(tHandle, rnd_init_res, &_clDelMLoop.second, &_clDelMProc)))
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

		if (_uiFPSTimer = CreateTimer(1000, &_clDelOnFPSTimer.second), _uiFPSTimer == -1) 
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
	_clDelMLoop.second.AllowInvoke(!bSuspend);
	_clDelMProc.AllowInvoke(!bSuspend);
}

DGLE_RESULT DGLE_API CCore::SetUpdateInterval(uint uiUpdateInterval)
{
	_updateInterval = milliseconds(uiUpdateInterval);
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

	if (!_clDelInit.second.IsNull() || !_vecEngineCallbacks.empty()) 
	{
		LOG("Calling user initialization procedure...", LT_INFO);
		
		_clDelInit.second();
		
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
	_startTime = GetPerfTimer<microseconds>();
	_timeOld = _startTime - _updateInterval;
	
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
	uiDeltaTime = _lastUpdateDeltaTime.count();
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetElapsedTime(uint64 &ui64ElapsedTime)
{
	const microseconds tick = GetPerfTimer<microseconds>();
	ui64ElapsedTime = duration_cast<milliseconds>(tick - _startTime - _pauseTime - (_iAllowPause && _bPause ? tick - _pauseStartTime : 0us)).count();
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetInstanceIndex(uint &uiIdx)
{
	uiIdx = InstIdx();
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::GetTimer(uint64 &ui64Tick)
{
	ui64Tick = GetPerfTimer<milliseconds>().count();
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent)
{
	if (_events[eEventType])
		_events[eEventType]->second(pEvent);

	const auto notify_callbacks = [=]
	{
		for_each(_vecEngineCallbacks.cbegin(), _vecEngineCallbacks.cend(), bind(&remove_pointer_t<decltype(_vecEngineCallbacks)::value_type>::OnEvent, _1, eEventType, pEvent));
	};

	CATCH_ALL_EXCEPTIONS(_eInitFlags & EIF_CATCH_UNHANDLED, InstIdx(), notify_callbacks();)
	
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::AddEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter)
{
	if (eEventType == ET_BEFORE_INITIALIZATION && _bInitedFlag) // Means that engine is already inited and event will never happen.
		return S_FALSE;

	if (!_events[eEventType])
	{
		_events[eEventType] = make_unique<remove_extent_t<decltype(_events)>::element_type>(CConnectionsTracker(), InstIdx());
		_events[eEventType]->second.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
	}
	_events[eEventType]->first.Add({ pListnerProc, pParameter }, _events[eEventType]->second.Add(bind(pListnerProc, pParameter, _1)));

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::RemoveEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter)
{
	if (!_events[eEventType])
		return E_INVALIDARG;

	_events[eEventType]->first.Remove({ pListnerProc, pParameter });
	return S_OK;
}

DGLE_RESULT DGLE_API CCore::AddEngineCallback(IEngineCallback *pEngineCallback)
{
	if (find(_vecEngineCallbacks.cbegin(), _vecEngineCallbacks.cend(), pEngineCallback) != _vecEngineCallbacks.cend())
		return S_FALSE;

	_vecEngineCallbacks.push_back(pEngineCallback);

	return S_OK;
}

DGLE_RESULT DGLE_API CCore::RemoveEngineCallback(IEngineCallback *pEngineCallback)
{
	const auto found = find(_vecEngineCallbacks.cbegin(), _vecEngineCallbacks.cend(), pEngineCallback);
	if (found != _vecEngineCallbacks.cend())
	{
		_vecEngineCallbacks.erase(found);
		return S_OK;
	}

	return S_FALSE;
}

inline auto CCore::_SelectProcDelegate(E_ENGINE_PROCEDURE_TYPE eProcType) noexcept -> pair<CConnectionsTracker, TProcDelegate> *
{
	switch (eProcType)
	{
	case EPT_UPDATE:
		return &_clDelUpdate;
	case EPT_RENDER:
		return &_clDelRender;
	case EPT_INIT:
		return &_clDelInit;
	case EPT_FREE:
		return &_clDelFree;
	}
	return nullptr;
}

DGLE_RESULT DGLE_API CCore::AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter)
{
	if (const auto delegate = _SelectProcDelegate(eProcType))
	{
		delegate->first.Add({ pProc, pParameter }, delegate->second.Add(bind(pProc, pParameter)));
		if (eProcType == EPT_INIT && _bStartedFlag)
		{
			pProc(pParameter);
			return S_FALSE;
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CCore::RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter)
{
	if (const auto delegate = _SelectProcDelegate(eProcType))
	{
		delegate->first.Remove({ pProc, pParameter });
		return S_OK;
	}
	return E_INVALIDARG;
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
	switch (eSubSystem)
	{
		case ESS_CORE_RENDERER:
			prSubSystem = (IEngineSubSystem *)_pCoreRenderer;
			break;

		case ESS_RENDER:
			prSubSystem = (IEngineSubSystem *)_pRender;
			break;

		case ESS_RESOURCE_MANAGER:
			prSubSystem = (IEngineSubSystem *)_pResMan;
			break;		

		case ESS_FILE_SYSTEM:
			prSubSystem = (IEngineSubSystem *)_pMainFS;
			break;		

		case ESS_INPUT:
			if (_pInput == NULL)
			{
				prSubSystem = NULL;
				return E_NOTIMPL;
			}
			else
				prSubSystem = (IEngineSubSystem *)_pInput;
			break;	

		case ESS_SOUND:
			if (_pSound == NULL)
			{
				prSubSystem = NULL;
				return E_NOTIMPL;
			}
			else
				prSubSystem = (IEngineSubSystem *)_pSound;
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