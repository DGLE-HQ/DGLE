/**
\author		Korotkov Andrey aka DRON
\date		20.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CSplashWindow final : public CInstancedObj, public ISplashWindow
{
	bool _bInSeparateThread;
	HWND _hOwnerWndHwnd = {}, _hWnd = {};
	HBITMAP _hBmp = {};
	char *_pcBmpFile;

	bool _CreateWindow();
	void _DestroyWindow();
	
	static DWORD WINAPI _s_ThreadProc(LPVOID lpParameter);

public:

	using CInstancedObj::CInstancedObj;

	DGLE_RESULT InitWindow(const char *pcBmpFileName) override;
	DGLE_RESULT SetOwnerWindow(TWindowHandle tOwnerHwnd) override;
	DGLE_RESULT Free() override;

	IDGLE_BASE_IMPLEMENTATION(ISplashWindow, INTERFACE_IMPL_END)
};