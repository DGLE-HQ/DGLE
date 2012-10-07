/**
\author		Korotkov Andrey aka DRON
\date		09.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CSplashWindow : public CInstancedObj, public ISplashWindow
{
	bool		_bInSeparateThread;
	HWND		_hOwnerWndHwnd;
	HWND		_hWnd;
	HBITMAP		_hBmp;
	char		*_pcBmpFile;

	bool		 _CreateWindow();
	void		 _DestroyWindow();
	
	static DWORD WINAPI _s_ThreadProc(LPVOID lpParameter);

public:

	CSplashWindow(uint uiInstIdx);

	DGLE2_RESULT InitWindow(bool bSeparateThread, const char *pcBmpFileName);
	DGLE2_RESULT SetOwnerWindow(TWinHandle tOwnerHwnd);
	DGLE2_RESULT Free();

};