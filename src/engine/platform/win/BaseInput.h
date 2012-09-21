/**
\author		Andrey Korotkov aka DRON
\date		17.09.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\..\Common.h"

#ifndef NO_BUILTIN_INPUT

//Use this define to exlude joysticks from build
//#define NO_JOYSTICKS

class CBaseInput : public CInstancedObj, public IBaseInput
{
	HCURSOR	_hCurNone;
	HWND	_hWnd;

#ifndef NO_JOYSTICKS

	struct TJoystick
	{
		uint id;
		JOYCAPS info;

		TJoystick(uint num, const JOYCAPS &caps) : id(num), info(caps) {}
	};

	std::vector<TJoystick> _clJoyCaps;

	uint _GetJoyIdx(uint id);
	uint32 _AxisValue(uint32 Value, uint Min, uint Max);

#endif

public:

	CBaseInput(uint uiInstIdx);
	~CBaseInput();

	void ShowCursor(bool bVisible);
	void GetCursorPos(int &x, int &y);
	void SetCursorPos(int x, int y);
	void ClipCursor(uint left, uint top, uint right, uint bottom);
	bool IsJoystickImplemented();
	uint JoysticksCount();
	std::string GetJoystickName(uint id);
	TJoystickStates GetJoystickStates(uint id);

};

#endif