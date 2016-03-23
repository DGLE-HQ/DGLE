/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

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
		E_LIGHT_TYPE type;
		TMatrix4 mview;
	};

	TLight *_pLights;

	bool _bStateFilterEnabled;

public:

	CFixedFunctionPipeline();
	~CFixedFunctionPipeline();

	void ToggleStateFilter(bool bEnabled);
	void InvalidateStateFilter();

	DGLE_RESULT DGLE_API PushStates() override;
	DGLE_RESULT DGLE_API PopStates() override;

	DGLE_RESULT DGLE_API SetMaterialDiffuseColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API SetMaterialSpecularColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API SetMaterialShininess(float fShininess) override;

	DGLE_RESULT DGLE_API GetMaterialDiffuseColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetMaterialSpecularColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetMaterialShininess(float &fShininess) override;

	DGLE_RESULT DGLE_API ToggleGlobalLighting(bool bEnabled) override;
	DGLE_RESULT DGLE_API SetGloablAmbientLight(const TColor4 &stColor) override;

	DGLE_RESULT DGLE_API GetMaxLightsPerPassCount(uint &uiCount) override;
	DGLE_RESULT DGLE_API IsGlobalLightingEnabled(bool &bEnabled) override;
	DGLE_RESULT DGLE_API GetGloablAmbientLight(TColor4 &stColor) override;

	DGLE_RESULT DGLE_API SetLightEnabled(uint uiIdx, bool bEnabled) override;
	DGLE_RESULT DGLE_API SetLightColor(uint uiIdx, const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API SetLightIntensity(uint uiIdx, float fIntensity) override;
	DGLE_RESULT DGLE_API ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection) override;
	DGLE_RESULT DGLE_API ConfigurePointLight(uint uiIdx, const TPoint3 &stPosition, float fRange) override;
	DGLE_RESULT DGLE_API ConfigureSpotLight(uint uiIdx, const TPoint3 &stPosition, const TVector3 &stDirection, float fRange, float fSpotAngle) override;

	DGLE_RESULT DGLE_API GetLightEnabled(uint uiIdx, bool &bEnabled) override;
	DGLE_RESULT DGLE_API GetLightColor(uint uiIdx, TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetLightIntensity(uint uiIdx, float &fIntensity) override;
	DGLE_RESULT DGLE_API GetLightType(uint uiIdx, E_LIGHT_TYPE &eType) override;
	DGLE_RESULT DGLE_API GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection) override;
	DGLE_RESULT DGLE_API GetPointLightConfiguration(uint uiIdx, TPoint3 &stPosition, float &fRange) override;
	DGLE_RESULT DGLE_API GetSpotLightConfiguration(uint uiIdx, TPoint3 &stPosition, TVector3 &stDirection, float &fRange, float &fSpotAngle) override;
		
	DGLE_RESULT DGLE_API SetFogEnabled(bool bEnabled) override;
	DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API ConfigureFog(float fStart, float fEnd) override;

	DGLE_RESULT DGLE_API GetFogEnabled(bool &bEnabled) override;
	DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetFogConfiguration(float &fStart, float &fEnd) override;

	IDGLE_BASE_IMPLEMENTATION(IFixedFunctionPipeline, INTERFACE_IMPL_END)
};