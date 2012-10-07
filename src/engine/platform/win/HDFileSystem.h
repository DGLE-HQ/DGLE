/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\Common.h"

class CHDFileIterator: public CInstancedObj, public IFileIterator
{
	char	_acName[MAX_PATH];
	HANDLE	_fileHandle;

public:
	
	CHDFileIterator(uint uiInstIdx, const char *pcName, HANDLE fileHandle);
	~CHDFileIterator();

	DGLE2_RESULT DGLE2_API FileName(char *pcName, uint &uiCharsCount);
	DGLE2_RESULT DGLE2_API Next();
	DGLE2_RESULT DGLE2_API Free();

	IDGLE2_BASE_IMPLEMENTATION(IFileIterator)
};

class CHDFileSystem: public CInstancedObj, public IFileSystem
{
public:
	CHDFileSystem(uint uiInstIdx):CInstancedObj(uiInstIdx){}

	DGLE2_RESULT DGLE2_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile);	
	DGLE2_RESULT DGLE2_API DeleteFile(const char *pcName);
	DGLE2_RESULT DGLE2_API FileExists(const char *pcName, bool &bExists);
	DGLE2_RESULT DGLE2_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator);
	DGLE2_RESULT DGLE2_API SendCommand(const char *pcCommand, char *pcResult, uint &uiCharsCount);

	IDGLE2_BASE_IMPLEMENTATION(IFileSystem)
};