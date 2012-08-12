/**
\author		Andrey Korotkov aka DRON
\date		29.04.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _CORERENDERERGL_H
#define _CORERENDERERGL_H

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#include "platform\win\BaseRendererGL.h"
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

	TState(){}

	TState(uint uiMaxTexLayers): clActivatedTexUnits(uiMaxTexLayers),
	iViewPortX(0), iViewPortY(0), iViewPortWidth(0), iViewPortHeight(0)
	{
		stModelviewMat = MatrixIdentity();
		stProjectionMat = MatrixIdentity();
		stTextureMat = MatrixIdentity();
	}
};

class CCoreRendererGL: public CBaseRendererGL, public ICoreRenderer
{
	static const uint _sc_uiMaxVerticesCountForLegact = 128;

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

	bool _bVerticesBufferBindedFlag, _bIndexesBufferBindedFlag;

	bool _bStateFilterEnabled;
	uint _uiUnfilteredDrawSetups, _uiOverallDrawSetups, _uiOverallDrawCalls, _uiOverallTrianglesCount;

	inline GLenum _GetGLBlendFactor(E_BLEND_FACTOR factor);
	inline GLenum _GetGLComparsionMode(E_COMPARISON_FUNC mode);
	inline GLenum _GetGLDrawMode(E_CORE_RENDERER_DRAW_MODE eMode);
	__forceinline bool _LegacyDraw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	inline void _TriangleStatistics(E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	void _ProfilerEventHandler(IBaseEvent *pEvent) const;

	static void CALLBACK _s_ConPrintGLExts(void *pParametr, const char *pcParam);
	static void CALLBACK _s_ProfilerEventHandler(void *pParametr, IBaseEvent *pEvent);

public:
	
	CCoreRendererGL(uint uiInstIdx);

	inline IStateManager* pStateMan() const {return _pStateMan;} /**< \warning Never copy IStateManager* because _pStateMan may be reallocated at any time! */

	HRESULT CALLBACK Prepare(TCRendererInitResult &stResults);
	HRESULT CALLBACK Initialize(TCRendererInitResult &stResults);
	HRESULT CALLBACK Finalize();
	HRESULT CALLBACK AdjustMode(TEngWindow &stNewWin);
	HRESULT CALLBACK MakeCurrent();
	HRESULT CALLBACK Present();

	HRESULT CALLBACK SetClearColor(const TColor4 &stColor);
	HRESULT CALLBACK Clear(bool bColor, bool bDepth, bool bStencil);
	HRESULT CALLBACK SetViewport(uint x, uint y, uint width, uint height);
	HRESULT CALLBACK GetViewport(uint &x, uint &y, uint &width, uint &height);
	HRESULT CALLBACK SetScissor(uint x, uint y, uint width, uint height);
	HRESULT CALLBACK SetLineWidth(float fWidth);
	HRESULT CALLBACK SetPointSize(float fSize);
	HRESULT CALLBACK ReadFrameBuffer(uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat);
	HRESULT CALLBACK SetRenderTarget(ICoreTexture *pTexture);
	HRESULT CALLBACK CreateTexture(ICoreTexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags);
	HRESULT CALLBACK CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType);
	HRESULT CALLBACK ToggleStateFilter(bool bEnabled);
	HRESULT CALLBACK PushStates();
	HRESULT CALLBACK PopStates();
	HRESULT CALLBACK SetMatrix(const TMatrix &stMat, E_MATRIX_TYPE eMatType);
	HRESULT CALLBACK GetMatrix(TMatrix &stMat, E_MATRIX_TYPE eMatType);
	HRESULT CALLBACK Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
	HRESULT CALLBACK DrawBuffer(ICoreGeometryBuffer *pBuffer);
	HRESULT CALLBACK SetColor(const TColor4 &stColor);
	HRESULT CALLBACK ToggleBlendState(bool bEnabled);
	HRESULT CALLBACK ToggleAlphaTestState(bool bEnabled);
	HRESULT CALLBACK SetBlendState(const TBlendStateDesc &stState);
	HRESULT CALLBACK GetBlendState(TBlendStateDesc &stState);
	HRESULT CALLBACK SetDepthStencilState(const TDepthStencilDesc &stState);
	HRESULT CALLBACK GetDepthStencilState(TDepthStencilDesc &stState);
	HRESULT CALLBACK SetRasterizerState(const TRasterizerStateDesc &stState);
	HRESULT CALLBACK GetRasterizerState(TRasterizerStateDesc &stState);
	HRESULT CALLBACK BindTexture(ICoreTexture *pTex, uint uiTextureLayer);
	HRESULT CALLBACK GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue);
	HRESULT CALLBACK IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported);
	HRESULT CALLBACK GetRendererType(E_CORE_RENDERER_TYPE &eType);
	HRESULT CALLBACK GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE2_BASE_IMPLEMENTATION(ICoreRenderer)
};

#endif

#endif //_CORERENDERERGL_H