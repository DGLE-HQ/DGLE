/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender3D: public CInstancedObj, public IRender3D
{
	ICoreRenderer *_pCoreRenderer;
	TMatrix4x4Stack _matrixStack;
	TColor4 _stColor, _stGlobalAmbient;
	TBlendStateDesc _stBlendStateDesc;
	TRasterizerStateDesc _stRasterStateDesc;
	TDepthStencilDesc _stDepthStencilDesc;
	IMaterial *_pCurMat;

	std::stack<E_PUSH_STATES_FLAGS> _stackStatesFlag;

	struct TFogDescriptor
	{
		bool bEnabled;
		TColor4 stColor;
		float fDensity, fStart, fEnd;

		TFogDescriptor() : bEnabled(false), stColor(ColorGray()),
			fDensity(0.25f), fStart(500.f), fEnd(1000.f){}

	} _stFogDesc;

public:

	CRender3D(uint uiInstIdx);
	~CRender3D();

	void BeginFrame();
	void EndFrame();
	void UnbindMaterial();
	void BindMaterial(IMaterial *pMaterial);

	DGLE_RESULT DGLE_API SetPerspective(float fFovAngle, float fZNear, float fZFar);
	DGLE_RESULT DGLE_API GetPerspective(float &fFovAngle, float &fZNear, float &fZFar);
	
	DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetColor(TColor4 &stColor);

	DGLE_RESULT DGLE_API SetBlendMode(E_EFFECT_BLENDING_FLAGS eMode);
	DGLE_RESULT DGLE_API GetBlendMode(E_EFFECT_BLENDING_FLAGS &eMode);

	DGLE_RESULT DGLE_API ToggleAlphaTest(bool bEnabled);
	DGLE_RESULT DGLE_API SetAlphaTreshold(float fValue);
	DGLE_RESULT DGLE_API GetAlphaTreshold(float &fValue);

	DGLE_RESULT DGLE_API ClearDepthBuffer();
	DGLE_RESULT DGLE_API ToggleDepthTest(bool bEnabled);

	DGLE_RESULT DGLE_API ToggleFog(bool bEnabled);
	DGLE_RESULT DGLE_API SetLinearFogBounds(float fStart, float fEnd);
	DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API SetFogDensity(float fValue);
	DGLE_RESULT DGLE_API GetLinearFogBounds(float &fStart, float &fEnd);
	DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API GetFogDensity(float &fValue);

	DGLE_RESULT DGLE_API SetMatrix(const TMatrix4x4 &stMatrix, bool bMult);
	DGLE_RESULT DGLE_API GetMatrix(TMatrix4x4 &stMatrix);

	DGLE_RESULT DGLE_API DrawAxes(float fSize, bool bNoDepthTest);

	DGLE_RESULT DGLE_API PushStates(E_PUSH_STATES_FLAGS eStates);
	DGLE_RESULT DGLE_API PopStates();

	DGLE_RESULT DGLE_API GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_FLAG eFlag);
	DGLE_RESULT DGLE_API GetPoint2(const TPoint3 &stPoint, TPoint2 &stResultPointOnScreen);

	DGLE_RESULT DGLE_API FrustumSetup();
	DGLE_RESULT DGLE_API CullPoint(const TPoint3 &stCoords, bool &bCull);
	DGLE_RESULT DGLE_API CullSphere(const TPoint3 &stCoords, float fRadius, bool &bCull);
	DGLE_RESULT DGLE_API CullBox(const TPoint3 &stCenterCoords, const TVector3 &stExtents, bool &bCull);

	DGLE_RESULT DGLE_API CreateLight(ILight *&prLight);
	DGLE_RESULT DGLE_API ToggleLighting(bool bEnabled);
	DGLE_RESULT DGLE_API SetGlobalAmbientLighting(const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetGlobalAmbientLighting(TColor4 &stColor);

	IDGLE_BASE_IMPLEMENTATION(IRender3D, INTERFACE_IMPL_END)
};