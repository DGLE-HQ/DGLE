/**
\author		Korotkov Andrey aka DRON
\date		30.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _RENDER2D_H
#define _RENDER2D_H

#include "Common.h"

class CRender2D: public CInstancedObj, public IRender2D
{
	ICoreRenderer *_pCoreRenderer;
	TRasterizerStateDesc _stRasterStateDesc;
	TBlendStateDesc _stBlendStateDesc;

	bool _bIn2D;

	// Batching //
	
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
	std::vector<ICoreGeometryBuffer*>
				 _pBatchBuffers;
	uint		 _batchBufferCurCounter;
	bool		 _batchBufferReadyToRender;/**< \remark Only in BM_ENABLED_UEP mode. */
	
	//Profilers//

	bool		 _bInProfilerMode;
	uint		 _batchBuffersRepetedUseCounter,
				 _batchBuffersNotModefiedPerFrameCounter,
				 _batchMaxSize,
				 _batchMinSize;
	inline void	 _BatchFlush();
	inline bool  _BatchSet(E_CORE_RENDERER_DRAW_MODE eDrawMode, ICoreTexture *pTex, bool bColor); /**< \note Returns true if batch was flushed. */

	// End Batching //

	bool		_bCameraWasSet;
	TMatrix		_stCamTransform;
	TPoint2		_stCamScale;
	
	// Saved State //
	TMatrix		_stPrevProjMat, _stPrevModelViewMat;
	uint		_uiPrevViewPortX, _uiPrevViewPortY, _uiPrevViewPortW, _uiPrevViewPortH;
	E_EFFECT2D_BLENDING_FLAGS
				_ePrevBlendingMode;

	float		*_pBuffer;
	uint		_uiBufferSize;

	TColor4		_stColormix, _stPrevColormix,
				_astVerticesColors[4];

	TPoint2		_stRotationPoint, _stScale,
				_astVerticesOffset[4];

	int			_iResCorWidth, _iResCorHeight;
	bool		_iResCorConstProp;
	float		_fResCorKoef;

	uint		_uiScreenWidth, _uiScreenHeight,
				_uiCropX, _uiCropY,
				_uiCropW, _uiCropH;

	bool		_bViewportChanged;

	uint64		_ui64DrawDelay;
	int			_iObjsDrawnCount;

	int			_iProfilerState,
				_iDoDrawBBoxes;

	void		_SetDefaultStates();
	void		_Set2DProjMatrix(uint width, uint height);

public:

	CRender2D(uint uiInstIdx);
	~CRender2D();

	inline bool In2DMode() {return _bIn2D;}
	__forceinline bool BBoxInScreen(const float *vertices/*8 floats*/, bool rotated/*(angle != 0)*/) const; /**< \note \a vertices size must be 8, x and y for each quad point. */
	__forceinline void DrawTexture(ITexture *tex, const TPoint2 &coord, const TPoint2 &dimension, const TRectF &rect, float angle, E_EFFECT2D_FLAGS flags);
	
	//Extra//

	/** Sets new colormix and pushs previous. 
		\note Function can not be included in to another SetColorMixPush - ColorMixPop block.
	*/
	void SetColorMixPush(const TColor4 &color);
	/** Restores previous colormix. */
	void ColorMixPop();

	/** Fastest way to draw textured quad.
		\note Function doesn't perform in-screen check and some other computes. EF_SCALE, EF_VERTICES_OFFSET, EF_ROTATEPT, EF_FLIPX, EF_FLIPY flags don't take effect.
		\note \a points is array of 16 floats. You must set x, y, u and w data for each quad point.
		\warning Use with care.
	*/
	void DrawTexQuadAsIs(ITexture *tex, const float *points/*16 floats*/, E_EFFECT2D_FLAGS flags); 

	//For CCore use only//
	void RefreshBatchData();/**< Forces Render2D to rebuild all batches. */
	void DrawProfiler();
	void BeginProfiler2D();
	void EndProfiler2D();
	//------------------//

	//Interface

	HRESULT CALLBACK Begin2D();
	HRESULT CALLBACK End2D();
	HRESULT CALLBACK BatchRender(E_BATCH_MODE2D eMode);
	HRESULT CALLBACK SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions); 
	HRESULT CALLBACK SetCamera(const TPoint2 &stCenter, float fAngle, const TPoint2 &stScale);

	HRESULT CALLBACK LineWidth(uint uiWidth);
	HRESULT CALLBACK DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor, uint uiSize);
	HRESULT CALLBACK DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT CALLBACK DrawRect(const TRectF &stRect, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT CALLBACK DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT CALLBACK DrawEllipse(const TPoint2 &stCoords, const TPoint2 &stRadius, uint uiQuality, const TColor4 &stColor, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT CALLBACK DrawPolygon(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);

	HRESULT CALLBACK DrawSpriteS(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, float fAngle, E_EFFECT2D_FLAGS eFlags);
	HRESULT CALLBACK DrawSpriteA(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, uint uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags);
	HRESULT CALLBACK DrawSpriteC(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, const TRectF &stRect, float fAngle, E_EFFECT2D_FLAGS eFlags);

	HRESULT CALLBACK DrawTriangles(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);
	HRESULT CALLBACK DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TPoint3 &stDimensions, const TPoint3 &stAxis, float fAngle, bool bClip, float fFovY, E_EFFECT2D_FLAGS eFlags);

	HRESULT CALLBACK SetRotationPoint(const TPoint2 &stCoords);
	HRESULT CALLBACK SetScale(const TPoint2 &stScale);
	HRESULT CALLBACK SetColorMix(const TColor4 &stColor);
	HRESULT CALLBACK SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode);
	HRESULT CALLBACK SetVerticesOffset(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4);
	HRESULT CALLBACK SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4);

	IDGLE2_BASE_IMPLEMENTATION(IRender2D)

};

#endif //_RENDER2D_H