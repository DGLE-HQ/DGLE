/**
\author		Korotkov Andrey aka DRON
\date		26.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Light.h"
#include "Render.h"
#include "Render3D.h"

CLight::CLight(uint uiInstIdx):
CInstancedObj(uiInstIdx), _bEnabled(true), _eType(LT_DIRECTIONAL),
_stMainCol(ColorWhite()), _stPos(TPoint3()), _stDir(TVector3(0.f, 0.f, 1.f)),
_fRange(100.f), _fIntensity(1.f), _fAngle(90.f)
{}

DGLE_RESULT DGLE_API CLight::SetEnabled(bool bEnabled)
{
	_bEnabled = bEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetColor(const TColor4 &stColor)
{
	_stMainCol = stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetPosition(const TPoint3 &stPos)
{
	_stPos = stPos;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetDirection(const TVector3 &stDir)
{
	_stDir = stDir;
	_stDir.Normalize();

	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetRange(float fRange)
{
	if (fRange < 0.f)
		return E_INVALIDARG;
	
	_fRange = fRange;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetIntensity(float fIntensity)
{
	if (fIntensity > 2.f || fIntensity < 0.f)
		return E_INVALIDARG;

	_fIntensity = fIntensity;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetSpotAngle(float fAngle)
{
	if (fAngle > 180.f || fAngle < 0.f)
		return E_INVALIDARG;

	_fAngle = fAngle;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetType(E_LIGHT_TYPE eType)
{
	_eType = eType;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetEnabled(bool &bEnabled)
{
	bEnabled = _bEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetColor(TColor4 &stColor)
{
	stColor = _stMainCol;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetPosition(TPoint3 &stPos)
{
	stPos = _stPos;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetDirection(TVector3 &stDir)
{
	stDir = _stDir;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetRange(float &fRange)
{
	fRange = _fRange;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetIntensity(float &fIntensity)
{
	fIntensity = _fIntensity;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetSpotAngle(float &fAngle)
{
	fAngle = _fAngle;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetType(E_LIGHT_TYPE &eType)
{
	eType = _eType;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::Update()
{
	Core()->pRender()->pRender3D()->UpdateLight(this);
	return S_OK;
}