/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _HDFILE_H
#define _HDFILE_H

#include "..\Common.h"

class CInstancedObj;

class CHDFile: public CInstancedObj, public IFile
{
	int		_iFile;
	char	_acName[MAX_PATH],
			_acPath[MAX_PATH];

public:

	CHDFile(uint uiInstIdx, const char* pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags = FSOF_READ);
	~CHDFile();

	HRESULT CALLBACK Read(void *pBuffer, uint uiCount, uint &uiRead);
	HRESULT CALLBACK Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	HRESULT CALLBACK Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	HRESULT CALLBACK GetSize(uint32 &ui32Size);
	HRESULT CALLBACK IsOpen(bool &bOpened);
	HRESULT CALLBACK GetName(char *pcName, uint uiCharsCount);
	HRESULT CALLBACK GetPath(char *pcPath, uint uiCharsCount);
	HRESULT CALLBACK Free();

	IDGLE2_BASE_IMPLEMENTATION(IFile)
};

#endif	//_HDFILE_H