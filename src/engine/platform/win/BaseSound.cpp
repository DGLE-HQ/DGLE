/**
\author		Andrey Korotkov aka DRON
\date		23.09.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "BaseSound.h"

using namespace std;

#ifndef NO_BUILTIN_SOUND

CBaseSound::CBaseSound(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

CBaseSound::~CBaseSound()
{}

#endif