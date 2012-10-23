/**
\author		Andrey Korotkov aka DRON
\date		19.08.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#ifdef PLATFORM_WINDOWS
#	include "platform\windows\BaseRendererGL.h"
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

	TCRendererInitResult _stInitResults;

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

	static void DGLE_API _s_ConPrintGLExts(void *pParametr, const char *pcParam);
	static void DGLE_API _s_ProfilerEventHandler(void *pParametr, IBaseEvent *pEvent);

public:
	
	CCoreRendererGL(uint uiInstIdx);

	inline IStateManager* pStateMan() const {return _pStateMan;} /**< \warning Never copy IStateManager* because _pStateMan may be reallocated at any time! */

	DGLE_RESULT DGLE_API Prepare(TCRendererInitResult &stResults);
	DGLE_RESULT DGLE_API Initialize(TCRendererInitResult &stResults);
	DGLE_RESULT DGLE_API Finalize();
	DGLE_RESULT DGLE_API AdjustMode(TEngWindow &stNewWin);
	DGLE_RESULT DGLE_API MakeCurrent();
	DGLE_RESULT DGLE_API Present();

	DGLE_RESULT DGLE_API SetClearColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API Clear(bool bColor, bool bDepth, bool bStencil);
	DGLE_RESULT DGLE_API SetViewport(uint x, uint y, uint width, uint height);
	DGLE_RESULT DGLE_API GetViewport(uint &x, uint &y, uint &width, uint &height);
	DGLE_RESULT DGLE_API SetScissor(uint x, uint y, uint width, uint height);
	DGLE_RESULT DGLE_API SetLineWidth(float fWidth);
	DGLE_RESULT DGLE_API SetPointSize(float fSize);
	DGLE_RESULT DGLE_API ReadFrameBuffer(uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat);
	DGLE_RESULT DGLE_API SetRenderTarget(ICoreTexture *pTexture);
	DGLE_RESULT DGLE_API CreateTexture(ICoreTexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags);
	DGLE_RESULT DGLE_API CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType);
	DGLE_RESULT DGLE_API ToggleStateFilter(bool bEnabled);
	DGLE_RESULT DGLE_API PushStates();
	DGLE_RESULT DGLE_API PopStates();
	DGLE_RESULT DGLE_API SetMatrix(const TMatrix &stMat, E_MATRIX_TYPE eMatType);
	DGLE_RESULT DGLE_API GetMatrix(TMatrix &stMat, E_MATRIX_TYPE eMatType);
	DGLE_RESULT DGLE_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	DGLE_RESULT DGLE_API DrawBuffer(ICoreGeometryBuffer *pBuffer);
	DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API ToggleBlendState(bool bEnabled);
	DGLE_RESULT DGLE_API ToggleAlphaTestState(bool bEnabled);
	DGLE_RESULT DGLE_API SetBlendState(const TBlendStateDesc &stState);
	DGLE_RESULT DGLE_API GetBlendState(TBlendStateDesc &stState);
	DGLE_RESULT DGLE_API SetDepthStencilState(const TDepthStencilDesc &stState);
	DGLE_RESULT DGLE_API GetDepthStencilState(TDepthStencilDesc &stState);
	DGLE_RESULT DGLE_API SetRasterizerState(const TRasterizerStateDesc &stState);
	DGLE_RESULT DGLE_API GetRasterizerState(TRasterizerStateDesc &stState);
	DGLE_RESULT DGLE_API BindTexture(ICoreTexture *pTex, uint uiTextureLayer);
	DGLE_RESULT DGLE_API GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue);
	DGLE_RESULT DGLE_API IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported);
	DGLE_RESULT DGLE_API GetRendererType(E_CORE_RENDERER_TYPE &eType);
	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE_BASE_IMPLEMENTATION(ICoreRenderer)
};

#endif