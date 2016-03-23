/**
\author		Andrey Korotkov aka DRON
\date		23.03.2016 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#if defined(PLATFORM_WINDOWS)
#	include "platform\windows\BaseRendererGL.h"
#endif

#include "FixedFunctionPipelineGL.h"
#include "StateManagerGL.h"

class CCoreGeometryBuffer;

class CCoreRendererGL: private CBaseRendererGL, public ICoreRenderer
{
	static const uint _sc_uiMaxVerticesCountForLegacy = 128;
	static const uint _sc_uiMaxDrawCallsPerFrame = 3000;
	static const uint _sc_uiMaxFBOIdleTime = 10; // in seconds
	static const uint _sc_uiFBOMaxCacheSize = 16;

	struct TFrameBuffer
	{
		bool bValid, isDepthStencil;
		GLuint uiFBObject, uiFBBlitObject,
			uiRBObjectColor, uiRBObjectDepthStencil;
		uint uiWidth, uiHeight, uiIdleTime;
	};

	TCrRndrInitResults _stInitResults;

	CFixedFunctionPipeline *_pFFP;

	CStateManager<false> _clPassThroughStateMan;
	CStateManager<true>	*_pCachedStateMan;
	IStateManager *_pStateMan;

	std::string _strOpenGLExtensions;
	int _iMaxTexResolution, _iMaxAnisotropy, _iMaxTexUnits,
		_iMaxFBOSamples;
	bool _bIsGLSLSupported;

	int _iProfilerState;

	TDrawDataDesc _stDrawDataDesc;
	E_CORE_RENDERER_DRAW_MODE _eDrawMode;
	uint _uiDrawCount;

	uint uiPrevVpX, uiPrevVpY, uiPrevVpWidth, uiPrevVpHeight; 
	std::vector<TFrameBuffer> _vecFrameBuffers;
	ICoreTexture *_pCurRenderTarget;
	uint _uiCurFrameBufferIdx;

	CCoreGeometryBuffer *_pCurBindedBuffer, *_pLastDrawnBuffer;
	bool _bVerticesBufferBindedFlag, _bIndexesBufferBindedFlag;

	bool _bStateFilterEnabled;
	uint _uiUnfilteredDrawSetups, _uiOverallDrawSetups,
		_uiOverallDrawCalls, _uiOverallTrianglesCount;

	inline GLenum _GetGLBlendFactor(E_BLEND_FACTOR factor) const;
	inline E_BLEND_FACTOR _GetEngBlendFactor(GLenum factor) const;
	
	inline GLenum _GetGLComparsionMode(E_COMPARISON_FUNC mode) const;
	inline E_COMPARISON_FUNC _GetEngComparsionMode(GLenum mode) const;
	
	inline GLenum _GetGLDrawMode(E_CORE_RENDERER_DRAW_MODE eMode) const;
	FORCE_INLINE bool _LegacyDraw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	inline void _TriangleStatistics(E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	
	void _ProfilerEventHandler() const;
	void _KillDeadFBOs();

	static bool DGLE_API _s_ConPrintGLExts(void *pParameter, const char *pcParam);
	static void DGLE_API _s_EventsHandler(void *pParameter, IBaseEvent *pEvent);

public:
	
	CCoreRendererGL(uint uiInstIdx);

	inline IStateManager* pStateMan() const {return _pStateMan;} /**< \warning Never copy IStateManager* because _pStateMan may be reallocated at any time! */
	static inline uint GetVertexSize(const TDrawDataDesc &stDesc);
	static inline bool GetTextureGLFormat(const E_TEXTURE_DATA_FORMAT eFormat, GLenum &uiFormat, GLenum &uiInternalFormat, uint &uiBytesPerPixel);

	DGLE_RESULT DGLE_API Prepare(TCrRndrInitResults &stResults) override;
	DGLE_RESULT DGLE_API Initialize(TCrRndrInitResults &stResults, TEngineWindow &stWin, E_ENGINE_INIT_FLAGS &eInitFlags) override;
	DGLE_RESULT DGLE_API Finalize() override;
	DGLE_RESULT DGLE_API AdjustMode(TEngineWindow &stNewWin) override;
	DGLE_RESULT DGLE_API MakeCurrent() override;
	DGLE_RESULT DGLE_API Present() override;

	DGLE_RESULT DGLE_API SetClearColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetClearColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API Clear(bool bColor, bool bDepth, bool bStencil) override;
	DGLE_RESULT DGLE_API SetViewport(uint x, uint y, uint width, uint height) override;
	DGLE_RESULT DGLE_API GetViewport(uint &x, uint &y, uint &width, uint &height) override;
	DGLE_RESULT DGLE_API SetScissorRectangle(uint x, uint y, uint width, uint height) override;
	DGLE_RESULT DGLE_API GetScissorRectangle(uint &x, uint &y, uint &width, uint &height) override;
	DGLE_RESULT DGLE_API SetLineWidth(float fWidth) override;
	DGLE_RESULT DGLE_API GetLineWidth(float &fWidth) override;
	DGLE_RESULT DGLE_API SetPointSize(float fSize) override;
	DGLE_RESULT DGLE_API GetPointSize(float &fSize) override;
	DGLE_RESULT DGLE_API ReadFrameBuffer(uint uiX, uint uiY, uint uiWidth, uint uiHeight, uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat) override;
	DGLE_RESULT DGLE_API SetRenderTarget(ICoreTexture *pTexture) override;
	DGLE_RESULT DGLE_API GetRenderTarget(ICoreTexture *&prTexture) override;
	DGLE_RESULT DGLE_API CreateTexture(ICoreTexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags) override;
	DGLE_RESULT DGLE_API CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType) override;
	DGLE_RESULT DGLE_API ToggleStateFilter(bool bEnabled) override;
	DGLE_RESULT DGLE_API InvalidateStateFilter() override;
	DGLE_RESULT DGLE_API PushStates() override;
	DGLE_RESULT DGLE_API PopStates() override;
	DGLE_RESULT DGLE_API SetMatrix(const TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType) override;
	DGLE_RESULT DGLE_API GetMatrix(TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType) override;
	DGLE_RESULT DGLE_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount) override;
	DGLE_RESULT DGLE_API DrawBuffer(ICoreGeometryBuffer *pBuffer) override;
	DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor) override;
	DGLE_RESULT DGLE_API GetColor(TColor4 &stColor) override;
	DGLE_RESULT DGLE_API ToggleBlendState(bool bEnabled) override;
	DGLE_RESULT DGLE_API ToggleAlphaTestState(bool bEnabled) override;
	DGLE_RESULT DGLE_API SetBlendState(const TBlendStateDesc &stState) override;
	DGLE_RESULT DGLE_API GetBlendState(TBlendStateDesc &stState) override;
	DGLE_RESULT DGLE_API SetDepthStencilState(const TDepthStencilDesc &stState) override;
	DGLE_RESULT DGLE_API GetDepthStencilState(TDepthStencilDesc &stState) override;
	DGLE_RESULT DGLE_API SetRasterizerState(const TRasterizerStateDesc &stState) override;
	DGLE_RESULT DGLE_API GetRasterizerState(TRasterizerStateDesc &stState) override;
	DGLE_RESULT DGLE_API BindTexture(ICoreTexture *pTex, uint uiTextureLayer) override;
	DGLE_RESULT DGLE_API GetBindedTexture(ICoreTexture *&prTex, uint uiTextureLayer) override;
	DGLE_RESULT DGLE_API GetFixedFunctionPipelineAPI(IFixedFunctionPipeline *&prFFP) override;
	DGLE_RESULT DGLE_API GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue) override;
	DGLE_RESULT DGLE_API IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported) override;
	DGLE_RESULT DGLE_API GetRendererType(E_CORE_RENDERER_TYPE &eType) override;
	
	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType) override;

	IDGLE_BASE_IMPLEMENTATION(ICoreRenderer, INTERFACE_IMPL(IEngineSubSystem, INTERFACE_IMPL_END))
};

#endif