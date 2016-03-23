/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CLight final : public CInstancedObj, public ILight
{
	bool _bEnabled;
	E_LIGHT_TYPE _eType;
	TColor4 _stMainCol;
	TPoint3 _stPos;
	TVector3 _stDir;
	float _fRange, _fIntensity, _fAngle;

public:

	CLight(uint uiInstIdx);

	DGLE_RESULT DGLE_API SetEnabled(bool bEnabled) override;
	DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API SetPosition(const TPoint3 &stPos) override;
	DGLE_RESULT DGLE_API SetDirection(const TVector3 &stDir) override;
	DGLE_RESULT DGLE_API SetRange(float fRange) override;
	DGLE_RESULT DGLE_API SetIntensity(float fIntensity) override;
	DGLE_RESULT DGLE_API SetSpotAngle(float fAngle) override;
	DGLE_RESULT DGLE_API SetType(E_LIGHT_TYPE eType) override;
		
	DGLE_RESULT DGLE_API GetEnabled(bool &bEnabled) override;
	DGLE_RESULT DGLE_API GetColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetPosition(TPoint3 &stPos) override;
	DGLE_RESULT DGLE_API GetDirection(TVector3 &stDir) override;
	DGLE_RESULT DGLE_API GetRange(float &fRange) override;
	DGLE_RESULT DGLE_API GetIntensity(float &fIntensity) override;
	DGLE_RESULT DGLE_API GetSpotAngle(float &fAngle) override;
	DGLE_RESULT DGLE_API GetType(E_LIGHT_TYPE &eType) override;

	DGLE_RESULT DGLE_API Update() override;

	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_LIGHT)
	IDGLE_BASE_IMPLEMENTATION(ILight, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};