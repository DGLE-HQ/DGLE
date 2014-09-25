{
\file     DGLE.pas
\author   Korotkov Andrey aka DRON
\version  2:0.3.0
\date     XX.XX.2012 (c)Korotkov Andrey
\brief    Main DGLE engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE_SDK.
}

unit DGLE;

interface

{$I include.inc}

{$IFNDEF DGLE_HEADER}
{$DEFINE DGLE_HEADER}
{$ENDIF}


uses DGLE_Types, DGLE_Base, DGLE_CoreRenderer;

const
    _DGLE_VER_             = '2:0.3.0';
    _DGLE_SDK_VER_         = 1;
    _DGLE_PLUGIN_SDK_VER_  = _DGLE_SDK_VER_;

type

{Engine Base Object interface flags}

{Events Interfaces flags}

  E_EVENT_TYPE =
  (
    ET_UNKNOWN                    = 0,
    ET_BEFORE_INITIALIZATION      = 1,
    ET_BEFORE_RENDER              = 2,
    ET_AFTER_RENDER               = 3,
    ET_ON_PROFILER_DRAW           = 4,
    ET_ON_WINDOW_MESSAGE          = 5,
    ET_ON_GET_SUBSYSTEM           = 6,
    ET_ON_ENGINE_FATAL_MESSAGE    = 7,
    ET_ON_CONSOLE_WRITE           = 8,
    ET_ON_FULLSCREEN              = 9,
    ET_ON_PER_SECOND_TIMER        = 10
  );

  {Main Engine System flags}

  E_ENGINE_PROCEDURE_TYPE =
  (
    EPT_UPDATE                    = 0,
    EPT_RENDER                    = 1,
    EPT_INIT                      = 2,
    EPT_FREE                      = 3
  );

  E_LOG_TYPE =
  (
    LT_INFO                       = 0,
    LT_WARNING                    = 1,
    LT_ERROR                      = 2,
    LT_FATAL                      = 3
  );

  {Render SubSystem flags}

  E_GET_POINT3_MODE =
  (
    GP3M_FROM_DEPTH_BUFFER        = 0,
    GP3M_FROM_FAR_PLANE           = 1,
    GP3M_FROM_NEAR_PLANE          = 2
  );

  E_BLENDING_EFFECT =
  (
    BE_NORMAL                    = 0,
    BE_ADD                       = 1,
    BE_MULT                      = 2,
    BE_BLACK                     = 3,
    BE_WHITE                     = 4,
    BE_MASK                      = 5
  );

  E_BATCH_MODE2D =
  (
    BM_AUTO                       = 0,
    BM_DISABLED                   = 1,
    BM_ENABLED_UPDATE_EVERY_TICK  = 2,
    BM_ENABLED_UPDATE_EVERY_FRAME = 3
  );

  E_FIND_FLAGS =
  (
    FF_RECURSIVE                  = 1
  );

  E_FILE_SYSTEM_SEEK_FLAG =
  (
    FSSF_BEGIN                    = 0,
    FSSF_CURRENT                  = 1,
    FSSF_END                      = 2
  );

const

  //E_ENGINE_INIT_FLAGS
    EIF_DEFAULT                   = $00000000;
    EIF_CATCH_UNHANDLED           = $00000001;
    EIF_FORCE_NO_SOUND            = $00000002;
    EIF_LOAD_ALL_PLUGINS          = $00000004;
    EIF_FORCE_LIMIT_FPS           = $00000010;
    EIF_FORCE_16_BIT_COLOR        = $00000020;
    EIF_ENABLE_FLOATING_UPDATE    = $00000040;
    EIF_ENABLE_PER_FRAME_UPDATE   = $00000100;
    EIF_FORCE_NO_WINDOW           = $00000200;
    EIF_NO_SPLASH                 = $10000000;

{Resource Manager SubSystem flags}

  //E_TEXTURE_CREATION_FLAGS

    TCF_DEFAULT                   = $00000000;
    TCF_PIXEL_ALIGNMENT_1         = $00000001;  //use only if your texture input data is not 4 byte aligned
    TCF_MIPMAPS_PRESENTED         = $00000002;  //all mip levels must be presented

  //E_TEXTURE_LOAD_FLAGS
    TLF_FILTERING_NONE            = $00000001;
    TLF_FILTERING_BILINEAR        = $00000002;
    TLF_FILTERING_TRILINEAR       = $00000004;
    TLF_FILTERING_ANISOTROPIC     = $00000008;

    TLF_DECREASE_QUALITY_MEDIUM   = $00000020;
    TLF_DECREASE_QUALITY_HIGH     = $00000040;

    TLF_COMPRESS                  = $00000100;

    TLF_COORDS_REPEAT             = $00001000;
    TLF_COORDS_CLAMP              = $00002000;
    TLF_COORDS_MIRROR_REPEAT      = $00004000;
    TLF_COORDS_MIRROR_CLAMP       = $00008000;

    TLF_GENERATE_MIPMAPS          = $00040000;

    TLF_ANISOTROPY_2X             = $00100000;
    TLF_ANISOTROPY_4X             = $00200000;
    TLF_ANISOTROPY_8X             = $00400000;
    TLF_ANISOTROPY_16X            = $00800000;

  //E_MESH_CREATION_FLAGS
    MCF_ONLY_DEFAULT_DATA         = $00000000;//vertex and index arrays must be presented
    MCF_NORMALS_PRESENTED         = $00000001;
    MCF_TEXTURE_COORDS_PRESENTED  = $00000002;
    MCF_TANGENT_SPACE_PRESENTED   = $00000004;
    MCF_VERTEX_DATA_INTERLEAVED   = $00000008;

  //E_MESH_MODEL_LOAD_FLAGS
    MMLF_FORCE_SOFTWARE_BUFFER    = $00000001;
    MMLF_DYNAMIC_BUFFER           = $00000002;
    MMLF_FORCE_MODEL_TO_MESH      = $00000004;

  //E_SOUND_SAMPLE_LOAD_FLAGS
    SSLF_LOAD_AS_MUSIC            = $00000001;

  TEXTURE_LOAD_DEFAULT_2D = TLF_FILTERING_BILINEAR  or TLF_COORDS_CLAMP;
  TEXTURE_LOAD_DEFAULT_3D = TLF_FILTERING_TRILINEAR  or TLF_GENERATE_MIPMAPS or TLF_COORDS_REPEAT;

  RES_LOAD_DEFAULT                = $00000000;

{Render2D interface FLAGS}

  //E_PRIMITIVE2D_FLAGS
    PF_DEFAULT                    = $00000000;
    PF_LINE                       = $00000000;
    PF_FILL                       = $00000001;
    PF_VERTICES_COLORS            = $00000002;

  //E_EFFECT2D_FLAGS
    EF_DEFAULT                    = $00000000;
    EF_NONE                       = $00000001;
    EF_ALPHA_TEST                 = $00000002;
    EF_BLEND                      = $00000004;
    EF_FLIP_HORIZONTALLY          = $00000008;
    EF_FLIP_VERTICALLY            = $00000010;
    EF_COLOR_MIX                  = $00000020;
    EF_SCALE                      = $00000040;
    EF_VERTICES_OFFSETS           = $00000080;
    EF_VERTICES_COLORS            = $00000100;
    EF_ROTATION_POINT             = $00000200;
    EF_TILE_TEXTURE               = $00000400;

{Render3D interface FLAGS}

  //E_PUSH_STATES_FLAGS
    PSF_ALL                       = $00000000;
    PSF_MATRIX                    = $00000001;
    PSF_STATES                    = $00000002;

{Input Subsystem flags}

  //E_INPUT_CONFIGURATION_FLAGS
    ICF_DEFAULT                   = $00000000;
    ICF_EXCLUSIVE                 = $00000001;
    ICF_HIDE_CURSOR               = $00000002;
    ICF_CURSOR_BEYOND_SCREEN      = $00000004;

{SoundSample interface}

  //E_SOUND_SAMPLE_PARAMS
    SSP_NONE                      = $00000000;
    SSP_LOOPED                    = $00000001;

{FileSystem interfaces flags}

  //E_FILE_SYSTEM_OPEN_FLAGS
    FSOF_READ                     = $00000001;
    FSOF_WRITE                    = $00000002;
    FSOF_TRUNC                    = $00000004;
    FSOF_BINARY                   = $00000008;

type

//Engine Plugin interface//

  IPlugin = interface(IDGLE_Base)
  ['{B94E0E40-8885-41dd-8BC5-4CD663AE5709}']

    function GetPluginInfo(out stInfo: TPluginInfo): DGLE_RESULT; stdcall;
    function GetPluginInterfaceName(pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;

  end;

//Engine Subsystem Plugin interface//

  // Base interface of any engine core subsystem plugin.
  ISubSystemPlugin = interface(IPlugin)
  ['{27908E31-8D8E-4076-AE33-087A1BE5DCB3}']
  
    function GetSubSystemInterface(out prSubSystem: IEngineSubSystem): DGLE_RESULT; stdcall;

  end;



//Engine Base Object interface//

  IEngineBaseObject = interface(IDGLE_Base)
  ['{C010239A-6457-40f5-87EF-FAA3156CE6E2}']

    function Free(): DGLE_RESULT; stdcall; // Never use this use IResourceManager.FreeResource instead!
    function GetType(out eObjType: E_ENGINE_OBJECT_TYPE): DGLE_RESULT; stdcall;
    function GetUnknownType(out uiObjUnknownType: Cardinal): DGLE_RESULT; stdcall;

  end;

//Events Interfaces//

  IBaseEvent = interface(IDGLE_Base)
  ['{6DFEF982-AADF-42e9-A369-378BDB31404A}']

    function GetEventType(out eEvType: E_EVENT_TYPE): DGLE_RESULT; stdcall;
    function GetUnknownEventType(out uiUnknEvType: Cardinal): DGLE_RESULT; stdcall;

  end;

  IEvBeforeInitialization = interface(IBaseEvent)
  ['{EB735739-3D12-4522-B6D7-EEE3225DF934}']

    function SetParameters(const stWindowParam: TEngineWindow; eInitFlags:{E_ENGINE_INIT_FLAGS}Integer): DGLE_RESULT; stdcall;
    function GetParameters(out stWindowParam: TEngineWindow; eInitFlags:{E_ENGINE_INIT_FLAGS}Integer): DGLE_RESULT; stdcall;

  end;

  IEvConsoleWrite = interface(IBaseEvent)
  ['{9E35969A-B0D4-4E5A-A89B-1A5AAD057028}']

    function GetText(pcTxt: PAnsiChar; out uiCharsCount: Cardinal; out bToPrevLine: Boolean): DGLE_RESULT; stdcall;

  end;


  IEvFatalMessage = interface(IBaseEvent)
  ['{DAA4E3BC-C958-4def-B603-F63EEC908226}']

    function GetMessageText(pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function FreezeEngine(bFreeze: Boolean): DGLE_RESULT; stdcall;
    function ForceNoMessage(): DGLE_RESULT; stdcall;
    function ForceIgnoreError(): DGLE_RESULT; stdcall;

  end;

  IEvWindowMessage = interface(IBaseEvent)
  ['{8D718E48-581D-4cbb-9C40-C04998106F8D}']

    function GetMessage(out stWinMsg: TWindowMessage): DGLE_RESULT; stdcall;
    function SetMessage(const stWinMsg: TWindowMessage): DGLE_RESULT; stdcall;

  end;

  IEvGetSubSystem = interface(IBaseEvent)
  ['{2B6D2547-716E-490c-B1F1-422CB428738F}']

    function GetSubSystemType(out eSubSystem: E_ENGINE_SUB_SYSTEM): DGLE_RESULT; stdcall;
    function OverrideSubSystem(prSubSystem: IEngineSubSystem): DGLE_RESULT; stdcall;

  end;

  IEvGoFullScreen = interface(IBaseEvent)
  ['{CEC9184C-74D9-4739-BF48-BB800467665B}']

    function GetResolution(out uiScreenWidth, uiScreenHeight: Cardinal; out bGoFullScreen: Boolean): DGLE_RESULT; stdcall;
    function SetResolution(uiScreenWidth, uiScreenHeight: Cardinal): DGLE_RESULT; stdcall;

  end;

//Main Engine System//

  IEngineCallback = interface(IDGLE_Base)
  ['{371B1338-BB25-4B8C-BD6A-BCDF241CC52C}']

    function Initialize(): DGLE_RESULT; stdcall;
    function Free(): DGLE_RESULT; stdcall;
    function Update(uiDeltaTime: Cardinal): DGLE_RESULT; stdcall;
    function Render(): DGLE_RESULT; stdcall;
    function OnEvent(eEventType: E_EVENT_TYPE; pEvent: IBaseEvent): DGLE_RESULT; stdcall;

  end;


  TProcedure = Procedure(pParameter: Pointer); stdcall;
  TEventProcedure = Procedure(pParameter: Pointer; const prBaseEvent: IBaseEvent); stdcall;
  TStringProcedure = Procedure(pParameter: Pointer; const pcParam: PAnsiChar); stdcall;
  TStringFunction = Function(pParameter: Pointer; const pcParam: PAnsiChar): DGLE_RESULT; stdcall;
  IFileSystem = interface;
  TFSDelProcedure = Procedure(pParameter: Pointer; const prVFS: IFileSystem); stdcall;
  
  IEngineCore = interface(IDGLE_Base)
  ['{111BB884-2BA6-4e84-95A5-5E4700309CBA}']

    function LoadSplashPicture(const pcBmpFileName: PAnsiChar): DGLE_RESULT; stdcall;
    function AddPluginToInitializationList(const pcFileName: PAnsiChar): DGLE_RESULT; stdcall;
    function InitializeEngine(tHandle: TWindowHandle; const pcApplicationName: PAnsiChar; const stWindowParam: TEngineWindow; uiUpdateInterval: Cardinal = 33; eInitFlags: {E_ENGINE_INIT_FLAGS}Integer = EIF_DEFAULT): DGLE_RESULT; stdcall;
    function SetUpdateInterval(uiUpdateInterval: Cardinal): DGLE_RESULT; stdcall;  
    function StartEngine(): DGLE_RESULT; stdcall;
    function QuitEngine(): DGLE_RESULT; stdcall;

    function ConnectPlugin(const pcFileName: PAnsiChar; out prPlugin: IPlugin): DGLE_RESULT; stdcall;
    function DisconnectPlugin(pPlugin: IPlugin): DGLE_RESULT; stdcall;
    function GetPlugin(const pcPluginName: PAnsiChar; out prPlugin: IPlugin): DGLE_RESULT; stdcall;

    function AddEngineCallback(pEngineCallback: IEngineCallback): DGLE_RESULT; stdcall;
    function RemoveEngineCallback(pEngineCallback: IEngineCallback): DGLE_RESULT; stdcall;

    function AddProcedure(eProcType: E_ENGINE_PROCEDURE_TYPE; pProc: TProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
    function RemoveProcedure(eProcType: E_ENGINE_PROCEDURE_TYPE; pProc: TProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;

    function CastEvent(eEventType: E_EVENT_TYPE; pEvent: IBaseEvent): DGLE_RESULT; stdcall;
    function AddEventListener(eEventType: E_EVENT_TYPE; pListenerProc: TEventProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
    function RemoveEventListener(eEventType: E_EVENT_TYPE; pListenerProc: TEventProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
    
    function GetSubSystem(eSubSystem: E_ENGINE_SUB_SYSTEM; out prSubSystem: IEngineSubSystem): DGLE_RESULT; stdcall;

    function RenderFrame(): DGLE_RESULT; stdcall;
    function RenderProfilerText(const pcTxt: PAnsiChar; const stColor: TColor4): DGLE_RESULT; stdcall;
    function GetInstanceIndex(out uiIdx: Cardinal): DGLE_RESULT; stdcall;
    function GetTimer(out uiTick: {$IF COMPILERVERSION >= 18} UInt64 {$ELSE} Int64 {$IFEND}): DGLE_RESULT; stdcall;
    function GetSystemInfo(out stSysInfo: TSystemInfo): DGLE_RESULT; stdcall;
    function GetCurrentWindow(out stWin: TEngineWindow): DGLE_RESULT; stdcall;
    function GetFPS(out uiFPS: Cardinal): DGLE_RESULT; stdcall;
    function GetLastUpdateDeltaTime(out uiDeltaTime: Cardinal): DGLE_RESULT; stdcall;
    function GetElapsedTime(out ui64ElapsedTime: {$IF COMPILERVERSION >= 18} UInt64 {$ELSE} Int64 {$IFEND}): DGLE_RESULT; stdcall;
    function GetWindowHandle(out tHandle: TWindowHandle): DGLE_RESULT; stdcall;

    function ChangeWindowMode(const stNewWin: TEngineWindow): DGLE_RESULT; stdcall;
    function GetDesktopResolution(out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
    function AllowPause(bAllow: Boolean): DGLE_RESULT; stdcall;

    function WriteToLog(const pcTxt: PAnsiChar): DGLE_RESULT; stdcall;
    function WriteToLogEx(const pcTxt: PAnsiChar; eType: E_LOG_TYPE; const pcSrcFileName: PAnsiChar; iSrcLineNumber: Integer): DGLE_RESULT; stdcall;

    function ConsoleVisible(bIsVisible: Boolean): DGLE_RESULT; stdcall;
    function ConsoleWrite(const pcTxt: PAnsiChar; bWriteToPreviousLine: Boolean = false): DGLE_RESULT; stdcall;
    function ConsoleExecute(const pcCommandTxt: PAnsiChar): DGLE_RESULT; stdcall;
    function ConsoleRegisterCommand(const pcCommandName: PAnsiChar; const pcCommandHelp: PAnsiChar; pProc: TStringFunction; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
    function ConsoleRegisterVariable(const pcCommandName: PAnsiChar; const pcCommandHelp: PAnsiChar; var piVar :Integer; iMinValue: Integer; iMaxValue: Integer; pProc: TStringFunction; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
    function ConsoleUnregister(const pcCommandName: PAnsiChar): DGLE_RESULT; stdcall;

    function GetVersion(pcBuffer: PAnsiChar; out uiBufferSize: Cardinal): DGLE_RESULT; stdcall;

  end;

//Resource Manager SubSystem//

  IFile      = interface;
  ITexture  = interface;
  IMesh      = interface;
  ILight     = interface;
  IModel    = interface;
  IMaterial  = interface;
  ISoundSample = interface;
  ISoundChannel = interface;

  TFileLoadProcedure = Procedure(out rpFile: IFile; out prObj: IEngineBaseObject; uiLoadFlags: Cardinal; pParameter: Pointer); stdcall;

  IResourceManager = interface(IEngineSubSystem)
  ['{139505B6-5EFC-4f02-A5E8-18CD1FBD69E3}']

    function CreateTexture(out prTex: ITexture; const pData: Pointer; uiWidth, uiHeight :Cardinal; eDataFormat: E_TEXTURE_DATA_FORMAT; eCreationFlags: {E_TEXTURE_CREATION_FLAGS} Integer;
      eLoadFlags: {E_TEXTURE_LOAD_FLAGS} Integer; const pcName: PAnsiChar = nil; bAddResource: Boolean = True): DGLE_RESULT; stdcall;
    function CreateMaterial(out prMaterial: IMaterial; const pcName: PAnsiChar = nil; bAddResource: Boolean = True): DGLE_RESULT; stdcall;
    function CreateLight(out prLight: ILight; const pcName: PAnsiChar = nil; bAddResource: Boolean = True): DGLE_RESULT; stdcall;
    function CreateMesh(out prMesh: IMesh; const pubtData: Pointer; uiDataSize, uiNumVerts, uiNumFaces: Cardinal; eCreationFlags: {E_MESH_CREATION_FLAGS} Integer;
      eLoadFlags: {E_MESH_MODEL_LOAD_FLAGS} Integer; const pcName: PAnsiChar = nil; bAddResource: Boolean = True): DGLE_RESULT; stdcall;
    function CreateModel(out prModel: IModel; const pcName: PAnsiChar = nil; bAddResource: Boolean = True): DGLE_RESULT; stdcall;
    function CreateSound(out prSndSample: ISoundSample; uiSamplesPerSec, uiBitsPerSample: Cardinal; bStereo: Boolean; const pData: Pointer; ui32DataSize: Cardinal; const pcName: PAnsiChar; bAddResource: Boolean = False): DGLE_RESULT; stdcall;

    function RegisterFileFormat(const pcExtension: PAnsiChar; eObjType: E_ENGINE_OBJECT_TYPE; const pcDescription: PAnsiChar; pLoadProc: TFileLoadProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
    function UnregisterFileFormat(const pcExtension: PAnsiChar): DGLE_RESULT; stdcall;
    function RegisterDefaultResource(eObjType: E_ENGINE_OBJECT_TYPE; pObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function UnregisterDefaultResource(eObjType: E_ENGINE_OBJECT_TYPE; pObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function GetRegisteredExtensions(pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetExtensionDescription(const pcExtension: PAnsiChar; pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetExtensionType(const pcExtension: PAnsiChar; out eType: E_ENGINE_OBJECT_TYPE): DGLE_RESULT; stdcall;

    function GetResourceByName(const pcName: PAnsiChar; out prObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function GetResourceByIndex(uiIdx: Cardinal; out prObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function GetResourceName(pObj: IEngineBaseObject; pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetDefaultResource(eObjType: E_ENGINE_OBJECT_TYPE; out prObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function GetResourcesCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;

    function Load(const pcFileName: PAnsiChar; out prObj: IEngineBaseObject; uiLoadFlags: Cardinal = RES_LOAD_DEFAULT; const pcName: PAnsiChar = nil): DGLE_RESULT; stdcall;
    function LoadEx(pFile: IFile; out prObj: IEngineBaseObject; uiLoadFlags: Cardinal = RES_LOAD_DEFAULT; const pcName: PAnsiChar = nil): DGLE_RESULT; stdcall;

    function FreeResource(var prObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function AddResource(const pcName: PAnsiChar; pObj: IEngineBaseObject): DGLE_RESULT; stdcall;
    function RemoveResource(pObj: IEngineBaseObject; out bCanDelete: Boolean): DGLE_RESULT; stdcall;

  end;

//Render SubSystem//

  IRender2D = interface;
  IRender3D = interface;

  IRender = interface(IEngineSubSystem)
  ['{EA03C661-A334-4225-B5DB-4C45452CCC41}']

    function SetClearColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function GetClearColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function ClearColorBuffer(): DGLE_RESULT; stdcall;
    function Unbind(eType: E_ENGINE_OBJECT_TYPE): DGLE_RESULT; stdcall; // use EOT_UNKNOWN to unbind all
    function EnableScissor(const stArea: TRectf): DGLE_RESULT; stdcall;
    function DisableScissor(): DGLE_RESULT; stdcall;
    function GetScissor(out bEnabled: Boolean; out stArea: TRectF): DGLE_RESULT; stdcall;
    function SetRenderTarget(pTargetTex: ITexture): DGLE_RESULT; stdcall;
    function GetRenderTarget(out prTargetTex: ITexture): DGLE_RESULT; stdcall;

    function GetRender2D(out prRender2D: IRender2D): DGLE_RESULT; stdcall;
    function GetRender3D(out prRender3D: IRender3D): DGLE_RESULT; stdcall;

  end;

//Render2D interface//

  IRender2D = interface(IDGLE_Base)
  ['{F5F3257A-F8B8-4d91-BA67-451167A8D63F}']

    function Begin2D(): DGLE_RESULT; stdcall;
    function End2D(): DGLE_RESULT; stdcall;
    function BatchRender(eMode: E_BATCH_MODE2D): DGLE_RESULT; stdcall;
    function InvalidateBatchData(): DGLE_RESULT; stdcall;
    function BeginBatch(bUpdateEveryFrame: Boolean = false): DGLE_RESULT; stdcall;
    function EndBatch(): DGLE_RESULT; stdcall;
    function NeedToUpdateBatchData(out bNeedUpdate: Boolean): DGLE_RESULT; stdcall;
    function SetResolutionCorrection(uiResX, uiResY: Cardinal; bConstantProportions: Boolean = True): DGLE_RESULT; stdcall; //Set resx and resy to current screen size to turn off correction
    function ResolutionCorrectToAbsolute(const stLogicCoord, stAbsoluteCoord: TPoint2): DGLE_RESULT; stdcall;
    function AbsoluteToResolutionCorrect(const stAbsoluteCoord, stLogicCoord: TPoint2): DGLE_RESULT; stdcall;
    function SetCamera(const stCenter: TPoint2; fAngle: Single; const stScale: TVector2): DGLE_RESULT; stdcall;
    function ResetCamera(): DGLE_RESULT; stdcall;
    function UnprojectCameraToScreen(const stCameraCoord: TPoint2; out stScreenCoord: TPoint2): DGLE_RESULT; stdcall;
    function ProjectScreenToCamera(const stScreenCoord: TPoint2; out stCameraCoord: TPoint2): DGLE_RESULT; stdcall;
    function CullBoundingBox(const stBBox: TRectF; fAngle: Single; out bCull: Boolean): DGLE_RESULT; stdcall;

    // 2D Primitives
    function SetLineWidth(uiWidth: Cardinal): DGLE_RESULT; stdcall;
    function DrawPoint(const stCoords: TPoint2; const stColor: TColor4; uiSize: Cardinal = 1): DGLE_RESULT; stdcall;
    function DrawLine(const stCoords1, stCoords2: TPoint2; const stColor: TColor4; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawRectangle(const stRect: TRectf; const stColor: TColor4; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawCircle(const stCoords: TPoint2; uiRadius, uiQuality: Cardinal; const stColor: TColor4; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawEllipse(const stCoords: TPoint2; stRadius: TVector2; uiQuality: Cardinal; const stColor: TColor4; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawPolygon(pTexture: ITexture; const pstVertices: PVertex2; uiVerticesCount: Cardinal; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;

    // 2D Sprites
    function DrawTexture(pTexture: ITexture; const stCoords: TPoint2; const stDimensions: TVector2; fAngle: Single = 0.; eFlags: {E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawTextureCropped(pTexture: ITexture; const stCoords: TPoint2; const stDimensions: TVector2; const stTexCropRect: TRectf; fAngle: Single = 0.; eFlags: {E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawTextureSprite(pTexture: ITexture; const stCoords: TPoint2; const stDimensions: TVector2; uiFrameIndex: Cardinal; fAngle: Single = 0.; eFlags: {E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;

    // Extra
    function DrawTriangles(pTexture: ITexture; const pstVertices: PVertex2; uiVerticesCount: Cardinal; eFlags: {E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawMesh(pMesh: IMesh; pTexture: ITexture; const stCoords: TPoint2; const stDimensions: TVector3; const stAxis: TVector3; fAngle: Single = 0; eFlags: {E_EFFECT2D_FLAGS}Integer = EF_DEFAULT;
      bClip: Boolean = True; fFovY: Single = 90.0; bClearDepthBuffer: Boolean = False): DGLE_RESULT; stdcall;

    //Advanced
    function Draw(pTexture: ITexture; const stDrawDesc: TDrawDataDesc; eMode: E_CORE_RENDERER_DRAW_MODE; uiCount: Cardinal; const stAABB: TRectF; eFlags: {E_EFFECT2D_FLAGS}Integer): DGLE_RESULT; stdcall;
    function DrawBuffer(pTexture: ITexture; pBuffer: ICoreGeometryBuffer; const stAABB: TRectF; eFlags: {E_EFFECT2D_FLAGS}Integer): DGLE_RESULT; stdcall;
    function DrawBuffer3D(pTexture: ITexture; pBuffer: ICoreGeometryBuffer; eFlags: {E_EFFECT2D_FLAGS} Integer; const stTransform: TMatrix4x4;
      const stCenter: TPoint3; const stExtents: TVector3; bClip: Boolean; fFovY: Single; bClearDepthBuffer: Boolean): DGLE_RESULT; stdcall;

    // Effects
    function SetRotationPoint(const stCoords: TPoint2): DGLE_RESULT; stdcall; //In texture stCoords system
    function SetScale(const stScale: TPoint2): DGLE_RESULT; stdcall;
    function SetColorMix(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetBlendMode(eMode: E_BLENDING_EFFECT = BE_NORMAL): DGLE_RESULT; stdcall;
    function SetVerticesOffsets(const stCoords1, stCoords2, stCoords3, stCoords4: TPoint2): DGLE_RESULT; stdcall;
    function SetVerticesColors(const stColor1,  stColor2,  stColor3,  stColor4 : TColor4): DGLE_RESULT; stdcall;

    function GetRotationPoint(out stCoords: TPoint2): DGLE_RESULT; stdcall;
    function GetScale(out stScale: TPoint2): DGLE_RESULT; stdcall;
    function GetColorMix(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetBlendMode(out eMode: E_BLENDING_EFFECT): DGLE_RESULT; stdcall;
    function GetVerticesOffsets(out stCoords1, stCoords2, stCoords3, stCoords4: TPoint2): DGLE_RESULT; stdcall;
    function GetVerticesColors(out stColor1, stColor2, stColor3, stColor4: TColor4): DGLE_RESULT; stdcall;

  end;

//Render3D interface//

  IRender3D = interface(IDGLE_Base)
  ['{5275F43A-4FF9-48b2-B88E-B2F842461AB3}']

    function SetPerspective(fFovAngle, fZNear, fZFar: Single): DGLE_RESULT; stdcall;
    function GetPerspective(out fFovAngle, fZNear, fZFar: Single): DGLE_RESULT; stdcall;

    function SetColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function GetColor(out stColor: TColor4): DGLE_RESULT; stdcall;

    function BindTexture(pTex: ITexture; uiTextureLayer: Cardinal): DGLE_RESULT; stdcall;
    function GetTexture(out prTex: ITexture; uiTextureLayer: Cardinal): DGLE_RESULT; stdcall;

    function GetMaxLightsPerPassCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
    function UpdateLight(pLight: ILight): DGLE_RESULT; stdcall;

    function BindMaterial(pMat: IMaterial): DGLE_RESULT; stdcall;
    function GetMaterial(out prMat: IMaterial): DGLE_RESULT; stdcall;

    function ToggleBlending(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function IsBlendingEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetBlendMode(eMode: E_BLENDING_EFFECT = BE_NORMAL): DGLE_RESULT; stdcall;
    function GetBlendMode(out eMode: E_BLENDING_EFFECT): DGLE_RESULT; stdcall;

    function ToggleAlphaTest(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetAlphaTreshold(fTreshold: Single): DGLE_RESULT; stdcall;
    function IsAlphaTestEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
    function GetAlphaTreshold(out fTreshold: Single): DGLE_RESULT; stdcall;

    function ClearDepthBuffer(): DGLE_RESULT; stdcall;
    function ToggleDepthTest(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function IsDepthTestEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;

    function ToggleBackfaceCulling(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function IsBackfaceCullingEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;

    function Draw(const stDrawDesc: TDrawDataDesc; eMode: E_CORE_RENDERER_DRAW_MODE; uiCount: Cardinal): DGLE_RESULT; stdcall;
    function DrawBuffer(pBuffer: ICoreGeometryBuffer): DGLE_RESULT; stdcall;

    function ToggleFog(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetLinearFogBounds(fStart, fEnd: Single): DGLE_RESULT; stdcall;
    function SetFogColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetFogDensity(fDensity: Single): DGLE_RESULT; stdcall;
    function IsFogEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
    function GetLinearFogBounds(out fStart, fEnd: Single): DGLE_RESULT; stdcall;
    function GetFogColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetFogDensity(out fDensity: Single): DGLE_RESULT; stdcall;

    function SetMatrix(const stMatrix: TMatrix4x4): DGLE_RESULT; stdcall;
    function MultMatrix(const stMatrix: TMatrix4x4): DGLE_RESULT; stdcall;
    function PushMatrix(): DGLE_RESULT; stdcall;
    function PopMatrix(): DGLE_RESULT; stdcall;
    function GetMatrix(out stMatrix: TMatrix4x4): DGLE_RESULT; stdcall;

    function DrawAxes(fSize: Single = 1.0; bNoDepthTest: Boolean = False): DGLE_RESULT; stdcall;

    function ResetStates(): DGLE_RESULT; stdcall;
    function PushStates(): DGLE_RESULT; stdcall;
    function PopStates(): DGLE_RESULT; stdcall;

    function GetPoint3(const stPointOnScreen: TPoint2; out stResultPoint: TPoint3; eFlag: E_GET_POINT3_MODE = GP3M_FROM_DEPTH_BUFFER): DGLE_RESULT; stdcall;
    function GetPoint2(const stPoint: TPoint3; out stResultPointOnScreen: TPoint2): DGLE_RESULT; stdcall;

    function SetupFrustum(): DGLE_RESULT; stdcall;
    function CullPoint(const stCoords: TPoint3; out bCull: Boolean): DGLE_RESULT; stdcall;
    function CullSphere(const stCenter: TPoint3; fRadius: Single; out bCull: Boolean): DGLE_RESULT; stdcall;
    function CullBox(const stCenterCoords, stExtents: TVector3; out bCull: Boolean): DGLE_RESULT; stdcall;

    function ToggleLighting(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetGlobalAmbientLighting(const stColor: TColor4): DGLE_RESULT; stdcall;
    function IsLightingEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
    function GetGlobalAmbientLighting(out stColor: TColor4): DGLE_RESULT; stdcall;

  end;

//Light interface//

  ILight = interface(IEngineBaseObject)
  ['{EB73AC84-A465-4554-994D-8BED29744C9D}']

    function SetEnabled(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetPosition(const stPos: TPoint3): DGLE_RESULT; stdcall;
    function SetDirection(const stDir: TVector3): DGLE_RESULT; stdcall;
    function SetRange(fRange: Single): DGLE_RESULT; stdcall;
    function SetIntensity(fIntensity: Single): DGLE_RESULT; stdcall;
    function SetSpotAngle(fAngle: Single): DGLE_RESULT; stdcall;
    function SetType(eType: E_LIGHT_TYPE): DGLE_RESULT; stdcall;

    function GetEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
    function GetColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetPosition(out stPos: TPoint3): DGLE_RESULT; stdcall;
    function GetDirection(out stDir: TVector3): DGLE_RESULT; stdcall;
    function GetRange(out fRange: Single): DGLE_RESULT; stdcall;
    function GetIntensity(out fIntensity: Single): DGLE_RESULT; stdcall;
    function GetSpotAngle(out fAngle: Single): DGLE_RESULT; stdcall;
    function GetType(out eType: E_LIGHT_TYPE): DGLE_RESULT; stdcall;

    function Update(): DGLE_RESULT; stdcall;

  end;

//Texture interface//

  ITexture = interface(IEngineBaseObject)
  ['{85BDDBC2-F126-4cae-946D-7D6B079E5CCE}']

    function GetDimensions(out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
    function SetFrameSize(uiFrameWidth, uiFrameHeight: Cardinal): DGLE_RESULT; stdcall;
    function GetFrameSize(out uiFrameWidth, uiFrameHeight: Cardinal): DGLE_RESULT; stdcall;
    function FramesCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;

    function GetCoreTexture(out prCoreTex: ICoreTexture): DGLE_RESULT; stdcall;

    function Draw2DSimple(iX, iY: Integer; uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;
    function Draw2D(iX, iY: Integer; uiWidth, uiHeight: Cardinal; fAngle: Single = 0.0; uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;
    function Draw3D(uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;

    function Bind(uiTextureLayer: Cardinal = 0): DGLE_RESULT; stdcall;

  end;

//Material interface//

  IMaterial = interface(IEngineBaseObject)
  ['{B6506749-BB41-423d-B6C0-982081EF63F9}']

    function SetDiffuseColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetSpecularColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetShininess(fShininess: Single): DGLE_RESULT; stdcall;
    function SetDiffuseTexture(pTexture: ITexture): DGLE_RESULT; stdcall;
    function SetBlending(bEnabled: Boolean; eMode: E_BLENDING_EFFECT): DGLE_RESULT; stdcall;
    function SetAlphaTest(bEnabled: Boolean; fTreshold: Single): DGLE_RESULT; stdcall;

    function GetDiffuseColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetSpecularColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetShininess(out fShininess: Single): DGLE_RESULT; stdcall;
    function GetDiffuseTexture(out prTexture: ITexture): DGLE_RESULT; stdcall;
    function GetBlending(out bEnabled: Boolean; out eMode: E_BLENDING_EFFECT): DGLE_RESULT; stdcall;
    function GetAlphaTest(out bEnabled: Boolean; out fTreshold: Single): DGLE_RESULT; stdcall;

    function Bind(): DGLE_RESULT; stdcall;

  end;

  //BitmapFont interface//

  IBitmapFont = interface(IEngineBaseObject)
  ['{0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA}']

    function GetTexture(out prTexture: ITexture): DGLE_RESULT; stdcall;
    function SetScale(fScale: Single): DGLE_RESULT; stdcall;
    function GetScale(out fScale: Single): DGLE_RESULT; stdcall;
    function GetTextDimensions(const pcTxt: PAnsiChar; out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
    function Draw2DSimple(iX, iY: Integer; const pcTxt: PAnsiChar; const stColor: TColor4): DGLE_RESULT; stdcall;
    function Draw2D(fX, fY: Single; const pcTxt: PAnsiChar; const stColor: TColor4; fAngle: Single = 0; bVerticesColors: Boolean = False): DGLE_RESULT; stdcall;
    function Draw3D(const pcTxt: PAnsiChar): DGLE_RESULT; stdcall;

  end;

//3D Objects interfaces//

//Mesh interface//

  IMesh = interface(IEngineBaseObject)
  ['{85E360A8-07B3-4f22-AA29-07C7FC7C6893}']

    function Draw(): DGLE_RESULT; stdcall;
    function GetCenter(out stCenter: TPoint3): DGLE_RESULT; stdcall;
    function GetExtents(out stExtents: TVector3): DGLE_RESULT; stdcall;
    function GetTrianglesCount(out uiCnt: Cardinal): DGLE_RESULT; stdcall;
    function GetGeometryBuffer(out prBuffer:ICoreGeometryBuffer): DGLE_RESULT; stdcall;
    function SetGeometryBuffer(pBuffer: ICoreGeometryBuffer; bFreeCurrentBuffer: Boolean): DGLE_RESULT; stdcall;
    function RecalculateNormals(bInvert: Boolean = False): DGLE_RESULT; stdcall;
    function RecalculateTangentSpace(): DGLE_RESULT; stdcall;
    function RecalculateBounds(): DGLE_RESULT; stdcall;
    function TransformVertices(const stTransMatrix: TMatrix4x4): DGLE_RESULT; stdcall;
    function Optimize(): DGLE_RESULT; stdcall;
    function GetOwner(out prModel: IModel): DGLE_RESULT; stdcall;
    function SetOwner(pModel: IModel): DGLE_RESULT; stdcall;

  end;

//Multi mesh interface//

  IModel = interface(IEngineBaseObject)
  ['{6107C296-FC07-48d1-B6A7-F88CC2DAE897}']

    function Draw(): DGLE_RESULT; stdcall;
    function DrawMesh(uiMeshIdx: Cardinal): DGLE_RESULT; stdcall;
    function GetCenter(out stCenter: TPoint3): DGLE_RESULT; stdcall;
    function GetExtents(out stExtents: TVector3): DGLE_RESULT; stdcall;
    function MeshesCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
    function GetMesh(uiMeshIdx: Cardinal; out prMesh: IMesh): DGLE_RESULT; stdcall;
    function SetModelMaterial(pMaterial: IMaterial): DGLE_RESULT; stdcall;
    function GetModelMaterial(out prMaterial: IMaterial): DGLE_RESULT; stdcall;
    function SetMeshMaterial(uiMeshIdx: Cardinal; pMaterial: IMaterial): DGLE_RESULT; stdcall;
    function GetMeshMaterial(uiMeshIdx: Cardinal; out prMaterial: IMaterial): DGLE_RESULT; stdcall;
    function AddMesh(pMesh: IMesh): DGLE_RESULT; stdcall;
    function RemoveMesh(pMesh: IMesh): DGLE_RESULT; stdcall;
    function ReplaceMesh(uiMeshIdx: Cardinal; pMesh: IMesh): DGLE_RESULT; stdcall;
    
  end;

//Input Subsystem//

  IInput = interface(IEngineSubSystem)
  ['{64DAAF7F-F92C-425f-8B92-3BE40D8C6666}']

    function Configure(eFlags: {E_INPUT_CONFIGURATION_FLAGS}Integer = ICF_DEFAULT): DGLE_RESULT; stdcall;

    function GetMouseStates(out stMStates: TMouseStates): DGLE_RESULT; stdcall;
    function GetKey(eKeyCode: E_KEYBOARD_KEY_CODES; out bPressed: Boolean): DGLE_RESULT; stdcall;
    function GetKeyName(eKeyCode: E_KEYBOARD_KEY_CODES; out cASCIICode: Char): DGLE_RESULT; stdcall;

    function BeginTextInput(pcBuffer: PAnsiChar; uiBufferSize: Cardinal): DGLE_RESULT; stdcall;
    function EndTextInput(): DGLE_RESULT; stdcall;

    function GetJoysticksCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
    function GetJoystickName(uiJoyId: Cardinal; pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetJoystickStates(uiJoyId: Cardinal; out stJoyStates: TJoystickStates): DGLE_RESULT; stdcall;

  end;

StreamCallbackFunc = function(pParameter: Pointer; ui32DataPos: Cardinal; pBufferData: Pointer; uiBufferSize: Cardinal): DGLE_RESULT; stdcall;

//Sound SubSystem interfaces//

  ISound = interface(IEngineSubSystem)
  ['{054C07EE-2724-42f2-AC2B-E81FCF5B4ADA}']

    function SetMasterVolume(uiVolume: Cardinal): DGLE_RESULT; stdcall;
    function MasterPause(bPaused: Boolean): DGLE_RESULT; stdcall;
    function StopAllChannels(): DGLE_RESULT; stdcall;
    function GetMaxChannelsCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
    function GetFreeChannelsCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
    function ReleaseChannelsByData(const pData: Pointer): DGLE_RESULT; stdcall;
    function CreateChannel(out prSndChnl: ISoundChannel; uiSamplesPerSec, uiBitsPerSample: Cardinal;
      bStereo: Boolean; const pData: Pointer; ui32DataSize: Cardinal): DGLE_RESULT; stdcall; //Data not copied!
    function CreateStreamableChannel(out prSndChnl: ISoundChannel; uiSamplesPerSec, uiBitsPerSample: Cardinal; bStereo: Boolean;
      ui32DataSize: Cardinal; pStreamCallback: StreamCallbackFunc; pParameter: Pointer): DGLE_RESULT; stdcall;

  end;

  // Sound Sample Interface

  ISoundChannel = interface(IDGLE_Base)
  ['{DE6F7CDD-8262-445c-8D20-68E3324D99A6}']

    function Play(bLooped: Boolean): DGLE_RESULT; stdcall;
    function Pause(): DGLE_RESULT; stdcall;
    function Stop(): DGLE_RESULT; stdcall;
    function IsPlaying(out bIsPlaying: Boolean): DGLE_RESULT; stdcall;
    function SetVolume(uiVolume: Cardinal): DGLE_RESULT; stdcall; //from 0 to 100
    function GetVolume(out uiVolume: Cardinal): DGLE_RESULT; stdcall;
    function SetPan(iPan: Integer): DGLE_RESULT; stdcall; //from -100 to 100
    function GetPan(out iPan: Integer): DGLE_RESULT; stdcall;
    function SetSpeed(uiSpeed: Cardinal): DGLE_RESULT; stdcall;//in percents
    function GetSpeed(out uiSpeed: Cardinal): DGLE_RESULT; stdcall;
    function SetCurrentPosition(uiPos: Cardinal): DGLE_RESULT; stdcall;
    function GetCurrentPosition(out uiPos: Cardinal): DGLE_RESULT; stdcall;
    function GetLength(out uiLength: Cardinal): DGLE_RESULT; stdcall;
    function IsStreamable(out bStreamable: Boolean): DGLE_RESULT; stdcall;
    function Unaquire(): DGLE_RESULT; stdcall;

  end;

//SoundSample interface//

  ISoundSample = interface(IEngineBaseObject)
  ['{30DD8C94-D3FA-40cf-9C49-649211424919}']

    function Play(): DGLE_RESULT; stdcall;
    function PlayEx(out pSndChnl: ISoundChannel; eFlags: {E_SOUND_SAMPLE_PARAMS}Integer = SSP_NONE): DGLE_RESULT; stdcall; //pSndChnl must be checked on nul

  end;

//Music interface//

  IMusic = interface(IEngineBaseObject)
  ['{81F1E67B-3FEB-4ab1-9AD2-D27C4E662164}']

    function Play(bLooped : Boolean = True): DGLE_RESULT; stdcall;
    function Pause(bPaused : Boolean): DGLE_RESULT; stdcall;
    function Stop(): DGLE_RESULT; stdcall;
    function IsPlaying(out bIsPlaying : Boolean): DGLE_RESULT; stdcall;
    function SetVolume(uiVolume : Cardinal): DGLE_RESULT; stdcall;
    function GetVolume(out uiVolume : Cardinal): DGLE_RESULT; stdcall;
    function SetCurrentPosition(uiPos : Cardinal): DGLE_RESULT; stdcall;
    function GetCurrentPosition(out uiPos : Cardinal): DGLE_RESULT; stdcall;
    function GetLength(out uiLength : Cardinal): DGLE_RESULT; stdcall;

  end;

//FileSystem Subsystem//

  IMainFileSystem = interface(IEngineSubSystem)
  ['{4850286F-4770-4bcf-A90A-33D7BE41E686}']

    function LoadFile(const pcFileName: PAnsiChar; out prFile: IFile): DGLE_RESULT; stdcall;// c:\data.zip|img.jpg
    function FreeFile(var prFile: IFile): DGLE_RESULT; stdcall;
    function GetVirtualFileSystem(const pcVFSExtension: PAnsiChar{NULL to get HDD file system}; out prVFS: IFileSystem): DGLE_RESULT; stdcall;
    function RegisterVirtualFileSystem(const pcVFSExtension: PAnsiChar; const pcDescription: PAnsiChar; pVFS: IFileSystem; pDeleteCallback: TFSDelProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
    function UnregisterVirtualFileSystem(const pcVFSExtension: PAnsiChar): DGLE_RESULT; stdcall;
    function GetRegisteredVirtualFileSystems(pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetVirtualFileSystemDescription(const pcVFSExtension: PAnsiChar; pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;

  end;

//File interface//

  IFile = interface(IDGLE_Base)
  ['{AE6E8AE7-3E5B-4bc4-A512-42E1CF1DF005}']

    function Read(pBuffer: Pointer; uiCount: Cardinal; out uiRead: Cardinal): DGLE_RESULT; stdcall;
    function Write(const pBuffer: Pointer; uiCount: Cardinal; out uiWritten: Cardinal): DGLE_RESULT; stdcall;
    function Seek(ui32Offset: Cardinal; eWay: E_FILE_SYSTEM_SEEK_FLAG; out ui32Position: Cardinal): DGLE_RESULT; stdcall;
    function GetSize(out ui32Size: Cardinal): DGLE_RESULT; stdcall;
    function IsOpen(out bOpened: Boolean): DGLE_RESULT; stdcall;
    function GetName(pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function GetPath(pcPath: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function Free(): DGLE_RESULT; stdcall; // Never use this use  IMainFileSystem.FreeFile instead!

  end;

//FileIterator interface//

  IFileIterator = interface(IDGLE_Base)
  ['{5D73F249-0E74-4cc5-9646-270CB1E22750}']

    function FileName(pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
    function Next(): DGLE_RESULT; stdcall;
    function Free(): DGLE_RESULT; stdcall;

  end;

//FileSystem interface//

  IFileSystem = interface(IDGLE_Base)
  ['{2DAE578E-9636-4fae-BABB-7D835EEA7518}']

    function OpenFile(const pcName: PAnsiChar; eFlags: {E_FILE_SYSTEM_OPEN_FLAGS}Integer; out prFile: IFile): DGLE_RESULT; stdcall;
    function DeleteFile(const pcName: PAnsiChar): DGLE_RESULT; stdcall;
    function FileExists(const pcName: PAnsiChar; out bExists: Boolean): DGLE_RESULT; stdcall;
    function Find(const pcMask: PAnsiChar; eFlags: E_FIND_FLAGS; out prIterator: IFileIterator): DGLE_RESULT; stdcall;

  end;

const
  //E_GET_ENGINE_FLAGS
  GEF_DEFAULT              = $00000000;
  GEF_FORCE_SINGLE_THREAD  = $00000001;
  GEF_FORCE_NO_LOG_FILE    = $00000002;
  GEF_FORCE_QUIT           = $00000004;

var
  hServer      : THandle = 0;
  pCreateEngine  : Function(out pEngineCore: IEngineCore; eFlags: {E_GET_ENGINE_FLAGS}Integer; ubtSDKVer: byte): Boolean; stdcall;
  pFreeEngine    : Function(pEngineCore: IEngineCore): Boolean; stdcall;

function CreateEngine(out pEngineCore: IEngineCore; eFlags: {E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
function GetEngine(pcDllFileName: AnsiString; out pEngineCore :IEngineCore; eFlags: {E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
function FreeEngine(pEngineCore: IEngineCore = nil; bFreeDLL: Boolean = true): Boolean;

implementation

uses
  Windows;

function CreateEngine(out pEngineCore: IEngineCore; eFlags: {E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
begin
  Result := false;
  if @pCreateEngine = nil then Exit;
  Result := pCreateEngine(pEngineCore, eFlags, _DGLE_SDK_VER_);
end;

function FreeEngine(pEngineCore: IEngineCore = nil; bFreeDLL: Boolean = true): Boolean;
begin
  if pEngineCore <> nil then pFreeEngine(pEngineCore);
  if bFreeDLL and(hServer <> 0) then
  begin
    FreeLibrary(hServer);
    hServer := 0;
    pCreateEngine := nil;
    pFreeEngine := nil;
  end;
  Result := true;  
end;

function GetEngine(pcDllFileName: AnsiString; out pEngineCore: IEngineCore; eFlags: {E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
begin
  Result := false;
  if hServer = 0 then
  begin
    pEngineCore  := nil;
    
    if hServer = 0 then
    begin
      hServer := LoadLibraryA(PAnsiChar(pcDllFileName));
      if hServer = 0 then Exit;
    end;
    
    if (@pCreateEngine = nil)and(@pFreeEngine = nil) then
    begin
      pCreateEngine := GetProcAddress(hServer,'CreateEngine');
      pFreeEngine := GetProcAddress(hServer,'FreeEngine');
      if (@pCreateEngine = nil)or(@pFreeEngine = nil) then
      begin
        FreeLibrary(hServer);
        hServer := 0;
      end;
    end;
  end;
  if hServer <> 0 then Result := CreateEngine(pEngineCore, eFlags);  
end;

begin
  Set8087CW($133F);
  IsMultiThread := True;
end.
