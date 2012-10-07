/**
\author		Korotkov Andrey aka DRON
\date		10.12.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"
#include "HDFileSystem.h"
#include "DCPFileSystem.h"

struct TVFileSystem
{
	std::string ext;
	std::string discr;
	IFileSystem *fs;
	void *param;
	void (DGLE2_API *pdc)(void *pParametr, IFileSystem *pVFS);

	TVFileSystem(const char *pcExt, const char *pcDiscr, IFileSystem *pVFS, void (DGLE2_API *pDeleteDGLE2_API)(void *pParametr, IFileSystem *pVFS), void *pParametr)
	{
		ext		= ToUpperCase(std::string(pcExt));
		discr	= std::string(pcDiscr);
		fs		= pVFS;
		pdc		= pDeleteDGLE2_API;
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

	static void DGLE2_API _s_ConListVFS(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_FSDeleteDGLE2_API(void *pParametr, IFileSystem *pVFS);

public:
	
	CMainFS(uint uiInstIdx);
	~CMainFS();

	void UnregisterAndFreeAll();

	DGLE2_RESULT DGLE2_API LoadFile(const char* pcFileName, IFile *&prFile);
	DGLE2_RESULT DGLE2_API GetVirtualFileSystem(const char *pcVFSExtension, IFileSystem *&prVFS);
	DGLE2_RESULT DGLE2_API RegisterVirtualFileSystem(const char* pcVFSExtension, const char *pcDiscription, IFileSystem *pVFS, void (DGLE2_API *pDeleteDGLE2_API)(void *pParametr, IFileSystem *pVFS), void *pParametr);
	DGLE2_RESULT DGLE2_API UnregisterVirtualFileSystem(const char* pcVFSExtension);
	DGLE2_RESULT DGLE2_API GetRegisteredVirtualFileSystems(char* pcTxt, uint &uiCharsCount);
	DGLE2_RESULT DGLE2_API GetVirtualFileSystemDescription(const char* pcVFSExtension, char* pcTxt, uint &uiCharsCount);

	DGLE2_RESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE2_BASE_IMPLEMENTATION1(IMainFileSystem, IEngineSubSystem)
};