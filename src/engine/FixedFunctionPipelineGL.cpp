/**
\author		Korotkov Andrey aka DRON
\date		26.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "FixedFunctionPipelineGL.h"

CFixedFunctionPipeline::CFixedFunctionPipeline() : _c_fAttenuationFactor(1.75f)
{
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, TColor4(50, 50, 50, 255));

	glGetIntegerv(GL_MAX_LIGHTS, &_iMaxLights);
	_pLights = new TLight[_iMaxLights];

	for (int i = 0; i < _iMaxLights; ++i)
	{
		_pLights[i].bEnabled = false;
		_pLights[i].eType = LT_DIRECTIONAL;
		_pLights[i].stDiffCol = ColorWhite();
		_pLights[i].stPos = TPoint3();
		_pLights[i].stDir = TVector3(0.f, 0.f, 1.f);
		_pLights[i].fRange = 100.f;
		_pLights[i].fIntensity = 1.f;
		_pLights[i].fAngle = 360.f;

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, _pLights[i].stDiffCol);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, _pLights[i].stDiffCol);
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.f / _pLights[i].fIntensity);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / _pLights[i].fRange);
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.f);

		const float pos[] = {_pLights[i].stDir.x, _pLights[i].stDir.y, _pLights[i].stDir.z, 0.f};
		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
	}
}

CFixedFunctionPipeline::~CFixedFunctionPipeline()
{
	delete[] _pLights;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::PushStates()
{
	glPushAttrib(GL_FOG_BIT | GL_LIGHTING_BIT);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::PopStates()
{
	glPopAttrib();
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetMaterialDiffuseColor(const TColor4 &stColor)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetMaterialSpecularColor(const TColor4 &stColor)
{
	glMaterialfv(GL_FRONT, GL_SPECULAR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetMaterialShininess(float fShininess)
{
	glMaterialf(GL_FRONT, GL_SHININESS, (128.f / 100.f) * (100.f - fShininess));
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialDiffuseColor(TColor4 &stColor)
{
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialSpecularColor(TColor4 &stColor)
{
	glGetMaterialfv(GL_FRONT, GL_SPECULAR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialShininess(float &fShininess)
{
	glGetMaterialfv(GL_FRONT, GL_SHININESS, &fShininess);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ToggleGlobalLighting(bool bEnabled)
{
	if (bEnabled)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetGloablAmbientLight(const TColor4 &stColor)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaxLightsPerPassCount(uint &uiCount)
{
	uiCount = (uint)_iMaxLights;
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::IsGlobalLightingEnabled(bool &bEnabled)
{
	bEnabled = glIsEnabled(GL_LIGHTING) == GL_TRUE;
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetGloablAmbientLight(TColor4 &stColor)
{
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightEnabled(uint uiIdx, bool bEnabled)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	if (bEnabled == _pLights[uiIdx].bEnabled)
		return S_OK;

	if (bEnabled)
		glEnable(GL_LIGHT0 + uiIdx);
	else
		glDisable(GL_LIGHT0 + uiIdx);
	
	_pLights[uiIdx].bEnabled = bEnabled;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightColor(uint uiIdx, const TColor4 &stColor)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	if (stColor == _pLights[uiIdx].stDiffCol)
		return S_OK;
	
	glLightfv(GL_LIGHT0 + uiIdx, GL_DIFFUSE, stColor);
	glLightfv(GL_LIGHT0 + uiIdx, GL_SPECULAR, stColor);

	_pLights[uiIdx].stDiffCol = stColor;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightPosition(uint uiIdx, const TPoint3 &stPosition)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	if (stPosition == _pLights[uiIdx].stPos)
		return S_OK;
	
	if (_pLights[uiIdx].eType != LT_DIRECTIONAL)
	{
		const float pos[] = {stPosition.x, stPosition.y, stPosition.z, 1.f};
		glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	}

	_pLights[uiIdx].stPos = stPosition;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightIntensity(uint uiIdx, float fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	if (fIntensity == _pLights[uiIdx].fIntensity)
		return S_OK;

	glLightf(GL_LIGHT0 + uiIdx, GL_CONSTANT_ATTENUATION, 1.f / fIntensity);

	_pLights[uiIdx].fIntensity = fIntensity;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	if (LT_DIRECTIONAL == _pLights[uiIdx].eType && stDirection == _pLights[uiIdx].stDir)
		return S_OK;

	const float pos[] = {stDirection.x, stDirection.y, stDirection.z, 0.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);
	
	_pLights[uiIdx].eType = LT_DIRECTIONAL;
	_pLights[uiIdx].stDir = stDirection;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigurePointLight(uint uiIdx, float fRange)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	if (LT_POINT == _pLights[uiIdx].eType && fRange == _pLights[uiIdx].fRange)
		return S_OK;
	
	const float pos[] = {_pLights[uiIdx].stPos.x, _pLights[uiIdx].stPos.y, _pLights[uiIdx].stPos.z, 1.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / fRange);

	_pLights[uiIdx].eType = LT_POINT;
	_pLights[uiIdx].fRange = fRange;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureSpotLight(uint uiIdx, const TVector3 &stDirection, float fRange, float fSpotAngle)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	if (LT_SPOT == _pLights[uiIdx].eType && stDirection == _pLights[uiIdx].stDir &&
		fRange == _pLights[uiIdx].fRange && fSpotAngle == _pLights[uiIdx].fAngle)
		return S_OK;

	const float pos[] = {_pLights[uiIdx].stPos.x, _pLights[uiIdx].stPos.y, _pLights[uiIdx].stPos.z, 1.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	
	glLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_DIRECTION, stDirection);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, fSpotAngle / 2.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / fRange);

	_pLights[uiIdx].eType = LT_SPOT;
	_pLights[uiIdx].fRange = fRange;
	_pLights[uiIdx].fAngle = fSpotAngle;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightEnabled(uint uiIdx, bool &bEnabled)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	bEnabled = _pLights[uiIdx].bEnabled;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightColor(uint uiIdx, TColor4 &stColor)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	stColor = _pLights[uiIdx].stDiffCol;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightPosition(uint uiIdx, TPoint3 &stPosition)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	stPosition = _pLights[uiIdx].stPos;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightIntensity(uint uiIdx, float &fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	fIntensity = _pLights[uiIdx].fIntensity;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightType(uint uiIdx, E_LIGHT_TYPE &eType)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	eType = _pLights[uiIdx].eType;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection)
{
	if (uiIdx >= (uint)_iMaxLights || _pLights[uiIdx].eType != LT_DIRECTIONAL)
		return E_INVALIDARG;

	stDirection = _pLights[uiIdx].stDir;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetPointLightConfiguration(uint uiIdx, float &fRange)
{
	if (uiIdx >= (uint)_iMaxLights || _pLights[uiIdx].eType != LT_POINT)
		return E_INVALIDARG;

	fRange = _pLights[uiIdx].fRange;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetSpotLightConfiguration(uint uiIdx, TVector3 &stDirection, float &fRange, float &fSpotAngle)
{
	if (uiIdx >= (uint)_iMaxLights || _pLights[uiIdx].eType != LT_SPOT)
		return E_INVALIDARG;

	stDirection = _pLights[uiIdx].stDir;
	fRange = _pLights[uiIdx].fRange;
	fSpotAngle = _pLights[uiIdx].fAngle;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetFogEnabled(bool bEnabled)
{
	if (bEnabled)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetFogColor(const TColor4 &stColor)
{
	glFogfv(GL_FOG_COLOR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureFog(float fStart, float fEnd, float fDensity)
{
	glFogf(GL_FOG_DENSITY, fDensity);
	glFogf(GL_FOG_START, fStart);
	glFogf(GL_FOG_END, fEnd);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetFogEnabled(bool &bEnabled)
{
	bEnabled = glIsEnabled(GL_FOG) == GL_TRUE;
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetFogColor(TColor4 &stColor)
{
	glGetFloatv(GL_FOG_COLOR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetFogConfiguration(float &fStart, float &fEnd, float &fDensity)
{
	glGetFloatv(GL_FOG_DENSITY, &fDensity);
	glGetFloatv(GL_FOG_START, &fStart);
	glGetFloatv(GL_FOG_END, &fEnd);

	return S_OK;
}