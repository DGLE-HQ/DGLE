/**
\author		Korotkov Andrey aka DRON
\date		20.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CHDDFileSystem final : public CInstancedObj, public IFileSystem
{
public:
	using CInstancedObj::CInstancedObj;

	DGLE_RESULT DGLE_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile) override;
	DGLE_RESULT DGLE_API DeleteFile(const char *pcName) override;
	DGLE_RESULT DGLE_API FileExists(const char *pcName, bool &bExists) override;
	DGLE_RESULT DGLE_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator) override;

	IDGLE_BASE_IMPLEMENTATION(IFileSystem, INTERFACE_IMPL_END)
};