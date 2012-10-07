/**
\author		Andrey Korotkov aka DRON
\date		19.08.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#ifdef PLATFORM_WINDOWS
#	include "platform\win\BaseRendererGL.h"
#endif

#include "StateManagerGL.h"

struct TState
{
	std::vector<bool> clActivatedTexUnits;
	GLint iViewPortX, iViewPortY, iViewPortWidth, iViewPortHeight;

	TBlendStateDesc stBlendDesc;
	TDepthStencilDesc stDepthStencilDesc;
	TRasterizerStateDesc stRasterDesc;

	TMatrix stModelviewMat, stProjectionMat, stTextureMat;

	TColor4 stColor;

	TState(): clActivatedTexUnits(32),
	iViewPortX(0), iViewPortY(0), iViewPortWidth(0), iViewPortHeight(0)
	{
		stModelviewMat = MatrixIdentity();
		stProjectionMat = MatrixIdentity();
		stTextureMat = MatrixIdentity();
	}
};

class CCoreGeometryBuffer;

class CCoreRendererGL: private CBaseRendererGL, public ICoreRenderer
{
	static const uint _sc_uiMaxVerticesCountForLegacy = 128;
	static const uint _sc_uiMaxDrawCallsPerFrame = 3000;

	CStateManager<false> _clPassThroughStateMan;
	CStateManager<true>	*_pCachedStateMan;
	IStateManager *_pStateMan;

	std::string _strOpenGLExtensions;
	int _iMaxTexResolution, _iMaxAnisotropy, _iMaxLight, _iMaxTexUnits;
	bool _bIsGLSLSupported;

	int _iProfilerState;

	std::stack<TState> _clStatesStack;
	TState _stCurrentState;

	TDrawDataDesc _stDrawDataDesc;
	E_CORE_RENDERER_DRAW_MODE _eDrawMode;
	uint _uiDrawCount;

	CCoreGeometryBuffer *_pCurBindedBuffer, *_pLastDrawnBuffer;
	bool _bVerticesBufferBindedFlag, _bIndexesBufferBindedFlag;

	bool _bStateFilterEnabled;
	uint _uiUnfilteredDrawSetups, _uiOverallDrawSetups, _uiOverallDrawCalls, _uiOverallTrianglesCount;

	inline GLenum _GetGLBlendFactor(E_BLEND_FACTOR factor);
	inline GLenum _GetGLComparsionMode(E_COMPARISON_FUNC mode);
	inline GLenum _GetGLDrawMode(E_CORE_RENDERER_DRAW_MODE eMode);
	__forceinline bool _LegacyDraw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	inline void _TriangleStatistics(E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	void _ProfilerEventHandler(IBaseEvent *pEvent) const;

	static void DGLE2_API _s_ConPrintGLExts(void *pParametr, const char *pcParam);
	static void DGLE2_API _s_ProfilerEventHandler(void *pParametr, IBaseEvent *pEvent);

public:
	
	CCoreRendererGL(uint uiInstIdx);

	inline IStateManager* pStateMan() const {return _pStateMan;} /**< \warning Never copy IStateManager* because _pStateMan may be reallocated at any time! */

	DGLE2_RESULT DGLE2_API Prepare(TCRendererInitResult &stResults);
	DGLE2_RESULT DGLE2_API Initialize(TCRendererInitResult &stResults);
	DGLE2_RESULT DGLE2_API Finalize();
	DGLE2_RESULT DGLE2_API AdjustMode(TEngWindow &stNewWin);
	DGLE2_RESULT DGLE2_API MakeCurrent();
	DGLE2_RESULT DGLE2_API Present();

	DGLE2_RESULT DGLE2_API SetClearColor(const TColor4 &stColor);
	DGLE2_RESULT DGLE2_API Clear(bool bColor, bool bDepth, bool bStencil);
	DGLE2_RESULT DGLE2_API SetViewport(uint x, uint y, uint width, uint height);
	DGLE2_RESULT DGLE2_API GetViewport(uint &x, uint &y, uint &width, uint &height);
	DGLE2_RESULT DGLE2_API SetScissor(uint x, uint y, uint width, uint height);
	DGLE2_RESULT DGLE2_API SetLineWidth(float fWidth);
	DGLE2_RESULT DGLE2_API SetPointSize(float fSize);
	DGLE2_RESULT DGLE2_API ReadFrameBuffer(uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat);
	DGLE2_RESULT DGLE2_API SetRenderTarget(ICoreTexture *pTexture);
	DGLE2_RESULT DGLE2_API CreateTexture(ICoreTexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags);
	DGLE2_RESULT DGLE2_API CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType);
	DGLE2_RESULT DGLE2_API ToggleStateFilter(bool bEnabled);
	DGLE2_RESULT DGLE2_API PushStates();
	DGLE2_RESULT DGLE2_API PopStates();
	DGLE2_RESULT DGLE2_API SetMatrix(const TMatrix &stMat, E_MATRIX_TYPE eMatType);
	DGLE2_RESULT DGLE2_API GetMatrix(TMatrix &stMat, E_MATRIX_TYPE eMatType);
	DGLE2_RESULT DGLE2_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	DGLE2_RESULT DGLE2_API DrawBuffer(ICoreGeometryBuffer *pBuffer);
	DGLE2_RESULT DGLE2_API SetColor(const TColor4 &stColor);
	DGLE2_RESULT DGLE2_API ToggleBlendState(bool bEnabled);
	DGLE2_RESULT DGLE2_API ToggleAlphaTestState(bool bEnabled);
	DGLE2_RESULT DGLE2_API SetBlendState(const TBlendStateDesc &stState);
	DGLE2_RESULT DGLE2_API GetBlendState(TBlendStateDesc &stState);
	DGLE2_RESULT DGLE2_API SetDepthStencilState(const TDepthStencilDesc &stState);
	DGLE2_RESULT DGLE2_API GetDepthStencilState(TDepthStencilDesc &stState);
	DGLE2_RESULT DGLE2_API SetRasterizerState(const TRasterizerStateDesc &stState);
	DGLE2_RESULT DGLE2_API GetRasterizerState(TRasterizerStateDesc &stState);
	DGLE2_RESULT DGLE2_API BindTexture(ICoreTexture *pTex, uint uiTextureLayer);
	DGLE2_RESULT DGLE2_API GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue);
	DGLE2_RESULT DGLE2_API IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported);
	DGLE2_RESULT DGLE2_API GetRendererType(E_CORE_RENDERER_TYPE &eType);
	DGLE2_RESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE2_BASE_IMPLEMENTATION(ICoreRenderer)
};

#endif