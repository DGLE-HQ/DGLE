/**
\author		Andrey Korotkov aka DRON
\date		19.04.2016 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "BaseInput.h"

using namespace std;

#ifndef NO_BUILTIN_INPUT

#ifndef NO_JOYSTICKS
#	pragma message("Linking with \"Winmm.lib\".")
#	pragma comment(linker, "/defaultlib:Winmm.lib")
#endif

CBaseInput::CBaseInput(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{
	Core()->pMainWindow()->GetWindowHandle(_hWnd);

	BYTE ANDmaskIcon[128], XORmaskIcon[128]; 

	memset(ANDmaskIcon, 0xFF, 128*sizeof(BYTE));
	memset(XORmaskIcon, 0x00, 128*sizeof(BYTE));

	_hCurNone = CreateCursor(GetModuleHandle(NULL), 1, 1, 32, 32, ANDmaskIcon, XORmaskIcon);

	if (!_hCurNone)
		LOG("Can't create blank cursor.", LT_ERROR);

#ifndef NO_JOYSTICKS

	const uint count = joyGetNumDevs();

	for (uint i = 0; i < count; ++i)
	{
		JOYCAPS caps{};
		
		joyGetDevCaps(i, &caps, sizeof(JOYCAPS));

		JOYINFO info;

		if (strlen(caps.szPname) > 0 && joyGetPos(i, &info) != JOYERR_UNPLUGGED)
			_vecJoyCaps.push_back(TJoystick(i, caps));
	}

	if (!_vecJoyCaps.empty())
	{
		LOG("Found " + to_string(_vecJoyCaps.size()) + " connected joystick" + (_vecJoyCaps.size() == 1 ? "." : "s."), LT_INFO);

		for (size_t i = 0; i < _vecJoyCaps.size(); ++i)
			LOG("Joystick with id: " + to_string(i) + " name: \"" + _vecJoyCaps[i].info.szPname + '\"', LT_INFO);
	}

	Console()->RegComProc("input_list_joys", "Prints the list of the connected joysticks.", &_s_PrintJoysList, this);

#endif
}

CBaseInput::~CBaseInput()
{
#ifndef NO_JOYSTICKS
	Console()->UnRegCom("input_list_joys");
#endif

	if (_hCurNone && 0 == DestroyCursor(_hCurNone))
		LOG("Can't destroy blank cursor.", LT_WARNING);
}

void CBaseInput::ShowCursor(bool bVisible)
{
	if (!_hCurNone)
		return;

	if (!bVisible)
		SetClassLongPtr(_hWnd, GCLP_HCURSOR, (LONG)_hCurNone);   
	else
	{
		SetClassLongPtr(_hWnd, GCLP_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));
		SetCursor(LoadCursor(NULL, IDC_ARROW)); // little hack to force cursor update
	}
}

void CBaseInput::GetCursorPos(int &x, int &y)
{
	POINT p;
	::GetCursorPos(&p);
	x = p.x; y = p.y;
}

void CBaseInput::SetCursorPos(int x, int y)
{
	::SetCursorPos(x, y);
}

void CBaseInput::ClipCursor(uint left, uint top, uint right, uint bottom)
{
	if (left == 0 && top == 0 && right == 0 && bottom == 0)
		::ClipCursor(NULL);
	else
	{
		RECT rect;

		rect.left = left;
		rect.right = right;
		rect.top = top;
		rect.bottom = bottom;

		::ClipCursor(&rect);
	}
}

#ifndef NO_JOYSTICKS

uint CBaseInput::_GetJoyIdx(uint id)
{
	for (uint i = 0; i < _vecJoyCaps.size(); ++i)
		if (_vecJoyCaps[i].id ==id)
			return i;
	
	return -1;
}

uint32 CBaseInput::_AxisValue(uint32 Value, uint Min, uint Max)
{
	if (Max - Min != 0)
		return (int)((float)(Value + Min) / (float)(Max - Min) * 200.f - 100.f);
	else
		return 0;
}

void CBaseInput::_PrintJoysList()
{
	string output("There is no connected joysticks.");

	if (!_vecJoyCaps.empty())
	{
		output = "----------- Joysticks -----------\n";

		for (size_t i = 0; i < _vecJoyCaps.size(); ++i)
			output += "id: " + to_string(i) + " name: \"" + _vecJoyCaps[i].info.szPname + "\"\n";

		output += "---------------------------------\n";
	}
		
	Console()->Write(output.c_str());
}

bool DGLE_API CBaseInput::_s_PrintJoysList(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CBaseInput, "No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CBaseInput)->_PrintJoysList();
		return true;
	}
}

#endif

bool CBaseInput::IsJoystickImplemented()
{
#ifdef NO_JOYSTICKS

	return false;

#else

	return true;

#endif
}

uint CBaseInput::JoysticksCount()
{
#ifdef NO_JOYSTICKS

	return 0;

#else

	return _vecJoyCaps.size();

#endif
}

string CBaseInput::GetJoystickName(uint id)
{
#ifndef NO_JOYSTICKS

	uint idx = _GetJoyIdx(id);

	if (idx == -1)
		return string();
	else
		return _vecJoyCaps[idx].info.szPname;

#else

	return string();

#endif
}

TJoystickStates CBaseInput::GetJoystickStates(uint id)
{
	TJoystickStates states;

	memset(&states, 0, sizeof(TJoystickStates));

#ifndef NO_JOYSTICKS

	uint idx = _GetJoyIdx(id);

	if (idx != -1)
	{
		const TJoystick &joy = _vecJoyCaps[idx];
		
		JOYINFOEX info;
		memset(&info, 0, sizeof(JOYINFOEX));
			
		info.dwSize = sizeof(JOYINFOEX);
		info.dwFlags = JOY_RETURNALL;
			
		if ((joy.info.wCaps & JOYCAPS_POVCTS) > 0)
			info.dwFlags |=  JOY_RETURNPOVCTS;
				
		if (joyGetPosEx(joy.id, &info) == JOYERR_NOERROR)
		{
			states.iXAxis = _AxisValue(info.dwXpos, joy.info.wXmin, joy.info.wXmax);
			states.iYAxis = _AxisValue(info.dwYpos, joy.info.wYmin, joy.info.wYmax);
			
			if ((joy.info.wCaps & JOYCAPS_HASZ) > 0)
				states.iZAxis = _AxisValue(info.dwZpos, joy.info.wZmin, joy.info.wZmax);
			
			if ((joy.info.wCaps & JOYCAPS_HASR) > 0)
				states.iRAxis = _AxisValue(info.dwRpos, joy.info.wRmin, joy.info.wRmax);
			
			if ((joy.info.wCaps & JOYCAPS_HASU) > 0)
				states.iUAxis = _AxisValue(info.dwUpos, joy.info.wUmin, joy.info.wUmax);
			
			if ((joy.info.wCaps & JOYCAPS_HASV) > 0)
				states.iVAxis = _AxisValue(info.dwVpos, joy.info.wVmin, joy.info.wVmax);

			if ((joy.info.wCaps & JOYCAPS_HASPOV) > 0 && (info.dwPOV & 0xFFFF) != 0xFFFF)
				states.iPOV = (int)((float)(info.dwPOV & 0xFFFF) / 100.f);

			states.uiButtonsCount = joy.info.wNumButtons;

			for (uint i = 0; i < joy.info.wNumButtons; ++i)
				states.bButtons[i] = (info.dwButtons & (1 << i)) != 0;
		}
	}

#endif

	return states;
}

#endif