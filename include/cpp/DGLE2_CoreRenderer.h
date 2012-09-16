/**
\file		DGLE2_CoreRenderer.h
\author		Korotkov Andrey aka DRON
\version	0.3.0 beta
\date		xx.xx.2012 (c)Korotkov Andrey

\brief		This header provides interface of low-level DGLE2 rendering API.

Using of this header is recommended only for experienced users familiar with some GAPI and rendering pipeline.
This header is a part of DGLE2 SDK.

\note		Include this header after "DGLE2.h".
*/

#ifndef DGLE2_CRENDERER
#define DGLE2_CRENDERER

#include "DGLE2.h"

namespace DGLE2
{
	enum E_CORE_RENDERER_TYPE
	{
		CRT_UNKNOWN = 0,
		CRT_OPENGL_LEGACY,
		CRT_OPENGL_4_1,
		CRT_OPENGL_ES_1_1,
		CRT_OPENGL_ES_2_0,
		CRT_DIRECT_3D_9_0c,
		CRT_DIRECT_3D_11
	};

	enum E_CORE_RENDERER_FEATURE_TYPE
	{
		CRSF_BUILTIN_FSCREEN_MODE = 0,
		CRSF_BUILTIN_STATE_FILTER,
		CRSF_MULTISAMPLING,
		CRDF_VSYNC,
		CRDF_PROGRAMMABLE_PIPELINE,
		CRSF_BGRA_DATA_FORMAT,
		CRSF_TEXTURE_COMPRESSION,
		CRSF_NON_POWER_OF_TWO_TEXTURES,
		CRSF_DEPTH_TEXTURES,
		CRSF_TEXTURE_ANISOTROPY,
		CRSF_TEXTURE_MIPMAP_GENERATION,
		CRDF_TEXTURE_MIRRORED_REPEAT,
		CRDF_TEXTURE_MIRROR_CLAMP,
		CRDF_GEOMETRY_BUFFER
	};
	
	enum E_MATRIX_TYPE
	{
		MT_PROJECTION = 0,
		MT_MODELVIEW,
		MT_TEXTURE
	};

	enum E_TEXTURE_TYPE
	{
		TT_2D = 0,
		TT_3D
	};

	enum E_CORE_RENDERER_METRIC_TYPE
	{
		CRMT_MAX_TEXTURE_RESOLUTION = 0,
		CRMT_MAX_ANISOTROPY_LEVEL,
		CRMT_MAX_LIGHTS_PER_PASS,
		CRMT_MAX_TEXTURE_LAYERS
	};

	enum E_COMPARISON_FUNC
	{
		CF_NEVER = 0,
		CF_LESS,
		CF_EQUAL,
		CF_LESS_EQUAL,
		CF_GREATER,
		CF_NOT_EQUAL,
		CF_GREATER_EQUAL,
		CF_ALWAYS
	};

	enum E_POLYGON_CULL_MODE
	{
		PCM_NONE = 0,
		PCM_FRONT,
		PCM_BACK
	};

	/* For future needs.
	enum E_STENCIL_OPERATION
	{
		SO_KEEP = 0,
		SO_ZERO,
		SO_REPLACE,
		SO_INVERT,
		SO_INCR,
		SO_DECR
	};

	enum E_BLEND_OPERATION
	{
		BO_ADD = 0,
		BO_SUBTRACT,
		BO_REV_SUBTRACT,
		BO_MIN,
		BO_MAX
	};
	*/

	enum E_BLEND_FACTOR
	{
		BF_ZERO = 0,
		BF_ONE,
		BF_SRC_COLOR,
		BF_SRC_ALPHA,
		BF_DST_COLOR,
		BF_DST_ALPHA,
		BF_ONE_MINUS_SRC_COLOR,
		BF_ONE_MINUS_SRC_ALPHA

		/* For future needs.
		BF_ONE_MINUS_DST_COLOR,
		BF_ONE_MINUS_DST_ALPHA,
		BF_SRC_ALPHA_SATURATE?,
		BF_SRC1_COLOR,
		BF_ONE_MINUS_SRC1_COLOR,
		BF_SRC1_ALPHA,
		BF_ONE_MINUS_SRC1_ALPHA
		*/
	};

	enum E_CORE_RENDERER_DATA_ALIGNMENT
	{
		CRDA_ALIGNED_BY_4 = 0,
		CRDA_ALIGNED_BY_1
	};

	enum E_CORE_RENDERER_BUFFER_TYPE
	{
		CRBT_SOFTWARE = 0,
		CRBT_HARDWARE_STATIC,
		CRBT_HARDWARE_DYNAMIC
	};

	enum E_CORE_RENDERER_DRAW_MODE
	{
		CRDM_POINTS = 0,
		CRDM_LINES,
		CRDM_TRIANGLES,
		CRDM_LINE_STRIP,
		CRDM_TRIANGLE_STRIP,
		CRDM_TRIANGLE_FAN
	};

#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack( push, 1 )
#endif

	struct TBlendStateDesc
	{
		bool				bEnable;

		E_BLEND_FACTOR		eSrcFactor;
		E_BLEND_FACTOR		eDestFactor;

		/* For future needs.
		E_BLEND_OPERATION	eOperation;

		bool bSeparate;
		E_BLEND_FACTOR		eSrcAlpha;
		E_BLEND_FACTOR		eDestAlpha;
		E_BLEND_OPERATION	eOpAlpha;
		*/
		TBlendStateDesc():
		bEnable(false), eSrcFactor(BF_SRC_ALPHA), eDestFactor(BF_ONE_MINUS_SRC_ALPHA)
		{}
	};

	/* For future needs.
	struct TStencilFaceDesc
	{
		E_STENCIL_OPERATION	eStencilFailOp;
		E_STENCIL_OPERATION	eStencilDepthFailOp;
		E_STENCIL_OPERATION	eStencilPassOp;
		E_COMPARISON_FUNC	eStencilFunc;
	};
	*/

	struct TDepthStencilDesc
	{
		bool	bDepthTestEnable;
		bool	bWriteToDepthBuffer;
		E_COMPARISON_FUNC
				eDepthFunc;
		
		/* For future needs.
		bool	bStencilEnable;
		uint8	ui8StencilReadMask;
		uint8	ui8StencilWriteMask;
		TStencilFaceDesc stFrontFace, stBackFace;
		*/

		TDepthStencilDesc():
		bDepthTestEnable(true), bWriteToDepthBuffer(true), eDepthFunc(CF_LESS_EQUAL)
		{}
	};

	struct TRasterizerStateDesc
	{
		bool	bWireframe;

		E_POLYGON_CULL_MODE
				eCullMode;
		bool	bFrontCounterClockwise;

		bool	bScissorEnable;

		bool	bAlphaTestEnable;
		E_COMPARISON_FUNC
				eAlphaTestFunc;
		float	fAlphaTestRefValue;

		/* For future needs.
		int		iDepthBias;
		float	fDepthBiasClamp;
		float	fSlopeScaledDepthBias;
		bool	bDepthClipEnable;
		*/

		TRasterizerStateDesc():
		bWireframe(false), eCullMode(PCM_NONE), bFrontCounterClockwise(true), bScissorEnable(false),
		bAlphaTestEnable(false), eAlphaTestFunc(CF_GREATER), fAlphaTestRefValue(0.25f)
		{}
	};

	struct TDrawDataDesc
	{
		uint8 *pData; //Must be start of the vertex data. 2 or 3 floats
		
		uint uiVertexStride;
		bool bVertexCoord2;

		uint uiNormalOffset; //3 floats
		uint uiNormalStride;

		uint uiTexCoordOffset; //2 floats
		uint uiTexCoordStride;

		uint uiColorOffset; //4 floats
		uint uiColorStride;

		uint8 *pIndexBuffer; //May point to separate memory. uint16 or uint32 data pointer.
		bool bIndexBuffer32;

		//ToDo: Add VertexAttribPointers.

		inline TDrawDataDesc():
		pData(NULL), uiVertexStride(0), bVertexCoord2(false),
		uiNormalOffset(-1), uiNormalStride(0),
		uiTexCoordOffset(-1), uiTexCoordStride(0),
		uiColorOffset(-1), uiColorStride(0),
		pIndexBuffer(NULL), bIndexBuffer32(false)
		{}

		inline TDrawDataDesc(uint8 *pDataPointer, uint uiTexCoordDataOffset = -1, bool bTwoCoordPerVertex = true):
		pData(pDataPointer), uiVertexStride(0), bVertexCoord2(bTwoCoordPerVertex),
		uiNormalOffset(-1), uiNormalStride(0),
		uiTexCoordOffset(uiTexCoordDataOffset), uiTexCoordStride(0),
		uiColorOffset(-1), uiColorStride(0),
		pIndexBuffer(NULL), bIndexBuffer32(false)
		{}
	};

#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack(pop)
#endif

	// {5C5C5973-D826-42ED-B641-A84DDDAAE2A3}
	static const GUID IID_IBaseRenderObjectContainer = 
	{ 0x5c5c5973, 0xd826, 0x42ed, { 0xb6, 0x41, 0xa8, 0x4d, 0xdd, 0xaa, 0xe2, 0xa3 } };

	class IBaseRenderObjectContainer : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API GetObjectType(E_ENG_OBJ_TYPE &eType) = 0;
	};

#if defined(OPENGL_LEGACY_BASE_OBJECTS)

#ifndef USE_GLEW_HEADER
#	include <gl/GL.h>
#else
#	include <gl/glew.h>
#endif

	// {7264D8D2-C3AF-4ED3-91D1-90E02BE6A4EE}
	static const GUID IID_IOpenGLTextureContainer = 
	{ 0x7264d8d2, 0xc3af, 0x4ed3, { 0x91, 0xd1, 0x90, 0xe0, 0x2b, 0xe6, 0xa4, 0xee } };

	class IOpenGLTextureContainer : public IBaseRenderObjectContainer
	{
	public:
		virtual HRESULT DGLE2_API GetTexture(GLuint &texture) = 0;
	};

	// {152B744F-7C1B-414F-BEC1-CD40A308E5DF}
	static const GUID IID_IOpenGLBufferContainer = 
	{ 0x152b744f, 0x7c1b, 0x414f, { 0xbe, 0xc1, 0xcd, 0x40, 0xa3, 0x8, 0xe5, 0xdf } };

	class IOpenGLBufferContainer : public IBaseRenderObjectContainer
	{
	public:
		virtual HRESULT DGLE2_API GetVertexBufferObject(GLuint &vbo) = 0;
		virtual HRESULT DGLE2_API GetIndexBufferObject(GLuint &vbo) = 0;
	};

#endif

	// {8BFF07F9-2A8E-41D0-8505-3128C1B8160A}
	static const GUID IID_ICoreTexture = 
	{ 0x8bff07f9, 0x2a8e, 0x41d0, { 0x85, 0x5, 0x31, 0x28, 0xc1, 0xb8, 0x16, 0xa } };

	class ICoreTexture : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API GetSize(uint &width, uint &height) = 0;
		virtual HRESULT DGLE2_API GetDepth(uint &depth) = 0;
		virtual HRESULT DGLE2_API GetType(E_TEXTURE_TYPE &eType) = 0;
		virtual HRESULT DGLE2_API GetFormat(E_TEXTURE_DATA_FORMAT &eFormat) = 0;
		virtual HRESULT DGLE2_API GetLoadFlags(E_TEXTURE_LOAD_FLAGS &eLoadFlags) = 0;
		virtual HRESULT DGLE2_API GetPixelData(uint8 *pData, uint &uiDataSize, uint uiLodLevel = 0) = 0;
		virtual HRESULT DGLE2_API SetPixelData(const uint8 *pData, uint uiDataSize, uint uiLodLevel = 0) = 0;
		virtual HRESULT DGLE2_API Reallocate(const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat) = 0;
		virtual HRESULT DGLE2_API GetBaseObject(IBaseRenderObjectContainer *&prObj) = 0;
		virtual HRESULT DGLE2_API Free() = 0;
	};

	// {9A77DCFF-9E4B-4716-9BBB-A316BF217F7A}
	static const GUID IID_ICoreGeometryBuffer = 
	{ 0x9a77dcff, 0x9e4b, 0x4716, { 0x9b, 0xbb, 0xa3, 0x16, 0xbf, 0x21, 0x7f, 0x7a } };

	class ICoreGeometryBuffer : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API GetGeometryData(TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize) = 0;
		virtual HRESULT DGLE2_API SetGeometryData(const TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize) = 0;
		virtual HRESULT DGLE2_API Reallocate(const TDrawDataDesc &stDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode) = 0;
		virtual HRESULT DGLE2_API GetBufferDemensions(uint &uiVerticesDataSize, uint &uiVerticesCount, uint &uiIndexesDataSize, uint &uiIndexesCount) = 0;
		virtual HRESULT DGLE2_API GetBufferDrawDataDesc(TDrawDataDesc &stDesc) = 0;
		virtual HRESULT DGLE2_API GetBufferDrawMode(E_CORE_RENDERER_DRAW_MODE &eMode) = 0;
		virtual HRESULT DGLE2_API GetBufferType(E_CORE_RENDERER_BUFFER_TYPE &eType) = 0;
		virtual HRESULT DGLE2_API GetBaseObject(IBaseRenderObjectContainer *&prObj) = 0;
		virtual HRESULT DGLE2_API Free() = 0;
	};

	// {C3B687A1-57B0-4E21-BE4C-4D92F3FAB311}
	static const GUID IID_ICoreRenderer = 
	{ 0xc3b687a1, 0x57b0, 0x4e21, { 0xbe, 0x4c, 0x4d, 0x92, 0xf3, 0xfa, 0xb3, 0x11 } };

	class ICoreRenderer : public IEngineSubSystem
	{
	public:
		//Must not be called by user
		virtual HRESULT DGLE2_API Prepare(TCRendererInitResult &stResults) = 0;
		virtual HRESULT DGLE2_API Initialize(TCRendererInitResult &stResults) = 0;
		virtual HRESULT DGLE2_API Finalize() = 0;
		virtual HRESULT DGLE2_API AdjustMode(TEngWindow &stNewWin) = 0;
		//
		
		virtual HRESULT DGLE2_API MakeCurrent() = 0;
		virtual HRESULT DGLE2_API Present() = 0;
		virtual HRESULT DGLE2_API SetClearColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API Clear(bool bColor = true, bool bDepth = true, bool bStencil = true) = 0;
		virtual HRESULT DGLE2_API SetViewport(uint x, uint y, uint width, uint height) = 0;
		virtual HRESULT DGLE2_API GetViewport(uint &x, uint &y, uint &width, uint &height) = 0;
		virtual HRESULT DGLE2_API SetScissor(uint x, uint y, uint width, uint height) = 0;
		virtual HRESULT DGLE2_API SetLineWidth(float fWidth) = 0;
		virtual HRESULT DGLE2_API SetPointSize(float fSize) = 0;
		virtual HRESULT DGLE2_API ReadFrameBuffer(uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat) = 0;
		virtual HRESULT DGLE2_API SetRenderTarget(ICoreTexture *pTexture) = 0;
		virtual HRESULT DGLE2_API CreateTexture(ICoreTexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags) = 0;
		virtual HRESULT DGLE2_API CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType) = 0;
		virtual HRESULT DGLE2_API ToggleStateFilter(bool bEnabled) = 0;
		virtual HRESULT DGLE2_API PushStates() = 0;
		virtual HRESULT DGLE2_API PopStates() = 0;
		virtual HRESULT DGLE2_API SetMatrix(const TMatrix &stMat, E_MATRIX_TYPE eMatType = MT_MODELVIEW) = 0;
		virtual HRESULT DGLE2_API GetMatrix(TMatrix &stMat, E_MATRIX_TYPE eMatType = MT_MODELVIEW) = 0;
		virtual HRESULT DGLE2_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount) = 0;
		virtual HRESULT DGLE2_API DrawBuffer(ICoreGeometryBuffer *pBuffer) = 0;
		virtual HRESULT DGLE2_API SetColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API ToggleBlendState(bool bEnabled) = 0;
		virtual HRESULT DGLE2_API ToggleAlphaTestState(bool bEnabled) = 0;
		virtual HRESULT DGLE2_API SetBlendState(const TBlendStateDesc &stState) = 0;
		virtual HRESULT DGLE2_API GetBlendState(TBlendStateDesc &stState) = 0;
		virtual HRESULT DGLE2_API SetDepthStencilState(const TDepthStencilDesc &stState) = 0;
		virtual HRESULT DGLE2_API GetDepthStencilState(TDepthStencilDesc &stState) = 0;
		virtual HRESULT DGLE2_API SetRasterizerState(const TRasterizerStateDesc &stState) = 0;
		virtual HRESULT DGLE2_API GetRasterizerState(TRasterizerStateDesc &stState) = 0;
		virtual HRESULT DGLE2_API BindTexture(ICoreTexture *pTex, uint uiTextureLayer = 0) = 0;
		virtual HRESULT DGLE2_API GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue) = 0;
		virtual HRESULT DGLE2_API IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported) = 0;
		virtual HRESULT DGLE2_API GetRendererType(E_CORE_RENDERER_TYPE &eType) = 0;
	};

}

#endif //DGLE2_CRENDERER