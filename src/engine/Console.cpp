/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Console.h"

#ifdef PLATFORM_WINDOWS
#include "platform\win\ConsoleWindow.h"
#endif

using namespace std;

#define CON_SPLIT_TWO_PARAMS(params)\
	size_t pos = params.find_first_of(' ');\
	string par1(params.substr(0,pos)), par2(params.substr(pos+1,params.size()-pos - 1));

CConsole::CConsole(bool bInSeparateThread):
_pConsoleWindow(NULL)
{
	RegComProc("terminate", "Terminates application (causes system to hardly terminate application process). Use it only if application is not responding.", &_s_Terminate, (void*)this, false);
	RegComProc("help", "Don't be stupid! :)", &_s_Help, this, false);
	RegComProc("cmdlist", "Outputs list of available console commands.", &_s_Cmdlist, (void*)this, false);
	RegComProc("con_resetpos", "Resets and recalculate console window screen position and size. Useful when console windows is out of screen area.", &_s_ResetPos,(void*)this, false);
	RegComProc("con_clear", "Clears all text in console.", &_s_Clear, (void*)this, false);
	RegComProc("con_show", "Shows console window, if is hidden.", &_s_Show, (void*)this, false);
	RegComProc("con_hide", "Hides console window.", &_s_Hide, (void*)this, false);
	RegComProc("con_pos", "Changes console window position.\r\nUsage: \"con_pos <x coord> <y coord>\"", &_s_SetPos, (void*)this, false);
	RegComProc("con_size", "Changes console window size.\r\nUsage: \"con_size <width value> <height value>\"", &_s_SetSize, (void*)this, false);

	_pConsoleWindow = new CConsoleWindow();
	_pConsoleWindow->InitWindow(bInSeparateThread, &_s_OnCmdExec, &_s_OnCmdComplete, this);
}

CConsole::~CConsole()
{
	for(uint i = 0; i<_commands.size(); i++)
	{
		delete[] _commands[i].pcName;
		delete[] _commands[i].pcHelp;
	}

	_commands.clear();

	_pConsoleWindow->Free();
}

void CConsole::_Help(const char* pcParam)
{
	string par(pcParam);

	if (par=="" || par=="help")
	{
		Write("Print \"cmdlist\" for list of available console commands.");
		Write("Print \"help <command>\" for specific command help.");
		Write("Press \"Tab\" key for command autocompletion.");
		Write("Press \"Up arrow\" key for previous command.");
	} 
	else
	{
		if(par[par.length()-1] == ' ')
			par.erase(par.length() - 1, 1);

		for (size_t i = 0; i < _commands.size(); i++)
		 if (par == string(_commands[i].pcName))
		 {
			 if(string(_commands[i].pcHelp)=="")
				Write(string("Help for command \"" + par + "\" not presented.").c_str());
			 else
				Write(_commands[i].pcHelp);
			 return;
		 }

		 Write(string("Command \"" + par + "\" not found.").c_str());
	}
}

void CConsole::Exec(const char* pcCommand)
{
	_ProcessConCmd(pcCommand);
}

void CConsole::_Cmdlist()
{
	string lst = "----------Commands List----------\r\n";
	if (!_commands.empty())
	{
		for (size_t i = 0; i<_commands.size(); i++)
			lst += " >" + string(_commands[i].pcName) + "\r\n";
		
		lst+=string("------" + IntToStr(_commands.size()) + " registered commands------\r\n");
		
		Write(lst.c_str());
	}
}

bool CConsole::EnterCrSection()
{
	return _pConsoleWindow->EnterThreadSafeSec() == S_OK ? true : false;
}

void CConsole::LeaveCrSection()
{
	_pConsoleWindow->LeaveThreadSafeSec();
}

bool CConsole::_ProcessConCmd(const char* pcParam)
{
	string cur(ToLowerCase(string(pcParam))), com, par;
	
	bool done = false, rparam = false;

	for (uint i = 0; i<cur.length(); i++)
	 if (cur[i]==' ' && !rparam) 
		 done = true;
	 else
		 if (!done) 
			 com+=cur[i]; 
		 else 
		 {
			 rparam = true; 
			 par+=cur[i];
		 }
	
	for (size_t i = 0; i < _commands.size(); i++)
		if (com == string(_commands[i].pcName))
		{
			if (_commands[i].piValue==NULL)
			{
				if (_commands[i].bNeedCritical) 
					_pConsoleWindow->EnterThreadSafeSec();

				(*_commands[i].pProc)(_commands[i].pParametr, par.c_str());
				
				if (_commands[i].bNeedCritical) 
					_pConsoleWindow->LeaveThreadSafeSec();
			}
			else
			{
				if (par == "")
				{
					Write(string(ToUpperCase(com) + " current value is " + IntToStr(*_commands[i].piValue) + ".").c_str());
					Write(string("Value may vary from " + IntToStr(_commands[i].iMinValue) + " up to " + IntToStr(_commands[i].iMaxValue) + ".").c_str());
				}
				else
				{
					int t = StrToInt(par);

					if (t == 0 && par != "0")
						Write(string("\"" + par + "\" is not a valid integer value.").c_str());
					else
						if (t < _commands[i].iMinValue || t > _commands[i].iMaxValue)
							Write(string("Value may vary from " + IntToStr(_commands[i].iMinValue) + " up to " + IntToStr(_commands[i].iMaxValue) + ".").c_str());
						else
						{
							if (_commands[i].bNeedCritical) 
								_pConsoleWindow->EnterThreadSafeSec();
						
							*_commands[i].piValue = t;
						
							if (_commands[i].pProc != NULL)
								(*_commands[i].pProc)(_commands[i].pParametr, par.c_str());

							if (_commands[i].bNeedCritical) 
								_pConsoleWindow->LeaveThreadSafeSec();

							Write(string(ToUpperCase(com) + " is set to " + IntToStr(t) + ".").c_str());
						}
				}
			}

			return true;
		}

	Write("Unknown command.");

	return false;
}

void CConsole::_OnCmdComplete(const char *pcParam)
{
	string cmds = "----\r\n";
	int count = 0, idx = 0;
	
	for (size_t i = 0; i < _commands.size(); i++)
	{	
		bool flag = true;
	
		if (strlen(pcParam) > strlen(_commands[i].pcName))
			flag = false;
		else
			for (uint j = 0; j < strlen(pcParam); j++)
				if (pcParam[j] != _commands[i].pcName[j])
				{
					flag = false;
					break;
				}

		if (flag)
		{
			count++;
			idx = i;
			cmds += " >"+string(_commands[i].pcName) + "\r\n";
		}
	}
		cmds+="----";
		
		if (count == 1) 
			_pConsoleWindow->SetEditTxt((string(_commands[idx].pcName) + " ").c_str());
		else
			if (count > 1)
				Write(cmds.c_str());
}

bool CConsole::UnRegCom(const char *pcName)
{
	if (_commands.size() > 0)
		for (size_t i = 0; i < _commands.size(); i++)
			if (string(_commands[i].pcName) == string(pcName))
			{
				delete[] _commands[i].pcName;
				delete[] _commands[i].pcHelp;
				_commands.erase(_commands.begin()+i);
				sort(_commands.begin(), _commands.end());
				return true;
			}

	return false;
}

void CConsole::RegComProc(const char *pcName, const char *pcHelp, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr, bool bShare)
{
	TConEntry t;
	t.pcName = new char[strlen(pcName)+1];
	strcpy(t.pcName, pcName);
	t.pcHelp = new char[strlen(pcHelp)+1];
	strcpy(t.pcHelp, pcHelp);
	t.pProc			= pProc;
	t.piValue		= NULL;
	t.iMaxValue		= 0;
	t.iMinValue		= 0;
	t.pParametr		= pParametr;
	t.bNeedCritical	= bShare;
	_commands.push_back(t);
	sort(_commands.begin(), _commands.end());
}

void CConsole::RegComValue(const char *pcName, const char *pcHelp, int *piValue, int iMin, int iMax, void (CALLBACK *pProc)(void *pParametr, const char *pcParam), void *pParametr, bool bShare) 
{
	TConEntry t;
	t.pcName = new char[strlen(pcName)+1];
	strcpy(t.pcName, pcName);
	t.pcHelp = new char[strlen(pcHelp)+1];
	strcpy(t.pcHelp, pcHelp);
	t.pProc			= pProc;
	t.piValue		= piValue;
	t.iMaxValue		= iMax;
	t.iMinValue		= iMin;
	t.pParametr		= pParametr;
	t.bNeedCritical	= bShare;
	_commands.push_back(t);	
	sort(_commands.begin(), _commands.end());
}

void CConsole::_SetPos(const char* pcParam)
{
	string par(pcParam);
	
	int x, y, w, h;

	_pConsoleWindow->GetSizeAndPos(x, y, w, h);
	
	if (par.size() == 0)
		Write(string("Current console position is " + IntToStr(x) + " " + IntToStr(y) + ".").c_str());
	else
	{
		CON_SPLIT_TWO_PARAMS(par);

		x = StrToInt(par1);
		
		if (x == 0 && par1 != "0") 
		{
			Write(string("\"" + par1 + "\" is not a valid integer value.").c_str());
			return;
		}

		y = StrToInt(par2);
		
		if (y == 0 && par2 != "0") 
		{
			Write(string("\"" + par2 + "\" is not a valid integer value.").c_str());
			return;
		}

		_pConsoleWindow->SetSizeAndPos(x, y, w, h);

		Write(string("Console position is set to " + IntToStr(x) + " " + IntToStr(y) + ".").c_str());
	}
}

void CConsole::_SetSize(const char *pcParam)
{
	string par(pcParam);
	
	int x, y, w, h;

	_pConsoleWindow->GetSizeAndPos(x, y, w, h);

	if (par.size() == 0)
		Write(string("Current console size is " + IntToStr(w) + " " + IntToStr(h) + ".").c_str());
	else
	{
		CON_SPLIT_TWO_PARAMS(par);

		w = StrToInt(par1);
		
		if (w == 0 && par1 != "0") 
		{
			Write(string("\"" + par1 + "\" is not a valid integer value.").c_str());
			return;
		}

		h = StrToInt(par2);
		
		if (h == 0 && par2 != "0") 
		{
			Write(string("\"" + par2 + "\" is not a valid integer value.").c_str());
			return;
		}

		_pConsoleWindow->SetSizeAndPos(x, y, w, h);

		Write(string("Console size is set to " + IntToStr(w) + " " + IntToStr(h) + ".").c_str());
	}
}

void CConsole::ResetWinPos()
{
	_pConsoleWindow->ResetSizeAndPos();
}

void CConsole::Visible(bool IsVisible)
{
	_pConsoleWindow->Visible(IsVisible);
}

void CConsole::Write(const char* pcText, bool bToPrevLine)
{
	_pConsoleWindow->OutputTxt(pcText, bToPrevLine);
}

void CALLBACK CConsole::_s_Hide(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		PTHIS(CConsole)->Visible(false);
}

void CALLBACK CConsole::_s_Show(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		PTHIS(CConsole)->Visible(true);
}

void CALLBACK CConsole::_s_Clear(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		PTHIS(CConsole)->_pConsoleWindow->Clear();
}

void CALLBACK CConsole::_s_Terminate(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		Terminate();
}

void CALLBACK CConsole::_s_SetPos(void *pParametr, const char *pcParam)
{
	PTHIS(CConsole)->_SetPos(pcParam);
}

void CALLBACK CConsole::_s_SetSize(void *pParametr, const char *pcParam)
{
	PTHIS(CConsole)->_SetSize(pcParam);
}

void CALLBACK CConsole::_s_ResetPos(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		PTHIS(CConsole)->_pConsoleWindow->ResetSizeAndPos();
}

void CALLBACK CConsole::_s_Cmdlist(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		PTHIS(CConsole)->Write("No parametrs expected.");
	else
		PTHIS(CConsole)->_Cmdlist();
}

void CALLBACK CConsole::_s_Help(void *pParametr, const char *pcParam)
{
	PTHIS(CConsole)->_Help(pcParam);
}

void CALLBACK CConsole::_s_OnCmdExec(CConsole *pConsole, const char *pcCommand)
{
	pConsole->_ProcessConCmd(pcCommand);
}

void CALLBACK CConsole::_s_OnCmdComplete(CConsole *pConsole, const char *pcCommand)
{
	pConsole->_OnCmdComplete(pcCommand);
}