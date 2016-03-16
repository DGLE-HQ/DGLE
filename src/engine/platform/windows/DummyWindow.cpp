/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "DummyWindow.h"

CDummyWindow::CDummyWindow(uint uiInstIdx):
CInstancedObj(uiInstIdx), _hWnd(NULL), _hDC(NULL)
{}

CDummyWindow::~CDummyWindow()
{
	LOG("Window closed properly.", LT_INFO);	
}

DGLE_RESULT CDummyWindow::InitWindow(TWindowHandle tHandle, const TCrRndrInitResults &stRndrInitResults, TProcDelegate *pDelMainLoop, TMsgProcDelegate *pDelMsgProc)
{
	_pDelMessageProc = pDelMsgProc;

	_hWnd = CreateWindowEx(0, "STATIC", NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL);

	if (!_hWnd)
	{
		_hWnd = NULL;
		LOG("Failed to create window.", LT_FATAL);
		return E_FAIL;
	}

	if (!(_hDC = GetDC(_hWnd)))
	{
		LOG("Can't get window Draw Context.", LT_FATAL);
		return E_FAIL;
	}

	LOG("Window created successfully.", LT_INFO);

	return S_OK;
}

DGLE_RESULT CDummyWindow::SendMessage(const TWindowMessage &stMsg)
{
	return E_NOTIMPL;
}

DGLE_RESULT CDummyWindow::GetWindowAccessType(E_WINDOW_ACCESS_TYPE &eType)
{
	eType = WAT_FULL_ACCESS;

	return S_OK;
}

DGLE_RESULT CDummyWindow::GetWindowHandle(TWindowHandle &stHandle)
{
	stHandle = _hWnd;

	return S_OK;
}

DGLE_RESULT CDummyWindow::GetDrawContext(HDC &hDC)
{
	if (!_hDC)
		return E_FAIL;

	hDC = _hDC;

	return S_OK;
}

DGLE_RESULT CDummyWindow::GetWinRect(int &iX, int &iY, int &iWidth, int &iHeight)
{
	if (!_hWnd)
		return E_FAIL;

	iX = iY = iWidth = iHeight = 0;

	return S_OK;
}

DGLE_RESULT CDummyWindow::ScreenToClient(int &iX, int &iY)
{
	if (!_hWnd)
		return E_FAIL;

	iX = iY = 0;

	return E_NOTIMPL;
}

DGLE_RESULT CDummyWindow::SetCaption(const char *pcTxt)
{
	return E_NOTIMPL;
}

DGLE_RESULT CDummyWindow::Minimize()
{
	return E_NOTIMPL;
}

DGLE_RESULT CDummyWindow::BeginMainLoop()
{
	return S_FALSE;
}

DGLE_RESULT CDummyWindow::KillWindow()
{
	_pDelMessageProc->Invoke(TWindowMessage(WMT_DESTROY));
	_pDelMessageProc->Invoke(TWindowMessage(WMT_RELEASED));

	return S_OK;
}

DGLE_RESULT CDummyWindow::ConfigureWindow(const TEngineWindow &stWind, bool bSetFocus)
{
	if (!_hWnd)
		return E_FAIL;

	return S_FALSE;
}

DGLE_RESULT CDummyWindow::ExitFullScreen()
{
	return E_NOTIMPL;
}

DGLE_RESULT CDummyWindow::Free()
{
	if (_hDC && ReleaseDC(_hWnd,_hDC) == FALSE)
		LOG("Failed to release Device Context.", LT_ERROR);

	if (DestroyWindow(_hWnd) == FALSE)
	{
		LOG("Can't destroy window.",LT_ERROR);
		return S_FALSE;
	}
	
	delete this;
	
	return S_OK;
}