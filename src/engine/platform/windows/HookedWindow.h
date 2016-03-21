/**
\author		Korotkov Andrey aka DRON
\date		22.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CHookedWindow : public CInstancedObj, public IMainWindow
{
	HWND _hWnd, _hRootHWnd;
	HDC _hDC;
	bool _bNoMloopHook;
	TMsgProcDelegate *_pDelMessageProc;
	TProcDelegate *_pDelMainLoop;
	WNDPROC _stOldWindowProc, _stOldRootWindowProc;
	uint _uiUpdateTimer;

	void _KillWindow();
	static bool DGLE_API _s_ConsoleQuit(void *pParameter, const char *pcParam);
	static LRESULT CALLBACK _s_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _s_RootWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	CHookedWindow(uint uiInstIdx);

	DGLE_RESULT InitWindow(TWindowHandle tHandle, const TCrRndrInitResults &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc);
	DGLE_RESULT SendMessage(const TWindowMessage &stMsg);
	DGLE_RESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType);
	DGLE_RESULT GetWindowHandle(TWindowHandle &tHandle);
	DGLE_RESULT GetDrawContext(TWindowDrawHandle &tHandle);
	DGLE_RESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight);
	DGLE_RESULT ScreenToClient(int &iX, int &iY);
	DGLE_RESULT ConfigureWindow(const TEngineWindow &stWind, bool bSetFocus);
	DGLE_RESULT ExitFullScreen();
	DGLE_RESULT SetCaption(const char *pcTxt);
	DGLE_RESULT Minimize();
	DGLE_RESULT BeginMainLoop();
	DGLE_RESULT KillWindow();
	DGLE_RESULT Free();

	IDGLE_BASE_IMPLEMENTATION(IMainWindow, INTERFACE_IMPL_END)
};