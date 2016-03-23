/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CHookedWindow final : public CInstancedObj, public IMainWindow
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

	DGLE_RESULT InitWindow(TWindowHandle tHandle, const TCrRndrInitResults &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc) override;
	DGLE_RESULT SendMessage(const TWindowMessage &stMsg) override;
	DGLE_RESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType) override;
	DGLE_RESULT GetWindowHandle(TWindowHandle &tHandle) override;
	DGLE_RESULT GetDrawContext(TWindowDrawHandle &tHandle) override;
	DGLE_RESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight) override;
	DGLE_RESULT ScreenToClient(int &iX, int &iY) override;
	DGLE_RESULT ConfigureWindow(const TEngineWindow &stWind, bool bSetFocus) override;
	DGLE_RESULT ExitFullScreen() override;
	DGLE_RESULT SetCaption(const char *pcTxt) override;
	DGLE_RESULT Minimize() override;
	DGLE_RESULT BeginMainLoop() override;
	DGLE_RESULT KillWindow() override;
	DGLE_RESULT Free() override;

	IDGLE_BASE_IMPLEMENTATION(IMainWindow, INTERFACE_IMPL_END)
};