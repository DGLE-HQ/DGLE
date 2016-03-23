/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CDummyWindow : public CInstancedObj, public IMainWindow
{
	HWND _hWnd;
	HDC _hDC;
	TMsgProcDelegate *_pDelMessageProc;

public:
	
	CDummyWindow(uint uiInstIdx);
	~CDummyWindow();

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