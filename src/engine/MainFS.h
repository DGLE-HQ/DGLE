/**
\author		Korotkov Andrey aka DRON
\date		10.12.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _MAINFS_H
#define _MAINFS_H

#include "Common.h"
#include "HDFileSystem.h"
#include "DCPFileSystem.h"

struct TVFileSystem
{
	std::string ext;
	std::string discr;
	IFileSystem *fs;
	void *param;
	void (CALLBACK *pdc)(void *pParametr, IFileSystem *pVFS);

	TVFileSystem(const char *pcExt, const char *pcDiscr, IFileSystem *pVFS, void (CALLBACK *pDeleteCallback)(void *pParametr, IFileSystem *pVFS), void *pParametr)
	{
		ext		= ToUpperCase(std::string(pcExt));
		discr	= std::string(pcDiscr);
		fs		= pVFS;
		pdc		= pDeleteCallback;
		param	= pParametr;
	}
};

class CDCPFileSystem;

class CMainFS: public CInstancedObj, public IMainFileSystem
{
	CHDFileSystem		*_pHddFS;
	CDCPFileSystem      *_pDCPFS;

	std::vector<TVFileSystem> _clVFileSystems;
	std::string _strVFSsDescs;

	static void CALLBACK _s_ConListVFS(void *pParametr, const char *pcParam);
	static void CALLBACK _s_FSDeleteCallback(void *pParametr, IFileSystem *pVFS);

public:
	
	CMainFS(uint uiInstIdx);
	~CMainFS();

	void UnregisterAndFreeAll();

	HRESULT CALLBACK LoadFile(const char* pcFileName, IFile *&prFile);
	HRESULT CALLBACK GetVirtualFileSystem(const char *pcVFSExtension, IFileSystem *&prVFS);
	HRESULT CALLBACK RegisterVirtualFileSystem(const char* pcVFSExtension, const char *pcDiscription, IFileSystem *pVFS, void (CALLBACK *pDeleteCallback)(void *pParametr, IFileSystem *pVFS), void *pParametr);
	HRESULT CALLBACK UnregisterVirtualFileSystem(const char* pcVFSExtension);
	HRESULT CALLBACK GetRegisteredVirtualFileSystems(char* pcTxt, uint uiCharsCount);
	HRESULT CALLBACK GetVirtualFileSystemDescription(const char* pcVFSExtension, char* pcTxt, uint uiCharsCount);

	HRESULT CALLBACK GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE2_BASE_IMPLEMENTATION1(IMainFileSystem, IEngineSubSystem)
};

#endif	//_MAINFS_H