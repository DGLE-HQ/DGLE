/**
\author		Korotkov Andrey aka DRON
\date		23.02.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
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
	static LRESULT DGLE2_API _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void DGLE2_API	_s_ConsoleQuit(void *pParametr, const char *pcParam);

public:
	
	CMainWindow(uint uiInstIdx);
	~CMainWindow();

	DGLE2_RESULT InitWindow(TWinHandle tHandle, const TCRendererInitResult &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc);
	DGLE2_RESULT SendMessage(const TWinMessage &stMsg);
	DGLE2_RESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType);
	DGLE2_RESULT GetWindowHandle(TWinHandle &tHandle);
	DGLE2_RESULT GetDrawContext(TWinDrawHandle &tHandle);
	DGLE2_RESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight);
	DGLE2_RESULT ScreenToClient(int &iX, int &iY);
	DGLE2_RESULT ConfigureWindow(const TEngWindow &stWind, bool bSetFocus);
	DGLE2_RESULT SetCaption(const char *pcTxt);
	DGLE2_RESULT Minimize();
	DGLE2_RESULT BeginMainLoop();
	DGLE2_RESULT KillWindow();
	DGLE2_RESULT Free();

};