/**
\author		Korotkov Andrey aka DRON
\date		23.04.2012 (c)Sivkov Ilya

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
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

	HRESULT CALLBACK FileName(char *pcName, uint uiCharsCount);
	HRESULT CALLBACK Next();
	HRESULT CALLBACK Free();

	IDGLE2_BASE_IMPLEMENTATION(IFileIterator);	
};

struct TDCPFileInfo
{
	uint32	ui32CRC32;
	uint32	ui32Size;
	uint32	ui32CompressedSize;
	uint32	ui32Offset;
	char	cPackedFName[256];
};

class CDCPFileSystem :public CInstancedObj, public IFileSystem
{

	IMainFileSystem *_pFS;
	TDCPFileInfo *_pInfoTable;

	IFile *_pPack;
	std::string _strPackName;	
	uint32 _ui32DataSize, _ui32FilesCount;
	std::vector<std::string> _clFindedFiles;

	bool _ReadFileInfo();
	uint32 _GetTableNumber(const std::string &strName);
	bool _OpenPack(const std::string &strPackName);
	void _Clean();
	void _ReplaceChInStr(std::string &outStr, const std::string &findCh, const std::string &repCh);
	void _ConvertFormatFromDirToRegEx(std::string &outStr, const std::string &inStr);

public:

	CDCPFileSystem(uint uiInstIdx, IMainFileSystem *pFS);
	~CDCPFileSystem();

	HRESULT CALLBACK OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile);	
	HRESULT CALLBACK DeleteFile(const char *pcName);
	HRESULT CALLBACK FileExists(const char *pcName, bool &bExists);
	HRESULT CALLBACK Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator);

	IDGLE2_BASE_IMPLEMENTATION(IFileSystem);
};