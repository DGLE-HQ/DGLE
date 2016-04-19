/**
\author		Sivkov Ilya
\date		19.04.2016 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "DCPFileSystem.h"
#include "MainFileSystem.h"
#include "BuffFile.h"
#include "minilzo.h"
#include <regex>

using namespace std;
using fs::path;

#if defined _MSC_VER && _MSC_VER <= 1900 && !__clang__
static constexpr auto Dcp = L".dcp";
#else
namespace DcpImpl
{
#	define DCP(prefix) prefix##".dcp"

	template<typename Char>
	static constexpr const Char Dcp[] = nullptr;

	template<>
	static constexpr const char Dcp<char>[] = DCP();

	template<>
	static constexpr const wchar_t Dcp<wchar_t>[] = DCP(L);

	template<>
	static constexpr const char16_t Dcp<char16_t>[] = DCP(u);

	template<>
	static constexpr const char32_t Dcp<char32_t>[] = DCP(U);

#	undef DCP
}

static constexpr auto Dcp = DcpImpl::Dcp<path::value_type>;
#endif

// CDCPPackager //

CDCPPackager::CDCPPackager(const string &strFileName)
{
	if (strFileName.empty())
		_isOpened = true;
	else
	{
		_isOpened = false;

		if (ToLowerCase(path(strFileName).extension().native()) != Dcp)
		{
			_strLastError = "Wrong file extension.";
			return;
		}

		ifstream pack(strFileName.c_str(), ios_base::in | ios_base::binary);

		if (!pack.is_open())
			return;

		char ftype[8];
		pack.read(ftype, 7);
		ftype[7] = '\0';

		if (strcmp(ftype, "DCP 1.0") != 0)
		{
			_strLastError = "Wrong file format.";
			return;
		}

		uint32 meta[2];
		pack.seekg(-2 * (int)sizeof(uint32), ios_base::end);
		pack.read((char *)meta, sizeof meta);
		pack.seekg(7, ios_base::beg);

		uint8 *data = new uint8[meta[0]];
		pack.read((char *)data, meta[0]);

		_clInfoTable.resize(meta[1]);
		pack.read((char *)&_clInfoTable[0], meta[1] * sizeof(TDCPFileInfo));

		pack.close();

		_data.resize(_clInfoTable.size());

		for (size_t i = 0; i < _clInfoTable.size(); ++i)
		{
			uint8 *const fdat = new uint8[_clInfoTable[i].ui32CmprsdSize];
			memcpy(fdat, &data[_clInfoTable[i].ui32Offset], _clInfoTable[i].ui32CmprsdSize);
			_data[i] = fdat;
		}

		delete[] data;

		_isOpened = true;
	}
}

CDCPPackager::~CDCPPackager()
{
	for (const auto item : _data)
		delete[] item;
}

string &CDCPPackager::GetLastError()
{
	return _strLastError;
}

bool CDCPPackager::IsOpened()
{
	return _isOpened;
}

string CDCPPackager::GetFilesList()
{
	if (!_isOpened)
	{
		_strLastError = "Package not specified.";
		return _strLastError;
	}

	string res;

	for (const auto &info : _clInfoTable)
		res.append(info.acPackedFName) += ';';

	if (!res.empty())
		res.pop_back();

	return res;
}

bool CDCPPackager::Save(const string &strFileName)
{
	if (!_isOpened)
	{
		_strLastError = "Package not specified.";
		return false;
	}

	if (ToLowerCase(path(strFileName).extension().native()) != Dcp)
	{
		_strLastError = "Wrong file extension.";
		return false;
	}

	ofstream pack(strFileName.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);

	if (!pack.is_open())
	{
		_strLastError = "Failed to open file for writing.";
		return false;
	}

	const char ftype[7] = { 'D', 'C', 'P', ' ', '1', '.', '0' };
	pack.write(ftype, sizeof ftype);

	uint32 size = 0;

	for (size_t i = 0; i < _clInfoTable.size(); ++i)
	{
		_clInfoTable[i].ui32Offset = size;
		pack.write((const char *)_data[i], _clInfoTable[i].ui32CmprsdSize);
		size += _clInfoTable[i].ui32CmprsdSize;
	}

	if (!_clInfoTable.empty())
		pack.write((char *)&_clInfoTable[0], _clInfoTable.size() * sizeof(TDCPFileInfo));

	uint32 meta[2] = {size, _clInfoTable.size()};
	pack.write((const char *)meta, sizeof meta);

	pack.close();

	return true;
}

bool CDCPPackager::AddFile(const string &strFileName, const string &strDir)
{
	if (!_isOpened)
	{
		_strLastError = "Package not specified.";
		return false;
	}

	string file_name = (strDir / path(strFileName).filename()).string();

	if (file_name.size() > 255)
	{
		_strLastError = "File name is too long.";
		return false;
	}

	CDCPFileSystem::s_CorrectSlashes(file_name);

	if (-1 != CDCPFileSystem::s_GetTableIdx(_clInfoTable, file_name))
	{
		_strLastError = "File with such name already exists.";
		return false;
	}

	ifstream file(strFileName, ios_base::in | ios_base::binary);

	if (!file.is_open())
	{
		_strLastError = "Failed to open source file.";
		return false;
	}

	file.seekg(0, ios_base::end);
	const streamsize size = file.tellg();
	
	file.seekg(0, ios_base::beg);
	uint8 *dat = new uint8[size];
	file.read((char *)dat, size);
	
	file.close();

	uint8 *cmprsd_buffer = new uint8[size + size / 16 + 64 + 3];

	lzo_uint cmprsd_size;
	lzo_align_t __LZO_MMODEL wrkmem [((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t)];

	if (LZO_E_OK != lzo1x_1_compress(dat, size, cmprsd_buffer, &cmprsd_size, wrkmem))
	{
		_strLastError = "Compression error.";
		delete[] dat;
		delete[] cmprsd_buffer;
		return false;
	}

	delete[] dat;

	_data.push_back(cmprsd_buffer);

	TDCPFileInfo info;

	memset(&info, 0, sizeof(TDCPFileInfo));

	info.ui32CRC32 = GetCRC32((uint8 *)file_name.c_str(), file_name.size());
	info.ui32Size = size;
	info.ui32CmprsdSize = cmprsd_size;
	info.ui32Offset = 0;
	strcpy(info.acPackedFName, file_name.c_str());

	_clInfoTable.push_back(info);

	return true;
}

bool CDCPPackager::RemoveFile(const string &strFileName)
{
	if (!_isOpened)
	{
		_strLastError = "Package not specified.";
		return false;
	}

	string file_name(strFileName);

	CDCPFileSystem::s_CorrectSlashes(file_name);

	uint32 idx = CDCPFileSystem::s_GetTableIdx(_clInfoTable, file_name);

	if (-1 == idx)
	{
		_strLastError = "File not found in package.";
		return false;
	}

	delete[] _data[idx];

	_data.erase(_data.begin() + idx);
	_clInfoTable.erase(_clInfoTable.begin() + idx);

	return true;
}

bool CDCPPackager::ExtractFile(const string &strSrcFileName, const string &strDestFileName)
{
	if (!_isOpened)
	{
		_strLastError = "Package not specified.";
		return false;
	}

	string file_name(strSrcFileName);

	CDCPFileSystem::s_CorrectSlashes(file_name);

	uint32 idx = CDCPFileSystem::s_GetTableIdx(_clInfoTable, file_name);

	if (-1 == idx)
	{
		_strLastError = "File not found in package.";
		return false;
	}

	uint8 *dcmprsd_buff = new uint8[_clInfoTable[idx].ui32Size];

	lzo_uint uncm_size;
	int r = lzo1x_decompress(_data[idx], _clInfoTable[idx].ui32CmprsdSize, dcmprsd_buff, &uncm_size, NULL);	

	if (r != LZO_E_OK || uncm_size != _clInfoTable[idx].ui32Size)
	{
		_strLastError = "Decompression error.";
		delete[] dcmprsd_buff;
		return false;
	}

	ofstream file(strDestFileName.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);

	if (!file.is_open())
	{
		_strLastError = "Failed to open output file for writing.";
		delete[] dcmprsd_buff;
		return false;
	}

	file.write((char *)dcmprsd_buff, uncm_size);

	file.close();

	delete[] dcmprsd_buff;

	return true;
}

// CDCPFileSystem //

CDCPFileSystem::CDCPFileSystem(uint uiInstIdx):
CInstancedObj(uiInstIdx), _pPack(NULL), _pPackager(NULL)
{
	Console()->RegComProc("dcp_cmd_help", "Prints help for extra text commands which you can send to DCP file system using ExecuteCommand, ExecuteTextCommand2 or ExecuteTextCommand methods of IFileSystem class or via \"dcp_exec_cmd\" console command.", &_s_ConCmdHelp, this);
	Console()->RegComProc("dcp_exec_cmd", "Executes extra command of DCP file system. These commands are used to manipulate with DCP packages. See \"dcp_cmd_help\" for more help.", &_s_ConExecCmd, this);
}

CDCPFileSystem::~CDCPFileSystem()
{
	delete _pPackager;
	
	_Clean();

	Console()->UnRegCom("dcp_exec_cmd");
	Console()->UnRegCom("dcp_cmd_help");
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
	uint32 meta[2];

	_pPack->GetSize(size);
	_pPack->Seek(size - 2 * sizeof(uint32), FSSF_BEGIN, pos);
	_pPack->Read(meta, 2 * sizeof(uint32), read);

	if (read != 2 * sizeof(uint32))
	{
		LOG("Failed to read package metadata.", LT_ERROR);
		return false;
	}

	_ui32DataSize = meta[0];

	_clInfoTable.resize(meta[1]);
		
	_pPack->Seek(size - (meta[1] * sizeof(TDCPFileInfo) + sizeof(meta)), FSSF_BEGIN, pos);

	_pPack->Read(&_clInfoTable[0], sizeof(TDCPFileInfo) * meta[1], read);

	if (read != sizeof(TDCPFileInfo) * meta[1])
	{
		LOG("Failed to read file info data chunk.", LT_ERROR);
		return false;
	}

	return true;
}

DGLE_RESULT DGLE_API CDCPFileSystem::DeleteFile(const char *pcName)
{
	LOG("Failed to delete file \""s + pcName + "\", DCP packages are read only.", LT_ERROR);
	return E_NOTIMPL;
}

DGLE_RESULT DGLE_API CDCPFileSystem::OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile)
{
	if (eFlags & FSOF_WRITE || eFlags & FSOF_TRUNC)
	{
		LOG("Failed to open file \""s + pcName + "\" for writing, DCP packages are read only.", LT_ERROR);
		return E_INVALIDARG;
	}

	string pack_name = pcName;

	const string::size_type delim_pos = pack_name.find_first_of('|');

	if (!(delim_pos > 0 && delim_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_INVALIDARG;
	}

	string file_name = pack_name.substr(delim_pos + 1);
	pack_name.erase(delim_pos).shrink_to_fit();

	if (!_OpenPack(pack_name))
	{
		LOG("Failed to open package \"" + _strPackName + "\".", LT_ERROR);
		return E_ABORT;
	}

	s_CorrectSlashes(file_name);

	const uint32 file_idx = s_GetTableIdx(_clInfoTable, file_name);

	if (file_idx == -1)
	{
		LOG("Failed to find file \"" + file_name + "\" in package \"" + pack_name + "\".", LT_ERROR);
		return E_ABORT;
	}

	uint8	*const cmprsd_buff = new uint8[_clInfoTable[file_idx].ui32CmprsdSize], 
			*const dcmprsd_buff = new uint8[_clInfoTable[file_idx].ui32Size];

	uint32 pos;
	_pPack->Seek(_clInfoTable[file_idx].ui32Offset + 7, FSSF_BEGIN, pos);

	uint read;
	_pPack->Read(cmprsd_buff, _clInfoTable[file_idx].ui32CmprsdSize, read);

	if (read != _clInfoTable[file_idx].ui32CmprsdSize)
	{
		LOG("Failed to read file \"" + file_name + "\" compressed data in package \"" + pack_name + "\".", LT_ERROR);
		delete[] cmprsd_buff;
		delete[] dcmprsd_buff;
		return E_ABORT;		
	}

	lzo_uint uncm_size;

	const int r = lzo1x_decompress(cmprsd_buff, _clInfoTable[file_idx].ui32CmprsdSize, dcmprsd_buff, &uncm_size, NULL);	

	if (r != LZO_E_OK || uncm_size != _clInfoTable[file_idx].ui32Size)
	{
		LOG("Failed to decompress file \"" + file_name + "\" in package \"" + pack_name + "\".", LT_ERROR);
		delete[] cmprsd_buff;
		delete[] dcmprsd_buff;
		return E_ABORT;		
	}

	delete[] cmprsd_buff;
	
	prFile = new CBuffFile(dcmprsd_buff, _clInfoTable[file_idx].ui32Size, true, file_name);

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

		if (S_OK == Core()->pMainFS()->LoadFile(_strPackName.c_str(), _pPack))
			_pPack->IsOpen(open);

		if (!open || !_ReadFileInfo())
		{
			_Clean();
			return false;
		}
	}

	return true;
}

uint32 CDCPFileSystem::s_GetTableIdx(const vector<TDCPFileInfo> &clInfoTable, const std::string &strName)
{
	const uint32 crc32 = GetCRC32((uint8 *)strName.c_str(), (uint32)strName.size());

	for (size_t i = 0; i < clInfoTable.size(); ++i)
		if (crc32 == clInfoTable[i].ui32CRC32)
			return i;

	return -1;
}

DGLE_RESULT DGLE_API CDCPFileSystem::FileExists(const char *pcName, bool &bExists)
{
	string pack_name = pcName;

	const string::size_type delim_pos = pack_name.find_first_of('|');

	if (!(delim_pos > 0 && delim_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_INVALIDARG;
	}

	string name = pack_name.substr(delim_pos + 1);
	pack_name.erase(delim_pos).shrink_to_fit();

	if (!_OpenPack(pack_name))
		return E_ABORT;

	s_CorrectSlashes(name);

	bExists = s_GetTableIdx(_clInfoTable, name) != -1;
		
	return S_OK;	
}

DGLE_RESULT DGLE_API CDCPFileSystem::Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator)
{
	string pack_name = pcMask;

	const string::size_type delim_pos = pack_name.find_first_of('|');

	if (!(delim_pos > 0 && delim_pos != string::npos))
	{
		LOG("Wrong file name, package delimiter \"|\" not found in name \"" + pack_name + "\".", LT_ERROR);
		return E_INVALIDARG;
	}

	string mask = pack_name.substr(delim_pos + 1);	// non-const in order to enable move semantic below
	pack_name.erase(delim_pos).shrink_to_fit();

	if (!_OpenPack(pack_name))
		return E_ABORT;

	if (_clInfoTable.empty())
		return E_FAIL;

	const string reg_exp_mask = _s_ConvertFormatFromDirToRegEx(move(mask));
	
	const regex regexp(reg_exp_mask);

	_clFoundFiles.clear();

	for (const auto &info : _clInfoTable)
		if (regex_match(info.acPackedFName, regexp))		
			_clFoundFiles.emplace_back(info.acPackedFName);

	if (!_clFoundFiles.empty())
		prIterator = new CDCPFileIterator(InstIdx(), _clFoundFiles);
	else
		prIterator = NULL;

	return S_OK;
}

void CDCPFileSystem::_s_ReplaceSubstrInStr(string &outStr, const string &findCh, const string &repCh)
{
	for (auto posCh = outStr.find(findCh); posCh != string::npos; posCh = outStr.find(findCh, posCh + repCh.size()))
		outStr.replace(posCh, findCh.size(), repCh);
}

void CDCPFileSystem::s_CorrectSlashes(string &strFileName)
{
	replace(strFileName.begin(), strFileName.end(), '/', '\\');
}

string CDCPFileSystem::_s_ConvertFormatFromDirToRegEx(string str)
{
	s_CorrectSlashes(str);
	_s_ReplaceSubstrInStr(str, "\\", "\\\\");
	_s_ReplaceSubstrInStr(str, ".", "\\.");
	_s_ReplaceSubstrInStr(str, "?", ".");
	_s_ReplaceSubstrInStr(str, "*", ".*");
	return str;
}

void CDCPFileSystem::_Clean()
{
	if (_pPack)
		_pPack->Free();

	_clInfoTable.clear();
}

bool DGLE_API CDCPFileSystem::_s_ConCmdHelp(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CDCPFileSystem, "No parameters expected.");
		return false;
	}
	else
	{
		CON(CDCPFileSystem, "You can send some extra commands to DCP file system using ExecuteCommand, ExecuteTextCommand or ExecuteTextCommandEx methods of IFileSystem class or via \"dcp_exec_cmd\" console command.\n"
		"Commands for ExecuteTextCommand method or console:\n"
		"For these commands return value is boolean.\n"
		"\"create\" - creates empty package in memory.\n"
		"\"close\" - closes current package.\n"
		"\"open [file name]\" - loads package to memory.\n"
		"\"save [file name]\" - saves current package to hdd file.\n"
		"\"add [file name];<directory in package>\" - adds file to current package, you could also specify directory in package.\n"
		"\"extract [src file name];[dest file name]\" - extracts file from current package to hdd.\n"
		"\"remove [file name]\" - deletes file from current package.\n"
		"Commands for ExecuteTextCommandEx method or console:\n"
		"\"list\" - returns string with all files names in current package separated by \";\".\n"
		"\"last_error\" - returns string with last error discription text.\n"
		"Commands for ExecuteCommand method only:\n"
		"-1 - deletes any memory cache data or junk.");
		return true;
	}
}

bool DGLE_API CDCPFileSystem::_s_ConExecCmd(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) == 0)
	{
		CON(CDCPFileSystem, "Parameters expected.");
		return false;
	}
	else
	{
		string cmd(pcParam), param;
		
		const string::size_type pos = cmd.find_first_of(' ');
		
		if (pos != string::npos)
		{
			param = cmd.substr(pos + 1);
			cmd.erase(pos);
		}

		if (cmd == "list")
		{
			if (!PTHIS(CDCPFileSystem)->_pPackager)
			{
				CON(CDCPFileSystem, "Package not specified.");
				return false;
			}
			else
			{
				string list = PTHIS(CDCPFileSystem)->_pPackager->GetFilesList();

				replace(list.begin(), list.end(), ';', '\n');

				CON(CDCPFileSystem, list.c_str());

				return true;
			}
		}
		else
			if (cmd == "last_error")
			{
				if (!PTHIS(CDCPFileSystem)->_pPackager)
				{
					CON(CDCPFileSystem, "Package not specified.");
					return false;
				}
				else
				{
					CON(CDCPFileSystem, PTHIS(CDCPFileSystem)->_pPackager->GetLastError().c_str());
					return true;
				}
			}
			else
			{
				TVariant var;
				
				PTHIS(CDCPFileSystem)->ExecuteTextCommand((cmd + ' ' + param).c_str(), var);
				
				if (!var.AsBool())
				{
					if (PTHIS(CDCPFileSystem)->_pPackager)
					{
						string res = PTHIS(CDCPFileSystem)->_pPackager->GetLastError();
						if (res.empty()) res = "Wrong command arguments.";
						CON(CDCPFileSystem, res.c_str());
						return false;
					}
					else
					{
						CON(CDCPFileSystem, "Package not specified.");
						return true;
					}
				}
				else
				{
					CON(CDCPFileSystem, "Command executed succesfully.");
					return true;
				}
			}
	}
}

DGLE_RESULT DGLE_API CDCPFileSystem::ExecuteCommand(uint uiCmd, TVariant &stVar)
{
	if (uiCmd == -1)
	{
		delete _pPackager;
		_pPackager = NULL;
		return S_OK;
	}
	
	stVar.Clear();

	return E_INVALIDARG;
}

DGLE_RESULT DGLE_API CDCPFileSystem::ExecuteTextCommandEx(const char *pcCommand, char *pcResult, uint &uiCharsCount)
{
	if (strcmp(pcCommand, "list") == 0)
	{
		if (!_pPackager)
			return E_ABORT;

		const string list = _pPackager->GetFilesList();

		if (!pcResult || uiCharsCount < list.size() + 1)
		{
			if (pcResult)
			{
				if (uiCharsCount == 0)
					return E_INVALIDARG;

				strcpy(pcResult, list.substr(0, uiCharsCount - 1).c_str());				
			}

			uiCharsCount = list.size() + 1;

			return S_OK;
		}

		strcpy(pcResult, list.c_str());

		return S_OK;
	}
	else
		if (strcmp(pcCommand, "last_error") == 0)
		{
			if (!_pPackager)
				return E_ABORT;

			const string err = _pPackager->GetLastError();

			if (!pcResult || uiCharsCount < err.size() + 1)
			{
				if (pcResult)
				{
					if (uiCharsCount == 0)
						return E_INVALIDARG;

					strcpy(pcResult, err.substr(0, uiCharsCount - 1).c_str());				
				}

				uiCharsCount = err.size() + 1;

				return S_OK;
			}

			strcpy(pcResult, err.c_str());

			return S_OK;
		}
		else
			if (uiCharsCount < 1)
				return E_INVALIDARG;
			else
			{		
				if (pcResult && uiCharsCount > 0)
					strcpy(pcResult, "");
			
				uiCharsCount = 0;

				return E_INVALIDARG;
			}
}

DGLE_RESULT DGLE_API CDCPFileSystem::ExecuteTextCommand(const char *pcCommand, TVariant &stVar)
{
	string cmd(pcCommand), param;

	const string::size_type pos = cmd.find_first_of(' ');
	
	if (pos != string::npos)
	{
		param = cmd.substr(pos + 1);
		cmd.erase(pos);
	}

	if (cmd == "create")
	{
		delete _pPackager;
		_pPackager = new CDCPPackager("");
		stVar.SetBool(true);

		return S_OK;
	}

	if (cmd == "close")
	{
		if (_pPackager)
		{
			delete _pPackager;
			_pPackager = NULL;
			stVar.SetBool(true);
		}
		else
			stVar.SetBool(false);

		return S_OK;
	}

	if (cmd == "open" && !param.empty())
	{
		delete _pPackager;
		_pPackager = new CDCPPackager(param);
		stVar.SetBool(_pPackager->IsOpened());

		return S_OK;
	}

	if (cmd == "save" && !param.empty())
	{
		stVar.SetBool(_pPackager->Save(param));

		return S_OK;
	}
	
	if (cmd == "add" && !param.empty())
	{
		const string::size_type pos = param.find_first_of(';');
		string dir;

		if (pos != string::npos)
		{
			dir = param.substr(pos + 1);
			param.erase(pos);
		}

		stVar.SetBool(_pPackager->AddFile(param, dir));

		return S_OK;
	}
	
	if (cmd == "extract" && !param.empty())
	{
		const string::size_type pos = param.find_first_of(';');

		if (pos == string::npos)
			return E_INVALIDARG;

		stVar.SetBool(_pPackager->ExtractFile(param.substr(0, pos), param.substr(pos + 1)));

		return S_OK;
	}
	
	if (cmd == "remove" && !param.empty())
	{
		stVar.SetBool(_pPackager->RemoveFile(param));

		return S_OK;
	}

	stVar.Clear();

	return E_INVALIDARG;
}

// CDCPFileIterator //

CDCPFileIterator::CDCPFileIterator(uint uiInstIdx, const std::vector<std::string> &clNameList):
CInstancedObj(uiInstIdx), _clNameList(clNameList)
{
	_clNameListIter = _clNameList.begin();
}

DGLE_RESULT DGLE_API CDCPFileIterator::FileName(char *pcName, DGLE::uint &uiCharsCount)
{
	if (_clNameListIter == _clNameList.end())
	{
		uiCharsCount = 0;
		return E_FAIL;
	}

	if (!pcName)
	{
		uiCharsCount = _clNameListIter->size() + 1;
		return S_OK;
	}

	if (_clNameListIter->size() >= uiCharsCount)
	{
		uiCharsCount = _clNameListIter->size() + 1;
		return E_INVALIDARG;
	}

	strcpy(pcName, _clNameListIter->c_str());

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFileIterator::Next()
{
	if (++_clNameListIter == _clNameList.end())
		return S_FALSE;

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFileIterator::Free()
{
	delete this;
	return S_OK;
}