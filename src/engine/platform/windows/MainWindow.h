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
	HINSTANCE _hInst;
	HWND _hWnd;
	HDC _hDC;
	TProcDelegate *_pDelMainLoop;
	TMsgProcDelegate *_pDelMessageProc;
	bool _bFScreen, _bIsLooping;

	int WINAPI _wWinMain(HINSTANCE hInstance);	
	static LRESULT DGLE_API _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static bool DGLE_API _s_ConsoleQuit(void *pParameter, const char *pcParam);

public:
	
	CMainWindow(uint uiInstIdx);
	~CMainWindow();

	DGLE_RESULT InitWindow(TWindowHandle tHandle, const TCrRndrInitResults &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc);
	DGLE_RESULT SendMessage(const TWindowMessage &stMsg);
	DGLE_RESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType);
	DGLE_RESULT GetWindowHandle(TWindowHandle &tHandle);
	DGLE_RESULT GetDrawContext(TWindowDrawHandle &tHandle);
	DGLE_RESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight);
	DGLE_RESULT ScreenToClient(int &iX, int &iY);
	DGLE_RESULT ConfigureWindow(const TEngineWindow &stWind, bool bSetFocus);
	DGLE_RESULT SetCaption(const char *pcTxt);
	DGLE_RESULT Minimize();
	DGLE_RESULT BeginMainLoop();
	DGLE_RESULT KillWindow();
	DGLE_RESULT Free();

	IDGLE_BASE_IMPLEMENTATION(IMainWindow, INTERFACE_IMPL_END)
};