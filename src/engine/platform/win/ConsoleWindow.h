/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _CONSOLEWINDOW_H
#define _CONSOLEWINDOW_H

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
	void (CALLBACK *_pOnCmdExec)(CConsole *pConsole, const char *pcCommand); 
	void (CALLBACK *_pOnCmdComplete)(CConsole *pConsole, const char *pcCommand); 

	void _Realign();
	int WINAPI _WinMain(HINSTANCE hInstance);	
	
	static LRESULT CALLBACK _s_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _s_WndEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI		_s_ThreadProc(LPVOID lpParameter);

public:

	CConsoleWindow();

	HRESULT InitWindow(bool bSeparateThread, void (CALLBACK *pOnCmdExec)(CConsole *pConsole, const char *pcCommand), void (CALLBACK *pOnCmdComplete)(CConsole *pConsole, const char *pcCommand), CConsole *pConsole);
	HRESULT Visible(bool bVisible);
	HRESULT SetSizeAndPos(int iX, int iY, int iWidth, int iHeight);
	HRESULT GetSizeAndPos(int &iX, int &iY, int &iWidth, int &iHeight);
	HRESULT OutputTxt(const char *pcTxt, bool bToPrevLine);
	HRESULT GetEditTxt(char *pcTxt, uint uiBufferSize);
	HRESULT SetEditTxt(const char *pcTxt);
	HRESULT Clear();
	HRESULT ResetSizeAndPos();
	HRESULT EnterThreadSafeSec();
	HRESULT LeaveThreadSafeSec();
	HRESULT Free();

};

#endif //_CONSOLEWINDOW_H