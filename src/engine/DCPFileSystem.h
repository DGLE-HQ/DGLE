/**
\author		Korotkov Andrey aka DRON
\date		30.10.2012 (c)Sivkov Ilya

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CDCPFileIterator: public CInstancedObj, public IFileIterator
{
	char _acName[MAX_PATH];
	std::string _strMask; 

	std::vector<std::string> _clNameList;
	std::vector<std::string>::iterator _clNameListIter;

public:
	CDCPFileIterator(uint uiInstIdx, const std::vector<std::string> &clNameList);

	DGLE_RESULT DGLE_API FileName(char *pcName, uint &uiCharsCount);
	DGLE_RESULT DGLE_API Next();
	DGLE_RESULT DGLE_API Free();

	IDGLE_BASE_IMPLEMENTATION(IFileIterator);	
};

#pragma pack( push, 1 )
struct TDCPFileInfo
{
	uint32	ui32CRC32;
	uint32	ui32Size;
	uint32	ui32CmprsdSize;
	uint32	ui32Offset;
	char	acPackedFName[256];
};
#pragma pack( pop )

class CDCPPackager
{
	std::vector<TDCPFileInfo> _clInfoTable;
	std::vector<uint8 *> _data;
	bool _isOpened;
	std::string _strLastError;

public:

	CDCPPackager(const std::string &strFileName);
	~CDCPPackager();

	std::string& GetLastError();
	bool IsOpened();
	std::string GetFilesList();
	bool Save(const std::string &strFileName);
	bool AddFile(const std::string &strFileName, const std::string &strDir);
	bool RemoveFile(const std::string &strFileName);
	bool ExtractFile(const std::string &strSrcFileName, const std::string &strDestFileName);
};

class CDCPFileSystem :public CInstancedObj, public IFileSystem
{
	IFile *_pPack;
	std::string _strPackName;	
	uint32 _ui32DataSize;
	std::vector<TDCPFileInfo> _clInfoTable;
	std::vector<std::string> _clFindedFiles;

	CDCPPackager *_pPackager;

	bool _ReadFileInfo();
	bool _OpenPack(const std::string &strPackName);
	void _Clean();
	void _ReplaceSubstrInStr(std::string &outStr, const std::string &findCh, const std::string &repCh);
	void _ConvertFormatFromDirToRegEx(std::string &outStr, const std::string &inStr);

	static void DGLE_API _s_ConCmdHelp(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ConExecCmd(void *pParametr, const char *pcParam);

public:

	CDCPFileSystem(uint uiInstIdx);
	~CDCPFileSystem();

	static uint32 s_GetTableIdx(const std::vector<TDCPFileInfo> &clInfoTable, const std::string &strName);
	static void s_CorrectSlashes(std::string &strFileName);

	DGLE_RESULT DGLE_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile);	
	DGLE_RESULT DGLE_API DeleteFile(const char *pcName);
	DGLE_RESULT DGLE_API FileExists(const char *pcName, bool &bExists);
	DGLE_RESULT DGLE_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator);

	DGLE_RESULT DGLE_API ExecCmd(uint uiCmd, TVariant &stVar);
	DGLE_RESULT DGLE_API ExecCmdStr(const char *pcCommand, char *pcResult, uint &uiCharsCount);
	DGLE_RESULT DGLE_API ExecCmdVar(const char *pcCommand, TVariant &stVar);

	IDGLE_BASE_GUID_IMPL(IFileSystem)
	IUNKNOWN_IMPL(IFileSystem)
};