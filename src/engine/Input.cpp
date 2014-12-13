/**
\author		Korotkov Andrey aka DRON
\date		25.09.2014 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Input.h"

using namespace std;

#ifndef NO_BUILTIN_INPUT

CInput::CInput(uint uiInstIdx):
CBaseInput(uiInstIdx), _bFocused(false),
_bIsTxtInput(false), _pcBuffer(NULL),
_bExclusive(false), _bHideCursor(false), _bCurBeyond(false)
{
	Core()->pDMessageProc()->Add(&_s_MessageProc, (void*)this);
	Core()->pDMLoopProc()->Add(&_s_Loop, (void*)this);
	Core()->AddProcedure(EPT_UPDATE, &_s_Update, (void*)this);

	memset(_abKeys, 0, 256 * sizeof(bool));
	memset(&_stMsts, 0, sizeof(TMouseStates));

	LOG("Input Subsystem initialized.", LT_INFO);
}

CInput::~CInput()
{
	Core()->pDMessageProc()->Remove(&_s_MessageProc, (void*)this);
	Core()->pDMLoopProc()->Remove(&_s_Loop, (void*)this);
	Core()->RemoveProcedure(EPT_UPDATE, &_s_Update, (void*)this);

	if (_bExclusive)
		ClipCursor(0, 0, 0, 0);

	LOG("Input Subsystem finalized.", LT_INFO);
}

void CInput::_Loop()
{
	if (_bExclusive && _bFocused && _bCurBeyond)
	{
		int x, y;
		GetCursorPos(x, y);

		int i_win_left, i_win_right, i_win_top, i_win_bottom;
		
		Core()->pMainWindow()->GetWinRect(i_win_left, i_win_top, i_win_right, i_win_bottom);
		i_win_right = i_win_left + i_win_right;
		i_win_bottom = i_win_top + i_win_bottom;

		bool flag = false;

		if (x >= i_win_right - 1)
		{
			flag = true;
			x = i_win_left + 1;
			_uiOldMouseX = -(i_win_right - i_win_left - _stMsts.iX);
		}

		if (x <= i_win_left)
		{
			flag = true;
			x = i_win_right - 2;
			_uiOldMouseX = i_win_right + _stMsts.iX - i_win_left;
		}

		if (y >= i_win_bottom - 1)
		{
			flag = true;				
			y = i_win_top + 1;
			_uiOldMouseY = -(i_win_bottom - i_win_top - _stMsts.iY);
		}

		if (y <= i_win_top)
		{		
			flag = true;
			y = i_win_bottom - 2;
			_uiOldMouseY = i_win_bottom + _stMsts.iY - i_win_top;
		}

		if (flag)
		{
			SetCursorPos(x, y);
			Core()->pMainWindow()->ScreenToClient(x, y);
			_stMsts.iX = x;
			_stMsts.iY = y;
		}
	}
}

void CInput::_Update()
{
	_stMsts.iDeltaX = _stMsts.iX - _uiOldMouseX;
	_stMsts.iDeltaY = _stMsts.iY - _uiOldMouseY;
	
	_uiOldMouseX = _stMsts.iX;
	_uiOldMouseY = _stMsts.iY;
	
	_stMsts.iDeltaWheel = _iDeltaWheel;
	_iDeltaWheel = 0;
}

void CInput::_ClipCursor()
{
	int x, y, w, h;
	Core()->pMainWindow()->GetWinRect(x, y, w, h);
	ClipCursor(x, y, x + w, y + h);
}

void CInput::_MessageProc(const TWindowMessage &stMsg)
{
	switch(stMsg.eMessage)
	{
		case WMT_CLOSE:
			
			if (_bHideCursor)
				ShowCursor(true);
			
			break;

		case WMT_DEACTIVATED:
			
			if (!_bFocused)
				break;
			
			memset(_abKeys, 0, 256 * sizeof(bool));
			_stMsts.bLeftButton = false;
			_stMsts.bRightButton = false;
			_stMsts.bMiddleButton = false;
			_bFocused = false;
			
			if (_bExclusive)
				ClipCursor(0, 0, 0, 0);
			
			break;

		case WMT_ACTIVATED:
			
			if (_bFocused)
				break;
			
			_bFocused = true;
			
		case WMT_SIZE:
		case WMT_PRESENT:

			if (_bExclusive)
				_ClipCursor();
			
			break;

		case WMT_KEY_DOWN:
			
			_abKeys[stMsg.ui32Param1] = true;
			
			break;

		case WMT_KEY_UP:
			
			_abKeys[stMsg.ui32Param1] = false;
			
			break;

		case WMT_MOUSE_LEAVE:

			_stMsts.bLeftButton = false;
			_stMsts.bRightButton = false;
			_stMsts.bMiddleButton = false;

			break;

		case WMT_MOUSE_MOVE:	
			
			_stMsts.iX = stMsg.ui32Param1;
			_stMsts.iY = stMsg.ui32Param2;
			
			break;
			
		case WMT_MOUSE_DOWN:
			
			if (stMsg.ui32Param1 == 0)
				_stMsts.bLeftButton = true;
			else
				if (stMsg.ui32Param1 == 1)
					_stMsts.bRightButton = true;
				else
					_stMsts.bMiddleButton = true;
			
			break;

		case WMT_MOUSE_UP:
			
			if (stMsg.ui32Param1 == 0)
				_stMsts.bLeftButton = false;
			else
				if (stMsg.ui32Param1 == 1)
					_stMsts.bRightButton = false;
				else
					_stMsts.bMiddleButton = false;
			
			break;

		case WMT_MOUSE_WHEEL: 
			
			_iDeltaWheel = (int)stMsg.pParam3;
			
			break;

		case WMT_INPUT_CHAR:
			
			if (_bIsTxtInput)
			{
				if (stMsg.ui32Param1 > 31)
				{
					if (_strInputTxt.size() + 1 < _uiBufSize)
						_strInputTxt += (char)stMsg.ui32Param1;
				}
				else
					if (stMsg.ui32Param1 == 8 /*Backspace*/ && _strInputTxt.length() > 0) 
						_strInputTxt.erase(_strInputTxt.length() - 1, 1);
				
				if (_uiBufSize > _strInputTxt.size())
					strcpy(_pcBuffer, _strInputTxt.c_str());
			}
			
			break;
	}
}

DGLE_RESULT DGLE_API CInput::Configure(E_INPUT_CONFIGURATION_FLAGS eParams)
{
	_bExclusive	= (eParams & ICF_EXCLUSIVE) != 0;

	_bCurBeyond = (eParams & ICF_CURSOR_BEYOND_SCREEN) != 0;

	_bHideCursor = (eParams & ICF_HIDE_CURSOR) != 0;

	if (_bFocused)
	{
		if (_bExclusive)
			_ClipCursor();
		else
			ClipCursor(0, 0, 0, 0);
	}

	ShowCursor(!_bHideCursor);

	return _bCurBeyond && !_bExclusive ? S_FALSE : S_OK;
}

DGLE_RESULT DGLE_API CInput::GetMouseStates(TMouseStates &stMs)
{
	stMs = _stMsts;
	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetKey(E_KEYBOARD_KEY_CODES eKey, bool &bPressed)
{
	bPressed = _abKeys[eKey];
	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetKeyName(E_KEYBOARD_KEY_CODES eKey, uchar &cAsciiCode)
{
	cAsciiCode = EngKeyToASCIIKey(eKey);
	return S_OK;
}

DGLE_RESULT DGLE_API CInput::BeginTextInput(char *cBuffer, uint uiBufferSize)
{
	_pcBuffer = cBuffer;
	_uiBufSize = uiBufferSize;
	_bIsTxtInput = true;
	_strInputTxt = "";
	strcpy(_pcBuffer, "");
	
	return S_OK;
}

DGLE_RESULT DGLE_API CInput::EndTextInput()
{
	_bIsTxtInput = false;
	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetJoysticksCount(uint &uiCount)
{
	if (!IsJoystickImplemented())
	{
		uiCount = 0;
		LOG("Joystick support is not implimented.", LT_WARNING);
		return E_NOTIMPL;
	}
	else
		uiCount = JoysticksCount();

	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetJoystickName(uint uiJoyId, char *pcName, uint &uiCharsCount)
{
	if (pcName)
		strcpy(pcName, "");
	else
		uiCharsCount = 0;

	if (!IsJoystickImplemented())
	{
		LOG("Joystick support is not implimented.", LT_WARNING);
		return E_NOTIMPL;
	}
	else
	{
		string name = CBaseInput::GetJoystickName(uiJoyId);

		if (pcName == NULL)
		{
			uiCharsCount = name.size();
			return S_OK;
		}

		if (uiCharsCount < name.size())
		{
			LOG("Too small \"pcName\" buffer size.", LT_ERROR);
			return E_INVALIDARG;
		}

		strcpy(pcName, name.c_str());
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetJoystickStates(uint uiJoyId, TJoystickStates &stJoyStates)
{
	if (!IsJoystickImplemented())
	{
		LOG("Joystick support is not implimented.", LT_WARNING);
		return E_NOTIMPL;
	}
	else
		stJoyStates = CBaseInput::GetJoystickStates(uiJoyId);

	return S_OK;
}

DGLE_RESULT DGLE_API CInput::GetType(E_ENGINE_SUB_SYSTEM &eSubsysType)
{
	eSubsysType = ESS_INPUT;
	return S_OK;
}

void DGLE_API CInput::_s_MessageProc(void *pParameter, const TWindowMessage &stMsg)
{
	PTHIS(CInput)->_MessageProc(stMsg);
}

void DGLE_API CInput::_s_Loop(void *pParameter)
{
	PTHIS(CInput)->_Loop();
}

void DGLE_API CInput::_s_Update(void *pParameter)
{
	PTHIS(CInput)->_Update();
}

#endif