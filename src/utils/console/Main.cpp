#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <DGLE.h>

using namespace DGLE;
using namespace std;

DGLE_DYNAMIC_FUNC

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\bin\\windows\\DGLE.dll"
#else
#	define DLL_PATH "..\\DGLE.dll"
#endif

IEngineCore *pEngineCore = NULL;

char *pcTxtBuff = NULL;
uint uiBufferSize = 0, uiPrevTxtSize = 0;
bool wasLastToPrevLine = false;

void DGLE_API ConsoleWriteEvent(void *pParameter, IBaseEvent *pEvent)
{
	IEvConsoleWrite *p_event = static_cast<IEvConsoleWrite *>(pEvent);
	
	uint txt_size;
	bool to_prev_line;
	
	p_event->GetText(NULL, txt_size, to_prev_line);

	if (txt_size > uiBufferSize)
	{
		delete[] pcTxtBuff;
		pcTxtBuff = new char [txt_size];
		uiBufferSize = txt_size;
	}

	p_event->GetText(pcTxtBuff, uiBufferSize, to_prev_line);

	if (to_prev_line)
	{
		wasLastToPrevLine = true;

		cout << "\r" << pcTxtBuff;

		if (uiPrevTxtSize > txt_size)
			for (uint i = 0; i < uiPrevTxtSize - txt_size; ++i)
				cout << " ";	
	}
	else
	{
		if (wasLastToPrevLine)
		{
			cout << endl;
			wasLastToPrevLine = false;
		}

		cout << pcTxtBuff << endl;
	}

	uiPrevTxtSize = txt_size;
}

void DGLE_API Init(void *pParameter)
{
	const vector<string> *params = reinterpret_cast<vector<string> *>(pParameter);
	
	if (params->empty())
		cout << "No arguments given." << endl
			<< "You can give this application an argument list, where any new command must be started with \">\" symbol. For example \">dcp_exec_cmd create >dcp_exec_cmd save my_pack.dcp\"." << endl;
	else
		for (size_t i = 0; i < params->size(); ++i)
			pEngineCore->ConsoleExecute((*params)[i].c_str());
}

int main(int argc, char* argv[])
{
	if (GetEngine(DLL_PATH, pEngineCore, (E_GET_ENGINE_FLAGS)(GEF_FORCE_NO_LOG_FILE | GEF_FORCE_SINGLE_THREAD)))
	{
		pEngineCore->AddEventListener(ET_ON_CONSOLE_WRITE, &ConsoleWriteEvent);

		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, "DGLE Console", TEngineWindow(), 33, (E_ENGINE_INIT_FLAGS)(EIF_LOAD_ALL_PLUGINS | EIF_FORCE_NO_WINDOW | EIF_NO_SPLASH))))
		{	
			string temp;
			for (int i = 1; i < argc; ++i)
			{
				temp.append(argv[i]);
				
				if (i != argc - 1 || (i + 1 < argc && argv[i + 1][0] != '>'))
					temp += " ";
			}

			stringstream all_params(temp);
			vector<string> params;
			
			while (getline(all_params, temp, '>'))
				if (!temp.empty())
					params.push_back(temp);
			
			pEngineCore->AddProcedure(EPT_INIT, &Init, &params);

			pEngineCore->StartEngine();

			if (params.empty())
			{
				uint buff_size;
				pEngineCore->GetVersion(NULL, buff_size);
				char *pc_version = new char[buff_size];
				pEngineCore->GetVersion(pc_version, buff_size);
				cout << "DGLE Console version " << pc_version << " started..." << endl;
				delete[] pc_version;
			
				cout << "Print \"cmdlist\" for available console commands." << endl
					<< "Print \"help <command>\" for specific command help." << endl
					<< "Note: Not all commands will work in console mode." << endl;

				bool is_finished = false;

				while (!is_finished)
				{
					string txt;

					if (wasLastToPrevLine)
					{
						cout << endl;
						wasLastToPrevLine = false;
					}

					cout << "DGLE>" << flush;

					getline(cin, txt);
				
					if (!txt.empty())
						pEngineCore->ConsoleExecute(txt.c_str());
					else
					{
						cout << "Quit DGLE Console? " << endl << "Print [Y]es or [N]o." << endl << ">" << flush;
						char c = getchar();
						is_finished = c == 'Y' || c == 'y';
						getline(cin, txt);
					}
				}
			}

			pEngineCore->QuitEngine();
		}

		FreeEngine();
	}
	else
		cout << "Couldn't load \""DLL_PATH"\"!" << endl;
	
	return 0;
}