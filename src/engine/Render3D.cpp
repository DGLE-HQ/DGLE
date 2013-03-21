/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Render3D.h"
#include "Render.h"

CRender3D::CRender3D(uint uiInstIdx):
CInstancedObj(uiInstIdx), _stGlobalAmbient(ColorBlack())
{
	_pCoreRenderer = Core()->pCoreRenderer();

	memset(_afFrPlanes, 0, 6 * 4 * sizeof(float));
	memset(_afAbsFrPlanes, 0, 6 * 3 * sizeof(float));
}

CRender3D::~CRender3D()
{}

void CRender3D::BeginFrame()
{
	_matrixStack.Clear(MatrixIdentity());
	_pCoreRenderer->SetMatrix(_matrixStack.Top());

	while (!_stackStatesFlag.empty())
		_stackStatesFlag.pop();

	_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
	_pCoreRenderer->SetDepthStencilState(_stDepthStencilDesc);

	_pCoreRenderer->SetColor(ColorWhite());
	Core()->pRender()->Unbind(EOT_UNKNOWN);
}

void CRender3D::EndFrame()
{}

void CRender3D::UnbindMaterial()
{
	_pCurMat = NULL;
}

void CRender3D::BindMaterial(IMaterial *pMaterial)
{
	// ToDo
}

DGLE_RESULT DGLE_API CRender3D::SetPerspective(float fFovAngle, float fZNear, float fZFar)
{
	Core()->pRender()->SetPerspective(fFovAngle, fZNear, fZFar);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetPerspective(float &fFovAngle, float &fZNear, float &fZFar)
{
	Core()->pRender()->GetPerspective(fFovAngle, fZNear, fZFar);
	return S_OK;
}
	
DGLE_RESULT DGLE_API CRender3D::SetColor(const TColor4 &stColor)
{
	_stColor = stColor;
	_pCoreRenderer->SetColor(_stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetColor(TColor4 &stColor)
{
	stColor = _stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetBlendMode(E_EFFECT_BLENDING_FLAGS eMode)
{
	switch(eMode)
	{
	case EBF_NORMAL:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_ALPHA;
		break;
	case EBF_ADD:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDstFactor = BF_ONE;
		break;
	case EBF_MULT:
		_stBlendStateDesc.eSrcFactor = BF_ZERO;
		_stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case EBF_BLACK:
		_stBlendStateDesc.eSrcFactor = BF_SRC_COLOR;
		_stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_COLOR;
		break;
	case EBF_WHITE:
		_stBlendStateDesc.eSrcFactor = BF_ONE_MINUS_SRC_COLOR;
		_stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case EBF_MASK:
		_stBlendStateDesc.eSrcFactor = BF_DST_ALPHA;
		_stBlendStateDesc.eDstFactor = BF_ZERO;
		break;
	}

	_pCoreRenderer->SetBlendState(_stBlendStateDesc);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetBlendMode(E_EFFECT_BLENDING_FLAGS &eMode)
{
	if (_stBlendStateDesc.eSrcFactor == BF_SRC_ALPHA && _stBlendStateDesc.eDstFactor == BF_ONE_MINUS_SRC_ALPHA)
		eMode = EBF_NORMAL;
	else
		if (_stBlendStateDesc.eSrcFactor == BF_SRC_ALPHA && _stBlendStateDesc.eDstFactor == BF_ONE)
			eMode = EBF_ADD;
		else
			if (_stBlendStateDesc.eSrcFactor == BF_ZERO && _stBlendStateDesc.eDstFactor == BF_SRC_COLOR)
				eMode = EBF_MULT;
			else
				if (_stBlendStateDesc.eSrcFactor == BF_SRC_COLOR && _stBlendStateDesc.eDstFactor == BF_ONE_MINUS_SRC_COLOR)
					eMode = EBF_BLACK;
				else
					if (_stBlendStateDesc.eSrcFactor == BF_ONE_MINUS_SRC_COLOR && _stBlendStateDesc.eDstFactor == BF_SRC_COLOR)
						eMode = EBF_WHITE;
					else
						if (_stBlendStateDesc.eSrcFactor == BF_DST_ALPHA && _stBlendStateDesc.eDstFactor == BF_ZERO)
							eMode = EBF_MASK;
							
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleAlphaTest(bool bEnabled)
{
	_stRasterStateDesc.bAlphaTestEnable = bEnabled;
	_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetAlphaTreshold(float fValue)
{
	_stRasterStateDesc.fAlphaTestRefValue = fValue;
	_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetAlphaTreshold(float &fValue)
{
	fValue = _stRasterStateDesc.fAlphaTestRefValue;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ClearDepthBuffer()
{
	_pCoreRenderer->Clear(false, true, false);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleDepthTest(bool bEnabled)
{
	_stDepthStencilDesc.bDepthTestEnable = bEnabled;
	_pCoreRenderer->GetDepthStencilState(_stDepthStencilDesc);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::ToggleFog(bool bEnabled)
{
	_stFogDesc.bEnabled = bEnabled;
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetLinearFogBounds(float fStart, float fEnd)
{
	_stFogDesc.fStart = fStart;
	_stFogDesc.fEnd = fEnd;
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetFogColor(const TColor4 &stColor)
{
	_stFogDesc.stColor = stColor;
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetFogDensity(float fValue)
{
	_stFogDesc.fDensity = fValue;
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetLinearFogBounds(float &fStart, float &fEnd)
{
	fStart = _stFogDesc.fStart;
	fEnd = _stFogDesc.fEnd;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetFogColor(TColor4 &stColor)
{
	stColor = _stFogDesc.stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetFogDensity(float &fValue)
{
	fValue = _stFogDesc.fDensity;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetMatrix(const TMatrix4x4 &stMatrix, bool bMult)
{
	if (bMult)
		_matrixStack.MultLocal(stMatrix);
	else
		_matrixStack.Top() = stMatrix;

	_pCoreRenderer->SetMatrix(_matrixStack.Top());

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetMatrix(TMatrix4x4 &stMatrix)
{
	stMatrix = _matrixStack.Top();
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::DrawAxes(float fSize, bool bNoDepthTest)
{
	const float axes[] = {
		0.f, 0.f, 0.f, fSize, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, fSize, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, fSize,
		1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f };
	
	TDrawDataDesc desc;
	desc.pData = (uint8 *)axes;
	desc.uiColorOffset = 72;

	bool dtest = _stDepthStencilDesc.bDepthTestEnable;

	if (dtest && bNoDepthTest)
	{
		_stDepthStencilDesc.bDepthTestEnable = false;
		_pCoreRenderer->SetDepthStencilState(_stDepthStencilDesc);
	}

	PARANOIC_CHECK_RES(_pCoreRenderer->Draw(desc, CRDM_LINES, 6));

	if (dtest && bNoDepthTest)
	{
		_stDepthStencilDesc.bDepthTestEnable = true;
		_pCoreRenderer->SetDepthStencilState(_stDepthStencilDesc);
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::PushStates(E_PUSH_STATES_FLAGS eStates)
{
	_stackStatesFlag.push(eStates);

	if (eStates == PSF_ALL)
		eStates = (E_PUSH_STATES_FLAGS)(PSF_MATRIX | PSF_STATES);

	if (eStates & PSF_MATRIX)
		_matrixStack.Push();

	if (eStates & PSF_STATES)
		_pCoreRenderer->PushStates();

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::PopStates()
{
	E_PUSH_STATES_FLAGS states = _stackStatesFlag.top();

	if (states & PSF_MATRIX)
		_matrixStack.Pop();

	if (states & PSF_STATES)
		_pCoreRenderer->PopStates();

	_stackStatesFlag.pop();

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_FLAG eFlag)
{
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

DGLE_RESULT DGLE_API CRender3D::FrustumSetup()
{
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

	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::CullPoint(const TPoint3 &stCoords, bool &bCull)
{
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
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::SetGlobalAmbientLighting(const TColor4 &stColor)
{
	_stGlobalAmbient = stColor;
	// ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CRender3D::GetGlobalAmbientLighting(TColor4 &stColor)
{
	stColor = _stGlobalAmbient;
	return S_OK;
}