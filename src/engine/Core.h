/**
\author		Korotkov Andrey aka DRON
\date		11.04.2016 (c)Korotkov Andrey

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

class CMainFileSystem;
class CResourceManager;
class CRender;

void InitDbgHelp(uint InstIdx);

struct TPlugin
{
	TDynLibHandle tLib;
	IPlugin *pPlugin;
};

// CCore //

class CCore final : public CInstancedObj, public IEngineCore
#ifdef DGLE_USE_COM
, public IEngineCoreWrapper
#endif
{
	static constexpr uint _sc_MaxUpdateCycles = 10;
	static constexpr uint _sc_AppCaptionMaxLength = 128;

	class CConnectionsTracker
	{
		// C++17 std::any could be use as well but unrestricted union provides better memory fit because size of all connections is known and is the same
		class GenericConnection
		{
			union
			{
				Signals::ScopedConnection<IBaseEvent *>	eventConnection;
				Signals::ScopedConnection<>				procConnection;
			};
			enum class ConnectionType : uint_least8_t
			{
				Event,
				Proc,
			} _type;

		public:
			GenericConnection(Signals::ScopedConnection<IBaseEvent *>	&&eventConnection) noexcept;
			GenericConnection(Signals::ScopedConnection<>				&&procConnection) noexcept;
			GenericConnection(GenericConnection &&connection) noexcept;
			GenericConnection &operator =(GenericConnection &&connection) noexcept;
			~GenericConnection() noexcept;
		};
		std::multimap<std::pair<void *, void *>, GenericConnection> _connections;

	public:
		typedef const decltype(_connections)::key_type &Slot;

	public:
		CConnectionsTracker() = default;
		CConnectionsTracker(CConnectionsTracker &&) = default;
		CConnectionsTracker &operator =(CConnectionsTracker &&) = default;

	public:
		void Add(Slot slot, Signals::ScopedConnection<IBaseEvent *> &&eventConnection), Add(Slot slot, Signals::ScopedConnection<> &&procConnection);
		void Remove(Slot slot);

	private:
		void Add(Slot slot, GenericConnection &&connection);
	};

	std::pair<CConnectionsTracker, TProcDelegate> _clDelUpdate, _clDelRender, _clDelInit, _clDelFree, *_SelectProcDelegate(E_ENGINE_PROCEDURE_TYPE eProcType) noexcept,
		_clDelMLoop, _clDelOnFPSTimer;
	TMsgProcDelegate _clDelMProc;

	std::vector<IEngineCallback *> _vecEngineCallbacks;

	std::unique_ptr<std::pair<CConnectionsTracker, TEventProcDelegate>> _events[ET_COUNT];

	std::fstream _clLogFile;
	uint _uiLogWarningsCount, _uiLogErrorsCount;
	IMainWindow *_pMainWindow;
	
	char *_pcCustomSplash;
	ISplashWindow *_pSplashWindow;

	bool _bCmdKeyIsPressed, _bFScreenKeyIsPressed;

	/** \remark: Setting _bDoExit to true is the the ONLY correct way to quit engine. */
	bool _bDoExit;

	TEngineWindow _stWin;

	std::chrono::milliseconds _updateInterval, _lastUpdateDeltaTime;
	std::chrono::microseconds _timeOld, _startTime, _pauseTime, _pauseStartTime, _renderDelay, _updateDelay;

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

	CMainFileSystem *_pMainFS;
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
	void _LogWinMode(const TEngineWindow &stWin);

	static bool DGLE_API _s_ConAutoPause(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConPrintVersion(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConFeatures(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConListPlugs(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConChangeMode(void *pParameter, const char *pcParam);	
	static bool DGLE_API _s_InstIdx(void *pParameter, const char *pcParam);

public:

	CCore(uint uiInstIdx);
	~CCore();

	inline	CRender *pRender() const {return _pRender;}
	inline	CMainFileSystem *pMainFS() const {return _pMainFS;}
	inline	CResourceManager *pResMan() const {return _pResMan;}

	inline  ICoreRenderer *pCoreRenderer() const {return _pCoreRenderer;}
	inline  IMainWindow *pMainWindow() const {return _pMainWindow;}
	inline	ISound *pSound() const {return _pSound;}
	inline	IInput *pInput() const {return _pInput;}

	inline	E_ENGINE_INIT_FLAGS InitFlags() const {return _eInitFlags;}
	inline	TMsgProcDelegate *pDMessageProc() {return &_clDelMProc;}
	inline	TProcDelegate *pDMLoopProc() {return &_clDelMLoop.second;}
	inline	TProcDelegate *pDFPSTimerProc() {return &_clDelOnFPSTimer.second;}
	inline	TEngineWindow *EngWindow() {return &_stWin;}
	
	inline	bool SoundEnabled() const {return _bSndEnabled;}
	void	ToogleSuspendEngine(bool bSuspend);

	DGLE_RESULT DGLE_API LoadSplashPicture(const char *pcBmpFileName) override;
	DGLE_RESULT DGLE_API AddPluginToInitializationList(const char *pcFileName) override;

	DGLE_RESULT DGLE_API InitializeEngine(TWindowHandle tHandle, const char *pcApplicationName, const TEngineWindow &stWindowParam, uint uiUpdateInterval, E_ENGINE_INIT_FLAGS eInitFlags) override;
	DGLE_RESULT DGLE_API SetUpdateInterval(uint uiUpdateInterval) override;
	DGLE_RESULT DGLE_API StartEngine() override;
	DGLE_RESULT DGLE_API QuitEngine() override;

	DGLE_RESULT DGLE_API ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin) override;
	DGLE_RESULT DGLE_API DisconnectPlugin(IPlugin *pPlugin) override;
	DGLE_RESULT DGLE_API GetPlugin(const char *pcPluginName, IPlugin *&prPlugin) override;

	DGLE_RESULT DGLE_API AddEngineCallback(IEngineCallback *pEngineCallback) override;
	DGLE_RESULT DGLE_API RemoveEngineCallback(IEngineCallback *pEngineCallback) override;

	DGLE_RESULT DGLE_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter) override;
	DGLE_RESULT DGLE_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter) override;

	DGLE_RESULT DGLE_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent) override;
	DGLE_RESULT DGLE_API AddEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter) override;
	DGLE_RESULT DGLE_API RemoveEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListnerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter) override;

	DGLE_RESULT DGLE_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem) override;

	DGLE_RESULT DGLE_API RenderFrame() override;
	DGLE_RESULT DGLE_API RenderProfilerText(const char *pcTxt, const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetInstanceIndex(uint &uiIdx) override;
	DGLE_RESULT DGLE_API GetTimer(uint64 &ui64Tick) override;
	DGLE_RESULT DGLE_API GetSystemInfo(TSystemInfo &stSysInfo) override;
	DGLE_RESULT DGLE_API GetCurrentWindow(TEngineWindow &stWin) override;
	DGLE_RESULT DGLE_API GetFPS(uint &uiFPS) override;
	DGLE_RESULT DGLE_API GetLastUpdateDeltaTime(uint &uiDeltaTime) override;
	DGLE_RESULT DGLE_API GetElapsedTime(uint64 &ui64ElapsedTime) override;
	DGLE_RESULT DGLE_API GetWindowHandle(TWindowHandle &tHandle) override;

	DGLE_RESULT DGLE_API ChangeWindowMode(const TEngineWindow &stNewWin) override;
	DGLE_RESULT DGLE_API GetDesktopResolution(uint &uiWidth, uint &uiHeight) override;
	DGLE_RESULT DGLE_API AllowPause(bool bAllow) override;

	DGLE_RESULT DGLE_API WriteToLog(const char *pcTxt) override;
	DGLE_RESULT DGLE_API WriteToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber) override;

	DGLE_RESULT DGLE_API ConsoleVisible(bool bIsVisible) override;
	DGLE_RESULT DGLE_API ConsoleWrite(const char *pcTxt, bool bWriteToPreviousLine) override;
	DGLE_RESULT DGLE_API ConsoleExecute(const char *pcCommandTxt) override;
	DGLE_RESULT DGLE_API ConsoleRegisterCommand(const char *pcCommandName, const char *pcCommandHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter) override;
	DGLE_RESULT DGLE_API ConsoleRegisterVariable(const char *pcCommandName, const char *pcCommandHelp, int *piVar, int iMinValue, int iMaxValue, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter) override;
	DGLE_RESULT DGLE_API ConsoleUnregister(const char *pcCommandName) override;

	DGLE_RESULT DGLE_API GetVersion(char *pcBuffer, uint &uiBufferSize) override;

#ifndef DGLE_USE_COM

	IDGLE_BASE_IMPLEMENTATION(IEngineCore, INTERFACE_IMPL_END)

#else

	DGLE_RESULT DGLE_API SetWindowAPI(IMainWindow *pMainWindow) override
	{
		_pMainWindow = pMainWindow;
		return S_OK;
	}

	DGLE_RESULT DGLE_API TranslateMessage(const TWindowMessage &stWinMsg) override
	{
		_clDelMProc(stWinMsg);
		return S_OK;
	}

	DGLE_RESULT DGLE_API KillEngine() override
	{
		_bDoExit = true;
		_MainLoop();
		_MessageProc(TWindowMessage(WMT_DESTROY));
		_MessageProc(TWindowMessage(WMT_RELEASED));
		return S_OK;
	}

	IDGLE_BASE_DUMMY_COMMANDS_IMPL

	IDGLE_BASE_GUID_IMPL(IEngineCore)

	HRESULT CALLBACK QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) override
	{
		*ppvObject = NULL;

		if (::memcmp(&riid, &__uuidof(IUnknown), sizeof(GUID)) == 0) 
			*ppvObject = static_cast<IUnknown *>(this);
		else 
			if (::memcmp(&riid,&IID_IDGLE_Base, sizeof(GUID)) == 0) 
				*ppvObject = static_cast<IDGLE_Base *>(this);
			else
				if (::memcmp(&riid, &IID_IEngineCore, sizeof(GUID)) == 0) 
					*ppvObject = static_cast<IEngineCore *>(this);
				else
					if (::memcmp(&riid, &IID_IEngineCoreWrapper, sizeof(GUID)) == 0) 
						*ppvObject = static_cast<IEngineCoreWrapper *>(this);
					else
						if (::memcmp(&riid, &IID_IMainWindow, sizeof(GUID)) == 0) 
							*ppvObject = static_cast<IMainWindow *>(_pMainWindow);
						else
							return E_NOINTERFACE;
		
		return S_OK;
	}
	
	ULONG CALLBACK AddRef() override
	{
		return 1;
	}
	
	ULONG CALLBACK Release() override
	{
		return 1;
	}

#endif//DGLE_USE_COM

};