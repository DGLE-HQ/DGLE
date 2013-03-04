/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Light.h"

CLight::CLight(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

DGLE_RESULT DGLE_API CLight::SetEnabled(bool bEnabled)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetDiffuseColor(const TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetSpecularColor(const TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetPosition(const TPoint3 &stPos)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetDirection(const TVector3 &stDir)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetDistance(float fDist)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetAttenuation(float fCoeff)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetSpotCutoff(float fDist)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetType(E_LIGHT_TYPE eType)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetEnabled(bool &bEnabled)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetDiffuseColor(TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetSpecularColor(TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetPosition(TPoint3 &stPos)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetDirection(TVector3 &stDir)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetDistance(float &fDist)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetAttenuation(float &fCoeff)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetSpotCutoff(float &fDist)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetType(E_LIGHT_TYPE &eType)
{
	return S_OK;
}