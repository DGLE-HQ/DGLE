/**
\file		DGLE.cs
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		XX.XX.2012 (c)Korotkov Andrey

\brief		Main DGLE engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE SDK.
*/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

/** Engines main namespace. */

namespace DGLE
{

//Engine Base interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IDGLE_Base
	{
		void GetGUID(ref Guid guid);
		//TODO
		void ExecCmd();
		void ExecCmdStr();
		void ExecCmdVar();
	}

//Engine SubSystem interface//

	public enum E_ENGINE_SUB_SYSTEM
	{
		ESS_RENDER = 0,
		ESS_INPUT,
		ESS_SOUND,
		ESS_RESOURCE_MANAGER,
		ESS_FILE_SYSTEM,
		ESS_CORE_RENDERER
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("C682F875-E0BD-4af9-B79C-E209850025F8")]
	public interface IEngineSubSystem : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		//TODO
		new void ExecCmd();
		new void ExecCmdStr();
		new void ExecCmdVar();
		#endregion

		void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
	};

//Engine Plugin interface//
		
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("B94E0E40-8885-41dd-8BC5-4CD663AE5709")]
	public interface IPlugin : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void GetPluginInfo(out TPluginInfo stInfo);
		void GetPluginInterfaceName(IntPtr pcName, out UInt32 uiCharsCount);
	};



//Engine Subsystem Plugin interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("27908E31-8D8E-4076-AE33-087A1BE5DCB3")]
	/** Base interface of any engine core subsystem plugin. */
	public interface ISubSystemPlugin : IPlugin
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void GetSubSystemInterface(out IEngineSubSystem prSubSystem);
	};



//Engine Base Object interface//

	public enum E_ENG_OBJ_TYPE
	{
		EOT_UNKNOWN = 0,
		EOT_TEXTURE,
		EOT_MATERIAL,
		EOT_MESH,
		EOT_MODEL,
		EOT_BITMAP_FONT,
		EOT_PARTICLE_EFFECT,
		EOT_SOUND_SAMPLE,
		EOT_SPRITE,
		EOT_GUI_FORMS
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("C010239A-6457-40f5-87EF-FAA3156CE6E2")]
	public interface IEngBaseObj : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void Free();
		void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		void GetUnknownType(out uint uiObjUnknownType);
	};

//Events Interfaces//

	public enum E_EVENT_TYPE
	{
		ET_UNKNOWN = 0,
		ET_BEFORE_INIT,
		ET_BEFORE_RENDER,
		ET_AFTER_RENDER,
		ET_ON_PROFILER_DRAW,
		ET_ON_WIN_MESSAGE,
		ET_ON_GET_SSYSTEM,
		ET_ON_ENGINE_FATAL_MESSAGE,
		ET_ON_CONSOLE_WRITE			
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("6DFEF982-AADF-42e9-A369-378BDB31404A")]
	public interface IBaseEvent : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void GetEventType(out E_EVENT_TYPE eEvType);
		void GetUnknownEventType(out uint uiUnknEvType);
	};    
	
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("EB735739-3D12-4522-B6D7-EEE3225DF934")]
	public interface IEvBeforeInit :  IBaseEvent
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IBaseEvent
		new void GetEventType(out E_EVENT_TYPE eEvType);
		new void GetUnknownEventType(out uint uiUnknEvType);
		#endregion

		void SetEngParams(ref TEngWindow stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags);
		void GetEngParams(out TEngWindow stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags);
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("9E35969A-B0D4-4E5A-A89B-1A5AAD057028")]
	public interface IEvConsoleWrite : IBaseEvent
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IBaseEvent
		new void GetEventType(out E_EVENT_TYPE eEvType);
		new void GetUnknownEventType(out uint uiUnknEvType);
		#endregion
		void GetText(IntPtr pcTxt, out uint uiCharsCount, [MarshalAs(UnmanagedType.U1)] bool bToPrevLine);
	};


	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("DAA4E3BC-C958-4def-B603-F63EEC908226")]
	public interface IEvFatalMessage : IBaseEvent
	{
		void GetMessageTxt(IntPtr pcTxt, out uint uiCharsCount);
		void FreezeEngine([MarshalAs(UnmanagedType.U1)] bool bFreeze);
		void ForceNoMessage();
		void ForceIgnoreError();
	};   
	
	
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("8D718E48-581D-4cbb-9C40-C04998106F8D")]
	public interface IEvWinMessage : IBaseEvent
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IBaseEvent
		new void GetEventType(out E_EVENT_TYPE eEvType);
		new void GetUnknownEventType(out uint uiUnknEvType);
		#endregion

		void GetWinMessage(out TWinMessage stWinMsg);
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("2B6D2547-716E-490c-B1F1-422CB428738F")]
	public interface IEvGetSubSystem : IBaseEvent
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IBaseEvent
		new void GetEventType(out E_EVENT_TYPE eEvType);
		new void GetUnknownEventType(out uint uiUnknEvType);
		#endregion

		void GetSubSystemType(E_ENGINE_SUB_SYSTEM eSubSystem);
		void OverrideSubSystem(ref IEngineSubSystem prSubSystem);
	};

//Main Engine System//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("371B1338-BB25-4B8C-BD6A-BCDF241CC52C")]
	public interface IUserCallback : IDGLE_Base
	{	
		void Initialize();
		void Free();
		void Update(UInt64 ui64DeltaTime);
		void Render();
		void OnEvent(E_EVENT_TYPE eEventType, IBaseEvent pEvent);
	};


	public enum E_ENGINE_PROCEDURE_TYPE
	{
		EPT_UPDATE = 0,
		EPT_RENDER,
		EPT_INIT,
		EPT_FREE
	};

	public enum E_LOG_TYPE
	{
		LT_INFO = 0,
		LT_WARNING,
		LT_ERROR,
		LT_FATAL
	};    

	public enum E_ENGINE_INIT_FLAGS
	{
		EIF_DEFAULT             = 0x00000000,
		EIF_CATCH_UNHANDLED     = 0x00000001,
		EIF_FORCE_NO_SOUND      = 0x00000002,
		EIF_LOAD_ALL_PLUGINS    = 0x00000004,
		EIF_FORCE_LIMIT_FPS		= 0x00000010,
		EIF_FORCE_16_BIT_COLOR	= 0x00000020,
		EIF_DISABLE_SMART_TIMING= 0x00000040,
		EIF_FORCE_NO_SPLASH     = 0x10000000
	};

	public delegate void DSubscriber(IntPtr pParam);
	public delegate void DListenerProc(IntPtr pParam, ref IBaseEvent pEvent);
	public delegate void DFSDeleteCallback(IntPtr pParam, ref IFileSystem pVFS);

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("111BB884-2BA6-4e84-95A5-5E4700309CBA")]
	public interface IEngineCore : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		//TODO
		new void ExecCmd();
		new void ExecCmdStr();
		new void ExecCmdVar();
		#endregion

		void LoadSplashPicture([MarshalAs(UnmanagedType.LPStr)] string pcBmpFileName);	
		void AddPluginToInitList([MarshalAs(UnmanagedType.LPStr)] string pcFileName);	

		void InitializeEngine(IntPtr stHandle, [MarshalAs(UnmanagedType.LPStr)] string pcApplicationName, ref TEngWindow stWindowParam, UInt32 uiProcessInterval, E_ENGINE_INIT_FLAGS eInitFlags);
		void SetProcessInterval(UInt32 uiProcessInterval);
		void StartEngine();
		void QuitEngine();

		void ConnectPlugin([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IPlugin prPlugin);
		void DisconnectPlugin(IPlugin pPlugin);
		void GetPlugin([MarshalAs(UnmanagedType.LPStr)] string pcPluginName, out IPlugin prPlugin);

		//TODO
		void AddUserCallback();
		void RemoveUserCallback();

		void AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc, IntPtr pParam);
		void RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc, IntPtr pParam);

		void CastEvent(E_EVENT_TYPE eEventType, out IBaseEvent pEvent);
		void AddEventListner(E_EVENT_TYPE eEventType, [MarshalAs(UnmanagedType.FunctionPtr)] DListenerProc pListnerProc, IntPtr pParam);
		void RemoveEventListner(E_EVENT_TYPE eEventType, [MarshalAs(UnmanagedType.FunctionPtr)] DListenerProc pListnerProc, IntPtr pParam);

		void GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, out IEngineSubSystem prSubSystem);

		void RenderProfilerTxt([MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor);
		void GetInstanceIdx(out UInt32 uiIdx);
		void GetTimer(out UInt64 uiTick);
		void GetSystemInfo(out TSystemInfo stSysInfo);
		void GetCurrentWin(out TEngWindow stWin);
		void GetFPS(out UInt32 uiFPS);
		void GetLastUpdateDeltaTime(out UInt64 ui64DeltaTime);
		void GetHandle(out IntPtr stHandle);

		void ChangeWinMode(ref TEngWindow stNewWin);
		void AllowPause([MarshalAs(UnmanagedType.U1)] bool bAllow);

		void AddToLog([MarshalAs(UnmanagedType.LPStr)] string pcTxt);
		void AddToLogEx([MarshalAs(UnmanagedType.LPStr)] string pcTxt, E_LOG_TYPE eType, [MarshalAs(UnmanagedType.LPStr)] string pcSrcFileName, Int32 iSrcLineNumber);

		void ConsoleVisible([MarshalAs(UnmanagedType.U1)] bool bIsVisible);
		void ConsoleWrite([MarshalAs(UnmanagedType.LPStr)] string pcTxt,[MarshalAs(UnmanagedType.U1)] bool bWriteToPreviousLine);
		void ConsoleExec([MarshalAs(UnmanagedType.LPStr)] string pcCommandTxt);
		void ConsoleRegComProc([MarshalAs(UnmanagedType.LPStr)] string pcCommandName, [MarshalAs(UnmanagedType.LPStr)] string pcCommandHelp, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc);
		void ConsoleRegComValue([MarshalAs(UnmanagedType.LPStr)] string pcCommandName, [MarshalAs(UnmanagedType.LPStr)] string pcCommandHelp, ref Int32 piValue, Int32 iMinValue, Int32 iMaxValue, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc);
		void ConsoleUnregCom([MarshalAs(UnmanagedType.LPStr)] string pcCommandName);
		
		void GetVersion(IntPtr pcBuffer, uint uiBufferSize);	
	};

//Resource Manager SubSystem//

	public enum E_TEXTURE_DATA_FORMAT
	{
		TDF_RGB8 = 0,
		TDF_RGBA8,
		TDF_ALPHA8,
		TDF_BGR8,
		TDF_BGRA8,
		TDF_DXT1,
		TDF_DXT5,
		TDF_DEPTH_COMPONENT24
	};

	public enum E_TEXTURE_CREATION_FLAGS
	{
		TCF_DEFAULT = 0x00000000,
		TCF_PIXEL_ALIGNMENT_1 = 0x00000001,//use only if your texture input data is not 4 byte aligned
		TCF_MIPMAPS_PRESENTED = 0x00000002 //all mip levels must be presented
	};


	public enum E_TEXTURE_LOAD_FLAGS
	{
		TLF_FILTERING_NONE          = 0x00000001,
		TLF_FILTERING_BILINEAR      = 0x00000002,
		TLF_FILTERING_TRILINEAR     = 0x00000004,
		TLF_FILTERING_ANISOTROPIC   = 0x00000008,

		TLF_DECREASE_QUALITY_MEDIUM = 0x00000020,
		TLF_DECREASE_QUALITY_HIGH   = 0x00000040,

		TLF_COMPRESS                = 0x00000100,

		TLF_COORDS_REPEAT           = 0x00001000,
		TLF_COORDS_CLAMP            = 0x00002000,
		TLF_COORDS_MIRROR_REPEAT	= 0x00004000,
		TLF_COORDS_MIRROR_CLAMP		= 0x00008000,

		TLF_GENERATE_MIPMAPS		= 0x00040000,

		TLF_ANISOTROPY_2X			= 0x00100000,
		TLF_ANISOTROPY_4X			= 0x00200000,
		TLF_ANISOTROPY_8X			= 0x00400000,
		TLF_ANISOTROPY_16X			= 0x00800000,
		
		TEXTURE_LOAD_DEFAULT_2D     = TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP,
		TEXTURE_LOAD_DEFAULT_3D     = TLF_FILTERING_TRILINEAR | TLF_GENERATE_MIPMAPS | TLF_COORDS_REPEAT,

		RES_LOAD_DEFAULT            = 0x00000000
	};
	
	public enum E_MESH_CREATION_FLAGS
	{
		MCF_ONLY_DEFAULT_DATA		= 0x00000000,//vertex and indexes arrays must be presented
		MCF_NORMALS_PRESENTED		= 0x00000001,
		MCF_TEXTURE_COORDS_PRESENTED= 0x00000002,
		MCF_TANGENT_SPACE_PRESENTED	= 0x00000004
	};

	public enum E_MESH_LOAD_FLAGS
	{
		MLF_EDITABLE				= 0x00000001,
		MLF_FORCE_NO_VBO			= 0x00000002,
		MLF_CALCULATE_TANGENT_SPACE = 0x00000004,
		MLF_CALCULATE_NORMALS		= 0x00000008,

		RES_LOAD_DEFAULT            = 0x00000000
	};

	public delegate bool DLoadProc(ref IFile pFile, ref IEngBaseObj prObj, UInt32 uiLoadFlags);
	
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("139505B6-5EFC-4f02-A5E8-18CD1FBD69E3")]
	public interface IResourceManager : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngineSubSystem
		new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
		#endregion

		void CreateTexture(out ITexture prTex, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 uiWidth, UInt32 uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName, bool bAddResourse = true);
		void CreateMaterial(out IMaterial prMaterial, [MarshalAs(UnmanagedType.LPStr)] string pcName, bool bAddResourse = true);
		void CreateMesh(out IMesh prMesh, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 uiDataSize, UInt32 uiNumVerts, UInt32 uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName, bool bAddResourse = true);
		void CreateSound(out ISoundSample prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 ui32DataSize, [MarshalAs(UnmanagedType.LPStr)] string pcName, bool bAddResourse = false);

		void RegisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj pObj);
		void UnregisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj pObj);
		void RegisterFileFormat([MarshalAs(UnmanagedType.LPStr)] string pcExtension, E_ENG_OBJ_TYPE eObjType, [MarshalAs(UnmanagedType.LPStr)] string pcDescription, [MarshalAs(UnmanagedType.FunctionPtr)] DLoadProc pLoadProc);
		void UnregisterFileFormat([MarshalAs(UnmanagedType.LPStr)] string pcExtension);
		void GetRegisteredExtensions(IntPtr pcTxt, out UInt32 uiCharsCount);
		void GetExtensionDescription([MarshalAs(UnmanagedType.LPStr)] string pcExtension, IntPtr pcTxt, out UInt32 uiCharsCount);
		void GetExtensionType([MarshalAs(UnmanagedType.LPStr)] string pcExtension, out E_ENG_OBJ_TYPE eType);

		void GetResourceByFileName([MarshalAs(UnmanagedType.LPStr)] string pcName, out IEngBaseObj prObj);
		void GetDefaultResource(E_ENG_OBJ_TYPE eObjType, out IEngBaseObj prObj);

		void Load([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IEngBaseObj prObj, UInt32 uiLoadFlags);
		void Load2(IFile pFile, out IEngBaseObj prObj, UInt32 uiLoadFlags);

		void FreeResource(ref IEngBaseObj pEngObj);
		void AddResource([MarshalAs(UnmanagedType.LPStr)] string pcFileName, IEngBaseObj pObj);
		void RemoveResource(IEngBaseObj pObj, [MarshalAs(UnmanagedType.U1)] ref bool bCanDelete);
	};

//Render SubSystem//

	public enum E_GET_POINT3_FLAG
	{
		GP3F_FROM_Z_BUFFER = 0,
		GP3F_FROM_FAR_PLANE,
		GP3F_FROM_NEAR_PLANE
	};


	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("EA03C661-A334-4225-B5DB-4C45452CCC41")]
	public interface IRender : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngineSubSystem
		new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
		#endregion

		void SetClearColor(ref TColor4 stColor);
		void Unbind(E_ENG_OBJ_TYPE eType); //use EOT_UNKNOWN to unbind all
		void EnableScissor(ref TRectF stArea);
		void DisableScissor();
		void SetRenderTarget(ITexture pTargetTex);
		void ScreenShotBMP([MarshalAs(UnmanagedType.LPStr)] string pFileName);
		void CreatePostProcess(out IPostprocess pPP);
		
		void GetRender2D(out IRender2D prRender2D);
		void GetRender3D(out IRender3D prRender3D);
	};

	//Render2D interface//

	//Flags for Primitives
	public enum E_PRIMITIVE2D_FLAGS
	{
		PF_DEFAULT          = 0x00000000,
		PF_FILL             = 0x00000001,
		PF_SMOOTH           = 0x00000002,
		PF_VCA              = 0x00000004
	};

	//Flags for Effects
	public enum E_EFFECT2D_FLAGS
	{
		EF_DEFAULT          = 0x00000000,
		EF_BLEND            = 0x00000001,
		EF_FLIPX            = 0x00000002,
		EF_FLIPY            = 0x00000004,
		EF_COLORMIX         = 0x00000008,
		EF_SCALE            = 0x00000010,
		EF_VERTICES_OFFSET  = 0x00000020,
		EF_VERTICES_COLOR   = 0x00000040,
		EF_ROTATEPT         = 0x00000080
	};

	//Flags for Effects Blend mode
	public enum E_EFFECT2D_BLENDING_FLAGS
	{
		EBF_NORMAL			= 0x00000001, 
		EBF_ADD				= 0x00000002, 
		EBF_MULT			= 0x00000004, 
		EBF_BLACK			= 0x00000008,
		EBF_WHITE			= 0x00000010,
		EBF_MASK			= 0x00000020
	};

	public enum E_BATCH_MODE2D
	{
		BM_AUTO = 0,
		BM_DISABLED,
		BM_ENABLED_UEP,//update batches every process tick
		BM_ENABLED_UER //update batches every render frame
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("F5F3257A-F8B8-4d91-BA67-451167A8D63F")]
	public interface IRender2D : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void Begin2D();
		void End2D();
		//Note: Only one pair Begin2D-End2D per frame is allowed or batching must be disabled.
		void BatchRender(E_BATCH_MODE2D eMode);
		void SetResolutionCorrection(UInt32 uiResX, UInt32 uiResY, [MarshalAs(UnmanagedType.U1)]bool bConstaintProportions); //Set resx and resy to current screen size to turn off correction
		void SetCamera(ref TPoint2 stCenter, float fAngle, TPoint2 stScale);

		// 2D Primitives
		void LineWidth(UInt32 uiWidth);
		void DrawPoint(ref TPoint2 stCoords, ref TColor4 stColor, UInt32 uiSize);
		void DrawLine(ref TPoint2 stCoords1, ref TPoint2 stCoords2, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags);
		void DrawRect(ref TRectF stRect, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags);
		void DrawCircle(ref TPoint2 stCoords, UInt32 uiRadius, UInt32 uiQuality, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags);
		void DrawEllipse(ref TPoint2 stCoords, ref TPoint2 stRadius, UInt32 uiQuality, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags);
		void DrawPolygon(ITexture pTexture, ref TVertex2 pstVertices, UInt32 uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);
	
		// 2D Sprites		
		void DrawSpriteS(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, float fAngle, E_EFFECT2D_FLAGS flags);
		void DrawSpriteA(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, UInt32 uiFrameIndex, float fAngle, E_EFFECT2D_FLAGS eFlags);
		void DrawSpriteC(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, ref TRectF stRect, float fAngle, E_EFFECT2D_FLAGS eFlags);

		// Extra
		void DrawTriangles(ITexture pTexture, ref TVertex2 pstVertices, UInt32 uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags);
		void DrawMesh(IMesh pMesh, ITexture pTexture, ref TPoint2 stCoords, ref TPoint3 stDimensions, ref TPoint3 stAxis, float fAngle, [MarshalAs(UnmanagedType.U1)]bool bClip, float fFovY, E_EFFECT2D_FLAGS eFlags);

		//Effects
		void SetRotationPoint(ref TPoint2 stCoords);//In texture coord system
		void SetScale(ref TPoint2 stScale);
		void SetColorMix(ref TColor4 stColor);
		void SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode);
		void SetVerticesOffset(ref TPoint2 stCoords1, ref TPoint2 stCoords2, ref TPoint2 stCoords3, ref TPoint2 stCoords4);
		void SetVerticesColors(ref TColor4 stColor1, ref TColor4 stColor2, ref TColor4 stColor3, ref TColor4 stColor4);
	};

	//Render3D interface//

	public enum E_PUSH_STATES_FLAGS
	{
		PSF_ALL		= 0x00000000,
		PSF_MATRIX	= 0x00000001,
		PSF_STATES	= 0x00000002
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("5275F43A-4FF9-48b2-B88E-B2F842461AB3")]
	public interface IRender3D : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void SetPerspective(float fFovAngle, float fZNear, float fZFar);

		void SetColor(ref TColor4 stColor);
		void SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode);
		void ToggleAlphaTest([MarshalAs(UnmanagedType.U1)]bool bEnabled, float fAlphaMinTreshold);
		void ToggleDepthTest([MarshalAs(UnmanagedType.U1)]bool bEnabled);

		void SetMatrix(ref TMatrix stMatrix, [MarshalAs(UnmanagedType.U1)]bool bMult);
		void GetMatrix(out TMatrix stMatrix);

		void DrawAxes(float fSize, [MarshalAs(UnmanagedType.U1)]bool bNoDepthTest);

		void PushStates(E_PUSH_STATES_FLAGS eStates);
		void PopStates();

		void GetPoint3(ref TPoint2 stPointOnScreen, out TPoint3 stResultPoint, E_GET_POINT3_FLAG eFlag);
		void GetPoint2(ref TPoint3 stPoint, out TPoint2 stResultPointOnScreen);

		void FrustumSetup();
		void CullPoint(ref TPoint3 stCoords, [MarshalAs(UnmanagedType.U1)]ref bool bCull);
		void CullSphere(ref TPoint3 stCoords, float radius, [MarshalAs(UnmanagedType.U1)]ref bool bCull);
		void CullBox(ref TPoint3 stCenterCoords, ref TPoint3 stExtents, [MarshalAs(UnmanagedType.U1)]ref bool bCull);

		void CreateLight(out ILight prLight);
		void EnableLighting([MarshalAs(UnmanagedType.U1)]bool enable);
		void SetAmbient(ref TColor4 color);
		void GetAmbient(out TColor4 color);
	};

	//Light interface//

	public enum E_LIGHT_TYPE
	{
		LT_DIRECTIONAL = 0,
		LT_POINT
	}

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("EB73AC84-A465-4554-994D-8BED29744C9D")]
	public interface ILight: IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void SetEnabled([MarshalAs(UnmanagedType.U1)]bool bEnabled);
		void SetDiffuseColor(ref TColor4 stColor);
		void SetSpecularColor(ref TColor4 stColor);
		void SetPos(ref TPoint3 stPos);
		void SetDir(ref TPoint3 stDir);
		void SetRange(float range);
		void SetConstantAttenuation(float attenuation);
		void SetLinearAttenuation(float attenuation);
		void SetQuadraticAttenuation(float attenuation);
		void SetType(E_LIGHT_TYPE eType);
		void GetEnabled([MarshalAs(UnmanagedType.U1)]out bool bEnabled);
		void GetDiffuseColor(out TColor4 stColor);
		void GetSpecularColor(out TColor4 stColor);
		void GetPos(out TPoint3 stPos);
		void GetDir(out TPoint3 stDir);
		void GetRange(out float range);
		void GetConstantAttenuation(out float attenuation);
		void GetLinearAttenuation(out float attenuation);
		void GetQuadraticAttenuation(out float attenuation);

		void GetType(out E_LIGHT_TYPE eType);
		void Free();
	};

	//Texture interface//  


	//public interface ICoreTexture
	//{ };

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("85BDDBC2-F126-4cae-946D-7D6B079E5CCE")]
	public interface ITexture : IEngBaseObj
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion        

		void GetDimensions(out UInt32 irWidth, out UInt32 irHeight);
		void SetFrameSize(UInt32 uiFrameWidth, UInt32 uiFrameHeight);        
		void GetFrameSize(out UInt32 uiFrameWidth, out UInt32 uiFrameHeight); 
	   
		void GetCoreTexture(out ICoreTexture prCoreTex);


		void GetPixelData([MarshalAs(UnmanagedType.LPArray)] byte[] pubtData);
		void SetPixelData([MarshalAs(UnmanagedType.LPArray)] byte[] pubtData);

		void Draw2DSimple(Int32 iX, Int32 iY, UInt32 uiFrameIndex);
		void Draw2D(Int32 iX, Int32 iY, UInt32 uiWidth, UInt32 uiHeight, float fAngle, UInt32 uiFrameIndex);
		void Draw3D(UInt32 uiFrameIndex);

		void Bind(UInt32 uiMTextureLayer);
	};

	//Material interface//

	public enum E_TEX_MAPPING
	{
		TM_STD = 0,
		TM_OBJ_SPACE = 1
	};

	public enum E_NORMAL_TECHNIQUE
	{
		NT_UNPERTURBED = 0,
		NT_NORMAL_MAP_3_COMPONENT = 1,
		NT_HEIGHT_MAP_HW_DIFF = 3
	};

	public enum E_PARALLAX_TECHNIQUE
	{
		PT_NONE = 0,
		PT_SPHERE = 1,
		PT_PLANE = 2
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("B6506749-BB41-423d-B6C0-982081EF63F9")]
	public interface IMaterial: IEngBaseObj
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion   

		void SetAmbientColor(ref TColor4 stColor);
		void SetDiffuseColor(ref TColor4 stColor);
		void SetSpecularColor(ref TColor4 stColor);
		void SetHeightScale(float scale);
		void SetEnvAmount(float amount);
		void SetShininess(float shininess);
		void SetTexMappingMode(E_TEX_MAPPING eTexMapping);
		void SetDiffuseTexture(ITexture pTexture);
		void SetSpecularTexture(ITexture pTexture);
		void SetNormalTexture(ITexture pTexture);
		void SetHeightTexture(ITexture pTexture);
		void SetEnvTexture(E_NORMAL_TECHNIQUE technique);
		void SetEnvMask(ITexture pTexture);
		void SetNormalTechnique(E_NORMAL_TECHNIQUE technique);
		void SetParallaxTechnique(E_PARALLAX_TECHNIQUE technique);
		void GetAmbientColor(out TColor4 stColor);
		void GetDiffuseColor(out TColor4 stColor);
		void GetSpecularColor(out TColor4 stColor);
		void GetHeightScale(out float scale);
		void GetEnvAmount(out float amount);
		void GetShininess(out float shininess);
		void GetTexMappingMode(out E_TEX_MAPPING eTexMapping);
		void GetDiffuseTexture(out ITexture pTexture);
		void GetSpecularTexture(out ITexture pTexture);
		void GetNormalTexture(out ITexture pTexture);
		void GetHeightTexture(out ITexture pTexture);
		void GetEnvTexture(out ITexture pTexture);
		void GetEnvMask(out ITexture pTexture);
		void GetNormalTechnique(out E_NORMAL_TECHNIQUE technique);
		void GetParallaxTechnique(out E_PARALLAX_TECHNIQUE technique);

		void Bind();
	};


	//BitmapFont interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA")]
	public interface IBitmapFont : IEngBaseObj
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion        
	
		void GetTexture(out ITexture prTexture);
		void SetScale(ref float fScale);
		void GetScale(out float fScale);
		void GetTextDimensions([MarshalAs(UnmanagedType.LPStr)] string pcTxt, out UInt32 uiWidth, out UInt32 uiHeight);
		void Draw2DSimple(int iX, int iY, [MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor);
		void Draw2DRect(ref TRectF stRect, [MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor);

		void Draw2D(float fX, float fY, [MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor, float fAngle = 0, [MarshalAs(UnmanagedType.U1)]bool bVerticesColors = false, [MarshalAs(UnmanagedType.U1)]bool bHyperlink = false);
		void Draw3D([MarshalAs(UnmanagedType.LPStr)] string pcTxt);
	};

	//3D Objects interfaces//

	//Mesh interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("85E360A8-07B3-4f22-AA29-07C7FC7C6893")]
	public interface IMesh : IEngBaseObj
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion        

		void Draw();
		void GetCenter(out TPoint3 stCenter);
		void GetExtents(out TPoint3 stExtents);
		void GetTrianglesCount(out UInt32 uiTriCnt);
		void GetGeometryInfo(out UInt32 uiDataSize, out E_MESH_CREATION_FLAGS eCreationFlags);
		/*Format x,y,z*vertex_count, nx,ny,nz*vertex_count s,q*vertex_count tx,ty,tz,bx,by,bz*vertex_count int16*uiFacesCount*/
		void GetGeometry([MarshalAs(UnmanagedType.LPArray)] out byte[] pubtData); //???
		void SetGeometry([MarshalAs(UnmanagedType.LPArray)] byte[] pubtData);
	};

	//Skeleton xform interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("1D12C62F-9974-4219-8F12-DA92A4CC259F")]
	public interface ISkeletonXform: IEngBaseObj
	{

	};

	//Model interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("6107C296-FC07-48d1-B6A7-F88CC2DAE897")]
	public interface IModel : IEngBaseObj
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion
		
		void MeshsCount(out UInt32 uiCount);
		void GetMesh(UInt32 uiIdx, out IMesh prMesh);
		void Draw(float fFrameIdx);
	};

		//Postprocess interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("DC6E4812-6B06-4de8-9DEF-2A13740BC45E")]
	public interface IPostprocess: IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void Free();

		void SetTargets(ITexture pSrc, ITexture pDst);
		void SetBlurAmount(float fBlur);
		void SetBloomAmount(float fBloom);
		void SetBloomThreshold(float fThreshold);
		void ToggleMonochrome([MarshalAs(UnmanagedType.U1)]bool bEnable);
		void Perform();
	};

//Input Subsystem//

	public enum E_INPUT_CONFIGURATION_FLAGS
	{
		ICF_DEFAULT = 0x00000000,
		ICF_EXCLUSIVE = 0x00000001,
		ICF_BLOCK_WINKEYS = 0x00000002,
		ICF_HIDE_CURSOR = 0x00000004,
		ICF_CURSOR_BEYOND_SCREEN = 0x00000008
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("64DAAF7F-F92C-425f-8B92-3BE40D8C6666")]
	public interface IInput : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngineSubSystem
		new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
		#endregion

		void Configure(E_INPUT_CONFIGURATION_FLAGS eFlags);

		void GetMouseStates(out TMouseStates stMStates);
		void GetKey(E_KEYBOARD_KEY_CODES eKeyCode, [MarshalAs(UnmanagedType.U1)] out bool bPressed);
		void GetKeyPressed(out E_KEYBOARD_KEY_CODES eKey);
		void GetKeyName(E_KEYBOARD_KEY_CODES eKeyCode, [MarshalAs(UnmanagedType.LPStr)] out char cASCIICode);

		void BeginTextInput([MarshalAs(UnmanagedType.LPStr)] string pcBuffer, UInt32 uiBufferSize);
		void EndTextInput();

		void GetJoystickCount(out UInt32 uiCount);
		void GetJoystickStates(UInt32 uiJoyId, out TJoystickStates stJoyStates);
		void GetJButtonPressed(UInt32 uiJoyId, UInt32 uiNumber, [MarshalAs(UnmanagedType.U1)]out bool bPressed); // 128 buttons [num 0..127]	
	};

//Sound SubSystem interfaces//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("054C07EE-2724-42f2-AC2B-E81FCF5B4ADA")]
	public interface ISound : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngineSubSystem
		new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
		#endregion

		/*Dummy*/
		void SetMasterVolume(UInt32 uiVolume);
		void MasterPause([MarshalAs(UnmanagedType.U1)]bool bPaused);
		void StopAllChannels();
		void GetMaxChannelsCount(out uint uiCount);
		void GetFreeChannelsCount(out uint uiCount);
		void ReleaseChannelsByData([MarshalAs(UnmanagedType.LPArray)] byte[] pData);	
		void CreateChannel(out ISoundChannel prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 ui32DataSize); //Data not copied!

// can't implement delegate for func // phomm

		//void CreateStreamableChannel(out ISoundChannel prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, UInt32 ui32DataSize, void (DGLE_API *pStreamCallback)(void *pParametr, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize), void *pParametr);

	};

	//SoundSample interface//

	public enum E_SOUND_CHANNEL_EFFECTS
	{
		SCE_NONE		= 0x00000000,
		SCE_CHORUS 		= 0x00000001,
		SCE_COMPRESSOR 	= 0x00000002,
		SCE_DISTORTION 	= 0x00000004,
		SCE_ECHO 		= 0x00000008,
		SCE_FLANGER 	= 0x00000010,
		SCE_GARGLE 		= 0x00000020,
		SCE_REVERB 		= 0x00000040
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("DE6F7CDD-8262-445c-8D20-68E3324D99A6")]
	public interface ISoundChannel : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void Play(bool bLooped);
		void Pause();
		void Stop();
		void IsPlaying([MarshalAs(UnmanagedType.U1)]out bool bIsPlaying);
		void SetVolume(UInt32 uiVolume); //from 0 to 100
		void GetVolume(out UInt32 uiVolume);
		void SetPan(int iPan); //from -100 to 100
		void GetPan(out int iPan);
		void SetSpeed(uint uiSpeed); // in percents
		void GetSpeed(out uint uiSpeed);
		void SetCurrentPosition(uint uiPos);
		void GetCurrentPosition(out uint uiPos);
		void GetLength(out uint uiLength);
		void IsStreamable(out bool bStreamable);
		void Unaquire();
	};
	
	public enum E_SOUND_SAMPLE_PARAMS
	{
		SSP_NONE			= 0x00000000,
		SSP_LOOPED			= 0x00000001,
	};

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("30DD8C94-D3FA-40cf-9C49-649211424919")]
	public interface  ISoundSample : IEngBaseObj
	{

		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngBaseObj
		new void Free();
		new void GetType(out E_ENG_OBJ_TYPE eEngObjType);
		new void GetUnknownType(out uint uiObjUnknownType);
		#endregion        

		void Play();
		void PlayEx(out ISoundChannel pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags); //pSndChnl must be checked on nul
	};

//FileSystem interfaces//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("4850286F-4770-4bcf-A90A-33D7BE41E686")]
	public interface IMainFileSystem : IEngineSubSystem
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		#region IEngineSubSystem
		new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
		#endregion

		void LoadFile([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IFile prFile);// c:\data.zip|img.jpg
		void GetVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, out IFileSystem vfs);		
		void RegisterVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, [MarshalAs(UnmanagedType.LPStr)] string pcDescription, ref IFileSystem pVFS, [MarshalAs(UnmanagedType.FunctionPtr)] DFSDeleteCallback pProc, IntPtr pParam);
		void UnregisterVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension);
		void GetRegisteredVirtualFileSystems(IntPtr pcTxt, out UInt32 uiCharsCount);
		void GetVirtualFileSystemDescription([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, IntPtr pcTxt, out UInt32 uiCharsCount);
	};

	public enum E_FIND_FLAGS
	{
		FF_RECURSIVE = 1
	};

	public enum E_FILE_SYSTEM_OPEN_FLAGS
	{
		FSOF_READ   = 0x00000001,
		FSOF_WRITE  = 0x00000002,
		FSOF_TRUNC  = 0x00000004,
		FSOF_BINARY = 0x00000008
	};

	public enum E_FILE_SYSTEM_SEEK_FLAG
	{
		FSSF_BEGIN = 0,
		FSSF_CURRENT,
		FSSF_END
	};

	//File interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("AE6E8AE7-3E5B-4bc4-A512-42E1CF1DF005")]
	public interface IFile : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void Read(IntPtr pBuffer, UInt32 uiCount, out UInt32 uiRead);
		void Write(IntPtr pBuffer, UInt32 uiCount, out UInt32 uiWritten);
		void Seek(UInt32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG Way, out UInt32 ui32Position);
		void GetSize(out UInt32 ui32Size);
		void IsOpen([MarshalAs(UnmanagedType.U1)] out bool bOpened);        
		void GetName(IntPtr pcName, out UInt32 uiCharsCount);
		void GetPath(IntPtr pcPath, out UInt32 uiCharsCount);
		void Free();
	};

	//FileIterator interface//

	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("5D73F249-0E74-4cc5-9646-270CB1E22750")]
	public interface IFileIterator : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void FileName(IntPtr pcName, out uint uiLength);
		void Next();
		void Free();
	};

	//FileSystem interface//
	
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
	Guid("2DAE578E-9636-4fae-BABB-7D835EEA7518")]
	public interface IFileSystem : IDGLE_Base
	{
		#region IDGLE_Base
		new void GetGUID(ref Guid guid);
		#endregion

		void OpenFile([MarshalAs(UnmanagedType.LPStr)] string pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, out IFile prFile);	
		void DeleteFile([MarshalAs(UnmanagedType.LPStr)] string pcName);
		void FileExists([MarshalAs(UnmanagedType.LPStr)] string pcName, [MarshalAs(UnmanagedType.U1)] out bool bExists);
		void Find([MarshalAs(UnmanagedType.LPStr)] string pcMask, E_FIND_FLAGS eFlags, out IFileIterator prIterator);
	};

	//Initialization and Relese Engine//

	public static class Engine
	{
		public enum E_GET_ENGINE_FLAGS
		{
		GEF_DEFAULT				= 0x00000000,	
		GEF_FORCE_SINGLE_THREAD	= 0x00000001,	
		GEF_FORCE_NO_LOG_FILE	= 0x00000002,
		GEF_FORCE_QUIT			= 0x00000004
		};

		delegate void GetEngineDelegate(out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags, byte ubtSDKVer);
		delegate void FreeEngineDelegate(IEngineCore pEngineCore);
		
		static GetEngineDelegate pGetEngine;
		static FreeEngineDelegate pFreeEngine;
		static IntPtr hServer = IntPtr.Zero;

		const string    _DGLE_VER_     = "Beta 1 RC4";
		const byte      _DGLE_SDK_VER_ = 1;

		public static bool GetEngine([MarshalAs(UnmanagedType.LPStr)] string pcDllFileName, out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags)
		{
			pEngineCore = null;

			if (hServer == IntPtr.Zero)
			{
				if (hServer == IntPtr.Zero)
				{
					hServer = Imports.LoadLibrary(pcDllFileName);
					if (hServer == IntPtr.Zero) return false;
				}
				if (pGetEngine == null && pFreeEngine == null)
				{
					IntPtr pGetEnginePointer = Imports.GetProcAddress(hServer, "CreateEngine");
					pGetEngine = Marshal.GetDelegateForFunctionPointer(pGetEnginePointer, typeof(GetEngineDelegate)) as GetEngineDelegate;
					IntPtr pFreeEnginePointer = Imports.GetProcAddress(hServer, "FreeEngine");
					pFreeEngine = Marshal.GetDelegateForFunctionPointer(pFreeEnginePointer, typeof(FreeEngineDelegate)) as FreeEngineDelegate;
					if (pGetEngine == null || pFreeEngine == null)
					{
						Imports.FreeLibrary(hServer);
						hServer = IntPtr.Zero;
						return false;
					}
				}

				CreateEngine(out pEngineCore, eFlags);
				return (pEngineCore != null);
			}
			else
				return false;
		}

		public static bool CreateEngine(out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags)
		{
			if (pGetEngine == null)
			{
				pEngineCore = null;
				return false;
			}
			pGetEngine(out pEngineCore, eFlags, _DGLE_SDK_VER_);
			return true;
		}

		public static bool FreeEngine(IEngineCore pEngineCore, bool bFreeDLL)
		{
			if(pEngineCore != null)
				pFreeEngine(pEngineCore);
			if (bFreeDLL && hServer != IntPtr.Zero)
			{
				Imports.FreeLibrary(hServer);
				hServer = IntPtr.Zero;
				pGetEngine = null;
				pFreeEngine = null;
			}
			return true;
		}

		public static bool GetEngine([MarshalAs(UnmanagedType.LPStr)] string pcDllFileName, out IEngineCore pEngineCore) { return GetEngine(pcDllFileName, out pEngineCore, E_GET_ENGINE_FLAGS.GEF_DEFAULT); }
		public static void FreeEngine() { if (hServer != IntPtr.Zero) { Imports.FreeLibrary(hServer); hServer = IntPtr.Zero; } }

		static class Imports
		{
			[DllImport("kernel32.dll")]
			public static extern IntPtr LoadLibrary(string lpFileName);

			[DllImport("kernel32.dll", CharSet = CharSet.Ansi, ExactSpelling = true)]
			public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

			[DllImport("kernel32.dll", SetLastError = true)]
			public static extern bool FreeLibrary(IntPtr hModule);
		}
	}
}