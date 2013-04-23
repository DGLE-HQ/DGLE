{
\file    DGLE_CoreRenderer.pas
\author    Korotkov Andrey aka DRON
\version  2:0.3.0
\date    xx.xx.2012 (c)Korotkov Andrey

\brief    This header provides interface of low-level DGLE rendering API.

This header is a part of DGLE_SDK.
}
unit DGLE_CoreRenderer;

interface

{$I include.inc}

{$IFNDEF DGLE_CRENDERER}
{$DEFINE DGLE_CRENDERER}
{$ENDIF}

uses
  DGLE_types, DGLE_Base;

type

  E_CORE_RENDERER_TYPE =
  (
    CRT_UNKNOWN                             = 0,
    CRT_OPENGL_LEGACY                       = 1,
    CRT_OPENGL_4_1                          = 2,
    CRT_OPENGL_ES_1_1                       = 3,
    CRT_OPENGL_ES_2_0                       = 4,
    CRT_DIRECT_3D_9_0c                      = 5,
    CRT_DIRECT_3D_11                        = 6
  );

  E_CORE_RENDERER_FEATURE_TYPE =
  (
    CRFT_BUILTIN_FULLSCREEN_MODE            = 0,
    CRFT_BUILTIN_STATE_FILTER               = 1,
    CRFT_MULTISAMPLING                      = 2,
    CRFT_VSYNC                              = 3,
    CRFT_PROGRAMMABLE_PIPELINE              = 4,
    CRFT_LEGACY_FIXED_FUNCTION_PIPELINE_API = 5,
    CRFT_BGRA_DATA_FORMAT                   = 6,
    CRFT_TEXTURE_COMPRESSION                = 7,
    CRFT_NON_POWER_OF_TWO_TEXTURES          = 8,
    CRFT_DEPTH_TEXTURES                     = 9,
    CRFT_TEXTURE_ANISOTROPY                 = 10,
    CRFT_TEXTURE_MIPMAP_GENERATION          = 11,
    CRFT_TEXTURE_MIRRORED_REPEAT            = 12,
    CRFT_TEXTURE_MIRROR_CLAMP               = 13,
    CRFT_GEOMETRY_BUFFER                    = 14,
    CRFT_FRAME_BUFFER                       = 15
  );

  E_MATRIX_TYPE =
  (
    MT_PROJECTION                           = 0,
    MT_MODELVIEW                            = 1,
    MT_TEXTURE                              = 2
  );

  E_TEXTURE_TYPE =
  (
    TT_2D                                   = 0,
    TT_3D                                   = 1
  );

  E_CORE_RENDERER_METRIC_TYPE =
  (
    CRMT_MAX_TEXTURE_RESOLUTION             = 0,
    CRMT_MAX_ANISOTROPY_LEVEL               = 1,
    CRMT_MAX_LIGHTS_PER_PASS                = 2,
    CRMT_MAX_TEXTURE_LAYERS                 = 3
  );

  E_COMPARISON_FUNC =
  (
    CF_NEVER                                = 0,
    CF_LESS                                 = 1,
    CF_EQUAL                                = 2,
    CF_LESS_EQUAL                           = 3,
    CF_GREATER                              = 4,
    CF_NOT_EQUAL                            = 5,
    CF_GREATER_EQUAL                        = 6,
    CF_ALWAYS                               = 7
  );

  E_POLYGON_CULL_MODE =
  (
    PCM_NONE                                = 0,
    PCM_FRONT                               = 1,
    PCM_BACK                                = 2
  );

 { For future needs.
  E_STENCIL_OPERATION =
  (
    SO_KEEP                                 = 0,
    SO_ZERO                                 = 1,
    SO_REPLACE                              = 2,
    SO_INVERT                               = 3,
    SO_INCR                                 = 4,
    SO_DECR                                 = 5,
  );

  E_BLEND_OPERATION =
  (
    BO_ADD                                  = 0,
    BO_SUBTRACT                             = 1,
    BO_REV_SUBTRACT                         = 2,
    BO_MIN                                  = 3,
    BO_MAX                                  = 4
  );
 }

  E_BLEND_FACTOR =
  (
    BF_ZERO                                 = 0,
    BF_ONE                                  = 1,
    BF_SRC_COLOR                            = 2,
    BF_SRC_ALPHA                            = 3,
    BF_DST_COLOR                            = 4,
    BF_DST_ALPHA                            = 5,
    BF_ONE_MINUS_SRC_COLOR                  = 6,
    BF_ONE_MINUS_SRC_ALPHA                  = 7
  { For future needs.
    BF_ONE_MINUS_DST_COLOR                  = 8,
    BF_ONE_MINUS_DST_ALPHA                  = 9,
    BF_SRC_ALPHA_SATURATE?                  = 10,
    BF_SRC1_COLOR                           = 11,
    BF_ONE_MINUS_SRC1_COLOR                 = 12,
    BF_SRC1_ALPHA                           = 13,
    BF_ONE_MINUS_SRC1_ALPHA                 = 14
  }
  );

  E_CORE_RENDERER_DATA_ALIGNMENT =
  (
    CRDA_ALIGNED_BY_4                       = 0,
    CRDA_ALIGNED_BY_1                       = 1
  );

  E_CORE_RENDERER_BUFFER_TYPE =
  (
    CRBT_SOFTWARE                           = 0,
    CRBT_HARDWARE_STATIC                    = 1,
    CRBT_HARDWARE_DYNAMIC                   = 2
  );

  E_CORE_RENDERER_DRAW_MODE =
  (
    CRDM_POINTS                             = 0,
    CRDM_LINES                              = 1,
    CRDM_TRIANGLES                          = 2,
    CRDM_LINE_STRIP                         = 3,
    CRDM_TRIANGLE_STRIP                     = 4,
    CRDM_TRIANGLE_FAN                       = 5
  );

  E_ATTRIBUTE_DATA_TYPE =
  (
    ADT_FLOAT                               = 0,
    ADT_BYTE                                = 1,
    ADT_UBYTE                               = 2,
    ADT_SHORT                               = 3,
    ADT_USHORT                              = 4,
    ADT_INT                                 = 5,
    ADT_UINT                                = 6
  );

  E_ATTRIBUTE_COMPONENTS_COUNT =
  (
    ACC_ONE                                 = 0,
    ACC_TWO                                 = 1,
    ACC_THREE                               = 2,
    ACC_FOUR                                = 3
  );

type

  TBlendStateDesc = record

    bEnabled: Boolean;

    eSrcFactor: E_BLEND_FACTOR;
    eDstFactor: E_BLEND_FACTOR;

  (* For future needs.
  eOperation: E_BLEND_OPERATION;

  bSeparate: Boolean;
  eSrcAlpha: E_BLEND_FACTOR;
  eDstAlpha: E_BLEND_FACTOR;
  eOpAlpha : E_BLEND_OPERATION;
    *)
{$IF CompilerVersion >= 18}
  constructor Create(Dummy: Byte);
{$IFEND}
  end;


 (* For future needs.
 TStencilFaceDesc = Record

  eStencilFailOp:   E_STENCIL_OPERATION;
  eStencilDepthFailOp:  E_STENCIL_OPERATION;
  eStencilPassOp:   E_STENCIL_OPERATION;
  eStencilFunc:     E_COMPARISON_FUNC;
  end;
 *)


TDepthStencilDesc = record
  bDepthTestEnabled: Boolean;
  bWriteToDepthBuffer: Boolean;
  eDepthFunc: E_COMPARISON_FUNC;

{ For future needs.
  bStencilEnabled: Boolean;
  ui8StencilReadMask: Byte;
  ui8StencilWriteMask: Byte;
  stFrontFace: TStencilFaceDesc
  stBackFace: TStencilFaceDesc;
}
{$IF CompilerVersion >= 18}
  constructor Create(Dummy: Byte);
{$IFEND}
end;


TRasterizerStateDesc = record

  bWireframe: Boolean;

  eCullMode: E_POLYGON_CULL_MODE;

  bFrontCounterClockwise: Boolean;

  bScissorEnabled: Boolean;

  bAlphaTestEnabled: Boolean;
  eAlphaTestFunc: E_COMPARISON_FUNC;

  fAlphaTestRefValue: Single;
{ For future needs.
  iDepthBias: Integer;
  fDepthBiasClamp: Single;
  fSlopeScaledDepthBias: Single;
  bDepthClipEnabled: Boolean;
}
{$IF CompilerVersion >= 18}
constructor Create(Dummy: Byte);
{$IFEND}
end;

  TDrawDataAttributes = record
    uiAttribOffset: array[0..7] of Cardinal;
    uiAttribStride: array[0..7] of Cardinal;
    eAttribDataType: array[0..7] of E_ATTRIBUTE_DATA_TYPE;
    eAttribCompsCount: array[0..7] of E_ATTRIBUTE_COMPONENTS_COUNT;

    {$IF CompilerVersion >= 18}
    constructor Create(Dummy: Byte);
    {$IFEND}
  end;
  PDrawDataAttributes = ^TDrawDataAttributes;

  TDrawDataDesc = record
    pData: Pointer; //Must be start of the vertex data. 2 or 3 floats

    uiVertexStride: Cardinal;
    bVertices2D: Boolean;

    uiNormalOffset : Cardinal; //3 floats
    uiNormalStride : Cardinal;

    uiTextureVertexOffset: Cardinal; //2 floats
    uiTextureVertexStride: Cardinal;

    uiColorOffset: Cardinal; //4 floats
    uiColorStride: Cardinal;

    {not implemeted} uiTangentOffset, uiBinormalOffset: Cardinal; //6 floats, 3 for tangent and 3 for binormal
    {not implemeted} uiTangentStride, uiBinormalStride: Cardinal;

    {not implemeted} pAttribs: PDrawDataAttributes;

    pIndexBuffer: Pointer; //May point to separate memory. uint16 or uint32 data pointer.
    bIndexBuffer32: Boolean;


    {$IF CompilerVersion >= 18}
    constructor Create(Dummy: Byte); overload;
    constructor Create(pDataPointer: Pointer; uiNormalDataOffset: Cardinal; uiTextureVertexDataOffset: Cardinal; bIs2d: Boolean); overload;
    {$IFEND}
    {$IF CompilerVersion >= 20}
    class operator Equal(const this, desc: TDrawDataDesc): Boolean; inline;
    {$IFEND}
  end;

IBaseRenderObjectContainer = interface(IDGLE_Base)
  ['{5C5C5973-D826-42ED-B641-A84DDDAAE2A3}']
  function GetObjectType(out eType: E_ENGINE_OBJECT_TYPE): DGLE_RESULT; stdcall;
end;

IOpenGLTextureContainer = interface(IBaseRenderObjectContainer)
  ['{7264D8D2-C3AF-4ED3-91D1-90E02BE6A4EE}']
  function GetTexture(out texture: GLuint): DGLE_RESULT; stdcall;
end;

IOpenGLBufferContainer = interface(IBaseRenderObjectContainer)
  ['{152B744F-7C1B-414F-BEC1-CD40A308E5DF}']
  function GetVertexBufferObject(out vbo: GLuint): DGLE_RESULT; stdcall;
  function GetIndexBufferObject(out vbo: GLuint): DGLE_RESULT; stdcall;
end;

ICoreTexture = interface(IDGLE_Base)
  ['{8BFF07F9-2A8E-41D0-8505-3128C1B8160A}']
  function GetSize(out width, height: Cardinal): DGLE_RESULT; stdcall;
  function GetDepth(out depth: Cardinal): DGLE_RESULT; stdcall;
  function GetType(out eType: E_TEXTURE_TYPE): DGLE_RESULT; stdcall;

  function GetFormat(out eFormat: E_TEXTURE_DATA_FORMAT): DGLE_RESULT; stdcall;
  function GetLoadFlags(out eLoadFlags: {E_TEXTURE_LOAD_FLAGS} cARDINAL): DGLE_RESULT; stdcall;
  function GetPixelData(pData: Pointer; out uiDataSize: Cardinal; uiLodLevel: Cardinal = 0): DGLE_RESULT; stdcall;
  function SetPixelData(const pData: Pointer; uiDataSize: Cardinal; uiLodLevel: Cardinal = 0): DGLE_RESULT; stdcall;
  function Reallocate(const pData: Pointer; uiWidth: Cardinal; uiHeight: Cardinal; eDataFormat: E_TEXTURE_DATA_FORMAT): DGLE_RESULT; stdcall;
  function GetBaseObject(out prObj: IBaseRenderObjectContainer): DGLE_RESULT; stdcall;
  function Free(): DGLE_RESULT; stdcall;
end;

ICoreGeometryBuffer = interface(IDGLE_Base)
  ['{9A77DCFF-9E4B-4716-9BBB-A316BF217F7A}']
  function GetGeometryData(out stDesc: TDrawDataDesc; uiVerticesDataSize, iIndexesDataSize: Cardinal): DGLE_RESULT; stdcall;
  function SetGeometryData(const stDesc: TDrawDataDesc; uiVerticesDataSize, uiIndexesDataSize: Cardinal): DGLE_RESULT; stdcall;
  function Reallocate(const stDesc: TDrawDataDesc; uiVerticesCount: Cardinal; uiIndexesCount: Cardinal; eMode: E_CORE_RENDERER_DRAW_MODE): DGLE_RESULT; stdcall;
  function GetBufferDimensions(out uiVerticesDataSize, uiVerticesCount, uiIndexesDataSize, uiIndexesCount: Cardinal): DGLE_RESULT; stdcall;
  function GetBufferDrawDataDesc(out stDesc: TDrawDataDesc): DGLE_RESULT; stdcall;
  function GetBufferDrawMode(out eMode: E_CORE_RENDERER_DRAW_MODE): DGLE_RESULT; stdcall;
  function GetBufferType(out eType: E_CORE_RENDERER_BUFFER_TYPE): DGLE_RESULT; stdcall;
  function GetBaseObject(out prObj: IBaseRenderObjectContainer): DGLE_RESULT; stdcall;
  function Free(): DGLE_RESULT; stdcall;
end;

IFixedFunctionPipeline = interface;

ICoreRenderer = interface(IEngineSubSystem)
  ['{C3B687A1-57B0-4E21-BE4C-4D92F3FAB311}']
  //Must not be called by user
  function Prepare(out stResults: TCRndrInitResults): DGLE_RESULT; stdcall;
  function Initialize(out stResults: TCRndrInitResults): DGLE_RESULT; stdcall;
  function Finalize(): DGLE_RESULT; stdcall;
  function AdjustMode(out stNewWin: TEngineWindow): DGLE_RESULT; stdcall;
  //

  function MakeCurrent(): DGLE_RESULT; stdcall;
  function Present(): DGLE_RESULT; stdcall;
  function SetClearColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function GetClearColor(out stColor: TColor4): DGLE_RESULT; stdcall;
  function Clear(bColor: Boolean = True; bDepth: Boolean = True; bStencil: Boolean = True): DGLE_RESULT; stdcall;
  function SetViewport(x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function GetViewport(out x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function SetScissorRectangle(x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function GetScissorRectangle(out x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function SetLineWidth(fWidth: Single): DGLE_RESULT; stdcall;
  function GetLineWidth(out fWidth: Single): DGLE_RESULT; stdcall;
  function SetPointSize(fSize: Single): DGLE_RESULT; stdcall;
  function GetPointSize(out fSize: Single): DGLE_RESULT; stdcall;
  function ReadFrameBuffer(uiX, uiY, uiWidth, uiHeight: Cardinal; pData: Pointer; uiDataSize: Cardinal; eDataFormat: E_TEXTURE_DATA_FORMAT): DGLE_RESULT; stdcall;
  function SetRenderTarget(pTexture: ICoreTexture): DGLE_RESULT; stdcall;
  function GetRenderTarget(out prTexture: ICoreTexture): DGLE_RESULT; stdcall;
  function CreateTexture(out prTex: ICoreTexture; const pData: Pointer; uiWidth, uiHeight: Cardinal; bMipmapsPresented: Boolean; eDataAlignment: E_CORE_RENDERER_DATA_ALIGNMENT;
    eDataFormat: E_TEXTURE_DATA_FORMAT; eLoadFlags: {E_TEXTURE_LOAD_FLAGS} Cardinal): DGLE_RESULT; stdcall;
  function CreateGeometryBuffer(out prBuffer: ICoreGeometryBuffer; const stDrawDesc: TDrawDataDesc; uiVerticesCount, uiIndexesCount: Cardinal;
    eMode: E_CORE_RENDERER_DRAW_MODE; eType: E_CORE_RENDERER_BUFFER_TYPE): DGLE_RESULT; stdcall;
  function ToggleStateFilter(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function InvalidateStateFilter(): DGLE_RESULT; stdcall;
  function PushStates(): DGLE_RESULT; stdcall;
  function PopStates(): DGLE_RESULT; stdcall;
  function SetMatrix(const stMatrix: TMatrix4x4; eMatType: E_MATRIX_TYPE = MT_MODELVIEW): DGLE_RESULT; stdcall;
  function GetMatrix(out stMatrix: TMatrix4x4; eMatType: E_MATRIX_TYPE = MT_MODELVIEW): DGLE_RESULT; stdcall;
  function Draw(const stDrawDesc: TDrawDataDesc; eMode: E_CORE_RENDERER_DRAW_MODE; uiCount: Cardinal): DGLE_RESULT; stdcall;
  function DrawBuffer(pBuffer: ICoreGeometryBuffer): DGLE_RESULT; stdcall;
  function SetColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function GetColor(out stColor: TColor4): DGLE_RESULT; stdcall;
  function ToggleBlendState(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function ToggleAlphaTestState(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function SetBlendState(const stState: TBlendStateDesc): DGLE_RESULT; stdcall;
  function GetBlendState(out stState: TBlendStateDesc): DGLE_RESULT; stdcall;
  function SetDepthStencilState(const stState: TDepthStencilDesc): DGLE_RESULT; stdcall;
  function GetDepthStencilState(out stState: TDepthStencilDesc): DGLE_RESULT; stdcall;
  function SetRasterizerState(const stState: TRasterizerStateDesc): DGLE_RESULT; stdcall;
  function GetRasterizerState(out stState: TRasterizerStateDesc): DGLE_RESULT; stdcall;
  function BindTexture(pTex: ICoreTexture; uiTextureLayer: Cardinal = 0): DGLE_RESULT; stdcall;
  function GetBindedTexture(out prTex: ICoreTexture; uiTextureLayer: Cardinal): DGLE_RESULT; stdcall;
  function GetFixedFunctionPipelineAPI(out prFFP: IFixedFunctionPipeline): DGLE_RESULT; stdcall;
  function GetDeviceMetric(eMetric: E_CORE_RENDERER_METRIC_TYPE; out iValue: Integer): DGLE_RESULT; stdcall;
  function IsFeatureSupported(eFeature: E_CORE_RENDERER_FEATURE_TYPE; out bIsSupported: Boolean): DGLE_RESULT; stdcall;
  function GetRendererType(out eType: E_CORE_RENDERER_TYPE): DGLE_RESULT; stdcall;
end;

IFixedFunctionPipeline = interface(IDGLE_Base)
  ['{CA99FAF4-D818-4E16-BF96-C84D4E5F3A8F}']

  function PushStates(): DGLE_RESULT; stdcall;
  function PopStates(): DGLE_RESULT; stdcall;

  function SetMaterialDiffuseColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function SetMaterialSpecularColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function SetMaterialShininess(fShininess: Single): DGLE_RESULT; stdcall;

  function GetMaterialDiffuseColor(out stColor: TColor4): DGLE_RESULT; stdcall;
  function GetMaterialSpecularColor(out stColor: TColor4): DGLE_RESULT; stdcall;
  function GetMaterialShininess(out fShininess: Single): DGLE_RESULT; stdcall;

  function ToggleGlobalLighting(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function SetGloablAmbientLight(const stColor: TColor4): DGLE_RESULT; stdcall;

  function IsGlobalLightingEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
  function GetGloablAmbientLight(out stColor: TColor4): DGLE_RESULT; stdcall;

  function SetLightEnabled(uiIdx: Cardinal; bEnabled: Boolean): DGLE_RESULT; stdcall;
  function SetLightColor(uiIdx: Cardinal; const stColor: TColor4): DGLE_RESULT; stdcall;
  function SetLightIntensity(uiIdx: Cardinal; fIntensity: Single): DGLE_RESULT; stdcall;
  function ConfigureDirectionalLight(uiIdx: Cardinal; const stDirection: TVector3): DGLE_RESULT; stdcall;
  function ConfigurePointLight(uiIdx: Cardinal; const stPosition: TPoint3; fRange: Single): DGLE_RESULT; stdcall;
  function ConfigureSpotLight(uiIdx: Cardinal; const stPosition: TPoint3; const stDirection: TVector3; fRange, fSpotAngle: Single): DGLE_RESULT; stdcall;

  function GetLightEnabled(uiIdx: Cardinal; out bEnabled: Boolean): DGLE_RESULT; stdcall;
  function GetLightColor(uiIdx: Cardinal; out stColor: TColor4): DGLE_RESULT; stdcall;
  function GetLightIntensity(uiIdx: Cardinal; out fIntensity: Single): DGLE_RESULT; stdcall;
  function GetLightType(uiIdx: Cardinal; out eType: E_LIGHT_TYPE ): DGLE_RESULT; stdcall;
  function GetDirectionalLightConfiguration(uiIdx: Cardinal; out stDirection: TVector3): DGLE_RESULT; stdcall;
  function GetPointLightConfiguration(uiIdx: Cardinal; out stPosition: TPoint3; out fRange: Single): DGLE_RESULT; stdcall;
  function GetSpotLightConfiguration(uiIdx: Cardinal; out stPosition: TPoint3; out stDirection: TVector3; out fRange, fSpotAngle: Single): DGLE_RESULT; stdcall;

  function SetFogEnabled(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function SetFogColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function ConfigureFog(fStart, fEnd, fDensity: Single): DGLE_RESULT; stdcall;

  function GetFogEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
  function GetFogColor(out stColor: TColor4): DGLE_RESULT; stdcall;
  function GetFogConfiguration(out fStart, fEnd, fDensity: Single): DGLE_RESULT; stdcall;

end;                                

function BlendStateDesc(): TBlendStateDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DepthStencilDesc(): TDepthStencilDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}
function RasterizerStateDesc(): TRasterizerStateDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DrawDataAttributes(): TDrawDataAttributes; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DrawDataDesc(): TDrawDataDesc; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DrawDataDesc(pDataPointer: Pointer; uiNormalDataOffset: Cardinal; uiTextureVertexDataOffset: Cardinal; bIs2d: Boolean): TDrawDataDesc; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}

implementation

function BlendStateDesc(): TBlendStateDesc;
begin
  Result.bEnabled := False;
  Result.eSrcFactor := BF_SRC_ALPHA;
  Result.eDstFactor := BF_ONE_MINUS_SRC_ALPHA;
end;

function DepthStencilDesc(): TDepthStencilDesc;
begin
  Result.bDepthTestEnabled := True;
  Result.bWriteToDepthBuffer := True;
  Result.eDepthFunc := CF_LESS_EQUAL;
end;

function RasterizerStateDesc(): TRasterizerStateDesc;
begin
  Result.bWireframe := False;
  Result.eCullMode := PCM_NONE;
  Result.bFrontCounterClockwise := True;
  Result.bScissorEnabled := False;
  Result.bAlphaTestEnabled := False;
  Result.eAlphaTestFunc := CF_GREATER;
  Result.fAlphaTestRefValue := 0.25;
end;

function DrawDataAttributes(): TDrawDataAttributes;
var
  i: Integer;
begin
  for i := 0 to 7 do
    Result.uiAttribOffset[i] := minus1;
end;

function DrawDataDesc(): TDrawDataDesc;
begin
  Result.pData            := nil;
  Result.uiVertexStride   := 0;
  Result.bVertices2D      := False;
  Result.uiNormalOffset   := minus1;
  Result.uiNormalStride   := 0;
  Result.uiTextureVertexOffset := minus1;
  Result.uiTextureVertexStride := 0;
  Result.uiColorOffset    := minus1;
  Result.uiColorStride    := 0;
  Result.uiTangentOffset  := minus1;
  Result.uiBinormalOffset := minus1;
  Result.uiTangentStride  := 0;
  Result.uiBinormalStride := 0;
  Result.pIndexBuffer     := nil;
  Result.bIndexBuffer32   := False;
  Result.pAttribs         := nil;
end;

function DrawDataDesc(pDataPointer: Pointer; uiNormalDataOffset: Cardinal; uiTextureVertexDataOffset: Cardinal; bIs2d: Boolean): TDrawDataDesc;
begin
  Result.pData            := pDataPointer;
  Result.uiVertexStride   := 0;
  Result.bVertices2D      := bIs2d;
  Result.uiNormalOffset   := uiNormalDataOffset;
  Result.uiNormalStride   := 0;
  Result.uiTextureVertexOffset := uiTextureVertexDataOffset;
  Result.uiTextureVertexStride := 0;
  Result.uiColorOffset    := minus1;
  Result.uiColorStride    := 0;
  Result.uiTangentOffset  := minus1;
  Result.uiBinormalOffset := minus1;
  Result.uiTangentStride  := 0;
  Result.uiBinormalStride := 0;
  Result.pIndexBuffer     := nil;
  Result.bIndexBuffer32   := False;
  Result.pAttribs         := nil;
end;

{$IF CompilerVersion >= 18}
constructor TBlendStateDesc.Create(Dummy: Byte);
begin
  Self := BlendStateDesc();
end;

constructor TDepthStencilDesc.Create(Dummy: Byte);
begin
  Self := DepthStencilDesc();
end;

constructor TRasterizerStateDesc.Create(Dummy: Byte);
begin
  Self := RasterizerStateDesc();
end;

constructor TDrawDataAttributes.Create(Dummy: Byte);
begin
  Self := DrawDataAttributes();
end;

constructor TDrawDataDesc.Create(Dummy: Byte);
begin
  Self := DrawDataDesc();
end;

constructor TDrawDataDesc.Create(pDataPointer: Pointer; uiNormalDataOffset: Cardinal; uiTextureVertexDataOffset: Cardinal; bIs2d: Boolean);
begin
  Self := DrawDataDesc(pDataPointer, uiNormalDataOffset, uiTextureVertexDataOffset, bIs2d);
end;
{$IFEND}

{$IF CompilerVersion >= 20}
class operator TDrawDataDesc.Equal(const this, desc: TDrawDataDesc): Boolean; 
begin
  Result := (this.pData = desc.pData) and (this.uiVertexStride = desc.uiVertexStride) and (this.bVertices2D = desc.bVertices2D) and
    (this.uiNormalOffset = desc.uiNormalOffset) and (this.uiNormalStride = desc.uiNormalStride) and (this.uiTextureVertexOffset = desc.uiTextureVertexOffset) and
    (this.uiTextureVertexStride = desc.uiTextureVertexStride) and (this.uiColorOffset = desc.uiColorOffset) and (this.uiColorStride = desc.uiColorStride) and
    (this.uiTangentOffset = desc.uiTangentOffset) and (this.uiBinormalOffset = desc.uiBinormalOffset) and (this.uiTangentStride = desc.uiTangentStride) and
    (this.uiBinormalStride = desc.uiBinormalStride) and (this.pAttribs = desc.pAttribs) and (this.pIndexBuffer = desc.pIndexBuffer) and (this.bIndexBuffer32 = desc.bIndexBuffer32);
end;
{$IFEND}
end.

