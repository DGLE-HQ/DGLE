/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\..\Common.h"

class CConsoleWindow : public IConsoleWindow
{

	HINSTANCE			_hInst;
	std::string			_strOnCreate;
	bool				_bIsLooping;
	int					_iX, _iY, _iWidth, _iHeight;
	bool				_bVisible;
	int					_iPrevLineSize;
	bool				_bToPrevLineActive;
	HWND				_hWnd,
						_hMemo,
						_hEdit;
	HFONT				_hFont;
	CRITICAL_SECTION	_cs;
	HANDLE				_hThreadHandle;
	DWORD				_threadId;
	void			   *_pOldEditProc;
	std::string			_strLastEditTxt;
	
	CConsole *_pConsole;
	void (DGLE2_API *_pOnCmdExec)(CConsole *pConsole, const char *pcCommand); 
	void (DGLE2_API *_pOnCmdComplete)(CConsole *pConsole, const char *pcCommand); 

	void _Realign();
	int WINAPI _WinMain(HINSTANCE hInstance);	
	
	static LRESULT DGLE2_API _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT DGLE2_API _s_WndEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI		_s_ThreadProc(LPVOID lpParameter);

public:

	CConsoleWindow();

	HRESULT InitWindow(bool bSeparateThread, void (DGLE2_API *pOnCmdExec)(CConsole *pConsole, const char *pcCommand), void (DGLE2_API *pOnCmdComplete)(CConsole *pConsole, const char *pcCommand), CConsole *pConsole);
	HRESULT Visible(bool bVisible);
	HRESULT SetSizeAndPos(int iX, int iY, int iWidth, int iHeight);
	HRESULT GetSizeAndPos(int &iX, int &iY, int &iWidth, int &iHeight);
	HRESULT GetWindowHandle(TWinHandle &tHandle);
	HRESULT GetThreadId(uint32 &ui32Id);
	HRESULT OutputTxt(const char *pcTxt, bool bToPrevLine);
	HRESULT GetEditTxt(char *pcTxt, uint uiBufferSize);
	HRESULT SetEditTxt(const char *pcTxt);
	HRESULT Clear();
	HRESULT ResetSizeAndPos();
	HRESULT EnterThreadSafeSec();
	HRESULT LeaveThreadSafeSec();
	HRESULT Free();

};