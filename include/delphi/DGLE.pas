{
\file		DGLE.pas
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		XX.XX.2012 (c)Korotkov Andrey
\brief		Main DGLE engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE_SDK.
}

unit DGLE;

interface

{$IFDEF FPC}
 {$MODE DELPHI}
 {$MACRO ON}
 {$DEFINE COMPILERVERSION := 12} // assume FPC is like Delphi4
{$ENDIF}

{$IFNDEF DGLE_HEADER}
{$DEFINE DGLE_HEADER}
{$ENDIF}


uses Windows, DGLE_types, DGLE_Base, DGLE_CoreRenderer;

const
		_DGLE_VER_						= '2:0.3.0';
		_DGLE_SDK_VER_				= 1;
		_DGLE_PLUGIN_SDK_VER_	= _DGLE_SDK_VER_;

{Engine Base Object interface flags}

  //E_ENG_OBJ_TYPE
		EOT_UNKNOWN 					= 0;
		EOT_TEXTURE						= 1;
		EOT_MATERIAL					= 2;
		EOT_MESH						  = 3;
		EOT_MULTI_MESH 			  = 4;
		EOT_BITMAP_FONT				= 5;
		EOT_SOUND_SAMPLE			= 6;
		EOT_MUSIC					  	= 7;

		EOT_EMPTY             = 8;

{Events Interfaces flags}

  //E_EVENT_TYPE
		ET_UNKNOWN						     = 0;
		ET_BEFORE_INIT				  	 = 1;
		ET_BEFORE_RENDER			  	 = 2;
		ET_AFTER_RENDER				  	 = 3;
		ET_ON_PROFILER_DRAW		  	 = 4;
		ET_ON_WIN_MESSAGE			  	 = 5;
		ET_ON_GET_SSYSTEM			  	 = 6;
		ET_ON_ENGINE_FATAL_MESSAGE = 7;
 		ET_ON_CONSOLE_WRITE		  	 = 8;
    ET_ON_FULLSCREEN           = 9;


 {Main Engine System flags}

	//E_ENGINE_PROCEDURE_TYPE
		EPT_UPDATE					= 0;
		EPT_RENDER					= 1;
		EPT_INIT 						= 2;
		EPT_FREE 						= 3;

	//E_LOG_TYPE
		LT_INFO							= 0;
		LT_WARNING					= 1;
		LT_ERROR						= 2;
		LT_FATAL						= 3;

	//E_ENGINE_INIT_FLAGS
		EIF_DEFAULT						    = $00000000;
		EIF_CATCH_UNHANDLED				= $00000001;
		EIF_FORCE_NO_SOUND				= $00000002;
 		EIF_LOAD_ALL_PLUGINS			= $00000004;
		EIF_FORCE_LIMIT_FPS				= $00000010;
		EIF_FORCE_16_BIT_COLOR		= $00000020;
		EIF_DISABLE_SMART_TIMING	= $00000040;
    EIF_FORCE_NO_WINDOW		    = $00000100;
		EIF_NO_SPLASH					    = $10000000;

{Resource Manager SubSystem flags}

	// E_TEXTURE_DATA_FORMAT
	
		TDF_RGB8 			= 0;
		TDF_RGBA8 		= 1;
		TDF_ALPHA8 		= 2;
		TDF_BGR8 			= 3;
		TDF_BGRA8 		= 4;
		TDF_DXT1 			= 5;
		TDF_DXT5 			= 6;
		TDF_DEPTH_COMPONENT24 		= 7;
	

	//E_TEXTURE_CREATION_FLAGS
		
		TCF_DEFAULT			       	      = $00000000;
		TCF_PIXEL_ALIGNMENT_1			    = $00000001;	//use only if your texture input data is not 4 byte aligned
		TCF_MIPMAPS_PRESENTED			    = $00000002;	//all mip levels must be presented

  //E_TEXTURE_LOAD_FLAGS
		TLF_FILTERING_NONE		 	  	  = $00000001;
		TLF_FILTERING_BILINEAR		  	= $00000002;
		TLF_FILTERING_TRILINEAR		  	= $00000004;
		TLF_FILTERING_ANISOTROPIC	  	= $00000008;

		TLF_DECREASE_QUALITY_MEDIUM 	= $00000020;
		TLX_DECREASE_QUALITY_HIGH	  	= $00000040;

		TLX_COMPRESS					        = $00000100;

		TLF_COORDS_REPEAT	 		    	  = $00001000;
		TLF_COORDS_CLAMP			    	  = $00002000;
		TLF_COORDS_MIRROR_REPEAT	  	= $00004000;
		TLF_COORDS_MIRROR_CLAMP		  	= $00008000;

		TLF_GENERATE_MIPMAPS		     	= $00040000;

		TLF_ANISOTROPY_2X			       	= $00100000;
		TLF_ANISOTROPY_4X			      	= $00200000;
		TLF_ANISOTROPY_8X			      	= $00400000;
		TLF_ANISOTROPY_16X		 	    	= $00800000;

	//E_MESH_CREATION_FLAGS
		MCF_ONLY_DEFAULT_DATA	 	 	    = $00000000;//vertex and index arrays must be presented
		MCF_NORMALS_PRESENTED		     	= $00000001;
		MCF_TEXTURE_COORDS_PRESENTED	= $00000002;
		MCF_TANGENT_SPACE_PRESENTED		= $00000004;

	//E_MESH_LOAD_FLAGS
	  MLF_FORCE_NO_HARDWARE_BUFFER  = $00000001;
    MLF_CALCULATE_TANGENT_SPACE   = $00000002;
    MLF_CALCULATE_NORMALS         = $00000004;

  //E_SOUND_SAMPLE_LOAD_FLAGS
    SSLF_LOAD_AS_MUSIC            = $00000001;



	TEXTURE_LOAD_DEFAULT_2D = TLF_FILTERING_BILINEAR	or TLF_COORDS_CLAMP;
	TEXTURE_LOAD_DEFAULT_3D = TLF_FILTERING_TRILINEAR	or TLF_GENERATE_MIPMAPS or TLF_COORDS_REPEAT;

	RES_LOAD_DEFAULT 				= $00000000;

{Render SubSystem flags}

	//E_GET_POINT3_FLAG
		GP3F_FROM_Z_BUFFER		= $00000000;
		GP3F_FROM_FAR_PLANE		= $00000001;
		GP3F_FROM_NEAR_PLANE	= $00000002;


{Render2D interface FLAGS}

	//E_PRIMITIVE2D_FLAGS
		PF_DEFAULT	 	   	= $00000000;
		PF_LINE			     	= $00000000;
		PF_FILL			     	= $00000001;
		PF_VERTICES_COLOR = $00000002;
    PF_SMOOTH       	= $00000004;


	//E_EFFECT2D_FLAGS
		EF_DEFAULT		    	= $00000000;
		EF_NONE			     	  = $00000001;
		EF_ALPHA_TEST	    	= $00000002;
		EF_BLEND	 	    	  = $00000004;
		EF_FLIPX		     	  = $00000008;
		EF_FLIPY			      = $00000010;
		EF_COLORMIX		    	= $00000020;
		EF_SCALE		     	  = $00000040;
		EF_VERTICES_OFFSET 	= $00000080;
		EF_VERTICES_COLOR  	= $00000100;
		EF_ROTATEPT		   	  = $00000200;


	//E_EFFECT_BLENDING_FLAGS
		EBF_NORMAL				  	= $00000001;
		EBF_ADD						 	  = $00000002;
		EBF_MULT					 	  = $00000004;
		EBF_BLACK					 	  = $00000008;
		EBF_WHITE					 	  = $00000010;
		EBF_MASK					 	  = $00000020;

	//E_BATCH_MODE2D
 		BM_AUTO						 	  = 0;
		BM_DISABLED						= 1;
		BM_ENABLED_UPDATE_EVERY_TICK = 2;
		BM_ENABLED_UPDATE_EVERY_FRAME = 3;


{Render3D interface FLAGS}

	//E_PUSH_STATES_FLAGS
		PSF_ALL						 	  = $00000000;
		PSF_MATRIX						= $00000001;
		PSF_STATES						= $00000002;


	//E_LIGHT_TYPE
		LT_DIRECTIONAL			 	= 0;
		LT_POINT						  = 1;
    LT_SPOT						    = 2;


{Input Subsystem flags}

	//E_INPUT_CONFIGURATION_FLAGS
		ICF_DEFAULT					  	= $00000000;
		ICF_EXCLUSIVE				  	= $00000001;
		ICF_HIDE_CURSOR			 		= $00000002;
		ICF_CURSOR_BEYOND_SCREEN 	= $00000004;

{SoundSample interface}

  //E_SOUND_SAMPLE_PARAMS
		SSP_NONE					   	= $00000000;
		SSP_LOOPED				   	= $00000001;

{FileSystem interfaces flags}

	//E_FIND_FLAGS
		FF_RECURSIVE			  		= 1;

	//E_FILE_SYSTEM_OPEN_FLAGS
		FSOF_READ					   	= $00000001;
		FSOF_WRITE			   		= $00000002;
		FSOF_TRUNC						  = $00000004;
		FSOF_BINARY					  	= $00000008;

	//E_FILE_SYSTEM_SEEK_FLAG
		FSSF_BEGIN					  	= 0;
		FSSF_CURRENT			  		= 1;
		FSSF_END					   	= 2;

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

	IEngBaseObj = interface(IDGLE_Base)
	['{C010239A-6457-40f5-87EF-FAA3156CE6E2}']

		function Free(): DGLE_RESULT; stdcall;
		function GetType(out eObjType :{ENG_OBJ_TYPE}Integer): DGLE_RESULT; stdcall;
		function GetUnknownType(out uiObjUnknownType: Cardinal): DGLE_RESULT; stdcall;

	end;

//Events Interfaces//

	IBaseEvent = interface(IDGLE_Base)
	['{6DFEF982-AADF-42e9-A369-378BDB31404A}']

		function GetEventType(out eEvType :{E_EVENT_TYPE}Integer): DGLE_RESULT; stdcall;
		function GetUnknownEventType(out uiUnknEvType: Cardinal): DGLE_RESULT; stdcall;

	end;

	IEvBeforeInit = interface(IBaseEvent)
	['{EB735739-3D12-4522-B6D7-EEE3225DF934}']

		function SetEngParams(const stWindowParam: TEngWindow; eInitFlags :{E_ENGINE_INIT_FLAGS}Integer): DGLE_RESULT; stdcall;
		function GetEngParams(out stWindowParam: TEngWindow; eInitFlags :{E_ENGINE_INIT_FLAGS}Integer): DGLE_RESULT; stdcall;

	end;

	IEvConsoleWrite = interface(IBaseEvent)
	['{9E35969A-B0D4-4E5A-A89B-1A5AAD057028}']
		function GetText(pcTxt: PAnsiChar; out uiCharsCount: Cardinal; out bToPrevLine: Boolean): DGLE_RESULT; stdcall;
	end;


	IEvFatalMessage = interface(IBaseEvent)
	['{DAA4E3BC-C958-4def-B603-F63EEC908226}']

		function GetMessageTxt(pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function FreezeEngine(bFreeze: Boolean): DGLE_RESULT; stdcall;
		function ForceNoMessage(): DGLE_RESULT; stdcall;
		function ForceIgnoreError(): DGLE_RESULT; stdcall;

	end;

	IEvWinMessage = interface(IBaseEvent)
	['{8D718E48-581D-4cbb-9C40-C04998106F8D}']

		function GetWinMessage(out stWinMsg: TWinMessage): DGLE_RESULT; stdcall;

	end;

	IEvGetSubSystem = interface(IBaseEvent)
	['{2B6D2547-716E-490c-B1F1-422CB428738F}']

		function GetSubSystemType(out eSubSystem :{E_ENGINE_SUB_SYSTEM}Integer): DGLE_RESULT; stdcall;
		function OverrideSubSystem(prSubSystem: IEngineSubSystem): DGLE_RESULT; stdcall;

	end;

	IEvGoFullScreen = interface(IBaseEvent)
	['{CEC9184C-74D9-4739-BF48-BB800467665B}']

	  function GetResolution(out uiScreenWidth, uiScreenHeight: Cardinal; out bGoFullScreen: Boolean): DGLE_RESULT; stdcall;
		function SetResolution(uiScreenWidth, uiScreenHeight: Cardinal): DGLE_RESULT; stdcall;
	end;

//Main Engine System//

	IUserCallback = interface(IDGLE_Base)
 ['{371B1338-BB25-4B8C-BD6A-BCDF241CC52C}']
		function Initialize(): DGLE_RESULT; stdcall;
		function Free(): DGLE_RESULT; stdcall;
		function Update(ui64DeltaTime: {$IF COMPILERVERSION >= 18} UInt64 {$ELSE} Int64 {$IFend} ): DGLE_RESULT; stdcall;
		function Render(): DGLE_RESULT; stdcall;
		function OnEvent({E_EVENT_TYPE} eEventType: Integer; pEvent: IBaseEvent ): DGLE_RESULT; stdcall;
	end;


	TProcedure = Procedure(pParameter: Pointer); stdcall;
	TEventProcedure = Procedure(pParameter: Pointer; const prBaseEvent: IBaseEvent); stdcall;
	TStringProcedure = Procedure(pParameter: Pointer; const pcParam: PAnsiChar); stdcall;
	IFileSystem = interface;
	TFSDelProcedure = Procedure(pParameter: Pointer; const prVFS: IFileSystem); stdcall;
	
	IEngineCore = interface(IDGLE_Base)
	['{111BB884-2BA6-4e84-95A5-5E4700309CBA}']

		function LoadSplashPicture(const pcBmpFileName: PAnsiChar): DGLE_RESULT; stdcall;
		function AddPluginToInitList(const pcFileName: PAnsiChar): DGLE_RESULT; stdcall;
		function InitializeEngine(stHandle: TWinHandle; const pcApplicationName: PAnsiChar; const stWindowParam: TEngWindow; uiUpdateInterval: Cardinal = 33; eInitFlags :{E_ENGINE_INIT_FLAGS}Integer = EIF_DEFAULT): DGLE_RESULT; stdcall;
		function SetUpdateInterval(uiUpdateInterval: Cardinal): DGLE_RESULT; stdcall;	
		function StartEngine(): DGLE_RESULT; stdcall;
		function QuitEngine(): DGLE_RESULT; stdcall;

		function ConnectPlugin(const pcFileName: PAnsiChar; out prPlugin: IPlugin): DGLE_RESULT; stdcall;
		function DisconnectPlugin(pPlugin: IPlugin): DGLE_RESULT; stdcall;
		function GetPlugin(const pcPluginName: PAnsiChar; out prPlugin: IPlugin): DGLE_RESULT; stdcall;

    function AddUserCallback(pUserCallback: IUserCallback): DGLE_RESULT; stdcall;
		function RemoveUserCallback(pUserCallback: IUserCallback): DGLE_RESULT; stdcall;

		function AddProcedure(eProcType :{E_ENGINE_PROCEDURE_TYPE}Integer; pProc: TProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
		function RemoveProcedure(eProcType :{E_ENGINE_PROCEDURE_TYPE}Integer; pProc: TProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;

		function CastEvent(eEventType: {E_EVENT_TYPE} Integer; pEvent: IBaseEvent): DGLE_RESULT; stdcall;
		function AddEventListener(eEventType :{E_EVENT_TYPE}Integer; pListenerProc: TEventProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
		function RemoveEventListener(eEventType :{E_EVENT_TYPE}Integer; pListenerProc: TEventProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
		
		function GetSubsystem( eSubSystem :{E_ENGINE_SUB_SYSTEM}Integer; out prSubSystem: IEngineSubSystem ): DGLE_RESULT; stdcall;
		
		function RenderProfilerTxt(const pcTxt: PAnsiChar; const stColor: TColor4): DGLE_RESULT; stdcall;
		function GetInstanceIdx(out uiIdx: Cardinal): DGLE_RESULT; stdcall;
		function GetTimer(out uiTick: {$IF COMPILERVERSION >= 18} UInt64 {$ELSE} Int64 {$IFEND} ): DGLE_RESULT; stdcall;
		function GetSystemInfo(out stSysInfo: TSystemInfo): DGLE_RESULT; stdcall;
		function GetCurrentWin(out stWin: TEngWindow): DGLE_RESULT; stdcall;
		function GetFPS(out uiFPS: Cardinal): DGLE_RESULT; stdcall;
    function GetLastUpdateDeltaTime(out ui64DeltaTime: {$IF COMPILERVERSION >= 18} UInt64 {$ELSE} Int64 {$IFEND} ): DGLE_RESULT; stdcall;

		function GetHandle(out stHandle: TWinHandle): DGLE_RESULT; stdcall;

		function ChangeWinMode(const stNewWin: TEngWindow): DGLE_RESULT; stdcall;
    function GetDesktopResolution(out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
		function AllowPause(bAllow: Boolean): DGLE_RESULT; stdcall;

		function AddToLog(const pcTxt: PAnsiChar): DGLE_RESULT; stdcall;
		function AddToLogEx(const pcTxt: PAnsiChar; eType :{E_LOG_TYPE}Integer; const pcSrcFileName: PAnsiChar; iSrcLineNumber: Integer): DGLE_RESULT; stdcall;

		function ConsoleVisible(bIsVisible: Boolean): DGLE_RESULT; stdcall;
		function ConsoleWrite(const pcTxt: PAnsiChar; bWriteToPreviousLine: Boolean = false): DGLE_RESULT; stdcall;
		function ConsoleExec(const pcCommandTxt: PAnsiChar): DGLE_RESULT; stdcall;
		function ConsoleRegComProc(const pcCommandName: PAnsiChar; const pcCommandHelp: PAnsiChar; pProc: TStringProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
		function ConsoleRegComValue(const pcCommandName: PAnsiChar; const pcCommandHelp: PAnsiChar; var piValue :Integer; iMinValue: Integer; iMaxValue: Integer; pProc: TStringProcedure; pParameter: Pointer = nil): DGLE_RESULT; stdcall;
		function ConsoleUnregCom(const pcCommandName: PAnsiChar): DGLE_RESULT; stdcall;
		function GetVersion(pcBuffer: PAnsiChar; out uiBufferSize: Cardinal): DGLE_RESULT; stdcall; { TODO: add out ? }

	end;

//Resource Manager SubSystem//

	IFile		= interface;
	ITexture	= interface;
	IMesh		= interface;
  IMultiMesh		= interface;
	IMaterial	= interface;
  ISoundSample = interface;
  ISoundChannel = interface;

	TFileLoadProcedure = Procedure(out rpFile: IFile; out prObj: IEngBaseObj; uiLoadFlags: Cardinal; pParameter: Pointer); stdcall;

	IResourceManager = interface(IEngineSubSystem)
	['{139505B6-5EFC-4f02-A5E8-18CD1FBD69E3}']

		function CreateTexture(out prTex: ITexture; const pubtData: PByte; uiWidth, uiHeight :Cardinal;
		  eDataFormat: {E_TEXTURE_DATA_FORMAT} Integer; eCreationFlags: {E_TEXTURE_CREATION_FLAGS} Integer ;
		  eLoadFlags: {E_TEXTURE_LOAD_FLAGS} Integer; const pcName: PAnsiChar = nil; bAddResource: Boolean = False): DGLE_RESULT; stdcall;
		function CreateMaterial(out prMaterial: IMaterial; const pcName: PAnsiChar = nil; bAddResource: Boolean = False): DGLE_RESULT; stdcall;
		function CreateMesh(out prMesh: IMesh; const pubtData: PByte; uiDataSize, uiNumVerts, uiNumFaces: Cardinal;
		  eCreationFlags :{E_MESH_CREATION_FLAGS}Integer; eLoadFlags :{E_MESH_LOAD_FLAGS}Integer; const pcName: PAnsiChar = nil;
		  bAddResource: Boolean = False): DGLE_RESULT; stdcall;



     ////////////////////////////////////////////////////////////////////////////////////
    //function CreateMultiMesh(out prMMesh:IMultiMesh; const pcName: PAnsiChar = nil; bAddResource: Boolean = False): DGLE_RESULT; stdcall;

    function CreateSound(out prSndSample: ISoundSample; uiSamplesPerSec, uiBitsPerSample: Cardinal; bStereo: Boolean; const pData: Pointer; ui32DataSize: Cardinal; const pcName: PAnsiChar; bAddResource: Boolean = False): DGLE_RESULT; stdcall;

		function RegisterFileFormat(const pcExtension: PAnsiChar; eObjType :{E_ENG_OBJ_TYPE}Integer; const pcDescription: PAnsiChar; pLoadProc: TFileLoadProcedure; pParameter: Pointer): DGLE_RESULT; stdcall;
		function UnregisterFileFormat(const pcExtension: PAnsiChar): DGLE_RESULT; stdcall;
		function RegisterDefaultResource(eObjType :{E_ENG_OBJ_TYPE}Integer; pObj: IEngBaseObj): DGLE_RESULT; stdcall;
		function UnregisterDefaultResource(eObjType :{E_ENG_OBJ_TYPE}Integer; pObj: IEngBaseObj): DGLE_RESULT; stdcall;
		function GetRegisteredExtensions(pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function GetExtensionDescription(const pcExtension: PAnsiChar; pcTxt: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function GetExtensionType(const pcExtension: PAnsiChar; out eType :{E_ENG_OBJ_TYPE}Integer): DGLE_RESULT; stdcall;

		function GetResourceByFileName(const pcFileName: PAnsiChar; out prObj: IEngBaseObj): DGLE_RESULT; stdcall;
		function GetDefaultResource(eObjType :{E_ENG_OBJ_TYPE}Integer; out prObj: IEngBaseObj): DGLE_RESULT; stdcall;

		function Load(const pcFileName: PAnsiChar; out prObj: IEngBaseObj; uiLoadFlags: Cardinal = RES_LOAD_DEFAULT): DGLE_RESULT; stdcall;
		function Load2(pFile: IFile; out prObj: IEngBaseObj; uiLoadFlags: Cardinal = RES_LOAD_DEFAULT): DGLE_RESULT; stdcall;

		function FreeResource(var rpOnj: IEngBaseObj): DGLE_RESULT; stdcall;
		function AddResource(const pcName: PAnsiChar; pObj: IEngBaseObj): DGLE_RESULT; stdcall;
		function RemoveResource(pObj: IEngBaseObj; out bCanDelete: Boolean): DGLE_RESULT; stdcall;

	end;

//Render SubSystem//

	IRender2D = interface;
	IRender3D = interface;
	ILight = interface;

	IRender = interface(IEngineSubSystem)
	['{EA03C661-A334-4225-B5DB-4C45452CCC41}']

		function SetClearColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function GetClearColor(out stColor: TColor4): DGLE_RESULT; stdcall;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function vetClearColor(out stColor: TColor4): DGLE_RESULT; stdcall;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		function Unbind(eType: {E_ENG_OBJ_TYPE}Integer): DGLE_RESULT; stdcall; //use EOT_UNKNOWN to unbind all
		function EnableScissor(const stArea: TRectf): DGLE_RESULT; stdcall;
		function DisableScissor(): DGLE_RESULT; stdcall;
		function SetRenderTarget(pTargetTex: ITexture): DGLE_RESULT; stdcall;

		function GetRender2D(out prRender2D: IRender2D): DGLE_RESULT; stdcall;
		function GetRender3D(out prRender3D: IRender3D): DGLE_RESULT; stdcall;

	end;

//Render2D interface//

	IRender2D = interface(IDGLE_Base)
	['{F5F3257A-F8B8-4d91-BA67-451167A8D63F}']

		function Begin2D(): DGLE_RESULT; stdcall;
		function End2D(): DGLE_RESULT; stdcall;
		//Note: Only one pair Begin2D-End2D per frame is allowed or batching must be disabled.
		function BatchRender(eMode :{E_BATCH_MODE2D} Integer): DGLE_RESULT; stdcall;
    function InvalidateBatchData(): DGLE_RESULT; stdcall;
		function BeginBatch(bUpdateEveryFrame: Boolean = false): DGLE_RESULT; stdcall;
		function EndBatch(): DGLE_RESULT; stdcall;
		function NeedToUpdateBatchData(out bNeedUpdate: Boolean): DGLE_RESULT; stdcall;
		function SetResolutionCorrection(uiResX, uiResY: Cardinal; bConstantProportions: Boolean = true): DGLE_RESULT; stdcall; //Set resx and resy to current screen size to turn off correction
    function CoordResCorrectToAbsolute(const stLogicCoord, stAbsoluteCoord: TPoint2): DGLE_RESULT; stdcall;
		function CoordAbsoluteToResCorrect(const stAbsoluteCoord, stLogicCoord: TPoint2): DGLE_RESULT; stdcall;
		function SetCamera(const stCenter: TPoint2; fAngle: Single; const stScale: TPoint2): DGLE_RESULT; stdcall;
    function ResetCamera(): DGLE_RESULT; stdcall;
    function CullBoundingBox(const stBBox: TRectF; fAngle: Single; out bCull: Boolean): DGLE_RESULT; stdcall;

		// 2D Primitives
		function LineWidth(uiWidth: Cardinal): DGLE_RESULT; stdcall;
		function DrawPoint(const stCoords: TPoint2; const stColor: TColor4; uiSize: Cardinal = 1): DGLE_RESULT; stdcall;
		function DrawLine(const stCoords1, stCoords2: TPoint2; const stColor: TColor4; eFlags :{E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawRect(const stRect: TRectf; const stColor: TColor4; eFlags :{E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawCircle(const stCoords: TPoint2; uiRadius, uiQuality: Cardinal; const stColor: TColor4; eFlags :{E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawEllipse(const stCoords, stRadius: TPoint2; uiQuality: Cardinal; const stColor: TColor4; eFlags :{E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawPolygon(pTexture: ITexture; const pstVertices: PVertex2; uiVerticesCount: Cardinal; eFlags :{E_PRIMITIVE2D_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;

		// 2D Sprites
		function DrawTex(pTexture: ITexture; const stCoords, stDimensions :TPoint2; fAngle: Single = 0.; eFlags :{E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawTexCropped(pTexture: ITexture; const stCoords, stDimensions :TPoint2; const stRect: TRectf; fAngle: Single = 0.; eFlags :{E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;
    function DrawTexSprite(pTexture: ITexture; const stCoords, stDimensions :TPoint2; uiFrame: Cardinal; fAngle: Single = 0.; eFlags :{E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;

		// Extra
		function DrawTriangles(pTexture: ITexture; const pstVertices: PVertex2; uiVerticesCount: Cardinal; eFlags :{PR_FLAGS}Integer = PF_DEFAULT): DGLE_RESULT; stdcall;
		function DrawMesh(pMesh: IMesh; pTexture: ITexture; const stCoords: TPoint2; const stDimensions: TPoint3; const stAxis: TPoint3; fAngle: Single; bClip: Boolean = true;
         fFovY: Single = 90.0; eFlags :{E_EFFECT2D_FLAGS}Integer = EF_DEFAULT): DGLE_RESULT; stdcall;

    //Advanced
		function Draw(pTexture: ITexture; const stDrawDesc: TDrawDataDesc; {E_CORE_RENDERER_DRAW_MODE} eMode: Integer; uiCount: Cardinal; const stAABB: TRectF; {E_EFFECT2D_FLAGS} eFlags: Integer): DGLE_RESULT; stdcall;
		function DrawBuffer(pTexture: ITexture; pBuffer: ICoreGeometryBuffer; const stAABB: TRectF; {E_EFFECT2D_FLAGS} eFlags: Integer): DGLE_RESULT; stdcall;
		function DrawBuffer3D(pTexture: ITexture; pBuffer: ICoreGeometryBuffer; {E_EFFECT2D_FLAGS} eFlags: Integer; const stTransform: TMatrix; const stCenter: TVector3; const stExtents: TVector3; bClip: Boolean; fFovY: Single): DGLE_RESULT; stdcall;

		// Effects
		function SetRotationPoint(const stCoords: TPoint2): DGLE_RESULT; stdcall; //In texture stCoords system
		function SetScale(const stScale: TPoint2): DGLE_RESULT; stdcall;
		function SetColorMix(const stColor: TColor4): DGLE_RESULT; stdcall;
		function SetBlendMode(eMode :{E_EFFECT_BLENDING_FLAGS}Integer = EBF_NORMAL): DGLE_RESULT; stdcall;
		function SetVerticesOffset(const stCoords1, stCoords2, stCoords3, stCoords4: TPoint2): DGLE_RESULT; stdcall;
		function SetVerticesColors(const stColor1,  stColor2,  stColor3,  stColor4 : TColor4): DGLE_RESULT; stdcall;

    function GetRotationPoint(out stCoords: TPoint2): DGLE_RESULT; stdcall;
		function GetScale(out stScale: TPoint2): DGLE_RESULT; stdcall;
		function GetColorMix(out stColor: TColor4): DGLE_RESULT; stdcall;
		function GetBlendMode(out eMode {E_EFFECT_BLENDING_FLAGS}: Integer): DGLE_RESULT; stdcall;
		function GetVerticesOffset(out stCoords1, stCoords2, stCoords3, stCoords4: TPoint2): DGLE_RESULT; stdcall;
		function GetVerticesColors(out stColor1, stColor2, stColor3, stColor4: TColor4): DGLE_RESULT; stdcall;

	end;

//Render3D interface//

	IRender3D = interface(IDGLE_Base)
	['{5275F43A-4FF9-48b2-B88E-B2F842461AB3}']

 		function SetPerspective(fFovAngle, fZNear, fZFar: Single): DGLE_RESULT; stdcall;
    function GetPerspective(out fFovAngle, fZNear, fZFar: Single): DGLE_RESULT; stdcall;

		function SetColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function GetColor(out stColor: TColor4): DGLE_RESULT; stdcall;

		function SetBlendMode(eMode :{E_EFFECT_BLENDING_FLAGS}Integer = EBF_NORMAL): DGLE_RESULT; stdcall;
    function GetBlendMode(out eMode :{E_EFFECT_BLENDING_FLAGS}Integer): DGLE_RESULT; stdcall;

		function ToggleAlphaTest(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetAlphaTreshold(fValue: Single): DGLE_RESULT; stdcall;
    function GetAlphaTreshold(out fValue: Single): DGLE_RESULT; stdcall;

		function ToggleDepthTest(bEnabled: Boolean): DGLE_RESULT; stdcall;

    function ToggleFog(bEnabled: Boolean): DGLE_RESULT; stdcall;
    function SetFogColor(const stColor: TColor4): DGLE_RESULT; stdcall;
    function SetFogDensity(fValue: Single): DGLE_RESULT; stdcall;
    function GetFogColor(out stColor: TColor4): DGLE_RESULT; stdcall;
    function GetFogDensity(out fValue: Single): DGLE_RESULT; stdcall;

		function SetMatrix(const stMatrix: TMatrix; bMult: Boolean = true): DGLE_RESULT; stdcall;
		function GetMatrix(out stMatrix: TMatrix): DGLE_RESULT; stdcall;

		function DrawAxes(fSize: Single = 1.0; bNoDepthTest: Boolean = false): DGLE_RESULT; stdcall;

		function PushStates(eStates :{E_PUSH_STATES_FLAGS}Integer = PSF_MATRIX): DGLE_RESULT; stdcall;
		function PopStates(): DGLE_RESULT; stdcall;

		function GetPoint3(const stPointOnScreen: TPoint2; out stResultPoint: TPoint3; eFlag :{E_GET_POINT3_FLAG}Integer = GP3F_FROM_Z_BUFFER): DGLE_RESULT; stdcall;
		function GetPoint2(const stPoint: TPoint3; out stResultPointOnScreen: TPoint2): DGLE_RESULT; stdcall;

		function FrustumSetup(): DGLE_RESULT; stdcall;
		function CullPoint(const stCoords: TPoint3; out bCull: Boolean): DGLE_RESULT; stdcall;
		function CullSphere(const stCoords: TPoint3; fRadius: Single; out bCull: Boolean): DGLE_RESULT; stdcall;
		function CullBox(const stCenterCoords, stExtents: TVector3; out bCull: Boolean): DGLE_RESULT; stdcall;

		function CreateLight(out prLight:ILight): DGLE_RESULT; stdcall;
		function ToggleLighting(bEnabled: Boolean): DGLE_RESULT; stdcall;
		function SetGlobalAmbientLighting(const stColor: TColor4): DGLE_RESULT; stdcall;
		function GetGlobalAmbientLighting(out stColor: TColor4): DGLE_RESULT; stdcall;

	end;

//Light interface//

	ILight = interface(IDGLE_Base)
	['{EB73AC84-A465-4554-994D-8BED29744C9D}']

		function SetEnabled(bEnabled: Boolean): DGLE_RESULT; stdcall;
		function SetDiffuseColor(const stColor: TColor4): DGLE_RESULT; stdcall;
		function SetSpecularColor(const stColor: TColor4): DGLE_RESULT; stdcall;
		function SetPosition(const stPos: TPoint3): DGLE_RESULT; stdcall;
		function SetDirection(const stDir: TVector3): DGLE_RESULT; stdcall;
		function SetDistance(fDist: Single): DGLE_RESULT; stdcall;
		function SetAttenuation(fCoeff: Single): DGLE_RESULT; stdcall;
    function SetSpotCutoff(fDist: Single): DGLE_RESULT; stdcall;

		function SetType(eType: Integer {E_LIGHT_TYPE}): DGLE_RESULT; stdcall;

		function GetEnabled(out bEnabled: Boolean): DGLE_RESULT; stdcall;
		function GetDiffuseColor(out stColor: TColor4): DGLE_RESULT; stdcall;
		function GetSpecularColor(out stColor: TColor4): DGLE_RESULT; stdcall;
		function GetPosition(out stPos: TPoint3): DGLE_RESULT; stdcall;
		function GetDirection(out stDir: TVector3): DGLE_RESULT; stdcall;
		function GetDistance(out fDist: Single): DGLE_RESULT; stdcall;
		function GetAttenuation(out fCoeff: Single): DGLE_RESULT; stdcall;
    function GetSpotCutoff(out fDist: Single): DGLE_RESULT; stdcall;

		function GetType(out eType: Integer {E_LIGHT_TYPE}): DGLE_RESULT; stdcall;
		function Free(): DGLE_RESULT; stdcall;

	end;

//Texture interface//

	ITexture = interface(IEngBaseObj)
	['{85BDDBC2-F126-4cae-946D-7D6B079E5CCE}']

		function GetDimension(out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
		function SetFrameSize(uiFrameWidth, uiFrameHeight: Cardinal): DGLE_RESULT; stdcall;
		function GetFrameSize(out uiFrameWidth, uiFrameHeight: Cardinal): DGLE_RESULT; stdcall;

		function GetCoreTexture(out prCoreTex: ICoreTexture): DGLE_RESULT; stdcall;

		function Draw2DSimple(iX, iY: Integer; uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;
		function Draw2D( iX, iY: Integer; uiWidth, uiHeight: Cardinal; fAngle: Single = 0.0; uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;
		function Draw3D(uiFrameIndex: Cardinal = 0): DGLE_RESULT; stdcall;

		function Bind(uiMTextureLayer: Cardinal = 0): DGLE_RESULT; stdcall;

	end;

//Material interface//

	IMaterial = interface(IEngBaseObj)
	['{B6506749-BB41-423d-B6C0-982081EF63F9}']

		function SetDiffuseColor(const stColor: TColor4): DGLE_RESULT; stdcall;
		function SetSpecularColor(const stColor: TColor4): DGLE_RESULT; stdcall;
		function SetDiffuseTexture(pTexture: ITexture): DGLE_RESULT; stdcall;

		function GetDiffuseColor(out stColor: TColor4): DGLE_RESULT; stdcall;
		function GetSpecularColor(out stColor: TColor4): DGLE_RESULT; stdcall;
		function GetDiffuseTexture(out pTexture:ITexture): DGLE_RESULT; stdcall;

		function Bind(): DGLE_RESULT; stdcall;

	end;


	//BitmapFont interface//

	IBitmapFont = interface(IEngBaseObj)
	['{0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA}']

		function GetTexture(out prTexture: ITexture): DGLE_RESULT; stdcall;
		function SetScale(fScale: Single): DGLE_RESULT; stdcall;
		function GetScale(out fScale: Single): DGLE_RESULT; stdcall;
		function GetTextDimensions(const pcTxt: PAnsiChar; out uiWidth, uiHeight: Cardinal): DGLE_RESULT; stdcall;
		function Draw2DSimple(iX, iY: Integer; const pcTxt: PAnsiChar; const stColor: TColor4): DGLE_RESULT; stdcall;
		function Draw2D(fX, fY: Single; const pcTxt: PAnsiChar; const stColor: TColor4; fAngle: Single = 0; bVerticesColors: Boolean = false): DGLE_RESULT; stdcall;
		function Draw3D(const pcTxt: PAnsiChar): DGLE_RESULT; stdcall;

	end;

//3D Objects interfaces//

//Mesh interface//

	IMesh = interface(IEngBaseObj)
	['{85E360A8-07B3-4f22-AA29-07C7FC7C6893}']

		function Draw(): DGLE_RESULT; stdcall;
		function GetCenter(out stCenter: TPoint3): DGLE_RESULT; stdcall;
		function GetExtents(out stExtents: TVector3): DGLE_RESULT; stdcall;
		function GetTrianglesCount(out uiTriCnt: Cardinal): DGLE_RESULT; stdcall;
		function GetGeometryBuffer(out prBuffer:ICoreGeometryBuffer): DGLE_RESULT; stdcall;
    function SetGeometryBuffer(pBuffer: ICoreGeometryBuffer): DGLE_RESULT; stdcall;
    function CalculateNormals(): DGLE_RESULT; stdcall;
    function CalculateTangentSpace(): DGLE_RESULT; stdcall;
    function GetOwner(out prMMesh: IMultiMesh): DGLE_RESULT; stdcall;


	end;

//Multi mesh interface//

	IMultiMesh = interface(IEngBaseObj)
	['{6107C296-FC07-48d1-B6A7-F88CC2DAE897}']

    function Draw(): DGLE_RESULT; stdcall;

    function DrawMesh(uiMeshIdx: Cardinal): DGLE_RESULT; stdcall;
    function GetCenter(out stCenter: TPoint3): DGLE_RESULT; stdcall;
    function GetExtents(out stExtents: TVector3): DGLE_RESULT; stdcall;
		function MeshsCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
		function GetMesh(uiIdx: Cardinal; out prMesh: IMesh ): DGLE_RESULT; stdcall;
    function SetMeshMaterial(uiMeshIdx: Cardinal; pMaterial: IMaterial): DGLE_RESULT; stdcall;
    function GetMeshMaterial(uiMeshIdx: Cardinal;  outprMaterial: IMaterial): DGLE_RESULT; stdcall;
    function AddMesh(pMesh: IMesh): DGLE_RESULT; stdcall;
    function RemoveMesh(pMesh: IMesh): DGLE_RESULT; stdcall;
    function ReplaceMesh(uiMeshIdx: Cardinal; pMesh: IMesh): DGLE_RESULT; stdcall;

		
	end;

//Input Subsystem//

	IInput = interface(IEngineSubSystem)
	['{64DAAF7F-F92C-425f-8B92-3BE40D8C6666}']

		function Configure(eFlags :{E_INPUT_CONFIGURATION_FLAGS}Integer = ICF_DEFAULT): DGLE_RESULT; stdcall;

		function GetMouseStates(out stMStates: TMouseStates): DGLE_RESULT; stdcall;
		function GetKey(eKeyCode :{E_KEYBOARD_KEY_CODES}Integer; out bPressed: Boolean): DGLE_RESULT; stdcall;
		function GetKeyName(eKeyCode :{E_KEYBOARD_KEY_CODES}Integer; out cASCIICode: Char): DGLE_RESULT; stdcall;

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
		function SetMaxChannelsCount(uiCount: Cardinal): DGLE_RESULT; stdcall;
		function GetListenerPosition(out stCoords: TPoint3): DGLE_RESULT; stdcall;
		function GetFreeChannelsCount(out uiCount: Cardinal): DGLE_RESULT; stdcall;
		function ReleaseChannelsByData(const pData: Pointer): DGLE_RESULT; stdcall;
		function CreateChannel(out prSndChnl: ISoundChannel ; uiSamplesPerSec, uiBitsPerSample: Cardinal ;
      bStereo: Boolean ; const pData: Pointer; ui32DataSize: Cardinal): DGLE_RESULT; stdcall; //Data not copied!
		function CreateStreamableChannel(out prSndChnl: ISoundChannel; uiSamplesPerSec, uiBitsPerSample: Cardinal; bStereo: Boolean; ui32DataSize: Cardinal;
      pStreamCallback: StreamCallbackFunc; pParameter: Pointer): DGLE_RESULT; stdcall;


	end;

	// Sound Sample Interface

	ISoundChannel = interface(IDGLE_Base)
	['{DE6F7CDD-8262-445c-8D20-68E3324D99A6}']

		function Play(bLooped: Boolean): DGLE_RESULT; stdcall;
    function Pause(): DGLE_RESULT; stdcall;
		function Stop(): DGLE_RESULT; stdcall;
		function IsPlaying(out bIsPlaying: Boolean): DGLE_RESULT; stdcall;
		function SetVolume(uiVolume: Cardinal ): DGLE_RESULT; stdcall; //from 0 to 100
		function GetVolume(out uiVolume: Cardinal): DGLE_RESULT; stdcall;
		function SetPan(iPan: Integer ): DGLE_RESULT; stdcall; //from -100 to 100
		function GetPan(out iPan: Integer): DGLE_RESULT; stdcall;
		function SetSpeed(uiSpeed: Cardinal): DGLE_RESULT; stdcall;//in percents
		function GetSpeed(out uiSpeed: Cardinal): DGLE_RESULT; stdcall;
		function SetCurrentPosition(uiPos: Cardinal): DGLE_RESULT; stdcall;
		function GetCurrentPosition(out uiPos: Cardinal): DGLE_RESULT; stdcall;
		function GetLength(out uiLength: Cardinal): DGLE_RESULT; stdcall;
		function IsStreamable(bStreamable: Boolean): DGLE_RESULT; stdcall;
		function Unaquire(): DGLE_RESULT; stdcall;

	end;

//SoundSample interface//

	ISoundSample = interface(IEngBaseObj)
	['{30DD8C94-D3FA-40cf-9C49-649211424919}']

		function Play(): DGLE_RESULT; stdcall;
		function PlayEx(out pSndChnl: ISoundChannel ; eFlags: {E_SOUND_SAMPLE_PARAMS}Integer = SSP_NONE): DGLE_RESULT; stdcall; //pSndChnl must be checked on nul
	end;

//Music interface//

	IMusic = interface(IEngBaseObj)
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
		function Seek(ui32Offset: Cardinal; eWay :{E_FILE_SYSTEM_SEEK_FLAG}Integer; out ui32Position: Cardinal): DGLE_RESULT; stdcall;
		function GetSize(out ui32Size: Cardinal): DGLE_RESULT; stdcall;
		function IsOpen(out bOpened: Boolean): DGLE_RESULT; stdcall;
		function GetName(pcName: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function GetPath(pcPath: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function Free(): DGLE_RESULT; stdcall;

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

		function OpenFile(const pcName: PAnsiChar; eFlags :{E_FILE_SYSTEM_OPEN_FLAGS}Integer; out prFile: IFile): DGLE_RESULT; stdcall;
		function DeleteFile(const pcName: PAnsiChar): DGLE_RESULT; stdcall;
		function FileExists(const pcName: PAnsiChar; out bExists: Boolean): DGLE_RESULT; stdcall;
		function Find(const pcMask: PAnsiChar; eFlags :{E_FIND_FLAGS}Integer; out prIterator: IFileIterator): DGLE_RESULT; stdcall;
 		function SendCommand(const pcCommand, pcResult: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall ;


 end;

const
	//E_GET_ENGINE_FLAGS
	GEF_DEFAULT						= $00000000;
	GEF_FORCE_SINGLE_THREAD			= $00000001;
	GEF_FORCE_NO_LOG_FILE			= $00000002;
	GEF_FORCE_QUIT					= $00000004;
	
var
	hServer			: THandle = 0;
	pCreateEngine	: Function(out pEngineCore: IEngineCore; eFlags :{E_GET_ENGINE_FLAGS}Integer; ubtSDKVer: byte): Boolean; stdcall;
	pFreeEngine		: Function(pEngineCore: IEngineCore): Boolean; stdcall;


function CreateEngine(out pEngineCore: IEngineCore; eFlags :{E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
function GetEngine(pcDllFileName: AnsiString; out pEngineCore :IEngineCore; eFlags :{E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
function FreeEngine(pEngineCore: IEngineCore = nil; bFreeDLL: Boolean = true): Boolean;

implementation

function CreateEngine(out pEngineCore: IEngineCore; eFlags :{E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
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

function GetEngine(pcDllFileName: AnsiString; out pEngineCore: IEngineCore; eFlags :{E_GET_ENGINE_FLAGS}Integer = GEF_DEFAULT): Boolean;
begin
	Result := false;
	if hServer = 0 then
	begin
		pEngineCore	:= nil;
		
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
