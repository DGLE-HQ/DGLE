/**
\author		Korotkov Andrey aka DRON
\date		23.11.2014 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "FixedFunctionPipelineGL.h"

CFixedFunctionPipeline::CFixedFunctionPipeline() : _c_fAttenuationFactor(1.75f), _bStateFilterEnabled(true)
{
	glGetIntegerv(GL_MAX_LIGHTS, &_iMaxLights);
	_pLights = new TLight[_iMaxLights];

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, TColor4(50, 50, 50, 255));

	for (int i = 0; i < _iMaxLights; ++i)
	{
		_pLights[i].type = LT_DIRECTIONAL;
		_pLights[i].mview = MatrixIdentity();

		glDisable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, ColorWhite());
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, ColorWhite());
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.f);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / 100.f);
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.f);

		const GLfloat pos[] = {0.f, 0.f, 1.f, 0.f};
		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
	}
}

CFixedFunctionPipeline::~CFixedFunctionPipeline()
{
	delete[] _pLights;
}

void CFixedFunctionPipeline::ToggleStateFilter(bool bEnabled)
{
	_bStateFilterEnabled = bEnabled;
}

void CFixedFunctionPipeline::InvalidateStateFilter()
{
	// for the future needs
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

DGLE_RESULT DGLE_API CFixedFunctionPipeline::SetLightIntensity(uint uiIdx, float fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	glLightf(GL_LIGHT0 + uiIdx, GL_CONSTANT_ATTENUATION, 1.f / fIntensity);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	const GLfloat pos[] = {stDirection.x, stDirection.y, stDirection.z, 0.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);
	
	_pLights[uiIdx].type = LT_DIRECTIONAL;
	glGetFloatv(GL_MODELVIEW_MATRIX, _pLights[uiIdx].mview._1D);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigurePointLight(uint uiIdx, const TPoint3 &stPosition, float fRange)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	const GLfloat pos[] = {stPosition.x, stPosition.y, stPosition.z, 1.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, 180.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / fRange);
	
	_pLights[uiIdx].type = LT_POINT;
	glGetFloatv(GL_MODELVIEW_MATRIX, _pLights[uiIdx].mview._1D);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureSpotLight(uint uiIdx, const TPoint3 &stPosition, const TVector3 &stDirection, float fRange, float fSpotAngle)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;

	const GLfloat pos[] = {stPosition.x, stPosition.y, stPosition.z, 1.f};
	glLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	
	glLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_DIRECTION, stDirection);
	glLightf(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, fSpotAngle / 2.f);
	glLightf(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, _c_fAttenuationFactor / fRange);

	_pLights[uiIdx].type = LT_SPOT;
	glGetFloatv(GL_MODELVIEW_MATRIX, _pLights[uiIdx].mview._1D);

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

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_DIFFUSE, stColor.rgba);

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightIntensity(uint uiIdx, float &fIntensity)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_CONSTANT_ATTENUATION, &fIntensity);
	fIntensity = 1.f / fIntensity;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetLightType(uint uiIdx, E_LIGHT_TYPE &eType)
{
	if (uiIdx >= (uint)_iMaxLights)
		return E_INVALIDARG;
	
	if (_bStateFilterEnabled)
		eType = _pLights[uiIdx].type;
	else
	{
		GLfloat pos[4];
		glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);

		if (pos[3] == 0.f)
			eType = LT_DIRECTIONAL;
		else
		{
			GLfloat cutoff;
			glGetLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, &cutoff);

			if (cutoff == 180.f)
				eType = LT_POINT;
			else
				eType = LT_SPOT;
		}
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection)
{
	E_LIGHT_TYPE type;
	GetLightType(uiIdx, type);

	if (uiIdx >= (uint)_iMaxLights || type != LT_DIRECTIONAL)
		return E_INVALIDARG;

	GLfloat dir[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, dir);

	// May work wrong when light position was set or changed directly via OpenGL outside this class.
	stDirection = MatrixInverse(_pLights[uiIdx].mview).ApplyToVector(TVector3(dir[0], dir[1], dir[2]));

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetPointLightConfiguration(uint uiIdx, TPoint3 &stPosition, float &fRange)
{
	E_LIGHT_TYPE type;
	GetLightType(uiIdx, type);

	if (uiIdx >= (uint)_iMaxLights || type != LT_POINT)
		return E_INVALIDARG;

	GLfloat pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);

	// May work wrong when light position was set or changed directly via OpenGL outside this class.
	stPosition = MatrixInverse(_pLights[uiIdx].mview).ApplyToPoint(TPoint3(pos[0], pos[1], pos[2]));
	
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, &fRange);
	fRange = _c_fAttenuationFactor / fRange;

	return S_OK;
}

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetSpotLightConfiguration(uint uiIdx, TPoint3 &stPosition, TVector3 &stDirection, float &fRange, float &fSpotAngle)
{
	E_LIGHT_TYPE type;
	GetLightType(uiIdx, type);

	if (uiIdx >= (uint)_iMaxLights || type != LT_SPOT)
		return E_INVALIDARG;

	GLfloat pos[4];
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_POSITION, pos);
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_DIRECTION, stDirection);

	// May work wrong when light position was set or changed directly via OpenGL outside this class.
	stPosition = MatrixInverse(_pLights[uiIdx].mview).ApplyToPoint(TPoint3(pos[0], pos[1], pos[2])); 
	stDirection = MatrixInverse(_pLights[uiIdx].mview).ApplyToVector(stDirection);
	
	glGetLightfv(GL_LIGHT0 + uiIdx, GL_SPOT_CUTOFF, &fSpotAngle);
	fSpotAngle *= 2.f;

	glGetLightfv(GL_LIGHT0 + uiIdx, GL_LINEAR_ATTENUATION, &fRange);
	fRange = _c_fAttenuationFactor / fRange;
	
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

DGLE_RESULT DGLE_API CFixedFunctionPipeline::ConfigureFog(float fStart, float fEnd)
{
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

DGLE_RESULT DGLE_API CFixedFunctionPipeline::GetFogConfiguration(float &fStart, float &fEnd)
{
	glGetFloatv(GL_FOG_START, &fStart);
	glGetFloatv(GL_FOG_END, &fEnd);

	return S_OK;
}