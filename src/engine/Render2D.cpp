/**
\author		Korotkov Andrey aka DRON
\date		23.09.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Render2D.h"
#include "Core.h"
#include "Render.h"

using namespace std;

#define _2D_BATCH_NEED_UPDATE(mode, tex, is_color)\
((_batchMode > BM_DISABLED || _bInLocalBatchMode) &&\
(_BatchSet(mode, tex, is_color) || ((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) || ((_batchMode == BM_ENABLED_UPDATE_EVERY_TICK || (_bInLocalBatchMode && _bLocalBatchUEP)) && !_batchNeedToRefreshBatches && !_batchBufferReadyToRender))))
#define _2D_IF_BATCH_NO_UPDATE_EXIT if (do_batch_update) if (!_batchNeedToRefreshBatches && !(_bInLocalBatchMode && !_bLocalBatchUEP)) _batchBufferReadyToRender = true;
#define _2D_IF_BATCH_DO_SET_STATES if ((_batchMode == BM_DISABLED && !_bInLocalBatchMode) || _batchAccumulator.empty())
#define _2D_BATCH_DUMMY_DRAW_CALL_EXIT if ((_batchMode != BM_DISABLED || _bInLocalBatchMode) && !do_batch_update) return S_OK;

#define IN_2D_GUARD if (!_bIn2D && !_bInProfilerMode) Begin2D();

CRender2D::CRender2D(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_iProfilerState(0), _iDoDrawBBoxes(0),
_bIn2D(false),_bInProfilerMode(false), _bInLocalBatchMode(false), _bLocalBatchUEP(false), _bLocalUEPWasTurnedOn(false),
_ui64DrawDelay(0), _iObjsDrawnCount(0),
_batchMode(BM_DISABLED),_batchBufferReadyToRender(false),_batchMaxSize(0),_batchMinSize(0),
_batchBufferCurCounter(0), _batchBuffersRepetedUseCounter(0), _batchBuffersNotModefiedPerFrameCounter(0),
_iResCorWidth(0), _iResCorHeight(0), _iResCorConstProp(false), _fLineWidth(1.f),
_ePrevBlendingMode(EBF_NORMAL),
_uiBufferSize(34)// never less than 34
{
	_pBuffer = new float[_uiBufferSize];
	_pCoreRenderer = Core()->pCoreRenderer();
	_pCoreRenderer->IsFeatureSupported(CRDF_GEOMETRY_BUFFER, _bUseGeometryBuffers);

	Console()->RegComValue("rnd2d_profiler", "Displays Render 2D subsystems profiler.", &_iProfilerState, 0, 2);
	Console()->RegComValue("rnd2d_draw_bboxes", "Displays bounding boxes of all 2D objects on screen.", &_iDoDrawBBoxes, 0, 1);
}

CRender2D::~CRender2D()
{
	Console()->UnRegCom("rnd2d_profiler");
	Console()->UnRegCom("rnd2d_draw_bboxes");

	delete[] _pBuffer;

	for (size_t i = 0; i < _pBatchBuffers.size(); ++i)
		_pBatchBuffers[i]->Free();

	_batchAccumulator.clear();
}

void CRender2D::BeginProfiler2D()
{
	if (_bIn2D) End2D();

	_bInProfilerMode = true;

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

	_bInProfilerMode = false;
}

void CRender2D::DrawProfiler()
{
	if (_iProfilerState > 0)
	{
		Core()->RenderProfilerTxt("======Render2D Profiler=====", TColor4());
		Core()->RenderProfilerTxt(("Objects on screen :" + IntToStr(_iObjsDrawnCount)).c_str(), TColor4());
		Core()->RenderProfilerTxt(("Batches per frame :" + UIntToStr(_batchsCount)).c_str(), _batchsCount > _sc_uiMaxBatchsPerFrame ? TColor4(255, 0, 0, 255) : TColor4());
		Core()->RenderProfilerTxt(("Render delay      :" + UInt64ToStr(_ui64DrawAverallDelay / 1000) + "." + UIntToStr(_ui64DrawAverallDelay % 1000) + " ms").c_str(), TColor4());

		if (_iProfilerState > 1)
		{
			uint  buffs_count = (uint)_pBatchBuffers.size();

			Core()->RenderProfilerTxt("--------Batch Render--------", TColor4());
			Core()->RenderProfilerTxt(("Buffers count  :" + UIntToStr(buffs_count)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Buffers in use :" + UIntToStr(_batchBufferCurCounter)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Effective calls:" + UIntToStr(_batchBuffersNotModefiedPerFrameCounter)).c_str(), !_batchNeedToRefreshBatches && _batchBuffersNotModefiedPerFrameCounter != _batchBufferCurCounter ? TColor4(255, 0, 0, 255) : TColor4());
			Core()->RenderProfilerTxt(("Max. batch size:" + UIntToStr(_batchMaxSize)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Min. batch size:" + UIntToStr(_batchMinSize == (numeric_limits<uint>::max)() ? 0 : _batchMinSize)).c_str(), TColor4());
			Core()->RenderProfilerTxt("----------------------------", TColor4());
		}
	}
}

void CRender2D::_SetDefaultStates()
{
	_stRotationPoint = TPoint2();
	_stScale = TPoint2(1.f, 1.f);
	_astVerticesOffset[0] = _astVerticesOffset[1] = _astVerticesOffset[2] = _astVerticesOffset[3] =  TPoint2();
	_stColormix = TColor4();

	_pCoreRenderer->SetColor(TColor4());
	
	_pCoreRenderer->SetLineWidth(1.f);
	_pCoreRenderer->SetPointSize(1.f);
	
	Core()->pRender()->Unbind(EOT_UNKNOWN);

	TDepthStencilDesc ds_desc;
	ds_desc.bDepthTestEnable = false;
	_pCoreRenderer->SetDepthStencilState(ds_desc);

	_stRasterStateDesc = TRasterizerStateDesc();
	_stRasterStateDesc.fAlphaTestRefValue = 0.75f;
	_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);

	_stBlendStateDesc = TBlendStateDesc();
	_pCoreRenderer->SetBlendState(_stBlendStateDesc);
}

__forceinline bool CRender2D::BBoxInScreen(const float *vertices, bool rotated) const
{
	if (_bInProfilerMode)
		return true;

	float vrtcs[10];

	if (_bCameraWasSet)
	{
		rotated = true;
		
		for (int i = 0; i < 4; ++i)
		{
			const float	&x = vertices[i*2], &y = vertices[i*2+1];

			vrtcs[i*2]		= _stCamTransform._2D[0][0] * x + _stCamTransform._2D[1][0] * y + _stCamTransform._2D[3][0];
			vrtcs[i*2+1]	= _stCamTransform._2D[0][1] * x + _stCamTransform._2D[1][1] * y + _stCamTransform._2D[3][1];		
		}

	}
	else
		memcpy(&vrtcs[0], &vertices[0], 8*sizeof(float));

	if (_iDoDrawBBoxes == 1)
	{
		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(MatrixInverse(_stCamTransform) * _stCamTransform);

		vrtcs[8] = vrtcs[0]; vrtcs[9] = vrtcs[1];
		
		_pCoreRenderer->BindTexture(NULL);
		_pCoreRenderer->SetColor(TColor4()); 
		_pCoreRenderer->SetLineWidth(1.f);
		
		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)vrtcs), CRDM_LINE_STRIP, 5);
		
		_pCoreRenderer->SetLineWidth(_fLineWidth);

		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(_stCamTransform);
	}

	if (!rotated && (((vrtcs[0] > _uiScreenWidth)  || (vrtcs[2] < 0.f) || (vrtcs[1] > _uiScreenHeight) || (vrtcs[5] < 0.f))))
		return false;

	bool left = false, right = false, up = false, down = false;

	if (vrtcs[0] > 0.f) left = true;
	if (vrtcs[0] < _uiScreenWidth) right = true;
	if (vrtcs[1] > 0.f) down = true;
	if (vrtcs[1] < _uiScreenHeight) up = true;

	if (vrtcs[2] > 0.f) left = true;
	if (vrtcs[2] < _uiScreenWidth) right = true;
	if (vrtcs[3] > 0.f) down = true;
	if (vrtcs[3] < _uiScreenHeight) up = true;

	if (vrtcs[4] > 0.f) left = true;
	if (vrtcs[4] < _uiScreenWidth) right = true;
	if (vrtcs[5] > 0.f) down = true;
	if (vrtcs[5] < _uiScreenHeight) up = true;

	if (vrtcs[6] > 0.f) left = true;
	if (vrtcs[6] < _uiScreenWidth) right = true;
	if (vrtcs[7] > 0.f) down = true;
	if (vrtcs[7] < _uiScreenHeight) up = true;

	return left && right && up && down;
}

DGLE_RESULT DGLE_API CRender2D::BatchRender(E_BATCH_MODE2D eMode)
{
	if (_bIn2D)
		return E_FAIL;

	_batchMode = eMode;

	switch(eMode)
	{
	case BM_AUTO:	
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

inline bool CRender2D::_BatchSet(E_CORE_RENDERER_DRAW_MODE eDrawMode, ICoreTexture *pTex, bool bColor)
{
	if (_eBatchDrawMode != eDrawMode || _pBatchCurTex != pTex || _bBatchColor != bColor)
	{
		_BatchFlush();
		_eBatchDrawMode = eDrawMode;
		_pBatchCurTex = pTex;
		_bBatchColor = bColor;
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
	if (!_batchBufferReadyToRender && _batchAccumulator.size() == 0)
		return;

	TDrawDataDesc desc;

	desc.bVertexCoord2 = true;
	desc.uiVertexStride = 8*sizeof(float);
	desc.uiTexCoordOffset = 2*sizeof(float);
	desc.uiTexCoordStride = 8*sizeof(float);
	desc.uiColorOffset = 4*sizeof(float);
	desc.uiColorStride = 8*sizeof(float);

	uint size = _batchAccumulator.size();

	if (size != 0)
		desc.pData = (uint8*)&_batchAccumulator[0];

	if ((_bInLocalBatchMode && _bLocalBatchUEP) || _batchMode == BM_ENABLED_UPDATE_EVERY_TICK )
	{
		++_batchBufferCurCounter;

		ICoreGeometryBuffer *p_buffer;
		
		if (_batchBufferCurCounter <= _pBatchBuffers.size())
		{
			p_buffer = _pBatchBuffers[_batchBufferCurCounter - 1];

			if (_batchNeedToRefreshBatches)
				p_buffer->Reallocate(desc, size, 0, _eBatchDrawMode);
			else
				++_batchBuffersRepetedUseCounter;
		}
		else
		{
			_pCoreRenderer->CreateGeometryBuffer(p_buffer, desc, size, 0, _eBatchDrawMode, _bUseGeometryBuffers ? CRBT_HARDWARE_STATIC : CRBT_SOFTWARE);
			_pBatchBuffers.push_back(p_buffer);
		}

		uint tmp;
		p_buffer->GetBufferDimensions(tmp, size, tmp, tmp);

		_pCoreRenderer->DrawBuffer(p_buffer);
	}
	else
		_pCoreRenderer->Draw(desc, _eBatchDrawMode, size);
	
	switch (_eBatchDrawMode)
	{
	case CRDM_POINTS:
		_iObjsDrawnCount += size;
		break;

	case CRDM_LINES:
		_iObjsDrawnCount += size / 2;
		break;

	case CRDM_TRIANGLES:
		_iObjsDrawnCount += size / 6;
		break;

	default:
		LOG("Unallowed draw mode " + UIntToStrX(_eBatchDrawMode) + " passed to 2D batching system! This is program logic issue, please report to engine developers.", LT_FATAL);
	}

	_batchMaxSize = max(_batchMaxSize, size);
	_batchMinSize = min(_batchMinSize, size);

	_batchAccumulator.clear();

	_batchBufferReadyToRender = false;

	++_batchsCount;
}

void CRender2D::_Set2DProjMatrix(uint width, uint height)
{
	_pCoreRenderer->SetMatrix(TMatrix(
		2.f/(float)width, 0.f, 0.f, 0.f,
		0.f, -2.f/(float)height, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f
		), MT_PROJECTION);
}

DGLE_RESULT DGLE_API CRender2D::Begin2D()
{
	if (_bIn2D)
		return E_FAIL;
	
	_bIn2D = true;
	_bCameraWasSet = false;
		
	_ui64DrawDelay = GetPerfTimer();

	_pCoreRenderer->PushStates();

	_SetDefaultStates();

	_bViewportChanged = false;

	_pCoreRenderer->GetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	if (_iResCorWidth + _iResCorHeight != 0)
	{
		if (_iResCorConstProp && fabs((float)_uiPrevViewPortW/(float)_uiPrevViewPortH - _fResCorCoef) > 0.001)
		{
			_uiCropW = (int)((float)_uiPrevViewPortH*_fResCorCoef);

			if (_uiPrevViewPortW > _uiCropW)
				_uiCropH = _uiPrevViewPortH;
			else
			{
				_uiCropH = (int)((float)_uiPrevViewPortW/_fResCorCoef);
				_uiCropW = _uiPrevViewPortW;
			}

			_uiCropX = (_uiPrevViewPortW - _uiCropW)/2;
			_uiCropY = (_uiPrevViewPortH - _uiCropH)/2;

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
	
	_pCoreRenderer->PopStates();

	_bIn2D = false;

	_ui64DrawDelay = GetPerfTimer() - _ui64DrawDelay;
	_ui64DrawAverallDelay += _ui64DrawDelay;

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
	_iObjsDrawnCount = 0;

	_batchMinSize = (numeric_limits<uint>::max)();
	_batchMaxSize = 0;

	_ui64DrawAverallDelay = 0;
}

void CRender2D::EndFrame()
{
	if (_bIn2D) End2D();

	_batchNeedToRefreshBatches = false;
}

DGLE_RESULT DGLE_API CRender2D::SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions)
{
	_iResCorWidth		= uiResX;
	_iResCorHeight		= uiResY;
	_iResCorConstProp	= bConstaintProportions;
	_fResCorCoef		= (float)_iResCorWidth/(float)_iResCorHeight;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetCamera(const TPoint2 &stCenter, float fAngle, const TPoint2 &stScale)
{
	IN_2D_GUARD

	_BatchFlush();

	if (stCenter.x == 0.f && stCenter.y == 0.f && fAngle == 0.f && stScale.x == 0.f && stScale.y == 0.f)
	{
		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(_stPrevCamTransform);

		_bCameraWasSet = false;
	}
	else
	{
		_pCoreRenderer->GetMatrix(_stPrevCamTransform);

		_stCamScale	= stScale;
		_stCamTransform = MatrixIdentity();
	
		if (fAngle != 0.f || stScale.x != 1.f || stScale.y != 1.f)
		{
			_stCamTransform = MatrixTranslate(TVec3((float)_uiScreenWidth/2.f, (float)_uiScreenHeight/2.f, 0.f)) * _stCamTransform;

			if (_stCamScale.x != 1.f || _stCamScale.y != 1.f)
				_stCamTransform = MatrixScale(TVec3(stScale.x, stScale.y, 1.f)) * _stCamTransform;

			if (fAngle != 0.f)
				_stCamTransform = MatrixRotate(fAngle, TVec3(0.f, 0.f, 1.f)) * _stCamTransform;

			_stCamTransform = MatrixTranslate(TVec3(-(float)_uiScreenWidth/2.f, -(float)_uiScreenHeight/2.f, 0.f)) * _stCamTransform;
		}

		_stCamTransform = MatrixTranslate(TVec3(stCenter.x - (float)_uiScreenWidth/2.f, stCenter.y - (float)_uiScreenHeight/2.f, 0.f)) * _stCamTransform;

		_pCoreRenderer->SetMatrix(_stCamTransform);

		_bCameraWasSet = true;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull)
{
	IN_2D_GUARD

	_pBuffer[0] = stBBox.x; _pBuffer[1] = stBBox.y;
	_pBuffer[2] = stBBox.x + stBBox.width; _pBuffer[2] = _pBuffer[1];
	_pBuffer[3] = _pBuffer[2]; _pBuffer[4] = stBBox.y + stBBox.height;
	_pBuffer[5] = _pBuffer[0]; _pBuffer[6] = _pBuffer[4];

	TMatrix transform;

	if (fAngle != 0.f)
	{
		TMatrix rot = MatrixIdentity();
		
		const float s = sinf(-fAngle * (float)M_PI/180.f), c = cosf(-fAngle * (float)M_PI/180.f);

		rot._2D[0][0] = +c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = +s;
		rot._2D[1][1] = +c;

		transform = MatrixTranslate(TVec3(-(stBBox.x + stBBox.width / 2.f), -(stBBox.y + stBBox.height / 2.f), 0.f)) * rot * MatrixTranslate(TVec3(stBBox.x + stBBox.width / 2.f, stBBox.y + stBBox.height / 2.f, 0.f));

		_pBuffer[7] = _pBuffer[0], _pBuffer[8] = _pBuffer[1];
		_pBuffer[0]	= transform._2D[0][0] * _pBuffer[7] + transform._2D[1][0] * _pBuffer[8] + transform._2D[3][0];
		_pBuffer[1]	= transform._2D[0][1] * _pBuffer[7] + transform._2D[1][1] * _pBuffer[8] + transform._2D[3][1];

		_pBuffer[7] = _pBuffer[2]; _pBuffer[8] = _pBuffer[3];
		_pBuffer[2]	= transform._2D[0][0] * _pBuffer[7] + transform._2D[1][0] * _pBuffer[8] + transform._2D[3][0];
		_pBuffer[3]	= transform._2D[0][1] * _pBuffer[7] + transform._2D[1][1] * _pBuffer[8] + transform._2D[3][1];

		_pBuffer[7] = _pBuffer[4]; _pBuffer[8] = _pBuffer[5];
		_pBuffer[4]	= transform._2D[0][0] * _pBuffer[7] + transform._2D[1][0] * _pBuffer[8] + transform._2D[3][0];
		_pBuffer[5]	= transform._2D[0][1] * _pBuffer[7] + transform._2D[1][1] * _pBuffer[8] + transform._2D[3][1];

		_pBuffer[7] = _pBuffer[6]; _pBuffer[8] = _pBuffer[7];
		_pBuffer[6]	= transform._2D[0][0] * _pBuffer[7] + transform._2D[1][0] * _pBuffer[8] + transform._2D[3][0];
		_pBuffer[7]	= transform._2D[0][1] * _pBuffer[7] + transform._2D[1][1] * _pBuffer[8] + transform._2D[3][1];
	}

	int prev = _iDoDrawBBoxes;
	_iDoDrawBBoxes = 0;

	bCull = !BBoxInScreen(_pBuffer, false);

	_iDoDrawBBoxes = prev;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::LineWidth(uint uiWidth)
{
	_fLineWidth = (float)uiWidth;
	_pCoreRenderer->SetLineWidth((float)uiWidth);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize)
{
	IN_2D_GUARD

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_POINTS, NULL, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float quad[] = {
		stCoords.x - uiSize/2.f, stCoords.y - uiSize/2.f,
		stCoords.x + uiSize/2.f, stCoords.y - uiSize/2.f,
		stCoords.x + uiSize/2.f, stCoords.y + uiSize/2.f,
		stCoords.x - uiSize/2.f, stCoords.y + uiSize/2.f
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	_pCoreRenderer->SetPointSize((float)uiSize);

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		if (stColor.a < 1.f)
		{
			_stBlendStateDesc.bEnable = true;
			_stRasterStateDesc.bAlphaTestEnable = false;
		}
		else
		{
			_stBlendStateDesc.bEnable = false;
			_stRasterStateDesc.bAlphaTestEnable = true;
		}

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
			_batchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f ,0.f , stColor.r, stColor.g, stColor.b, stColor.a));
	else
	{
		_pCoreRenderer->SetColor(stColor);

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)stCoords.xy), CRDM_POINTS, 1);
		
		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_LINES, NULL, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float quad[] = {
		min(stCoords1.x, stCoords2.x), min(stCoords1.y, stCoords2.y),
		max(stCoords1.x, stCoords2.x), min(stCoords1.y, stCoords2.y),
		max(stCoords1.x, stCoords2.x), max(stCoords1.y, stCoords2.y),
		min(stCoords1.x, stCoords2.x), max(stCoords1.y, stCoords2.y)
	};
	
	if (!BBoxInScreen(quad, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = stColor.a < 1.f;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);

		_pCoreRenderer->SetColor(stColor);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			if (eFlags & PF_VERTICES_COLOR)
			{
				_batchAccumulator.push_back(TVertex2(stCoords1.x, stCoords1.y, 0.f ,0.f , _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a));
				_batchAccumulator.push_back(TVertex2(stCoords2.x, stCoords2.y, 0.f ,0.f , _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a));
			}
			else
			{
				_batchAccumulator.push_back(TVertex2(stCoords1.x, stCoords1.y, 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(stCoords2.x, stCoords2.y, 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
			}
		}
	else
	{
		_pBuffer[0] = stCoords1.x; _pBuffer[1] = stCoords1.y;
		_pBuffer[2] = stCoords2.x; _pBuffer[3] = stCoords2.y;

		TDrawDataDesc desc((uint8*)_pBuffer);

		if (eFlags & PF_VERTICES_COLOR)
		{
			memcpy(&_pBuffer[4], _astVerticesColors, 8*sizeof(float));
			desc.uiColorOffset = 4*sizeof(float);
		}

		_pCoreRenderer->Draw(desc, CRDM_LINES, 2);

		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawRect(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, NULL, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	_pBuffer[0] = stRect.x; _pBuffer[1] = stRect.y;
	_pBuffer[2] = stRect.x + stRect.width; _pBuffer[3] = stRect.y;
	_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = stRect.y + stRect.height;
	_pBuffer[6] = stRect.x; _pBuffer[7] = _pBuffer[5];
	_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

	if (!BBoxInScreen(_pBuffer, false))
		return S_OK;

	_2D_IF_BATCH_DO_SET_STATES
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = stColor.a < 1.f;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);

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

			if (eFlags & PF_VERTICES_COLOR)
			{
				_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a));
				
				if (!(eFlags & PF_FILL))
				{
					_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a));
				}
			}
			else
			{
				_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				
				if (!(eFlags & PF_FILL))
				{
					_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
				}
			}
		}
	else
	{
		if (eFlags & PF_FILL)
		{
			_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

			_pBuffer[0] = _pBuffer[2];	_pBuffer[1] = _pBuffer[3];
			_pBuffer[2] = _pBuffer[4];	_pBuffer[3] = _pBuffer[5];
			_pBuffer[4] = _pBuffer[8];	_pBuffer[5] = _pBuffer[9];
		}

		TDrawDataDesc desc((uint8*)_pBuffer);

		if(eFlags & PF_VERTICES_COLOR)
		{
			memcpy(&_pBuffer[10], _astVerticesColors, 16*sizeof(float));
			desc.uiColorOffset = 8*sizeof(float);
		}

		_pCoreRenderer->Draw(desc, eFlags & PF_FILL ? CRDM_TRIANGLE_STRIP : CRDM_LINE_STRIP, eFlags & PF_FILL ? 4 : 5);

		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	return DrawEllipse(stCoords, TPoint2((float)uiRadius, (float)uiRadius), uiQuality, stColor, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::DrawEllipse(const TPoint2 &stCoords, const TPoint2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, NULL, true);
	
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

		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = stColor.a < 1.f;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);

		_pCoreRenderer->SetColor(stColor);
	}

	if (uiQuality > 360 / 2) uiQuality = 360 / 2;
	float k = 360.f / uiQuality;

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			_batchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			
			if (eFlags & PF_FILL)
				_batchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			
			_batchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x * cosf(k*(float)M_PI/180.f), stCoords.y + stRadius.y * sinf(k*(float)M_PI/180.f), 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));

			for (uint i = 2; i <= uiQuality; ++i)
			{
				if (eFlags & PF_FILL)
				{
					_batchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
					_batchAccumulator.push_back(_batchAccumulator[_batchAccumulator.size() - 2]);
				}
				else
					_batchAccumulator.push_back(_batchAccumulator[_batchAccumulator.size() - 1]);

				_batchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x * cosf(i*k*(float)M_PI/180.f), stCoords.y + stRadius.y * sinf(i*k*(float)M_PI/180.f), 0.f, 0.f, stColor.r, stColor.g, stColor.b, stColor.a));
			}
		}
	else
	{
		if (_uiBufferSize < uiQuality*2 + 4)
		{
			_uiBufferSize = uiQuality*2 + 4;
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
			_pBuffer[2 + i*2] = stCoords.x + stRadius.x * cosf(i*k*(float)M_PI/180.f);
			_pBuffer[3 + i*2] = stCoords.y + stRadius.y * sinf(i*k*(float)M_PI/180.f);
		}

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)&_pBuffer[eFlags & PF_FILL ? 0 : 2]), eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, eFlags & PF_FILL ? uiQuality + 2 : uiQuality + 1);

		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawPolygon(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	// Triangulation//

	struct TTriangle
	{
		uint16 index[3];
	};

	struct TVector2D : TPoint2
	{
		TVector2D() {}

		TVector2D(const TPoint2 &p) : TPoint2(p) {}

		TVector2D(const TVertex2 &v) : TPoint2(v.data) {}

		TVector2D(float x, float y) : TPoint2(x, y) {}

		TVector2D Diff(const TVector2D &p) const
		{
			return TPoint2(x - p.x, y - p.y);
		}

		TVector2D &OrthoCCW()
		{
			return *new(this) TVector2D(-y, x);
		}

		TVector2D OrthoCCW() const
		{
			return TVector2D(*this).OrthoCCW();
		}

		TVector2D &OrthoCW()
		{
			return *new(this) TVector2D(y, -x);
		}

		TVector2D OrthoCW() const
		{
			return TVector2D(*this).OrthoCW();
		}

	};

	struct TFContainer
	{
	private:
		static inline int32 GetNextActive(int32 x, int32 vertexCount, const bool *active)
		{
			for(;;)
			{
				if (++x == vertexCount) x = 0;
				if (active[x]) return x;
			}
		}

		static inline int32 GetPrevActive(int32 x, int32 vertexCount, const bool *active)
		{
			for(;;)
			{
				if (--x == -1) x = vertexCount - 1;
				if (active[x]) return x;
			}
		}

	public:
		static __forceinline int32 TriangulatePolygon(int32 vertexCount, const TVertex2 *vertex, bool ccw, TTriangle *triangle)
		{
			const float c_epsilon = 0.001f;

			bool *active = new bool[vertexCount];
			for (int32 a = 0; a < vertexCount; ++a)
				active[a] = true;

			int32 triangleCount = 0;
			int32 start = 0;
			int32 p1 = 0;
			int32 p2 = 1;
			int32 m1 = vertexCount - 1;
			int32 m2 = vertexCount - 2;

			bool last_positive = false;
			for(;;)
			{
				if (p2 == m2)
				{	// Only three vertices remain
					triangle->index[0] = (uint16)m1;
					triangle->index[1] = (uint16)p1;
					triangle->index[2] = (uint16)p2;
					++triangleCount;
					break;
				}

				const TVector2D c_vp1 = vertex[p1], c_vp2 = vertex[p2],
				c_vm1 = vertex[m1], c_vm2 = vertex[m2];
				bool  positive = false, negative = false;

				// Determine whether c_vp1, c_vp2 and c_vm1 form a valid triangle
				TVector2D n1 = c_vm1.Diff(c_vp2).Normalize();
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				if (n1.Dot(c_vp1.Diff(c_vp2)) > c_epsilon)
				{
					positive = true;
					TVector2D n2 = c_vp1.Diff(c_vm1).Normalize();
					TVector2D n3 = c_vp2.Diff(c_vp1).Normalize();
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

					for (int32 a = 0; a < vertexCount; ++a)
					{	// Look for other vertices inside the triangle
						if (active[a] && a != p1 && a != p2 && a != m1)
						{
							const TVector2D c_v = vertex[a];
							if( n1.Dot(c_v.Diff(c_vp2).Normalize()) > -c_epsilon &&
								n2.Dot(c_v.Diff(c_vm1).Normalize()) > -c_epsilon &&
								n3.Dot(c_v.Diff(c_vp1).Normalize()) > -c_epsilon)
							{
								positive = false;
								break;
							}
						}
					}
				}

				// Determine whether c_vm1, c_vm2 and c_vp1 form a valid triangle
				n1 = c_vm2.Diff(c_vp1).Normalize();
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				if (n1.Dot(c_vm1.Diff(c_vp1)) > c_epsilon)
				{
					negative = true;
					TVector2D n2 = (c_vm1.Diff(c_vm2)).Normalize();
					TVector2D n3 = (c_vp1.Diff(c_vm1)).Normalize();
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

					for (int32 a = 0; a < vertexCount; ++a)
					{	// Look for other vertices inside the triangle
						if (active[a] && a != m1 && a != m2 && a != p1)
						{
							const TVector2D v = vertex[a];
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

				// If both triangles valid, choose the one having the larger smallest angle
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
				{	// Output the triangle m1, p1, p2
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
				{	// Output the triangle m2, m1, p1
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
					// Exit if we've gone all the way aroud the polygon without finding a valid triangle
					if (start == -1) start = p2;
					else if (p2 == start) break;

					// Advance working set of vertices
					m2 = m1;
					m1 = p1;
					p1 = p2;
					p2 = GetNextActive(p2, vertexCount, active);
				}
			}

			delete[] active;
			return triangleCount;
		}
	};

	IN_2D_GUARD

	if (((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && !pstVertices) || uiVerticesCount < 3)
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINE_STRIP, p_tex, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; ++i)
	{
		max_x = max(pstVertices[i].x, max_x);
		max_y = max(pstVertices[i].y, max_y);
		min_x = min(pstVertices[i].x, min_x);
		min_y = min(pstVertices[i].y, min_y);
	}

	float quad[] = {
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

		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = true;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
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
		TVector2D e1 = (TVector2D(pstVertices[0]).Diff(pstVertices[uiVerticesCount - 1])).Normalize(), e2;
		for (uint vert_idx = 0; vert_idx < uiVerticesCount; e1 = e2, ++vert_idx)
		{
			e2 = TVector2D(TVector2D(pstVertices[(vert_idx + 1) % uiVerticesCount]).Diff(pstVertices[vert_idx])).Normalize();
			const float s = e1.Cross(e2), c = e1.Dot(e2);
			angle += atan2(s, c);
		}

		TTriangle *tris = new TTriangle[uiVerticesCount - 2];
		int32 tri_count;

#ifdef PLATFORM_WINDOWS
		__try
		{
#endif
		tri_count = TFContainer::TriangulatePolygon(uiVerticesCount, pstVertices, angle > 0, tris);
#ifdef PLATFORM_WINDOWS
		}
		__except(EXCEPTION_EXECUTE_HANDLER)\
		{
			delete[] tris;
			return E_INVALIDARG;
		}
#endif
		_2D_IF_BATCH_NO_UPDATE_EXIT
			else
				for (int32 tri_idx = 0; tri_idx < tri_count; ++tri_idx)
					for (uint8 v = 0; v < 3; ++v)
						_batchAccumulator.push_back(pstVertices[tris[tri_idx].index[v]]);
		else
		{
			if (_uiBufferSize < (uint)8*tri_count*3)
			{
				_uiBufferSize = 8*tri_count*3;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (int32 tri_idx = 0; tri_idx < tri_count; ++tri_idx)
				for (uint8 v = 0; v < 3; ++v)
					memcpy(&_pBuffer[tri_idx*8*3 + v*8], pstVertices[tris[tri_idx].index[v]].data, sizeof(TVertex2));

			TDrawDataDesc desc;

			desc.pData = (uint8*)_pBuffer;
			desc.bVertexCoord2 = true;
			desc.uiVertexStride = 8*sizeof(float);
			desc.uiTexCoordOffset = 2*sizeof(float);
			desc.uiTexCoordStride = 8*sizeof(float);
			desc.uiColorOffset = 4*sizeof(float);
			desc.uiColorStride = 8*sizeof(float);

			_pCoreRenderer->Draw(desc, CRDM_TRIANGLES, tri_count*3);

			++_iObjsDrawnCount;
		}

		delete[] tris;

	}
	else //Draw Line
	{
		_2D_IF_BATCH_NO_UPDATE_EXIT
			else
			{
				for (uint i = 0; i < uiVerticesCount; ++i)
					_batchAccumulator.push_back(pstVertices[i]);
				_batchAccumulator.push_back(pstVertices[0]);
			}
		else
		{
			if (_uiBufferSize < (uiVerticesCount + 1)*8)
			{
				_uiBufferSize = (uiVerticesCount + 1)*8;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			memcpy(_pBuffer, pstVertices, uiVerticesCount*sizeof(TVertex2));
			memcpy(&_pBuffer[uiVerticesCount*8], pstVertices, sizeof(TVertex2));

			TDrawDataDesc desc;

			desc.pData = (uint8*)_pBuffer;
			desc.bVertexCoord2 = true;
			desc.uiVertexStride = 8*sizeof(float);
			desc.uiTexCoordOffset = 2*sizeof(float);
			desc.uiTexCoordStride = 8*sizeof(float);
			desc.uiColorOffset = 4*sizeof(float);
			desc.uiColorStride = 8*sizeof(float);

			_pCoreRenderer->Draw(desc, CRDM_LINE_STRIP, uiVerticesCount + 1);

			++_iObjsDrawnCount;
	}

	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawTriangles(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if (((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && !pstVertices) || uiVerticesCount % 3 != 0)
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, p_tex, true);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; ++i)
	{
		max_x = max(pstVertices[i].x, max_x);
		max_y = max(pstVertices[i].y, max_y);
		min_x = min(pstVertices[i].x, min_x);
		min_y = min(pstVertices[i].y, min_y);
	}

	float quad[] = {
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

		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = true;

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	}

	
	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
			for (uint i = 0; i < uiVerticesCount/3; ++i)
				for (uint8 j = 0; j < 3; ++j)
					{
						if (!(eFlags & PF_FILL) && j > 0)
							_batchAccumulator.push_back(pstVertices[i*3 + j - 1]);
						_batchAccumulator.push_back(pstVertices[i*3 + j]);
					}
	else
	{
		TDrawDataDesc desc;
		
		if (!(eFlags & PF_FILL))
		{
			if (_uiBufferSize < uiVerticesCount*8*2)
			{
				_uiBufferSize = uiVerticesCount*8*2;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (uint i = 0; i < uiVerticesCount; ++i)
			{
				memcpy(&_pBuffer[i*8*2], &pstVertices[i], sizeof(TVertex2));

				if (i != 0 && (i + 1)% 3 == 0)
					memcpy(&_pBuffer[i*8*2 + 8], &pstVertices[i - 2], sizeof(TVertex2));
				else
					memcpy(&_pBuffer[i*8*2 + 8], &pstVertices[i + 1], sizeof(TVertex2));
			}

			desc.pData = (uint8*)_pBuffer;
		}
		else
			desc.pData = (uint8*)pstVertices;
				
		desc.bVertexCoord2 = true;
		desc.uiVertexStride = 8*sizeof(float);
		desc.uiTexCoordOffset = 2*sizeof(float);
		desc.uiTexCoordStride = 8*sizeof(float);
		desc.uiColorOffset = 4*sizeof(float);
		desc.uiColorStride = 8*sizeof(float);

		_pCoreRenderer->Draw(desc, eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, eFlags & PF_FILL ? uiVerticesCount : uiVerticesCount*2);

		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis, float fAngle, bool bClip, float fFovY, E_EFFECT2D_FLAGS eFlags)
{
	IN_2D_GUARD
	/*
	if(!pMesh || !pTexture)
		return E_INVALIDARG;

	_BatchFlush();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();

	TPoint3 center, extents, corner;
	pMesh->GetCenter(center);
	pMesh->GetExtents(extents);

	corner.x = extents.x + abs(center.x);
	corner.y = extents.y + abs(center.y);
	corner.z = extents.z + abs(center.z);
	corner.x *= abs(stDimensions.x);
	corner.y *= abs(stDimensions.y);
	corner.z *= abs(stDimensions.z);
	float max_dist = sqrt(corner.x * corner.x + corner.y * corner.y + corner.z * corner.z);

	GLdouble zplane = _uiScreenHeight / tan(fFovY / 2 * nv_to_rad);

	GLdouble zfar = zplane;

	if(!bClip)
		zfar += max_dist;

	// restrict znear in a way to [0.998..1.0] ndc z range ([-1..1]) occupy no more then half of eye space z range ([-znear..-zplane])
	const GLdouble z = 0.999;
	GLdouble zmin = (zplane - zplane * sqrt(1 - 4 * z * (1 - z))) / (2 * z);
	GLdouble znear = max(zmin, zplane - max_dist);
	GLdouble plane_right = _uiScreenWidth / 2., plane_left = -plane_right, plane_bottom = _uiScreenHeight / +2., plane_top = -plane_bottom;
	GLdouble factor = znear / zplane;
	GLdouble near_right = plane_right * factor, near_left = -near_right, near_bottom = plane_bottom * factor, near_top = -near_bottom;

	glFrustum(near_left, near_right, near_bottom, near_top, znear, zfar);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	float scale_z = 1.f;

	if(_bCameraWasSet)
	{
		glLoadIdentity();
		glTranslated(plane_left, plane_top, 0);
		glMultMatrixf(_stCamTransform.mat_array);
		scale_z = (_stCamScale.x+_stCamScale.y)/2.f;
	}
	else
		 glTranslated(plane_left, plane_top, 0);

	glTranslatef(stCoords.x, stCoords.y, -zplane);

	if(fAngle)
		glRotatef(fAngle, stAxis.x, stAxis.y, stAxis.z);

	glScalef(stDimensions.x, stDimensions.y, stDimensions.z*scale_z);

	CFrustum frustum;

	frustum.Setup();

	if(!frustum.Cull(vec3(&center.x), vec3(&extents.x)))
	{
		if(_batchMode > BM_DISABLED)
		{
			GL_SMAN->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			GL_SMAN->glDisableClientState(GL_COLOR_ARRAY);
			if(GLEW_ARB_vertex_buffer_object)
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}

		if(eFlags & EF_COLORMIX)
			GL_SMAN->glColor4f(_stColormix.r,_stColormix.g,_stColormix.b,_stColormix.a);
		else  
			GL_SMAN->glColor4f(1.f,1.f,1.f,1.f);

		if(eFlags & EF_BLEND)
		{
			GL_SMAN->glDisable(GL_ALPHA_TEST);
			GL_SMAN->glEnable(GL_BLEND);
			glEnable(GL_CULL_FACE);
		}
		else 
		{
			GL_SMAN->glDisable(GL_BLEND);
			GL_SMAN->glEnable(GL_ALPHA_TEST);
		}

		GLuint tex_glident;
		pTexture->GetGLTexIdent(tex_glident);
		GL_SMAN->glBindTexture(GL_TEXTURE_2D, tex_glident);

		glEnable(GL_DEPTH_TEST);
		pMesh->Draw();
		glDisable(GL_DEPTH_TEST);

		if(eFlags & EF_BLEND)
			glDisable(GL_CULL_FACE);

		if(_batchMode > BM_DISABLED)
			glEnableClientState(GL_VERTEX_ARRAY);

		++_iObjsDrawnCount;
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	*/
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if ((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) && (!stDrawDesc.bVertexCoord2 || !stDrawDesc.pData || stDrawDesc.uiNormalOffset != -1))
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
		do_batch_update = _2D_BATCH_NEED_UPDATE(eMode, p_tex, (eFlags & EF_COLORMIX) != 0);
	
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
		if (eFlags & EF_COLORMIX)
			_pCoreRenderer->SetColor(_stColormix);
		else  
			_pCoreRenderer->SetColor(TColor4());

		_pCoreRenderer->BindTexture(p_tex);

		if (eFlags & EF_BLEND)
		{
			_stRasterStateDesc.bAlphaTestEnable = false;
			_stBlendStateDesc.bEnable = true;
		}
		else
			if (eFlags & EF_NONE)
			{
				_stRasterStateDesc.bAlphaTestEnable = false;
				_stBlendStateDesc.bEnable = false;
			}
			else 
			{
				_stRasterStateDesc.bAlphaTestEnable = true;
				_stBlendStateDesc.bEnable = false;
			}

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);
	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
		{
			const float *data = (float *)stDrawDesc.pData;
			
			const uint	v_stride = stDrawDesc.uiVertexStride == 0 ? 2 : stDrawDesc.uiVertexStride/sizeof(float),
						t_stride = stDrawDesc.uiTexCoordStride == 0 ? 2 : stDrawDesc.uiTexCoordStride/sizeof(float),
						c_stride = stDrawDesc.uiColorStride == 0 ? 4 : stDrawDesc.uiColorStride/sizeof(float),
						t_offset = stDrawDesc.uiTexCoordOffset == -1 ? 0 : stDrawDesc.uiTexCoordOffset/sizeof(float),
						c_offset = stDrawDesc.uiColorOffset == -1 ? 0 : stDrawDesc.uiColorOffset/sizeof(float);

			for (uint i = 0; i < uiCount; ++i)
			{
				const uint	v_idx = i*v_stride,
							t_idx = t_offset + i*t_stride,
							c_idx = c_offset + i*c_stride;

				TVertex2 v;

				v.x = data[v_idx]; v.y = data[v_idx + 1];

				if (stDrawDesc.uiTexCoordOffset != -1)
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

				_batchAccumulator.push_back(v);
			}
		}
	else
		_pCoreRenderer->Draw(stDrawDesc, eMode, uiCount);

	++_iObjsDrawnCount;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags)
{
	IN_2D_GUARD

	if (!pBuffer)
		return E_INVALIDARG;

	TDrawDataDesc desc;
	pBuffer->GetBufferDrawDataDesc(desc);

	if (!desc.bVertexCoord2 || desc.uiNormalOffset != -1)
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

	if (eFlags & EF_COLORMIX)
		_pCoreRenderer->SetColor(_stColormix);
	else  
		_pCoreRenderer->SetColor(TColor4());

	if (pTexture)
		pTexture->Bind();
	else
		_pCoreRenderer->BindTexture(NULL);

	if (eFlags & EF_BLEND)
	{
		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = true;
	}
	else
		if (eFlags & EF_NONE)
		{
			_stRasterStateDesc.bAlphaTestEnable = false;
			_stBlendStateDesc.bEnable = false;
		}
		else 
		{
			_stRasterStateDesc.bAlphaTestEnable = true;
			_stBlendStateDesc.bEnable = false;
		}

	_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
	_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);

	_pCoreRenderer->DrawBuffer(pBuffer);

	++_iObjsDrawnCount;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix &stTransform, const TVector3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY)
{
	IN_2D_GUARD

	//ToDo

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::DrawTexture(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	uint width = 0, height = 0;

	if (pTexture)
		pTexture->GetDimensions(width, height);

	return DrawTexture(pTexture, stCoords, stDimensions, TRectF(0.f, 0.f, (float)width, (float)height), fAngle, eFlags);
}

DGLE_RESULT DGLE_API CRender2D::DrawSprite(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags)
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

DGLE_RESULT DGLE_API CRender2D::DrawTexCropped(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, const TRectF &stTexCropRect, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	if (pTexture == NULL)
		return E_INVALIDARG;

	return DrawTexture(pTexture, stCoords, stDimensions, stTexCropRect, fAngle, eFlags);
}

__forceinline DGLE_RESULT CRender2D::DrawTexture(ITexture *tex, const TPoint2 &coord, const TPoint2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags)
{
	IN_2D_GUARD

	ICoreTexture *p_tex = NULL;

	if (tex)
		tex->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_TRIANGLES, p_tex, (flags & EF_COLORMIX) || (flags & EF_VERTICES_COLOR));
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	_pBuffer[0] = coord.x; _pBuffer[1] = coord.y;
	_pBuffer[2] = coord.x + dimension.x; _pBuffer[3] = coord.y;
	_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = coord.y + dimension.y;
	_pBuffer[6] = coord.x; _pBuffer[7] = _pBuffer[5];

	if (flags & EF_VERTICES_OFFSET)
	{
		_pBuffer[0] += _astVerticesOffset[0].x;
		_pBuffer[1] += _astVerticesOffset[0].y;
		_pBuffer[2] += _astVerticesOffset[1].x;
		_pBuffer[3] += _astVerticesOffset[1].y;
		_pBuffer[4] += _astVerticesOffset[2].x;
		_pBuffer[5] += _astVerticesOffset[2].y;
		_pBuffer[6] += _astVerticesOffset[3].x;
		_pBuffer[7] += _astVerticesOffset[3].y;
	}

	TMatrix transform;
	
	if (angle != 0.f)
	{
		TMatrix translate, translate_back, rot = MatrixIdentity();
		
		const float s = sinf(-angle * (float)M_PI/180.f), c = cosf(-angle * (float)M_PI/180.f);

		rot._2D[0][0] = +c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = +s;
		rot._2D[1][1] = +c;

		if (flags & EF_ROTATEPT)
		{
			translate_back = MatrixTranslate(TVec3(coord.x + _stRotationPoint.x, coord.y + _stRotationPoint.y, 0.f));
			translate = MatrixTranslate(TVec3(-(coord.x + _stRotationPoint.x), -(coord.y + _stRotationPoint.y), 0.f));
		}
		else
		{
			translate_back = MatrixTranslate(TVec3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f));
			translate = MatrixTranslate(TVec3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
		}

		transform = translate * rot * translate_back;
	}
	else
		transform = MatrixIdentity();

	if (flags & EF_SCALE)
	{
		TMatrix scale = MatrixIdentity();
		scale._2D[0][0] = _stScale.x;
		scale._2D[1][1] = _stScale.y;

		TMatrix translate_back = MatrixTranslate(TVec3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f));
		TMatrix translate = MatrixTranslate(TVec3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
		transform = translate * scale * translate_back * transform;
	}

	if (angle != 0.f || flags & EF_SCALE)
	{
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

	if (!BBoxInScreen(_pBuffer, angle != 0.f))
		return S_OK;

	_pBuffer[12] = 0.f; //tile left
	_pBuffer[8]	 = 1.f; //tile right
	_pBuffer[9]	 = 0.f; //tile top
	_pBuffer[11] = 1.f; //tile bottom

	if (tex)
	{
		uint tex_width, tex_height;

		tex->GetDimensions(tex_width, tex_height);

		_pBuffer[12] = rect.x,
		_pBuffer[8]	 = rect.x + rect.width,
		_pBuffer[9]	 = rect.y,
		_pBuffer[11] = rect.y + rect.height;

		_pBuffer[12] /= tex_width;
		_pBuffer[8]	 /= tex_width;
		_pBuffer[9]	 /= tex_height;
		_pBuffer[11] /= tex_height;

		if (flags & EF_FLIPX)
		{
			_pBuffer[10] = _pBuffer[12];
			_pBuffer[12] = _pBuffer[8];
			_pBuffer[8]	 = _pBuffer[10];
		}

		if (flags & EF_FLIPY)
		{
			_pBuffer[10] = _pBuffer[9];
			_pBuffer[9]	 = _pBuffer[11];
			_pBuffer[11] = _pBuffer[10];
		}
	}

	_2D_IF_BATCH_DO_SET_STATES
	{
		if (flags & EF_COLORMIX)
			_pCoreRenderer->SetColor(_stColormix);
		else  
			_pCoreRenderer->SetColor(TColor4());

		_pCoreRenderer->BindTexture(p_tex);

		if (flags & EF_BLEND)
		{
			_stRasterStateDesc.bAlphaTestEnable = false;
			_stBlendStateDesc.bEnable = true;
		}
		else
			if (flags & EF_NONE)
			{
				_stRasterStateDesc.bAlphaTestEnable = false;
				_stBlendStateDesc.bEnable = false;
			}
			else 
			{
				_stRasterStateDesc.bAlphaTestEnable = true;
				_stBlendStateDesc.bEnable = false;
			}

		_pCoreRenderer->ToggleAlphaTestState(_stRasterStateDesc.bAlphaTestEnable);
		_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);

	}

	_2D_IF_BATCH_NO_UPDATE_EXIT
		else
			{
				if (flags & EF_VERTICES_COLOR)
				{
					_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], _pBuffer[8],	_pBuffer[9], _astVerticesColors[1].r,_astVerticesColors[1].g,_astVerticesColors[1].b,_astVerticesColors[1].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8],	_pBuffer[11],_astVerticesColors[2].r,_astVerticesColors[2].g,_astVerticesColors[2].b,_astVerticesColors[2].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12],_pBuffer[9], _astVerticesColors[0].r,_astVerticesColors[0].g,_astVerticesColors[0].b,_astVerticesColors[0].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8],	_pBuffer[11],_astVerticesColors[2].r,_astVerticesColors[2].g,_astVerticesColors[2].b,_astVerticesColors[2].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12],_pBuffer[9], _astVerticesColors[0].r,_astVerticesColors[0].g,_astVerticesColors[0].b,_astVerticesColors[0].a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], _pBuffer[12],_pBuffer[11], _astVerticesColors[3].r,_astVerticesColors[3].g,_astVerticesColors[3].b,_astVerticesColors[3].a));
				}
				else
				{
					TColor4 col;

					if (flags & EF_COLORMIX)
						col = _stColormix;

					_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], _pBuffer[8], _pBuffer[9], col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8], _pBuffer[11],col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12],_pBuffer[9], col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8], _pBuffer[11],col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12],_pBuffer[9], col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], _pBuffer[12],_pBuffer[11], col.r,col.g,col.b,col.a));
				}
			}
	else
	{
		_pBuffer[32] = _pBuffer[0]; _pBuffer[33] = _pBuffer[1];

		_pBuffer[0] = _pBuffer[2];	_pBuffer[1] = _pBuffer[3];
		_pBuffer[2] = _pBuffer[4];	_pBuffer[3] = _pBuffer[5];
		_pBuffer[4] = _pBuffer[32];	_pBuffer[5] = _pBuffer[33];

		_pBuffer[10] = _pBuffer[8];
		_pBuffer[13] = _pBuffer[9];
		_pBuffer[14] = _pBuffer[12];
		_pBuffer[15] = _pBuffer[11];

		TDrawDataDesc desc((uint8 *)_pBuffer, 8*sizeof(float));
		
		if (flags & EF_VERTICES_COLOR)
		{
			desc.uiColorOffset = 16*sizeof(float);
			memcpy(&_pBuffer[16], _astVerticesColors, sizeof(TColor4)*4);
		}

		_pCoreRenderer->Draw(desc, CRDM_TRIANGLE_STRIP, 4);
		
		++_iObjsDrawnCount;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetRotationPoint(const TPoint2 &stCoords)
{
	_stRotationPoint = stCoords;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetScale(const TPoint2 &stScale)
{
	_stScale = stScale;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetColorMix(const TColor4 &stColor)
{
	_stColormix = stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode)
{
	IN_2D_GUARD

	if (_ePrevBlendingMode == eMode)
		return S_OK;
	else
		_BatchFlush();

	switch(eMode)
	{
	case EBF_NORMAL:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDestFactor = BF_ONE_MINUS_SRC_ALPHA;
		break;
	case EBF_ADD:
		_stBlendStateDesc.eSrcFactor = BF_SRC_ALPHA;
		_stBlendStateDesc.eDestFactor = BF_ONE;
		break;
	case EBF_MULT:
		_stBlendStateDesc.eSrcFactor = BF_ZERO;
		_stBlendStateDesc.eDestFactor = BF_SRC_COLOR;
		break;
	case EBF_BLACK:
		_stBlendStateDesc.eSrcFactor = BF_SRC_COLOR;
		_stBlendStateDesc.eDestFactor = BF_ONE_MINUS_SRC_COLOR;
		break;
	case EBF_WHITE:
		_stBlendStateDesc.eSrcFactor = BF_ONE_MINUS_SRC_COLOR;
		_stBlendStateDesc.eDestFactor = BF_SRC_COLOR;
		break;
	case EBF_MASK:
		_stBlendStateDesc.eSrcFactor = BF_DST_ALPHA;
		_stBlendStateDesc.eDestFactor = BF_ZERO;
		break;
	}

	_pCoreRenderer->SetBlendState(_stBlendStateDesc);

	_ePrevBlendingMode = eMode;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetVerticesOffset(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4)
{
	_astVerticesOffset[0] = stCoords2;
	_astVerticesOffset[1] = stCoords3;
	_astVerticesOffset[2] = stCoords1;
	_astVerticesOffset[3] = stCoords4;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4)
{
	_astVerticesColors[0] = stColor2;
	_astVerticesColors[1] = stColor3;
	_astVerticesColors[2] = stColor1;
	_astVerticesColors[3] = stColor4;

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetRotationPoint(TPoint2 &stCoords)
{
	stCoords = _stRotationPoint;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetScale(TPoint2 &stScale)
{
	stScale = _stScale;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetColorMix(TColor4 &stColor)
{
	stColor = _stColormix;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetBlendMode(E_EFFECT2D_BLENDING_FLAGS &eMode)
{
	eMode = _ePrevBlendingMode;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetVerticesOffset(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4)
{
	stCoords2 = _astVerticesOffset[0];
	stCoords3 = _astVerticesOffset[1];
	stCoords1 = _astVerticesOffset[2];
	stCoords4 = _astVerticesOffset[3];

	return S_OK;
}

DGLE_RESULT DGLE_API CRender2D::GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4)
{
	stColor2 = _astVerticesColors[0];
	stColor3 = _astVerticesColors[1];
	stColor1 = _astVerticesColors[2];
	stColor4 = _astVerticesColors[3];

	return S_OK;
}