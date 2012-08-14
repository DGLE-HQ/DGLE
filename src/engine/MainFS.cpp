/**
\author		Korotkov Andrey aka DRON
\date		10.12.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "MainFS.h"

using namespace std;

CMainFS::CMainFS(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{
	Console()->RegComProc("mfs_list_reged_vfs", "Lists all registered virtual file systems.", &_s_ConListVFS, this);

	_pHddFS = new CHDFileSystem(InstIdx());
	_pDCPFS = new CDCPFileSystem(InstIdx(), this);

	RegisterVirtualFileSystem("", "base Hard Disc Drive(HDD) file system.", (IFileSystem*)_pHddFS, &_s_FSDeleteCallback, this);
	RegisterVirtualFileSystem("dcp","Dgle Compiled Package(DCP) file archives.",(IFileSystem*)_pDCPFS, &_s_FSDeleteCallback, this);
	
	LOG("Filesystem Subsystem initialized.", LT_INFO);
}

CMainFS::~CMainFS()
{
	LOG("Filesystem Subsystem finalized.", LT_INFO);
}

void CMainFS::UnregisterAndFreeAll()
{
	for (size_t i = 0; i<_clVFileSystems.size(); ++i)
		_clVFileSystems[i].pdc(_clVFileSystems[i].param, _clVFileSystems[i].fs);

	_clVFileSystems.clear();
	_strVFSsDescs.clear();
}

HRESULT CALLBACK CMainFS::UnregisterVirtualFileSystem(const char* pcVFSExtension)
{
	for (size_t i = 0; i<_clVFileSystems.size(); ++i)
		if (_clVFileSystems[i].ext == ToUpperCase(string(pcVFSExtension)))
		{
			_clVFileSystems[i].pdc(_clVFileSystems[i].param, _clVFileSystems[i].fs);
			_clVFileSystems.erase(_clVFileSystems.begin()+i);
			return S_OK;
		}

	return E_INVALIDARG;
}

HRESULT CALLBACK CMainFS::GetRegisteredVirtualFileSystems(char* pcTxt, uint uiCharsCount)
{
	string exts;

	if (_clVFileSystems.size() > 1)
		for (size_t i = 1; i<_clVFileSystems.size(); ++i)
			exts += _clVFileSystems[i].ext + ";";

	if (exts.size() > uiCharsCount)
	{
		LOG("Too small \"pcTxt\" buffer size.", LT_ERROR);
		strcpy(pcTxt, "");
		return E_INVALIDARG;
	}

	strcpy(pcTxt, exts.c_str());

	return S_OK;
}

void CALLBACK CMainFS::_s_ConListVFS(void *pParametr, const char *pcParam)
{
	if (strlen(pcParam) != 0)
		CON(CMainFS, "No parametrs expected.");
	else 
		CON(CMainFS, string("---Supported VFile Systems---\r\n" + PTHIS(CMainFS)->_strVFSsDescs + "-----------------------------").c_str());
}

void CALLBACK CMainFS::_s_FSDeleteCallback(void *pParametr, IFileSystem *pVFS)
{
	if (PTHIS(CMainFS)->_pHddFS == pVFS)
		delete PTHIS(CMainFS)->_pHddFS;

	if (PTHIS(CMainFS)->_pDCPFS == pVFS)
		delete PTHIS(CMainFS)->_pDCPFS;
}

HRESULT CALLBACK CMainFS::LoadFile(const char* pcFileName, IFile *&prFile)
{
	prFile = NULL;

	IFileSystem *p_fs = NULL;

	if (string(pcFileName).find("|") == string::npos)
		p_fs = (IFileSystem*)_pHddFS;
	else
		for (size_t i = 0; i<_clVFileSystems.size(); ++i)
			if ((ToUpperCase(string(pcFileName)).find("." + ToUpperCase(_clVFileSystems[i].ext) + "|") != string::npos))
			{
				p_fs = _clVFileSystems[i].fs;
				break;
			}

	if (!p_fs)
	{
		LOG("Failed to find virtual file system to open file \"" + string(pcFileName) + "\".",LT_ERROR);
		return E_INVALIDARG;
	}

	bool exists = false;	
	p_fs->FileExists(pcFileName, exists);

	if (!exists)
	{
		LOG("File \"" + string(pcFileName) + "\" doesn't exists.", LT_WARNING);
		return S_FALSE;
	}

	return p_fs->OpenFile(pcFileName, (E_FILE_SYSTEM_OPEN_FLAGS)(FSOF_READ | FSOF_BINARY), prFile);
}

HRESULT CALLBACK CMainFS::GetVirtualFileSystem(const char *pcVFSExtension, IFileSystem *&prVFS)
{
	if (pcVFSExtension == NULL)
	{
		prVFS = _clVFileSystems[0].fs;
		return S_OK;
	}

	for (size_t i = 0; i<_clVFileSystems.size(); ++i)
		if (_clVFileSystems[i].ext == ToUpperCase(string(pcVFSExtension)))
		{
			prVFS = _clVFileSystems[i].fs;
			return S_OK;
		}

	LOG("Virtual file system with extension \"" + string(pcVFSExtension) + "\" hasn't been registered.", LT_ERROR);

	return E_INVALIDARG;
}

HRESULT CALLBACK CMainFS::GetVirtualFileSystemDescription(const char* pcVFSExtension, char* pcTxt, uint uiCharsCount)
{
	for (size_t i = 0; i<_clVFileSystems.size(); ++i)
		if (_clVFileSystems[i].ext == ToUpperCase(string(pcVFSExtension)))
		{
			if (_clVFileSystems[i].discr.size() > uiCharsCount)
			{
				LOG("Too small \"pcTxt\" buffer size.", LT_ERROR);
				return E_INVALIDARG;
			}

			strcpy(pcTxt, _clVFileSystems[i].discr.c_str());

			return S_OK;
		}

	strcpy(pcTxt, "");

	return S_FALSE;
}

HRESULT CALLBACK CMainFS::RegisterVirtualFileSystem(const char* pcVFSExtension, const char *pcDiscription, IFileSystem *pVFS, void (CALLBACK *pDeleteCallback)(void *pParametr, IFileSystem *pVFS), void *pParametr)
{
	for (size_t i = 0; i<_clVFileSystems.size(); ++i)
		if (_clVFileSystems[i].ext == string(pcVFSExtension))
		{
			LOG("Virtual file system with extension \"" + string(pcVFSExtension) + "\" already registered.", LT_WARNING);
			return E_INVALIDARG;
		}
	
	_clVFileSystems.push_back(TVFileSystem(pcVFSExtension, pcDiscription, pVFS, pDeleteCallback, pParametr));

	_strVFSsDescs += string("- " + ToUpperCase(string(pcVFSExtension)) + " " + string(pcDiscription) + "\r\n");

	return S_OK;
}

HRESULT CALLBACK CMainFS::GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType)
{
	eSubSystemType = ESS_FILE_SYSTEM;
	return S_OK;
}