/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CLight: public CInstancedObj, public ILight
{
public:

	CLight(uint uiInstIdx);

	DGLE_RESULT DGLE_API SetEnabled(bool bEnabled);
	DGLE_RESULT DGLE_API SetDiffuseColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetSpecularColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetPosition(const TPoint3 &stPos);
	DGLE_RESULT DGLE_API SetDirection(const TVector3 &stDir);
	DGLE_RESULT DGLE_API SetDistance(float fDist);
	DGLE_RESULT DGLE_API SetAttenuation(float fCoeff);
	DGLE_RESULT DGLE_API SetSpotCutoff(float fDist);
	DGLE_RESULT DGLE_API SetType(E_LIGHT_TYPE eType);
		
	DGLE_RESULT DGLE_API GetEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetDiffuseColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetSpecularColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetPosition(TPoint3 &stPos);
	DGLE_RESULT DGLE_API GetDirection(TVector3 &stDir);
	DGLE_RESULT DGLE_API GetDistance(float &fDist);
	DGLE_RESULT DGLE_API GetAttenuation(float &fCoeff);
	DGLE_RESULT DGLE_API GetSpotCutoff(float &fDist);
	DGLE_RESULT DGLE_API GetType(E_LIGHT_TYPE &eType);

	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_LIGHT)
	IDGLE_BASE_IMPLEMENTATION(ILight, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};