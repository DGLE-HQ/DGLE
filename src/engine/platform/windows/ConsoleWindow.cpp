/**
\author		Korotkov Andrey aka DRON
\date		17.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "ConsoleWindow.h"
#include "..\..\..\..\build\windows\engine\resource.h"

using namespace std;

extern HMODULE hModule;

#define C_WND_X 20
#define C_WND_Y -10
#define C_WND_WIDTH  450
#define C_WND_HEIGHT 250
#define C_WND_MIN_WIDTH  300
#define C_WND_MIN_HEIGHT 115
#define C_WND_EDIT_HEIGHT 16

LOGFONT LF = {12, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Lucida Console"};

CConsoleWindow::CConsoleWindow():
_strOnCreate("DGLE Console created..."),
_iPrevLineSize(22),
_bIsLooping(false),
_bToPrevLineActive(false),
_hWnd(NULL), _hMemo(NULL), _hEdit(NULL),
_hInst(NULL), _hThreadHandle(NULL), _threadId(0),
_pOldEditProc(NULL), _bVisible(false),
_pConWindowEvent(NULL), _pConsole(NULL)
{}

DGLE_RESULT CConsoleWindow::InitWindow(bool bSeparateThread, void (DGLE_API *pConWindowEvent)(CConsole *pConsole, E_CONSOLE_WINDOW_EVENT eEventType, const char *pcCommand), CConsole *pConsole)
{
	_pConWindowEvent = pConWindowEvent;
	_pConsole = pConsole;

	if (bSeparateThread)
	{
		InitializeCriticalSection(&_cs);
		_hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_s_ThreadProc, (PVOID)this, 0, &_threadId);
	}
	
	if (!_hThreadHandle)
		_WinMain(hModule);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::Visible(bool bVisible)
{
	if (!bVisible)
		ShowWindow(_hWnd, SW_HIDE);
	else
	{
		if (_hWnd == NULL) 
		{
			_bVisible = true;
			return S_OK;
		}
		ShowWindow(_hWnd, SW_SHOWNA);
		SetForegroundWindow(_hWnd);
		RECT hwrc;
		GetWindowRect(_hWnd, &hwrc);
		SetCursorPos(hwrc.left + (hwrc.right - hwrc.left) / 2, hwrc.top + (hwrc.bottom - hwrc.top) / 2);
		SetFocus(_hEdit);
		UpdateWindow(_hWnd);
	}

	return S_OK;
}

DGLE_RESULT CConsoleWindow::SetSizeAndPos(int iX, int iY, int iWidth, int iHeight)
{
	_iX = iX;
	_iY = iY;
	_iWidth = iWidth;
	_iHeight = iHeight;

	MoveWindow(_hWnd, iX, iY, iWidth, iHeight, true);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::GetSizeAndPos(int &iX, int &iY, int &iWidth, int &iHeight)
{
	iX = _iX;
	iY = _iY;
	iWidth = _iWidth;
	iHeight = _iHeight;

	return S_OK;
}

DGLE_RESULT CConsoleWindow::GetWindowHandle(TWindowHandle &tHandle)
{
	tHandle = _hWnd;

	return S_OK;
}

DGLE_RESULT CConsoleWindow::GetThreadId(uint32 &ui32Id)
{
	ui32Id = _threadId;

	return S_OK;
}

DGLE_RESULT CConsoleWindow::OutputTxt(const char *pcTxt, bool bToPrevLine)
{
	int cur_l = GetWindowTextLength(_hMemo);
	
	if (cur_l + strlen(pcTxt) + 4 >= _sc_uiMaxConsoleTxtLength)
	{
		SetWindowText(_hMemo, "DGLE Console cleared...");
		cur_l = GetWindowTextLength(_hMemo);
	}
	
	if (_bToPrevLineActive && bToPrevLine) 
	{
		SendMessage(_hMemo, EM_SETSEL, cur_l - _iPrevLineSize, cur_l);
		SendMessage(_hMemo, EM_REPLACESEL, false, (LPARAM)pcTxt);
	}
	else
	{
		_bToPrevLineActive = bToPrevLine;

		if (_hThreadHandle && _hMemo == NULL)
			_strOnCreate += "\r\n"s + pcTxt;
		else
		{
			SendMessage(_hMemo, EM_SETSEL, cur_l, cur_l);
			SendMessage(_hMemo, EM_REPLACESEL, false, (LPARAM)("\r\n"s + pcTxt).c_str());
			SendMessage(_hMemo, EM_SCROLL, SB_BOTTOM, 0);
		}
	} 

	_iPrevLineSize = (int)strlen(pcTxt);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::GetEditTxt(char *pcTxt, uint uiBufferSize)
{
	GetWindowText(_hEdit, pcTxt, uiBufferSize);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::SetEditTxt(const char *pcTxt)
{
	SetWindowText(_hEdit, pcTxt);
	SendMessage(_hEdit, WM_KEYDOWN, 35 /*end*/, 0);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::GetConsoleTxt(char *pcTxt, uint &uiBufferSize)
{
	if (!pcTxt)
	{
		uiBufferSize = _sc_uiMaxConsoleTxtLength + 1;
		return S_OK;
	}

	return uiBufferSize - 1 == GetWindowText(_hMemo, pcTxt, uiBufferSize) ? S_FALSE : S_OK;
}

DGLE_RESULT CConsoleWindow::Clear()
{
	SetWindowText(_hMemo, "DGLE Console cleared...");

	return S_OK;
}

DGLE_RESULT CConsoleWindow::ResetSizeAndPos()
{
	uint dt_w, dt_h;
	GetDisplaySize(dt_w, dt_h);

	RECT rc_tb;
	GetWindowRect(FindWindow("Shell_TrayWnd", NULL), &rc_tb);

	_iWidth = C_WND_WIDTH;
	_iHeight = C_WND_HEIGHT;

	RECT rc = { 0, 0, C_WND_WIDTH, C_WND_HEIGHT };
	AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX, false, WS_EX_TOOLWINDOW);

	_iX = (int)(C_WND_X + min(rc_tb.bottom - rc_tb.top, rc_tb.right - rc_tb.left));
	_iY = (int)(dt_h - (rc.bottom - rc.top) + C_WND_Y - min(rc_tb.bottom - rc_tb.top, rc_tb.right - rc_tb.left));

	MoveWindow(_hWnd, _iX, _iY, _iWidth, _iHeight, true);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::EnterThreadSafeSection()
{
	if (_hThreadHandle)
		EnterCriticalSection(&_cs);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::LeaveThreadSafeSection()
{
	if (_hThreadHandle)
		LeaveCriticalSection(&_cs);

	return S_OK;
}

DGLE_RESULT CConsoleWindow::Free()
{
	if (_hThreadHandle)
	{
		PostThreadMessage(_threadId, WM_QUIT, 0, NULL);

		//Note: WaitForSingleObject won't work here, so we are using this walkaround.
		DWORD tick = GetTickCount();
		while (_hWnd && GetTickCount() - tick < 500) Sleep(0);

		if (_hWnd)
			TerminateThread(_hThreadHandle, 1);

		CloseHandle(_hThreadHandle);
		DeleteCriticalSection(&_cs);
	}

	if (_hWnd && FALSE != DestroyWindow(_hWnd))
		UnregisterClass("DGLEConsoleClass", _hInst);

	delete this;

	return S_OK;
}

int WINAPI CConsoleWindow::_WinMain(HINSTANCE hInstance)	
{
	_hInst = hInstance;

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc    = (WNDPROC)CConsoleWindow::_s_WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = _hInst; 
	wcex.hIcon          = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_GRAYTEXT);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "DGLEConsoleClass";
	wcex.hIconSm        = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));
	
	if (FindAtom("DGLEConsoleClass") == NULL && !RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed to register console class!", "DGLE Console", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return E_FAIL;
	}

	_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "DGLEConsoleClass", "DGLE Console", 
							WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							NULL, NULL, hInstance, NULL);

	if (!_hWnd)
	{
		MessageBox(NULL, "Failed to create console window!", "DGLE Console", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return E_FAIL;
	}

	SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	_hMemo = CreateWindow(	"EDIT", "DGLE Console created...", 
							WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | 
							ES_MULTILINE | ES_READONLY, 
							0, 0, 0, 0, _hWnd, 0, 0, NULL );

	if (_strOnCreate.size() != 0)
	{
		SetWindowText(_hMemo, _strOnCreate.c_str());
		_strOnCreate.clear();
	}

	_hFont = CreateFontIndirect(&LF);
	
	SendMessage(_hMemo, WM_SETFONT, (WPARAM)_hFont, MAKELPARAM(TRUE,0));

	_hEdit = CreateWindow(	"EDIT", "",
							WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
							0, 0, 0, 0, _hWnd, 0, 0, NULL);
	
	SetWindowLongPtr(_hEdit, GWLP_USERDATA, (LONG_PTR)this);

	_pOldEditProc = (void*)SetWindowLongPtr(_hEdit, GWLP_WNDPROC, (LONG_PTR)(WNDPROC)CConsoleWindow::_s_WndEditProc); 

	SendMessage(_hEdit, WM_SETFONT, (WPARAM)_hFont, MAKELPARAM(TRUE, 0));
	
	ResetSizeAndPos();

	if (_bVisible)
		ShowWindow(_hWnd, SW_SHOWNORMAL);

	return S_OK;
}

void CConsoleWindow::_Realign()
{
	RECT rect;
	GetClientRect(_hWnd, &rect);
	MoveWindow(_hMemo, 0, 0, rect.right, rect.bottom - C_WND_EDIT_HEIGHT, true);
	MoveWindow(_hEdit, 0, rect.bottom - C_WND_EDIT_HEIGHT, rect.right, C_WND_EDIT_HEIGHT, true);
}

LRESULT CALLBACK CConsoleWindow::_s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CConsoleWindow *this_ptr = (CConsoleWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(message) 
	{
	case WM_MOVE:
		this_ptr->_iX = LOWORD(lParam);
		this_ptr->_iY = HIWORD(lParam);
		break;

	case WM_SHOWWINDOW:
		this_ptr->_bVisible = (wParam == TRUE);
		SetFocus(this_ptr->_hEdit);
		break;

	case WM_CLOSE:
		ShowWindow(this_ptr->_hWnd, SW_HIDE );
		break;

	case WM_SIZE:
		this_ptr->_iWidth = LOWORD(lParam);
		this_ptr->_iHeight = HIWORD(lParam);
		this_ptr->_Realign();
		break;

	case WM_DESTROY:
		DeleteObject(this_ptr->_hFont);
		break;

	case WM_GETMINMAXINFO:
		POINT pt;
		pt.x = C_WND_MIN_WIDTH;
		pt.y = C_WND_MIN_HEIGHT;
		((MINMAXINFO*)lParam)->ptMinTrackSize = pt;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;

}

LRESULT CALLBACK CConsoleWindow::_s_WndEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CConsoleWindow *this_ptr = (CConsoleWindow*)GetWindowLongPtr(GetParent(hWnd), GWLP_USERDATA);
	
	char tmp[_sc_uiTmpBufferSize];

	switch(message) 
	{
	case WM_KEYUP:
		switch(wParam)
		{
			case 192: //tilda
				this_ptr->Visible(false);
				SetWindowText(this_ptr->_hEdit, "");
				break;

			case 38: //up			
				this_ptr->_pConWindowEvent(this_ptr->_pConsole, CWE_PREVIOUS_COMMAND, "");
				break;

			case 40: //down				
				this_ptr->_pConWindowEvent(this_ptr->_pConsole, CWE_NEXT_COMMAND, "");
				break;
		}
		break;

	case WM_CHAR:
		if (wParam == 96 /*tilda*/)
			break;

		if (GetWindowTextLength(this_ptr->_hEdit) > 0)
		{
			if (wParam == 9 /*tab*/)
			{
				GetWindowText(this_ptr->_hEdit, tmp, _sc_uiTmpBufferSize);
				this_ptr->_pConWindowEvent(this_ptr->_pConsole, CWE_COMPLETE_COMMAND, tmp);

				break;
			}
			else
				if (wParam == 13 /*return*/)
				{
					GetWindowText(this_ptr->_hEdit, tmp, _sc_uiTmpBufferSize);
					SetWindowText(this_ptr->_hEdit, NULL);
				
					this_ptr->_pConWindowEvent(this_ptr->_pConsole, CWE_EXECUTE_COMMAND, tmp);
				
					break;
				}
		}
		goto callDefWndPros;

	case WM_KEYDOWN:
		if (wParam == 38 /*up*/ || wParam == 40 /*down*/)
			break;
		else
			goto callDefWndPros;

	default:
		goto callDefWndPros;
	}

	return 0;

callDefWndPros:
	return CallWindowProc((WNDPROC)this_ptr->_pOldEditProc, hWnd, message, wParam, lParam);
}

DWORD WINAPI CConsoleWindow::_s_ThreadProc(LPVOID lpParameter)
{
	CConsoleWindow *this_ptr = (CConsoleWindow*)lpParameter;

	if (this_ptr->_WinMain(GetModuleHandle(NULL)) != S_OK)
		return FALSE;

	MSG msg = {0};

	this_ptr->_bIsLooping = true;

	while (this_ptr->_bIsLooping)
		if (WaitMessage() && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				this_ptr->_bIsLooping = false;
			else
			{   
				TranslateMessage(&msg);
				DispatchMessage (&msg);
			}
		}

	if (FALSE != DestroyWindow(this_ptr->_hWnd))
		UnregisterClass("DGLEConsoleClass", this_ptr->_hInst);

	this_ptr->_hWnd = NULL;

	return 0;
}