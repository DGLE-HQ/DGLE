/**
\author		Korotkov Andrey aka DRON
\date		24.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Light.h"
#include "Render.h"
#include "Render3D.h"

CLight::CLight(uint uiInstIdx):
CInstancedObj(uiInstIdx), _bEnabled(false), _type(LT_SPOT),
_stMainCol(ColorWhite()), _stPos(TPoint3()), _stDir(TVector3(0.f, 0.f, 1.f)),
_fRange(100.f), _fIntense(1.f), _fAngle(45.f)
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
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetRange(float fRange)
{
	_fRange = fRange;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetIntensity(float fIntensity)
{
	_fIntense = fIntensity;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetSpotAngle(float fAngle)
{
	_fAngle = fAngle;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::SetType(E_LIGHT_TYPE eType)
{
	_type = eType;
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
	fIntensity = _fIntense;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetSpotAngle(float &fAngle)
{
	fAngle = _fAngle;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::GetType(E_LIGHT_TYPE &eType)
{
	eType = _type;
	return S_OK;
}

DGLE_RESULT DGLE_API CLight::Update()
{
	Core()->pRender()->pRender3D()->UpdateLight(this);
	return S_OK;
}