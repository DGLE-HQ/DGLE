/**
\author		Korotkov Andrey aka DRON
\date		20.11.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CDummyWindow : public CInstancedObj, public IMainWindow
{
	HWND _hWnd;
	HDC _hDC;
	TMsgProcDelegate *_pDelMessageProc;

public:
	
	CDummyWindow(uint uiInstIdx);
	~CDummyWindow();

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