/**
\author		Korotkov Andrey aka DRON
\date		26.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "MainWindow.h"
#include "..\..\..\..\build\win\engine\resource.h"

extern HMODULE hModule;

CMainWindow::CMainWindow(uint uiInstIdx):
CInstancedObj(uiInstIdx), _tWnd(NULL),
_tInst(GetModuleHandle(NULL)), _bFScreen(false),
_bIsLooping(false)
{}

CMainWindow::~CMainWindow()
{
	if (_tInst && ((InstIdx()==0 || EngineInstance(0)->pclCore == NULL) &&
		UnregisterClass("DGLE2WindowClass", _tInst)==FALSE))
	{
		_tInst = NULL;
		LOG("Can't unregister window class.",LT_ERROR);
	}
	else
		LOG("Window closed properly.",LT_INFO);	
}

int CMainWindow::_wWinMain(HINSTANCE hInstance)
{
	LOG("**Entering main loop**", LT_INFO);

	_bIsLooping = true;
	
	MSG st_msg = {0};

	while (_bIsLooping)
		if (PeekMessage(&st_msg, NULL, 0, 0, PM_REMOVE ))
		{
			if (WM_QUIT == st_msg.message) 
				_bIsLooping = false;
			else
			{   
				TranslateMessage(&st_msg);
				DispatchMessage (&st_msg);
			}
		}
		else 
			_pDelMainLoop->Invoke();

	_pDelMessageProc->Invoke(TWinMessage(WMT_RELEASED));

	LOG("**Exiting main loop**", LT_INFO);

	return (int) st_msg.wParam;
}

void CALLBACK CMainWindow::_s_ConsoleQuit(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		CON(CMainWindow, "No parametrs expected.");
	else 
		::SendMessage(PTHIS(CMainWindow)->_tWnd, WM_CLOSE, NULL, NULL);
}

LRESULT CALLBACK CMainWindow::_s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CMainWindow *this_ptr = (CMainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (this_ptr)
	{
		if (message == WM_DESTROY)
			PostQuitMessage(0);

		this_ptr->_pDelMessageProc->Invoke(WinAPIMsgToEngMsg(message, wParam, lParam));

		if ((message == WM_SYSCOMMAND && (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)) || 
			message == WM_CLOSE)
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT CMainWindow::InitWindow(TWinHandle tHandle, const TCRendererInitResult &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc)
{
	_tWnd				= tHandle;
	_pDelMainLoop		= pDelMainLoop;
	_pDelMessageProc	= pDelMsgProc;

	WNDCLASSEX wcex;
	wcex.cbSize 		= sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc    = (WNDPROC)CMainWindow::_s_WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = _tInst;
	wcex.hIcon          = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(0);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "DGLE2WindowClass";
	wcex.hIconSm        = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));

	bool need_register = true;

	if (InstIdx()!=0)
	{
		WNDCLASSEX tmp;
		need_register = GetClassInfoEx(_tInst, "DGLE2WindowClass", &tmp) == FALSE;
	}

	if (need_register && RegisterClassEx(&wcex) == FALSE)
	{
		LOG("Couldn't register window class!", LT_FATAL);
		return E_FAIL;
	}

	if (_tWnd)
		return E_INVALIDARG;

	_tWnd = CreateWindowExA(WS_EX_APPWINDOW, "DGLE2WindowClass", "DGLE2 Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, _tInst, NULL);

	if (!_tWnd)
	{
		_tWnd = NULL;
		LOG("Failed to create window.",LT_FATAL);
		return E_FAIL;
	}

	SetWindowLongPtr(_tWnd, GWLP_USERDATA, (LONG_PTR)this);

	if (!(_tDC = GetDC(_tWnd)))
	{
		LOG("Can't get window Draw Context.",LT_FATAL);
		return E_FAIL;
	}

	Console()->RegComProc("quit", "Quits engine and releases all resources.", &_s_ConsoleQuit, (void*)this);

	LOG("Window created successfully.",LT_INFO);

	return S_OK;
}

HRESULT CMainWindow::SendMessage(const TWinMessage &stMsg)
{
	UINT msg; WPARAM wparam; LPARAM lparam;
	EngMsgToWinAPIMsg(stMsg, msg, wparam, lparam);
	::SendMessage(_tWnd, msg, wparam, lparam);
	return S_OK;
}

HRESULT CMainWindow::GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType)
{
	eType = WAT_FULL_ACCESS;
	return S_OK;
}

HRESULT CMainWindow::GetWindowHandle(TWinHandle &stHandle)
{
	stHandle = _tWnd;
	return S_OK;
}

HRESULT CMainWindow::GetDrawContext(HDC &hDC)
{
	hDC = _tDC;
	return S_OK;
}

HRESULT CMainWindow::GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight)
{
	RECT rect;
	
	if (GetClientRect(_tWnd, &rect) == FALSE)
	{
		iX = iY = iWidth = iHeight = 0;
		LOG("Can't get window client rect.", LT_ERROR);
		return E_FAIL;
	}

	POINT lt, rb;
	
	lt.x = rect.left;
	lt.y = rect.top;
	rb.x = rect.right;
	rb.y = rect.bottom;

	ClientToScreen(_tWnd, &lt);
	ClientToScreen(_tWnd, &rb);

	iX = lt.x;
	iY = rb.x;
	iWidth = rb.x - lt.x;
	iHeight = rb.y - lt.y;

	return S_OK;
}

HRESULT CMainWindow::SetCaption(const char *pcTxt)
{
	SetWindowText(_tWnd, pcTxt);
	return S_OK;
}

HRESULT CMainWindow::BeginMainLoop()
{
	return _wWinMain(GetModuleHandle(NULL)) != -1 ? S_OK : E_FAIL;
}

HRESULT CMainWindow::KillWindow()
{
	if (_tDC && !ReleaseDC(_tWnd,_tDC))
		LOG("Failed to release Device Context.",LT_ERROR);

	if (DestroyWindow(_tWnd) == FALSE)
	{
		LOG("Can't destroy window.",LT_ERROR);
		return S_FALSE;
	}

	return S_OK;
}

HRESULT CMainWindow::ConfigureWindow(const TEngWindow &stWind)
{
	bool builtin_fscreen;

	Core()->pCoreRenderer()->IsFeatureSupported(CRSF_BUILTIN_FSCREEN_MODE, builtin_fscreen);

	if (builtin_fscreen)
		return S_OK;

	DWORD dw_style		= WS_VISIBLE;
	DWORD dw_style_ex	= WS_EX_APPWINDOW;
	HRESULT res = S_OK;

	if (_bFScreen && !stWind.bFullScreen)
		if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			LOG("Can't switch off fullscreen mode.", LT_ERROR);
			res = S_FALSE;
		}

	if (stWind.bFullScreen)												
	{
		DEVMODE dm_scr_settings;								
		memset(&dm_scr_settings, 0, sizeof(dm_scr_settings));	
		dm_scr_settings.dmSize			= sizeof(dm_scr_settings);		
		dm_scr_settings.dmPelsWidth		= stWind.uiWidth;				
		dm_scr_settings.dmPelsHeight	= stWind.uiHeight;				
		dm_scr_settings.dmBitsPerPel	= Core()->InitFlags() & EIF_FORCE_16_BIT_COLOR ? 16 : 32;					
		dm_scr_settings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettingsEx(NULL ,&dm_scr_settings, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
		{
			LOG("Can't set fullscreen mode(" +IntToStr(stWind.uiWidth)+"X"+IntToStr(stWind.uiHeight)+"), switching back to windowed mode.", LT_ERROR);
			_bFScreen = false;
			const_cast<TEngWindow *>(&stWind)->bFullScreen = false; //hack consider to remove const in method prototype
			res = S_FALSE;
		}
		else
			_bFScreen = true;
	}

	if (stWind.bFullScreen)	
		dw_style |= WS_POPUP;
	else
	{
		if (stWind.uiFlags & EWF_ALLOW_SIZEING)
			dw_style |= WS_OVERLAPPEDWINDOW;
		else
			dw_style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}

	if (stWind.uiFlags & EWF_TOPMOST)
		dw_style_ex |= WS_EX_TOPMOST;

	if (SetWindowLong(_tWnd, GWL_EXSTYLE, dw_style_ex) == 0)
	{
		LOG("Can't change window styleEx.", LT_ERROR);
		res = S_FALSE;
	}

	if (SetWindowLong(_tWnd, GWL_STYLE, dw_style) == 0)
	{
		LOG("Can't change window style.", LT_ERROR);
		res = S_FALSE;
	}

	uint desktop_width = 0, desktop_height = 0;

	RECT rc = {0, 0, stWind.uiWidth, stWind.uiHeight};
	int	 top_x = 0, top_y = 0;

	AdjustWindowRectEx(&rc, dw_style , FALSE, dw_style_ex);

	if (!stWind.bFullScreen)
	{
		GetDisplaySize(desktop_width, desktop_height);

		top_x = (int)(desktop_width - (rc.right - rc.left))/2, 
		top_y = (int)(desktop_height - (rc.bottom - rc.top))/2;

		if (top_x < 0) top_x = 0;
		if (top_y < 0) top_y = 0;
	}

	SetWindowPos(_tWnd, HWND_TOP, top_x, top_y, rc.right - rc.left, rc.bottom - rc.top, SWP_FRAMECHANGED);

	if (_bFScreen && !stWind.bFullScreen)
	{
		RECT r;
		r.left = 0; r.top = 0;
		r.right = desktop_width; r.bottom = desktop_height;
		InvalidateRect(NULL, &r, TRUE);
		_bFScreen = false;
	}

	SetCursorPos(top_x + (rc.right - rc.left)/2, top_y + (rc.bottom - rc.top)/2);
		
	if (IsWindowVisible(_tWnd) == FALSE)
		ShowWindow(_tWnd, SW_SHOWNA);

	SetForegroundWindow(_tWnd);

	return res;
}

HRESULT CMainWindow::Free()
{
	delete this;
	return S_OK;
}