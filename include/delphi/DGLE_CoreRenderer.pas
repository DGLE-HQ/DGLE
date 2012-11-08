{
\file		DGLE_CoreRenderer.pas
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		xx.xx.2012 (c)Korotkov Andrey

\brief		This header provides interface of low-level DGLE rendering API.

This header is a part of DGLE_SDK.
}
unit DGLE_CoreRenderer;

interface

{$IFDEF FPC}
{$MODE DELPHI}
{$ENDIF}

{$IFNDEF DGLE_CRENDERER}
{$DEFINE DGLE_CRENDERER}
{$ENDIF}

uses
  DGLE, DGLE_types;

const

 // E_CORE_RENDERER_TYPE

  CRT_UNKNOWN = 0;
  CRT_OPENGL_LEGACY = 1;
  CRT_OPENGL_4_1 = 2;
  CRT_OPENGL_ES_1_1 = 3;
  CRT_OPENGL_ES_2_0 = 4;
  CRT_DIRECT_3D_9_0c = 5;
  CRT_DIRECT_3D_11 = 6;


 // E_CORE_RENDERER_FEATURE_TYPE

  CRSF_BUILTIN_FSCREEN_MODE = 0;
  CRSF_BUILTIN_STATE_FILTER = 1;
  CRSF_MULTISAMPLING = 2;
  CRDF_VSYNC = 3;
  CRDF_PROGRAMMABLE_PIPELINE = 4;
  CRSF_BGRA_DATA_FORMAT = 5;
  CRSF_TEXTURE_COMPRESSION = 6;
  CRSF_NON_POWER_OF_TWO_TEXTURES = 7;
  CRSF_DEPTH_TEXTURES = 8;
  CRSF_TEXTURE_ANISOTROPY = 9;
  CRSF_TEXTURE_MIPMAP_GENERATION = 10;
  CRDF_TEXTURE_MIRRORED_REPEAT = 11;
  CRDF_TEXTURE_MIRROR_CLAMP = 12;
  CRDF_GEOMETRY_BUFFER = 13;


 // E_MATRIX_TYPE

  MT_PROJECTION = 0;
  MT_MODELVIEW = 1;
  MT_TEXTURE = 2;

  // E_TEXTURE_TYPE

  TT_2D = 0;
  TT_3D = 1;




 // E_CORE_RENDERER_METRIC_TYPE

  CRMT_MAX_TEXTURE_RESOLUTION = 0;
  CRMT_MAX_ANISOTROPY_LEVEL = 1;
  CRMT_MAX_LIGHTS_PER_PASS = 2;
  CRMT_MAX_TEXTURE_LAYERS = 3;


 // E_COMPARISON_FUNC

  CF_NEVER = 0;
  CF_LESS = 1;
  CF_EQUAL = 2;
  CF_LESS_EQUAL = 3;
  CF_GREATER = 4;
  CF_NOT_EQUAL = 5;
  CF_GREATER_EQUAL = 6;
  CF_ALWAYS = 7;


 // E_POLYGON_CULL_MODE

  PCM_NONE = 0;
  PCM_FRONT = 1;
  PCM_BACK = 2;


 { For future needs.
 // E_STENCIL_OPERATION
 
  SO_KEEP		= 0;
  SO_ZERO		= 1;
  SO_REPLACE	= 2;
  SO_INVERT	= 3;
  SO_INCR		= 4;
  SO_DECR		= 5;
 

 // E_BLEND_OPERATION
 
  BO_ADD		= 0;
  BO_SUBTRACT	= 1;
  BO_REV_SUBTRACT	= 2;
  BO_MIN		= 3;
  BO_MAX		= 4;
 
 }

 // E_BLEND_FACTOR

  BF_ZERO = 0;
  BF_ONE = 2;
  BF_SRC_COLOR = 3;
  BF_SRC_ALPHA = 4;
  BF_DST_COLOR = 5;
  BF_DST_ALPHA = 6;
  BF_ONE_MINUS_SRC_COLOR = 7;
  BF_ONE_MINUS_SRC_ALPHA = 8;

  { For future needs.
  BF_ONE_MINUS_DST_COLOR	= 9;
  BF_ONE_MINUS_DST_ALPHA	= 10;
  BF_SRC_ALPHA_SATURATE?	= 11;
  BF_SRC1_COLOR		= 12;
  BF_ONE_MINUS_SRC1_COLOR	= 13;
  BF_SRC1_ALPHA		= 14;
  BF_ONE_MINUS_SRC1_ALPHA	= 15;
  }


 // E_CORE_RENDERER_DATA_ALIGNMENT

  CRDA_ALIGNED_BY_4 = 0;
  CRDA_ALIGNED_BY_1 = 1;


 // E_CORE_RENDERER_BUFFER_TYPE

  CRBT_SOFTWARE = 0;
  CRBT_HARDWARE_STATIC = 1;
  CRBT_HARDWARE_DYNAMIC = 2;


 // E_CORE_RENDERER_DRAW_MODE

  CRDM_POINTS = 0;
  CRDM_LINES = 1;
  CRDM_TRIANGLES = 2;
  CRDM_LINE_STRIP = 3;
  CRDM_TRIANGLE_STRIP = 4;
  CRDM_TRIANGLE_FAN = 5;

type

  TBlendStateDesc = record

    bEnable: Boolean;

    eSrcFactor: {E_BLEND_FACTOR} Integer;
    eDestFactor: {E_BLEND_FACTOR} Integer;



  (* For future needs.
  eOperation: {E_BLEND_OPERATION} Integer;

  bSeparate: Boolean;
  eSrcAlpha: {E_BLEND_FACTOR} Integer;
  eDestAlpha: {E_BLEND_FACTOR} Integer;
  eOpAlpha : {E_BLEND_OPERATION} Integer;
    *)
{$IF CompilerVersion >= 18}
  constructor Create(Dummy: Byte);
{$IFEND}
  end;


 (* For future needs.
 TStencilFaceDesc : Record

  eStencilFailOp: 	{E_STENCIL_OPERATION} Integer;
  eStencilDepthFailOp:	{E_STENCIL_OPERATION} Integer;
  eStencilPassOp: 	{E_STENCIL_OPERATION} Integer;
  eStencilFunc: 		{E_COMPARISON_FUNC} Integer;
  end;
 *)


TDepthStencilDesc = record
  bDepthTestEnable: Boolean;
  bWriteToDepthBuffer: Boolean;
  eDepthFunc: {E_COMPARISON_FUNC} Integer;

  { For future needs.
   bStencilEnable: Boolean;
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

  eCullMode: {E_POLYGON_CULL_MODE} Integer;

  bFrontCounterClockwise: Boolean;

  bScissorEnable: Boolean;

  bAlphaTestEnable: Boolean;
  eAlphaTestFunc: {E_COMPARISON_FUNC} Integer;

  fAlphaTestRefValue: single;
    { For future needs.
  iDepthBias: Integer;
  fDepthBiasClamp: Single;
  fSlopeScaledDepthBias: Single;
  bDepthClipEnable: Boolean;
  }
{$IF CompilerVersion >= 18}
constructor Create(Dummy: Byte);
{$IFEND}
end;


IBaseRenderObjectContainer = interface(IDGLE_Base)
  ['{5C5C5973-D826-42ED-B641-A84DDDAAE2A3}']
  function GetObjectType(out eType: {E_ENG_OBJ_TYPE} Integer): DGLE_RESULT; stdcall;
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
  function GetType(out {E_TEXTURE_TYPE} eType: Integer): DGLE_RESULT; stdcall;

  function GetFormat(out eFormat: {E_TEXTURE_DATA_FORMAT} Integer): DGLE_RESULT; stdcall;
  function GetLoadFlags(out eLoadFlags: {E_TEXTURE_LOAD_FLAGS} Integer): DGLE_RESULT; stdcall;
  function GetPixelData(pData: Pointer; out uiDataSize: Cardinal; uiLodLevel: Cardinal = 0): DGLE_RESULT; stdcall;
  function SetPixelData(const pData: Pointer; uiDataSize: Cardinal; uiLodLevel: Cardinal = 0): DGLE_RESULT; stdcall;
  function Reallocate(const pData: Pointer; uiWidth: Cardinal; uiHeight: Cardinal; {E_TEXTURE_DATA_FORMAT} eDataFormat: Integer): DGLE_RESULT; stdcall;
  function GetBaseObject(out prObj: IBaseRenderObjectContainer): DGLE_RESULT; stdcall;
  function Free(): DGLE_RESULT; stdcall;
end;

ICoreGeometryBuffer = interface(IDGLE_Base)
  ['{9A77DCFF-9E4B-4716-9BBB-A316BF217F7A}']
  function GetGeometryData(out stDesc: TDrawDataDesc; uiVerticesDataSize, iIndexesDataSize: Cardinal): DGLE_RESULT; stdcall;
  function SetGeometryData(const stDesc: TDrawDataDesc; uiVerticesDataSize, uiIndexesDataSize: Cardinal): DGLE_RESULT; stdcall;
  function Reallocate(const stDesc: TDrawDataDesc; uiVerticesCount: Cardinal; uiIndexesCount: Cardinal; {E_CORE_RENDERER_DRAW_MODE} eMode: Integer): DGLE_RESULT; stdcall;
  function GetBufferDimensions(out uiVerticesDataSize, uiVerticesCount, uiIndexesDataSize, uiIndexesCount: Cardinal): DGLE_RESULT; stdcall;
  function GetBufferDrawDataDesc(out stDesc: TDrawDataDesc): DGLE_RESULT; stdcall;
  function GetBufferDrawMode(out eMode: {E_CORE_RENDERER_DRAW_MODE} Integer): DGLE_RESULT; stdcall;
  function GetBufferType(out eType: {E_CORE_RENDERER_BUFFER_TYPE} Integer): DGLE_RESULT; stdcall;
  function GetBaseObject(out prObj: IBaseRenderObjectContainer): DGLE_RESULT; stdcall;
  function Free(): DGLE_RESULT; stdcall;
end;

ICoreRenderer = interface(IEngineSubSystem)
  ['{C3B687A1-57B0-4E21-BE4C-4D92F3FAB311}']
  //Must not be called by user
  function Prepare(out stResults: TCRendererInitResult): DGLE_RESULT; stdcall;
  function Initialize(out stResults: TCRendererInitResult): DGLE_RESULT; stdcall;
  function Finalize(): DGLE_RESULT; stdcall;
  function AdjustMode(out stNewWin: TEngWindow): DGLE_RESULT; stdcall;
  //

  function MakeCurrent(): DGLE_RESULT; stdcall;
  function Present(): DGLE_RESULT; stdcall;
  function SetClearColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function Clear(bColor: Boolean = True; bDepth: Boolean = True; bStencil: Boolean = True): DGLE_RESULT; stdcall;
  function SetViewport(x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function GetViewport(out x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function SetScissor(x, y, width, height: Cardinal): DGLE_RESULT; stdcall;
  function SetLineWidth(fWidth: Single): DGLE_RESULT; stdcall;
  function SetPointSize(fSize: Single): DGLE_RESULT; stdcall;
  function ReadFrameBuffer(pData: Pointer; uiDataSize: Cardinal; eDataFormat: {E_TEXTURE_DATA_FORMAT} Integer): DGLE_RESULT; stdcall;
  function SetRenderTarget(pTexture: ICoreTexture): DGLE_RESULT; stdcall;
  function CreateTexture(out prTex: ICoreTexture; const pData: Pointer; uiWidth, uiHeight: Cardinal; bMipmapsPresented: Boolean; eDataAlignment: {E_CORE_RENDERER_DATA_ALIGNMENT} Integer; eDataFormat: {E_TEXTURE_DATA_FORMAT} Integer; eLoadFlags: {E_TEXTURE_LOAD_FLAGS} Integer): DGLE_RESULT; stdcall;
  function CreateGeometryBuffer(out prBuffer: ICoreGeometryBuffer; const stDrawDesc: TDrawDataDesc; uiVerticesCount, uiIndexesCount: Cardinal; eMode: {E_CORE_RENDERER_DRAW_MODE} Integer; eType: {E_CORE_RENDERER_BUFFER_TYPE} Integer): DGLE_RESULT; stdcall;
  function ToggleStateFilter(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function PushStates(): DGLE_RESULT; stdcall;
  function PopStates(): DGLE_RESULT; stdcall;
  function SetMatrix(const stMat: TMatrix; eMatType: {E_MATRIX_TYPE} Integer = MT_MODELVIEW): DGLE_RESULT; stdcall;
  function GetMatrix(out stMat: TMatrix; eMatType: {E_MATRIX_TYPE} Integer = MT_MODELVIEW): DGLE_RESULT; stdcall;
  function Draw(const stDrawDesc: TDrawDataDesc; eMode: {E_CORE_RENDERER_DRAW_MODE} Integer; uiCount: Cardinal): DGLE_RESULT; stdcall;
  function DrawBuffer(pBuffer: ICoreGeometryBuffer): DGLE_RESULT; stdcall;
  function SetColor(const stColor: TColor4): DGLE_RESULT; stdcall;
  function ToggleBlendState(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function ToggleAlphaTestState(bEnabled: Boolean): DGLE_RESULT; stdcall;
  function SetBlendState(const stState: TBlendStateDesc): DGLE_RESULT; stdcall;
  function GetBlendState(out stState: TBlendStateDesc): DGLE_RESULT; stdcall;
  function SetDepthStencilState(const stState: TDepthStencilDesc): DGLE_RESULT; stdcall;
  function GetDepthStencilState(out stState: TDepthStencilDesc): DGLE_RESULT; stdcall;
  function SetRasterizerState(const stState: TRasterizerStateDesc): DGLE_RESULT; stdcall;
  function GetRasterizerState(out stState: TRasterizerStateDesc): DGLE_RESULT; stdcall;
  function BindTexture(pTex: ICoreTexture; uiTextureLayer: Cardinal = 0): DGLE_RESULT; stdcall;
  function GetDeviceMetric(eMetric: {E_CORE_RENDERER_METRIC_TYPE} Integer; out iValue: Integer): DGLE_RESULT; stdcall;
  function IsFeatureSupported(eFeature: {E_CORE_RENDERER_FEATURE_TYPE} Integer; out bIsSupported: Boolean): DGLE_RESULT; stdcall;
  function GetRendererType(out eType: {E_CORE_RENDERER_TYPE} Integer): DGLE_RESULT; stdcall;
end;


function BlendStateDesc(): TBlendStateDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DepthStencilDesc(): TDepthStencilDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}
function RasterizerStateDesc(): TRasterizerStateDesc; {$IF CompilerVersion >= 18}inline;{$IFEND}

implementation


{$IF CompilerVersion >= 18}
constructor TBlendStateDesc.Create(Dummy: Byte);
begin
  Self := BlendStateDesc();
end;
{$IFEND}

function BlendStateDesc(): TBlendStateDesc;
begin
  Result.bEnable := false;
  Result.eSrcFactor := BF_SRC_ALPHA;
  Result.eDestFactor := BF_ONE_MINUS_SRC_ALPHA;
end;

{$IF CompilerVersion >= 18}
constructor TDepthStencilDesc.Create(Dummy: Byte);
begin
  Self := DepthStencilDesc();
end;
{$IFEND}

function DepthStencilDesc(): TDepthStencilDesc;
begin
  Result.bDepthTestEnable := true;
  Result.bWriteToDepthBuffer := true;
  Result.eDepthFunc := CF_LESS_EQUAL;
end;



{$IF CompilerVersion >= 18}
constructor TRasterizerStateDesc.Create(Dummy: Byte);
begin
  Self := RasterizerStateDesc();
end;
{$IFEND}

function RasterizerStateDesc(): TRasterizerStateDesc;
begin
  Result.bWireframe := false;
  Result.eCullMode := PCM_NONE;
  Result.bFrontCounterClockwise := true;
  Result.bScissorEnable := false;
  Result.bAlphaTestEnable := false;
  Result.eAlphaTestFunc := CF_GREATER;
  Result.fAlphaTestRefValue := 0.25;
end;

end.

