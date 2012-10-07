/**
\author		Korotkov Andrey aka DRON
\date		17.09.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
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
#ifdef DGLE2_USE_COM
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

	static void DGLE2_API _s_MainLoop(void *pParametr);
	static void DGLE2_API _s_OnTimer(void *pParametr);
	static void DGLE2_API _s_MessageProc(void *pParametr, const TWinMessage &stMsg);

	static void DGLE2_API _s_ConAutoPause(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_ConPrintVersion(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_ConFeatures(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_ConListPlugs(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_ConChangeMode(void *pParametr, const char *pcParam);	
	static void DGLE2_API _s_InstIdx(void *pParametr, const char *pcParam);

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

	HRESULT DGLE2_API LoadSplashPicture(const char *pcBmpFileName);
	HRESULT DGLE2_API AddPluginToInitList(const char *pcFileName);

	HRESULT DGLE2_API InitializeEngine(TWinHandle tHandle, const char* pcApplicationName, const TEngWindow &stWindowParam, uint uiProcessInterval, E_ENGINE_INIT_FLAGS eInitFlags);
	HRESULT DGLE2_API SetProcessInterval(uint uiProcessInterval);
	HRESULT DGLE2_API StartEngine();
	HRESULT DGLE2_API QuitEngine();

	HRESULT DGLE2_API ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin);
	HRESULT DGLE2_API DisconnectPlugin(IPlugin *pPlugin);
	HRESULT DGLE2_API GetPlugin(const char *pcPluginName, IPlugin *&prPlugin);

	HRESULT DGLE2_API AddUserCallback(IUserCallback *pUserCallback);
	HRESULT DGLE2_API RemoveUserCallback(IUserCallback *pUserCallback);

	HRESULT DGLE2_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE2_API *pProc)(void *pParametr), void *pParametr);
	HRESULT DGLE2_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE2_API *pProc)(void *pParametr), void *pParametr);

	HRESULT DGLE2_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent);
	HRESULT DGLE2_API AddEventListner(E_EVENT_TYPE eEventType, void (DGLE2_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);
	HRESULT DGLE2_API RemoveEventListner(E_EVENT_TYPE eEventType, void (DGLE2_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);

	HRESULT DGLE2_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem);

	HRESULT DGLE2_API RenderProfilerTxt(const char *pcTxt, const TColor4 &stColor);
	HRESULT DGLE2_API GetInstanceIdx(uint &uiIdx);
	HRESULT DGLE2_API GetTimer(uint64 &uiTick);
	HRESULT DGLE2_API GetSystemInfo(TSystemInfo &stSysInfo);
	HRESULT DGLE2_API GetCurrentWin(TEngWindow &stWin);
	HRESULT DGLE2_API GetFPS(uint &uiFPS);
	HRESULT DGLE2_API GetLastUpdateDeltaTime(uint64 &ui64DeltaTime);
	HRESULT DGLE2_API GetHandle(TWinHandle &tHandle);

	HRESULT DGLE2_API ChangeWinMode(const TEngWindow &stNewWin);
	HRESULT DGLE2_API AllowPause(bool bAllow);

	HRESULT DGLE2_API AddToLog(const char *pcTxt);
	HRESULT DGLE2_API AddToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

	HRESULT DGLE2_API ConsoleVisible(bool bIsVisible);
	HRESULT DGLE2_API ConsoleWrite(const char* pcTxt, bool bWriteToPreviousLine);
	HRESULT DGLE2_API ConsoleExec(const char* pcCommandTxt);
	HRESULT DGLE2_API ConsoleRegComProc(const char *pcCommandName, const char *pcCommandHelp, void (DGLE2_API *pProc)(void *pParametr, const char *pcParam), void *pParametr); 
	HRESULT DGLE2_API ConsoleRegComValue(const char *pcCommandName, const char *pcCommandHelp, int *piValue, int iMinValue, int iMaxValue, void (DGLE2_API *pProc)(void *pParametr, const char *pcParam), void *pParametr);
	HRESULT DGLE2_API ConsoleUnregCom(const char* pcCommandName);

	HRESULT DGLE2_API GetVersion(char* pcBuffer, uint uiBufferSize);

#ifndef DGLE2_USE_COM

	IDGLE2_BASE_IMPLEMENTATION(IEngineCore)

#else

	HRESULT DGLE2_API SetWindowAPI(IMainWindow *pMainWindow)
	{
		_pMainWindow = pMainWindow;
		return S_OK;
	}

	HRESULT DGLE2_API TranslateMessage(const TWinMessage &stWinMsg)
	{
		_clDelMProc.Invoke(stWinMsg);
		return S_OK;
	}

	HRESULT DGLE2_API KillEngine()
	{
		_bDoExit = true;
		_MainLoop();
		_MessageProc(TWinMessage(WMT_DESTROY));
		_MessageProc(TWinMessage(WMT_RELEASED));
		return S_OK;
	}

	HRESULT DGLE2_API GetGUID(GUID &guid)
	{
		guid = IID_IEngineCore;
		return S_OK;
	}

	HRESULT DGLE2_API QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
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
	
	ULONG DGLE2_API AddRef(){return 1;}
	ULONG DGLE2_API Release(){return 1;}

#endif//DGLE2_USE_COM

};