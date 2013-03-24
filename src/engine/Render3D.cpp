/**
\author		Korotkov Andrey aka DRON
\date		24.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "ResourceManager.h"
#include "Render3D.h"
#include "Render.h"
#include "Render2D.h"

#define IN_3D_GUARD if (Core()->pRender()->pRender2D()->In2DMode()) Core()->pRender()->pRender2D()->End2D();

CRender3D::CRender3D(uint uiInstIdx):
CInstancedObj(uiInstIdx), _iProfilerState(0),
_bFrCalculated(false), _uiObjsDrawnCount(0)
{
	_pCoreRenderer = Core()->pCoreRenderer();

	bool supported;
	
	_pCoreRenderer->IsFeatureSupported(CRFT_LEGACY_FIXED_FUNCTION_PIPELINE_API, supported);

	if (supported)
		_pCoreRenderer->GetFixedFunctionPipelineAPI(_pFFP);
	else
		_pFFP = NULL;

	_stCurState.isLightingEnabled = false;
	_stCurState.stGlobalAmbient = ColorBlack();
	_stCurState.pCurMat = NULL;

	int count;
	
	_pCoreRenderer->GetDeviceMetric(CRMT_MAX_TEXTURE_LAYERS, count);
	_stCurState.pCurTexs.resize((size_t)count, NULL);
	_uiMaxTextsCount = (uint)count;

	_pCoreRenderer->GetDeviceMetric(CRMT_MAX_LIGHTS_PER_PASS, count);
	_uiMaxLightsCount = (uint)count;

	Console()->RegComVar("rnd3d_profiler", "Displays Render 3D subsystems profiler.", &_iProfilerState, 0, 1);
}

CRender3D::~CRender3D()
{
	Console()->UnRegCom("rnd3d_profiler");
}

void CRender3D::DrawProfiler()
{
	if (_iProfilerState > 0)
	{
		Core()->RenderProfilerText("======Render3D Profiler=====", ColorWhite());
		Core()->RenderProfilerText(("Objects on screen :" + UIntToStr(_uiObjsDrawnCount)).c_str(), ColorWhite());
		Core()->RenderProfilerText(("Render delay      :" + UInt64ToStr(_ui64DrawDelay / 1000) + "." + UIntToStr(_ui64DrawDelay % 1000) + " ms").c_str(), ColorWhite());
	}
}

void CRender3D::BeginFrame()
{
	_ui64DrawDelay = GetPerfTimer();
	_bFrCalculated = false;
	_uiObjsDrawnCount = 0;
	_uiLightsEnabledCount = 0;

	_stCurState.matrixStack.Clear(MatrixIdentity());
	_pCoreRenderer->SetMatrix(_stCurState.matrixStack.Top());

	while (!_stackStates.empty())
		_stackStates.pop();
	
	while (!_stackStatesFlag.empty())
		_stackStatesFlag.pop();

	_pCoreRenderer->SetBlendState(_stCurState.stBlendStateDesc);
	_pCoreRenderer->SetRasterizerState(_stCurState.stRasterStateDesc);
	_pCoreRenderer->SetDepthStencilState(_stCurState.stDepthStencilDesc);

	_pCoreRenderer->SetColor(ColorWhite());
	Core()->pRender()->Unbind(EOT_UNKNOWN);
}

void CRender3D::EndFrame()
{
	_ui64DrawDelay = GetPerfTimer() - _ui64DrawDelay - Core()->pRender()->pRender2D()->GetAverallDelay();
}

void CRender3D::UnbindMaterial()
{
	_stCurState.pCurMat = NULL;
}

DGLE_RESULT DGLE_API CRender3D::BindMaterial(IMaterial *pMat)
{
	IN_3D_GUARD

	if (_stCurState.pCurMat == pMat)
		return S_OK;

	_stCurState.pCurMat = pMat;

	if (pMat == NULL)
		BindMaterial(Core()->pResMan()->pIDefaultMaterial());
	else
	{
		TColor4 diff_col;
		pMat->GetDiffuseColor(diff_col);

		ITexture *p_diff_tex;
		pMat->GetDiffuseTexture(p_diff_tex);
		
		if (p_diff_tex)
			p_diff_tex->Bind(0);
		else
			_pCoreRenderer->BindTexture(NULL, 0);

		float shininess;
		pMat->GetShininess(shininess);

		TColor4 spec_col;
		pMat->GetSpecularColor(spec_col);

		if (_pFFP)
		{
			_pFFP->SetMaterialDiffuseColor(diff_col);
			_pFFP->SetMaterialShininess(shininess);
			_pFFP->SetMaterialSpecularColor(spec_col);
		}
	}

	return S_OK;
}

void CRender3D::UnbindTextures()
{
	memset(&_stCurState.pCurTexs[0], NULL, _stCurState.pCurTexs.size() * sizeof(ITexture *));
}

DGLE_RESULT DGLE_API CRender3D::BindTexture(ITexture *pTex, uint uiTextureLayer)
{
	IN_3D_GUARD
	
	if (uiTextureLayer >= _uiMaxTextsCount)
		return E_INVALIDARG;

	if (_stCurState.pCurTexs[uiTextureLayer] == pTex)
		return S_OK;

	_stCurState.pCurTexs[uiTextureLayer] = pTex;

	ICoreTexture *p_core_tex = NULL;
	
	if (pTex)
		pTex->GetCoreTexture(p_core_tex);

	_pCoreRenderer->BindTexture(p_core_tex, uiTextureLayer);

	return S_OK;
}

void CRender3D::UnbindLights()
{
	if (_pFFP)
	{
		for (size_t i = 0; i < _stCurState.pCurLights.size(); ++i)
			if (_stCurState.pCurLights[i])
			{
				bool is_enabled;

				_stCurState.pCurLights[i]->GetEnabled(is_enabled);

				if (is_enabled)
					_pFFP->SetLightEnabled(i, false);
			}
	}
	
	_stCurState.pCurLights.clear();
}

DGLE_RESULT DGLE_API CRender3D::UpdateLight(ILight *pLight)
{
	// ToDo: if light is becoming desabled it is not removed from pool

	IN_3D_GUARD

	if (!pLight)
		return E_INVALIDARG;

	uint idx = -1;

	for (size_t i = 0; i < _stCurState.pCurLights.size(); ++i)
		if (_stCurState.pCurLights[i] == pLight)
		{
			idx = i;
			break;
		}

	if (idx == -1 && _uiLightsEnabledCount + 1 == _uiMaxLightsCount)
		return E_ABORT;

	bool is_enabled;
	pLight->GetEnabled(is_enabled);
	
	TColor4 col;
	pLight->GetColor(col);

	TPoint3 pos;
	pLight->GetPosition(pos);

	TVector3 dir;
	pLight->GetDirection(dir);

	float intense;
	pLight->GetIntensity(intense);

	float range;
	pLight->GetRange(range);

	float angle;
	pLight->GetSpotAngle(angle);

	E_LIGHT_TYPE type;
	pLight->GetType(type);

	if (idx == -1)
	{
		_stCurState.pCurLights.push_back(pLight);
		++_uiLightsEnabledCount;
	}

	if (_pFFP)
	{
		_pFFP->SetLightEnabled(idx, is_enabled);
		_pFFP->SetLightColor(idx, col);
		_pFFP->SetLightPosition(idx, pos);
		_pFFP->SetLightIntensity(idx, intense);

		switch (type)
		{
		case LT_POINT:
			_pFFP->ConfigurePointLight(idx, range);
			break;

		case LT_DIRECTIONAL:
			_pFFP->ConfigureDirectionalLight(idx, dir);
			break;

		case LT_SPOT:
			_pFFP->ConfigureSpotLight(idx, dir, range, angle);
			break;
		}
	}

	return S_OK;
}

void CRender3D::PushSelfStates()
{
	_stackStates.push(_stCurState);

	if (_pFFP)
		_pFFP->PushStates();
}

void CRender3D::PopSelfStates()
{
	_stCurState = _stackStates.top();

	_stackStates.pop();

	if (_pFFP)
		_pFFP->PopStates();
}

void CRender3D::PrepareFor2D()
{
	if (_pFFP)
	{
		_pFFP->ToggleGlobalLighting(false);
		_pFFP->SetFogEnabled(false);
	}
}

DGLE_RESULT DGLE_API CRender3D::SetPerspective(float fFovAngle, float fZNear, float fZFar)
{
	IN_3D_GUARD

	Core()->pRender()->SetPerspective(fFovAngle, fZNear, fZFar);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetPerspective(float &fFovAngle, float &fZNear, float &fZFar)
{
	Core()->pRender()->GetPerspective(fFovAngle, fZNear, fZFar);
	return S_OK;
}
	
DGLE_RESULT DGLE_API CRender3D::GetTexture(ITexture *&prTex, uint uiTextureLayer)
{
	IN_3D_GUARD

	if (uiTextureLayer >= _uiMaxTextsCount)
		return E_INVALIDARG;

	prTex = _stCurState.pCurTexs[uiTextureLayer];

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetMaterial(IMaterial *&prMat)
{
	IN_3D_GUARD
	
	prMat = _stCurState.pCurMat;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetBlendMode(E_EFFECT_BLENDING_FLAGS eMode)
{
	IN_3D_GUARD

	switch(eMode)
	{
	case EBF_NORMAL:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stCurState.stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_ALPHA;
		break;
	case EBF_ADD:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stCurState.stBlendStateDesc.eDstFactor = BF_ONE;
		break;
	case EBF_MULT:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_ZERO;
		_stCurState.stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case EBF_BLACK:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_SRC_COLOR;
		_stCurState.stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_COLOR;
		break;
	case EBF_WHITE:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_ONE_MINUS_SRC_COLOR;
		_stCurState.stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case EBF_MASK:
		_stCurState.stBlendStateDesc.eSrcFactor = BF_DST_ALPHA;
		_stCurState.stBlendStateDesc.eDstFactor = BF_ZERO;
		break;
	}

	_pCoreRenderer->SetBlendState(_stCurState.stBlendStateDesc);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetBlendMode(E_EFFECT_BLENDING_FLAGS &eMode)
{
	if (_stCurState.stBlendStateDesc.eSrcFactor == BF_SRC_ALPHA && _stCurState.stBlendStateDesc.eDstFactor == BF_ONE_MINUS_SRC_ALPHA)
		eMode = EBF_NORMAL;
	else
		if (_stCurState.stBlendStateDesc.eSrcFactor == BF_SRC_ALPHA && _stCurState.stBlendStateDesc.eDstFactor == BF_ONE)
			eMode = EBF_ADD;
		else
			if (_stCurState.stBlendStateDesc.eSrcFactor == BF_ZERO && _stCurState.stBlendStateDesc.eDstFactor == BF_SRC_COLOR)
				eMode = EBF_MULT;
			else
				if (_stCurState.stBlendStateDesc.eSrcFactor == BF_SRC_COLOR && _stCurState.stBlendStateDesc.eDstFactor == BF_ONE_MINUS_SRC_COLOR)
					eMode = EBF_BLACK;
				else
					if (_stCurState.stBlendStateDesc.eSrcFactor == BF_ONE_MINUS_SRC_COLOR && _stCurState.stBlendStateDesc.eDstFactor == BF_SRC_COLOR)
						eMode = EBF_WHITE;
					else
						if (_stCurState.stBlendStateDesc.eSrcFactor == BF_DST_ALPHA && _stCurState.stBlendStateDesc.eDstFactor == BF_ZERO)
							eMode = EBF_MASK;
							
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleAlphaTest(bool bEnabled)
{
	IN_3D_GUARD
	
	_stCurState.stRasterStateDesc.bAlphaTestEnabled = bEnabled;
	_pCoreRenderer->SetRasterizerState(_stCurState.stRasterStateDesc);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetAlphaTreshold(float fTreshold)
{
	IN_3D_GUARD

	_stCurState.stRasterStateDesc.fAlphaTestRefValue = fTreshold;
	_pCoreRenderer->SetRasterizerState(_stCurState.stRasterStateDesc);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::IsAlphaTestEnabled(bool &bEnabled)
{
	bEnabled = _stCurState.stRasterStateDesc.bAlphaTestEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetAlphaTreshold(float &fTreshold)
{
	fTreshold = _stCurState.stRasterStateDesc.fAlphaTestRefValue;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ClearDepthBuffer()
{
	IN_3D_GUARD
	
	_pCoreRenderer->Clear(false, true, false);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleDepthTest(bool bEnabled)
{
	IN_3D_GUARD

	_stCurState.stDepthStencilDesc.bDepthTestEnabled = bEnabled;
	_pCoreRenderer->SetDepthStencilState(_stCurState.stDepthStencilDesc);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::IsDepthTestEnabled(bool &bEnabled)
{
	bEnabled = _stCurState.stDepthStencilDesc.bDepthTestEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	IN_3D_GUARD

	++_uiObjsDrawnCount;

	return _pCoreRenderer->Draw(stDrawDesc, eMode, uiCount);
}

DGLE_RESULT DGLE_API CRender3D::DrawBuffer(ICoreGeometryBuffer *pBuffer)
{
	IN_3D_GUARD

	++_uiObjsDrawnCount;

	return _pCoreRenderer->DrawBuffer(pBuffer);
}

DGLE_RESULT DGLE_API CRender3D::ToggleFog(bool bEnabled)
{
	IN_3D_GUARD

	_stCurState.stFogDesc.bEnabled = bEnabled;
	
	if (_pFFP)
		_pFFP->SetFogEnabled(bEnabled);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetLinearFogBounds(float fStart, float fEnd)
{
	IN_3D_GUARD

	_stCurState.stFogDesc.fStart = fStart;
	_stCurState.stFogDesc.fEnd = fEnd;
	
	if (_pFFP)
		_pFFP->GonfigureFog(fStart, fEnd, _stCurState.stFogDesc.fDensity);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetFogColor(const TColor4 &stColor)
{
	IN_3D_GUARD

	_stCurState.stFogDesc.stColor = stColor;

	if (_pFFP)
		_pFFP->SetFogColor(stColor);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetFogDensity(float fDensity)
{
	IN_3D_GUARD

	_stCurState.stFogDesc.fDensity = fDensity;
	
	if (_pFFP)
		_pFFP->GonfigureFog(_stCurState.stFogDesc.fStart, _stCurState.stFogDesc.fEnd, fDensity);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::IsFogEnabled(bool &bEnabled)
{
	bEnabled = _stCurState.stFogDesc.bEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetLinearFogBounds(float &fStart, float &fEnd)
{
	fStart = _stCurState.stFogDesc.fStart;
	fEnd = _stCurState.stFogDesc.fEnd;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetFogColor(TColor4 &stColor)
{
	stColor = _stCurState.stFogDesc.stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetFogDensity(float &fDensity)
{
	fDensity = _stCurState.stFogDesc.fDensity;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetMatrix(const TMatrix4x4 &stMatrix, bool bMult)
{
	IN_3D_GUARD

	if (bMult)
		_stCurState.matrixStack.MultLocal(stMatrix);
	else
		_stCurState.matrixStack.Top() = stMatrix;

	_pCoreRenderer->SetMatrix(_stCurState.matrixStack.Top());

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetMatrix(TMatrix4x4 &stMatrix)
{
	stMatrix = _stCurState.matrixStack.Top();
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::DrawAxes(float fSize, bool bNoDepthTest)
{
	IN_3D_GUARD

	const float axes[] = {
		0.f, 0.f, 0.f, fSize, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, fSize, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, fSize,
		1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f };
	
	TDrawDataDesc desc;
	desc.pData = (uint8 *)axes;
	desc.uiColorOffset = 72;

	bool dtest = _stCurState.stDepthStencilDesc.bDepthTestEnabled;

	if (dtest && bNoDepthTest)
	{
		_stCurState.stDepthStencilDesc.bDepthTestEnabled = false;
		_pCoreRenderer->SetDepthStencilState(_stCurState.stDepthStencilDesc);
	}

	PARANOIC_CHECK_RES(_pCoreRenderer->Draw(desc, CRDM_LINES, 6));

	if (dtest && bNoDepthTest)
	{
		_stCurState.stDepthStencilDesc.bDepthTestEnabled = true;
		_pCoreRenderer->SetDepthStencilState(_stCurState.stDepthStencilDesc);
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::PushStates(E_PUSH_STATES_FLAGS eStates)
{
	IN_3D_GUARD

	_stackStatesFlag.push(eStates);

	if (eStates == PSF_ALL)
		eStates = (E_PUSH_STATES_FLAGS)(PSF_MATRIX | PSF_STATES);

	if (eStates & PSF_MATRIX)
		_stCurState.matrixStack.Push();

	if (eStates & PSF_STATES)
	{
		_pCoreRenderer->PushStates();
		PushSelfStates();
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::PopStates()
{
	IN_3D_GUARD

	E_PUSH_STATES_FLAGS states = _stackStatesFlag.top();

	if (states & PSF_MATRIX)
		_stCurState.matrixStack.Pop();

	if (states & PSF_STATES)
	{
		PopSelfStates();
		_pCoreRenderer->PopStates();
	}

	_stackStatesFlag.pop();

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_FLAG eFlag)
{
	IN_3D_GUARD

	TMatrix4 mview, proj;
	
	_pCoreRenderer->GetMatrix(mview, MT_MODELVIEW);
	_pCoreRenderer->GetMatrix(proj, MT_PROJECTION);

	uint vport[4];
	_pCoreRenderer->GetViewport(vport[0], vport[1], vport[2], vport[3]);

	float win[3] = {stPointOnScreen.x, vport[3] - stPointOnScreen.y, 0.f};

	if (eFlag & GP3F_FROM_FAR_PLANE)
		win[2] = 1.f;
	else
		if (eFlag & GP3F_FROM_NEAR_PLANE)
			win[2] = 0.f;
		else
			if (FAILED(_pCoreRenderer->ReadFrameBuffer((uint)stPointOnScreen.x, (uint)stPointOnScreen.y, 1, 1, reinterpret_cast<uint8 *>(&win[2]), sizeof(float), TDF_DEPTH_COMPONENT32)))
				return E_ABORT;

	const TMatrix4 m = MatrixInverse(mview * proj);

	float in[4], out[4];

	in[0] = (win[0] - vport[0]) / vport[2] * 2.f - 1.f;
	in[1] = (win[1] - vport[1]) / vport[3] * 2.f - 1.f;
	in[2] = 2.f * win[2] - 1.f;
	in[3] = 1.f;

	out[0] = m._1D[0] * in[0] + m._1D[4] * in[1] + m._1D[8] * in[2] + m._1D[12] * in[3];
	out[1] = m._1D[1] * in[0] + m._1D[5] * in[1] + m._1D[9] * in[2] + m._1D[13] * in[3];
	out[2] = m._1D[2] * in[0] + m._1D[6] * in[1] + m._1D[10] * in[2] + m._1D[14] * in[3];
	out[3] = m._1D[3] * in[0] + m._1D[7] * in[1] + m._1D[11] * in[2] + m._1D[15] * in[3];
 
	if (out[3] == 0.f)
		return E_ABORT;
	
	out[3] = 1.f / out[3];

	stResultPoint.x = out[0] * out[3];
	stResultPoint.y = out[1] * out[3];
	stResultPoint.z = out[2] * out[3];

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetPoint2(const TPoint3 &stPoint, TPoint2 &stResultPointOnScreen)
{
	IN_3D_GUARD

	TMatrix4 mview, proj;
	
	_pCoreRenderer->GetMatrix(mview, MT_MODELVIEW);
	_pCoreRenderer->GetMatrix(proj, MT_PROJECTION);

	uint vport[4];
	_pCoreRenderer->GetViewport(vport[0], vport[1], vport[2], vport[3]);

	float tmp[8];
	  
	tmp[0] = mview._1D[0] * stPoint.x + mview._1D[4] * stPoint.y + mview._1D[8] * stPoint.z + mview._1D[12];
	tmp[1] = mview._1D[1] * stPoint.x + mview._1D[5] * stPoint.y + mview._1D[9] * stPoint.z + mview._1D[13];
	tmp[2] = mview._1D[2] * stPoint.x + mview._1D[6] * stPoint.y + mview._1D[10] * stPoint.z + mview._1D[14];
	tmp[3] = mview._1D[3] * stPoint.x + mview._1D[7] * stPoint.y + mview._1D[11] * stPoint.z + mview._1D[15];

	tmp[4] = proj._1D[0] * tmp[0] + proj._1D[4] * tmp[1] + proj._1D[8] * tmp[2] + proj._1D[12] * tmp[3];
	tmp[5] = proj._1D[1] * tmp[0] + proj._1D[5] * tmp[1] + proj._1D[9] * tmp[2] + proj._1D[13] * tmp[3];
	tmp[6] = proj._1D[2] * tmp[0] + proj._1D[6] * tmp[1] + proj._1D[10] * tmp[2] + proj._1D[14] * tmp[3];
	tmp[7] = -tmp[2];
	
	if (tmp[7] == 0.f)
		return E_ABORT;

	tmp[7] = 1.f / tmp[7];
	tmp[4] *= tmp[7];
	tmp[5] *= tmp[7];
	tmp[6] *= tmp[7];
	
	stResultPointOnScreen.x = (tmp[4] * 0.5f + 0.5f) * vport[2] + vport[0];
	stResultPointOnScreen.y = vport[3] - ((tmp[5] * 0.5f + 0.5f) * vport[3] + vport[1]);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetupFrustum()
{
	IN_3D_GUARD

	TMatrix4 mview, proj;
	
	_pCoreRenderer->GetMatrix(mview, MT_MODELVIEW);
	_pCoreRenderer->GetMatrix(proj, MT_PROJECTION);

	TMatrix4 plane_transform = mview * proj;

	_afFrPlanes[0][0] = -plane_transform._2D[0][0] - plane_transform._2D[0][3];
	_afFrPlanes[0][1] = -plane_transform._2D[1][0] - plane_transform._2D[1][3];
	_afFrPlanes[0][2] = -plane_transform._2D[2][0] - plane_transform._2D[2][3];
	_afFrPlanes[0][3] = -plane_transform._2D[3][0] - plane_transform._2D[3][3];

	_afFrPlanes[1][0] = plane_transform._2D[0][0] - plane_transform._2D[0][3];
	_afFrPlanes[1][1] = plane_transform._2D[1][0] - plane_transform._2D[1][3];
	_afFrPlanes[1][2] = plane_transform._2D[2][0] - plane_transform._2D[2][3];
	_afFrPlanes[1][3] = plane_transform._2D[3][0] - plane_transform._2D[3][3];

	_afFrPlanes[2][0] = -plane_transform._2D[0][1] - plane_transform._2D[0][3];
	_afFrPlanes[2][1] = -plane_transform._2D[1][1] - plane_transform._2D[1][3];
	_afFrPlanes[2][2] = -plane_transform._2D[2][1] - plane_transform._2D[2][3];
	_afFrPlanes[2][3] = -plane_transform._2D[3][1] - plane_transform._2D[3][3];

	_afFrPlanes[3][0] = plane_transform._2D[0][1] - plane_transform._2D[0][3];
	_afFrPlanes[3][1] = plane_transform._2D[1][1] - plane_transform._2D[1][3];
	_afFrPlanes[3][2] = plane_transform._2D[2][1] - plane_transform._2D[2][3];
	_afFrPlanes[3][3] = plane_transform._2D[3][1] - plane_transform._2D[3][3];

	_afFrPlanes[4][0] = -plane_transform._2D[0][2] - plane_transform._2D[0][3];
	_afFrPlanes[4][1] = -plane_transform._2D[1][2] - plane_transform._2D[1][3];
	_afFrPlanes[4][2] = -plane_transform._2D[2][2] - plane_transform._2D[2][3];
	_afFrPlanes[4][3] = -plane_transform._2D[3][2] - plane_transform._2D[3][3];

	_afFrPlanes[5][0] = plane_transform._2D[0][2] - plane_transform._2D[0][3];
	_afFrPlanes[5][1] = plane_transform._2D[1][2] - plane_transform._2D[1][3];
	_afFrPlanes[5][2] = plane_transform._2D[2][2] - plane_transform._2D[2][3];
	_afFrPlanes[5][3] = plane_transform._2D[3][2] - plane_transform._2D[3][3];

	_afAbsFrPlanes[0][0] = fabs(_afFrPlanes[0][0]); _afAbsFrPlanes[0][1] = fabs(_afFrPlanes[0][1]); _afAbsFrPlanes[0][2] = fabs(_afFrPlanes[0][2]);
	_afAbsFrPlanes[1][0] = fabs(_afFrPlanes[1][0]); _afAbsFrPlanes[1][1] = fabs(_afFrPlanes[1][1]); _afAbsFrPlanes[1][2] = fabs(_afFrPlanes[1][2]);
	_afAbsFrPlanes[2][0] = fabs(_afFrPlanes[2][0]); _afAbsFrPlanes[2][1] = fabs(_afFrPlanes[2][1]); _afAbsFrPlanes[2][2] = fabs(_afFrPlanes[2][2]);
	_afAbsFrPlanes[3][0] = fabs(_afFrPlanes[3][0]); _afAbsFrPlanes[3][1] = fabs(_afFrPlanes[3][1]); _afAbsFrPlanes[3][2] = fabs(_afFrPlanes[3][2]);
	_afAbsFrPlanes[4][0] = fabs(_afFrPlanes[4][0]); _afAbsFrPlanes[4][1] = fabs(_afFrPlanes[4][1]); _afAbsFrPlanes[4][2] = fabs(_afFrPlanes[4][2]);
	_afAbsFrPlanes[5][0] = fabs(_afFrPlanes[5][0]); _afAbsFrPlanes[5][1] = fabs(_afFrPlanes[5][1]); _afAbsFrPlanes[5][2] = fabs(_afFrPlanes[5][2]);

	_bFrCalculated = true;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::CullPoint(const TPoint3 &stCoords, bool &bCull)
{
	if (!_bFrCalculated)
	{
		bCull = false;
		return S_FALSE;
	}

	bCull = stCoords.Dot(TVector3(&_afFrPlanes[0][0])) + _afFrPlanes[0][3] > 0 ||
		stCoords.Dot(TVector3(&_afFrPlanes[1][0])) + _afFrPlanes[1][3] > 0 ||
		stCoords.Dot(TVector3(&_afFrPlanes[2][0])) + _afFrPlanes[2][3] > 0 ||
		stCoords.Dot(TVector3(&_afFrPlanes[3][0])) + _afFrPlanes[3][3] > 0 ||
		stCoords.Dot(TVector3(&_afFrPlanes[4][0])) + _afFrPlanes[4][3] > 0 ||
		stCoords.Dot(TVector3(&_afFrPlanes[5][0])) + _afFrPlanes[5][3] > 0;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::CullSphere(const TPoint3 &stCenter, float fRadius, bool &bCull)
{
	if (!_bFrCalculated)
	{
		bCull = false;
		return S_FALSE;
	}
	
	bCull = stCenter.Dot(TVector3(&_afFrPlanes[0][0])) + _afFrPlanes[0][3] - fRadius > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[1][0])) + _afFrPlanes[1][3] - fRadius > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[2][0])) + _afFrPlanes[2][3] - fRadius > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[3][0])) + _afFrPlanes[3][3] - fRadius > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[4][0])) + _afFrPlanes[4][3] - fRadius > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[5][0])) + _afFrPlanes[5][3] - fRadius > 0;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::CullBox(const TPoint3 &stCenter, const TVector3 &stExtents, bool &bCull)
{
	if (!_bFrCalculated)
	{
		bCull = false;
		return S_FALSE;
	}

	bCull = stCenter.Dot(TVector3(&_afFrPlanes[0][0])) + _afFrPlanes[0][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[0][0])) > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[1][0])) + _afFrPlanes[1][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[1][0])) > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[2][0])) + _afFrPlanes[2][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[2][0])) > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[3][0])) + _afFrPlanes[3][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[3][0])) > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[4][0])) + _afFrPlanes[4][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[4][0])) > 0 ||
		stCenter.Dot(TVector3(&_afFrPlanes[5][0])) + _afFrPlanes[5][3] - stExtents.Dot(TVector3(&_afAbsFrPlanes[5][0])) > 0;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleLighting(bool bEnabled)
{
	IN_3D_GUARD

	_stCurState.isLightingEnabled = bEnabled;
	
	if (_pFFP)
		_pFFP->ToggleGlobalLighting(bEnabled);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetGlobalAmbientLighting(const TColor4 &stColor)
{
	IN_3D_GUARD

	_stCurState.stGlobalAmbient = stColor;

	if (_pFFP)
		_pFFP->SetGloablAmbientLight(stColor);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::IsLightingEnabled(bool &bEnabled)
{
	bEnabled = _stCurState.isLightingEnabled;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetGlobalAmbientLighting(TColor4 &stColor)
{
	stColor = _stCurState.stGlobalAmbient;
	return S_OK;
}