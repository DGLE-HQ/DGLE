/**
\author		Korotkov Andrey aka DRON
\date		07.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\..\Common.h"

class CConsoleWindow : public IConsoleWindow
{
	static const uint _sc_uiTmpBufferSize = 256;
	static const uint _sc_uiMaxConsoleTxtLength = 30000;

	HINSTANCE _hInst;
	std::string	_strOnCreate;
	bool _bIsLooping;
	int	_iX, _iY, _iWidth, _iHeight;
	bool _bVisible;
	int	_iPrevLineSize;
	bool _bToPrevLineActive;
	HWND _hWnd, _hMemo, _hEdit;
	HFONT _hFont;
	CRITICAL_SECTION _cs;
	HANDLE _hThreadHandle;
	DWORD _threadId;
	void *_pOldEditProc;
	
	CConsole *_pConsole;

	void (DGLE_API *_pConWindowEvent)(CConsole *pConsole, E_CONSOLE_WINDOW_EVENT eEventType, const char *pcCommand); 

	void _Realign();
	int WINAPI _WinMain(HINSTANCE hInstance);	
	
	static LRESULT CALLBACK _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _s_WndEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI _s_ThreadProc(LPVOID lpParameter);

public:

	CConsoleWindow();

	DGLE_RESULT InitWindow(bool bSeparateThread, void (DGLE_API *pConWindowEvent)(CConsole *pConsole, E_CONSOLE_WINDOW_EVENT eEventType, const char *pcCommand), CConsole *pConsole);
	DGLE_RESULT Visible(bool bVisible);
	DGLE_RESULT SetSizeAndPos(int iX, int iY, int iWidth, int iHeight);
	DGLE_RESULT GetSizeAndPos(int &iX, int &iY, int &iWidth, int &iHeight);
	DGLE_RESULT GetWindowHandle(TWindowHandle &tHandle);
	DGLE_RESULT GetThreadId(uint32 &ui32Id);
	DGLE_RESULT OutputTxt(const char *pcTxt, bool bToPrevLine);
	DGLE_RESULT GetEditTxt(char *pcTxt, uint uiBufferSize);
	DGLE_RESULT SetEditTxt(const char *pcTxt);
	DGLE_RESULT GetConsoleTxt(char *pcTxt, uint &uiBufferSize);
	DGLE_RESULT Clear();
	DGLE_RESULT ResetSizeAndPos();
	DGLE_RESULT EnterThreadSafeSection();
	DGLE_RESULT LeaveThreadSafeSection();
	DGLE_RESULT Free();

	IDGLE_BASE_IMPLEMENTATION(IConsoleWindow, INTERFACE_IMPL_END)
};