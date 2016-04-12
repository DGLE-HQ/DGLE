/**
\author		Korotkov Andrey aka DRON
\date		12.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender2D final : public CInstancedObj, public IRender2D
{
	ICoreRenderer *_pCoreRenderer;
	TRasterizerStateDesc _stRasterStateDesc;
	TBlendStateDesc _stBlendStateDesc;

	bool _bIn2D;
	bool _bUseGeometryBuffers;
	
	static constexpr uint _sc_uiMaxBatchsPerFrame = 64;

	std::vector<TVertex2> _vecBatchAccumulator;
	ICoreTexture *_pBatchCurTex;
	E_CORE_RENDERER_DRAW_MODE _eBatchDrawMode;
	bool _bBatchColor, _bBatchAlphaTest, _bBatchBlending;
	E_BATCH_MODE2D _batchMode, _batchPreProfilerMode;
	bool _batchNeedToRefreshBatches;
	std::vector<ICoreGeometryBuffer *> _vecBatchBuffers;
	uint _batchBufferCurCounter;
	bool _batchBufferReadyToRender;
	
	bool _bInLocalBatchMode, _bLocalBatchUEP, _bLocalUEPWasTurnedOn;
	uint _batchBuffersRepetedUseCounter,
		_batchBuffersNotModefiedPerFrameCounter,
		_batchsCount, _batchMaxSize, _batchMinSize;

	inline void _BatchFlush();
	inline bool _BatchSet(E_CORE_RENDERER_DRAW_MODE eDrawMode, ICoreTexture *pTex, bool bColor, bool bAlphaTest, bool bBlending);

	bool _bInProfilerMode, _bCameraWasSet;
	TMatrix4 _stCamTransform, _stPrevCamTransform;
	TPoint2 _stCamScale;
	
	TMatrix4 _stPrevProjMat, _stPrevModelViewMat;
	uint _uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH;
	E_BLENDING_EFFECT _ePrevBlendingMode;

	float *_pBuffer;
	uint _uiBufferSize;

	uint8 *_pPolyTrisBuffer;
	uint _uiPolyTrisBufferSize;

	uint _uiLineWidth, _uiPointSize;

	TColor4 _stColormix, _astVerticesColors[4];

	TPoint2	_stRotationPoint, _stScale,
		_astVerticesOffset[4];

	int	_iResCorWidth, _iResCorHeight;
	bool _bResCorConstProp;
	float _fResCorCoef;

	uint _uiScreenWidth, _uiScreenHeight,
		_uiCropX, _uiCropY,
		_uiCropW, _uiCropH;

	bool _bViewportChanged;

	uint64 _ui64DrawDelay, _ui64DrawAverallDelay;
	uint _uiObjsDrawnCount;

	int _iProfilerState, _iDoDrawBBoxes;

	void _SetDefaultStates();
	void _Set2DProjMatrix(uint width, uint height);

public:

	CRender2D(uint uiInstIdx);
	~CRender2D();

	inline uint64 GetAverallDelay() const {return _ui64DrawAverallDelay;}
	inline bool In2DMode() const {return _bIn2D;}
	FORCE_INLINE bool BBoxInScreen(const float *vertices, bool rotated) const; /**< \note \a vertices size must be 8, x and y for each quad point. \a rotated true if quad is not AABB. */
	FORCE_INLINE DGLE_RESULT DrawTexture(ITexture *tex, const TPoint2 &coord, const TVector2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags);

	void BeginFrame();
	void EndFrame();
	void RefreshBatchData();
	void DrawProfiler();
	void BeginProfiler2D();
	void EndProfiler2D();

	DGLE_RESULT DGLE_API Begin2D() override;
	DGLE_RESULT DGLE_API End2D() override;
	DGLE_RESULT DGLE_API BatchRender(E_BATCH_MODE2D eMode) override;
	DGLE_RESULT DGLE_API InvalidateBatchData() override;
	DGLE_RESULT DGLE_API BeginBatch(bool bUpdateEveryFrame) override;
	DGLE_RESULT DGLE_API EndBatch() override;
	DGLE_RESULT DGLE_API NeedToUpdateBatchData(bool &bNeedUpdate) override;
	DGLE_RESULT DGLE_API SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions) override;
	DGLE_RESULT DGLE_API ResolutionCorrectToAbsolute(const TPoint2 &stLogicCoord, TPoint2 &stAbsoluteCoord) override;
	DGLE_RESULT DGLE_API AbsoluteToResolutionCorrect(const TPoint2 &stAbsoluteCoord, TPoint2 &stLogicCoord) override;
	DGLE_RESULT DGLE_API SetCamera(const TPoint2 &stCenter, float fAngle, const TVector2 &stScale) override;
	DGLE_RESULT DGLE_API ResetCamera() override;
	DGLE_RESULT DGLE_API UnprojectCameraToScreen(const TPoint2 &stCameraCoord, TPoint2 &stScreenCoord) override;
	DGLE_RESULT DGLE_API ProjectScreenToCamera(const TPoint2 &stScreenCoord, TPoint2 &stCameraCoord) override;
	DGLE_RESULT DGLE_API CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull) override;

	DGLE_RESULT DGLE_API SetLineWidth(uint uiWidth) override;
	DGLE_RESULT DGLE_API DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize) override;
	DGLE_RESULT DGLE_API DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawRectangle(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawEllipse(const TPoint2 &stCoords, const TVector2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawPolygon(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags) override;

	DGLE_RESULT DGLE_API DrawTexture(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawTextureCropped(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, const TRectF &stTexCropRect, float fAngle, E_EFFECT2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawTextureSprite(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags) override;

	DGLE_RESULT DGLE_API DrawTriangles(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis, float fAngle, E_EFFECT2D_FLAGS eFlags, bool bClip, float fFovY, bool bClearDepthBuffer) override;

	DGLE_RESULT DGLE_API Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) override;
	DGLE_RESULT DGLE_API DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix4x4 &stTransform, const TPoint3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY, bool bClearDepthBuffer) override;

	DGLE_RESULT DGLE_API SetRotationPoint(const TPoint2 &stCoords) override;
	DGLE_RESULT DGLE_API SetScale(const TPoint2 &stScale) override;
	DGLE_RESULT DGLE_API SetColorMix(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API SetBlendMode(E_BLENDING_EFFECT eMode) override;
	DGLE_RESULT DGLE_API SetVerticesOffsets(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4) override;
	DGLE_RESULT DGLE_API SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4) override;

	DGLE_RESULT DGLE_API GetRotationPoint(TPoint2 &stCoords) override;
	DGLE_RESULT DGLE_API GetScale(TPoint2 &stScale) override;
	DGLE_RESULT DGLE_API GetColorMix(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetBlendMode(E_BLENDING_EFFECT &eMode) override;
	DGLE_RESULT DGLE_API GetVerticesOffsets(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4) override;
	DGLE_RESULT DGLE_API GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4) override;

	IDGLE_BASE_IMPLEMENTATION(IRender2D, INTERFACE_IMPL_END)
};

#pragma region inline
#define _2D_BATCH_NEED_UPDATE(mode, tex, is_color, do_alpha_test, do_blending)\
((_batchMode > BM_DISABLED || _bInLocalBatchMode) &&\
(_BatchSet(mode, tex, is_color, do_alpha_test, do_blending) || ((_batchNeedToRefreshBatches || (_bInLocalBatchMode && !_bLocalBatchUEP)) || ((_batchMode == BM_ENABLED_UPDATE_EVERY_TICK || (_bInLocalBatchMode && _bLocalBatchUEP)) && !_batchNeedToRefreshBatches && !_batchBufferReadyToRender))))
#define _2D_IF_BATCH_NO_UPDATE_EXIT if (do_batch_update) if (!_batchNeedToRefreshBatches && !(_bInLocalBatchMode && !_bLocalBatchUEP)) _batchBufferReadyToRender = true;
#define _2D_IF_BATCH_DO_SET_STATES if ((_batchMode == BM_DISABLED && !_bInLocalBatchMode) || _vecBatchAccumulator.empty())
#define _2D_BATCH_DUMMY_DRAW_CALL_EXIT if ((_batchMode != BM_DISABLED || _bInLocalBatchMode) && !do_batch_update) return S_OK;

#define IN_2D_GUARD if (!_bIn2D && !_bInProfilerMode) Begin2D();

#define COPY_COLOR_TO_BUFFER(buffer, start_idx, color) \
buffer[start_idx + 0] = color.rgba[0];\
buffer[start_idx + 1] = color.rgba[1];\
buffer[start_idx + 2] = color.rgba[2];\
buffer[start_idx + 3] = color.rgba[3];

FORCE_INLINE bool CRender2D::BBoxInScreen(const float *vertices, bool rotated) const
{
	if (_bInProfilerMode)
		return true;

	float vrtcs[10];

	if (_bCameraWasSet)
	{
		rotated = true;

		for (int i = 0; i < 4; ++i)
		{
			const float	&x = vertices[i * 2], &y = vertices[i * 2 + 1];

			vrtcs[i * 2] = _stCamTransform._2D[0][0] * x + _stCamTransform._2D[1][0] * y + _stCamTransform._2D[3][0];
			vrtcs[i * 2 + 1] = _stCamTransform._2D[0][1] * x + _stCamTransform._2D[1][1] * y + _stCamTransform._2D[3][1];
		}
	}
	else
	{
		vrtcs[0] = vertices[0]; vrtcs[1] = vertices[1]; vrtcs[2] = vertices[2]; vrtcs[3] = vertices[3];
		vrtcs[4] = vertices[4]; vrtcs[5] = vertices[5]; vrtcs[6] = vertices[6]; vrtcs[7] = vertices[7];
	}

	if (_iDoDrawBBoxes == 1 && (_batchMode == BM_DISABLED && !_bInLocalBatchMode))
	{
		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(MatrixInverse(_stCamTransform) * _stCamTransform);

		vrtcs[8] = vrtcs[0]; vrtcs[9] = vrtcs[1];

		_pCoreRenderer->BindTexture(NULL);
		_pCoreRenderer->SetColor(ColorWhite());
		_pCoreRenderer->SetLineWidth(1.f);

		_pCoreRenderer->Draw(TDrawDataDesc((uint8*)vrtcs, -1, -1, true), CRDM_LINE_STRIP, 5);

		_pCoreRenderer->SetLineWidth((float)_uiLineWidth);

		if (_bCameraWasSet)
			_pCoreRenderer->SetMatrix(_stCamTransform);
	}

	if (!rotated && (((vrtcs[0] > _uiScreenWidth) || (vrtcs[2] < 0.f) || (vrtcs[1] > _uiScreenHeight) || (vrtcs[5] < 0.f))))
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

DGLE_RESULT CRender2D::DrawTexture(ITexture *tex, const TPoint2 &coord, const TVector2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags)
{
	IN_2D_GUARD

	ICoreTexture *p_tex = NULL;

	if (tex)
		tex->GetCoreTexture(p_tex);

	const bool do_batch_update = _2D_BATCH_NEED_UPDATE(CRDM_TRIANGLES, p_tex, (flags & EF_COLOR_MIX) || (flags & EF_VERTICES_COLORS), !(flags & EF_NONE) && !(flags & EF_BLEND), (flags & EF_BLEND) != 0);
	
	_2D_BATCH_DUMMY_DRAW_CALL_EXIT

	_pBuffer[0] = coord.x; _pBuffer[1] = coord.y;
	_pBuffer[2] = coord.x + dimension.x; _pBuffer[3] = coord.y;
	_pBuffer[4] = _pBuffer[2]; _pBuffer[5] = coord.y + dimension.y;
	_pBuffer[6] = coord.x; _pBuffer[7] = _pBuffer[5];

	if (flags & EF_VERTICES_OFFSETS)
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

	TMatrix4 transform;
	
	if (angle != 0.f)
	{
		TMatrix4 translate, translate_back, rot = MatrixIdentity();
		
		const float s = sinf(-angle * (float)M_PI / 180.f), c = cosf(-angle * (float)M_PI / 180.f);

		rot._2D[0][0] = c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = s;
		rot._2D[1][1] = c;

		if (flags & EF_ROTATION_POINT)
		{
			translate_back = MatrixTranslate(TVector3(coord.x + _stRotationPoint.x, coord.y + _stRotationPoint.y, 0.f));
			translate = MatrixTranslate(TVector3(-(coord.x + _stRotationPoint.x), -(coord.y + _stRotationPoint.y), 0.f));
		}
		else
		{
			translate_back = MatrixTranslate(TVector3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f));
			translate = MatrixTranslate(TVector3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
		}

		transform = translate * rot * translate_back;
	}
	else
		transform = MatrixIdentity();

	if (flags & EF_SCALE)
	{
		TMatrix4 scale = MatrixIdentity();
		scale._2D[0][0] = _stScale.x;
		scale._2D[1][1] = _stScale.y;

		const TMatrix4 translate_back = MatrixTranslate(TVector3(coord.x + dimension.x / 2.f, coord.y + dimension.y / 2.f, 0.f)),
			translate = MatrixTranslate(TVector3(-(coord.x + dimension.x / 2.f), -(coord.y + dimension.y / 2.f), 0.f));
		
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

		_pBuffer[12] = rect.x;
		_pBuffer[9]	 = rect.y;
		
		if (flags & EF_TILE_TEXTURE)
		{
			_pBuffer[8]	 = rect.x + dimension.x;
			_pBuffer[11] = rect.y + dimension.y;
		}
		else
		{
			_pBuffer[8]	 = rect.x + rect.width;
			_pBuffer[11] = rect.y + rect.height;
		}

		_pBuffer[12] /= tex_width;
		_pBuffer[8]	 /= tex_width;
		_pBuffer[9]	 /= tex_height;
		_pBuffer[11] /= tex_height;

		if (flags & EF_FLIP_HORIZONTALLY)
		{
			_pBuffer[10] = _pBuffer[12];
			_pBuffer[12] = _pBuffer[8];
			_pBuffer[8]	 = _pBuffer[10];
		}

		if (flags & EF_FLIP_VERTICALLY)
		{
			_pBuffer[10] = _pBuffer[9];
			_pBuffer[9]	 = _pBuffer[11];
			_pBuffer[11] = _pBuffer[10];
		}
	}

	_2D_IF_BATCH_DO_SET_STATES
	{
		if (flags & EF_COLOR_MIX)
			_pCoreRenderer->SetColor(_stColormix);
		else  
			_pCoreRenderer->SetColor(ColorWhite());

		_pCoreRenderer->BindTexture(p_tex);

		if (flags & EF_BLEND)
		{
			_stRasterStateDesc.bAlphaTestEnabled = false;
			_stBlendStateDesc.bEnabled = true;
		}
		else
			if (flags & EF_NONE)
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
				if (flags & EF_VERTICES_COLORS)
				{
					const uint idx = _vecBatchAccumulator.size();
					_vecBatchAccumulator.resize(_vecBatchAccumulator.size() + 6);

					_vecBatchAccumulator[idx + 0] = TVertex2(_pBuffer[2], _pBuffer[3], _pBuffer[8], _pBuffer[9], _astVerticesColors[0].r, _astVerticesColors[0].g, _astVerticesColors[0].b, _astVerticesColors[0].a);
					_vecBatchAccumulator[idx + 1] = TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8],	_pBuffer[11], _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 2] = TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12], _pBuffer[9], _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
					_vecBatchAccumulator[idx + 3] = TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8],	_pBuffer[11], _astVerticesColors[1].r, _astVerticesColors[1].g, _astVerticesColors[1].b, _astVerticesColors[1].a);
					_vecBatchAccumulator[idx + 4] = TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12], _pBuffer[9], _astVerticesColors[2].r, _astVerticesColors[2].g, _astVerticesColors[2].b, _astVerticesColors[2].a);
					_vecBatchAccumulator[idx + 5] = TVertex2(_pBuffer[6], _pBuffer[7], _pBuffer[12], _pBuffer[11], _astVerticesColors[3].r, _astVerticesColors[3].g, _astVerticesColors[3].b, _astVerticesColors[3].a);
				}
				else
				{
					TColor4 col;

					if (flags & EF_COLOR_MIX)
						col = _stColormix;

					const uint idx = _vecBatchAccumulator.size();
					_vecBatchAccumulator.resize(_vecBatchAccumulator.size() + 6);

					_vecBatchAccumulator[idx + 0] = TVertex2(_pBuffer[2], _pBuffer[3], _pBuffer[8], _pBuffer[9], col.r, col.g, col.b, col.a);
					_vecBatchAccumulator[idx + 1] = TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8], _pBuffer[11], col.r, col.g, col.b, col.a);
					_vecBatchAccumulator[idx + 2] = TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12],_pBuffer[9], col.r, col.g, col.b, col.a);
					_vecBatchAccumulator[idx + 3] = TVertex2(_pBuffer[4], _pBuffer[5], _pBuffer[8], _pBuffer[11], col.r, col.g, col.b, col.a);
					_vecBatchAccumulator[idx + 4] = TVertex2(_pBuffer[0], _pBuffer[1], _pBuffer[12], _pBuffer[9], col.r, col.g, col.b, col.a);
					_vecBatchAccumulator[idx + 5] = TVertex2(_pBuffer[6], _pBuffer[7], _pBuffer[12], _pBuffer[11], col.r, col.g, col.b, col.a);
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

		TDrawDataDesc desc((uint8 *)_pBuffer, -1, 8 * sizeof(float), true);
		
		if (flags & EF_VERTICES_COLORS)
		{
			desc.uiColorOffset = 16 * sizeof(float);
			COPY_COLOR_TO_BUFFER(_pBuffer, 16, _astVerticesColors[0])
			COPY_COLOR_TO_BUFFER(_pBuffer, 20, _astVerticesColors[1])
			COPY_COLOR_TO_BUFFER(_pBuffer, 24, _astVerticesColors[2])
			COPY_COLOR_TO_BUFFER(_pBuffer, 28, _astVerticesColors[3])
		}

		_pCoreRenderer->Draw(desc, CRDM_TRIANGLE_STRIP, 4);
		
		++_uiObjsDrawnCount;
	}

	return S_OK;
}

#ifndef __RENDER2D__
#undef _2D_BATCH_NEED_UPDATE
#undef _2D_IF_BATCH_NO_UPDATE_EXIT
#undef _2D_IF_BATCH_DO_SET_STATES
#undef _2D_BATCH_DUMMY_DRAW_CALL_EXIT

#undef IN_2D_GUARD

#undef COPY_COLOR_TO_BUFFER
#endif
#pragma endregion implementation