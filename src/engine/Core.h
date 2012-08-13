/**
\author		Korotkov Andrey aka DRON
\date		26.04.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

const uint c_MaxProcessCycles		= 10;
const uint c_AppCaptionMaxLength	= 128;
const uint c_MaxPluginInterfaceName = 128;

class CInput;
class CDirectInput;
class CMainFS;
class CResourceManager;
class CRender;
class CSoundDX;
class CSoundMCI;

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
	TProcDelegate		 _clDelUpdate,
						 _clDelRender,
						 _clDelInit,
						 _clDelFree,

						 //For system use
						 _clDelMLoop,
						 _clDelOnFPSTimer;
	TMsgProcDelegate	 _clDelMProc;

	std::vector<TEvent>	 _clEvents;

	std::fstream		 _clLogFile;
	int					 _iLogWarningsCount;
	int					 _LogErrorsCount;
	IMainWindow		    *_pMainWindow;
	
	char				*_pcCustomSplash;
	ISplashWindow		*_pSplashWindow;

	/** \remark: Setting _bDoExit to true is the the ONLY correct way to quit engine. */
	bool				 _bDoExit;

	TEngWindow			 _stWin;

	uint				 _uiProcessInterval;
	uint64				 _ui64TimeOld;
	bool				 _bPause;
	char				 _pcApplicationCaption[c_AppCaptionMaxLength];

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
						 _ui64ProcessDelay;

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
	
	void				_LogWriteEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
	void				_LogWrite(const char *pcTxt, bool bFlush = false);
	
	int					_iAllowPause,
						_iFPSToCaption,
						_iAllowDrawProfilers,
						_iDrawProfiler;

	TEngWindow			_stWndToApply;
	bool				_bNeedApplyNewWnd;

	static void CALLBACK _s_MainLoop(void *pParametr);
	static void CALLBACK _s_OnTimer(void *pParametr);
	static void CALLBACK _s_MessageProc(void *pParametr, const TWinMessage &stMsg);

	static void CALLBACK _s_ConAutoPause(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ConPrintVersion(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ConFeatures(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ConListPlugs(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ConChangeMode(void *pParametr, const char *pcParam);	
	static void CALLBACK _s_InstIdx(void *pParametr, const char *pcParam);

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

	HRESULT CALLBACK LoadSplashPicture(const char *pcBmpFileName);
	HRESULT CALLBACK AddPluginToInitList(const char *pcFileName);

	HRESULT CALLBACK InitializeEngine(TWinHandle tHandle, const char* pcApplicationName, const TEngWindow &stWindowParam, uint uiProcessInterval, E_ENGINE_INIT_FLAGS eInitFlags);
	HRESULT CALLBACK SetProcessInterval(uint uiProcessInterval);
	HRESULT CALLBACK StartEngine();
	HRESULT CALLBACK QuitEngine();

	HRESULT CALLBACK ConnectPlugin(const char* pcFileName, IPlugin *&prPlugin);
	HRESULT CALLBACK DisconnectPlugin(IPlugin *pPlugin);
	HRESULT CALLBACK GetPlugin(const char* pcPluginName, IPlugin *&prPlugin);

	HRESULT CALLBACK AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (CALLBACK *pProc)(void *pParametr), void *pParametr);
	HRESULT CALLBACK RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (CALLBACK *pProc)(void *pParametr), void *pParametr);

	HRESULT CALLBACK CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent);
	HRESULT CALLBACK AddEventListner(E_EVENT_TYPE eEventType, void (CALLBACK *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);
	HRESULT CALLBACK RemoveEventListner(E_EVENT_TYPE eEventType, void (CALLBACK *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr);

	HRESULT CALLBACK GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem);

	HRESULT CALLBACK RenderProfilerTxt(const char* pcTxt, const TColor4 &stColor);
	HRESULT CALLBACK GetInstanceIdx(uint &uiIdx);
	HRESULT CALLBACK GetTimer(uint64 &uiTick);
	HRESULT CALLBACK GetSystemInfo(TSystemInfo &stSysInfo);
	HRESULT CALLBACK GetCurrentWin(TEngWindow &stWin);
	HRESULT CALLBACK GetFPS(uint &uiFPS);
	HRESULT CALLBACK GetHandle(TWinHandle &tHandle);

	HRESULT CALLBACK ChangeWinMode(const TEngWindow &stNewWin);
	HRESULT CALLBACK AllowPause(bool bAllow);

	HRESULT CALLBACK AddToLog(const char *pcTxt);
	HRESULT CALLBACK AddToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

	HRESULT CALLBACK ConsoleVisible(bool bIsVisible);
	HRESULT CALLBACK ConsoleWrite(const char* pcTxt, bool bWriteToPreviousLine);
	HRESULT CALLBACK ConsoleExec(const char* pcCommandTxt);
	HRESULT CALLBACK ConsoleRegComProc(const char *pcCommandName, const char *pcCommandHelp, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr); 
	HRESULT CALLBACK ConsoleRegComValue(const char *pcCommandName, const char *pcCommandHelp, int *piValue, int iMinValue, int iMaxValue, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr);
	HRESULT CALLBACK ConsoleUnregCom(const char* pcCommandName);

	HRESULT CALLBACK GetVersion(char* pcBuffer, uint uiBufferSize);

#ifndef DGLE2_USE_COM

	IDGLE2_BASE_IMPLEMENTATION(IEngineCore)

#else

	HRESULT CALLBACK SetWindowAPI(IMainWindow *pMainWindow)
	{
		_pMainWindow = pMainWindow;
		return S_OK;
	}

	HRESULT CALLBACK TranslateMessage(const TWinMessage &stWinMsg)
	{
		_clDelMProc.Invoke(stWinMsg);
		return S_OK;
	}

	HRESULT CALLBACK KillEngine()
	{
		_bDoExit = true;
		_MainLoop();
		_MessageProc(TWinMessage(WMT_DESTROY));
		_MessageProc(TWinMessage(WMT_RELEASED));
		return S_OK;
	}

	HRESULT CALLBACK GetGUID(GUID &guid)
	{
		guid = IID_IEngineCore;
		return S_OK;
	}

	HRESULT CALLBACK QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
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
	
	ULONG CALLBACK AddRef(){return 1;}
	ULONG CALLBACK Release(){return 1;}

#endif//DGLE2_USE_COM

};