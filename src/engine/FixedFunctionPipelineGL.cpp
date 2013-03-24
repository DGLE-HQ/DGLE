/**
\author		Korotkov Andrey aka DRON
\date		23.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "FixedFunctionPipelineGL.h"

CFixedFunctionPipeline::CFixedFunctionPipeline()
{
	glGetIntegerv(GL_MAX_LIGHTS, &_iMaxLights);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ColorGray());
	glFogi(GL_FOG_MODE, GL_LINEAR);

	_clLights.resize(_iMaxLights);

	for (size_t i = 0; i < (uint)_iMaxLights; ++i)
	{
		_clLights[i] = LT_DIRECTIONAL;

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, ColorWhite());
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, ColorWhite());

		const float pos[] = {0.f, 0.f, 1.f, 0.f};
		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
	}
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
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetMaterialSpecularColor(const TColor4 &stColor)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetMaterialShininess(float fShininess)
{
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, fShininess);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialDiffuseColor(TColor4 &stColor)
{
	glGetMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialSpecularColor(TColor4 &stColor)
{
	glGetMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetMaterialShininess(float &fShininess)
{
	glGetMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &fShininess);
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

	if (bEnabled)
		glEnable(GL_LIGHT0 + uiIdx);
	else
		glDisable(GL_LIGHT0 + uiIdx);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightColor(uint uiIdx, const TColor4 &stColor)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	glLightfv(GL_LIGHT0 + uiIdx, GL_DIFFUSE, stColor);
	glLightfv(GL_LIGHT0 + uiIdx, GL_SPECULAR, stColor);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightPosition(uint uiIdx, const TPoint3 &stPosition)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	const float pos[] = {stPosition.x, stPosition.y, stPosition.z, _clLights[uiIdx] == LT_DIRECTIONAL ? 0.f : 1.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightIntensity(uint uiIdx, float fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	glLightf(GL_LIGHT0 + uiIdx, GL_CONSTANT_ATTENUATION, fIntensity);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	_clLights[uiIdx] = LT_DIRECTIONAL;

	const float pos[] = {stDirection.x, stDirection.y, stDirection.z, 0.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigurePointLight(uint uiIdx, float fRange)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	_clLights[uiIdx] = LT_POINT;

	float pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	pos[3] = 1.f;
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, fRange);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureSpotLight(uint uiIdx, const TVector3 &stDirection, float fRange, float fSpotAngle)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	_clLights[uiIdx] = LT_SPOT;

	float pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	pos[3] = 1.f;
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	
	glLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_DIRECTION, stDirection);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, fSpotAngle * 2.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, fRange);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightEnabled(uint uiIdx, bool &bEnabled)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	bEnabled = glIsEnabled(GL_LIGHT0 + uiIdx) == GL_TRUE;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightColor(uint uiIdx, TColor4 &stColor)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_DIFFUSE, stColor);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightPosition(uint uiIdx, TPoint3 &stPosition)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	float pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	stPosition = pos;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightIntensity(uint uiIdx, float &fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_CONSTANT_ATTENUATION, &fIntensity);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightType(uint uiIdx, E_LIGHT_TYPE &eType)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	eType = _clLights[uiIdx];

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection)
{
	if (uiIdx >= (uint)_iMaxLights || _clLights[uiIdx] != LT_DIRECTIONAL)
		return E_INVALIDARG;

	float pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	stDirection = pos;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetPointLightConfiguration(uint uiIdx, float &fRange)
{
	if (uiIdx >= (uint)_iMaxLights || _clLights[uiIdx] != LT_POINT)
		return E_INVALIDARG;

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, &fRange);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetSpotLightConfiguration(uint uiIdx, TVector3 &stDirection, float &fRange, float &fSpotAngle)
{
	if (uiIdx >= (uint)_iMaxLights || _clLights[uiIdx] != LT_SPOT)
		return E_INVALIDARG;

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_DIRECTION, stDirection);
	
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, &fSpotAngle);
	fSpotAngle *= 2.f;

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, &fRange);

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

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GonfigureFog(float &fStart, float &fEnd, float &fDensity)
{
	glGetFloatv(GL_FOG_DENSITY, &fDensity);
	glGetFloatv(GL_FOG_START, &fStart);
	glGetFloatv(GL_FOG_END, &fEnd);

	return S_OK;
}