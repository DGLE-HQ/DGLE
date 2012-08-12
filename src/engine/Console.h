/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "Common.h"

struct TConEntry
{
	char	*pcName;
	char	*pcHelp;
	void	*pParametr;
	int		*piValue;
	int		 iMinValue,
			 iMaxValue;
	bool	 bNeedCritical;

	void(CALLBACK *pProc)(void *pParametr, const char *pcParam);

	bool operator < (const TConEntry &entry)
	{
		std::string s1(pcName), s2(entry.pcName);
		return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
	}

};

class CConsole
{
	IConsoleWindow		*_pConsoleWindow;
	std::vector<TConEntry>	_commands;

	bool _ProcessConCmd(const char *pcParam);
	void _OnCmdComplete(const char *pcParam);
	void _SetPos(const char *pcParam);
	void _SetSize(const char *pcParam);
	void _Cmdlist();
	void _Help(const char* pcParam);
	
	static void CALLBACK _s_Hide(void *pParametr, const char *pcParam);
	static void CALLBACK _s_Show(void *pParametr, const char *pcParam);
	static void CALLBACK _s_Clear(void *pParametr, const char *pcParam);
	static void CALLBACK _s_Terminate(void *pParametr, const char *pcParam);
	static void CALLBACK _s_SetPos(void *pParametr, const char *pcParam);
	static void CALLBACK _s_SetSize(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ResetPos(void *pParametr, const char *pcParam);
	static void CALLBACK _s_Cmdlist(void *pParametr, const char *pcParam);
	static void CALLBACK _s_Help(void *pParametr, const char *pcParam);
	static void CALLBACK _s_OnCmdExec(CConsole *pConsole, const char *pcCommand);
	static void CALLBACK _s_OnCmdComplete(CConsole *pConsole, const char *pcCommand);

public:

	CConsole(bool bInSeparateThread);	 
	~CConsole();

	void  Visible(bool bVisible);
	void  Write(const char *pcText, bool bToPrevLine = false);
	void  Exec(const char *pcCommand);
	void  RegComProc(const char *pcName, const char *pcHelp, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr, bool bShare = true); 
	void  RegComValue(const char *pcName, const char *pcHelp, int *piValue, int iMin, int iMax, void (CALLBACK *pProc)(void *pParametr, const char *pcParam) = NULL, void *pParametr = NULL, bool bShare = true);
	bool  UnRegCom(const char *pcName);
	bool  EnterCrSection();
	void  LeaveCrSection();
	void  ResetWinPos();
};

#endif //_CONSOLE_H