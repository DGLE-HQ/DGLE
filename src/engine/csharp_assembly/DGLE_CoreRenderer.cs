/**
\file		DGLE_CoreRenderer.cs
\author		Korotkov Andrey aka DRON
\version	2:0.3.1
\date		17.11.2014 (c)Korotkov Andrey

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
        /* for future needs
        CRT_OPENGL_4_1,
        CRT_OPENGL_ES_1_1,
        CRT_OPENGL_ES_2_0,
        CRT_DIRECT_3D_9_0c,
        CRT_DIRECT_3D_11
         */
    };

    public enum E_CORE_RENDERER_FEATURE_TYPE
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
        CRMT_MAX_TEXTURE_LAYERS,
        CRMT_MAX_ANISOTROPY_LEVEL
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

    public enum E_ATTRIBUTE_DATA_TYPE
    {
        ADT_FLOAT = 0,
        ADT_BYTE,
        ADT_UBYTE,
        ADT_SHORT,
        ADT_USHORT,
        ADT_INT,
        ADT_UINT
    };

    public enum E_ATTRIBUTE_COMPONENTS_COUNT
    {
        ACC_ONE = 0,
        ACC_TWO,
        ACC_THREE,
        ACC_FOUR
    };


    /*
    #if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
    #pragma pack( push, 1 )
    #endif
    */
    public struct TBlendStateDesc
    {
        public bool bEnabled;

        public E_BLEND_FACTOR eSrcFactor;
        public E_BLEND_FACTOR eDstFactor;

        /* For future needs.
        public E_BLEND_OPERATION	eOperation;

        public bool bSeparate;
        public E_BLEND_FACTOR		eSrcAlpha;
        public E_BLEND_FACTOR		eDestAlpha;
        public E_BLEND_OPERATION	eOpAlpha;
        */
        public static TBlendStateDesc Default()
        {
            TBlendStateDesc result;
            result.bEnabled = false;
            result.eSrcFactor = E_BLEND_FACTOR.BF_SRC_ALPHA;
            result.eDstFactor = E_BLEND_FACTOR.BF_ONE_MINUS_SRC_ALPHA;
            return result;
        }
    };

    /* For future needs.
    public struct TStencilFaceDesc
    {
        public E_STENCIL_OPERATION	eStencilFailOp;
        public E_STENCIL_OPERATION	eStencilDepthFailOp;
        public E_STENCIL_OPERATION	eStencilPassOp;
        public E_COMPARISON_FUNC	eStencilFunc;
    };
    */

    public struct TDepthStencilDesc
    {
        public bool bDepthTestEnabled;
        public bool bWriteToDepthBuffer;
        public E_COMPARISON_FUNC eDepthFunc;

        /* For future needs.
        public bool	bStencilEnable;
        public uint8	ui8StencilReadMask;
        public 	ui8StencilWriteMask;
        public TStencilFaceDesc stFrontFace, stBackFace;
        */

        public static TDepthStencilDesc Default()
        {
            TDepthStencilDesc result;
            result.bDepthTestEnabled = true;
            result.bWriteToDepthBuffer = true;
            result.eDepthFunc = E_COMPARISON_FUNC.CF_LESS_EQUAL;
            return result;
        }
    };

    public struct TRasterizerStateDesc
    {
        public bool bWireframe;

        public E_POLYGON_CULL_MODE eCullMode;
        public bool bFrontCounterClockwise;

        public bool bScissorEnabled;

        public bool bAlphaTestEnabled;
        public E_COMPARISON_FUNC eAlphaTestFunc;
        public float fAlphaTestRefValue;

        /* For future needs.
        public int		iDepthBias;
        public float	fDepthBiasClamp;
        public float	fSlopeScaledDepthBias;
        public bool	bDepthClipEnabled;
        */

        public static TRasterizerStateDesc Default()
        {
            TRasterizerStateDesc result;
            result.bWireframe = false;
            result.eCullMode = E_POLYGON_CULL_MODE.PCM_NONE;
            result.bFrontCounterClockwise = true;
            result.bScissorEnabled = false;

            result.bAlphaTestEnabled = false;
            result.eAlphaTestFunc = E_COMPARISON_FUNC.CF_GREATER;
            result.fAlphaTestRefValue = 0.25f;
            return result;
        }
    };

    public struct TDrawDataAttributes
    {
        public uint[] uiAttribOffset;
        public uint[] uiAttribStride;
        public E_ATTRIBUTE_DATA_TYPE[] eAttribDataType;
        public E_ATTRIBUTE_COMPONENTS_COUNT[] eAttribCompsCount;


        const uint minus1 = 0xFFFFFFFF; // -1 should be, but uint doesn't allow -1 

        public static TDrawDataAttributes Default()
        {
            TDrawDataAttributes result;
            int len = 8;
            result.uiAttribOffset = new uint[len];
            result.uiAttribStride = new uint[len];
            result.eAttribDataType = new E_ATTRIBUTE_DATA_TYPE[len];
            result.eAttribCompsCount = new E_ATTRIBUTE_COMPONENTS_COUNT[len];
            for (int i = 0; i < len; ++i)
            {
                result.uiAttribOffset[i] = minus1;
                result.uiAttribStride[i] = 0;
                result.eAttribDataType[i] = E_ATTRIBUTE_DATA_TYPE.ADT_FLOAT;
                result.eAttribCompsCount[i] = E_ATTRIBUTE_COMPONENTS_COUNT.ACC_ONE;
            }
            return result;
        }
    };

    public struct TDrawDataDesc
    {
        //Pointer?
        [MarshalAs(UnmanagedType.LPArray)]
        public byte[] pData; //Must be start of the vertex data. 2 or 3 floats

        public uint uiVertexStride;
        public bool bVertices2D;

        public uint uiNormalOffset; //3 floats
        public uint uiNormalStride;

        public uint uiTextureVertexOffset; //2 floats
        public uint uiTextureVertexStride;

        public uint uiColorOffset; //4 floats
        public uint uiColorStride;

        /*not implemeted*/
        public uint uiTangentOffset;
        public uint uiBinormalOffset; //6 floats, 3 for tangent and 3 for binormal
        /*not implemeted*/
        public uint uiTangentStride, uiBinormalStride;

        /*not implemeted*/
        //public TDrawDataAttributes pAttribs;  // what to do with it ? //phomm


        //Pointer?

        [MarshalAs(UnmanagedType.LPArray)]
        public byte[] pIndexBuffer; //May point to separate memory. uint16 or uint32 data pointer.
        public bool bIndexBuffer32;

        //ToDo: Add VertexAttribPointers.


        const uint minus1 = 0xFFFFFFFF; // -1 should be, but uint doesn't allow -1 


        public static TDrawDataDesc Default()
        {
            TDrawDataDesc result;
            result.pData = null;
            result.uiVertexStride = 0;
            result.bVertices2D = false;
            result.uiNormalOffset = minus1;
            result.uiNormalStride = 0;
            result.uiTextureVertexOffset = minus1;
            result.uiTextureVertexStride = 0;
            result.uiColorOffset = minus1;
            result.uiColorStride = 0;
            result.pIndexBuffer = null;
            result.bIndexBuffer32 = false;
            result.uiTangentOffset = minus1;
            result.uiBinormalOffset = minus1;
            result.uiTangentStride = 0;
            result.uiBinormalStride = 0;
            result.pIndexBuffer = null;
            result.bIndexBuffer32 = false;
            //pAttribs = null;
            return result;
        }

        public TDrawDataDesc([MarshalAs(UnmanagedType.LPArray)] byte[] pDataPointer, uint uiNormalDataOffset, uint uiTextureVertexDataOffset, [MarshalAs(UnmanagedType.U1)] bool bIs2D)
        {
            pData = pDataPointer;
            uiVertexStride = 0;
            bVertices2D = bIs2D;
            uiNormalOffset = uiNormalDataOffset;
            uiNormalStride = 0;
            uiTextureVertexOffset = uiTextureVertexDataOffset;
            uiTextureVertexStride = 0;
            uiColorOffset = minus1;
            uiColorStride = 0;
            pIndexBuffer = null;
            bIndexBuffer32 = false;
            uiTangentOffset = minus1;
            uiBinormalOffset = minus1;
            uiTangentStride = 0;
            uiBinormalStride = 0;
            pIndexBuffer = null;
            bIndexBuffer32 = false;
            //pAttribs = null;
        }

        public static bool operator ==(TDrawDataDesc self, TDrawDataDesc desc)
        {
            return self.pData == desc.pData && self.uiVertexStride == desc.uiVertexStride && self.bVertices2D == desc.bVertices2D &&
                self.uiNormalOffset == desc.uiNormalOffset && self.uiNormalStride == desc.uiNormalStride && self.uiTextureVertexOffset == desc.uiTextureVertexOffset &&
                self.uiTextureVertexStride == desc.uiTextureVertexStride && self.uiColorOffset == desc.uiColorOffset && self.uiColorStride == desc.uiColorStride &&
                self.uiTangentOffset == desc.uiTangentOffset && self.uiBinormalOffset == desc.uiBinormalOffset && self.uiTangentStride == desc.uiTangentStride &&
                self.uiBinormalStride == desc.uiBinormalStride /*&& self.pAttribs == desc.pAttribs*/ && self.pIndexBuffer == desc.pIndexBuffer && self.bIndexBuffer32 == desc.bIndexBuffer32;
        }
        public static bool operator !=(TDrawDataDesc self, TDrawDataDesc desc)
        {
            return !(self == desc);
        }
    };


    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("5C5C5973-D826-42ED-B641-A84DDDAAE2A3")]
    public interface IBaseRenderObjectContainer : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        #endregion
        void GetObjectType(out E_ENGINE_OBJECT_TYPE eType);
    };


    // --------------GLuint not found in openGL.cs, here in openGL interfaces uint used instead-----------------

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("7264D8D2-C3AF-4ED3-91D1-90E02BE6A4EE")]
    public interface IOpenGLTextureContainer : IBaseRenderObjectContainer
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        #endregion
        void GetTexture(out uint texture);
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
        void GetBaseObject(out IBaseRenderObjectContainer prObj);
        void Free();
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

        // must not be called by user
        // here bool is TCRndrInitResults
        void Prepare([MarshalAs(UnmanagedType.U1)] ref bool stResults);
        void Initialize([MarshalAs(UnmanagedType.U1)] ref bool stResults);
        void Finalize();
        void AdjustMode(ref TEngineWindow stNewWin);
        //

        void MakeCurrent();
        void Present();
        void SetClearColor(ref TColor4 stColor);
        void GetClearColor(out TColor4 stColor);
        void Clear([MarshalAs(UnmanagedType.U1)] bool bColor = true, bool bDepth = true, [MarshalAs(UnmanagedType.U1)] bool bStencil = true);
        void SetViewport(uint x, uint y, uint width, uint height);
        void GetViewport(out uint x, out uint y, out uint width, out uint height);
        void SetScissorRectangle(uint x, uint y, uint width, uint height);
        void GetScissorRectangle(out uint x, out uint y, out uint width, out uint height);
        void SetLineWidth(float fWidth);
        void GetLineWidth(out float fWidth);
        void SetPointSize(float fSize);
        void GetPointSize(out float fSize);
        void ReadFrameBuffer(uint uiX, uint uiY, uint uiWidth, uint uiHeight, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat);
        void SetRenderTarget(ICoreTexture pTexture);
        void GetRenderTarget(out ICoreTexture prTexture);
        void CreateTexture(out ICoreTexture prTex, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiWidth, uint uiHeight, [MarshalAs(UnmanagedType.U1)] bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags);
        void CreateGeometryBuffer(out ICoreGeometryBuffer prBuffer, TDrawDataDesc stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType);
        void ToggleStateFilter([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void InvalidateStateFilter();
        void PushStates();
        void PopStates();
        void SetMatrix(TMatrix4x4 stMatrix, E_MATRIX_TYPE eMatType = E_MATRIX_TYPE.MT_MODELVIEW);
        void GetMatrix(out TMatrix4x4 stMatrix, E_MATRIX_TYPE eMatType = E_MATRIX_TYPE.MT_MODELVIEW);
        void Draw(TDrawDataDesc stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
        void DrawBuffer(ICoreGeometryBuffer pBuffer);
        void SetColor(TColor4 stColor);
        void GetColor(out TColor4 stColor);
        void ToggleBlendState([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void ToggleAlphaTestState([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetBlendState(TBlendStateDesc stState);
        void GetBlendState(out TBlendStateDesc stState);
        void SetDepthStencilState(TDepthStencilDesc stState);
        void GetDepthStencilState(out TDepthStencilDesc stState);
        void SetRasterizerState(TRasterizerStateDesc stState);
        void GetRasterizerState(out TRasterizerStateDesc stState);
        void BindTexture(ICoreTexture pTex, uint uiTextureLayer);
        void GetBindedTexture(out ICoreTexture prTex, uint uiTextureLayer);
        void GetFixedFunctionPipelineAPI(out IFixedFunctionPipeline prFFP);
        void GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, out int iValue);
        void IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, [MarshalAs(UnmanagedType.U1)] out bool bIsSupported);
        void GetRendererType(out E_CORE_RENDERER_TYPE eType);
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("CA99FAF4-D818-4E16-BF96-C84D4E5F3A8F")]
    public interface IFixedFunctionPipeline : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        #endregion

        void PushStates();
        void PopStates();

        void SetMaterialDiffuseColor(ref TColor4 stColor);
        void SetMaterialSpecularColor(ref TColor4 stColor);
        void SetMaterialShininess(float fShininess);

        void GetMaterialDiffuseColor(out TColor4 stColor);
        void GetMaterialSpecularColor(out TColor4 stColor);
        void GetMaterialShininess(out float fShininess);

        void ToggleGlobalLighting([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetGloablAmbientLight(ref TColor4 stColor);

        void GetMaxLightsPerPassCount(out uint uiCount);
        void IsGlobalLightingEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void GetGloablAmbientLight(out TColor4 stColor);

        void SetLightEnabled(uint uiIdx, [MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetLightColor(uint uiIdx, ref TColor4 stColor);
        void SetLightIntensity(uint uiIdx, float fIntensity);
        void ConfigureDirectionalLight(uint uiIdx, ref TPoint3 stDirection);
        void ConfigurePointLight(uint uiIdx, ref TPoint3 stPosition, float fRange);
        void ConfigureSpotLight(uint uiIdx, ref TPoint3 stPosition, ref TPoint3 stDirection, float fRange, float fSpotAngle);

        void GetLightEnabled(uint uiIdx, [MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void GetLightColor(uint uiIdx, out TColor4 stColor);
        void GetLightIntensity(uint uiIdx, out float fIntensity);
        void GetLightType(uint uiIdx, out E_LIGHT_TYPE eType);
        void GetDirectionalLightConfiguration(uint uiIdx, out TPoint3 stDirection);
        void GetPointLightConfiguration(uint uiIdx, out TPoint3 stPosition, out float fRange);
        void GetSpotLightConfiguration(uint uiIdx, out TPoint3 stPosition, out TPoint3 stDirection, out float fRange, out float fSpotAngle);

        void SetFogEnabled([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetFogColor(ref TColor4 stColor);
        void ConfigureFog(float fStart, float fEnd, float fDensity);

        void GetFogEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void GetFogColor(out TColor4 stColor);
        void GetFogConfiguration(out float fStart, out float fEnd, out float fDensity);
    };

}

//#endif //DGLE_CRENDERER