/**
\author		Korotkov Andrey aka DRON
\date		23.02.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CMainWindow : public CInstancedObj, public IMainWindow
{
	HINSTANCE			_hInst;
	HWND				_hWnd;
	HDC					_hDC;
	TProcDelegate		*_pDelMainLoop;
	TMsgProcDelegate	*_pDelMessageProc;
	bool				_bFScreen;
	bool				_bIsLooping;

	int WINAPI				_wWinMain(HINSTANCE hInstance);	
	static LRESULT DGLE_API _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void DGLE_API	_s_ConsoleQuit(void *pParametr, const char *pcParam);

public:
	
	CMainWindow(uint uiInstIdx);
	~CMainWindow();

	DGLE_RESULT InitWindow(TWinHandle tHandle, const TCRendererInitResult &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc);
	DGLE_RESULT SendMessage(const TWinMessage &stMsg);
	DGLE_RESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType);
	DGLE_RESULT GetWindowHandle(TWinHandle &tHandle);
	DGLE_RESULT GetDrawContext(TWinDrawHandle &tHandle);
	DGLE_RESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight);
	DGLE_RESULT ScreenToClient(int &iX, int &iY);
	DGLE_RESULT ConfigureWindow(const TEngWindow &stWind, bool bSetFocus);
	DGLE_RESULT SetCaption(const char *pcTxt);
	DGLE_RESULT Minimize();
	DGLE_RESULT BeginMainLoop();
	DGLE_RESULT KillWindow();
	DGLE_RESULT Free();

};