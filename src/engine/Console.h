/**
\author		Korotkov Andrey aka DRON
\date		27.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

struct TConEntry
{
	char *pcName;
	char *pcHelp;
	void *pParameter;
	int *piVar, iMinValue, iMaxValue;

	bool(DGLE_API *pProc)(void *pParameter, const char *pcParam);

	bool operator < (const TConEntry &entry) const
	{
		std::string s_1(pcName), s_2(entry.pcName);
		return std::lexicographical_compare(s_1.begin(), s_1.end(), s_2.begin(), s_2.end());
	}

};

class CConsole : public CInstancedObj
{
	IConsoleWindow *_pConsoleWindow;
	std::vector<TConEntry>	_vecCommands;
	std::vector<std::string> _vecPrevCommands;
	int _iPrevMarker;

	bool _ProcessConCmd(const std::string &strCommand);
	void _OnCmdComplete(const char *pcParam);
	void _OnCmdPrev();
	void _OnCmdNext();
	bool _SetPos(const char *pcParam);
	bool _SetSize(const char *pcParam);
	bool _Save(const std::string &strFileName);
	void _Cmdlist();
	bool _Help(const char *pcParam);
	
	static bool DGLE_API _s_Hide(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Show(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Save(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Clear(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Terminate(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_SetPos(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_SetSize(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ResetPos(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Cmdlist(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_Help(void *pParameter, const char *pcParam);

	static void DGLE_API _s_OnConWindowEvent(CConsole *pConsole, E_CONSOLE_WINDOW_EVENT eEventType, const char *pcCommand);

public:

	CConsole(uint uiInsIdx);	 
	~CConsole();

	void  Visible(bool bVisible);
	TWindowHandle GetWindowHandle();
	uint32 GetThreadId();
	void  Write(const std::string &strTxt, bool bToPrevLine = false);
	void  Exec(const char *pcCommand);
	void  RegComProc(const char *pcName, const char *pcHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter); 
	void  RegComVar(const char *pcName, const char *pcHelp, int *piVar, int iMin, int iMax, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam) = NULL, void *pParameter = NULL);
	bool  UnRegCom(const char *pcName);
	void  EnterThreadSafeSection();
	void  LeaveThreadSafeSection();
	void  ResetWinPos();
};