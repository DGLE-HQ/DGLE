/**
\author		Korotkov Andrey aka DRON
\date		25.02.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CHookedWindow : public CInstancedObj, public IMainWindow
{
	HWND			 _hWnd,
					 _hRootHWnd;
	HDC				 _hDC;

	bool			 _bNoMloopHook;
	
	TMsgProcDelegate
					*_pDelMessageProc;
	TProcDelegate	*_pDelMainLoop;

	WNDPROC			 _stOldWindowProc,
					 _stOldRootWindowProc;
	
	uint			 _uiUpdateTimer;

	void _KillWindow();
	static void DGLE2_API	_s_ConsoleQuit(void *pParametr, const char *pcParam);
	static LRESULT CALLBACK _s_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _s_RootWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	CHookedWindow(uint uiInstIdx);

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