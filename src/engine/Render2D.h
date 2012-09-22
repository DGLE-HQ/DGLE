/**
\author		Korotkov Andrey aka DRON
\date		22.09.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender2D: public CInstancedObj, public IRender2D
{
	ICoreRenderer *_pCoreRenderer;
	TRasterizerStateDesc _stRasterStateDesc;
	TBlendStateDesc _stBlendStateDesc;

	bool _bIn2D;
	bool _bUseGeometryBuffers;
	
	static const uint _sc_uiMaxBatchsPerFrame = 64;

	std::vector<TVertex2>	
				 _batchAccumulator;
	ICoreTexture *_pBatchCurTex;
	E_CORE_RENDERER_DRAW_MODE
				 _eBatchDrawMode;
	bool		 _bBatchColor;
	E_BATCH_MODE2D	
				 _batchMode,
				 _batchPreProfilerMode;
	bool		 _batchNeedToRefreshBatches;
	std::vector<ICoreGeometryBuffer *>
				 _pBatchBuffers;
	uint		 _batchBufferCurCounter;
	bool		 _batchBufferReadyToRender;
	
	bool		 _bInLocalBatchMode, _bLocalBatchUEP, _bLocalUEPWasTurnedOn;
	uint		 _batchBuffersRepetedUseCounter,
				 _batchBuffersNotModefiedPerFrameCounter,
				 _batchsCount,
				 _batchMaxSize,
				 _batchMinSize;

	inline void	 _BatchFlush();
	inline bool  _BatchSet(E_CORE_RENDERER_DRAW_MODE eDrawMode, ICoreTexture *pTex, bool bColor);

	bool		_bInProfilerMode, _bCameraWasSet;
	TMatrix		_stCamTransform, _stPrevCamTransform;
	TPoint2		_stCamScale;
	
	TMatrix		_stPrevProjMat, _stPrevModelViewMat;
	uint		_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH;
	E_EFFECT2D_BLENDING_FLAGS
				_ePrevBlendingMode;

	float		*_pBuffer;
	uint		_uiBufferSize;

	float		_fLineWidth;

	TColor4		_stColormix, _astVerticesColors[4];

	TPoint2		_stRotationPoint, _stScale,
				_astVerticesOffset[4];

	int			_iResCorWidth, _iResCorHeight;
	bool		_iResCorConstProp;
	float		_fResCorCoef;

	uint		_uiScreenWidth, _uiScreenHeight,
				_uiCropX, _uiCropY,
				_uiCropW, _uiCropH;

	bool		_bViewportChanged;

	uint64		_ui64DrawDelay, _ui64DrawAverallDelay;
	int			_iObjsDrawnCount;

	int			_iProfilerState,
				_iDoDrawBBoxes;

	void		_SetDefaultStates();
	void		_Set2DProjMatrix(uint width, uint height);

public:

	CRender2D(uint uiInstIdx);
	~CRender2D();

	inline bool In2DMode() const {return _bIn2D;}
	__forceinline bool BBoxInScreen(const float *vertices, bool rotated) const; /**< \note \a vertices size must be 8, x and y for each quad point. \a rotated true if quad is not AABB. */
	__forceinline HRESULT DrawTexture(ITexture *tex, const TPoint2 &coord, const TPoint2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags);

	void BeginFrame();
	void EndFrame();
	void RefreshBatchData();
	void DrawProfiler();
	void BeginProfiler2D();
	void EndProfiler2D();

	HRESULT DGLE2_API Begin2D();
	HRESULT DGLE2_API End2D();
	HRESULT DGLE2_API BatchRender(E_BATCH_MODE2D eMode);
	HRESULT DGLE2_API InvalidateBatchData();
	HRESULT DGLE2_API BeginBatch(bool bUpdateEveryFrame);
	HRESULT DGLE2_API EndBatch();
	HRESULT DGLE2_API NeedToUpdateBatchData(bool &bNeedUpdate);
	HRESULT DGLE2_API SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions); 
	HRESULT DGLE2_API SetCamera(const TPoint2 &stCenter, float fAngle, const TPoint2 &stScale);
	HRESULT DGLE2_API CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull);

	HRESULT DGLE2_API LineWidth(uint uiWidth);
	HRESULT DGLE2_API DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize);
	HRESULT DGLE2_API DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawRect(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawEllipse(const TPoint2 &stCoords, const TPoint2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawPolygon(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);

	HRESULT DGLE2_API DrawSpriteS(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawSpriteA(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawSpriteC(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, const TRectF &stRect, float fAngle, E_EFFECT2D_FLAGS eFlags);

	HRESULT DGLE2_API DrawTriangles(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis, float fAngle, bool bClip, float fFovY, E_EFFECT2D_FLAGS eFlags);

	HRESULT DGLE2_API Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags);
	HRESULT DGLE2_API DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix &stTransform, const TVector3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY);

	HRESULT DGLE2_API SetRotationPoint(const TPoint2 &stCoords);
	HRESULT DGLE2_API SetScale(const TPoint2 &stScale);
	HRESULT DGLE2_API SetColorMix(const TColor4 &stColor);
	HRESULT DGLE2_API SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode);
	HRESULT DGLE2_API SetVerticesOffset(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4);
	HRESULT DGLE2_API SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4);

	HRESULT DGLE2_API GetRotationPoint(TPoint2 &stCoords);
	HRESULT DGLE2_API GetScale(TPoint2 &stScale);
	HRESULT DGLE2_API GetColorMix(TColor4 &stColor);
	HRESULT DGLE2_API GetBlendMode(E_EFFECT2D_BLENDING_FLAGS &eMode);
	HRESULT DGLE2_API GetVerticesOffset(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4);
	HRESULT DGLE2_API GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4);

	IDGLE2_BASE_IMPLEMENTATION(IRender2D)

};