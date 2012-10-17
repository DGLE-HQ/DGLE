/**
\author		Korotkov Andrey aka DRON
\date		17.09.2011 (c)Korotkov Andrey

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
	static const uint _sc_MaxProcessCycles		= 10;
	static const uint _sc_AppCaptionMaxLength	= 128;

	TProcDelegate		 _clDelUpdate,
						 _clDelRender,
						 _clDelInit,
						 _clDelFree,

						 //For system use
						 _clDelMLoop,
						 _clDelOnFPSTimer;
	TMsgProcDelegate	 _clDelMProc;

	std::vector<IUserCallback *>
						 _clUserCallbacks;

	std::vector<TEvent>	 _clEvents;

	std::fstream		 _clLogFile;
	int					 _iLogWarningsCount;
	int					 _LogErrorsCount;
	IMainWindow		    *_pMainWindow;
	
	char				*_pcCustomSplash;
	ISplashWindow		*_pSplashWindow;

	bool				 _bCmdKeyIsPressed, _bFScreenKeyIsPressed;

	/** \remark: Setting _bDoExit to true is the the ONLY correct way to quit engine. */
	bool				 _bDoExit;

	TEngWindow			 _stWin;

	uint				 _uiProcessInterval;
	uint64				 _ui64TimeOld;
	bool				 _bPause, _bWasFScreen;
	char				 _pcApplicationCaption[_sc_AppCaptionMaxLength];

	bool				 _bInitedFlag,
						 _bStartedFlag,
						 _bQuitFlag;

	uint				 _uiFPSTimer, 
						 _uiFPSCount,
						 _uiLastFPS,
						 _uiUPSCount,
						 _uiLastUPS;

	uint64				 _ui64FPSSumm,
						 _ui64CiclesCount,
						 _ui64RenderDelay,
						 _ui64UpdateDelay,
						 _ui64LastUpdateDeltaTime;

	bool				 _bInDrawProfilers;
	uint				 _uiProfilerCurTxtXOffset,
						 _uiProfilerCurTxtYOffset,
						 _uiProfilerCurTxtMaxLength;

	bool				 _bSndEnabled;

	E_ENGINE_INIT_FLAGS  _eInitFlags;

	TSystemInfo			 _stSysInfo;

	CMainFS				*_pMainFS;
	CResourceManager
						*_pResMan;
	CRender				*_pRender;

	ISound				*_pSound;
	ICoreRenderer		*_pCoreRenderer;
	IInput				*_pInput;

	bool				_bBuiltInSound, _bBuiltInRenderer, _bBuiltInInput;

	std::vector<std::string> _clPluginInitList;
	std::vector<TPlugin> _clPlugins;
	bool				_LoadPlugin(const std::string &clFileName, IPlugin *&prPlugin);
	bool				_UnloadPlugin(IPlugin *pPlugin);
	void				_PrintPluginsInfo();

	void				_MainLoop();
	void				_OnTimer();
	void				_MessageProc(const TWinMessage &msg);
	
	void				_InvokeUserCallback(E_ENGINE_PROCEDURE_TYPE eProcType);

	void				_LogWriteEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
	void				_LogWrite(const char *pcTxt, bool bFlush = false);
	
	int					_iAllowPause,
						_iFPSToCaption,
						_iAllowDrawProfilers,
						_iDrawProfiler;

	TEngWindow			_stWndToApply;
	bool				_bNeedApplyNewWnd;

	static void DGLE_API _s_MainLoop(void *pParametr);
	static void DGLE_API _s_OnTimer(void *pParametr);
	static void DGLE_API _s_MessageProc(void *pParametr, const TWinMessage &stMsg);

	static void DGLE_API _s_ConAutoPause(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ConPrintVersion(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ConFeatures(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ConListPlugs(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ConChangeMode(void *pParametr, const char *pcParam);	
	static void DGLE_API _s_InstIdx(void *pParametr, const char *pcParam);

public:

	CCore(uint uiInstIdx, bool bForceSingleThreaded);
	~CCore();

	inline  ICoreRenderer* pCoreRenderer() const {return _pCoreRenderer;}
	inline	CRender* pRender() const {return _pRender;}
	inline	CMainFS* pMainFS() const {return _pMainFS;}
	inline	CResourceManager* pResMan() const {return _pResMan;}
	inline  IMainWindow* pMainWindow() const {return _pMainWindow;}
	
	inline	E_ENGINE_INIT_FLAGS InitFlags() const {return _eInitFlags;}
	inline	TMsgProcDelegate* pDMessageProc() {return &_clDelMProc;}
	inline	TProcDelegate* pDMLoopProc() {return &_clDelMLoop;}
	inline	TProcDelegate* pDFPSTimerProc() {return &_clDelOnFPSTimer;}
	inline	TEngWindow* EngWindow() {return &_stWin;}
	inline	bool SoundEnabled() const {return _bSndEnabled;}

	void	ToogleSuspendEngine(bool bSuspend);

	DGLE_RESULT DGLE_API LoadSplashPicture(const char *pcBmpFileName);
	DGLE_RESULT DGLE_API AddPluginToInitList(const char *pcFileName);

	DGLE_RESULT DGLE_API InitializeEngine(TWinHandle tHandle, const char* pcApplicationName, const TEngWindow &stWindowParam, uint uiProcessInterval, E_ENGINE_INIT_FLAGS eInitFlags);
	DGLE_RESULT DGLE_API SetProcessInterval(uint uiProcessInterval);
	DGLE_RESULT DGLE_API StartEngine();
	DGLE_RESULT DGLE_API QuitEngine();

	DGLE_RESULT DGLE_API ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin);
	DGLE_RESULT DGLE_API DisconnectPlugin(IPlugin *pPlugin);
	DGLE_RESULT DGLE_API GetPlugin(const char *pcPluginName, IPlugin *&prPlugin);

	DGLE_RESULT DGLE_API AddUserCallback(IUserCallback *pUserCallback);
	DGLE_RESULT DGLE_API RemoveUserCallback(IUserCallback *pUserCallback);

	DGLE_RESULT DGLE_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParametr), void *pParametr);
	DGLE_RESULT DGLE_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParametr), void *pParametr);

	DGLE_RESULT DGLE_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent);
	DGLE_RESULT DGLE_API AddEventListner(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);
	DGLE_RESULT DGLE_API RemoveEventListner(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);

	DGLE_RESULT DGLE_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem);

	DGLE_RESULT DGLE_API RenderProfilerTxt(const char *pcTxt, const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetInstanceIdx(uint &uiIdx);
	DGLE_RESULT DGLE_API GetTimer(uint64 &uiTick);
	DGLE_RESULT DGLE_API GetSystemInfo(TSystemInfo &stSysInfo);
	DGLE_RESULT DGLE_API GetCurrentWin(TEngWindow &stWin);
	DGLE_RESULT DGLE_API GetFPS(uint &uiFPS);
	DGLE_RESULT DGLE_API GetLastUpdateDeltaTime(uint64 &ui64DeltaTime);
	DGLE_RESULT DGLE_API GetHandle(TWinHandle &tHandle);

	DGLE_RESULT DGLE_API ChangeWinMode(const TEngWindow &stNewWin);
	DGLE_RESULT DGLE_API AllowPause(bool bAllow);

	DGLE_RESULT DGLE_API AddToLog(const char *pcTxt);
	DGLE_RESULT DGLE_API AddToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

	DGLE_RESULT DGLE_API ConsoleVisible(bool bIsVisible);
	DGLE_RESULT DGLE_API ConsoleWrite(const char* pcTxt, bool bWriteToPreviousLine);
	DGLE_RESULT DGLE_API ConsoleExec(const char* pcCommandTxt);
	DGLE_RESULT DGLE_API ConsoleRegComProc(const char *pcCommandName, const char *pcCommandHelp, void (DGLE_API *pProc)(void *pParametr, const char *pcParam), void *pParametr); 
	DGLE_RESULT DGLE_API ConsoleRegComValue(const char *pcCommandName, const char *pcCommandHelp, int *piValue, int iMinValue, int iMaxValue, void (DGLE_API *pProc)(void *pParametr, const char *pcParam), void *pParametr);
	DGLE_RESULT DGLE_API ConsoleUnregCom(const char* pcCommandName);

	DGLE_RESULT DGLE_API GetVersion(char* pcBuffer, uint uiBufferSize);

#ifndef DGLE_USE_COM

	IDGLE_BASE_IMPLEMENTATION(IEngineCore)

#else

	DGLE_RESULT DGLE_API SetWindowAPI(IMainWindow *pMainWindow)
	{
		_pMainWindow = pMainWindow;
		return S_OK;
	}

	DGLE_RESULT DGLE_API TranslateMessage(const TWinMessage &stWinMsg)
	{
		_clDelMProc.Invoke(stWinMsg);
		return S_OK;
	}

	DGLE_RESULT DGLE_API KillEngine()
	{
		_bDoExit = true;
		_MainLoop();
		_MessageProc(TWinMessage(WMT_DESTROY));
		_MessageProc(TWinMessage(WMT_RELEASED));
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetGUID(GUID &guid)
	{
		guid = IID_IEngineCore;
		return S_OK;
	}

	DGLE_RESULT DGLE_API QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		*ppvObject = NULL;
		
		if(::memcmp(&riid,&__uuidof(IUnknown),sizeof(GUID)) == 0) 
			*ppvObject = static_cast<IUnknown *>(this);
		else 
			if(::memcmp(&riid,&IID_IEngineCore,sizeof(GUID)) == 0) 
				*ppvObject = static_cast<IEngineCore *>(this);
			else
				if(::memcmp(&riid,&IID_IEngineCoreWrapper,sizeof(GUID)) == 0) 
					*ppvObject = static_cast<IEngineCoreWrapper *>(this);
				else
					if(::memcmp(&riid,&IID_IMainWindow,sizeof(GUID)) == 0) 
						*ppvObject = static_cast<IMainWindow *>(_pMainWindow);
					else
						return E_NOINTERFACE;
		
		return S_OK;
	}
	
	ULONG DGLE_API AddRef(){return 1;}
	ULONG DGLE_API Release(){return 1;}

#endif//DGLE_USE_COM

};