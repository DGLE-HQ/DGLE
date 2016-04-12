/**
\author		Korotkov Andrey aka DRON
\date		12.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "SplashWindow.h"
#include "..\..\..\..\build\windows\engine\resource.h"

using namespace std;

extern HMODULE hModule;

CSplashWindow::CSplashWindow(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_hWnd(), _hOwnerWndHwnd(), _hBmp()
{}

DGLE_RESULT CSplashWindow::InitWindow(const char *pcBmpFileName)
{
	_bInSeparateThread = !(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_SINGLE_THREAD);

	_pcBmpFile = new char [strlen(pcBmpFileName) + 1];
	strcpy(_pcBmpFile, pcBmpFileName);

	HANDLE thread_handle = NULL;

	if (_bInSeparateThread)
	{
		thread thread(&CSplashWindow::_ThreadProc, this);
		thread_handle = thread.native_handle();
		thread.detach();
	}

	return thread_handle || _CreateWindow() ? S_OK : E_ABORT;
}

DGLE_RESULT CSplashWindow::Free()
{
	delete[] _pcBmpFile;

	ShowWindow(_hWnd, SW_HIDE);

	if (_hOwnerWndHwnd)
		SetForegroundWindow(_hOwnerWndHwnd);

	if (_bInSeparateThread)
		PostMessage(_hWnd, WM_EXIT, 0, 0);
	else
		_DestroyWindow();

	return S_OK;
}

DGLE_RESULT CSplashWindow::SetOwnerWindow(TWindowHandle tOwnerHwnd)
{
	_hOwnerWndHwnd = tOwnerHwnd;
	return S_OK;
}

bool CSplashWindow::_CreateWindow()
{
	_hWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "STATIC", "", WS_POPUP | SS_BITMAP, 0, 0, 0, 0, NULL, NULL, hModule, NULL);

	if (!_hWnd)
	{
		LOG("Can't create splash window.", LT_ERROR);
		return false;
	}

	if (strlen(_pcBmpFile) > 0)
	{
		HBITMAP bmp = (HBITMAP)LoadImage(hModule, _pcBmpFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		
		if (!bmp)
			LOG("Can't load custom splash bitmap \""s + _pcBmpFile + "\".", LT_ERROR);
		else
		{
			_hBmp = bmp;
			bmp = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP2));

			BITMAP bm1, bm2; 
			GetObject(_hBmp, sizeof(BITMAP), &bm1);
			GetObject(bmp, sizeof(BITMAP), &bm2);

			if (bm1.bmWidth < bm2.bmWidth || bm1.bmHeight < bm2.bmHeight)
			{
				LOG("Custom splash picture must be greater than " + to_string(bm2.bmWidth) + 'X' + to_string(bm2.bmHeight) + " pixels.", LT_ERROR);
				
				DeleteObject(_hBmp);
				DeleteObject(bmp);

				_hBmp = NULL;
			}
			else
			{
				const HDC desktop_dc	= GetDC(GetDesktopWindow()), hdc1 = CreateCompatibleDC(desktop_dc), hdc2 = CreateCompatibleDC(desktop_dc);

				SelectObject(hdc1, _hBmp);
				SelectObject(hdc2, bmp);
				BitBlt(hdc1, bm1.bmWidth - bm2.bmWidth, bm1.bmHeight - bm2.bmHeight, bm2.bmWidth, bm2.bmHeight, hdc2, 0, 0, SRCCOPY);

				DeleteDC(hdc2);
				DeleteObject(bmp);
				
				const HBITMAP tmp = CreateCompatibleBitmap(desktop_dc, bm1.bmWidth, bm1.bmHeight);
				const HDC hdc3 = CreateCompatibleDC(desktop_dc);
				const HGDIOBJ old = SelectObject(hdc3, tmp);

				BitBlt(hdc3, 0, 0, bm1.bmWidth, bm1.bmHeight, hdc1, 0, 0, SRCCOPY);

				DeleteDC(hdc1);
				DeleteObject(_hBmp);

				_hBmp = (HBITMAP)GetCurrentObject(hdc3, OBJ_BITMAP);

				SelectObject(hdc3, old);

				DeleteDC(hdc3);
				ReleaseDC(GetDesktopWindow(), desktop_dc);
			}
		}
	}

	if (!_hBmp)
		_hBmp = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP1));


	if (!_hBmp)
	{
		LOG("Can't load splash bitmap from resource.", LT_ERROR);
		return false;
	}

	SendMessage(_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)_hBmp);
	
	RECT st_rect;
	GetClientRect(_hWnd, &st_rect);
	
	uint w, h;
	GetDisplaySize(w, h);

	MoveWindow(_hWnd, (w - (st_rect.right - st_rect.left))/2, (h - (st_rect.bottom - st_rect.top))/2, st_rect.right - st_rect.left,st_rect.bottom - st_rect.top, false);
	
	if (AnimateWindow(_hWnd, 750, AW_BLEND) == 0)
		ShowWindow(_hWnd, SW_SHOWNORMAL);
	
	UpdateWindow(_hWnd);

	LOG("Splash window created.", LT_INFO);
	
	return true;
}

void CSplashWindow::_DestroyWindow()
{
	if (!DestroyWindow(_hWnd))
		LOG("Can't destroy splash window.", LT_ERROR);
	
	if (!DeleteObject(_hBmp))
		LOG("Can't release splash window bitmap.", LT_ERROR);
 
	LOG("Splash window destroyed.", LT_INFO);

	delete this;
}

void CSplashWindow::_ThreadProc()
{
	if (!_CreateWindow())
		return;

	MSG st_msg = {0};

	while (true)
	{
		if (PeekMessage(&st_msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (st_msg.message)
			{
			case WM_EXIT:
				PostQuitMessage(st_msg.wParam);
				break;
			case WM_QUIT:
				_DestroyWindow();
				return;
			default:
				TranslateMessage(&st_msg);
				DispatchMessage(&st_msg);
				break;
			}
		}
		
	}
}