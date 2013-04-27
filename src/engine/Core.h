/**
\author		Korotkov Andrey aka DRON
\date		12.04.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifdef PLATFORM_WINDOWS
	const E_KEYBOARD_KEY_CODES c_eFScreenKeyFirst[2] = {KEY_LALT, KEY_RALT};
	const E_KEYBOARD_KEY_CODES c_eFScreenKeySecond = KEY_RETURN;
#endif

class CMainFS;
class CResourceManager;
class CRender;

void InitDbgHelp(uint InstIdx);

struct TPlugin
{
	TDynLibHandle tLib;
	IPlugin *pPlugin;
};

struct TEvent
{
	E_EVENT_TYPE eType;
	TEventProcDelegate *pDEvent;
};

// CCore //

class CCore: public CInstancedObj, public IEngineCore
#ifdef DGLE_USE_COM
, public IEngineCoreWrapper
#endif
{
	static const uint _sc_MaxUpdateCycles = 10;
	static const uint _sc_AppCaptionMaxLength = 128;

	TProcDelegate _clDelUpdate, _clDelRender, _clDelInit, _clDelFree,
		_clDelMLoop, _clDelOnFPSTimer;
	TMsgProcDelegate _clDelMProc;

	std::vector<IEngineCallback *> _vecEngineCallbacks;

	std::vector<TEvent> _vecEvents;

	std::fstream _clLogFile;
	uint _uiLogWarningsCount, _uiLogErrorsCount;
	IMainWindow *_pMainWindow;
	
	char *_pcCustomSplash;
	ISplashWindow *_pSplashWindow;

	bool _bCmdKeyIsPressed, _bFScreenKeyIsPressed;

	/** \remark: Setting _bDoExit to true is the the ONLY correct way to quit engine. */
	bool _bDoExit;

	TEngineWindow _stWin;

	uint _uiUpdateInterval, _uiLastUpdateDeltaTime; // in milliseconds
	uint64 _ui64TimeOld, _ui64StartTime, _ui64PauseTime, _ui64PauseStartTime,  // in microseconds
		_ui64RenderDelay, _ui64UpdateDelay;

	bool _bPause, _bWasFScreen;
	char _pcApplicationCaption[_sc_AppCaptionMaxLength];

	bool _bInitedFlag, _bStartedFlag, _bQuitFlag;

	uint _uiFPSTimer, _uiFPSCount, _uiLastFPS,
		_uiUPSCount, _uiLastUPS, _uiLastMemUsage;

	uint64 _ui64FPSSumm, _ui64CyclesCount;

	bool _bInDrawProfilers;
	uint _uiProfilerCurTxtXOffset, _uiProfilerCurTxtYOffset,
		_uiProfilerCurTxtMaxLength;

	bool _bSndEnabled;

	E_ENGINE_INIT_FLAGS _eInitFlags;

	TSystemInfo _stSysInfo;

	CMainFS *_pMainFS;
	CResourceManager *_pResMan;
	CRender *_pRender;

	ISound *_pSound;
	ICoreRenderer *_pCoreRenderer;
	IInput *_pInput;

	bool _bBuiltInSound, _bBuiltInRenderer, _bBuiltInInput;

	std::vector<std::string> _vecPluginInitList;
	std::vector<TPlugin> _vecPlugins;
	bool _LoadPlugin(const std::string &strFileName, IPlugin *&prPlugin);
	bool _UnloadPlugin(IPlugin *pPlugin);
	void _PrintPluginsInfo();

	void _MainLoop();
	void _OnTimer();
	void _MessageProc(const TWindowMessage &msg);

	void _RenderFrame();
	bool _bRendering;

	void _InvokeUserCallback(E_ENGINE_PROCEDURE_TYPE eProcType);

	void _LogWriteEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
	void _LogWrite(const char *pcTxt, bool bFlush = false);
	
	int	_iAllowPause, _iFPSToCaption, _iAllowDrawProfilers, _iDrawProfiler;

	TEngineWindow _stWndToApply;
	bool _bNeedApplyNewWnd;
	DGLE_RESULT	_ChangeWinMode(const TEngineWindow &stNewWin, bool bForceNoEvents);


	static void DGLE_API _s_MainLoop(void *pParameter);
	static void DGLE_API _s_OnTimer(void *pParameter);
	static void DGLE_API _s_MessageProc(void *pParameter, const TWindowMessage &stMsg);

	static bool DGLE_API _s_ConAutoPause(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConPrintVersion(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConFeatures(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConListPlugs(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConChangeMode(void *pParameter, const char *pcParam);	
	static bool DGLE_API _s_InstIdx(void *pParameter, const char *pcParam);

public:

	CCore(uint uiInstIdx);
	~CCore();

	inline	CRender* pRender() const {return _pRender;}
	inline	CMainFS* pMainFS() const {return _pMainFS;}
	inline	CResourceManager* pResMan() const {return _pResMan;}

	inline  ICoreRenderer* pCoreRenderer() const {return _pCoreRenderer;}
	inline  IMainWindow* pMainWindow() const {return _pMainWindow;}
	inline	ISound* pSound() const {return _pSound;}
	inline	IInput* pInput() const {return _pInput;}

	inline	E_ENGINE_INIT_FLAGS InitFlags() const {return _eInitFlags;}
	inline	TMsgProcDelegate* pDMessageProc() {return &_clDelMProc;}
	inline	TProcDelegate* pDMLoopProc() {return &_clDelMLoop;}
	inline	TProcDelegate* pDFPSTimerProc() {return &_clDelOnFPSTimer;}
	inline	TEngineWindow* EngWindow() {return &_stWin;}
	
	inline	bool SoundEnabled() const {return _bSndEnabled;}
	void	ToogleSuspendEngine(bool bSuspend);

	DGLE_RESULT DGLE_API LoadSplashPicture(const char *pcBmpFileName);
	DGLE_RESULT DGLE_API AddPluginToInitializationList(const char *pcFileName);

	DGLE_RESULT DGLE_API InitializeEngine(TWindowHandle tHandle, const char* pcApplicationName, const TEngineWindow &stWindowParam, uint uiUpdateInterval, E_ENGINE_INIT_FLAGS eInitFlags);
	DGLE_RESULT DGLE_API SetUpdateInterval(uint uiUpdateInterval);
	DGLE_RESULT DGLE_API StartEngine();
	DGLE_RESULT DGLE_API QuitEngine();

	DGLE_RESULT DGLE_API ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin);
	DGLE_RESULT DGLE_API DisconnectPlugin(IPlugin *pPlugin);
	DGLE_RESULT DGLE_API GetPlugin(const char *pcPluginName, IPlugin *&prPlugin);

	DGLE_RESULT DGLE_API AddEngineCallback(IEngineCallback *pEngineCallback);
	DGLE_RESULT DGLE_API RemoveEngineCallback(IEngineCallback *pEngineCallback);

	DGLE_RESULT DGLE_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter);
	DGLE_RESULT DGLE_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter);

	DGLE_RESULT DGLE_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent);
	DGLE_RESULT DGLE_API AddEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter);
	DGLE_RESULT DGLE_API RemoveEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter);

	DGLE_RESULT DGLE_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem);

	DGLE_RESULT DGLE_API RenderFrame();
	DGLE_RESULT DGLE_API RenderProfilerText(const char *pcTxt, const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetInstanceIndex(uint &uiIdx);
	DGLE_RESULT DGLE_API GetTimer(uint64 &ui64Tick);
	DGLE_RESULT DGLE_API GetSystemInfo(TSystemInfo &stSysInfo);
	DGLE_RESULT DGLE_API GetCurrentWindow(TEngineWindow &stWin);
	DGLE_RESULT DGLE_API GetFPS(uint &uiFPS);
	DGLE_RESULT DGLE_API GetLastUpdateDeltaTime(uint &uiDeltaTime);
	DGLE_RESULT DGLE_API GetElapsedTime(uint64 &ui64ElapsedTime);
	DGLE_RESULT DGLE_API GetWindowHandle(TWindowHandle &tHandle);

	DGLE_RESULT DGLE_API ChangeWindowMode(const TEngineWindow &stNewWin);
	DGLE_RESULT DGLE_API GetDesktopResolution(uint &uiWidth, uint &uiHeight);
	DGLE_RESULT DGLE_API AllowPause(bool bAllow);

	DGLE_RESULT DGLE_API WriteToLog(const char *pcTxt);
	DGLE_RESULT DGLE_API WriteToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

	DGLE_RESULT DGLE_API ConsoleVisible(bool bIsVisible);
	DGLE_RESULT DGLE_API ConsoleWrite(const char* pcTxt, bool bWriteToPreviousLine);
	DGLE_RESULT DGLE_API ConsoleExecute(const char* pcCommandTxt);
	DGLE_RESULT DGLE_API ConsoleRegisterCommand(const char *pcCommandName, const char *pcCommandHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter); 
	DGLE_RESULT DGLE_API ConsoleRegisterVariable(const char *pcCommandName, const char *pcCommandHelp, int *piVar, int iMinValue, int iMaxValue, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter);
	DGLE_RESULT DGLE_API ConsoleUnregister(const char* pcCommandName);

	DGLE_RESULT DGLE_API GetVersion(char *pcBuffer, uint &uiBufferSize);

#ifndef DGLE_USE_COM

	IDGLE_BASE_IMPLEMENTATION(IEngineCore, INTERFACE_IMPL_END)

#else

	DGLE_RESULT DGLE_API SetWindowAPI(IMainWindow *pMainWindow)
	{
		_pMainWindow = pMainWindow;
		return S_OK;
	}

	DGLE_RESULT DGLE_API TranslateMessage(const TWindowMessage &stWinMsg)
	{
		_clDelMProc.Invoke(stWinMsg);
		return S_OK;
	}

	DGLE_RESULT DGLE_API KillEngine()
	{
		_bDoExit = true;
		_MainLoop();
		_MessageProc(TWindowMessage(WMT_DESTROY));
		_MessageProc(TWindowMessage(WMT_RELEASED));
		return S_OK;
	}

	IDGLE_BASE_DUMMY_COMMANDS_IMPL

	IDGLE_BASE_GUID_IMPL(IEngineCore)

	HRESULT CALLBACK QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		*ppvObject = NULL;

		if(::memcmp(&riid, &__uuidof(IUnknown), sizeof(GUID)) == 0) 
			*ppvObject = static_cast<IUnknown *>(this);
		else 
			if(::memcmp(&riid,&IID_IDGLE_Base, sizeof(GUID)) == 0) 
				*ppvObject = static_cast<IDGLE_Base *>(this);
			else
				if(::memcmp(&riid, &IID_IEngineCore, sizeof(GUID)) == 0) 
					*ppvObject = static_cast<IEngineCore *>(this);
				else
					if(::memcmp(&riid, &IID_IEngineCoreWrapper, sizeof(GUID)) == 0) 
						*ppvObject = static_cast<IEngineCoreWrapper *>(this);
					else
						if(::memcmp(&riid, &IID_IMainWindow, sizeof(GUID)) == 0) 
							*ppvObject = static_cast<IMainWindow *>(_pMainWindow);
						else
							return E_NOINTERFACE;
		
		return S_OK;
	}
	
	ULONG CALLBACK AddRef()
	{
		return 1;
	}
	
	ULONG CALLBACK Release()
	{
		return 1;
	}

#endif//DGLE_USE_COM

};