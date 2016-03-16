/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "Console.h"

#ifdef PLATFORM_WINDOWS
#include "platform\windows\ConsoleWindow.h"
#endif

using namespace std;

#define CON_SPLIT_TWO_PARAMS(params)\
	string::size_type pos = params.find_first_of(' ');\
	string par1(params.substr(0, pos)), par2(params.substr(pos + 1));

class CEvConsoleWrite : public IEvConsoleWrite
{
	string _txt;
	bool _bToPrev;

public:

	CEvConsoleWrite(const string &txt, bool toPrev):_txt(txt), _bToPrev(toPrev) {}

	DGLE_RESULT DGLE_API GetText(char *pcTxt, uint &uiCharsCount, bool &bToPrevLine)
	{
		bToPrevLine = _bToPrev;

		if (pcTxt == NULL)
		{
			uiCharsCount = _txt.size() + 1;
			return S_OK;
		}

		if (uiCharsCount <= _txt.size())
		{
			uiCharsCount = _txt.size() + 1;
			strcpy(pcTxt, "");
			return E_INVALIDARG;
		}

		strcpy(pcTxt, _txt.c_str());

		return S_OK;
	}

	DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType)
	{
		eEvType = ET_ON_CONSOLE_WRITE;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType)
	{
		uiUnknEvType = -1;
		return S_FALSE;
	}

	IDGLE_BASE_IMPLEMENTATION(IEvConsoleWrite, INTERFACE_IMPL(IBaseEvent, INTERFACE_IMPL_END))
};

CConsole::CConsole(uint uiInsIdx):
CInstancedObj(uiInsIdx), _pConsoleWindow(NULL), _iPrevMarker(0)
{
	RegComProc("terminate", "Terminates application (causes system to hardly terminate application process). Use it only if application is not responding.", &_s_Terminate, (void*)this);
	RegComProc("help", "", &_s_Help, this);
	RegComProc("cmdlist", "Outputs list of available console commands.", &_s_Cmdlist, (void*)this);
	RegComProc("clear", "Clears all text in console.", &_s_Clear, (void*)this);
	RegComProc("save", "Saves current console output to file. When filename is not specified, output saves to \"console.txt\".\nUsage: \"save [filename]\"", &_s_Save, (void*)this);
	RegComProc("con_resetpos", "Resets and recalculate console window screen position and size. Useful when console windows is out of screen area.", &_s_ResetPos, (void*)this);
	RegComProc("con_show", "Shows console window, if is hidden.", &_s_Show, (void*)this);
	RegComProc("con_hide", "Hides console window.", &_s_Hide, (void*)this);
	RegComProc("con_pos", "Changes console window position.\nUsage: \"con_pos <x coord> <y coord>\"", &_s_SetPos, (void*)this);
	RegComProc("con_size", "Changes console window size.\nUsage: \"con_size <width value> <height value>\"", &_s_SetSize, (void*)this);

	_pConsoleWindow = new CConsoleWindow();
	_pConsoleWindow->InitWindow(!(EngineInstance(InstIdx())->eGetEngFlags & GEF_FORCE_SINGLE_THREAD), &_s_OnConWindowEvent, this);
}

CConsole::~CConsole()
{
	for(uint i = 0; i<_vecCommands.size(); ++i)
	{
		delete[] _vecCommands[i].pcName;
		delete[] _vecCommands[i].pcHelp;
	}

	_vecCommands.clear();

	_pConsoleWindow->Free();
}

bool CConsole::_Help(const char* pcParam)
{
	string par(pcParam);

	if (par == "" || par == "help")
	{
		Write(	"Print \"cmdlist\" for list of available console commands.\n"
				"Print \"help <command>\" for specific command help.\n"
				"Press \"Tab\" key for command autocompletion.\n"
				"Press \"Up arrow\" key for previous command.\n"
				"Press \"Down arrow\" key for next command.");
	} 
	else
	{
		if (par[par.length() - 1] == ' ')
			par.erase(par.length() - 1, 1);

		for (size_t i = 0; i < _vecCommands.size(); ++i)
		 if (ToLowerCase(par) == _vecCommands[i].pcName)
		 {
			 if (strlen(_vecCommands[i].pcHelp) == 0)
				Write(("Help for command \"" + par + "\" is not presented.").c_str());
			 else
				Write(_vecCommands[i].pcHelp);
			 
			 return true;
		 }

		 Write(("Command \"" + par + "\" not found.").c_str());
	}

	return true;
}

void CConsole::Exec(const char* pcCommand)
{
	const string command(pcCommand);

	Write(('>' + command).c_str());
	
	_ProcessConCmd(ToLowerCase(command));

	_vecPrevCommands.push_back(command);
	_iPrevMarker = _vecPrevCommands.size();
}

void CConsole::_Cmdlist()
{
	string lst = "----------Commands List----------\n";

	if (!_vecCommands.empty())
	{
		for (size_t i = 0; i < _vecCommands.size(); ++i)
			lst += " >" + string(_vecCommands[i].pcName) + '\n';
		
		lst += "------" + to_string(_vecCommands.size()) + " registered commands-----" + (_vecCommands.size() < 100 ? "-" : "") + '\n';
		
		Write(lst.c_str());
	}
}

bool CConsole::_Save(const string &strFileName)
{
	uint size;
	_pConsoleWindow->GetConsoleTxt(NULL, size);

	char *p_txt = new char[size];

	_pConsoleWindow->GetConsoleTxt(p_txt, size);

	fstream file;
	file.setf(ios_base::right, ios_base::adjustfield);

	if (strFileName.empty())
		file.open("console.txt", ios::out | ios::trunc);
	else
		file.open(strFileName.c_str(), ios::out | ios::trunc);

	if (!file.is_open())
	{
		Write("Failed to open output file for writing.");
		return false;
	}

	file << p_txt << endl;

	file.close();

	return true;
}

void CConsole::EnterThreadSafeSection()
{
	_pConsoleWindow->EnterThreadSafeSection();
}

void CConsole::LeaveThreadSafeSection()
{
	_pConsoleWindow->LeaveThreadSafeSection();
}

void CConsole::_OnCmdPrev()
{
	--_iPrevMarker;

	if (_iPrevMarker < 0)
		_iPrevMarker = 0;

	if ((uint)_iPrevMarker < _vecPrevCommands.size())
		_pConsoleWindow->SetEditTxt(_vecPrevCommands[_iPrevMarker].c_str());
}

void CConsole::_OnCmdNext()
{
	++_iPrevMarker;

	if ((uint)_iPrevMarker >= _vecPrevCommands.size())
	{
		_iPrevMarker = _vecPrevCommands.size() - 1;
		_pConsoleWindow->SetEditTxt("");
	}
	else
		_pConsoleWindow->SetEditTxt(_vecPrevCommands[_iPrevMarker].c_str());
}

bool CConsole::_ProcessConCmd(const std::string &strCommand)
{
	string cmd, param;
	
	string::size_type pos = strCommand.find_first_of(' ');
		
	if (pos != string::npos)
	{
		param = strCommand.substr(pos + 1);
		cmd = strCommand.substr(0, pos);
	}
	else
		cmd = strCommand;
	
	for (size_t i = 0; i < _vecCommands.size(); ++i)
		if (cmd == _vecCommands[i].pcName)
		{
			if (_vecCommands[i].piVar == NULL)
			{
				_pConsoleWindow->EnterThreadSafeSection();

				if (!(*_vecCommands[i].pProc)(_vecCommands[i].pParameter, param.c_str()))
					Write("Failed to execute command.");

				_pConsoleWindow->LeaveThreadSafeSection();
			}
			else
			{
				if (param.empty())
				{
					Write((ToUpperCase(cmd) + " current value is " + to_string(*_vecCommands[i].piVar) + ".\n"
						"Value may vary from " + to_string(_vecCommands[i].iMinValue) + " up to " + to_string(_vecCommands[i].iMaxValue) + '.').c_str());
				}
				else
				{
					const int t = stoi(param);

					if (t == 0 && param != "0")
						Write(('\"' + param + "\" is not a valid integer value.").c_str());
					else
						if (t < _vecCommands[i].iMinValue || t > _vecCommands[i].iMaxValue)
							Write(("Value may vary from " + to_string(_vecCommands[i].iMinValue) + " up to " + to_string(_vecCommands[i].iMaxValue) + '.').c_str());
						else
						{
							_pConsoleWindow->EnterThreadSafeSection();
							
							bool res = true;

							if (_vecCommands[i].pProc != NULL)
								res = (*_vecCommands[i].pProc)(_vecCommands[i].pParameter, param.c_str());
							
							if (res)
							{
								*_vecCommands[i].piVar = t;
								Write((ToUpperCase(cmd) + " is set to " + to_string(t) + '.').c_str());
							}

							_pConsoleWindow->LeaveThreadSafeSection();
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
	string cmds = "----\n", cmd = ToLowerCase(pcParam);
	int count = 0, idx = 0;
	
	for (size_t i = 0; i < _vecCommands.size(); ++i)
	{	
		bool flag = true;
	
		if (cmd.length() > strlen(_vecCommands[i].pcName))
			flag = false;
		else
			for (uint j = 0; j < cmd.length(); ++j)
				if (cmd[j] != _vecCommands[i].pcName[j])
				{
					flag = false;
					break;
				}

		if (flag)
		{
			++count;
			idx = (int)i;
			cmds += " >" + string(_vecCommands[i].pcName) + '\n';
		}
	}
		cmds += "----";
		
		if (count == 1) 
			_pConsoleWindow->SetEditTxt((string(_vecCommands[idx].pcName) + ' ').c_str());
		else
			if (count > 1)
				Write(cmds.c_str());
}

bool CConsole::UnRegCom(const char *pcName)
{
	string cmd = ToLowerCase(pcName);
	
	for (size_t i = 0; i < _vecCommands.size(); ++i)
		if (strcmp(_vecCommands[i].pcName, cmd.c_str()) == 0)
		{
			delete[] _vecCommands[i].pcName;
			delete[] _vecCommands[i].pcHelp;
			
			_vecCommands.erase(_vecCommands.begin() + i);
			
			return true;
		}

	return false;
}

void CConsole::RegComProc(const char *pcName, const char *pcHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter)
{
	TConEntry t;
	
	t.pcName = new char[strlen(pcName) + 1];
	strcpy(t.pcName, ToLowerCase(pcName).c_str());
	
	t.pcHelp = new char[strlen(pcHelp) + 1];
	strcpy(t.pcHelp, pcHelp);
	
	t.pProc	= pProc;
	t.piVar = NULL;
	t.iMaxValue	= 0;
	t.iMinValue	= 0;
	t.pParameter	= pParameter;
	
	_vecCommands.push_back(t);
	
	sort(_vecCommands.begin(), _vecCommands.end());
}

void CConsole::RegComVar(const char *pcName, const char *pcHelp, int *piVar, int iMin, int iMax, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter) 
{
	TConEntry t;
	
	t.pcName = new char[strlen(pcName) + 1];
	strcpy(t.pcName, ToLowerCase(pcName).c_str());
	
	t.pcHelp = new char[strlen(pcHelp) + 1];
	strcpy(t.pcHelp, pcHelp);
	
	t.pProc	= pProc;
	t.piVar = piVar;
	t.iMaxValue	= iMax;
	t.iMinValue	= iMin;
	t.pParameter	= pParameter;
	
	_vecCommands.push_back(t);	
	
	sort(_vecCommands.begin(), _vecCommands.end());
}

bool CConsole::_SetPos(const char* pcParam)
{
	const string par(pcParam);
	
	int x, y, w, h;

	_pConsoleWindow->GetSizeAndPos(x, y, w, h);
	
	if (par.size() == 0)
	{
		Write(("Current console position is left: " + to_string(x) + " top: " + to_string(y) + " .").c_str());
		return true;
	}
	else
	{
		CON_SPLIT_TWO_PARAMS(par);

		x = stoi(par1);
		
		if (x == 0 && par1 != "0") 
		{
			Write(('\"' + par1 + "\" is not a valid integer value.").c_str());
			return false;
		}

		y = stoi(par2);
		
		if (y == 0 && par2 != "0") 
		{
			Write(('\"' + par2 + "\" is not a valid integer value.").c_str());
			return false;
		}

		_pConsoleWindow->SetSizeAndPos(x, y, w, h);

		Write(("Console position is set to left: " + to_string(x) + " top: " + to_string(y) + " .").c_str());

		return true;
	}
}

bool CConsole::_SetSize(const char *pcParam)
{
	const string par(pcParam);
	
	int x, y, w, h;

	_pConsoleWindow->GetSizeAndPos(x, y, w, h);

	if (par.size() == 0)
	{
		Write(("Current console size is width: " + to_string(w) + " height: " + to_string(h) + " .").c_str());
		return true;
	}
	else
	{
		CON_SPLIT_TWO_PARAMS(par);

		w = stoi(par1);
		
		if (w == 0 && par1 != "0") 
		{
			Write(('\"' + par1 + "\" is not a valid integer value.").c_str());
			return false;
		}

		h = stoi(par2);
		
		if (h == 0 && par2 != "0") 
		{
			Write(('\"' + par2 + "\" is not a valid integer value.").c_str());
			return false;
		}

		_pConsoleWindow->SetSizeAndPos(x, y, w, h);

		Write(("Console size is set to width: " + to_string(w) + " height: " + to_string(h) + " .").c_str());

		return true;
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

TWindowHandle CConsole::GetWindowHandle()
{
	TWindowHandle handle;
	_pConsoleWindow->GetWindowHandle(handle);
	
	return handle;
}

uint32 CConsole::GetThreadId()
{
	uint32 thread_id;
	_pConsoleWindow->GetThreadId(thread_id);

	return thread_id;
}

void CConsole::Write(const std::string &strTxt, bool bToPrevLine)
{
	string txt(strTxt);

	if (Core())
	{
		CEvConsoleWrite event(txt, bToPrevLine);
		Core()->CastEvent(ET_ON_CONSOLE_WRITE, &event);
	}

	for (size_t i = 0; i < txt.size(); ++i)
		if (txt[i] == '\n')
		{
			txt.insert(i, 1, '\r');
			++i;
		}

	_pConsoleWindow->OutputTxt(txt.c_str(), bToPrevLine);
}

bool DGLE_API CConsole::_s_Hide(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CConsole)->Visible(false);
		return true;
	}
}

bool DGLE_API CConsole::_s_Show(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CConsole)->Visible(true);
		return true;
	}
}

bool DGLE_API CConsole::_s_Clear(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CConsole)->_pConsoleWindow->Clear();
		return true;
	}
}

bool DGLE_API CConsole::_s_Save(void *pParameter, const char *pcParam)
{
	return PTHIS(CConsole)->_Save(pcParam);
}

bool DGLE_API CConsole::_s_Terminate(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		Terminate();
		return true;
	}
}

bool DGLE_API CConsole::_s_SetPos(void *pParameter, const char *pcParam)
{
	return PTHIS(CConsole)->_SetPos(pcParam);
}

bool DGLE_API CConsole::_s_SetSize(void *pParameter, const char *pcParam)
{
	return PTHIS(CConsole)->_SetSize(pcParam);
}

bool DGLE_API CConsole::_s_ResetPos(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CConsole)->_pConsoleWindow->ResetSizeAndPos();
		return true;
	}
}

bool DGLE_API CConsole::_s_Cmdlist(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		PTHIS(CConsole)->Write("No parameters expected.");
		return false;
	}
	else
	{
		PTHIS(CConsole)->_Cmdlist();
		return true;
	}
}

bool DGLE_API CConsole::_s_Help(void *pParameter, const char *pcParam)
{
	return PTHIS(CConsole)->_Help(pcParam);
}

void DGLE_API CConsole::_s_OnConWindowEvent(CConsole *pConsole, E_CONSOLE_WINDOW_EVENT eEventType, const char *pcCommand)
{
	switch (eEventType)
	{
	case CWE_EXECUTE_COMMAND: pConsole->Exec(pcCommand); break;
	case CWE_COMPLETE_COMMAND: pConsole->_OnCmdComplete(pcCommand); break;
	case CWE_PREVIOUS_COMMAND: pConsole->_OnCmdPrev(); break;
	case CWE_NEXT_COMMAND: pConsole->_OnCmdNext(); break;
	}
}