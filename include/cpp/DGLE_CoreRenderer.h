/**
\file		DGLE_CoreRenderer.h
\author		Korotkov Andrey aka DRON
\version	2:0.3.5
\date		XX.XX.XXXX (c)Korotkov Andrey

\brief		This header provides interface of low-level DGLE rendering API.

Using of this header is recommended only for experienced users familiar with some GAPI and rendering pipeline.
This header is a part of DGLE SDK.

\note		Include this header after "DGLE.h".
*/

#ifndef DGLE_CRENDERER
#define DGLE_CRENDERER

#ifndef DGLE_HEADER
#error You must include "DGLE.h" first.
#endif

namespace DGLE
{
	enum E_CORE_RENDERER_TYPE
	{
		CRT_UNKNOWN = 0,
		CRT_OPENGL_LEGACY
		/*  For future needs.
		CRT_OPENGL_4_1,
		CRT_OPENGL_ES_1_1,
		CRT_OPENGL_ES_2_0*/,
		CRT_DIRECT_3D_9_0c,
		CRT_DIRECT_3D_11
	};

	enum E_CORE_RENDERER_FEATURE_TYPE
	{
		CRFT_BUILTIN_FULLSCREEN_MODE = 0,
		CRFT_BUILTIN_STATE_FILTER,
		CRFT_MULTISAMPLING,
		CRFT_VSYNC,
		CRFT_PROGRAMMABLE_PIPELINE,
		CRFT_LEGACY_FIXED_FUNCTION_PIPELINE_API,
		CRFT_BGRA_DATA_FORMAT,
		CRFT_TEXTURE_COMPRESSION,
		CRFT_NON_POWER_OF_TWO_TEXTURES,
		CRFT_DEPTH_TEXTURES,
		CRFT_TEXTURE_ANISOTROPY,
		CRFT_TEXTURE_MIPMAP_GENERATION,
		CRFT_TEXTURE_MIRRORED_REPEAT,
		CRFT_TEXTURE_MIRROR_CLAMP,
		CRFT_GEOMETRY_BUFFER,
		CRFT_FRAME_BUFFER
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
		CRMT_MAX_TEXTURE_LAYERS,
		CRMT_MAX_ANISOTROPY_LEVEL
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

	enum ENUM_FORWARD_DECLARATION(E_CORE_RENDERER_DRAW_MODE)
	{
		CRDM_POINTS = 0,
			CRDM_LINES,
			CRDM_TRIANGLES,
			CRDM_LINE_STRIP,
			CRDM_TRIANGLE_STRIP,
			CRDM_TRIANGLE_FAN
	};

	enum E_ATTRIBUTE_DATA_TYPE
	{
		ADT_FLOAT = 0,
		ADT_BYTE,
		ADT_UBYTE,
		ADT_SHORT,
		ADT_USHORT,
		ADT_INT,
		ADT_UINT
	};

	enum E_ATTRIBUTE_COMPONENTS_COUNT
	{
		ACC_ONE = 0,
		ACC_TWO,
		ACC_THREE,
		ACC_FOUR
	};

#ifdef STRUCT_ALIGNMENT_1
#pragma pack( push, 1 )
#endif

	struct TBlendStateDesc
	{
		bool bEnabled;

		E_BLEND_FACTOR eSrcFactor;
		E_BLEND_FACTOR eDstFactor;

		/* For future needs.
		E_BLEND_OPERATION eOperation;

		bool bSeparate;
		E_BLEND_FACTOR eSrcAlpha;
		E_BLEND_FACTOR eDstAlpha;
		E_BLEND_OPERATION eOpAlpha;
		*/
		TBlendStateDesc() :
			bEnabled(false), eSrcFactor(BF_SRC_ALPHA), eDstFactor(BF_ONE_MINUS_SRC_ALPHA)
		{}
	};

	/* For future needs.
	struct TStencilFaceDesc
	{
	E_STENCIL_OPERATION eStencilFailOp;
	E_STENCIL_OPERATION eStencilDepthFailOp;
	E_STENCIL_OPERATION eStencilPassOp;
	E_COMPARISON_FUNC eStencilFunc;
	};
	*/

	struct TDepthStencilDesc
	{
		bool bDepthTestEnabled;
		bool bWriteToDepthBuffer;
		E_COMPARISON_FUNC eDepthFunc;

		/* For future needs.
		bool bStencilEnabled;
		uint8 ui8StencilReadMask;
		uint8 ui8StencilWriteMask;
		TStencilFaceDesc stFrontFace, stBackFace;
		*/

		TDepthStencilDesc() :
			bDepthTestEnabled(true), bWriteToDepthBuffer(true), eDepthFunc(CF_LESS_EQUAL)
		{}
	};

	struct TRasterizerStateDesc
	{
		bool bWireframe;

		E_POLYGON_CULL_MODE eCullMode;
		bool bFrontCounterClockwise;

		bool bScissorEnabled;

		bool bAlphaTestEnabled;
		E_COMPARISON_FUNC eAlphaTestFunc;
		float fAlphaTestRefValue;

		/* For future needs.
		int iDepthBias;
		float fDepthBiasClamp;
		float fSlopeScaledDepthBias;
		bool bDepthClipEnabled;
		*/

		TRasterizerStateDesc() :
			bWireframe(false), eCullMode(PCM_NONE), bFrontCounterClockwise(true), bScissorEnabled(false),
			bAlphaTestEnabled(false), eAlphaTestFunc(CF_GREATER), fAlphaTestRefValue(0.25f)
		{}
	};

	struct TDrawDataAttributes
	{
		uint uiAttribOffset[8];
		uint uiAttribStride[8];
		E_ATTRIBUTE_DATA_TYPE eAttribDataType[8];
		E_ATTRIBUTE_COMPONENTS_COUNT eAttribCompsCount[8];

		TDrawDataAttributes()
			//:uiAttribStride(), eAttribDataType(), eAttribCompsCount()
		{
			uiAttribOffset[0] = uiAttribOffset[1] = uiAttribOffset[2] = uiAttribOffset[3] =
				uiAttribOffset[4] = uiAttribOffset[5] = uiAttribOffset[6] = uiAttribOffset[7] = -1;
		}
	};

	struct TDrawDataDesc
	{
		uint8 *pData; //Must be start of the vertex data. 2 or 3 floats

		uint uiVertexStride;
		bool bVertices2D;

		uint uiNormalOffset; //3 floats
		uint uiNormalStride;

		uint uiTextureVertexOffset; //2 floats
		uint uiTextureVertexStride;

		uint uiColorOffset; //4 floats
		uint uiColorStride;

		/*not implemeted*/ uint uiTangentOffset, uiBinormalOffset; //6 floats, 3 for tangent and 3 for binormal
		/*not implemeted*/ uint uiTangentStride, uiBinormalStride;

		/*not implemeted*/ TDrawDataAttributes *pAttribs;

		uint8 *pIndexBuffer; //May point to separate memory. uint16 or uint32 data pointer.
		bool bIndexBuffer32;

		inline TDrawDataDesc() :
			pData(NULL), uiVertexStride(0), bVertices2D(false),
			uiNormalOffset(-1), uiNormalStride(0),
			uiTextureVertexOffset(-1), uiTextureVertexStride(0),
			uiColorOffset(-1), uiColorStride(0),
			uiTangentOffset(-1), uiBinormalOffset(-1), uiTangentStride(0), uiBinormalStride(0),
			pIndexBuffer(NULL), bIndexBuffer32(false), pAttribs(NULL)
		{}

		inline TDrawDataDesc(uint8 *pDataPointer, uint uiNormalDataOffset, uint uiTextureVertexDataOffset, bool bIs2D) :
			pData(pDataPointer), uiVertexStride(0), bVertices2D(bIs2D),
			uiNormalOffset(uiNormalDataOffset), uiNormalStride(0),
			uiTextureVertexOffset(uiTextureVertexDataOffset), uiTextureVertexStride(0),
			uiColorOffset(-1), uiColorStride(0),
			uiTangentOffset(-1), uiBinormalOffset(-1), uiTangentStride(0), uiBinormalStride(0),
			pIndexBuffer(NULL), bIndexBuffer32(false), pAttribs(NULL)
		{}

		inline bool operator == (const TDrawDataDesc &desc) const
		{
			return pData == desc.pData && uiVertexStride == desc.uiVertexStride && bVertices2D == desc.bVertices2D &&
				uiNormalOffset == desc.uiNormalOffset && uiNormalStride == desc.uiNormalStride && uiTextureVertexOffset == desc.uiTextureVertexOffset &&
				uiTextureVertexStride == desc.uiTextureVertexStride && uiColorOffset == desc.uiColorOffset && uiColorStride == desc.uiColorStride &&
				uiTangentOffset == desc.uiTangentOffset && uiBinormalOffset == desc.uiBinormalOffset && uiTangentStride == desc.uiTangentStride &&
				uiBinormalStride == desc.uiBinormalStride && pAttribs == desc.pAttribs && pIndexBuffer == desc.pIndexBuffer && bIndexBuffer32 == desc.bIndexBuffer32;
		}

	};

#ifdef STRUCT_ALIGNMENT_1
#pragma pack(pop)
#endif

	// {5C5C5973-D826-42ED-B641-A84DDDAAE2A3}
	static const GUID IID_IBaseRenderObjectContainer =
	{ 0x5c5c5973, 0xd826, 0x42ed, { 0xb6, 0x41, 0xa8, 0x4d, 0xdd, 0xaa, 0xe2, 0xa3 } };

	class IBaseRenderObjectContainer : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API GetObjectType(E_ENGINE_OBJECT_TYPE &eType) = 0;
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
		virtual DGLE_RESULT DGLE_API GetTexture(GLuint &texture) = 0;
	};

	// {152B744F-7C1B-414F-BEC1-CD40A308E5DF}
	static const GUID IID_IOpenGLBufferContainer =
	{ 0x152b744f, 0x7c1b, 0x414f, { 0xbe, 0xc1, 0xcd, 0x40, 0xa3, 0x8, 0xe5, 0xdf } };

	class IOpenGLBufferContainer : public IBaseRenderObjectContainer
	{
	public:
		virtual DGLE_RESULT DGLE_API GetVertexBufferObject(GLuint &vbo) = 0;
		virtual DGLE_RESULT DGLE_API GetIndexBufferObject(GLuint &vbo) = 0;
	};

#endif

#ifdef DX9_LEGACY_BASE_OBJECTS
}

struct IDirect3DTexture9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DVertexDeclaration9;

namespace DGLE
{
	// {7EAB859A-ED30-43E3-81E8-FC25755CD681}
	static const GUID IID_IDX9TextureContainer =
	{ 0x7eab859a, 0xed30, 0x43e3, { 0x81, 0xe8, 0xfc, 0x25, 0x75, 0x5c, 0xd6, 0x81 } };

	class IDX9TextureContainer : public IBaseRenderObjectContainer
	{
	public:
		virtual DGLE_RESULT DGLE_API GetTexture(IDirect3DTexture9 *&texture) = 0;
	};

	// {A3814299-88BC-4EED-AF7F-5FF70BEBC4E5}
	static const GUID IID_IDX9BufferContainer =
	{ 0xa3814299, 0x88bc, 0x4eed, { 0xaf, 0x7f, 0x5f, 0xf7, 0xb, 0xeb, 0xc4, 0xe5 } };

	class IDX9BufferContainer : public IBaseRenderObjectContainer
	{
	public:
		virtual DGLE_RESULT DGLE_API GetVB(IDirect3DVertexBuffer9 *&VB) = 0;
		virtual DGLE_RESULT DGLE_API GetIB(IDirect3DIndexBuffer9 *&IB) = 0;
		virtual DGLE_RESULT DGLE_API GetVBDecl(IDirect3DVertexDeclaration9 *&VBDecl) = 0;
	};
#endif

#ifdef DX11_LEGACY_BASE_OBJECTS
}

struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11Buffer;

namespace DGLE
	// {C18527AB-2804-410B-9822-176F8FAABADE}
	static const GUID IID_IDX11TextureContainer =
	{ 0xc18527ab, 0x2804, 0x410b,{ 0x98, 0x22, 0x17, 0x6f, 0x8f, 0xaa, 0xba, 0xde } };

	class IDX11TextureContainer : public IBaseRenderObjectContainer
	{
		virtual DGLE_RESULT DGLE_API GetView(ID3D11ShaderResourceView *&texture) = 0;
		virtual DGLE_RESULT DGLE_API GetSampler(ID3D11SamplerState *&texture) = 0;
	};

	// {964F1A36-D1C8-4C86-9DDB-56BE646DE58B}
	static const GUID IID_IDX9BufferContainer =
	{ 0x964f1a36, 0xd1c8, 0x4c86,{ 0x9d, 0xdb, 0x56, 0xbe, 0x64, 0x6d, 0xe5, 0x8b } };
	
	class IDX9BufferContainer : public IBaseRenderObjectContainer
	{
		virtual DGLE_RESULT DGLE_API GetVB(ID3D11Buffer *&VB) = 0;
		virtual DGLE_RESULT DGLE_API GetIB(ID3D11Buffer *&IB) = 0;
	};
#endif

	// {8BFF07F9-2A8E-41D0-8505-3128C1B8160A}
	static const GUID IID_ICoreTexture = 
	{ 0x8bff07f9, 0x2a8e, 0x41d0, { 0x85, 0x5, 0x31, 0x28, 0xc1, 0xb8, 0x16, 0xa } };

	class ICoreTexture : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API GetSize(uint &width, uint &height) = 0;
		virtual DGLE_RESULT DGLE_API GetDepth(uint &depth) = 0;
		virtual DGLE_RESULT DGLE_API GetType(E_TEXTURE_TYPE &eType) = 0;
		virtual DGLE_RESULT DGLE_API GetFormat(E_TEXTURE_DATA_FORMAT &eFormat) = 0;
		virtual DGLE_RESULT DGLE_API GetLoadFlags(E_TEXTURE_LOAD_FLAGS &eLoadFlags) = 0;
		virtual DGLE_RESULT DGLE_API GetPixelData(uint8 *pData, uint &uiDataSize, uint uiLodLevel = 0) = 0; // Note: changes current texture
		virtual DGLE_RESULT DGLE_API SetPixelData(const uint8 *pData, uint uiDataSize, uint uiLodLevel = 0) = 0;
		virtual DGLE_RESULT DGLE_API Reallocate(const uint8 *pData, uint uiWidth, uint uiHeight, bool bMipMaps, E_TEXTURE_DATA_FORMAT eDataFormat) = 0; // Note: changes current texture
		virtual DGLE_RESULT DGLE_API GetBaseObject(IBaseRenderObjectContainer *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API Free() = 0;
	};

	// {9A77DCFF-9E4B-4716-9BBB-A316BF217F7A}
	static const GUID IID_ICoreGeometryBuffer = 
	{ 0x9a77dcff, 0x9e4b, 0x4716, { 0x9b, 0xbb, 0xa3, 0x16, 0xbf, 0x21, 0x7f, 0x7a } };

	class ICoreGeometryBuffer : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API GetGeometryData(TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize) = 0;
		virtual DGLE_RESULT DGLE_API SetGeometryData(const TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize) = 0;
		virtual DGLE_RESULT DGLE_API Reallocate(const TDrawDataDesc &stDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode) = 0;
		virtual DGLE_RESULT DGLE_API GetBufferDimensions(uint &uiVerticesDataSize, uint &uiVerticesCount, uint &uiIndexesDataSize, uint &uiIndexesCount) = 0;
		virtual DGLE_RESULT DGLE_API GetBufferDrawDataDesc(TDrawDataDesc &stDesc) = 0;
		virtual DGLE_RESULT DGLE_API GetBufferDrawMode(E_CORE_RENDERER_DRAW_MODE &eMode) = 0;
		virtual DGLE_RESULT DGLE_API GetBufferType(E_CORE_RENDERER_BUFFER_TYPE &eType) = 0;
		virtual DGLE_RESULT DGLE_API GetBaseObject(IBaseRenderObjectContainer *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API Free() = 0;
	};

	class IFixedFunctionPipeline;

	// {C3B687A1-57B0-4E21-BE4C-4D92F3FAB311}
	static const GUID IID_ICoreRenderer = 
	{ 0xc3b687a1, 0x57b0, 0x4e21, { 0xbe, 0x4c, 0x4d, 0x92, 0xf3, 0xfa, 0xb3, 0x11 } };

	class ICoreRenderer : public IEngineSubSystem
	{
	public:
		//Must not be called by user
		virtual DGLE_RESULT DGLE_API Prepare(TCrRndrInitResults &stResults) = 0;
		virtual DGLE_RESULT DGLE_API Initialize(TCrRndrInitResults &stResults, TEngineWindow &stWin, E_ENGINE_INIT_FLAGS &eInitFlags) = 0;
		virtual DGLE_RESULT DGLE_API Finalize() = 0;
		virtual DGLE_RESULT DGLE_API AdjustMode(TEngineWindow &stNewWin) = 0;
		//
		
		virtual DGLE_RESULT DGLE_API MakeCurrent() = 0;
		virtual DGLE_RESULT DGLE_API Present() = 0;
		virtual DGLE_RESULT DGLE_API SetClearColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetClearColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API Clear(bool bColor = true, bool bDepth = true, bool bStencil = true) = 0;
		virtual DGLE_RESULT DGLE_API SetViewport(uint x, uint y, uint width, uint height) = 0;
		virtual DGLE_RESULT DGLE_API GetViewport(uint &x, uint &y, uint &width, uint &height) = 0;
		virtual DGLE_RESULT DGLE_API SetScissorRectangle(uint x, uint y, uint width, uint height) = 0;
		virtual DGLE_RESULT DGLE_API GetScissorRectangle(uint &x, uint &y, uint &width, uint &height) = 0;
		virtual DGLE_RESULT DGLE_API SetLineWidth(float fWidth) = 0;
		virtual DGLE_RESULT DGLE_API GetLineWidth(float &fWidth) = 0;
		virtual DGLE_RESULT DGLE_API SetPointSize(float fSize) = 0;
		virtual DGLE_RESULT DGLE_API GetPointSize(float &fSize) = 0;
		virtual DGLE_RESULT DGLE_API ReadFrameBuffer(uint uiX, uint uiY, uint uiWidth, uint uiHeight, uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat) = 0;
		virtual DGLE_RESULT DGLE_API SetRenderTarget(ICoreTexture *pTexture) = 0; //no stencil for 32bit depth texture
		virtual DGLE_RESULT DGLE_API GetRenderTarget(ICoreTexture *&prTexture) = 0;
		virtual DGLE_RESULT DGLE_API CreateTexture(ICoreTexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags) = 0;
		virtual DGLE_RESULT DGLE_API CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType) = 0;
		virtual DGLE_RESULT DGLE_API ToggleStateFilter(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API InvalidateStateFilter() = 0;
		virtual DGLE_RESULT DGLE_API PushStates() = 0;
		virtual DGLE_RESULT DGLE_API PopStates() = 0;
		virtual DGLE_RESULT DGLE_API SetMatrix(const TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType = MT_MODELVIEW) = 0;
		virtual DGLE_RESULT DGLE_API GetMatrix(TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType = MT_MODELVIEW) = 0;
		virtual DGLE_RESULT DGLE_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount) = 0;
		virtual DGLE_RESULT DGLE_API DrawBuffer(ICoreGeometryBuffer *pBuffer) = 0;
		virtual DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API ToggleBlendState(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API ToggleAlphaTestState(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetBlendState(const TBlendStateDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API GetBlendState(TBlendStateDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API SetDepthStencilState(const TDepthStencilDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API GetDepthStencilState(TDepthStencilDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API SetRasterizerState(const TRasterizerStateDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API GetRasterizerState(TRasterizerStateDesc &stState) = 0;
		virtual DGLE_RESULT DGLE_API BindTexture(ICoreTexture *pTex, uint uiTextureLayer = 0) = 0;
		virtual DGLE_RESULT DGLE_API GetBindedTexture(ICoreTexture *&prTex, uint uiTextureLayer) = 0;
		virtual DGLE_RESULT DGLE_API GetFixedFunctionPipelineAPI(IFixedFunctionPipeline *&prFFP) = 0;
		virtual DGLE_RESULT DGLE_API GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue) = 0;
		virtual DGLE_RESULT DGLE_API IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported) = 0;
		virtual DGLE_RESULT DGLE_API GetRendererType(E_CORE_RENDERER_TYPE &eType) = 0;
	};

	// {CA99FAF4-D818-4E16-BF96-C84D4E5F3A8F}
	static const GUID IID_IFixedFunctionPipeline = 
	{ 0xca99faf4, 0xd818, 0x4e16, { 0xbf, 0x96, 0xc8, 0x4d, 0x4e, 0x5f, 0x3a, 0x8f } };

	class IFixedFunctionPipeline : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API PushStates() = 0;
		virtual DGLE_RESULT DGLE_API PopStates() = 0;

		virtual DGLE_RESULT DGLE_API SetMaterialDiffuseColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetMaterialSpecularColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetMaterialShininess(float fShininess) = 0;

		virtual DGLE_RESULT DGLE_API GetMaterialDiffuseColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetMaterialSpecularColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetMaterialShininess(float &fShininess) = 0;

		virtual DGLE_RESULT DGLE_API ToggleGlobalLighting(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetGloablAmbientLight(const TColor4 &stColor) = 0;

		virtual DGLE_RESULT DGLE_API GetMaxLightsPerPassCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API IsGlobalLightingEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetGloablAmbientLight(TColor4 &stColor) = 0;

		virtual DGLE_RESULT DGLE_API SetLightEnabled(uint uiIdx, bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetLightColor(uint uiIdx, const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetLightIntensity(uint uiIdx, float fIntensity) = 0;
		virtual DGLE_RESULT DGLE_API ConfigureDirectionalLight(uint uiIdx, const TVector3 &stDirection) = 0;
		virtual DGLE_RESULT DGLE_API ConfigurePointLight(uint uiIdx, const TPoint3 &stPosition, float fRange) = 0;
		virtual DGLE_RESULT DGLE_API ConfigureSpotLight(uint uiIdx, const TPoint3 &stPosition, const TVector3 &stDirection, float fRange, float fSpotAngle) = 0;

		virtual DGLE_RESULT DGLE_API GetLightEnabled(uint uiIdx, bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetLightColor(uint uiIdx, TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetLightIntensity(uint uiIdx, float &fIntensity) = 0;
		virtual DGLE_RESULT DGLE_API GetLightType(uint uiIdx, E_LIGHT_TYPE &eType) = 0;
		virtual DGLE_RESULT DGLE_API GetDirectionalLightConfiguration(uint uiIdx, TVector3 &stDirection) = 0;
		virtual DGLE_RESULT DGLE_API GetPointLightConfiguration(uint uiIdx, TPoint3 &stPosition, float &fRange) = 0;
		virtual DGLE_RESULT DGLE_API GetSpotLightConfiguration(uint uiIdx, TPoint3 &stPosition, TVector3 &stDirection, float &fRange, float &fSpotAngle) = 0;

		virtual DGLE_RESULT DGLE_API SetFogEnabled(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API ConfigureFog(float fStart, float fEnd) = 0;

		virtual DGLE_RESULT DGLE_API GetFogEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetFogConfiguration(float &fStart, float &fEnd) = 0;
	};

}

#endif //DGLE_CRENDERER