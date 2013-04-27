/**
\author		Korotkov Andrey aka DRON
\date		09.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CSplashWindow : public CInstancedObj, public ISplashWindow
{
	bool _bInSeparateThread;
	HWND _hOwnerWndHwnd, _hWnd;
	HBITMAP _hBmp;
	char *_pcBmpFile;

	bool _CreateWindow();
	void _DestroyWindow();
	
	static DWORD WINAPI _s_ThreadProc(LPVOID lpParameter);

public:

	CSplashWindow(uint uiInstIdx);

	DGLE_RESULT InitWindow(const char *pcBmpFileName);
	DGLE_RESULT SetOwnerWindow(TWindowHandle tOwnerHwnd);
	DGLE_RESULT Free();

	IDGLE_BASE_IMPLEMENTATION(ISplashWindow, INTERFACE_IMPL_END)
};