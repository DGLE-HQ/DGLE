/**
\author		Andrey Korotkov aka DRON
\date		22.03.2016 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_INPUT

//Use this define to exlude joysticks from build
//#define NO_JOYSTICKS

class CBaseInput : public CInstancedObj, public CPlatformBaseInput
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

	std::vector<TJoystick> _vecJoyCaps;

	uint _GetJoyIdx(uint id);
	uint32 _AxisValue(uint32 Value, uint Min, uint Max);

	void _PrintJoysList();
	static bool DGLE_API _s_PrintJoysList(void *pParameter, const char *pcParam);

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