/**
\author		Korotkov Andrey aka DRON
\date		12.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#define __RENDER2D__
#include "Render2D.h"
#include "Core.h"
#include "Render.h"
#include "Render3D.h"

using namespace std;

#define COPY_VERTEX_TO_BUFFER(buffer, start_idx, vertex) \
buffer[start_idx + 0] = vertex.data[0];\
buffer[start_idx + 1] = vertex.data[1];\
buffer[start_idx + 2] = vertex.data[2];\
buffer[start_idx + 3] = vertex.data[3];\
buffer[start_idx + 4] = vertex.data[4];\
buffer[start_idx + 5] = vertex.data[5];\
buffer[start_idx + 6] = vertex.data[6];\
buffer[start_idx + 7] = vertex.data[7];

CRender2D::CRender2D(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_iProfilerState(0), _iDoDrawBBoxes(0),
_bIn2D(false), _bInProfilerMode(false),
_bInLocalBatchMode(false), _bLocalBatchUEP(false), _bLocalUEPWasTurnedOn(false),
_drawDelay(0), _uiObjsDrawnCount(0),
_batchMode(BM_DISABLED),_batchBufferReadyToRender(false),_batchMaxSize(0),_batchMinSize(0),
_batchBufferCurCounter(0), _batchBuffersRepetedUseCounter(0), _batchBuffersNotModefiedPerFrameCounter(0),
_iResCorWidth(0), _iResCorHeight(0), _bResCorConstProp(false), _uiLineWidth(1), _uiPointSize(1),
_ePrevBlendingMode(BE_NORMAL), _pPolyTrisBuffer(NULL), _uiPolyTrisBufferSize(0),
_uiBufferSize(34)// never less than 34
{
	_pBuffer = new float[_uiBufferSize];
	_pCoreRenderer = Core()->pCoreRenderer();
	_pCoreRenderer->IsFeatureSupported(CRFT_GEOMETRY_BUFFER, _bUseGeometryBuffers);

	Console()->RegComVar("rnd2d_profiler", "Displays Render 2D subsystems profiler.", &_iProfilerState, 0, 2);
	Console()->RegComVar("rnd2d_draw_bboxes", "Displays bounding boxes of all 2D objects on screen.", &_iDoDrawBBoxes, 0, 1);
}

CRender2D::~CRender2D()
{
	Console()->UnRegCom("rnd2d_profiler");
	Console()->UnRegCom("rnd2d_draw_bboxes");

	delete[] _pPolyTrisBuffer;
	delete[] _pBuffer;

	for (const auto batchbuffer : _vecBatchBuffers)
		batchbuffer->Free();

	_vecBatchAccumulator.clear();
}

void CRender2D::BeginProfiler2D()
{
	if (_bIn2D) End2D();

	_bInProfilerMode = true;

	Core()->pRender()->pRender3D()->PushSelfStates();
	_pCoreRenderer->PushStates();

	_batchPreProfilerMode = _batchMode;
	_batchMode = BM_DISABLED;

	_SetDefaultStates();

	_pCoreRenderer->GetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	_pCoreRenderer->GetMatrix(_stPrevProjMat, MT_PROJECTION);

	_Set2DProjMatrix(_uiPrevViewPortW, _uiPrevViewPortH);

	_pCoreRenderer->GetMatrix(_stPrevModelViewMat);

	_pCoreRenderer->SetMatrix(MatrixIdentity());
}

void CRender2D::EndProfiler2D()
{
	_pCoreRenderer->SetMatrix(_stPrevProjMat, MT_PROJECTION);
	_pCoreRenderer->SetMatrix(_stPrevModelViewMat);
	
	_batchMode = _batchPreProfilerMode;

	_pCoreRenderer->PopStates();
	Core()->pRender()->pRender3D()->PopSelfStates();

	_bInProfilerMode = false;
}

void CRender2D::DrawProfiler()
{
	if (_iProfilerState > 0)
	{
		Core()->RenderProfilerText("======Render2D Profiler=====", ColorWhite());
		Core()->RenderProfilerText(("Objects on screen :" + to_string(_uiObjsDrawnCount)).c_str(), ColorWhite());
		Core()->RenderProfilerText(("Batches per frame :" + to_string(_batchsCount)).c_str(), _batchsCount > _sc_uiMaxBatchsPerFrame ? ColorRed() : ColorWhite());
		const auto ms = floor<chrono::milliseconds>(_drawAverallDelay);
		Core()->RenderProfilerText(("Render delay      :" + to_string(ms.count()) + '.' + to_string((_drawAverallDelay - ms).count()) + " ms").c_str(), ColorWhite());

		if (_iProfilerState > 1)
		{
			uint  buffs_count = (uint)_vecBatchBuffers.size();

			Core()->RenderProfilerText("--------Batch Render--------", ColorWhite());
			Core()->RenderProfilerText(("Buffers count  :" + to_string(buffs_count)).c_str(), ColorWhite());
			Core()->RenderProfilerText(("Buffers in use :" + to_string(_batchBufferCurCounter)).c_str(), ColorWhite());
			Core()->RenderProfilerText(("Effective calls:" + to_string(_batchBuffersNotModefiedPerFrameCounter)).c_str(), !_batchNeedToRefreshBatches && _batchBuffersNotModefiedPerFrameCounter != _batchBufferCurCounter ? ColorRed() : ColorWhite());
			Core()->RenderProfilerText(("Max. batch size:" + to_string(_batchMaxSize)).c_str(), ColorWhite());
			Core()->RenderProfilerText(("Min. batch size:" + to_string(_batchMinSize == (numeric_limits<uint>::max)() ? 0 : _batchMinSize)).c_str(), ColorWhite());
			Core()->RenderProfilerText("----------------------------", ColorWhite());
		}
	}
}

void CRender2D::_SetDefaultStates()
{
	_stRotationPoint = TPoint2();
	_stScale = TPoint2(1.f, 1.f);
	_astVerticesOffset[0] = _astVerticesOffset[1] = _astVerticesOffset[2] = _astVerticesOffset[3] = TPoint2();
	_stColormix = ColorWhite();

	Core()->pRender()->Unbind(EOT_UNKNOWN);

	_pCoreRenderer->SetColor(ColorWhite());
	
	_uiPointSize = 1;
	_pCoreRenderer->SetPointSize(1.f);

	_uiLineWidth = 1;
	_pCoreRenderer->SetLineWidth(1.f);
	
	TDepthStencilDesc ds_desc;
	ds_desc.bDepthTestEnabled = false;
	_pCoreRenderer->SetDepthStencilState(ds_desc);

	_stRasterStateDesc = TRasterizerStateDesc();
	_stRasterStateDesc.fAlphaTestRefValue = 0.75f;
	_stRasterStateDesc.bFrontCounterClockwise = false;
	_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);

	_stBlendStateDesc = TBlendStateDesc();
	_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	_ePrevBlendingMode = BE_NORMAL;

	Core()->pRender()->pRender3D()->PrepareFor2D();
}

DGLE_RESULT DGLE_API CRender2D::BatchRender(E_BATCH_MODE2D eMode)
{
	if (_bIn2D)
		return E_FAIL;

	_batchMode = eMode;

	switch (eMode)
	{
	case BM_AUTO:	
		if (Core()->InitFlags() & EIF_FORCE_LIMIT_FPS)
			_batchMode = BM_ENABLED_UPDATE_EVERY_FRAME;
		else
			_batchMode = BM_ENABLED_UPDATE_EVERY_TICK;
		break;

	case BM_ENABLED_UPDATE_EVERY_TICK:
		_batchMode = BM_ENABLED_UPDATE_EVERY_TICK;
		break;

	case BM_ENABLED_UPDATE_EVERY_FRAME:
		_batchMode = BM_ENABLED_UPDATE_EVERY_FRAME;
		break;
	}
	
	_pBatchCurTex = NULL;
	(uint&)_eBatchDrawMode = -1;
	_bBatchColor = false;
	_bBatchAlphaTest = false;
	_bBatchBlending = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::InvalidateBatchData()
{
	IN_2D_GUARD

	_BatchFlush();

	_batchNeedToRefreshBatches = true;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::BeginBatch(bool bUpdateEveryFrame)
{
	IN_2D_GUARD

	if (_batchMode > BM_DISABLED)
		return S_FALSE;

	_bInLocalBatchMode = true;

	if (!bUpdateEveryFrame)
	{
		_bLocalBatchUEP = true;
		_bLocalUEPWasTurnedOn = true;
	}
	else
		_bLocalBatchUEP = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::EndBatch()
{
	IN_2D_GUARD

	if (_batchMode > BM_DISABLED)
		return S_FALSE;

	if (!_bInLocalBatchMode)
		return E_FAIL;

	_BatchFlush();

	_bInLocalBatchMode = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::NeedToUpdateBatchData(bool &bNeedUpdate)
{
	IN_2D_GUARD

	bNeedUpdate = _batchMode != BM_ENABLED_UPDATE_EVERY_TICK || (!_bInProfilerMode && (_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)));

	return S_OK;
}

inline bool CRender2D::_BatchSet(E_CORE_RENDERER_DRAW_MODE eDrawMode, ICoreTexture *pTex, bool bColor, bool bAlphaTest, bool bBlending)
{
	if (_eBatchDrawMode != eDrawMode || _pBatchCurTex != pTex ||
		_bBatchColor != bColor || _bBatchAlphaTest != bAlphaTest || _bBatchBlending != bBlending)
	{
		_BatchFlush();
		
		_eBatchDrawMode = eDrawMode;
		_pBatchCurTex = pTex;
		_bBatchColor = bColor;
		_bBatchAlphaTest = bAlphaTest;
		_bBatchBlending = bBlending;

		return true;
	}
	else
		return false;
}

void CRender2D::RefreshBatchData()
{
	_batchNeedToRefreshBatches = true;
}

inline void CRender2D::_BatchFlush()
{
	if (!_batchBufferReadyToRender && _vecBatchAccumulator.size() == 0)
		return;

	TDrawDataDesc desc;

	desc.bVertices2D = true;
	desc.uiVertexStride = 8 * sizeof(float);
	desc.uiTextureVertexOffset = 2 * sizeof(float);
	desc.uiTextureVertexStride = 8 * sizeof(float);
	desc.uiColorOffset = 4 * sizeof(float);
	desc.uiColorStride = 8 * sizeof(float);

	uint size = _vecBatchAccumulator.size();

	if (size != 0)
		desc.pData = (uint8 *)&_vecBatchAccumulator[0];

	if ((_bInLocalBatchMode && _bLocalBatchUEP) || _batchMode == BM_ENABLED_UPDATE_EVERY_TICK )
	{
		++_batchBufferCurCounter;

		ICoreGeometryBuffer *p_buffer = NULL;
		
		if (_batchBufferCurCounter <= _vecBatchBuffers.size())
		{
			p_buffer = _vecBatchBuffers[_batchBufferCurCounter - 1];

			if (_batchNeedToRefreshBatches)
				p_buffer->Reallocate(desc, size, 0, _eBatchDrawMode);
			else
				++_batchBuffersRepetedUseCounter;
		}
		else
			if (size != 0)
			{
				_pCoreRenderer->CreateGeometryBuffer(p_buffer, desc, size, 0, _eBatchDrawMode, _bUseGeometryBuffers ? CRBT_HARDWARE_STATIC : CRBT_SOFTWARE);
				_vecBatchBuffers.push_back(p_buffer);
			}

		if (p_buffer)
		{
			uint tmp;
			p_buffer->GetBufferDimensions(tmp, size, tmp, tmp);
			_pCoreRenderer->DrawBuffer(p_buffer);
		}
	}
	else
		_pCoreRenderer->Draw(desc, _eBatchDrawMode, size);
	
	switch (_eBatchDrawMode)
	{
	case CRDM_POINTS:
		_uiObjsDrawnCount += size;
		break;

	case CRDM_LINES:
		_uiObjsDrawnCount += size / 2;
		break;

	case CRDM_TRIANGLES:
		_uiObjsDrawnCount += size / 6;
		break;

	default:
		LOG("Unallowed draw mode " + ToStrX(_eBatchDrawMode) + " passed to 2D batching system! This is program logic issue, please report to engine developers.", LT_FATAL);
	}

	_batchMaxSize = max(_batchMaxSize, size);
	_batchMinSize = min(_batchMinSize, size);

	_vecBatchAccumulator.clear();

	_batchBufferReadyToRender = false;

	++_batchsCount;
}

void CRender2D::_Set2DProjMatrix(uint width, uint height)
{
	_pCoreRenderer->SetMatrix(TMatrix4(
		2.f / (float)width, 0.f, 0.f, 0.f,
		0.f, -2.f / (float)height, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f
		), MT_PROJECTION);
}

DGLE_RESULT DGLE_API CRender2D::Begin2D()
{
	if (_bIn2D)
		return E_FAIL;
			
	_drawDelay = GetPerfTimer();

	_pCoreRenderer->PushStates();
	Core()->pRender()->pRender3D()->PushSelfStates();

	_SetDefaultStates();

	_bViewportChanged = false;

	_pCoreRenderer->GetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	if (_iResCorWidth + _iResCorHeight != 0)
	{
		if (_bResCorConstProp && fabs((float)_uiPrevViewPortW / (float)_uiPrevViewPortH - _fResCorCoef) > 0.001)
		{
			_uiCropW = (int)((float)_uiPrevViewPortH * _fResCorCoef);

			if (_uiPrevViewPortW > _uiCropW)
				_uiCropH = _uiPrevViewPortH;
			else
			{
				_uiCropH = (int)((float)_uiPrevViewPortW / _fResCorCoef);
				_uiCropW = _uiPrevViewPortW;
			}

			_uiCropX = (_uiPrevViewPortW - _uiCropW) / 2;
			_uiCropY = (_uiPrevViewPortH - _uiCropH) / 2;

			_pCoreRenderer->SetViewport(_uiCropX, _uiCropY, _uiCropW, _uiCropH);

			_bViewportChanged = true;
		}
		else
		{
			_uiCropX = 0;
			_uiCropY = 0;
		}

		_uiScreenWidth = _iResCorWidth;
		_uiScreenHeight	= _iResCorHeight;
	}
	else
	{
		_uiCropX = 0;
		_uiCropY = 0;
		_uiScreenWidth = _uiPrevViewPortW;
		_uiScreenHeight	= _uiPrevViewPortH;
	}

	_pCoreRenderer->GetMatrix(_stPrevProjMat, MT_PROJECTION);

	_Set2DProjMatrix(_uiScreenWidth, _uiScreenHeight);

	_pCoreRenderer->GetMatrix(_stPrevModelViewMat);

	_pCoreRenderer->SetMatrix(MatrixIdentity());

	_bIn2D = true;
	_bCameraWasSet = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::End2D()
{
	if (!_bIn2D)
		return E_FAIL;

	_BatchFlush();

	_bInLocalBatchMode = false;

	(uint&)_eBatchDrawMode = -1;

	if (_bCameraWasSet)
		_pCoreRenderer->SetMatrix(_stPrevCamTransform);

	_batchBuffersNotModefiedPerFrameCounter = _batchBuffersRepetedUseCounter;

	if (_bViewportChanged)
		_pCoreRenderer->SetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	_pCoreRenderer->SetMatrix(_stPrevProjMat, MT_PROJECTION);
	_pCoreRenderer->SetMatrix(_stPrevModelViewMat);
	
	Core()->pRender()->pRender3D()->PopSelfStates();
	_pCoreRenderer->PopStates();

	_bIn2D = false;

	_drawDelay = GetPerfTimer() - _drawDelay;
	_drawAverallDelay += _drawDelay;

	return S_OK;
}

void CRender2D::BeginFrame()
{
	if (_batchMode != BM_ENABLED_UPDATE_EVERY_TICK && !_bLocalUEPWasTurnedOn)
		_batchNeedToRefreshBatches = true;

	_bLocalUEPWasTurnedOn = false;

	_batchBufferCurCounter = 0;
	_batchBuffersRepetedUseCounter = 0;
	_batchsCount = 0;
	_uiObjsDrawnCount = 0;

	_batchMinSize = (numeric_limits<uint>::max)();
	_batchMaxSize = 0;

	_drawAverallDelay = {};
}

void CRender2D::EndFrame()
{
	if (_bIn2D) End2D();

	_batchNeedToRefreshBatches = false;
}

DGLE_RESULT DGLE_API CRender2D::SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions)
{
	_iResCorWidth = uiResX;
	_iResCorHeight = uiResY;
	_bResCorConstProp = bConstaintProportions;
	_fResCorCoef = (float)_iResCorWidth / (float)_iResCorHeight;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::ResolutionCorrectToAbsolute(const TPoint2 &stLogicCoord, TPoint2 &stAbsoluteCoord)
{
	stAbsoluteCoord.x = stLogicCoord.x * ((float)_uiPrevViewPortW - _uiCropX * 2.f) / (float)_uiScreenWidth + (float)_uiCropX;
	stAbsoluteCoord.y = stLogicCoord.y * ((float)_uiPrevViewPortH - _uiCropY * 2.f) / (float)_uiScreenHeight + (float)_uiCropY;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::AbsoluteToResolutionCorrect(const TPoint2 &stAbsoluteCoord, TPoint2 &stLogicCoord)
{
	stLogicCoord.x = (stAbsoluteCoord.x - (float)_uiCropX) * (float)_uiScreenWidth / ((float)_uiPrevViewPortW - _uiCropX * 2.f);
	stLogicCoord.y = (stAbsoluteCoord.y - (float)_uiCropY) * (float)_uiScreenHeight / ((float)_uiPrevViewPortH - _uiCropY * 2.f);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetCamera(const TPoint2 &stCenter, float fAngle, const TVector2 &stScale)
{
	IN_2D_GUARD

	_BatchFlush();

	if (stCenter.x == (float)_uiScreenWidth / 2.f && stCenter.y == (float)_uiScreenHeight / 2.f && fAngle == 0.f && stScale.x == 1.f && stScale.y == 1.f)
	{
		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(_stPrevCamTransform);

		_bCameraWasSet = false;
	}
	else
	{
		if (!_bCameraWasSet)
			_pCoreRenderer->GetMatrix(_stPrevCamTransform);

		_stCamScale	= stScale;
		_stCamTransform = MatrixIdentity();
	
		if (fAngle != 0.f || stScale.x != 1.f || stScale.y != 1.f)
		{
			_stCamTransform = MatrixTranslate(TVector3((float)_uiScreenWidth / 2.f, (float)_uiScreenHeight / 2.f, 0.f)) * _stCamTransform;

			if (_stCamScale.x != 1.f || _stCamScale.y != 1.f)
				_stCamTransform = MatrixScale(TVector3(stScale.x, stScale.y, 1.f)) * _stCamTransform;

			if (fAngle != 0.f)
				_stCamTransform = MatrixRotate(fAngle, TVector3(0.f, 0.f, 1.f)) * _stCamTransform;

			_stCamTransform = MatrixTranslate(TVector3(-(float)_uiScreenWidth / 2.f, -(float)_uiScreenHeight / 2.f, 0.f)) * _stCamTransform;
		}

		_stCamTransform = MatrixTranslate(TVector3((float)_uiScreenWidth / 2.f - stCenter.x, (float)_uiScreenHeight / 2.f - stCenter.y, 0.f)) * _stCamTransform;

		_pCoreRenderer->SetMatrix(_stCamTransform);

		_bCameraWasSet = true;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::ResetCamera()
{
	IN_2D_GUARD

	if (!_bCameraWasSet)
		return S_OK;

	_BatchFlush();

	_pCoreRenderer->SetMatrix(_stPrevCamTransform);

	_bCameraWasSet = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::UnprojectCameraToScreen(const TPoint2 &stCameraCoord, TPoint2 &stScreenCoord)
{
	IN_2D_GUARD

	if (!_bCameraWasSet)
	{
		stScreenCoord = stCameraCoord;
		return S_OK;
	}

	stScreenCoord = _stCamTransform.ApplyToPoint(stCameraCoord);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::ProjectScreenToCamera(const TPoint2 &stScreenCoord, TPoint2 &stCameraCoord)
{
	IN_2D_GUARD

	if (!_bCameraWasSet)
	{
		stCameraCoord = stScreenCoord;
		return S_OK;
	}
	
	stCameraCoord = MatrixInverse(_stCamTransform).ApplyToPoint(stScreenCoord);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull)
{
	IN_2D_GUARD

	_pBuffer[0] = stBBox.x; _pBuffer[1] = stBBox.y;
	_pBuffer[2] = stBBox.x + stBBox.width; _pBuffer[3] = _pBuffer[1];
	_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = stBBox.y + stBBox.height;
	_pBuffer[6] = _pBuffer[0]; _pBuffer[7] = _pBuffer[5];
	_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

	TMatrix4 transform;

	if (fAngle != 0.f)
	{
		TMatrix4 rot = MatrixIdentity();
		
		const float s = sinf(-fAngle * (float)M_PI / 180.f), c = cosf(-fAngle * (float)M_PI / 180.f);

		rot._2D[0][0] = c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = s;
		rot._2D[1][1] = c;

		transform = MatrixTranslate(TVector3(-(stBBox.x + stBBox.width / 2.f), -(stBBox.y + stBBox.height / 2.f), 0.f)) * rot * MatrixTranslate(TVector3(stBBox.x + stBBox.width / 2.f, stBBox.y + stBBox.height / 2.f, 0.f));

		float x = _pBuffer[0], y = _pBuffer[1];
		_pBuffer[0]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		_pBuffer[1]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		 x = _pBuffer[2]; y = _pBuffer[3];
		_pBuffer[2]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		_pBuffer[3]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		x = _pBuffer[4]; y = _pBuffer[5];
		_pBuffer[4]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		_pBuffer[5]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		x = _pBuffer[6]; y = _pBuffer[7];
		_pBuffer[6]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		_pBuffer[7]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];
	}

	const int prev = _iDoDrawBBoxes;
	_iDoDrawBBoxes = 0;

	bCull = !BBoxInScreen(_pBuffer, false);

	_iDoDrawBBoxes = prev;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetLineWidth(uint uiWidth)
{
	IN_2D_GUARD

	if (_uiLineWidth != uiWidth)
		_BatchFlush();

	_uiLineWidth = uiWidth;
	_pCoreRenderer->SetLineWidth((float)uiWidth);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize)
{
	IN_2D_GUARD

	const bool do_blending = stColor.a < 1.f,
		do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_POINTS, NULL, true, false, do_blending) || (_batchMode > BM_DISABLED && _uiPointSize != uiSize);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float quad[] = {
		stCoords.x - uiSize / 2.f, stCoords.y - uiSize / 2.f,
		stCoords.x + uiSize / 2.f, stCoords.y - uiSize / 2.f,
		stCoords.x + uiSize / 2.f, stCoords.y + uiSize / 2.f,
		stCoords.x - uiSize / 2.f, stCoords.y + uiSize / 2.f
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	if (_uiPointSize != uiSize)
	{
		_BatchFlush();
		_uiPointSize = uiSize;
		_pCoreRenderer->SetPointSize((float)uiSize);
	}

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stBlendStateDesc.bEnabled = do_blending;
		_stRasterStateDesc.bAlphaTestEnabled = false;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
			_vecBatchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f ,0.f , stColor.r, stColor.g, stColor.b, stColor.a));
	else
	{
		_pCoreRenderer->SetColor(stColor);

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)stCoords.xy, -1, -1, true), CRDM_POINTS, 1);
		
		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	const bool do_blending = stColor.a < 1.f || eFlags & PF_VERTICES_COLORS,
		do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_LINES, NULL, true, false, do_blending);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	const float quad[] = {
		fmin(stCoords1.x, stCoords2.x), fmin(stCoords1.y, stCoords2.y),
		fmax(stCoords1.x, stCoords2.x), fmin(stCoords1.y, stCoords2.y),
		fmax(stCoords1.x, stCoords2.x), fmax(stCoords1.y, stCoords2.y),
		fmin(stCoords1.x, stCoords2.x), fmax(stCoords1.y, stCoords2.y)
	};
	
	if (!BBoxInScreen(quad, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = do_blending;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);

		_pCoreRenderer->SetColor(stColor);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			if (eFlags & PF_VERTICES_COLORS)
			{
				_vecBatchAccumulator.push_back(TVertex2(stCoords1.x, stCoords1.y, 0.f ,0.f , _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a));
				_vecBatchAccumulator.push_back(TVertex2(stCoords2.x, stCoords2.y, 0.f ,0.f , _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a));
			}
			else
			{
				_vecBatchAccumulator.push_back(TVertex2(stCoords1.x, stCoords1.y, 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				_vecBatchAccumulator.push_back(TVertex2(stCoords2.x, stCoords2.y, 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
			}
		}
	else
	{
		_pBuffer[0] = stCoords1.x; _pBuffer[1] = stCoords1.y;
		_pBuffer[2] = stCoords2.x; _pBuffer[3] = stCoords2.y;

		TDrawDataDesc desc((uint8*)_pBuffer, -1, -1, true);

		if (eFlags & PF_VERTICES_COLORS)
		{
			_pBuffer[4] = _astVerticesColors[2].r; _pBuffer[5] = _astVerticesColors[2].g; _pBuffer[6] = _astVerticesColors[2].b; _pBuffer[7] = _astVerticesColors[2].a;
			_pBuffer[8] = _astVerticesColors[0].r; _pBuffer[9] = _astVerticesColors[0].g; _pBuffer[10] = _astVerticesColors[0].b; _pBuffer[11] = _astVerticesColors[0].a;
			desc.uiColorOffset = 4 * sizeof(float);
		}

		_pCoreRenderer->Draw(desc, CRDM_LINES, 2);

		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawRectangle(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	const bool do_blending = stColor.a < 1.f || eFlags & PF_VERTICES_COLORS,
		do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, NULL, true, false, do_blending);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	_pBuffer[0] = stRect.x;
	_pBuffer[1] = stRect.y;
	_pBuffer[2] = stRect.x + stRect.width;
	_pBuffer[3] = _pBuffer[1];
	_pBuffer[4] = _pBuffer[2];
	_pBuffer[5] = stRect.y + stRect.height;
	_pBuffer[6] = _pBuffer[0];
	_pBuffer[7] = _pBuffer[5];
	_pBuffer[8] = _pBuffer[0];
	_pBuffer[9] = _pBuffer[1];

	if (!BBoxInScreen(_pBuffer, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = do_blending;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);

		_pCoreRenderer->SetColor(stColor);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			if (eFlags & PF_FILL)
			{
				_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

				_pBuffer[0] = _pBuffer[2];	_pBuffer[1] = _pBuffer[3];
				_pBuffer[2] = _pBuffer[4];	_pBuffer[3] = _pBuffer[5];
				_pBuffer[4] = _pBuffer[8];	_pBuffer[5] = _pBuffer[9];
			}

			if (eFlags & PF_VERTICES_COLORS)
				if (eFlags & PF_FILL)
				{
					const uint idx = _vecBatchAccumulator.size();
					_vecBatchAccumulator.resize(_vecBatchAccumulator.size() + 6);
					
					_vecBatchAccumulator[idx + 0] = TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a);
					_vecBatchAccumulator[idx + 1] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 2] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
					_vecBatchAccumulator[idx + 3] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 4] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
					_vecBatchAccumulator[idx + 5] = TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a);
				}
				else
				{
					const uint idx = _vecBatchAccumulator.size();
					_vecBatchAccumulator.resize(_vecBatchAccumulator.size() + 8);

					_vecBatchAccumulator[idx + 0] = TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
					_vecBatchAccumulator[idx + 1] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a);
					_vecBatchAccumulator[idx + 2] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 3] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a);
					_vecBatchAccumulator[idx + 4] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 5] = TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a);
					_vecBatchAccumulator[idx + 6] = TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a);
					_vecBatchAccumulator[idx + 7] = TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
				}
			else
			{
				const uint idx = _vecBatchAccumulator.size();
				_vecBatchAccumulator.resize(_vecBatchAccumulator.size() + 6);

				_vecBatchAccumulator[idx + 0] = TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);
				_vecBatchAccumulator[idx + 1] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);
				_vecBatchAccumulator[idx + 2] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);
				_vecBatchAccumulator[idx + 3] = TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);
				_vecBatchAccumulator[idx + 4] = TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);
				_vecBatchAccumulator[idx + 5] = TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a);

				if (!(eFlags & PF_FILL))
				{
					_vecBatchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
					_vecBatchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				}
			}
		}
	else
	{
		if (eFlags & PF_FILL)
		{
			_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

			_pBuffer[0] = _pBuffer[2]; _pBuffer[1] = _pBuffer[3];
			_pBuffer[2] = _pBuffer[4]; _pBuffer[3] = _pBuffer[5];
			_pBuffer[4] = _pBuffer[8]; _pBuffer[5] = _pBuffer[9];
		}

		TDrawDataDesc desc((uint8*)_pBuffer, -1, -1, true);

		if (eFlags & PF_VERTICES_COLORS)
		{
			if (eFlags & PF_FILL)
			{
				COPY_COLOR_TO_BUFFER(_pBuffer, 10, _astVerticesColors[0])
				COPY_COLOR_TO_BUFFER(_pBuffer, 14, _astVerticesColors[1])
				COPY_COLOR_TO_BUFFER(_pBuffer, 18, _astVerticesColors[2])
				COPY_COLOR_TO_BUFFER(_pBuffer, 22, _astVerticesColors[3])
			}
			else
			{
				_pBuffer[10] = _astVerticesColors[2].r; _pBuffer[11] = _astVerticesColors[2].g; _pBuffer[12] = _astVerticesColors[2].b; _pBuffer[13] = _astVerticesColors[2].a;
				_pBuffer[14] = _astVerticesColors[0].r; _pBuffer[15] = _astVerticesColors[0].g; _pBuffer[16] = _astVerticesColors[0].b; _pBuffer[17] = _astVerticesColors[0].a;
				_pBuffer[18] = _astVerticesColors[1].r; _pBuffer[19] = _astVerticesColors[1].g; _pBuffer[20] = _astVerticesColors[1].b; _pBuffer[21] = _astVerticesColors[1].a;
				_pBuffer[22] = _astVerticesColors[3].r; _pBuffer[23] = _astVerticesColors[3].g; _pBuffer[24] = _astVerticesColors[3].b; _pBuffer[25] = _astVerticesColors[3].a;
				_pBuffer[26] = _astVerticesColors[2].r; _pBuffer[27] = _astVerticesColors[2].g; _pBuffer[28] = _astVerticesColors[2].b; _pBuffer[29] = _astVerticesColors[2].a;
			}

			desc.uiColorOffset = 10 * sizeof(float);
		}

		_pCoreRenderer->Draw(desc, eFlags & PF_FILL ? CRDM_TRIANGLE_STRIP : CRDM_LINE_STRIP, eFlags & PF_FILL ? 4 : 5);

		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	return DrawEllipse(stCoords, TPoint2((float)uiRadius, (float)uiRadius), uiQuality, stColor, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::DrawEllipse(const TPoint2 &stCoords, const TVector2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	const bool do_blending = stColor.a < 1.f,
		do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, NULL, true, false, do_blending);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float quad[] = {
		stCoords.x - stRadius.x, stCoords.y - stRadius.y,
		stCoords.x + stRadius.x, stCoords.y - stRadius.y,
		stCoords.x + stRadius.x, stCoords.y + stRadius.y,
		stCoords.x - stRadius.x, stCoords.y + stRadius.y
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = do_blending;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);

		_pCoreRenderer->SetColor(stColor);
	}

	if (uiQuality > 360 / 2) uiQuality = 360 / 2;
	if (uiQuality < 4) uiQuality = 4;
	float k = 360.f / uiQuality;

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			_vecBatchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			
			if (eFlags & PF_FILL)
				_vecBatchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			
			_vecBatchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x * cosf(k * (float)M_PI / 180.f), stCoords.y + stRadius.y * sinf(k * (float)M_PI / 180.f), 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));

			for (uint i = 2; i <= uiQuality; ++i)
			{
				if (eFlags & PF_FILL)
				{
					_vecBatchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
					_vecBatchAccumulator.push_back(_vecBatchAccumulator[_vecBatchAccumulator.size() - 2]);
				}
				else
					_vecBatchAccumulator.push_back(_vecBatchAccumulator[_vecBatchAccumulator.size() - 1]);

				_vecBatchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x * cosf(i * k * (float)M_PI / 180.f), stCoords.y + stRadius.y * sinf(i * k * (float)M_PI / 180.f),
					0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			}
		}
	else
	{
		if (_uiBufferSize < uiQuality * 2 + 4)
		{
			_uiBufferSize = uiQuality * 2 + 4;
			delete[] _pBuffer;
			_pBuffer = new float[_uiBufferSize];
		}

		if (eFlags & PF_FILL)
		{
			_pBuffer[0] = stCoords.x;
			_pBuffer[1] = stCoords.y;
		}

		for (uint i = 0; i <= uiQuality; ++i)
		{
			_pBuffer[2 + i * 2] = stCoords.x + stRadius.x * cosf(i * k * (float)M_PI / 180.f);
			_pBuffer[3 + i * 2] = stCoords.y + stRadius.y * sinf(i * k * (float)M_PI / 180.f);
		}

		_pCoreRenderer->Draw(TDrawDataDesc((uint8 *)&_pBuffer[eFlags & PF_FILL ? 0 : 2], -1, -1, true), eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, eFlags & PF_FILL ? uiQuality + 2 : uiQuality + 1);

		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawPolygon(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	// Triangulation //

	struct TTriangle
	{
		uint16 index[3];
	};

	struct TVector2Ex : TVector2
	{
		TVector2Ex() {}

		TVector2Ex(const TPoint2 &p) : TPoint2(p) {}

		TVector2Ex(const TVertex2 &v) : TPoint2(v.data) {}

		TVector2Ex(float x, float y) : TPoint2(x, y) {}

		TVector2Ex Diff(const TVector2Ex &p) const
		{
			return TPoint2(x - p.x, y - p.y);
		}

		TVector2Ex &OrthoCCW()
		{
			return *new(this) TVector2Ex(-y, x);
		}

		TVector2Ex OrthoCCW() const
		{
			return TVector2Ex(*this).OrthoCCW();
		}

		TVector2Ex &OrthoCW()
		{
			return *new(this) TVector2Ex(y, -x);
		}

		TVector2Ex OrthoCW() const
		{
			return TVector2Ex(*this).OrthoCW();
		}

	};

	struct TFContainer
	{
	private:
		static inline uint GetNextActive(uint x, uint vertexCount, const bool *active)
		{
			for (;;)
			{
				if (++x == vertexCount) x = 0;
				if (active[x]) return x;
			}
		}

		static inline uint GetPrevActive(uint x, uint vertexCount, const bool *active)
		{
			for (;;)
			{
				if (--x == -1) x = vertexCount - 1;
				if (active[x]) return x;
			}
		}

	public:
		static uint TriangulatePolygon(bool *active, uint vertexCount, const TVertex2 *vertex, bool ccw, TTriangle *triangle)
		{
			const float c_epsilon = 0.001f;

			fill_n(active, vertexCount, true);

			uint triangleCount = 0, start = 0,
				p1 = 0, p2 = 1,
				m1 = vertexCount - 1, m2 = vertexCount - 2;

			bool last_positive = false;
			for (;;)
			{
				if (p2 == m2) // Only three vertices remains.
				{
					triangle->index[0] = (uint16)m1;
					triangle->index[1] = (uint16)p1;
					triangle->index[2] = (uint16)p2;
					++triangleCount;
					break;
				}

				const TVector2Ex c_vp1 = vertex[p1], c_vp2 = vertex[p2],
				c_vm1 = vertex[m1], c_vm2 = vertex[m2];
				bool  positive = false, negative = false;

				// Determine whether c_vp1, c_vp2 and c_vm1 form a valid triangle.
				TVector2Ex n1 = c_vm1.Diff(c_vp2).Normalize();
				
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				
				if (n1.Dot(c_vp1.Diff(c_vp2)) > c_epsilon)
				{
					positive = true;
					TVector2Ex n2 = c_vp1.Diff(c_vm1).Normalize();
					TVector2Ex n3 = c_vp2.Diff(c_vp1).Normalize();
					
					if (ccw)
					{
						n2.OrthoCCW();
						n3.OrthoCCW();
					}
					else
					{
						n2.OrthoCW();
						n3.OrthoCW();
					}

					for (uint a = 0; a < vertexCount; ++a)
					{
						// Look for other vertices inside the triangle.
						if (active[a] && a != p1 && a != p2 && a != m1)
						{
							const TVector2Ex c_v = vertex[a];
							if (n1.Dot(c_v.Diff(c_vp2).Normalize()) > -c_epsilon &&
								n2.Dot(c_v.Diff(c_vm1).Normalize()) > -c_epsilon &&
								n3.Dot(c_v.Diff(c_vp1).Normalize()) > -c_epsilon)
							{
								positive = false;
								break;
							}
						}
					}
				}

				// Determine whether c_vm1, c_vm2 and c_vp1 form a valid triangle.
				n1 = c_vm2.Diff(c_vp1).Normalize();
				
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				
				if (n1.Dot(c_vm1.Diff(c_vp1)) > c_epsilon)
				{
					negative = true;
					TVector2Ex n2 = (c_vm1.Diff(c_vm2)).Normalize();
					TVector2Ex n3 = (c_vp1.Diff(c_vm1)).Normalize();
					
					if (ccw)
					{
						n2.OrthoCCW();
						n3.OrthoCCW();
					}
					else
					{
						n2.OrthoCW();
						n3.OrthoCW();
					}

					for (uint a = 0; a < vertexCount; ++a)
					{
						// Look for other vertices inside the triangle.
						if (active[a] && a != m1 && a != m2 && a != p1)
						{
							const TVector2Ex v = vertex[a];
							if (n1.Dot(v.Diff(c_vp1).Normalize()) > -c_epsilon &&
								n2.Dot(v.Diff(c_vm2).Normalize()) > -c_epsilon &&
								n3.Dot(v.Diff(c_vm1).Normalize()) > -c_epsilon)
							{
								negative = false;
								break;
							}
						}
					}
				}

				// If both triangles valid, choose the one having the larger smallest angle.
				if (positive && negative)
				{
					float pd = (c_vp2.Diff(c_vm1)).Normalize().Dot(c_vm2.Diff(c_vm1).Normalize());
					float md = (c_vm2.Diff(c_vp1)).Normalize().Dot(c_vp2.Diff(c_vp1).Normalize());

					if (abs(pd - md) < c_epsilon)
					{
						if (last_positive)
							positive = false;
						else
							negative = false;
					}
					else
					{
						if (pd < md)
							negative = false;
						else
							positive = false;
					}
				}

				if (positive)
				{
					// Output the triangle m1, p1, p2.
					active[p1] = false;
					triangle->index[0] = (uint16)m1;
					triangle->index[1] = (uint16)p1;
					triangle->index[2] = (uint16)p2;
					++triangleCount;
					++triangle;

					p1 = GetNextActive(p1, vertexCount, active);
					p2 = GetNextActive(p2, vertexCount, active);
					last_positive = true;
					start = -1;
				}
				else
					if (negative)
					{
						// Output the triangle m2, m1, p1.
						triangle->index[0] = (uint16)m2;
						triangle->index[1] = (uint16)m1;
						triangle->index[2] = (uint16)p1;
						++triangleCount;
						++triangle;

						m1 = GetPrevActive(m1, vertexCount, active);
						m2 = GetPrevActive(m2, vertexCount, active);
						last_positive = false;
						start = -1;
					}
					else
					{
						// Exit if we've gone all the way aroud the polygon without finding a valid triangle.
						if (start == -1) start = p2;
						else if (p2 == start) break;

						// Advance working set of vertices.
						m2 = m1;
						m1 = p1;
						p1 = p2;
						p2 = GetNextActive(p2, vertexCount, active);
					}
			}

			return triangleCount;
		}
	};

	IN_2D_GUARD

	if (((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && !pstVertices) || uiVerticesCount < 3)
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	const bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, p_tex, true, false, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; ++i)
	{
		max_x = fmax(pstVertices[i].x, max_x);
		max_y = fmax(pstVertices[i].y, max_y);
		min_x = fmin(pstVertices[i].x, min_x);
		min_y = fmin(pstVertices[i].y, min_y);
	}

	const float quad[] = {
		min_x, min_y,
		max_x, min_y,
		max_x, max_y,
		min_x, max_y
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(p_tex);

		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = true;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	}

	if (eFlags & PF_FILL)
	{
		if (do_batch_update && !_batchNeedToRefreshBatches && !(_bInLocalBatchMode && !_bLocalBatchUEP))
		{		
			_batchBufferReadyToRender = true;
			return S_OK;
		}

		float angle = 0;
		TVector2Ex e1 = (TVector2Ex(pstVertices[0]).Diff(pstVertices[uiVerticesCount - 1])).Normalize(), e2;
		for (uint vert_idx = 0; vert_idx < uiVerticesCount; e1 = e2, ++vert_idx)
		{
			e2 = TVector2Ex(TVector2Ex(pstVertices[(vert_idx + 1) % uiVerticesCount]).Diff(pstVertices[vert_idx])).Normalize();
			const float s = e1.Cross(e2), c = e1.Dot(e2);
			angle += atan2(s, c);
		}

		if (_uiPolyTrisBufferSize < uiVerticesCount)
		{
			_uiPolyTrisBufferSize = uiVerticesCount;
			delete[] _pPolyTrisBuffer;
			_pPolyTrisBuffer = new uint8[sizeof(TTriangle) * (2 * uiVerticesCount - 2)];
		}

		TTriangle *tris = reinterpret_cast<TTriangle *>(&_pPolyTrisBuffer[uiVerticesCount]);
		int32 tri_count;

#ifdef PLATFORM_WINDOWS
		__try
		{
#endif
		tri_count = TFContainer::TriangulatePolygon(reinterpret_cast<bool *>(_pPolyTrisBuffer), uiVerticesCount, pstVertices, angle > 0, tris);
#ifdef PLATFORM_WINDOWS
		}
		__except(EXCEPTION_EXECUTE_HANDLER)\
		{
			return E_INVALIDARG;
		}
#endif
		_2D_IF_BATCH_NO_UPDATE_EXIT
			else
				for (int32 tri_idx = 0; tri_idx < tri_count; ++tri_idx)
					for (const auto idx : tris[tri_idx].index)
						_vecBatchAccumulator.push_back(pstVertices[idx]);
		else
		{
			if (_uiBufferSize < (uint)8 * tri_count * 3)
			{
				_uiBufferSize = 8 * tri_count * 3;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (int32 tri_idx = 0; tri_idx < tri_count; ++tri_idx)
				for (uint_fast8_t v = 0; v < 3; ++v)
				{
					const uint idx_1 = tri_idx * 8 * 3 + v * 8;
					const uint16 idx_2 = tris[tri_idx].index[v];
					COPY_VERTEX_TO_BUFFER(_pBuffer, idx_1, pstVertices[idx_2])
				}

			TDrawDataDesc desc;

			desc.pData = (uint8 *)_pBuffer;
			desc.bVertices2D = true;
			desc.uiVertexStride = 8 * sizeof(float);
			desc.uiTextureVertexOffset = 2 * sizeof(float);
			desc.uiTextureVertexStride = 8 * sizeof(float);
			desc.uiColorOffset = 4 * sizeof(float);
			desc.uiColorStride = 8 * sizeof(float);

			_pCoreRenderer->Draw(desc, CRDM_TRIANGLES, tri_count * 3);

			++_uiObjsDrawnCount;
		}
	}
	else //Draw Line
	{
		_2D_IF_BATCH_NO_UPDATE_EXIT
			else
			{
				_vecBatchAccumulator.push_back(pstVertices[0]);
				_vecBatchAccumulator.push_back(pstVertices[1]);

				for (uint i = 2; i < uiVerticesCount; ++i)
				{
					_vecBatchAccumulator.push_back(pstVertices[i - 1]);
					_vecBatchAccumulator.push_back(pstVertices[i]);
				}

				_vecBatchAccumulator.push_back(pstVertices[uiVerticesCount - 1]);
				_vecBatchAccumulator.push_back(pstVertices[0]);
			}
		else
		{
			if (_uiBufferSize < (uiVerticesCount + 1) * 8)
			{
				_uiBufferSize = (uiVerticesCount + 1) * 8;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			memcpy(_pBuffer, pstVertices, uiVerticesCount * sizeof(TVertex2));
			COPY_VERTEX_TO_BUFFER(_pBuffer, uiVerticesCount * 8, pstVertices[0])

			TDrawDataDesc desc;

			desc.pData = (uint8 *)_pBuffer;
			desc.bVertices2D = true;
			desc.uiVertexStride = 8 * sizeof(float);
			desc.uiTextureVertexOffset = 2 * sizeof(float);
			desc.uiTextureVertexStride = 8 * sizeof(float);
			desc.uiColorOffset = 4 * sizeof(float);
			desc.uiColorStride = 8 * sizeof(float);

			_pCoreRenderer->Draw(desc, CRDM_LINE_STRIP, uiVerticesCount + 1);

			++_uiObjsDrawnCount;
		}
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawTriangles(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if (((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && !pstVertices) || uiVerticesCount % 3 != 0)
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	const bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, p_tex, true, false, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; ++i)
	{
		max_x = fmax(pstVertices[i].x, max_x);
		max_y = fmax(pstVertices[i].y, max_y);
		min_x = fmin(pstVertices[i].x, min_x);
		min_y = fmin(pstVertices[i].y, min_y);
	}

	const float quad[] = {
		min_x, min_y,
		max_x, min_y,
		max_x, max_y,
		min_x, max_y
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(p_tex);

		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = true;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	}
	
	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
			for (uint i = 0; i < uiVerticesCount / 3; ++i)
				for (uint_fast8_t j = 0; j < 3; ++j)
					{
						if (!(eFlags & PF_FILL) && j > 0)
							_vecBatchAccumulator.push_back(pstVertices[i * 3 + j - 1]);

						_vecBatchAccumulator.push_back(pstVertices[i * 3 + j]);
					}
	else
	{
		TDrawDataDesc desc;
		
		if (!(eFlags & PF_FILL))
		{
			if (_uiBufferSize < uiVerticesCount * 8 * 2)
			{
				_uiBufferSize = uiVerticesCount * 8 * 2;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (uint i = 0; i < uiVerticesCount; ++i)
			{
				const uint idx_1 = i * 8 * 2;
				
				COPY_VERTEX_TO_BUFFER(_pBuffer, idx_1, pstVertices[i])

				uint idx_2;

				if (i != 0 && (i + 1) % 3 == 0)
					idx_2 = i - 2;
				else
					idx_2 = i + 1;

				COPY_VERTEX_TO_BUFFER(_pBuffer, idx_1 + 8, pstVertices[idx_2])
			}

			desc.pData = (uint8 *)_pBuffer;
		}
		else
			desc.pData = (uint8 *)pstVertices;
				
		desc.bVertices2D = true;
		desc.uiVertexStride = 8 * sizeof(float);
		desc.uiTextureVertexOffset = 2 * sizeof(float);
		desc.uiTextureVertexStride = 8 * sizeof(float);
		desc.uiColorOffset = 4 * sizeof(float);
		desc.uiColorStride = 8 * sizeof(float);

		_pCoreRenderer->Draw(desc, eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, eFlags & PF_FILL ? uiVerticesCount : uiVerticesCount*2);

		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis, float fAngle, E_EFFECT2D_FLAGS eFlags, bool bClip, float fFovY, bool bClearDepthBuffer)
{
	if (!pMesh)
		return E_INVALIDARG;

	ICoreGeometryBuffer *p_buff;

	pMesh->GetGeometryBuffer(p_buff);

	TPoint3 center;
	TVector3 extents;

	pMesh->GetCenter(center);
	pMesh->GetExtents(extents);

	return DrawBuffer3D(pTexture, p_buff, eFlags, MatrixScale(stDimensions) * (fAngle == 0.f ? MatrixIdentity() : MatrixRotate(fAngle, stAxis)) * MatrixTranslate(stCoords), center, extents, bClip, fFovY, bClearDepthBuffer);
}

DGLE_RESULT DGLE_API CRender2D::DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix4 &stTransform, const TPoint3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY, bool bClearDepthBuffer)
{
	IN_2D_GUARD

	if (!pBuffer || !pTexture)
		return E_INVALIDARG;

	_BatchFlush();

	TVector3 corner;

	corner.x = stExtents.x + fabs(stCenter.x);
	corner.y = stExtents.y + fabs(stCenter.y);
	corner.z = stExtents.z + fabs(stCenter.z);

	corner.x *= TVector3(stTransform._2D[0][0], stTransform._2D[1][0], stTransform._2D[2][0]).Length();
	corner.y *= TVector3(stTransform._2D[0][1], stTransform._2D[1][1], stTransform._2D[2][1]).Length();
	corner.z *= TVector3(stTransform._2D[0][2], stTransform._2D[1][2], stTransform._2D[2][2]).Length();
	
	float scale_z;

	if (_bCameraWasSet)
		scale_z = (_stCamScale.x + _stCamScale.y) / 2.f;
	else
		scale_z = 1.f;

	const float max_dist = corner.Length() * scale_z,
		z_plane = (float)_uiScreenHeight / tanf(fFovY / 2.f * ((float)M_PI / 180.f));
	
	float z_far;
	
	if (!bClip)
		z_far = z_plane + max_dist;
	else
		z_far = z_plane;

	// restrict znear in a way to [0.998..1.0] ndc z range ([-1..1]) occupy no more then half of eye space z range ([-znear..-zplane])
	const float z = 0.999f,
		z_min = (z_plane - z_plane * sqrtf(1.f - 4.f * z * (1.f - z))) / (2.f * z),
		z_near = fmax(z_min, z_plane - max_dist),
		factor = z_near / z_plane,

		plane_right = (float)_uiScreenWidth / 2.f,
		plane_left = -plane_right,
		plane_bottom = (float)_uiScreenHeight / 2.f,
		plane_top = -plane_bottom,

		near_right = (float)(plane_right * factor),
		near_left = -near_right,
		near_bottom = (float)(plane_bottom * factor),
		near_top = -near_bottom;

	const TMatrix4 new_proj(
		2.f * z_near / (near_right - near_left), 0.f, 0.f, 0.f,
		0.f, 2.f * z_near / (near_top - near_bottom), 0.f, 0.f,
		(near_right + near_left) / (near_right - near_left), (near_top + near_bottom) / (near_top - near_bottom), -(z_far + z_near) / (z_far - z_near), -1.f,
		0.f, 0.f, -2.f * z_far * z_near / (z_far - z_near), 0.f);
	
	TMatrix4 new_mview;

	if (_bCameraWasSet)
		new_mview = _stCamTransform * MatrixTranslate(TVector3(plane_left, plane_top, 0.f));
	else
		new_mview = MatrixTranslate(TVector3(plane_left, plane_top, 0.f));

	new_mview = MatrixScale(TVector3(1.f, 1.f, scale_z)) * stTransform * MatrixTranslate(TVector3(0.f, 0.f, -z_plane)) * new_mview;

	const TMatrix4 plane_transform = new_mview * new_proj;

	float planes[6][4];

	planes[0][0] = -plane_transform._2D[0][0] - plane_transform._2D[0][3];
	planes[0][1] = -plane_transform._2D[1][0] - plane_transform._2D[1][3];
	planes[0][2] = -plane_transform._2D[2][0] - plane_transform._2D[2][3];
	planes[0][3] = -plane_transform._2D[3][0] - plane_transform._2D[3][3];

	planes[1][0] = plane_transform._2D[0][0] - plane_transform._2D[0][3];
	planes[1][1] = plane_transform._2D[1][0] - plane_transform._2D[1][3];
	planes[1][2] = plane_transform._2D[2][0] - plane_transform._2D[2][3];
	planes[1][3] = plane_transform._2D[3][0] - plane_transform._2D[3][3];

	planes[2][0] = -plane_transform._2D[0][1] - plane_transform._2D[0][3];
	planes[2][1] = -plane_transform._2D[1][1] - plane_transform._2D[1][3];
	planes[2][2] = -plane_transform._2D[2][1] - plane_transform._2D[2][3];
	planes[2][3] = -plane_transform._2D[3][1] - plane_transform._2D[3][3];

	planes[3][0] = plane_transform._2D[0][1] - plane_transform._2D[0][3];
	planes[3][1] = plane_transform._2D[1][1] - plane_transform._2D[1][3];
	planes[3][2] = plane_transform._2D[2][1] - plane_transform._2D[2][3];
	planes[3][3] = plane_transform._2D[3][1] - plane_transform._2D[3][3];

	planes[4][0] = -plane_transform._2D[0][2] - plane_transform._2D[0][3];
	planes[4][1] = -plane_transform._2D[1][2] - plane_transform._2D[1][3];
	planes[4][2] = -plane_transform._2D[2][2] - plane_transform._2D[2][3];
	planes[4][3] = -plane_transform._2D[3][2] - plane_transform._2D[3][3];

	planes[5][0] = plane_transform._2D[0][2] - plane_transform._2D[0][3];
	planes[5][1] = plane_transform._2D[1][2] - plane_transform._2D[1][3];
	planes[5][2] = plane_transform._2D[2][2] - plane_transform._2D[2][3];
	planes[5][3] = plane_transform._2D[3][2] - plane_transform._2D[3][3];

	if (!(stCenter.Dot(TVector3(&planes[0][0])) + planes[0][3] - stExtents.Dot(TVector3(fabs(planes[0][0]), fabs(planes[0][1]), fabs(planes[0][2]))) > 0 ||
		stCenter.Dot(TVector3(&planes[1][0])) + planes[1][3] - stExtents.Dot(TVector3(fabs(planes[1][0]), fabs(planes[1][1]), fabs(planes[1][2]))) > 0 ||
		stCenter.Dot(TVector3(&planes[2][0])) + planes[2][3] - stExtents.Dot(TVector3(fabs(planes[2][0]), fabs(planes[2][1]), fabs(planes[2][2]))) > 0 ||
		stCenter.Dot(TVector3(&planes[3][0])) + planes[3][3] - stExtents.Dot(TVector3(fabs(planes[3][0]), fabs(planes[3][1]), fabs(planes[3][2]))) > 0 ||
		stCenter.Dot(TVector3(&planes[4][0])) + planes[4][3] - stExtents.Dot(TVector3(fabs(planes[4][0]), fabs(planes[4][1]), fabs(planes[4][2]))) > 0 ||
		stCenter.Dot(TVector3(&planes[5][0])) + planes[5][3] - stExtents.Dot(TVector3(fabs(planes[5][0]), fabs(planes[5][1]), fabs(planes[5][2]))) > 0))
	{
		TMatrix4 prev_proj, prev_mview;

		_pCoreRenderer->GetMatrix(prev_proj, MT_PROJECTION);
		_pCoreRenderer->SetMatrix(new_proj, MT_PROJECTION);

		_pCoreRenderer->GetMatrix(prev_mview);
		_pCoreRenderer->SetMatrix(new_mview);

		if (eFlags & EF_COLOR_MIX)
			_pCoreRenderer->SetColor(_stColormix);
		else  
			_pCoreRenderer->SetColor(ColorWhite());

		ICoreTexture *p_ctex;
		pTexture->GetCoreTexture(p_ctex);
		_pCoreRenderer->BindTexture(p_ctex);

		if (eFlags & EF_BLEND)
		{
			_stRasterStateDesc.bAlphaTestEnabled = false;
			_stRasterStateDesc.eCullMode = PCM_BACK;
			
			_stBlendStateDesc.bEnabled = true;
		}
		else
			if (eFlags & EF_NONE)
			{
				_stRasterStateDesc.bAlphaTestEnabled = false;
				_stBlendStateDesc.bEnabled = false;
			}
			else 
			{
				_stRasterStateDesc.bAlphaTestEnabled = true;
				_stBlendStateDesc.bEnabled = false;
			}

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);
		
		TDepthStencilDesc ds_desc;
		ds_desc.bDepthTestEnabled = true;
		_pCoreRenderer->SetDepthStencilState(ds_desc);

		if (bClearDepthBuffer)
			_pCoreRenderer->Clear(false, true, false);

		_pCoreRenderer->DrawBuffer(pBuffer);

		if (bClearDepthBuffer)
			_pCoreRenderer->Clear(false, true, false);
		
		ds_desc.bDepthTestEnabled = false;
		_pCoreRenderer->SetDepthStencilState(ds_desc);

		if (eFlags & EF_BLEND)
		{
			_stRasterStateDesc.eCullMode = PCM_NONE;
			_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		}

		_pCoreRenderer->SetMatrix(prev_proj, MT_PROJECTION);
		_pCoreRenderer->SetMatrix(prev_mview);
		
		++_uiObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if ((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && (!stDrawDesc.bVertices2D || !stDrawDesc.pData || stDrawDesc.uiNormalOffset != -1))
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	bool do_batch_update, flag = false;
	
	if (stDrawDesc.pIndexBuffer != NULL || eMode > CRDM_TRIANGLES)
	{
		flag = true;
		do_batch_update = false;
	}
	else
		do_batch_update = _2D_BATCH_NEED_UPDATE(eMode, p_tex, (eFlags & EF_COLOR_MIX) != 0, !(eFlags & EF_NONE) && !(eFlags & EF_BLEND), (eFlags & EF_BLEND) != 0);
	
	if ((_batchMode != BM_DISABLED || _bInLocalBatchMode) && !do_batch_update)
	{
		if (flag)
			_BatchFlush();
		else
			return S_OK;
	}

	if (stAABB.x != 0.f || stAABB.y != 0.f || stAABB.width != 0.f || stAABB.height != 0.f)
	{
		_pBuffer[0] = stAABB.x; _pBuffer[1] = stAABB.y;
		_pBuffer[2] = stAABB.x + stAABB.width; _pBuffer[3] = stAABB.y;
		_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = stAABB.y + stAABB.height;
		_pBuffer[6] = stAABB.x; _pBuffer[7] = _pBuffer[5];

		if (!BBoxInScreen(_pBuffer, false))
			return S_OK;
	}

	_2D_IF_BATCH_DO_SET_STATES
	{
		if (eFlags & EF_COLOR_MIX)
			_pCoreRenderer->SetColor(_stColormix);
		else  
			_pCoreRenderer->SetColor(ColorWhite());

		_pCoreRenderer->BindTexture(p_tex);

		if (eFlags & EF_BLEND)
		{
			_stRasterStateDesc.bAlphaTestEnabled = false;
			_stBlendStateDesc.bEnabled = true;
		}
		else
			if (eFlags & EF_NONE)
			{
				_stRasterStateDesc.bAlphaTestEnabled = false;
				_stBlendStateDesc.bEnabled = false;
			}
			else 
			{
				_stRasterStateDesc.bAlphaTestEnabled = true;
				_stBlendStateDesc.bEnabled = false;
			}

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			const float * const data = (float *)stDrawDesc.pData;
			
			const uint	v_stride = stDrawDesc.uiVertexStride == 0 ? 2 : stDrawDesc.uiVertexStride / sizeof(float),
						t_stride = stDrawDesc.uiTextureVertexStride == 0 ? 2 : stDrawDesc.uiTextureVertexStride / sizeof(float),
						c_stride = stDrawDesc.uiColorStride == 0 ? 4 : stDrawDesc.uiColorStride / sizeof(float),
						t_offset = stDrawDesc.uiTextureVertexOffset == -1 ? 0 : stDrawDesc.uiTextureVertexOffset / sizeof(float),
						c_offset = stDrawDesc.uiColorOffset == -1 ? 0 : stDrawDesc.uiColorOffset / sizeof(float);

			for (uint i = 0; i < uiCount; ++i)
			{
				const uint	v_idx = i * v_stride,
							t_idx = t_offset + i * t_stride,
							c_idx = c_offset + i * c_stride;

				TVertex2 v;

				v.x = data[v_idx]; v.y = data[v_idx + 1];

				if (stDrawDesc.uiTextureVertexOffset != -1)
				{
					v.u = data[t_idx];
					v.w = data[t_idx + 1];
				}

				if (stDrawDesc.uiColorOffset != -1)
				{
					v.r = data[c_idx];
					v.g = data[c_idx + 1];
					v.b = data[c_idx + 2];
					v.a = data[c_idx + 3];
				}
				else
				{
					v.r = _stColormix.r;
					v.g = _stColormix.g;
					v.b = _stColormix.b;
					v.a = _stColormix.a;
				}

				_vecBatchAccumulator.push_back(v);
			}
		}
	else
		_pCoreRenderer->Draw(stDrawDesc, eMode, uiCount);

	if (!_bInProfilerMode)
		++_uiObjsDrawnCount;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if (!pBuffer)
		return E_INVALIDARG;

	TDrawDataDesc desc;
	pBuffer->GetBufferDrawDataDesc(desc);

	if (!desc.bVertices2D || desc.uiNormalOffset != -1)
		return E_INVALIDARG;

	if (stAABB.x != 0.f || stAABB.y != 0.f || stAABB.width != 0.f || stAABB.height != 0.f)
	{
		_pBuffer[0] = stAABB.x; _pBuffer[1] = stAABB.y;
		_pBuffer[2] = stAABB.x + stAABB.width; _pBuffer[3] = stAABB.y;
		_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = stAABB.y + stAABB.height;
		_pBuffer[6] = stAABB.x; _pBuffer[7] = _pBuffer[5];

		if (!BBoxInScreen(_pBuffer, false))
			return S_OK;
	}

	_BatchFlush();

	if (eFlags & EF_COLOR_MIX)
		_pCoreRenderer->SetColor(_stColormix);
	else  
		_pCoreRenderer->SetColor(ColorWhite());

	if (pTexture)
	{
		ICoreTexture *p_ctex;
		pTexture->GetCoreTexture(p_ctex);
		_pCoreRenderer->BindTexture(p_ctex);
	}
	else
		_pCoreRenderer->BindTexture(NULL);

	if (eFlags & EF_BLEND)
	{
		_stRasterStateDesc.bAlphaTestEnabled = false;
		_stBlendStateDesc.bEnabled = true;
	}
	else
		if (eFlags & EF_NONE)
		{
			_stRasterStateDesc.bAlphaTestEnabled = false;
			_stBlendStateDesc.bEnabled = false;
		}
		else 
		{
			_stRasterStateDesc.bAlphaTestEnabled = true;
			_stBlendStateDesc.bEnabled = false;
		}

	_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnabled);
	_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnabled);

	_pCoreRenderer->DrawBuffer(pBuffer);

	++_uiObjsDrawnCount;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawTexture(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	uint width = 0, height = 0;

	if (pTexture)
		pTexture->GetDimensions(width, height);

	return DrawTexture(pTexture, stCoords, stDimensions, TRectF(0.f, 0.f, (float)width, (float)height), fAngle, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::DrawTextureSprite(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	if (pTexture == NULL)
		return E_INVALIDARG;

	uint width, height, frame_width, frame_height;

	pTexture->GetDimensions(width, height);
	pTexture->GetFrameSize(frame_width, frame_height);

	return DrawTexture(pTexture, stCoords, stDimensions,
		   TRectF(	(float)(uiFrameIndex * frame_width % width), 
					(float)(uiFrameIndex * frame_width / width * frame_height),
					(float)frame_width, (float)frame_height
					),
		fAngle, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::DrawTextureCropped(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, const TRectF &stTexCropRect, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	if (pTexture == NULL)
		return E_INVALIDARG;

	return DrawTexture(pTexture, stCoords, stDimensions, stTexCropRect, fAngle, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::SetRotationPoint(const TPoint2 &stCoords)
{
	IN_2D_GUARD

	_stRotationPoint = stCoords;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetScale(const TPoint2 &stScale)
{
	IN_2D_GUARD

	_stScale = stScale;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetColorMix(const TColor4 &stColor)
{
	IN_2D_GUARD

	_stColormix = stColor;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetBlendMode(E_BLENDING_EFFECT eMode)
{
	IN_2D_GUARD

	if (_ePrevBlendingMode == eMode)
		return S_OK;
	else
		_BatchFlush();

	switch (eMode)
	{
	case BE_NORMAL:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_ALPHA;
		break;
	case BE_ADD:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDstFactor = BF_ONE;
		break;
	case BE_MULT:
		_stBlendStateDesc.eSrcFactor = BF_ZERO;
		_stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case BE_BLACK:
		_stBlendStateDesc.eSrcFactor = BF_SRC_COLOR;
		_stBlendStateDesc.eDstFactor = BF_ONE_MINUS_SRC_COLOR;
		break;
	case BE_WHITE:
		_stBlendStateDesc.eSrcFactor = BF_ONE_MINUS_SRC_COLOR;
		_stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	case BE_MASK:
		_stBlendStateDesc.eSrcFactor = BF_ZERO;
		_stBlendStateDesc.eDstFactor = BF_SRC_COLOR;
		break;
	}

	_pCoreRenderer->SetBlendState(_stBlendStateDesc);

	_ePrevBlendingMode = eMode;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetVerticesOffsets(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4)
{
	IN_2D_GUARD

	_astVerticesOffset[0] = stCoords1;
	_astVerticesOffset[1] = stCoords2;
	_astVerticesOffset[2] = stCoords3;
	_astVerticesOffset[3] = stCoords4;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4)
{
	IN_2D_GUARD

	_astVerticesColors[0] = stColor2;
	_astVerticesColors[1] = stColor3;
	_astVerticesColors[2] = stColor1;
	_astVerticesColors[3] = stColor4;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetRotationPoint(TPoint2 &stCoords)
{
	IN_2D_GUARD

	stCoords = _stRotationPoint;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetScale(TPoint2 &stScale)
{
	IN_2D_GUARD

	stScale = _stScale;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetColorMix(TColor4 &stColor)
{
	IN_2D_GUARD
	
	stColor = _stColormix;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetBlendMode(E_BLENDING_EFFECT &eMode)
{
	IN_2D_GUARD
	
	eMode = _ePrevBlendingMode;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetVerticesOffsets(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4)
{
	IN_2D_GUARD

	stCoords2 = _astVerticesOffset[0];
	stCoords3 = _astVerticesOffset[1];
	stCoords1 = _astVerticesOffset[2];
	stCoords4 = _astVerticesOffset[3];

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4)
{
	IN_2D_GUARD

	stColor2 = _astVerticesColors[0];
	stColor3 = _astVerticesColors[1];
	stColor1 = _astVerticesColors[2];
	stColor4 = _astVerticesColors[3];

	return S_OK;
}