/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "..\Common.h"
#include <DbgHelp.h>

using namespace std;

bool bUnhandledFilterEnabled = false;

#ifdef PDB_DEBUG

#pragma comment(linker, "/defaultlib:dbghelp.lib")
#pragma message("PDB_DEBUG symbol defined: automatically linking with dbghelp.lib (dbghelp.dll)")

#define DBGHELP5_1 1

LONG WINAPI M2UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
void GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);
void WriteStackDetails(PCONTEXT pContext);
bool GetTextFileString(const char* szFileName, unsigned int iNo, char* szBuf, size_t bufLen);

static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter = NULL;
static string str_info = "";

void DGLE_API con_dump(void *pParameter, const char *pcParam)
{
	CConsole *p_console = EngineInstance(0)->pclConsole;

	if(!p_console)
		return;

	if(strlen(pcParam)!=0)
		p_console->Write("No parameters expected.");
	else
	{
		if(str_info=="")
			p_console->Write("This command works only after application crashes or any unhandled exception appears.");
		else
		{
			std::fstream dfile;
			dfile.setf(std::ios_base::right,std::ios_base::adjustfield);
			dfile.open("crash_dump.txt", std::ios::out|std::ios::trunc);
			dfile << "**DGLE Crash Dump**\n";
			dfile << str_info.c_str();
			dfile.close();
			p_console->Write("Crash dump saved to \"crash_dump.txt\".");
		}
	}
}

void InitDbgHelp(uint uiInstIdx)
{
	if(uiInstIdx == 0)
	{
		EngineInstance(0)->pclConsole->RegComProc("crash_dump", "Dumps unhandled crash callstack to txt file in current directory \"crash_dump.txt\".", &con_dump, NULL, false);

		m_previousFilter = SetUnhandledExceptionFilter(M2UnhandledExceptionFilter);

		if(NULL!=m_previousFilter) 
		{
			bUnhandledFilterEnabled = true;
			LogWrite(0, "Unhandled Exceptions Filter has been set successfully.", LT_INFO, __FILE__, __LINE__);
		}
		else
			LogWrite(0, "Setting Unhandled Exception Filter failed!", LT_INFO, __FILE__, __LINE__);
	}
	else
		if(bUnhandledFilterEnabled)
			LogWrite(uiInstIdx, "Unhandled Exception Filter already has been set to zero engine instance.", LT_INFO, __FILE__, __LINE__);
		else
			LogWrite(uiInstIdx, "Unhandled Exception Filter must be set to zero engine instance.", LT_ERROR, __FILE__, __LINE__);
}

LONG WINAPI M2UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	GenerateExceptionReport(pExceptionInfo);

	if(m_previousFilter)
		return m_previousFilter(pExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

const char* GetExceptionString(DWORD dwCode)
{
#define EXCEPTION(x) case EXCEPTION_##x: return (#x);

	switch (dwCode)
	{
		EXCEPTION(ACCESS_VIOLATION)
		EXCEPTION(DATATYPE_MISALIGNMENT)
		EXCEPTION(BREAKPOINT)
		EXCEPTION(SINGLE_STEP)
		EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
		EXCEPTION(FLT_DENORMAL_OPERAND)
		EXCEPTION(FLT_DIVIDE_BY_ZERO)
		EXCEPTION(FLT_INEXACT_RESULT)
		EXCEPTION(FLT_INVALID_OPERATION)
		EXCEPTION(FLT_OVERFLOW)
		EXCEPTION(FLT_STACK_CHECK)
		EXCEPTION(FLT_UNDERFLOW)
		EXCEPTION(INT_DIVIDE_BY_ZERO)
		EXCEPTION(INT_OVERFLOW)
		EXCEPTION(PRIV_INSTRUCTION)
		EXCEPTION(IN_PAGE_ERROR)
		EXCEPTION(ILLEGAL_INSTRUCTION)
		EXCEPTION(NONCONTINUABLE_EXCEPTION)
		EXCEPTION(STACK_OVERFLOW)
		EXCEPTION(INVALID_DISPOSITION)
		EXCEPTION(GUARD_PAGE)
		EXCEPTION(INVALID_HANDLE)
	}

	static char szBuffer[512] = { 0 };

	FormatMessageA(	FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		GetModuleHandleA("NTDLL.DLL"),
		dwCode, 0, szBuffer, sizeof( szBuffer ), 0);

	return szBuffer;
}

void GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo)
{
	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
	str_info+= "################################################\n";
	str_info+= "################ PROGRAM CRASH! ################\n";
	str_info+= "################################################\n\n";
	str_info+= "ERROR: \"" + std::string(GetExceptionString(pExceptionRecord->ExceptionCode)) + "\"\n\n";
	str_info+= "Sorry, but application must be restarted.\nIf this unhandled exception appears again please inform application/engine/plugin developer.\nUse console for debugging or make a dump file using \"crash_dump\" command.\nAlso see engine log file for any additional information available.\n\n";
	if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
	{
		str_info+= "(Can't write call stack - SymInitialize failed!)\n";
		str_info+= "################################################\n";
		return;
	}
	PCONTEXT pCtx = pExceptionInfo->ContextRecord;
	CONTEXT* pTrashableContext = pCtx;

	WriteStackDetails(pTrashableContext);
	str_info+= "################################################\n";

	CConsole *p_console = EngineInstance(0)->pclConsole;

	if(p_console)
	{
		p_console->Visible(true);
		p_console->Write(str_info.c_str());
	}

	LogWrite(0, "We are very sorry, but program crashed! See console or log for additional information.", LT_FATAL, __FILE__, __LINE__);
}

void WriteStackDetails(PCONTEXT pContext)
{				
	str_info+= "################################################\n";
	str_info+= "################## Call stack ##################\n";
	str_info+= "################################################\n";

	str_info+=
		"|Address|{FrameFunction}\nFunction or method in source file[Sourcefile and line number]\n";

	DWORD dwMachineType = 0;
	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));

	#ifdef _M_IX86
	sf.AddrPC.Offset       = pContext->Eip;
	sf.AddrPC.Mode         = AddrModeFlat;
	sf.AddrStack.Offset    = pContext->Esp;
	sf.AddrStack.Mode      = AddrModeFlat;
	sf.AddrFrame.Offset    = pContext->Ebp;
	sf.AddrFrame.Mode      = AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_I386;
	#endif

	char szTopFunc[4096] = "Call stack functions:\n\n";
	char szTextBuf[4096] = "";
	char szUndecoratedName[4096] = "";

	while(true)
	{
		if (!StackWalk(	dwMachineType,
			GetCurrentProcess(),
			GetCurrentThread(),
			&sf,
			pContext,
			0,
			SymFunctionTableAccess,
			SymGetModuleBase,
			0)
			)
			break;

		if (!sf.AddrFrame.Offset)		
			break;						

			char res1[16],res2[16];
			sprintf_s(res1,16, "%d", sf.AddrPC.Offset);
			sprintf_s(res2,16, "%d", sf.AddrFrame.Offset);
			str_info += "================================================\n";
			str_info += '|' + std::string(res1) + "|{" + std::string(res2) + "}\n";

		DWORD64 symDisplacement = 0;	

	#if DBGHELP5_1
		BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLen = 1024;

		if (SymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symDisplacement, pSymbol))
	#else			
		BYTE symbolBuffer[ sizeof(IMAGEHLP_SYMBOL64) + 1024 ];
		PIMAGEHLP_SYMBOL64 pSymbol = (PIMAGEHLP_SYMBOL64)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = 1024;

		if ( SymGetSymFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset, &symDisplacement, pSymbol ) )
	#endif
		{
			UnDecorateSymbolName(pSymbol->Name, szUndecoratedName, sizeof (szUndecoratedName), UNDNAME_COMPLETE);
			sprintf_s(szTextBuf, "%hs+%I64X  ", pSymbol->Name, symDisplacement);
		}
		else    
		{
			strcpy(szTextBuf, "[No symbol found]");
		}

		IMAGEHLP_LINE lineInfo;
		DWORD dwLineDisplacement;
		if (SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			char buf[4096] = {0};

			GetTextFileString (lineInfo.FileName, lineInfo.LineNumber, buf, 4096);
			sprintf_s(buf, "%s [%s line %u]", buf, lineInfo.FileName, lineInfo.LineNumber); 

			strcat_s(szTextBuf, buf);
		}

		str_info += std::string(szTextBuf) + '\n';
		strcat_s(szTopFunc, szTextBuf);
		strcat_s(szTopFunc, "\n");
	}

}

bool GetTextFileString(const char* szFileName, unsigned int iNo, char* szBuf, size_t bufLen)
{
	HANDLE hFile = CreateFileA(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
		return false;

	bool bFound = false;

	size_t size = GetFileSize(hFile, NULL);

	char* pFile = (char*)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

	DWORD dwDataRead;

	BOOL bRes = ReadFile(hFile, pFile, size, &dwDataRead, NULL);

	CloseHandle(hFile);

	if (!bRes)
		return false;

	unsigned int iLine = 1;

	for (unsigned int i = 0; i < size && !bFound; ++i)
	{
		if (pFile[i] == 0x0D)
			++iLine;

		if (iLine == iNo)
		{
			char* pStart = &pFile [i + 2];
			char* pEnd = strchr(pStart, 0x0DU);
			size_t len = min(size_t(pEnd - pStart), bufLen - 1);
			strncpy_s(szBuf, bufLen, pStart, len);
			szBuf[len] = 0;
			bFound = true;
		}
	}

	if (pFile)
		VirtualFree(pFile, 0, MEM_RELEASE);

	return bFound;
}
#else
void InitDbgHelp(uint uiInstIdx){}

const char* FormWin32ExceptionString(DWORD dwCode)
{
	string exp_txt;

#define EXCEPTION(x) case EXCEPTION_##x: exp_txt = #x; break;

	switch(dwCode)
	{
			EXCEPTION(ACCESS_VIOLATION)
			EXCEPTION(DATATYPE_MISALIGNMENT)
			EXCEPTION(BREAKPOINT)
			EXCEPTION(SINGLE_STEP)
			EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
			EXCEPTION(FLT_DENORMAL_OPERAND)
			EXCEPTION(FLT_DIVIDE_BY_ZERO)
			EXCEPTION(FLT_INEXACT_RESULT)
			EXCEPTION(FLT_INVALID_OPERATION)
			EXCEPTION(FLT_OVERFLOW)
			EXCEPTION(FLT_STACK_CHECK)
			EXCEPTION(FLT_UNDERFLOW)
			EXCEPTION(INT_DIVIDE_BY_ZERO)
			EXCEPTION(INT_OVERFLOW)
			EXCEPTION(PRIV_INSTRUCTION)
			EXCEPTION(IN_PAGE_ERROR)
			EXCEPTION(ILLEGAL_INSTRUCTION)
			EXCEPTION(NONCONTINUABLE_EXCEPTION)
			EXCEPTION(STACK_OVERFLOW)
			EXCEPTION(INVALID_DISPOSITION)
			EXCEPTION(GUARD_PAGE)
			EXCEPTION(INVALID_HANDLE)
	}

#undef EXCEPTION

	static char szBuffer[512] = { 0 };

	strcpy(szBuffer, (string("We are very sorry, but program crashed! Unhandled WIN32 exception ") + exp_txt + " occured.").c_str());

	return szBuffer;
}
#endif
