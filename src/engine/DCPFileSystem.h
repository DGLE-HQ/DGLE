/**
\author		Korotkov Andrey aka DRON
\date		20.04.2016 (c)Sivkov Ilya

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CDCPFileIterator final : public CInstancedObj, public IFileIterator
{
	std::string _strMask; 

	std::vector<std::string> _clNameList;
	std::vector<std::string>::const_iterator _clNameListIter;

public:
	CDCPFileIterator(uint uiInstIdx, const std::vector<std::string> &clNameList);

	DGLE_RESULT DGLE_API FileName(char *pcName, uint &uiCharsCount) override;
	DGLE_RESULT DGLE_API Next() override;
	DGLE_RESULT DGLE_API Free() override;

	IDGLE_BASE_IMPLEMENTATION(IFileIterator, INTERFACE_IMPL_END);	
};

#pragma pack(push, 1)
struct TDCPFileInfo
{
	uint32	ui32CRC32;
	uint32	ui32Size;
	uint32	ui32CmprsdSize;
	uint32	ui32Offset;
	char	acPackedFName[256];
};
#pragma pack(pop)

class CDCPPackager final
{
	std::vector<TDCPFileInfo> _clInfoTable;
	std::vector<uint8 *> _data;
	bool _isOpened;
	std::string _strLastError;

public:

	CDCPPackager(const std::string &strFileName);
	~CDCPPackager();

	std::string &GetLastError();
	bool IsOpened();
	std::string GetFilesList();
	bool Save(const std::string &strFileName);
	bool AddFile(const std::string &strFileName, const std::string &strDir);
	bool RemoveFile(const std::string &strFileName);
	bool ExtractFile(const std::string &strSrcFileName, const std::string &strDestFileName);
};

class CDCPFileSystem final : public CInstancedObj, public IFileSystem
{
	IFile *_pPack;
	std::string _strPackName;	
	uint32 _ui32DataSize;
	std::vector<TDCPFileInfo> _clInfoTable;
	std::vector<std::string> _clFoundFiles;

	CDCPPackager *_pPackager;

	bool _ReadFileInfo();
	bool _OpenPack(std::string strPackName);
	void _Clean();
	
	static bool DGLE_API _s_ConCmdHelp(void *pParameter, const char *pcParam);
	static bool DGLE_API _s_ConExecCmd(void *pParameter, const char *pcParam);

public:

	CDCPFileSystem(uint uiInstIdx);
	~CDCPFileSystem();

	DGLE_RESULT DGLE_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile) override;
	DGLE_RESULT DGLE_API DeleteFile(const char *pcName) override;
	DGLE_RESULT DGLE_API FileExists(const char *pcName, bool &bExists) override;
	DGLE_RESULT DGLE_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator) override;

	DGLE_RESULT DGLE_API ExecuteCommand(uint uiCmd, TVariant &stVar) override;
	DGLE_RESULT DGLE_API ExecuteTextCommand(const char *pcCommand, TVariant &stVar) override;
	DGLE_RESULT DGLE_API ExecuteTextCommandEx(const char *pcCommand, char *pcResult, uint &uiCharsCount) override;

	IDGLE_BASE_GUID_IMPL(IFileSystem)
	IUNKNOWN_IMPL(INTERFACE_IMPL(IDGLE_Base, INTERFACE_IMPL(IFileSystem, INTERFACE_IMPL_END)))
};