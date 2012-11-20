/**
\author		Korotkov Andrey aka DRON
\date		17.09.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_INPUT

#ifdef PLATFORM_WINDOWS
#	include "platform\windows\BaseInput.h"
#endif

class CInput: private CBaseInput, public IInput
{
	bool		 _bExclusive;
	bool		 _bHideCursor;
	bool		 _bFocused;
	bool		 _bCurBeyond;

	bool		 _bIsTxtInput;
	char		*_pcBuffer;
	std::string	 _clInputTxt;
	uint		 _uiBufSize;

	bool		 _abKeys[256];
	TMouseStates _stMsts;
	int			 _uiOldMouseX, _uiOldMouseY;

	void		 _ClipCursor();
	void		 _MessageProc(const TWinMessage &stMsg);
	void		 _Loop();
	void		 _Update();

	static void DGLE_API _s_MessageProc(void *pParametr, const TWinMessage &stMsg);
	static void DGLE_API _s_Update(void *pParametr);
	static void DGLE_API _s_Loop(void *pParametr);

public:

	CInput(uint uiInstIdx);
	~CInput();

	DGLE_RESULT DGLE_API Configure(E_INPUT_CONFIGURATION_FLAGS eParams);
	DGLE_RESULT DGLE_API GetMouseStates(TMouseStates &stMs);
	DGLE_RESULT DGLE_API GetKey(E_KEYBOARD_KEY_CODES eKey, bool &bPressed);
	DGLE_RESULT DGLE_API GetKeyName(E_KEYBOARD_KEY_CODES eKey, uchar &cAsciiCode);
	DGLE_RESULT DGLE_API BeginTextInput(char *cBuffer, uint uiBufferSize);
	DGLE_RESULT DGLE_API EndTextInput();
	DGLE_RESULT DGLE_API GetJoysticksCount(uint &uiCount);
	DGLE_RESULT DGLE_API GetJoystickName(uint uiJoyId, char *pcName, uint &uiCharsCount);
	DGLE_RESULT DGLE_API GetJoystickStates(uint uiJoyId, TJoystickStates &stJoyStates);
	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubsysType);

	IDGLE_BASE_IMPLEMENTATION(IInput, INTERFACE_IMPL(IEngineSubSystem, INTERFACE_IMPL_END))
};

#endif