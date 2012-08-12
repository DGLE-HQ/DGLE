/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _HDFILESYSTEM_H
#define _HDFILESYSTEM_H

#include "..\Common.h"

class CHDFileIterator: public CInstancedObj, public IFileIterator
{
	char	_acName[MAX_PATH];
	HANDLE	_fileHandle;

public:
	
	CHDFileIterator(uint uiInstIdx, const char *pcName, HANDLE fileHandle);
	~CHDFileIterator();

	HRESULT CALLBACK FileName(char *pcName, uint uiCharsCount);
	HRESULT CALLBACK Next();
	HRESULT CALLBACK Free();

	IDGLE2_BASE_IMPLEMENTATION(IFileIterator)
};

class CHDFileSystem: public CInstancedObj, public IFileSystem
{
public:
	CHDFileSystem(uint uiInstIdx):CInstancedObj(uiInstIdx){}

	HRESULT CALLBACK OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile);	
	HRESULT CALLBACK DeleteFile(const char *pcName);
	HRESULT CALLBACK FileExists(const char *pcName, bool &bExists);
	HRESULT CALLBACK Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator);

	IDGLE2_BASE_IMPLEMENTATION(IFileSystem)
};

#endif	//_HDFILESYSTEM_H