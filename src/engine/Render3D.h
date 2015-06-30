/**
\author		Korotkov Andrey aka DRON
\date		1.07.2015 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender3D: public CInstancedObj, public IRender3D
{
	ICoreRenderer *_pCoreRenderer;
	
	int _iProfilerState, _iDrawAxes, _iDrawLights;
	uint64 _ui64DrawDelay;
	uint _uiObjsDrawnCount;

	struct TState
	{
		float fFovAngle, fZNear, fZFar;
		TMatrix4x4Stack matrixStack;		
		bool isLightingEnabled;
		TColor4 stGlobalAmbient, stColor;
		E_BLENDING_EFFECT eBlendingMode;
		TBlendStateDesc stBlendStateDesc;
		TRasterizerStateDesc stRasterStateDesc;
		TDepthStencilDesc stDepthStencilDesc;
		IMaterial *pCurMat;
		std::vector<ITexture *> pCurTexs;
		
		struct TLight
		{
			bool bEnabled;
			E_LIGHT_TYPE eType;
			TColor4 stDiffCol;
			TPoint3 stPos;
			TVector3 stDir;
			float fRange, fIntensity, fAngle;

			ILight *pLight;
		};
		std::vector<TLight> vecCurLights;

		struct TFogDescriptor
		{
			bool bEnabled;
			TColor4 stColor;
			float fStart, fEnd;
		} stFogDesc;
		
	} _stCurState;

	uint _uiMaxLightsCount, _uiMaxTexUnits;

	std::stack<TState> _stackStates;

	bool _bFrCalculated;
	float _afFrPlanes[6][4], _afAbsFrPlanes[6][3];

	IFixedFunctionPipeline *_pFFP;

	void _SetPerspectiveMatrix(uint width, uint height);
	void _DrawLight(uint idx);

public:

	CRender3D(uint uiInstIdx);
	~CRender3D();

	void SetDefaultStates();
	void BeginFrame();
	void EndFrame();
	void OnResize(uint uiWidth, uint uiHeight);
	void DrawProfiler();
	
	void UnbindMaterial();
	void UnbindTextures();
	void UnbindLights();
	
	void PrepareFor2D();
	void RefreshBatchData();

	void PushSelfStates();
	void PopSelfStates();

	DGLE_RESULT DGLE_API SetPerspective(float fFovAngle, float fZNear, float fZFar);
	DGLE_RESULT DGLE_API GetPerspective(float &fFovAngle, float &fZNear, float &fZFar);
	
	DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetColor(TColor4 &stColor);

	DGLE_RESULT DGLE_API GetMaxLightsPerPassCount(uint &uiCount);
	DGLE_RESULT DGLE_API UpdateLight(ILight *pLight);

	DGLE_RESULT DGLE_API BindTexture(ITexture *pTex, uint uiTextureLayer);
	DGLE_RESULT DGLE_API GetTexture(ITexture *&prTex, uint uiTextureLayer);

	DGLE_RESULT DGLE_API BindMaterial(IMaterial *pMat);
	DGLE_RESULT DGLE_API GetMaterial(IMaterial *&prMat);

	DGLE_RESULT DGLE_API ToggleBlending(bool bEnabled);
	DGLE_RESULT DGLE_API IsBlendingEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API SetBlendMode(E_BLENDING_EFFECT eMode);
	DGLE_RESULT DGLE_API GetBlendMode(E_BLENDING_EFFECT &eMode);

	DGLE_RESULT DGLE_API ToggleAlphaTest(bool bEnabled);
	DGLE_RESULT DGLE_API SetAlphaTreshold(float fTreshold);
	DGLE_RESULT DGLE_API IsAlphaTestEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetAlphaTreshold(float &fTreshold);

	DGLE_RESULT DGLE_API ClearDepthBuffer();
	DGLE_RESULT DGLE_API ToggleDepthTest(bool bEnabled);
	DGLE_RESULT DGLE_API IsDepthTestEnabled(bool &bEnabled);

	DGLE_RESULT DGLE_API ToggleBackfaceCulling(bool bEnabled);
	DGLE_RESULT DGLE_API IsBackfaceCullingEnabled(bool &bEnabled);

	DGLE_RESULT DGLE_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	DGLE_RESULT DGLE_API DrawBuffer(ICoreGeometryBuffer *pBuffer);

	DGLE_RESULT DGLE_API ToggleFog(bool bEnabled);
	DGLE_RESULT DGLE_API SetLinearFogBounds(float fStart, float fEnd);
	DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API IsFogEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetLinearFogBounds(float &fStart, float &fEnd);
	DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor);

	DGLE_RESULT DGLE_API SetMatrix(const TMatrix4x4 &stMatrix);
	DGLE_RESULT DGLE_API MultMatrix(const TMatrix4x4 &stMatrix);
	DGLE_RESULT DGLE_API PushMatrix();
	DGLE_RESULT DGLE_API PopMatrix();
	DGLE_RESULT DGLE_API GetMatrix(TMatrix4x4 &stMatrix);

	DGLE_RESULT DGLE_API DrawAxes(float fSize, bool bNoDepthTest);

	DGLE_RESULT DGLE_API ResetStates();
	DGLE_RESULT DGLE_API PushStates();
	DGLE_RESULT DGLE_API PopStates();

	DGLE_RESULT DGLE_API GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_MODE eFlag);
	DGLE_RESULT DGLE_API GetPoint2(const TPoint3 &stPoint, TPoint2 &stResultPointOnScreen);

	DGLE_RESULT DGLE_API SetupFrustum();
	DGLE_RESULT DGLE_API CullPoint(const TPoint3 &stCoords, bool &bCull);
	DGLE_RESULT DGLE_API CullSphere(const TPoint3 &stCenter, float fRadius, bool &bCull);
	DGLE_RESULT DGLE_API CullBox(const TPoint3 &stCenter, const TVector3 &stExtents, bool &bCull);

	DGLE_RESULT DGLE_API CreateLight(ILight *&prLight);
	DGLE_RESULT DGLE_API ToggleLighting(bool bEnabled);
	DGLE_RESULT DGLE_API SetGlobalAmbientLighting(const TColor4 &stColor);
	DGLE_RESULT DGLE_API IsLightingEnabled(bool &bEnabled);
	DGLE_RESULT DGLE_API GetGlobalAmbientLighting(TColor4 &stColor);

	IDGLE_BASE_IMPLEMENTATION(IRender3D, INTERFACE_IMPL_END)
};