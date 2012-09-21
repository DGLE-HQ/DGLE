/**
\author		Korotkov Andrey aka DRON
\date		17.09.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_INPUT

#ifdef PLATFORM_WINDOWS
#	include "platform\win\BaseInput.h"
#endif

class CInput: public CBaseInput, public IInput
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

	static void DGLE2_API _s_MessageProc(void *pParametr, const TWinMessage &stMsg);
	static void DGLE2_API _s_Update(void *pParametr);
	static void DGLE2_API _s_Loop(void *pParametr);

public:

	CInput(uint uiInstIdx);
	~CInput();

	HRESULT DGLE2_API Configure(E_INPUT_CONFIGURATION_FLAGS eParams);
	HRESULT DGLE2_API GetMouseStates(TMouseStates &stMs);
	HRESULT DGLE2_API GetKey(E_KEYBOARD_KEY_CODES eKey, bool &bPressed);
	HRESULT DGLE2_API GetKeyName(E_KEYBOARD_KEY_CODES eKey, uchar &cAsciiCode);
	HRESULT DGLE2_API BeginTextInput(char *cBuffer, uint uiBufferSize);
	HRESULT DGLE2_API EndTextInput();
	HRESULT DGLE2_API GetJoysticksCount(uint &uiCount);
	HRESULT DGLE2_API GetJoystickName(uint uiJoyId, char *pcName, uint &uiCharsCount);
	HRESULT DGLE2_API GetJoystickStates(uint uiJoyId, TJoystickStates &stJoyStates);
	HRESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubsysType);

	IDGLE2_BASE_IMPLEMENTATION1(IInput, IEngineSubSystem)
};

#endif