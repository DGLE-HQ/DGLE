/**
\author		Sivkov Ilya
\date		23.04.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "DCPFileSystem.h"
#include "DCPFile.h"
#include "minilzo.h"
#include "deelx.h"

using namespace std;

//CDCPFileSystem//

CDCPFileSystem::CDCPFileSystem(uint uiInstIdx, IMainFileSystem *pFS):
CInstancedObj(uiInstIdx), _pFS(pFS)
{
	_pPack = NULL;
	_pInfoTable = NULL;
}

CDCPFileSystem::~CDCPFileSystem()
{
	_Clean();
}

bool CDCPFileSystem::_ReadFileInfo()
{
	uint read;

	char ftype[7];

	_pPack->Read(ftype, 7, read);

	if (ftype[0] != 'D' || ftype[1] != 'C' || ftype[2] != 'P')
	{
		LOG("Wrong DCP package file.", LT_ERROR);
		return false;
	}

	if (ftype[4] != '1' || ftype[5] != '.' || ftype[6] != '0')
	{
		LOG("Incorrect DCP package version.", LT_ERROR);
		return false;
	}

	uint32 pos, size;
	uint32 metadata[2];

	_pPack->GetSize(size);
	_pPack->Seek(size - sizeof(metadata), FSSF_BEGIN, pos);
	_pPack->Read((void*)metadata, sizeof(metadata), read);

	if (read != sizeof(metadata))
	{
		LOG("Failed to read package metadata.", LT_ERROR);
		return false;
	}

	_ui32DataSize = metadata[0];
	_ui32FilesCount = metadata[1];

	_pInfoTable = new TDCPFileInfo[_ui32FilesCount];
		
	_pPack->Seek(size - (_ui32FilesCount*sizeof(TDCPFileInfo) + sizeof(metadata)), FSSF_BEGIN, pos);

	_pPack->Read((void*)_pInfoTable, sizeof(TDCPFileInfo)*_ui32FilesCount, read);

	if (read != sizeof(TDCPFileInfo)*_ui32FilesCount)
	{
		LOG("Failed to read file info data array.", LT_ERROR);
		return false;
	}

	return true;
}

HRESULT DGLE2_API CDCPFileSystem::DeleteFile(const char *pcName)
{
	return E_NOTIMPL;
}

HRESULT DGLE2_API CDCPFileSystem::OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile)
{
	if (eFlags & FSOF_WRITE || eFlags & FSOF_TRUNC)
	{
		LOG("DCP packages are read only.", LT_ERROR);
		return E_INVALIDARG;
	}

	string pack_name = string(pcName);

	uint palka_pos = (uint)pack_name.find_first_of("|");

	if (!(palka_pos > 0 && palka_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_FAIL;
	}

	string file_name = pack_name.substr(palka_pos + 1, pack_name.size() - 1);
	pack_name = pack_name.substr(0, palka_pos);

	if (!_OpenPack(pack_name))
	{
		LOG("Failed to open package \"" + _strPackName + "\".", LT_ERROR);
		return E_ABORT;
	}

	uint32 file_num = _GetTableNumber(file_name);

	if (file_num == -1)
	{
		LOG("Failed to find file \"" + file_name + "\" in package \"" + pack_name + "\".", LT_ERROR);
		return E_ABORT;
	}

	uint8	*cmprsd_buff = new uint8[_pInfoTable[file_num].ui32CompressedSize], 
			*dcmprsd_buff = new uint8[_pInfoTable[file_num].ui32Size];

	uint32 pos;
	_pPack->Seek(_pInfoTable[file_num].ui32Offset, FSSF_BEGIN, pos);

	uint read;
	_pPack->Read((void*)cmprsd_buff, _pInfoTable[file_num].ui32CompressedSize, read);

	if (read != _pInfoTable[file_num].ui32CompressedSize)
	{
		LOG("Failed to read file \"" + file_name + "\" compressed data in package \"" + pack_name + "\".", LT_ERROR);
		delete [] cmprsd_buff;
		delete [] dcmprsd_buff;
		return E_ABORT;		
	}

	lzo_uint uncm_size;

	int r = lzo1x_decompress(cmprsd_buff, _pInfoTable[file_num].ui32CompressedSize, dcmprsd_buff, &uncm_size, NULL);	

	if ((r != LZO_E_OK) || (uncm_size != _pInfoTable[file_num].ui32Size))
	{
		LOG("Failed to decompress file \"" + file_name + "\" in package \"" + pack_name + "\".", LT_ERROR);
		delete [] cmprsd_buff;
		delete [] dcmprsd_buff;
		return E_ABORT;		
	}

	delete [] cmprsd_buff;
	
	prFile = new CDCPFile(InstIdx(), file_name.c_str(), eFlags, dcmprsd_buff, _pInfoTable[file_num].ui32Size);

	return S_OK;
}

bool CDCPFileSystem::_OpenPack(const string &strPackName)
{
	bool open = false;
	
	if (_pPack) _pPack->IsOpen(open);
	
	if ((open && strPackName != _strPackName) || !open)
	{
		if (open) _Clean();

		_strPackName = strPackName;

		_pFS->LoadFile(_strPackName.c_str(), _pPack);

		_pPack->IsOpen(open);

		if (!open || !_ReadFileInfo())
		{
			_Clean();
			return false;
		}
	}

	return true;
}

uint32 CDCPFileSystem::_GetTableNumber(const std::string &strName) const
{
	uint32 crc32 = GetCRC32((uint8*)strName.c_str(), (uint32)strName.size());

	if (_pInfoTable)
		for (uint32 i = 0; i < _ui32FilesCount; ++i)
			if (crc32 == _pInfoTable[i].ui32CRC32)
				return i;

	return -1;
}

HRESULT DGLE2_API CDCPFileSystem::FileExists(const char *pcName, bool &bExists)
{
	string pack_name = string(pcName);

	uint palka_pos = (uint)pack_name.find_first_of("|");

	if (!(palka_pos > 0 && palka_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_FAIL;
	}

	string name = pack_name.substr(palka_pos + 1, pack_name.size() - 1);
	pack_name = pack_name.substr(0, palka_pos);

	if (!_OpenPack(pack_name))
		return E_ABORT;

	bExists = _GetTableNumber(name) != -1;
		
	return S_OK;	
}

HRESULT DGLE2_API CDCPFileSystem::Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator)
{
	string pack_name = string(pcMask);

	uint palka_pos = (uint)pack_name.find_first_of("|");

	if (!(palka_pos > 0 && palka_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_FAIL;
	}

	string mask = pack_name.substr(palka_pos + 1, pack_name.size() - 1);
	pack_name = pack_name.substr(0, palka_pos);

	if (!_OpenPack(pack_name))
		return E_ABORT;

	if (!_pInfoTable)
		return E_FAIL;

	string reg_exp_mask;

	_ConvertFormatFromDirToRegEx(reg_exp_mask, mask);
	
	CRegexpT <char> regexp(reg_exp_mask.c_str());

	_clFindedFiles.resize(0);

	for (uint32 i = 0; i < _ui32FilesCount; ++i)
	{
		MatchResult result = regexp.Match(_pInfoTable[i].cPackedFName);

		if (result.IsMatched())		
			_clFindedFiles.push_back(string(_pInfoTable[i].cPackedFName));		
	}	

	if (_clFindedFiles.size() > 0)
		prIterator = new CDCPFileIterator(InstIdx(), _clFindedFiles);
	else
		prIterator = NULL;

	return S_OK;
}

HRESULT DGLE2_API CDCPFileSystem::SendCommand(const char *pcCommand, char *pcResult, uint uiCharsCount)
{
	return E_FAIL;
}

void CDCPFileSystem::_ReplaceChInStr(string &outStr, const string &findCh, const string &repCh)
{
	uint posCh = 0;

	posCh = (uint)outStr.find_first_of(findCh.c_str(), posCh);

	while (posCh != string::npos)
	{		
		outStr.replace(posCh, findCh.size(), repCh);

		posCh += (uint)repCh.size();

		posCh = (uint)outStr.find_first_of(findCh.c_str(), posCh);	
	}
}

void CDCPFileSystem::_ConvertFormatFromDirToRegEx(string &outStr, const string &inStr)
{
	outStr = inStr;

	_ReplaceChInStr(outStr, string("\\"), string("\\\\"));
	_ReplaceChInStr(outStr, string("."), string("\\."));
	_ReplaceChInStr(outStr, string("?"), string("."));
	_ReplaceChInStr(outStr, string("*"), string(".*"));
}

void CDCPFileSystem::_Clean()
{
	if (_pPack)
		_pPack->Free();

	if (_pInfoTable)
		delete [] _pInfoTable;

	_pInfoTable = NULL;
}

//CDCPFileIterator//

CDCPFileIterator::CDCPFileIterator(uint uiInstIdx, const std::vector<std::string> &clNameList):
CInstancedObj(uiInstIdx), _clNameList(clNameList)
{
	_clNameListIter = _clNameList.begin();
}

HRESULT DGLE2_API CDCPFileIterator::FileName(char *pcName, DGLE2::uint uiCharsCount)
{
	if ((*_clNameListIter).size() > uiCharsCount)
			return E_ABORT;
	
		if (_clNameListIter == _clNameList.end())
			pcName = "";	
		else
			strcpy(pcName, (*_clNameListIter).c_str());

	return S_OK;
}

HRESULT DGLE2_API CDCPFileIterator::Next()
{
	if (++_clNameListIter == _clNameList.end())
		return S_FALSE;

	return S_OK;
}

HRESULT DGLE2_API CDCPFileIterator::Free()
{
	delete this;
	return S_OK;
}