/**
\author		Korotkov Andrey aka DRON
\date		30.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Render2D.h"
//#include "Frustum.h"
#include "Core.h"
#include "Render.h"

using namespace std;

#define _2D_DO_BATCH_UPDATE(mode, tex, is_color)\
(_batchMode > BM_DISABLED && (_BatchSet(mode, tex, is_color) || (_batchNeedToRefreshBatches || (_batchMode == BM_ENABLED_UEP && !_batchNeedToRefreshBatches && !_batchBufferReadyToRender))))

CRender2D::CRender2D(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_iProfilerState(0), _iDoDrawBBoxes(0),
_bIn2D(false),_bInProfilerMode(false),
_ui64DrawDelay(0), _iObjsDrawnCount(0),
_batchMode(BM_DISABLED),_batchBufferReadyToRender(false),_batchMaxSize(0),_batchMinSize(0),
_batchBufferCurCounter(0), _batchBuffersRepetedUseCounter(0), _batchBuffersNotModefiedPerFrameCounter(0),
_iResCorWidth(0), _iResCorHeight(0), _iResCorConstProp(false),
_ePrevBlendingMode(EBF_NORMAL),
_uiBufferSize(34)// never less than 34
{
	_pBuffer = new float[_uiBufferSize];
	_pCoreRenderer = Core()->pCoreRenderer();
	Console()->RegComValue("r2d_profiler", "Displays render 2D subsystems profiler.", &_iProfilerState, 0, 2);
	Console()->RegComValue("r2d_drawbboxes", "Displays bounding boxes of 2D objects on screen.", &_iDoDrawBBoxes, 0, 1);
}

CRender2D::~CRender2D()
{
	delete[] _pBuffer;

	for (size_t i = 0; i < _pBatchBuffers.size(); i++)
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
		uint 
			objs_count  = _iObjsDrawnCount,
			vbos_count  = _pBatchBuffers.size(),
			vbos_in_use = _batchBufferCurCounter,
			effective_b = _batchBuffersNotModefiedPerFrameCounter,
			max_b_size	= _batchMaxSize,
			min_b_size	= _batchMinSize;
		uint64
			draw_ticks  = _ui64DrawDelay;

		Core()->RenderProfilerTxt("======Render2D Profiler=====", TColor4());
		Core()->RenderProfilerTxt(("Objects on screen :" + IntToStr(objs_count)).c_str(), TColor4());
		Core()->RenderProfilerTxt(("Render delay      :" + UInt64ToStr(draw_ticks/1000) + "." + IntToStr(draw_ticks%1000) + " ms.").c_str(), TColor4());

		if (_iProfilerState > 1)
		{
			Core()->RenderProfilerTxt("--------Batch Render--------", TColor4());
			Core()->RenderProfilerTxt(("Buffers count  :" + IntToStr(vbos_count)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Buffers in use :" + IntToStr(vbos_in_use)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Effective calls:" + IntToStr(effective_b)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Max. batch size:" + IntToStr(max_b_size)).c_str(), TColor4());
			Core()->RenderProfilerTxt(("Min. batch size:" + IntToStr(min_b_size)).c_str(), TColor4());
			Core()->RenderProfilerTxt("----------------------------", TColor4());
		}
	}
}

void CRender2D::_SetDefaultStates()
{
	_stRotationPoint = TPoint2();
	_stScale = TPoint2();
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

//	GL_SMAN->glDisable(GL_LIGHTING);
}

__forceinline bool CRender2D::BBoxInScreen(const float *vertices, bool rotated) const
{
	//Case when calling from profiler
	if (!_bIn2D) return true;

	static float vrtcs[10];

/*	if (_bCameraWasSet)
	{
		rotated = true;
		for(int i = 0; i<4; i++)
		{
			float	x = vertices[i*2], 
					y = vertices[i*2+1];

			vrtcs[i*2]		= _stCamTransform.a00 * x + _stCamTransform.a01 * y + _stCamTransform.a03;
			vrtcs[i*2+1]	= _stCamTransform.a10 * x + _stCamTransform.a11 * y + _stCamTransform.a13;		
		}
	}
	else*/
		memcpy(&vrtcs[0], &vertices[0], 8*sizeof(float));

	if (_iDoDrawBBoxes == 1)
	{
		/*
		if (_bCameraWasSet)
		{	
			glPushMatrix();
			mat4 inv_transform;
			invert(inv_transform,_stCamTransform);
			glMultMatrixf(inv_transform.mat_array);
		}*/

		vrtcs[8] = vrtcs[0]; vrtcs[9] = vrtcs[1];
		_pCoreRenderer->BindTexture(NULL);
		_pCoreRenderer->SetColor(TColor4()); 
		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)vrtcs), CRDM_LINE_STRIP, 5);

//		if(_bCameraWasSet)
//			glPopMatrix();
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

HRESULT CALLBACK CRender2D::BatchRender(E_BATCH_MODE2D eMode)
{
	if (eMode != _batchMode && _bIn2D)
		_BatchFlush();

	_batchMode = eMode;

	bool b_buffers_supported;
	_pCoreRenderer->IsFeatureSupported(CRDF_GEOMETRY_BUFFER, b_buffers_supported);

	switch(eMode)
	{
	case BM_AUTO:
		if (!b_buffers_supported)
		{	
			_batchMode = BM_DISABLED;
			return S_FALSE;
		}
		else
			_batchMode = BM_ENABLED_UEP;
	case BM_ENABLED_UEP:
	case BM_ENABLED_UER:
//		if(!GLEW_ARB_vertex_buffer_object && _batchMode==BM_ENABLED_UEP)
//			_batchMode	= BM_ENABLED_UER;
//		GL_SMAN->glEnableClientState(GL_VERTEX_ARRAY);
		_pBatchCurTex = NULL;
		(uint&)_eBatchDrawMode = -1;
		_bBatchColor = false;
		break;

	case BM_DISABLED:
//		GL_SMAN->glDisableClientState(GL_VERTEX_ARRAY);
//		GL_SMAN->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		GL_SMAN->glDisableClientState(GL_COLOR_ARRAY);
		break;
	}
	
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

//	for(uint i = 0; i<_batchVBOs.size(); i++)
//		_batchVBOs[i].uiSize = 0;
}

inline void CRender2D::_BatchFlush()
{/*
	if(!_batchVBOReadyToRender && _batchAccumulator.size()==0)
		return;

	void *data;

	if(GLEW_ARB_vertex_buffer_object)
	{
		_batchVBOCurCounter++;

		if(_batchVBOCurCounter <= _batchVBOs.size())
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, _batchVBOs[_batchVBOCurCounter-1].uiVboIdx);
			
			if(_batchNeedToRefreshBatches)
			{
				_batchVBOs[_batchVBOCurCounter-1].uiSize = _batchAccumulator.size();
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, _batchVBOs[_batchVBOCurCounter-1].uiSize*sizeof(TBatchVertex), NULL, GL_DYNAMIC_DRAW_ARB);
				glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, _batchVBOs[_batchVBOCurCounter-1].uiSize*sizeof(TBatchVertex), &_batchAccumulator[0]);
				
			}
			else
				_batchVBOsRepetedUseCounter++;
		}
		else
		{
			TBatchVBO temp_vbo;
			glGenBuffersARB(1, &temp_vbo.uiVboIdx);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, temp_vbo.uiVboIdx);
			temp_vbo.uiSize = _batchAccumulator.size();
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, temp_vbo.uiSize*sizeof(TBatchVertex), &_batchAccumulator[0], GL_DYNAMIC_DRAW_ARB);
			_batchVBOs.push_back(temp_vbo);
		}

		data = NULL;
	}
	else
		data = &_batchAccumulator[0];
	
	if(_batchCurTex!=0)
	{
		GL_SMAN->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(TBatchVertex), (GLvoid*)(((char*)data)+sizeof(float)*2));
	}
	else
		GL_SMAN->glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(_batchColor)
	{
		GL_SMAN->glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT, sizeof(TBatchVertex), (GLvoid*)(((char*)data)+sizeof(float)*4));
	}
	else
		GL_SMAN->glDisableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(TBatchVertex), (GLvoid*)(data));

	uint size;

	if(!_batchVBOReadyToRender)
		size = _batchAccumulator.size();
	else
		size = _batchVBOs[_batchVBOCurCounter-1].uiSize;

	glDrawArrays(_batchPrimType,0,size);

	switch(_batchPrimType)
	{
	case GL_LINES:
		_iObjsDrawnCount+=size/2;
		break;
	case GL_QUADS:
		_iObjsDrawnCount+=size/4;
		break;
	case GL_TRIANGLES:
		_iObjsDrawnCount+=size/3;
		break;
	default:
		_iObjsDrawnCount+=size;
		break;
	}
	
	_batchMaxSize = max(_batchMaxSize,size);
	_batchMinSize = min(_batchMinSize,size);

	_batchAccumulator.clear();
	_batchVBOReadyToRender = false;*/
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

HRESULT CALLBACK CRender2D::Begin2D()
{
	if (_bIn2D) 
		return S_FALSE;
	
	_bIn2D = true;
	_bCameraWasSet = false;
	
	_batchBufferCurCounter = 0;
	_batchBuffersRepetedUseCounter = 0;
	_iObjsDrawnCount = 0;
	
	if (_batchMode == BM_ENABLED_UER)
		_batchNeedToRefreshBatches = true;

	_batchMinSize = (numeric_limits<uint>::max)();
	_batchMaxSize = 0;

	_ui64DrawDelay = GetPerfTimer();

	_pCoreRenderer->PushStates();

	_SetDefaultStates();

	_bViewportChanged = false;

	_pCoreRenderer->GetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	if (_iResCorWidth + _iResCorHeight != 0)
	{
		if (_iResCorConstProp && fabs((float)_uiPrevViewPortW/(float)_uiPrevViewPortH - _fResCorKoef) > 0.001)
		{
			_uiCropW = (int)((float)_uiPrevViewPortH*_fResCorKoef);

			if (_uiPrevViewPortW > _uiCropW)
				_uiCropH = _uiPrevViewPortH;
			else
			{
				_uiCropH = (int)((float)_uiPrevViewPortW/_fResCorKoef);
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

HRESULT CALLBACK CRender2D::End2D()
{
	if (!_bIn2D)
		return S_FALSE;

	_BatchFlush();
	(uint&)_eBatchDrawMode = -1;

//	if (_bCameraWasSet)
//		glPopMatrix();

	_batchBuffersNotModefiedPerFrameCounter = _batchBuffersRepetedUseCounter;

	if (_bViewportChanged)
		_pCoreRenderer->SetViewport(_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH);

	_pCoreRenderer->SetMatrix(_stPrevProjMat, MT_PROJECTION);
	_pCoreRenderer->SetMatrix(_stPrevModelViewMat);
	
	_pCoreRenderer->PopStates();

	_bIn2D = false;
	_batchNeedToRefreshBatches = false;
	_ui64DrawDelay = GetPerfTimer() - _ui64DrawDelay;

	return S_OK;
}

HRESULT CALLBACK CRender2D::SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions)
{
	_iResCorWidth		= uiResX;
	_iResCorHeight		= uiResY;
	_iResCorConstProp	= bConstaintProportions;
	_fResCorKoef		= (float)_iResCorWidth/(float)_iResCorHeight;

	return S_OK;
}

HRESULT CALLBACK CRender2D::SetCamera(const TPoint2 &stCenter, float fAngle, const TPoint2 &stScale)
{
	/*
	_BatchFlush();

	if(_bCameraWasSet)
		glPopMatrix();

	if(stCenter.x == 0.f && stCenter.y == 0.f && fAngle == 0.f && stScale.x == 1.f && stScale.y == 1.f)
		_bCameraWasSet = false;
	else
	{
		_stCamScale		= stScale;
	
		glPushMatrix();

		if(fAngle!=0.f || stScale.x!=1.f || stScale.y!=1.f)
		{
			glTranslatef((float)_uiScreenWidth/2.f, (float)_uiScreenHeight/2.f,0.f);
			if(stScale.x!=1.f || stScale.y!=1.f)
				glScalef(stScale.x, stScale.y, 1.f);
			if(fAngle!=0.f)
				glRotatef(fAngle, 0.f, 0.f, 1.f);
			glTranslatef(-(float)_uiScreenWidth/2.f, -(float)_uiScreenHeight/2.f, 0.f);
		}

		glTranslatef(_uiScreenWidth/2.f - stCenter.x, _uiScreenHeight/2.f - stCenter.y, 0.f);

		glGetFloatv(GL_MODELVIEW_MATRIX, _stCamTransform.mat_array);

		_bCameraWasSet = true;
	}
	*/
	return S_OK;
}

HRESULT CALLBACK CRender2D::LineWidth(uint uiWidth)
{
	_pCoreRenderer->SetLineWidth((float)uiWidth);
	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize)
{
	bool do_batch_update = _2D_DO_BATCH_UPDATE(CRDM_POINTS, NULL, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	float quad[] = {
		stCoords.x - uiSize/2.f, stCoords.y - uiSize/2.f,
		stCoords.x + uiSize/2.f, stCoords.y - uiSize/2.f,
		stCoords.x + uiSize/2.f, stCoords.y + uiSize/2.f,
		stCoords.x - uiSize/2.f, stCoords.y + uiSize/2.f
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	_pCoreRenderer->SetPointSize((float)uiSize);

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(NULL);

		bool smooth = uiSize > 1 || stColor.a < 1.f;

		if (smooth != _stRasterStateDesc.bSmoothPointsAndLines)
		{
			_stRasterStateDesc.bAlphaTestEnable = false;
			_stRasterStateDesc.bSmoothPointsAndLines = smooth;
			_stBlendStateDesc.bEnable = _stRasterStateDesc.bSmoothPointsAndLines;
			_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		}
		else
		{
			_pCoreRenderer->ToggleAlphaTestState(false);
			_pCoreRenderer->ToggleBlendState(_stBlendStateDesc.bEnable);
		}
	}

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
			_batchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f ,0.f , stColor.r, stColor.g, stColor.b, stColor.a));
	else
	{
		_pCoreRenderer->SetColor(stColor);

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)stCoords.xy), CRDM_POINTS, 1);
		
		_iObjsDrawnCount++;
	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	bool do_batch_update = _2D_DO_BATCH_UPDATE(CRDM_LINES, NULL, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	float quad[] = {
		min(stCoords1.x, stCoords2.x), min(stCoords1.y, stCoords2.y),
		max(stCoords1.x, stCoords2.x), min(stCoords1.y, stCoords2.y),
		max(stCoords1.x, stCoords2.x), max(stCoords1.y, stCoords2.y),
		min(stCoords1.x, stCoords2.x), max(stCoords1.y, stCoords2.y)
	};
	
	if (!BBoxInScreen(quad, false))
		return S_OK;

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bSmoothPointsAndLines = (eFlags & PF_SMOOTH) != 0;
		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = stColor.a < 1.f || eFlags & PF_SMOOTH;

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);

		_pCoreRenderer->SetColor(stColor);
	}

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
		{
			if (eFlags & PF_VCA)
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

		if (eFlags & PF_VCA)
		{
			memcpy(&_pBuffer[4], _astVerticesColors, 8*sizeof(float));
			desc.uiColorOffset = 4*sizeof(float);
		}

		_pCoreRenderer->Draw(desc, CRDM_LINES, 2);

		_iObjsDrawnCount++;
	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawRect(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	bool do_batch_update = _2D_DO_BATCH_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, NULL, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	_pBuffer[0] = stRect.x; _pBuffer[1] = stRect.y;
	_pBuffer[2] = stRect.x + stRect.width; _pBuffer[3] = stRect.y;
	_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = stRect.y + stRect.height;
	_pBuffer[6] = stRect.x; _pBuffer[7] = _pBuffer[5];
	_pBuffer[8] = _pBuffer[0]; _pBuffer[9] = _pBuffer[1];

	if (!BBoxInScreen(_pBuffer, false))
		return S_OK;

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bSmoothPointsAndLines = false;
		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = stColor.a < 1.f;

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);

		_pCoreRenderer->SetColor(stColor);
	}

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
		{
			if (eFlags & PF_VCA)
			{
				_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f ,0.f , _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f ,0.f , _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f ,0.f , _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f ,0.f , _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a));
				if (!(eFlags & PF_FILL))
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f ,0.f , _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a));
			}
			else
			{
				_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[4], _pBuffer[5], 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				_batchAccumulator.push_back(TVertex2(_pBuffer[6], _pBuffer[7], 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
				if (!(eFlags & PF_FILL))
					_batchAccumulator.push_back(TVertex2(_pBuffer[0], _pBuffer[1], 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
			}
		}
	else
	{
		TDrawDataDesc desc((uint8*)_pBuffer);

		if(eFlags & PF_VCA)
		{
			memcpy(&_pBuffer[10], _astVerticesColors, 16*sizeof(float));
			desc.uiColorOffset = 8*sizeof(float);
		}

		_pCoreRenderer->Draw(desc, eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, eFlags & PF_FILL ? 4 : 5);

		_iObjsDrawnCount++;
	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	return DrawEllipse(stCoords, TPoint2((float)uiRadius, (float)uiRadius), uiQuality, stColor, eFlags);
}

HRESULT CALLBACK CRender2D::DrawEllipse(const TPoint2 &stCoords, const TPoint2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags)
{
	bool do_batch_update = _2D_DO_BATCH_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, NULL, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	float quad[] = {
		stCoords.x - stRadius.x, stCoords.y - stRadius.y,
		stCoords.x + stRadius.x, stCoords.y - stRadius.y,
		stCoords.x + stRadius.x, stCoords.y + stRadius.y,
		stCoords.x - stRadius.x, stCoords.y + stRadius.y
	};

	if (!BBoxInScreen(quad, false))
		return S_OK;

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(NULL);

		_stRasterStateDesc.bSmoothPointsAndLines = eFlags & PF_SMOOTH && !(eFlags & PF_FILL);
		_stRasterStateDesc.bAlphaTestEnable = false;

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);//медленно, подумать как исправить
		_pCoreRenderer->ToggleBlendState(stColor.a < 1.f || eFlags & PF_SMOOTH);

		_pCoreRenderer->SetColor(stColor);
	}

	if (uiQuality > 360/2) uiQuality = 360/2;
	float k = 360.f / uiQuality;

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
		{
			if (eFlags & PF_FILL)
				_batchAccumulator.push_back(TVertex2(stCoords.x, stCoords.y, 0.f, 0.f ,stColor.r, stColor.g, stColor.b, stColor.a));

			for (uint i = 0; i <= uiQuality; i++)
				_batchAccumulator.push_back(TVertex2(stCoords.x + stRadius.x * cosf(i*k*(float)M_PI/180.f), stCoords.y + stRadius.y * sinf(i*k*(float)M_PI/180.f), 0.f ,0.f ,stColor.r, stColor.g, stColor.b, stColor.a));
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

		for (uint i = 0; i <= uiQuality; i++)
		{
			_pBuffer[2 + i*2] = stCoords.x + stRadius.x * cosf(i*k*(float)M_PI/180.f);
			_pBuffer[3 + i*2] = stCoords.y + stRadius.y * sinf(i*k*(float)M_PI/180.f);
		}

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)&_pBuffer[eFlags & PF_FILL ? 0 : 2]), eFlags & PF_FILL ? CRDM_TRIANGLE_FAN : CRDM_LINE_STRIP, eFlags & PF_FILL ? uiQuality + 2 : uiQuality + 1);

		_iObjsDrawnCount++;
	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawPolygon(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	// Triangulation//

	struct TTriangle
	{
		uint16 index[3];
	};

	struct TVector2D
	{
		float x, y;

		TVector2D() {}

		TVector2D(float r, float s)
		{
			x = r;
			y = s;
		}

		// Dot product
		float operator *(const TVector2D &v) const
		{
			return x * v.x + y * v.y;
		}

		// Cross product
		float operator %(const TVector2D &v) const
		{
			return x * v.y - v.x * y;
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

		TVector2D &Normalize()
		{
			const float len = sqrt(x * x + y * y);
			x /= len, y /= len;
			return *this;
		}
	};

	struct TPoint2D: TVector2D
	{
		TPoint2D() {}
		TPoint2D(float r, float s): TVector2D(r, s) {}
		TPoint2D(const TVertex2 &v): TVector2D(v.x, v.y) {}

		// Difference between two points is a vector
		TVector2D operator -(const TPoint2D &p) const
		{
			return TVector2D(x - p.x, y - p.y);
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
			const float c_epsilon = 0.001F;

			bool *active = new bool[vertexCount];
			for (int32 a = 0; a < vertexCount; a++)
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
					triangleCount++;
					break;
				}

				const TPoint2D c_vp1 = vertex[p1];
				const TPoint2D c_vp2 = vertex[p2];
				const TPoint2D c_vm1 = vertex[m1];
				const TPoint2D c_vm2 = vertex[m2];
				bool  positive = false;
				bool  negative = false;

				// Determine whether c_vp1, c_vp2 and c_vm1 form a valid triangle
				TVector2D n1 = (c_vm1 - c_vp2).Normalize();
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				if (n1 * (c_vp1 - c_vp2) > c_epsilon)
				{
					positive = true;
					TVector2D n2 = (c_vp1 - c_vm1).Normalize();
					TVector2D n3 = (c_vp2 - c_vp1).Normalize();
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

					for (int32 a = 0; a < vertexCount; a++)
					{	// Look for other vertices inside the triangle
						if (active[a] && a != p1 && a != p2 && a != m1)
						{
							const TPoint2D c_v = vertex[a];
							if( n1 * (c_v - c_vp2).Normalize() > -c_epsilon &&
								n2 * (c_v - c_vm1).Normalize() > -c_epsilon &&
								n3 * (c_v - c_vp1).Normalize() > -c_epsilon)
							{
								positive = false;
								break;
							}
						}
					}
				}

				// Determine whether c_vm1, c_vm2 and c_vp1 form a valid triangle
				n1 = (c_vm2 - c_vp1).Normalize();
				if (ccw)
					n1.OrthoCCW();
				else
					n1.OrthoCW();
				if (n1 * (c_vm1 - c_vp1) > c_epsilon)
				{
					negative = true;
					TVector2D n2 = (c_vm1 - c_vm2).Normalize();
					TVector2D n3 = (c_vp1 - c_vm1).Normalize();
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

					for (int32 a = 0; a < vertexCount; a++)
					{	// Look for other vertices inside the triangle
						if (active[a] && a != m1 && a != m2 && a != p1)
						{
							const TPoint2D v = vertex[a];
							if (n1 * (v - c_vp1).Normalize() > -c_epsilon &&
								n2 * (v - c_vm2).Normalize() > -c_epsilon &&
								n3 * (v - c_vm1).Normalize() > -c_epsilon)
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
					float pd = (c_vp2 - c_vm1).Normalize() * (c_vm2 - c_vm1).Normalize();
					float md = (c_vm2 - c_vp1).Normalize() * (c_vp2 - c_vp1).Normalize();

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
					triangleCount++;
					triangle++;

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
					triangleCount++;
					triangle++;

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

	if (uiVerticesCount<3)
		return E_FAIL;

	ICoreTexture *p_tex = NULL;;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_DO_BATCH_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINE_STRIP, p_tex, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; i++)
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

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(p_tex);

		_stRasterStateDesc.bSmoothPointsAndLines = eFlags & PF_SMOOTH && !(eFlags & PF_FILL);
		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = true;

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	}

	if (eFlags & PF_FILL)
	{
		if (do_batch_update && !_batchNeedToRefreshBatches)
		{		
			_batchBufferReadyToRender = true;
			return S_OK;
		}

		float angle = 0;
		TVector2D e1 = (TPoint2D(pstVertices[0]) - pstVertices[uiVerticesCount - 1]).Normalize(), e2;
		for (uint vert_idx = 0; vert_idx < uiVerticesCount; e1 = e2, vert_idx++)
		{
			e2 = (TPoint2D(pstVertices[(vert_idx + 1) % uiVerticesCount]) - pstVertices[vert_idx]).Normalize();
			const float s = e1 % e2, c = e1 * e2;
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
		if (do_batch_update)
		{
			if (_batchNeedToRefreshBatches)
				for (int32 tri_idx = 0; tri_idx < tri_count; tri_idx++)
					for (uint8 v = 0; v < 3; v++)
						_batchAccumulator.push_back(pstVertices[tris[tri_idx].index[v]]);
		}
		else
		{
			if (_uiBufferSize < (uint)8*tri_count*3)
			{
				_uiBufferSize = 8*tri_count*3;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (int32 tri_idx = 0; tri_idx < tri_count; tri_idx++)
				for (uint8 v = 0; v < 3; v++)
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

			_iObjsDrawnCount++;
		}

		delete[] tris;

	}
	else//Draw Line
	{
		if (do_batch_update)
		{
			if (_batchNeedToRefreshBatches)
			{
				for (uint i = 0; i < uiVerticesCount; i++)
					_batchAccumulator.push_back(pstVertices[i]);
				_batchAccumulator.push_back(pstVertices[0]);
			}
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

			_iObjsDrawnCount++;
	}

	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawTriangles(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags)
{
	if (uiVerticesCount%3 != 0)
		return E_INVALIDARG;

	ICoreTexture *p_tex = NULL;;

	if (pTexture)
		pTexture->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_DO_BATCH_UPDATE(eFlags & PF_FILL ? CRDM_TRIANGLES : CRDM_LINES, p_tex, true);
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return S_OK;

	float	max_x = -numeric_limits<float>::infinity(),
			max_y = -numeric_limits<float>::infinity(),
			min_x =  numeric_limits<float>::infinity(),
			min_y =  numeric_limits<float>::infinity();

	for (uint i = 0; i < uiVerticesCount; i++)
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

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
	{
		_pCoreRenderer->BindTexture(p_tex);

		_stRasterStateDesc.bSmoothPointsAndLines = eFlags & PF_SMOOTH && !(eFlags & PF_FILL);
		_stRasterStateDesc.bAlphaTestEnable = false;
		_stBlendStateDesc.bEnable = true;

		_pCoreRenderer->SetRasterizerState(_stRasterStateDesc);
		_pCoreRenderer->SetBlendState(_stBlendStateDesc);
	}

	
	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
			for (uint i = 0; i < uiVerticesCount/3; i++)
				for (uint8 j = 0; j < 3; j++)
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
			if (_uiBufferSize < uiVerticesCount*6*8)
			{
				_uiBufferSize = uiVerticesCount*6*8;
				delete[] _pBuffer;
				_pBuffer = new float[_uiBufferSize];
			}

			for (uint i = 0; i < uiVerticesCount/3; i++)
				for (uint8 j = 0; j < 6; j++)
					memcpy(&_pBuffer[i*6*8 + j], &pstVertices[i*3 + j], sizeof(TVertex2));
	
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

		_iObjsDrawnCount++;
	}

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TPoint3 &stDimensions, const TPoint3 &stAxis, float fAngle, bool bClip, float fFovY, E_EFFECT2D_FLAGS eFlags)
{
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

		_iObjsDrawnCount++;
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	*/
	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawSpriteS(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	uint width = 0, height = 0;

	if (pTexture)
		pTexture->GetDimensions(width, height);

	DrawTexture(pTexture, stCoords, stDimensions, TRectF(0.f, 0.f, (float)width, (float)height), fAngle, eFlags);

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawSpriteA(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	if (pTexture == NULL)
		return S_FALSE;

	uint width, height, frame_width, frame_height;

	pTexture->GetDimensions(width, height);
	pTexture->GetFrameSize(frame_width, frame_height);

	DrawTexture(pTexture, stCoords, stDimensions,
		   TRectF(	(float)(uiFrameIndex * frame_width % width), 
					(float)(uiFrameIndex * frame_width / width * frame_height),
					(float)frame_width, (float)frame_height
					),
		fAngle, eFlags);

	return S_OK;
}

HRESULT CALLBACK CRender2D::DrawSpriteC(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, const TRectF &stRect, float fAngle, E_EFFECT2D_FLAGS eFlags)
{
	if (pTexture == NULL)
		return S_FALSE;
	
	DrawTexture(pTexture, stCoords, stDimensions, stRect, fAngle, eFlags);

	return S_OK;
}

__forceinline void CRender2D::DrawTexture(ITexture *tex, const TPoint2 &coord, const TPoint2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags)
{
	ICoreTexture *p_tex = NULL;

	if (tex)
		tex->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_DO_BATCH_UPDATE(CRDM_TRIANGLE_STRIP, p_tex, (flags & EF_COLORMIX) || (flags & EF_VERTICES_COLOR));
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return;

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
			translate_back = MatrixTranslate(TPoint3(coord.x + _stRotationPoint.x, coord.y + _stRotationPoint.y, 0.f));
			translate = MatrixTranslate(TPoint3(-(coord.x + _stRotationPoint.x), -(coord.y + _stRotationPoint.y), 0.f));
		}
		else
		{
			translate_back = MatrixTranslate(TPoint3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f));
			translate = MatrixTranslate(TPoint3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
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

		TMatrix translate_back = MatrixTranslate(TPoint3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f));
		TMatrix translate = MatrixTranslate(TPoint3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
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
		return;

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

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
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
			_pCoreRenderer->ToggleAlphaTestState(false);
			_pCoreRenderer->ToggleBlendState(true);
		}
		else 
		{
			_stRasterStateDesc.bAlphaTestEnable = true;
			_stBlendStateDesc.bEnable = false;
			_pCoreRenderer->ToggleAlphaTestState(true);
			_pCoreRenderer->ToggleBlendState(false);
		}
	}

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
			{
				if (flags & EF_VERTICES_COLOR)
				{
					_batchAccumulator.push_back(TVertex2(_pBuffer[2], _pBuffer[3], _pBuffer[8],	_pBuffer[9], _astVerticesColors[1].r,_astVerticesColors[1].g,_astVerticesColors[1].b,_astVerticesColors[1].a));
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
		
		_iObjsDrawnCount++;
	}
}

void CRender2D::DrawTexQuadAsIs(ITexture *tex, const float *points, E_EFFECT2D_FLAGS flags)
{
	ICoreTexture *p_tex = NULL;

	if (tex)
		tex->GetCoreTexture(p_tex);

	bool do_batch_update = _2D_DO_BATCH_UPDATE(CRDM_TRIANGLE_STRIP, p_tex, (flags & EF_COLORMIX) || (flags & EF_VERTICES_COLOR));
	
	if (_batchMode != BM_DISABLED && !do_batch_update)
		return;

	if (_batchMode == BM_DISABLED || _batchAccumulator.empty())
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
			_pCoreRenderer->ToggleAlphaTestState(false);
			_pCoreRenderer->ToggleBlendState(true);
		}
		else 
		{
			_stRasterStateDesc.bAlphaTestEnable = true;
			_stBlendStateDesc.bEnable = false;
			_pCoreRenderer->ToggleAlphaTestState(true);
			_pCoreRenderer->ToggleBlendState(false);
		}
	}

	if (do_batch_update)
		if (!_batchNeedToRefreshBatches)
			_batchBufferReadyToRender = true;
		else
			{
				if (flags & EF_VERTICES_COLOR)
				{
					_batchAccumulator.push_back(TVertex2(points[4], points[5], points[6],	points[7], _astVerticesColors[1].r,_astVerticesColors[1].g,_astVerticesColors[1].b,_astVerticesColors[1].a));
					_batchAccumulator.push_back(TVertex2(points[8], points[9], points[10],	points[11],_astVerticesColors[2].r,_astVerticesColors[2].g,_astVerticesColors[2].b,_astVerticesColors[2].a));
					_batchAccumulator.push_back(TVertex2(points[0], points[1], points[2],	points[3], _astVerticesColors[0].r,_astVerticesColors[0].g,_astVerticesColors[0].b,_astVerticesColors[0].a));
					_batchAccumulator.push_back(TVertex2(points[12],points[13],points[14],  points[15],_astVerticesColors[3].r,_astVerticesColors[3].g,_astVerticesColors[3].b,_astVerticesColors[3].a));
				}
				else
				{
					TColor4 col;
					if (flags & EF_COLORMIX) col = _stColormix;
					_batchAccumulator.push_back(TVertex2(points[4], points[5], points[6],	points[7], col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(points[8], points[9], points[10],	points[11],col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(points[0], points[1], points[2],	points[3], col.r,col.g,col.b,col.a));
					_batchAccumulator.push_back(TVertex2(points[12],points[13],points[14],  points[15],col.r,col.g,col.b,col.a));
				}
			}
	else
	{

		_pBuffer[0] = points[4]; _pBuffer[1] = points[5]; _pBuffer[2] = points[6]; _pBuffer[3] = points[7];
		_pBuffer[4] = points[8]; _pBuffer[5] = points[9]; _pBuffer[6] = points[10]; _pBuffer[7] = points[11];
		_pBuffer[8] = points[0]; _pBuffer[9] = points[1]; _pBuffer[10] = points[2]; _pBuffer[11] = points[3];
		_pBuffer[12] = points[12]; _pBuffer[13] = points[13]; _pBuffer[14] = points[14]; _pBuffer[15] = points[15];

		TDrawDataDesc desc((uint8 *)_pBuffer, 2*sizeof(float));
		desc.uiVertexStride = 4*sizeof(float);
		desc.uiTexCoordStride = 4*sizeof(float);

		if (flags & EF_VERTICES_COLOR)
		{
			desc.uiColorOffset = 16*sizeof(float);
			memcpy(&_pBuffer[16], _astVerticesColors, sizeof(TColor4)*4);
		}
		
		_pCoreRenderer->Draw(desc, CRDM_TRIANGLE_STRIP, 4);
		
		if (!_bInProfilerMode)
			_iObjsDrawnCount++;
	}
}

void CRender2D::SetColorMixPush(const TColor4 &color)
{
	_stPrevColormix = _stColormix;
	_stColormix = color;
}

void CRender2D::ColorMixPop()
{
	_stColormix = _stPrevColormix;
}

HRESULT CALLBACK CRender2D::SetRotationPoint(const TPoint2 &stCoords)
{
	_stRotationPoint = stCoords;
	return S_OK;
}

HRESULT CALLBACK CRender2D::SetScale(const TPoint2 &stScale)
{
	_stScale = stScale;
	return S_OK;
}

HRESULT CALLBACK CRender2D::SetColorMix(const TColor4 &stColor)
{
	_stColormix = stColor;
	return S_OK;
}

HRESULT CALLBACK CRender2D::SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode)
{
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

HRESULT CALLBACK CRender2D::SetVerticesOffset(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4)
{
	_astVerticesOffset[0] = stCoords1;
	_astVerticesOffset[1] = stCoords2;
	_astVerticesOffset[2] = stCoords3;
	_astVerticesOffset[3] = stCoords4;

	return S_OK;
}

HRESULT CALLBACK CRender2D::SetVerticesColors( const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4 )
{
	_astVerticesColors[0] = stColor1;
	_astVerticesColors[1] = stColor2;
	_astVerticesColors[2] = stColor3;
	_astVerticesColors[3] = stColor4;

	return S_OK;
}