/**
\file		DGLE_CoreRenderer.cs
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		xx.xx.2012 (c)Korotkov Andrey

\brief		This header provides interface of low-level DGLE rendering API.

This header is a part of DGLE_SDK.
*/

using System;
using System.Runtime.InteropServices;

namespace DGLE
{
	public enum E_CORE_RENDERER_TYPE
	{
		CRT_UNKNOWN = 0,
		CRT_OPENGL_LEGACY,
		CRT_OPENGL_4_1,
		CRT_OPENGL_ES_1_1,
		CRT_OPENGL_ES_2_0,
		CRT_DIRECT_3D_9_0c,
		CRT_DIRECT_3D_11
	};

	public enum E_CORE_RENDERER_FEATURE_TYPE
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

	public enum E_TEXTURE_TYPE
	{
		TT_2D = 0,
		TT_3D
	};


	public enum E_MATRIX_TYPE
	{
		MT_PROJECTION = 0,
		MT_MODELVIEW,
		MT_TEXTURE
	};

	public enum E_CORE_RENDERER_METRIC_TYPE
	{
		CRMT_MAX_TEXTURE_RESOLUTION = 0,
		CRMT_MAX_ANISOTROPY_LEVEL,
		CRMT_MAX_LIGHTS_PER_PASS,
		CRMT_MAX_TEXTURE_LAYERS
	};

	public enum E_COMPARISON_FUNC
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

	public enum E_POLYGON_CULL_MODE
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

	public enum E_BLEND_FACTOR
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

	public enum E_CORE_RENDERER_DATA_ALIGNMENT
	{
		CRDA_ALIGNED_BY_4 = 0,
		CRDA_ALIGNED_BY_1
	};

	public enum E_CORE_RENDERER_BUFFER_TYPE
	{
		CRBT_SOFTWARE = 0,
		CRBT_HARDWARE_STATIC,
		CRBT_HARDWARE_DYNAMIC
	};

	public enum E_CORE_RENDERER_DRAW_MODE
	{
		CRDM_POINTS = 0,
		CRDM_LINES,
		CRDM_TRIANGLES,
		CRDM_LINE_STRIP,
		CRDM_TRIANGLE_STRIP,
		CRDM_TRIANGLE_FAN
	};

/*
#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack( push, 1 )
#endif
*/
	public struct TBlendStateDesc
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
		public TBlendStateDesc(byte dummy)
		{
			bEnable = false;
			eSrcFactor = E_BLEND_FACTOR.BF_SRC_ALPHA;
			eDestFactor = E_BLEND_FACTOR.BF_ONE_MINUS_SRC_ALPHA;
		}
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

	public struct TDepthStencilDesc
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

		public TDepthStencilDesc(byte dummy)
		{
			bDepthTestEnable = true;
			bWriteToDepthBuffer = true;
			eDepthFunc = E_COMPARISON_FUNC.CF_LESS_EQUAL;
		}
	};

	public struct TRasterizerStateDesc
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

		public TRasterizerStateDesc(byte dummy)
		{
		bWireframe = false;
			eCullMode = E_POLYGON_CULL_MODE.PCM_NONE;
			bFrontCounterClockwise = true;
			bScissorEnable = false;

		bAlphaTestEnable = false;
			eAlphaTestFunc = E_COMPARISON_FUNC.CF_GREATER;
			fAlphaTestRefValue = 0.25f;
		}
	};

	public struct TDrawDataDesc
	{
		//Pointer?
		[MarshalAs(UnmanagedType.LPArray)] byte[] pData; //Must be start of the vertex data. 2 or 3 floats

		uint uiVertexStride;
		bool bVertexCoord2;

		uint uiNormalOffset; //3 floats
		uint uiNormalStride;

		uint uiTexCoordOffset; //2 floats
		uint uiTexCoordStride;

		uint uiColorOffset; //4 floats
		uint uiColorStride;

		//Pointer?
		[MarshalAs(UnmanagedType.LPArray)] byte[] pIndexBuffer; //May point to separate memory. uint16 or uint32 data pointer.
		bool bIndexBuffer32;

		//ToDo: Add VertexAttribPointers.


		const int minus1 = 0; // -1 should be, but uint doesn't allow -1 


		public TDrawDataDesc(byte dummy)
		{
			pData = null; 
			uiVertexStride = 0; 
			bVertexCoord2 = false;
			uiNormalOffset = minus1; 
			uiNormalStride = 0;
			uiTexCoordOffset = minus1; 
			uiTexCoordStride = 0;
			uiColorOffset = minus1; 
			uiColorStride = 0;
			pIndexBuffer = null; 
			bIndexBuffer32 = false;
		}

		public TDrawDataDesc([MarshalAs(UnmanagedType.LPArray)] byte[] pDataPointer, uint uiTexCoordDataOffset = minus1, bool bTwoCoordPerVertex = true)
		{
			pData = pDataPointer; 
			uiVertexStride = 0; 
			bVertexCoord2 = bTwoCoordPerVertex;
			uiNormalOffset = minus1; 
			uiNormalStride = 0;
			uiTexCoordOffset = uiTexCoordDataOffset; 
			uiTexCoordStride = 0;
			uiColorOffset = minus1; 
			uiColorStride = 0;
			pIndexBuffer = null; 
			bIndexBuffer32 = false;
		}
	};
/*
#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack(pop)
#endif
*/


	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("5C5C5973-D826-42ED-B641-A84DDDAAE2A3")]
	public interface IBaseRenderObjectContainer : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion
		void GetObjectType(out E_ENG_OBJ_TYPE eType);
	};

/*
#if defined(OPENGL_LEGACY_BASE_OBJECTS)

#ifndef USE_GLEW_HEADER
#	include <gl/GL.h>
#else
#	include <gl/glew.h>
#endif
 */

 // --------------GLuint not found in openGL.cs, here in openGL interfaces uint used instead-----------------

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("7264D8D2-C3AF-4ED3-91D1-90E02BE6A4EE")]
	public interface IOpenGLTextureContainer : IBaseRenderObjectContainer
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion
		void  GetTexture(out uint texture);
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("152B744F-7C1B-414F-BEC1-CD40A308E5DF")]
	public interface IOpenGLBufferContainer : IBaseRenderObjectContainer
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion
		void GetVertexBufferObject(out uint vbo);
		void GetIndexBufferObject(out uint vbo);
	};

//#endif

	// {8BFF07F9-2A8E-41D0-8505-3128C1B8160A}

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("8BFF07F9-2A8E-41D0-8505-3128C1B8160A")]
	public interface ICoreTexture : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion
		void GetSize(out uint width, uint height);
		void GetDepth(out uint depth);
		void GetType(out E_TEXTURE_TYPE eType);

		void GetFormat(out E_TEXTURE_DATA_FORMAT eFormat);
		void GetLoadFlags(out E_TEXTURE_LOAD_FLAGS eLoadFlags);
		void GetPixelData([MarshalAs(UnmanagedType.LPArray)] out byte[] pData, uint uiDataSize, uint uiLodLevel);
		void SetPixelData([MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiDataSize, uint uiLodLevel);
		void Reallocate([MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat);
		void  GetBaseObject(out IBaseRenderObjectContainer prObj);
		void  Free();
	};


	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("9A77DCFF-9E4B-4716-9BBB-A316BF217F7A")]
	public interface ICoreGeometryBuffer : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion
		void GetGeometryData(out TDrawDataDesc stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize);
		void SetGeometryData(ref TDrawDataDesc stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize);
		void Reallocate(ref TDrawDataDesc stDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode);
		void GetBufferDimensions(out uint uiVerticesDataSize, out uint uiVerticesCount, out uint uiIndexesDataSize, out uint uiIndexesCount);
		void GetBufferDrawDataDesc(out TDrawDataDesc stDesc);
		void GetBufferDrawMode(out E_CORE_RENDERER_DRAW_MODE eMode);
		void GetBufferType(out E_CORE_RENDERER_BUFFER_TYPE eType);
		void GetBaseObject(out IBaseRenderObjectContainer prObj);
		void Free();
	};


	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("C3B687A1-57B0-4E21-BE4C-4D92F3FAB311")]
	public interface ICoreRenderer : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion	
		//Must not be called by user

		// here bool is TCRendererInitResult
		void Prepare(ref bool stResults);
		void Initialize(ref bool stResults);
		void Finalize();
		void AdjustMode(ref TEngWindow stNewWin);
		//

		void MakeCurrent();
		void Present();
		void SetClearColor(ref TColor4 stColor);
		void Clear(bool bColor = true, bool bDepth = true, bool bStencil = true);
		void SetViewport(uint x, uint y, uint width, uint height);
		void GetViewport(out uint x, out uint y, out uint width, out uint height);
		void SetScissor(uint x, uint y, uint width, uint height);
		void SetLineWidth(float fWidth);
		void SetPointSize(float fSize);
		void ReadFrameBuffer([MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat);
		void SetRenderTarget(ICoreTexture pTexture);
		void CreateTexture(out ICoreTexture prTex, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags);
		void CreateGeometryBuffer(out ICoreGeometryBuffer prBuffer, TDrawDataDesc stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType);
		void ToggleStateFilter(bool bEnabled);
		void PushStates();
		void PopStates();
		void SetMatrix(TMatrix stMat, E_MATRIX_TYPE eMatType = E_MATRIX_TYPE.MT_MODELVIEW);
		void GetMatrix(out TMatrix stMat, E_MATRIX_TYPE eMatType = E_MATRIX_TYPE.MT_MODELVIEW);
		void Draw(TDrawDataDesc stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
		void DrawBuffer(ICoreGeometryBuffer pBuffer);
		void SetColor(TColor4 stColor);
		void ToggleBlendState(bool bEnabled);
		void ToggleAlphaTestState(bool bEnabled);
		void SetBlendState(TBlendStateDesc stState);
		void GetBlendState(out TBlendStateDesc stState);
		void SetDepthStencilState( TDepthStencilDesc stState);
		void GetDepthStencilState(out TDepthStencilDesc stState);
		void SetRasterizerState(TRasterizerStateDesc stState);
		void GetRasterizerState(out TRasterizerStateDesc stState);
		void BindTexture(ICoreTexture pTex, uint uiTextureLayer);
		void GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, out int iValue);
		void IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, out bool bIsSupported);
		void GetRendererType(out E_CORE_RENDERER_TYPE eType);
	};

}

//#endif //DGLE_CRENDERER