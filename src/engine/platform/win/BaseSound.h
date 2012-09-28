/**
\author		Andrey Korotkov aka DRON
\date		23.09.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "..\..\Common.h"

#ifndef NO_BUILTIN_SOUND

class CBaseSound : public CInstancedObj, public IBaseSound
{

public:

	CBaseSound(uint uiInstIdx);
	~CBaseSound();

};

#endif