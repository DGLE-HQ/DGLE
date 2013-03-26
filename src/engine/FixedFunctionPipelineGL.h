/**
\author		Korotkov Andrey aka DRON
\date		26.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CFixedFunctionPipeline : public IFixedFunctionPipeline
{
	const float _c_fAttenuationFactor;

	GLint _iMaxLights;

	struct TLight
	{
		bool bEnabled;
		E_LIGHT_TYPE eType;
		TColor4 stDiffCol;
		TPoint3 stPos;
		TVector3 stDir;
		float fRange, fIntensity, fAngle;
	};

	TLight *_pLights;

public:

	CFixedFunctionPipeline();
	~CFixedFunctionPipeline();

	DGLE_RESULT DGLE_API PushStates();
	DGLE_RESULT DGLE_API PopStates();

	DGLE_RESULT DGLE_API SetMaterialDiffuseColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetMaterialSpecularColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetMaterialShininess(float fShininess);

	DGLE_RESULT DGLE_API GetMaterialDiffuseColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetMaterialSpecularColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetMaterialShininess(float &fShininess);

	DGLE_RESULT DGLE_API ToggleGlobalLighting(bool bEnabled);
	DGLE_RESULT DGLE_API SetGloablAmbientLight(const TColor4 &stColor);

	DGLE_RESULT DGLE_API GetMaxLightsPerPassCount(uint &uiCount);
	DGLE_RESULT DGLE_API IsGlobalLightingEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetGloablAmbientLight(TColor4 &stColor);

	DGLE_RESULT DGLE_API SetLightEnabled(uint uiIdx, bool bEnabled);
	DGLE_RESULT DGLE_API SetLightColor(uint uiIdx, const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetLightPosition(uint uiIdx, const TPoint3 &stPosition);
	DGLE_RESULT DGLE_API SetLightIntensity(uint uiIdx, float fIntensity);
	DGLE_RESULT DGLE_API ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection);
	DGLE_RESULT DGLE_API ConfigurePointLight(uint uiIdx, float fRange);
	DGLE_RESULT DGLE_API ConfigureSpotLight(uint uiIdx, const TVector3 &stDirection, float fRange, float fSpotAngle);

	DGLE_RESULT DGLE_API GetLightEnabled(uint uiIdx, bool &bEnabled);
	DGLE_RESULT DGLE_API GetLightColor(uint uiIdx, TColor4 &stColor);
	DGLE_RESULT DGLE_API GetLightPosition(uint uiIdx, TPoint3 &stPosition);
	DGLE_RESULT DGLE_API GetLightIntensity(uint uiIdx, float &fIntensity);
	DGLE_RESULT DGLE_API GetLightType(uint uiIdx, E_LIGHT_TYPE &eType);
	DGLE_RESULT DGLE_API GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection);
	DGLE_RESULT DGLE_API GetPointLightConfiguration(uint uiIdx, float &fRange);
	DGLE_RESULT DGLE_API GetSpotLightConfiguration(uint uiIdx, TVector3 &stDirection, float &fRange, float &fSpotAngle);
		
	DGLE_RESULT DGLE_API SetFogEnabled(bool bEnabled);
	DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API ConfigureFog(float fStart, float fEnd, float fDensity);

	DGLE_RESULT DGLE_API GetFogEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetFogConfiguration(float &fStart, float &fEnd, float &fDensity);

	IDGLE_BASE_IMPLEMENTATION(IFixedFunctionPipeline, INTERFACE_IMPL_END)
};