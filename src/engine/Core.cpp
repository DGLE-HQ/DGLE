/**
\author		Korotkov Andrey aka DRON
\date		26.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Core.h"
#include "MainWindow.h"
#include "HookedWindow.h"
#include "SplashWindow.h"
#include "MainFS.h"
#include "CoreRendererGL.h"
#include "Render.h"
#include "ResourceManager.h"
#include "Render2D.h"

using namespace std;

extern bool bUnhandledFilterEnabled;

//Event Classes//

class CBaseEvent : public IBaseEvent
{
	E_EVENT_TYPE _eEvType;

public:
	
	CBaseEvent(E_EVENT_TYPE eEvType):_eEvType(eEvType){}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = _eEvType;
		return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE2_BASE_IMPLEMENTATION(IBaseEvent)
};

class CEvBeforeInit : public IEvBeforeInit
{
	 TEngWindow			 *_pstWindowParam;
	 E_ENGINE_INIT_FLAGS *_peInitFlags; 

public:

	CEvBeforeInit(TEngWindow *pstWindowParam, E_ENGINE_INIT_FLAGS *peInitFlags)
	{
		_pstWindowParam = pstWindowParam;
		_peInitFlags	= peInitFlags;
	}

	HRESULT CALLBACK SetEngParams(const TEngWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags)
	{
		(*_pstWindowParam)= stWindowParam;
		(*_peInitFlags)	= eInitFlags;
		return S_OK;
	}
	
	HRESULT CALLBACK GetEngParams(TEngWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags)
	{
		stWindowParam = (*_pstWindowParam);
		eInitFlags	  = (*_peInitFlags); 	
		return S_OK;
	}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_BEFORE_INIT;
		return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE2_BASE_IMPLEMENTATION(IEvBeforeInit)
};

class CEvWinMessage : public IEvWinMessage
{
	TWinMessage _stMessage;

public:

	CEvWinMessage(const TWinMessage &stMessage):_stMessage(stMessage){}

	HRESULT CALLBACK GetWinMessage(TWinMessage &stWinMsg)
	{
		stWinMsg = _stMessage;
		return S_OK;
	}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_WIN_MESSAGE;
		return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE2_BASE_IMPLEMENTATION(IEvWinMessage)
};

class CEvGetSubSystem : public IEvGetSubSystem
{

	E_ENGINE_SUB_SYSTEM _eSSType;
	IEngineSubSystem	**_ppEngSS;

public:

	CEvGetSubSystem(E_ENGINE_SUB_SYSTEM eSSType, IEngineSubSystem	*pEngSS)
	{
		_eSSType = eSSType;
		_ppEngSS = &pEngSS;
	}

	HRESULT CALLBACK GetSubSystemType(E_ENGINE_SUB_SYSTEM eSubSystem)
	{
		eSubSystem = _eSSType;
		return S_OK;
	}

	HRESULT CALLBACK OverrideSubSystem(IEngineSubSystem *prSubSystem)
	{
		_ppEngSS = &prSubSystem;
		return S_OK;
	}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_GET_SSYSTEM;
		return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE2_BASE_IMPLEMENTATION(IEvGetSubSystem)
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

	HRESULT CALLBACK GetMessageTxt(char *pcTxt, uint uiCharsCount)
	{
		if(uiCharsCount < _txt.length())
			return E_INVALIDARG;

		strcpy(pcTxt, _txt.c_str());

		return S_OK;
	}
	
	HRESULT CALLBACK FreezeEngine(bool bFreeze)
	{
		_pCore->ToogleSuspendEngine(bFreeze);
		return S_OK;
	}

	HRESULT CALLBACK ForceNoMessage()
	{
		_bNoMessage = true;
		return S_OK;
	}
		
	HRESULT CALLBACK ForceIgnoreError()
	{
		_bNoHalt = true;
		return S_OK;
	}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_ENGINE_FATAL_MESSAGE;
		return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE2_BASE_IMPLEMENTATION(IEvFatalMessage)
};

//Engine Core//

CCore::CCore(uint uiInstIdx, bool bForceSingleThreaded):
CInstancedObj(uiInstIdx),
_pMainWindow(NULL), _uiFPSTimer(0),
_iLogWarningsCount(0), _LogErrorsCount(0),
_pRender(NULL), _pMainFS(NULL), _pResMan(NULL), _pInput(NULL), _pSound(NULL), _pCoreRenderer(NULL),
_bBuiltInSound(true), _bBuiltInRenderer(true), _bBuiltInInput(true),
_pSplashWindow(NULL),
_bDoExit(false), _bInDrawProfilers(false),
_bStartedFlag(false), _bInitedFlag(false), _bQuitFlag(false),
_iAllowPause(1), _iFPSToCaption(0), _iAllowDrawProfilers(1), _iDrawProfiler(0),
_bNeedApplyNewWnd(false),
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
		_clLogFile.open((string("log")+(InstIdx() != 0 ? IntToStr(InstIdx()) : string(""))+".txt").c_str(), ios::out|ios::trunc);

		TSysTimeAndDate time;
		GetLocalTimaAndDate(time);

		_clLogFile << "DGLE2 Engine Log File" << endl;
		_clLogFile << "Version: "<< DGLE2_VERSION << endl;
		_clLogFile << "Visit http://dgle.dronprogs.org/ for more information." << endl;
		_clLogFile << "Log Started at " << time.ui16Day << "." << time.ui16Month << "." << time.ui16Year << "." << endl;
	}
}

CCore::~CCore()
{
	if (!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_NO_LOG_FILE))
	{
		if (!_bDoExit)
			_LogWrite("***Abnormal Engine Termination***");

		_LogWrite("Engine Finalized.");
		_clLogFile << "----" << endl;
		_clLogFile << "Warning(s): " << _iLogWarningsCount << " Error(s): " << _LogErrorsCount << endl;
		
		if (_ui64CiclesCount == 0) _ui64CiclesCount = 1;
		_clLogFile << "Average FPS: " << (uint64)(_ui64FPSSumm/_ui64CiclesCount) << endl;		

		_clLogFile << "Log Closed.";
		_clLogFile.close();
	}

	delete[] _pcCustomSplash;
}

void CCore::_LogWriteEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	string msg = "";
	
	switch (eType)
	{
		 case LT_WARNING:	msg="[WARNING] " + string(pcTxt); ++_iLogWarningsCount; break;
		 case LT_ERROR:		msg="[ERROR] " + string(pcTxt) + " (File: \"" + string(pcSrcFileName) + "\", Line: "+IntToStr(iSrcLineNumber)+")"; ++_LogErrorsCount; break;
		 case LT_FATAL:		msg="[FATAL] " + string(pcTxt) + " (File: \"" + string(pcSrcFileName) + "\", Line: "+IntToStr(iSrcLineNumber)+")"; ++_LogErrorsCount;
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

								ShowModalUserAlert(pcTxt, "DGLE2 Fatal Error!");
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

		 default: msg = pcTxt; break;
	}
	
	if (eType != LT_FATAL)
		_LogWrite(msg.c_str(), eType > 1/*immediately flushs only errors and fatals*/);

#ifdef _DEBUG
	assert(eType == LT_INFO);
#endif
}

void CCore::_LogWrite(const char *pcTxt, bool bFlush)
{
	if (_clLogFile.is_open())
	{
		TSysTimeAndDate time;
		GetLocalTimaAndDate(time);
		_clLogFile.fill('0');
		_clLogFile.width(2);
		_clLogFile << time.ui16Hour << ":";
		_clLogFile.width(2);
		_clLogFile << time.ui16Minute << ":";
		_clLogFile.width(2);
		_clLogFile << time.ui16Second << ".";	
		_clLogFile.width(3);
		_clLogFile << time.ui16Milliseconds;
		_clLogFile.width(0);
		_clLogFile << " - ";

		_clLogFile << pcTxt << endl;

		if (bFlush)
			_clLogFile.flush();
	}

#ifdef _DEBUG
	OutputDebugTxt(pcTxt);
#endif

	Console()->Write(pcTxt);
}

void CCore::_MessageProc(const TWinMessage &stMsg)
{
	switch (stMsg.uiMsgType)
	{
	case WMT_REDRAW:
		_clDelMLoop.Invoke();
		break;

	case WMT_CLOSE:
		_bDoExit = true;
		break;

	case WMT_DESTROY:

		LOG("Finalizing Engine...", LT_INFO);

		if (!_clDelFree.IsNull()) 
		{
			LOG("Calling user finalization procedure...", LT_INFO);
			_clDelFree.Invoke();
			LOG("Done.", LT_INFO);
		}

		size_t i;
		i = 0;
		while (i < _clPlugins.size())
		{
			char iname[c_MaxPluginInterfaceName];
			PARANOIC_CHECK_HR(_clPlugins[i].pPlugin->GetPluginInterfaceName(iname, c_MaxPluginInterfaceName));
					
			if (strcmp(iname, "ISubSystemPlugin") != 0)
				_UnloadPlugin(_clPlugins[i].pPlugin);
			else
				++i;
		}

		_pResMan->FreeAllResources();
		_pMainFS->UnregisterAndFreeAll();

		if (!ReleaseTimer(_uiFPSTimer))
			LOG("Can't release FPS timer.", LT_ERROR);
		break;

	case WMT_RELEASED:
		
		delete _pResMan;

		delete _pRender;

		_pCoreRenderer->Finalize();

		while (!_clPlugins.empty())
			_UnloadPlugin(_clPlugins.begin()->pPlugin);

		//
//		if(_eInitFlags & EIF_FORCE_API_INPUT)
//			delete (CInput*)_pInput;
//#ifndef NO_DIRECTX
//		else
//			delete (CDirectInput*)_pInput;
//#endif
//
//		if(_pSound)
//		{
//			if(_eInitFlags & EIF_FORCE_API_SOUND)
//				delete (CSoundMCI*)_pSound;
//#ifndef NO_DIRECTX
//			else
//				delete (CSoundDX*)_pSound;
//#endif
//		}
//

#ifndef NO_BUILTIN_RENDERER
		if (_bBuiltInRenderer)
			delete (CCoreRendererGL*)_pCoreRenderer;
#endif

		delete _pMainFS;

		_pMainWindow->Free();

		for (size_t i = 0; i < _clEvents.size(); ++i)
			delete _clEvents[i].pDEvent;

		_clEvents.clear();
		break;

	case WMT_ACTIVATED:
		_bPause = false;	
		_pMainWindow->SetCaption(_pcApplicationCaption);
		break;

	case WMT_DEACTIVATED:
		_bPause = true;
		if (_iAllowPause)
			_pMainWindow->SetCaption((string(_pcApplicationCaption)+string(" [Paused]")).c_str());
		break;

	case WMT_RESTORED:
	case WMT_SIZE:
		_stWin.uiWidth = stMsg.ui32Param1;
		_stWin.uiHeight= stMsg.ui32Param2;
		_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);
		break;

	case WMT_KEY_UP:
		if (stMsg.ui32Param1 == KEY_GRAVE && !(_stWin.uiFlags & EWF_RESTRICT_CONSOLE_HOTKEY))
			Console()->Visible(true);
		break;
	}

	CastEvent(ET_ON_WIN_MESSAGE, (IBaseEvent*)&CEvWinMessage(stMsg));
}

void CCore::_OnTimer()
{
	if (_bNeedApplyNewWnd)
	{
		ChangeWinMode(_stWndToApply);
		_bNeedApplyNewWnd = false;
	}

	_uiLastFPS  = _uiFPSCount;
	_uiLastUPS  = _uiUPSCount;

	if (!_bPause)
	{
		_ui64FPSSumm  += _uiLastFPS;
		++_ui64CiclesCount;

		if (_iFPSToCaption == 1)
			_pMainWindow->SetCaption((string(_pcApplicationCaption) + string(" FPS:") + IntToStr(_uiLastFPS)).c_str());
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

	uint64 time			= GetPerfTimer()/1000;
	uint64 time_delta	= time - _ui64TimeOld;

	bool flag = false;

	if (Console()->EnterCrSection())
	{
		uint cycles_cnt = (uint)(time_delta/_uiProcessInterval);
		
		if ((_eInitFlags & EIF_DISABLE_SMART_TIMING) && cycles_cnt > 1)
			cycles_cnt = 1;
		else
			if (cycles_cnt > c_MaxProcessCycles)
				cycles_cnt = c_MaxProcessCycles;

		if (cycles_cnt > 0)
			_ui64ProcessDelay = GetPerfTimer();

		for (uint i = 0; i < cycles_cnt; ++i)
		{
			if (((!_bPause && _iAllowPause) ||
				!_iAllowPause) && !_clDelUpdate.IsNull() && !_bQuitFlag) 
			{
				if (_pRender && i == 0)
					_pRender->pRender2D()->RefreshBatchData();
				
				_clDelUpdate.Invoke();

				++_uiUPSCount;
			}

			flag = true;
		}

		if (flag)
		{
			_ui64ProcessDelay = GetPerfTimer() - _ui64ProcessDelay;
			_ui64TimeOld = GetPerfTimer()/1000 - time_delta % _uiProcessInterval;
		}

		_pRender->BeginRender();

		//	_pRender->pRender3D()->Prepare();

		_ui64RenderDelay = GetPerfTimer();

		CastEvent(ET_BEFORE_RENDER, (IBaseEvent*)&CBaseEvent(ET_BEFORE_RENDER));

		_clDelRender.Invoke();

		CastEvent(ET_AFTER_RENDER, (IBaseEvent*)&CBaseEvent(ET_AFTER_RENDER));

		_ui64RenderDelay = GetPerfTimer() - _ui64RenderDelay;

		//begin of profilers//

		if (_iAllowDrawProfilers == 1)
		{
			_pRender->pRender2D()->BeginProfiler2D();

			_bInDrawProfilers = true;

			_uiProfilerCurTxtXOffset	= 0;
			_uiProfilerCurTxtYOffset	= 0;
			_uiProfilerCurTxtMaxLength	= 0;
				
			if (_iDrawProfiler > 0)
			{
				TColor4 col;

				if (_uiLastFPS < 60)
					col.b = col.g = 0.f;
					
				RenderProfilerTxt(("FPS:" + IntToStr(_uiLastFPS)).c_str(), col);

				col = TColor4();

				if (_uiLastUPS < 1000/_uiProcessInterval)
					col.b = col.g = 0.f;
					
				RenderProfilerTxt(("UPS:" + IntToStr(_uiLastUPS)).c_str(), col);

				if (_iDrawProfiler > 1)
				{
					RenderProfilerTxt(("Render  delay:" + UInt64ToStr(_ui64RenderDelay/1000) + "." + UIntToStr(_ui64RenderDelay%1000) + " ms.").c_str(), TColor4());
					RenderProfilerTxt(("Process delay:" + UInt64ToStr(_ui64ProcessDelay/1000) + "." + UIntToStr(_ui64RenderDelay%1000) + " ms.").c_str(), TColor4());
				}
			}
				
			_pRender->pRender2D()->DrawProfiler();

			CastEvent(ET_ON_PROFILER_DRAW, (IBaseEvent*)&CBaseEvent(ET_ON_PROFILER_DRAW));

			_bInDrawProfilers = false;

			_pRender->pRender2D()->EndProfiler2D();
		}

		//end of profilers//

		_pRender->EndRender();

		++_uiFPSCount;

		Console()->LeaveCrSection();
	}

	uint sleep = (int)((_eInitFlags & EIF_FORCE_LIMIT_FPS) && (_uiLastFPS > _uiProcessInterval || _bPause))*10 +
				 (int)(_bPause && _iAllowPause)*15 + (int)(_stSysInfo.uiCPUCount < 2 && _ui64CiclesCount < 4)*5;

	if (sleep > 0)
		Suspend(sleep);
}

HRESULT CALLBACK CCore::RenderProfilerTxt(const char* pcTxt, const TColor4 &stColor)
{
	if (!_bInDrawProfilers)
		return S_FALSE;

	uint tw, th;
	
	_pResMan->pISystemFont()->GetTextDimensions(pcTxt,tw,th);
	_pResMan->pISystemFont()->Draw2D((float)_uiProfilerCurTxtXOffset, (float)_uiProfilerCurTxtYOffset, pcTxt, stColor);
	
	_uiProfilerCurTxtYOffset	+= th;
	_uiProfilerCurTxtMaxLength  = max(_uiProfilerCurTxtMaxLength,tw);

	uint x,y, w, h;
	_pCoreRenderer->GetViewport(x, y, w, h);

	if (_uiProfilerCurTxtYOffset + th > h) 
	{
		_uiProfilerCurTxtXOffset	+= _uiProfilerCurTxtMaxLength;
		_uiProfilerCurTxtMaxLength	= 0;
		_uiProfilerCurTxtYOffset	= 0;
	}

	return S_OK;
}

HRESULT CALLBACK CCore::ConnectPlugin(const char* pcFileName, IPlugin *&prPlugin)
{
	if (_access(pcFileName, 0) != -1)
		return _LoadPlugin(string(pcFileName),prPlugin) ? S_OK : E_ABORT;
	else
	{
		prPlugin = NULL;
		LOG("Plugin file \""+string(pcFileName)+"\" doesn't exists.", LT_ERROR);
		return S_FALSE;
	}
}

HRESULT CALLBACK CCore::DisconnectPlugin(IPlugin *pPlugin)
{
	return _UnloadPlugin(pPlugin) ? S_OK : E_INVALIDARG;
}

HRESULT CALLBACK CCore::GetPlugin(const char* pcPluginName, IPlugin *&prPlugin)
{
	for (size_t i = 0; i < _clPlugins.size(); ++i)
	{
		TPluginInfo info;
		_clPlugins[i].pPlugin->GetPluginInfo(info);
		if (strcmp(pcPluginName,info.cName) == 0)
		{
			prPlugin = _clPlugins[i].pPlugin;
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

bool CCore::_UnloadPlugin(IPlugin *pPlugin)
{
	for (size_t i = 0; i < _clPlugins.size(); ++i)
		if (_clPlugins[i].pPlugin == pPlugin)
		{
			TPluginInfo info;
			_clPlugins[i].pPlugin->GetPluginInfo(info);

			void (CALLBACK *pFreePlugin)(IPlugin *plugin) = NULL;

			pFreePlugin = reinterpret_cast<void (CALLBACK *)(IPlugin *)>(GetProcAddress(_clPlugins[i].tLib,("FreePlugin")));

			if (pFreePlugin)
				(*pFreePlugin)(_clPlugins[i].pPlugin);
			
			if (!ReleaseDynamicLib(_clPlugins[i].tLib))
				LOG("Can't free \"" + string(info.cName) + "\" plugin library.", LT_ERROR);
			else
				LOG("Plugin \"" + string(info.cName) + "\" disconnected succesfully.", LT_INFO);
			
			_clPlugins.erase(_clPlugins.begin() + i);
			return true;
		}

	return false;
}

bool CCore::_LoadPlugin(const string &clFileName, IPlugin *&prPlugin)
{
	prPlugin = NULL;
	TPlugin tmp;
	tmp.tLib = NULL;
	tmp.pPlugin = NULL;
	
	tmp.tLib = LoadDynamicLib(clFileName.c_str());
	
	if (!tmp.tLib)
	{
		LOG("Can't load plugin from library \""+clFileName+"\".", LT_ERROR);
		return false;
	}

	void (CALLBACK *pInitPlugin)(IEngineCore *engineCore, IPlugin *&plugin) = NULL;
	pInitPlugin = reinterpret_cast<void (CALLBACK *)(IEngineCore *, IPlugin *&)>(GetFuncAddress(tmp.tLib, "InitPlugin"));

	if (!pInitPlugin)
	{
		LOG("Library \""+clFileName+"\" is not a valid DGLE2 plugin.", LT_ERROR);
		ReleaseDynamicLib(tmp.tLib);
		return false;
	}

	(*pInitPlugin)((IEngineCore*)this, tmp.pPlugin);

	TPluginInfo info;

	tmp.pPlugin->GetPluginInfo(info);

	if (info.btPluginSDKVersion != _DGLE2_PLUGIN_SDK_VER_)
	{
		LOG("Plugin \""+clFileName+"\" SDK version differs from engine version.", LT_ERROR);
		ReleaseDynamicLib(tmp.tLib);
		return false;
	}

	_clPlugins.push_back(tmp);

	prPlugin = tmp.pPlugin;

	LOG("Plugin \""+string(info.cName)+" " + string(info.cVersion) +"\" by \"" + string(info.cVendor) + "\" connected succesfully.",LT_INFO);
	LOG("Plugin description: \""+string(info.cDiscription)+"\"",LT_INFO);

	return true;
}

void CCore::_PrintPluginsInfo()
{
	string tmp = "------Connected Plugins------\r\n";
	for (size_t i = 0; i < _clPlugins.size(); ++i)
	{
		TPluginInfo info;
		_clPlugins[i].pPlugin->GetPluginInfo(info);
		tmp += "- " + string(info.cName)+" " + string(info.cVersion) +" by " + string(info.cVendor) + "\r\n";
	}
	tmp += "-----------------------------";
	Console()->Write(tmp.c_str());
}

HRESULT CALLBACK CCore::LoadSplashPicture(const char *pcBmpFileName)
{
	if (_bInitedFlag)
		return S_FALSE;

	delete[] _pcCustomSplash;
	_pcCustomSplash = new char [strlen(pcBmpFileName) + 1];
	strcpy(_pcCustomSplash, pcBmpFileName);

	return S_OK;
}

HRESULT CALLBACK CCore::AddPluginToInitList(const char *pcFileName)
{
	if (_bInitedFlag)
		return S_FALSE;

	_clPluginInitList.push_back(pcFileName);

	return S_OK;
}

HRESULT CALLBACK CCore::InitializeEngine(TWinHandle tHandle, const char* pcApplicationName, const TEngWindow &stWindowParam, uint uiProcessInterval, E_ENGINE_INIT_FLAGS eInitFlags)
{
	if (!_bInitedFlag)
	{
		_eInitFlags = eInitFlags;

		LOG("Initializing Engine...",LT_INFO);

		if (!PlatformInit())
		{
			LOG("Can't initialize platform API.", LT_FATAL);
			return E_ABORT;
		}

		_stWin = stWindowParam;
		_uiProcessInterval = uiProcessInterval;
		_bPause = false;

		CastEvent(ET_BEFORE_INIT, (IBaseEvent*)&CEvBeforeInit(&_stWin, &_eInitFlags));

		bool do_spl = !((uint)_eInitFlags & EIF_NO_SPLASH) && InstIdx() == 0;

		if (do_spl)
		{
			_pSplashWindow = new CSplashWindow(InstIdx());
			_pSplashWindow->InitWindow(!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_SINGLE_THREAD), _pcCustomSplash);
		}

		if (_eInitFlags & EIF_CATCH_UNHANDLED) 
			InitDbgHelp(InstIdx());

		string system_info;
		GetSystemInformation(system_info, _stSysInfo);
		LOG(system_info.c_str(), LT_INFO);

		_bSndEnabled = !(_eInitFlags & EIF_FORCE_NO_SOUND);

		string eng_path;
		GetEngineFilePath(eng_path);

		if (_eInitFlags & EIF_LOAD_ALL_PLUGINS)
		{
			if (!FindFilesInDir((eng_path + "*.dplug").c_str(), _clPluginInitList, true))
				LOG("Plugin search routine fails.", LT_ERROR);
		}
		else
		{
			string ext_fnames[] = {eng_path + "DGLE2_EXT.dplug", eng_path + "plugins\\DGLE2_EXT.dplug"};

			for (int i = 0; i < sizeof(ext_fnames)/sizeof(ext_fnames[0]); ++i)
				if (_access(ext_fnames[i].c_str(), 0) != -1)
				{
					_clPluginInitList.push_back(ext_fnames[i]);
					break;
				}
		}

		_clDelUpdate.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelRender.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelInit.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);
		_clDelFree.CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);

		Console()->RegComValue("core_allow_pause", "Pauses main process rutine when window losts focus.", &_iAllowPause, 0, 1, &_s_ConAutoPause, (void*)this);
		Console()->RegComValue("core_fps_in_caption", "Displays current fps value in window caption.", &_iFPSToCaption, 0, 1, NULL, (void*)this);
		Console()->RegComValue("core_profiler", "Displays engine core profiler.\r\0 - hide profiler.\r\n1 - simple draw FPS and UPS.\r\n2 - additional draw performance graphs.", &_iDrawProfiler, 0, 2, NULL, (void*)this);
		Console()->RegComValue("core_allow_profilers", "Allow or not rendering various engine profilers.", &_iAllowDrawProfilers, 0, 1, NULL, (void*)this);
		Console()->RegComProc ("core_version", "Prints engine version.", &_s_ConPrintVersion, (void*)this);
		Console()->RegComProc ("core_features", "Prints list of features with which engine was build.\r\nw - write to logfile.", &_s_ConFeatures, (void*)this);
		Console()->RegComProc ("core_list_plugins", "Prints list of connected plugins.", &_s_ConListPlugs, (void*)this);
		Console()->RegComProc ("core_instidx", "Prints Instance Index of current engine unit.", &_s_InstIdx, (void*)this);
		Console()->RegComProc("core_setmode", "Changes display mode.\r\nUsage: \"core_setmode [ScrWidth] [ScrHeight] [Fullscreen(0 or 1)] [VSync(0 or 1)] [MSAA(from 1 to 8)]\"\r\nExample:\"core_setmode 800 600 1 1 4\"", &_s_ConChangeMode, (void*)this);

		if (!_pMainWindow)
		{
			if (tHandle == NULL)
				_pMainWindow = new CMainWindow(InstIdx());
			else
			{
				_pMainWindow = new CHookedWindow(InstIdx());
				_iAllowPause = 0;
			}
		}

		_clDelOnFPSTimer.Add(&_s_OnTimer, (void*)this);
		_clDelMLoop.Add(&_s_MainLoop, (void*)this);
		_clDelMProc.Add(&_s_MessageProc, (void*)this);

		if (!_clPluginInitList.empty())
		{
			for (size_t i = 0; i < _clPluginInitList.size(); ++i)
				for (size_t j = i; j < _clPluginInitList.size(); ++j)
					if (ToUpperCase(GetOnlyFileName(_clPluginInitList[i].c_str())) == ToUpperCase(GetOnlyFileName(_clPluginInitList[j].c_str())))
					{
						_clPluginInitList.erase(_clPluginInitList.begin() + j);
						LOG("Found duplicated plugin \"" + GetOnlyFileName(_clPluginInitList[i].c_str()) + "\" in plugins initialization list.", LT_WARNING);
						break;
					}

			for (size_t i = 0; i < _clPluginInitList.size(); ++i)
			{
				IPlugin *plugin;
				if (S_OK == ConnectPlugin(_clPluginInitList[i].c_str(), plugin))
				{
					char iname[c_MaxPluginInterfaceName];
					PARANOIC_CHECK_HR(plugin->GetPluginInterfaceName(iname, c_MaxPluginInterfaceName));
					
					if (strcmp(iname, "ISubSystemPlugin") == 0)
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
							if (!_bBuiltInInput)
							{
								LOG("Only one Sound plugin could be connected at the same time.", LT_WARNING);
								_UnloadPlugin(plugin);
								break;
							}
							else
							{
								if (_bSndEnabled)
								{ 
									_pSound = (ISound*)pss;
									_bBuiltInSound = false;
									break;
								}
							}
						default: 
							LOG("The subsystem which plugin tries to override is not overridable.", LT_ERROR);
							_UnloadPlugin(plugin);
						}
					}
				}			
			}
		}

		if (_bBuiltInRenderer)
		{
#ifndef NO_BUILTIN_RENDERER

			LOG("Using builtin OpenGL Legacy renderer.", LT_INFO);
			_pCoreRenderer = new CCoreRendererGL(InstIdx());
#else
			LOG("No Core Renderer plugin connected!", LT_FATAL);
			return E_INVALIDARG;
#endif
		}

		TCRendererInitResult rnd_init_res;

		if (FAILED(_pCoreRenderer->Prepare(rnd_init_res)))
			return E_ABORT;

		if (FAILED(_pMainWindow->InitWindow(tHandle, rnd_init_res, &_clDelMLoop, &_clDelMProc)))
			return E_ABORT;

		if (do_spl)
		{
			TWinHandle t_h;
			_pMainWindow->GetWindowHandle(t_h);
			_pSplashWindow->SetOwnerWindow(t_h);
		}

		if (strlen(pcApplicationName) < c_AppCaptionMaxLength)
			strcpy(_pcApplicationCaption, pcApplicationName);
		else
		{
			memcpy(_pcApplicationCaption, pcApplicationName, c_AppCaptionMaxLength - 1);
			_pcApplicationCaption[c_AppCaptionMaxLength - 1] = '\0';
			LOG("Application name is too long.", LT_WARNING);	   
		}

		_pMainWindow->SetCaption(_pcApplicationCaption);

		if (_uiFPSTimer = CreateTimer(1000, &_clDelOnFPSTimer), _uiFPSTimer == -1) 
			LOG("Can't set fps timer.", LT_FATAL);	   

		_pMainFS = new CMainFS(InstIdx());

		if (FAILED(_pCoreRenderer->Initialize(rnd_init_res)))
			return E_ABORT;

		_pRender = new CRender(InstIdx());

		_pResMan = new CResourceManager(InstIdx());

//		if (_bSndEnabled)
//			_pSound = new CSoundMCI(InstIdx());
		
//		_pInput	= (IInput*)(new CInput(InstIdx()));

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

HRESULT CALLBACK CCore::SetProcessInterval(uint uiProcessInterval)
{
	_uiProcessInterval = uiProcessInterval;
	return S_OK;
}

HRESULT CALLBACK CCore::GetSystemInfo(TSystemInfo &stSysInfo)
{
	stSysInfo = _stSysInfo;
	return S_OK;
}

HRESULT CALLBACK CCore::GetCurrentWin(TEngWindow &stWin)
{
	stWin = _stWin;
	return S_OK;
}

HRESULT CALLBACK CCore::ChangeWinMode(const TEngWindow &stNewWin)
{
	TEngWindow wnd = stNewWin;

	if (SUCCEEDED(_pMainWindow->ConfigureWindow(wnd)) && SUCCEEDED(_pCoreRenderer->AdjustMode(wnd)))
	{
		_stWin = wnd;

		_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);

		Console()->ResetWinPos();

		E_WINDOW_ACCESS_TYPE e_actype;
		_pMainWindow->GetWindowAccessType(e_actype);
		string access;
		
		switch(e_actype)
		{
			case WAT_FULL_ACCESS:
				access = "Full";
				break;
			case WAT_RESTRICTED_ACCESS:
				access = "Restricted";
				break;
			case WAT_NO_ACCESS:
				access = "Isolated";
				break;
			default:
				access = "Unknown";
		}

		LOG(string("Window mode (Viewport: ") + IntToStr(_stWin.uiWidth) + "X" + IntToStr(_stWin.uiHeight) +
			(_eInitFlags & EIF_FORCE_16_BIT_COLOR ? " 16 bit" : "") +
			", WinCtrlAccess: " + access + (_stWin.bFullScreen?", Fullscreen" : "") + (_stWin.bVSync?", VSync":"") +
			(_stWin.eMSampling != MM_NONE ? ", MSAA: " + IntToStr((int)pow(2.f, (int)_stWin.eMSampling)) + "X" : "") +
			") has been set properly.", LT_INFO);
	}
	else
	{
		LOG("Required window mode has not been set due to some errors.", LT_ERROR);
		return E_ABORT;
	}

	return S_OK;
}

HRESULT CALLBACK CCore::StartEngine()
{
	if (_bStartedFlag)
		return S_FALSE;

	if (FAILED(_pCoreRenderer->MakeCurrent()))
		return E_ABORT;

	if (!_clDelInit.IsNull()) 
	{
		LOG("Calling user initialization procedure...", LT_INFO);
		_clDelInit.Invoke();
		LOG("Done.", LT_INFO);
	}

	_uiLastUPS			= 0;
	_uiUPSCount			= 0;
	_uiLastFPS			= 0;
	_uiFPSCount			= 0;
	_ui64FPSSumm		= 0;
	_ui64CiclesCount	= 0;
	_ui64TimeOld		= GetPerfTimer()/1000 - _uiProcessInterval;

	if (_pSplashWindow) 
		_pSplashWindow->Free();

	if (FAILED(_pMainWindow->ConfigureWindow(_stWin)) || FAILED(_pCoreRenderer->AdjustMode(_stWin)))
		return E_ABORT;

	_pRender->OnResize(_stWin.uiWidth, _stWin.uiHeight);

	HRESULT hr = _pMainWindow->BeginMainLoop();

	_bStartedFlag = SUCCEEDED(hr);

	return hr;
}

HRESULT CALLBACK CCore::QuitEngine()
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
		LOG("Direct calling of \"QuitEngine()\" rootine is note allowed when engine doesn't own window, close root window instead.", LT_WARNING);
		return S_FALSE;
	}

	_bDoExit = true;
	_bQuitFlag = true;

	return S_OK;
}

HRESULT CALLBACK CCore::AllowPause(bool bAllow)
{
	_iAllowPause = bAllow;
	_s_ConAutoPause((void*)this, "");
	return S_OK;
}

HRESULT CALLBACK CCore::GetFPS(uint &uiFPS)
{
	uiFPS = _uiLastFPS;
	return S_OK;
}

HRESULT CALLBACK CCore::GetInstanceIdx(uint &uiIdx)
{
	uiIdx = InstIdx();
	return S_OK;
}

HRESULT CALLBACK CCore::GetTimer(uint64 &uiTick)
{
	uiTick = GetPerfTimer()/1000;
	return S_OK;
}

HRESULT CALLBACK CCore::CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent)
{	
	for(size_t i = 0; i < _clEvents.size(); ++i)
		if(eEventType == _clEvents[i].eType)
		{
			if(!_clEvents[i].pDEvent->IsNull())
				_clEvents[i].pDEvent->Invoke(pEvent);
			return S_OK;
		}

	return S_FALSE;
}

HRESULT CALLBACK CCore::AddEventListner(E_EVENT_TYPE eEventType, void (CALLBACK *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr)
{
	if (eEventType == ET_BEFORE_INIT && _bInitedFlag)//Means that engine already inited and event will never happend
		return S_FALSE;
	
	for (size_t i = 0; i < _clEvents.size(); ++i)
		if (eEventType == _clEvents[i].eType)
		{
			_clEvents[i].pDEvent->Add(pListnerProc, pParametr);
			return S_OK;
		}

	TEvent new_event;
	new_event.eType = eEventType;
	new_event.pDEvent = new TEventProcDelegate(InstIdx());
	new_event.pDEvent->Add(pListnerProc, pParametr);
	new_event.pDEvent->CatchExceptions(_eInitFlags & EIF_CATCH_UNHANDLED);

	_clEvents.push_back(new_event);

	return S_OK;
}

HRESULT CALLBACK CCore::RemoveEventListner(E_EVENT_TYPE eEventType, void (CALLBACK *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr)
{
	for (size_t i = 0; i < _clEvents.size(); ++i)
		if (eEventType == _clEvents[i].eType)
		{
			_clEvents[i].pDEvent->Remove(pListnerProc, pParametr);
			return S_OK;
		}

	return E_INVALIDARG;
}

HRESULT CALLBACK CCore::AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (CALLBACK *pProc)(void *pParametr), void *pParametr)
{
	switch(eProcType)
	{
	case EPT_UPDATE:
		_clDelUpdate.Add(pProc, pParametr);
		break;
	case EPT_RENDER:
		_clDelRender.Add(pProc, pParametr);
		break;
	case EPT_INIT:
		_clDelInit.Add(pProc, pParametr);
		if(_bStartedFlag)
		{
			(*pProc)(pParametr);
			return S_FALSE;
		}
		break;
	case EPT_FREE:
		_clDelFree.Add(pProc, pParametr);
		break;
	default: return E_INVALIDARG;
	}

	return S_OK;
}

HRESULT CALLBACK CCore::RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (CALLBACK *pProc)(void *pParametr), void *pParametr)
{
	switch(eProcType)
	{
	case EPT_UPDATE:
		_clDelUpdate.Remove(pProc, pParametr);
		break;
	case EPT_RENDER:
		_clDelRender.Remove(pProc, pParametr);
		break;
	case EPT_INIT:
		_clDelInit.Remove(pProc, pParametr);
		break;
	case EPT_FREE:
		_clDelFree.Remove(pProc, pParametr);
		break;
	default: return E_INVALIDARG;
	}

	return S_OK;
}

HRESULT CALLBACK CCore::AddToLog(const char *pcTxt)
{
	_LogWrite(pcTxt);
	return S_OK;
}

HRESULT CALLBACK CCore::AddToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	_LogWriteEx(pcTxt, eType, pcSrcFileName, iSrcLineNumber);
	return S_OK;
}

void CALLBACK CCore::_s_InstIdx(void *pParametr, const char *pcParam)
{
	CON(CCore, (string("Instance Index is ")+IntToStr(PTHIS(CCore)->InstIdx())+".").c_str());
}

void CALLBACK CCore::_s_ConFeatures(void *pParametr, const char *pcParam)
{
	bool write = strlen(pcParam)!=0 && pcParam[0] == 'w';

	string res = string("Engine was build with:\r\n") +
#ifdef _DEBUG
		"* DEBUG build.\r\n"
#else
		"* Release build.\r\n"
#endif		
		+
		"* Engine SDK version: " + IntToStr(_DGLE2_SDK_VER_) + ".\r\n" +
		"* Plugin SDK version: " + IntToStr(_DGLE2_PLUGIN_SDK_VER_) + ".\r\n" +
#ifdef DGLE2_STATIC
		"* Static library build.\r\n"
#endif
#ifdef PLATFORM_WINDOWS
#ifdef _WIN64
		"* Target platform Win64.\r\n"
#elif _WIN32
		"* Target platform Win32.\r\n"
#endif
#endif
#ifdef STRUCT_ALIGNMENT_1
		"* All engine structures are aligned by 1 byte.\r\n"
#endif
#ifdef PDB_DEBUG
		"* Unhandled Exceptions Filter with support of PDB files.\r\n"
#endif
#ifdef DGLE2_USE_COM
		"* Support of Microsoft COM technology for engine interfaces.\r\n"
#endif
#ifdef PLATFORM_WINDOWS
#	ifdef NO_DIRECTX
		"* No Microsoft DirectX build.\r\n"
#	elif defined(DXDIAG_VIDEO_INFO)
		"* Uses Microsoft DirectX diagnostic information.\r\n"
#	endif
#endif
#ifdef NO_BUILTIN_RENDERER
		"* No builtin Renderer.\r\n"
#else
		"* Builtin OpenGL Legacy Renderer.\r\n"
#endif
#ifdef NO_BUILTIN_SOUND
		"* No builtin Sound Subsystem.\r\n"
#else
		"* Builtin Sound Subsystem.\r\n"
#endif
#ifdef NO_BUILTIN_INPUT
		"* No builtin Input Subsystem.\r\n"
#else
		"* Builtin Input Subsystem.\r\n"
#endif
		;

	if (write)
		LOG2(CCore, res, LT_INFO);
	else
		CON(CCore, res.c_str());
}

void CALLBACK CCore::_s_ConPrintVersion(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam)!=0)
		CON(CCore, "No parametrs expected.");
	else 
		CON(CCore, (string("Engine version: ")+string(DGLE2_VERSION)).c_str());
}

void CALLBACK CCore::_s_ConAutoPause(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam)!=0)
		CON(CCore, "No parametrs expected.");
}

void CALLBACK CCore::_s_ConListPlugs(void *pParametr, const char *pcParam)
{
	if(strlen(pcParam)!=0)
		CON(CCore, "No parametrs expected.");
	else
		PTHIS(CCore)->_PrintPluginsInfo();
}

void CALLBACK CCore::_s_ConChangeMode(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) == 0)
		CON(CCore, "Parametrs expected.");
	else
	{
		TEngWindow wnd; int samples;
		istrstream params_str(pcParam);
		params_str >> wnd.uiWidth >> wnd.uiHeight >> wnd.bFullScreen >> wnd.bVSync >> samples;

		switch (samples)
		{
		case 2: wnd.eMSampling = MM_2X; break;
		case 4: wnd.eMSampling = MM_4X; break;
		case 8: wnd.eMSampling = MM_8X; break;
		default: wnd.eMSampling = MM_NONE;
		}

		wnd.uiFlags = PTHIS(CCore)->_stWin.uiFlags;
		PTHIS(CCore)->_stWndToApply = wnd;
		PTHIS(CCore)->_bNeedApplyNewWnd = true;
	}
}

HRESULT CALLBACK CCore::GetHandle(TWinHandle &tHandle)
{
	_pMainWindow->GetWindowHandle(tHandle);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleVisible(bool bIsVisible)
{
	Console()->Visible(bIsVisible);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleWrite(const char *pcTxt, bool bWriteToPreviousLine)
{
	Console()->Write(pcTxt, bWriteToPreviousLine);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleExec(const char *pcCommandTxt)
{
	Console()->Exec(pcCommandTxt);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleRegComProc(const char *pcCommandName, const char *pcCommandHelp, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr)
{
	Console()->RegComProc(pcCommandName, pcCommandHelp, pProc, pParametr);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleRegComValue(const char *pcCommandName, const char *pcCommandHelp, int *piValue, int iMinValue, int iMaxValue, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr)
{
	Console()->RegComValue(pcCommandName, pcCommandHelp, piValue, iMinValue, iMaxValue, pProc, pParametr);
	return S_OK;
}

HRESULT CALLBACK CCore::ConsoleUnregCom(const char *pcCommandName)
{
	if (Console()->UnRegCom(pcCommandName))
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CALLBACK CCore::GetVersion(char* pcBuffer, uint uiBufferSize)
{
	if(uiBufferSize < strlen(DGLE2_VERSION))
		return E_INVALIDARG;

	strcpy(pcBuffer, DGLE2_VERSION);

	return S_OK;
}

HRESULT CALLBACK CCore::GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem)
{
	prSubSystem = NULL;

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
			prSubSystem = (IEngineSubSystem*)_pInput;
			break;	

		case ESS_SOUND:
			if(_pSound == NULL)
				return E_INVALIDARG;
			else
				prSubSystem = (IEngineSubSystem*)_pSound;
			break;

		default:
			return E_INVALIDARG;
	}

	CastEvent(ET_ON_GET_SSYSTEM, (IBaseEvent*)&CEvGetSubSystem(eSubSystem, prSubSystem));

	return S_OK;
}

void CALLBACK CCore::_s_OnTimer(void *pParametr)
{
	PTHIS(CCore)->_OnTimer();
}

void CALLBACK CCore::_s_MainLoop(void *pParametr)
{
	PTHIS(CCore)->_MainLoop();
}

void CALLBACK CCore::_s_MessageProc(void *pParametr, const TWinMessage &stMsg)
{
	PTHIS(CCore)->_MessageProc(stMsg);
}