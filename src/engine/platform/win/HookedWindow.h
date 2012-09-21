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
	TWinHandle		 _tWnd,
					 _tRootHWnd;
	TWinDrawHandle	 _tDC;

	bool			 _bNoMloopHook;
	
	TMsgProcDelegate
					*_pDelMessageProc;
	TProcDelegate	*_pDelMainLoop;

	WNDPROC			 _stOldWindowProc,
					 _stOldRootWindowProc;
	
	uint			 _uiUpdateTimer;

	void _KillWindow();
	static void DGLE2_API	_s_ConsoleQuit(void *pParametr, const char *pcParam);
	static LRESULT DGLE2_API _s_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT DGLE2_API _s_RootWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	CHookedWindow(uint uiInstIdx);

	HRESULT InitWindow(TWinHandle tHandle, const TCRendererInitResult &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc);
	HRESULT SendMessage(const TWinMessage &stMsg);
	HRESULT GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType);
	HRESULT GetWindowHandle(TWinHandle &tHandle);
	HRESULT GetDrawContext(TWinDrawHandle &tHandle);
	HRESULT GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight);
	HRESULT ScreenToClient(int &iX, int &iY);
	HRESULT ConfigureWindow(const TEngWindow &stWind, bool bSetFocus);
	HRESULT SetCaption(const char *pcTxt);
	HRESULT BeginMainLoop();
	HRESULT KillWindow();
	HRESULT Free();

};