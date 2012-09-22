/**
\file		DGLE2.h
\author		Korotkov Andrey aka DRON
\version	0.3.0 beta
\date		XX.XX.2012 (c)Korotkov Andrey

\brief		Main DGLE2 engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE2 SDK.
*/

/** 
\mainpage	DGLE2 Main Help Page
 
\section	intro_sec Introduction

- DGLE2 is a powerful cross platform engine for 2D/3D games and real-time visualizations. Young, strong and crazy!
- DGLE2 is opensource project and free to use (under the terms of license, see below).
- DGLE2 is based on widely spread open standarts, formats and APIs such as OpenGL, OpenAL, OpenCL, OGG Vorbis, Lua, Box2D, Bullet physics, Mono, e.t.c..

\image html Open_Standarts.png

This file is official engine documentation for C++ programmers (DGLE2 users).
This documentation is also suitable for any supported programming language.

\section	aim_sec The aim of the project

The goal of the project is to provide developers with flexible & extandable cross platform easy-to-learn professional technology,
capable of building any 2D/3D games, real-time visualizations, scientific applications etc. It should be easy to make great projects
only by using editors and scripts or go deeper and use your programming skills of your favorite language to create really exciting projects.
Users can also add new formats and functionality by creating new plugins for engine and share them with others over the web or just get
plugins already made by others. When project is complete it can be easily build for every popular platform.

\section gstart_sec Getting started

Simple steps to make your first DGLE2 application.

- 1. Include this header ("DGLE2.h") to your project.
- 2. Connect DGLE2 namespace to your project (ex. "using namespace DGLE2;").
- 3. Paste "ENG_DYNAMIC_FUNC" macros to your main source file (ex. "main.cpp").
- 4. Declare pointer to IEngineCore class like "IEngineCore *pEngineCore".
- 5. Call "GetEngine" function to retrieve IEngineCore pointer. For dynamic library it should be like this "GetEngine("DGLE2.dll", pEngineCore);".
- 6. Now you can use IEngineCore methods. For example, you can initialize engine like this "pEngineCore->InitializeEngine(NULL, "HelloWorld");".
- 7. After engine initialization you should call "pEngineCore->StartEngine();" to start engine.
- 8. When you are done with engine don't forget to call "FreeEngine()" routine before exit.

Please see "HelloWorld" sample code at the "Examples" tab of this help file.

You can browse ".\src\examples" folder or go to "Examples" tab of this manual for futher information.

\section license_sec License

DGLE2 is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DGLE2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.

\section dev_letter_sec The letter from the developers

If you are using DGLE2 in your project and you like it. Please do not remove DGLE2 splash screen
and paste the link to the official DGLE2 website in your program credits section or elsewhere.
It is the easiest way you can say "Thank you for your work!" to DGLE2 project developers.
Also we would be happy for any donations to support futher project development.

\section additional_sec Additional help and information

Visit official DGLE2 engine website http://dgle.dronprogs.org/ for additional information.
There you can download latest version, gain help, support project or just say your oppinion or ask question at official DGLE2 forum ( http://forum.dronprogs.org/viewforum.php?f=3 ).

Feel free to write directly to project leader on e-mail dgle@dronprogs.org.

\example HelloWorld.cpp
This is simplest DGLE2 application for Windows.
\note To make this sample work you should copy "DGLE2.h" header to your project source directory and place DGLE2 library file (ex. "DGLE2.dll" for Windows) in your project output folder. 

*/

#ifndef DGLE2_HEADER
#define DGLE2_HEADER

//Engine version defines//

/** Defines DGLE2 version string. 
	\warning Do not edit!
*/
#define _DGLE2_VER_	"0.3.0 beta"
 
/** Defines DGLE2 version integer. 
	\warning Do not edit!
*/
#define _DGLE2_SDK_VER_ 1

/** Defines the current version of plugin SDK. 
	\warning Do not edit!
*/
#define _DGLE2_PLUGIN_SDK_VER_ _DGLE2_SDK_VER_

//Platform definition macros//

#if defined(_WIN32) || defined(_WIN64)

//Platform Windows//

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#include <Windows.h>

/** Internal engine define, shows that target platform is Windows.*/
#define PLATFORM_WINDOWS

/** If defined, all interfaces will be derived from IUnknown. */
#define DGLE2_USE_COM

/** Define calling convention used by engine. */
#define DGLE2_API APIENTRY

/** If defined, all structures will be aligned by 1 byte. */
#define STRUCT_ALIGNMENT_1

#else//_WIN32 or _WIN64

//Unknown platform//
#error Unknown platform!

#endif

#include "DGLE2_types.h"

/** Engines main namespace. */
namespace DGLE2
{

//Engine Base interface//

	/** Engine base fundamental interface.
		Any engine interface must be derived from this interface.
		\attention On Windows platform IDGLE2_Base is derived from IUnknown for more flexibility and compatibility,
		but DGLE2 doesn't provides real COM technology. The reference counter is always 1,
		"Release" and "AddRef" methods are dummies, "QueryInterface" can return pointer only
		to IUnknown or to the last interface in the inheritance chain.
	*/
	class IDGLE2_Base
#if defined(PLATFORM_WINDOWS) && defined(DGLE2_USE_COM)
		: public IUnknown
#endif
	{
	public:
		/** Returns uniq identifier of the last interface in the inheritance chain.
			\param[out] guid Uniq interface identifier.
			\return Always returnes DGLE2_types.h::S_OK.
		 */		
		virtual HRESULT DGLE2_API GetGUID(GUID &guid) = 0;
	};

//Engine SubSystem interface//

	/** Engine subsystems types. */
	enum E_ENGINE_SUB_SYSTEM
	{
		ESS_RENDER = 0,			/**< Main rendering subsystem. \see IRender */
		ESS_INPUT,				/**< Input subsystem. \see IInput */
		ESS_SOUND,				/**< Sound subsystem. \see ISound */
		ESS_RESOURCE_MANAGER,	/**< Resource manager subsystem. \see IResourceManager */
		ESS_FILE_SYSTEM,		/**< Main file system(manager of virtual file systems). \see IMainFileSystem */
		ESS_CORE_RENDERER		/**< Low-level base rendering interface. \see ICoreRenderer */
	};

	// {C682F875-E0BD-4af9-B79C-E209850025F8}
	static const GUID IID_IEngineSubSystem = 
	{ 0xc682f875, 0xe0bd, 0x4af9, { 0xb7, 0x9c, 0xe2, 0x9, 0x85, 0x0, 0x25, 0xf8 } };

	/** Base interface of any engine subsystem. */
	class IEngineSubSystem : public IDGLE2_Base
	{
	public:
		/** Returns type of subsystem. 
			\param[out] eSubSystemType Type of the subsystem to which you may cast this interface pointer.
			\return Always returnes DGLE2_types.h::S_OK.	
		*/
		virtual HRESULT DGLE2_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType) = 0;
	};

//Engine Plugin interface//

	// {B94E0E40-8885-41dd-8BC5-4CD663AE5709}
	static const GUID IID_IPlugin = 
	{ 0xb94e0e40, 0x8885, 0x41dd, { 0x8b, 0xc5, 0x4c, 0xd6, 0x63, 0xae, 0x57, 0x9 } };

	/** Base interface of any engine plugin. */
	class IPlugin : public IDGLE2_Base
	{
	public:
		/** Returns structure with plugin description.
			\param[out] stInfo Structure in which plugin description will be stored.
		*/
		virtual HRESULT DGLE2_API GetPluginInfo(TPluginInfo &stInfo) = 0;
		/** Returns the name of interface which plugin implements or empty string if it implements nothing.
			\param[out] pcName Pointer to allocated string.
			\param[in] uiCharsCount Count of the chars in allocated string.
			\return E_INVALIDARG must be returned if allocated string is too small. 
		*/
		virtual HRESULT DGLE2_API GetPluginInterfaceName(char* pcName, uint uiCharsCount) = 0;
	};

//Engine Subsystem Plugin interface//

	// {27908E31-8D8E-4076-AE33-087A1BE5DCB3}
	static const GUID IID_ISubSystemPlugin = 
	{ 0x27908e31, 0x8d8e, 0x4076, { 0xae, 0x33, 0x8, 0x7a, 0x1b, 0xe5, 0xdc, 0xb3 } };

	/** Base interface of any engine core subsystem plugin. */
	class ISubSystemPlugin : public IPlugin
	{
	public:
		/** Returns interface of subsystem realised in this plugin.
			\param[out] prSubSystem Interface of the subsystem.
		*/
		virtual HRESULT DGLE2_API GetSubSystemInterface(IEngineSubSystem *&prSubSystem) = 0;
	};

//Engine Base Object interface//

	/** Types of engine objects. */
	enum E_ENG_OBJ_TYPE
	{
		EOT_UNKNOWN = 0,		/**< Undefined or custom object type. */ 
		EOT_TEXTURE,			/**< Texture. \see ITexture*/ 
		EOT_MATERIAL,			/**< Material. \see IMaterial */ 
		EOT_MESH,				/**< Mesh. \see IMesh */ 
		EOT_MODEL,				/**< Model. \see IModel*/ 
		EOT_BITMAP_FONT,		/**< Bitmap font. \see IBitmapFont */ 
		EOT_PARTICLE_EFFECT,	/**< Particle effect. \see IParticleEffect*/ 
		EOT_SOUND_SAMPLE,		/**< Sound sample \see ISoundSample */ 
		EOT_MUSIC,				/**< Music. \see IMusic*/ 
		EOT_SPRITE,				/**< Sprite. \see ISprite */ 
		EOT_GUI_FORMS			/**< Collection of user interface forms. \see IGUIForms */ 
	};

	// {C010239A-6457-40f5-87EF-FAA3156CE6E2}
	static const GUID IID_IEngBaseObj = 
	{ 0xc010239a, 0x6457, 0x40f5, { 0x87, 0xef, 0xfa, 0xa3, 0x15, 0x6c, 0xe6, 0xe2 } };

	/** Base interface of any engine object. 
		Engine objects are commonly loaded from files by Resource Manager subsystem.
		\see IResourceManager
	*/
	class IEngBaseObj : public IDGLE2_Base
	{
	public:
		/** Releases object and deallocates memory. Also removes it from IResourceManager lists. 
			After calling Free() method you can safely null the pointer to the object.
		*/
		virtual HRESULT DGLE2_API Free() = 0;
		/** Returns type of object. 
			\param[out] eObjType Type of the object to which you may cast this interface pointer.
			\return Always returnes DGLE2_types.h::S_OK.	
		*/
		virtual HRESULT DGLE2_API GetType(E_ENG_OBJ_TYPE &eObjType) = 0;
		/** In case object type is EOT_UNKNOWN, you can use this function to get specific object type id.
			\param[out] uiObjUnknownType Integer with unique object type index. Meaning of these indexes must be provided by the developer of specific object type.
			\return Returnes DGLE2_types.h::S_FALSE if object is not of EOT_UNKNOWN type and DGLE2_types.h::S_OK otherwise.	
		*/
		virtual HRESULT DGLE2_API GetUnknownType(uint &uiObjUnknownType) = 0;
	};

//Events Interfaces//

	/** Types of engine events. 
		\see IBaseEvent
	 */
	enum E_EVENT_TYPE
	{
		ET_UNKNOWN = 0,				/**< Undefined or custom event type. */ 
		ET_BEFORE_INIT,				/**< Event occures just before engine will call its initialization routines. \see IEvBeforeInit */ 
		ET_BEFORE_RENDER,			/**< Event occures before every frame. */ 
		ET_AFTER_RENDER,			/**< Event occures after every frame. */ 
		ET_ON_PROFILER_DRAW,		/**< It is a special event on which you can render some text information on screen. \note If you want to output some statistic or profiling information use this event and special RenderProfilerTxt method. \see IEngineCore::RenderProfilerTxt */ 
		ET_ON_WIN_MESSAGE,			/**< Event occures every time when window receives message. Use this event to hook engine window messages. \see IEvWinMessage */ 
		ET_ON_GET_SSYSTEM,			/**< Event occures when someone calls IEngineCore::GetSubSystem method and you can substitute any subsystem by your own realisation. \see IEvGetSubSystem */ 
		ET_ON_ENGINE_FATAL_MESSAGE	/**< Event occures on engine fatal error. \see IEvFatalMessage */ 
	};

	// {6DFEF982-AADF-42e9-A369-378BDB31404A}
	static const GUID IID_IBaseEvent = 
	{ 0x6dfef982, 0xaadf, 0x42e9, { 0xa3, 0x69, 0x37, 0x8b, 0xdb, 0x31, 0x40, 0x4a } };

	/** Base interface of any engine event. 
		\see IEngineCore::AddEventListner, IEngineCore::RemoveEventListner
	 */
	class IBaseEvent : public IDGLE2_Base
	{
	public:
		/** Returns type of event. 
			\param[out] eEvType Type of the event. You may cast this interface pointer to special event interface if such exists.
			\return Always returnes DGLE2_types.h::S_OK.	
		*/
		virtual HRESULT DGLE2_API GetEventType(E_EVENT_TYPE &eEvType) = 0;
		/** In case event type is ET_UNKNOWN, you can use this function to get specific event type id.
			\param[out] uiUnknEvType Integer with unique event type index. Meaning of these indexes must be provided by the developer of specific event type.
			\return Returnes DGLE2_types.h::S_FALSE if event is not of ET_UNKNOWN type and DGLE2_types.h::S_OK otherwise.	
		*/
		virtual HRESULT DGLE2_API GetUnknownEventType(uint &uiUnknEvType) = 0;
	};

	// {EB735739-3D12-4522-B6D7-EEE3225DF934}
	static const GUID IID_IEvBeforeInit = 
	{ 0xeb735739, 0x3d12, 0x4522, { 0xb6, 0xd7, 0xee, 0xe3, 0x22, 0x5d, 0xf9, 0x34 } };

	enum E_ENGINE_INIT_FLAGS;

	/** Event occures just before engine will call its initialization routines.
		On this event tou can hook engine init parametrs.
		\see ET_BEFORE_INIT, IEngineCore::InitializeEngine, IBaseEvent
	 */
	class IEvBeforeInit : public IBaseEvent
	{
	public:
		/** Sets new engine initialization parametrs.
			\param[in] stWindowParam New engine window structure to replace current.
			\param[in] eInitFlags New engine initialization flags to replace current.
		 */
		virtual HRESULT DGLE2_API SetEngParams(const TEngWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags) = 0;
		/** Retrieves current engine initialization parametrs. 
			\param[in] stWindowParam Current engine window structure.
			\param[in] eInitFlags Current engine initialization flags.
		 */
		virtual HRESULT DGLE2_API GetEngParams(TEngWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags) = 0;
	};
	
	// {DAA4E3BC-C958-4def-B603-F63EEC908226}
	static const GUID IID_IEvFatalMessage = 
	{ 0xdaa4e3bc, 0xc958, 0x4def, { 0xb6, 0x3, 0xf6, 0x3e, 0xec, 0x90, 0x82, 0x26 } };

	/** Event occures on engine fatal error.
		Also application errors such as "Access Violation" are handled by this event if EIF_CATCH_UNHANDLED
		flag is set in IEngineCore::InitializeEngine initialization flags.
		By handling this event 
		\see ET_ON_ENGINE_FATAL_MESSAGE, LT_FATAL, IBaseEvent
	 */
	class IEvFatalMessage : public IBaseEvent
	{
	public:
		/** Returns the fatal message text.
		 \param[out] pcTxt Pointer to allocated string.
		 \param[in] uiCharsCount Count of the chars in allocated string.
		 \return E_INVALIDARG must be returned if allocated string is too small. 
		 */		
		virtual HRESULT DGLE2_API GetMessageTxt(char *pcTxt, uint uiCharsCount) = 0;
		/** Suspends all engine threads and pauses all engine routines. 
			\param[in] bFreeze Suspends if true or resumes if fales engine threads and routines.
		 */
		virtual HRESULT DGLE2_API FreezeEngine(bool bFreeze) = 0;
		/** Forces engine not to show error message and console. 
			\note If you decided not to show error message you should inform user about error somehow.
		 */
		virtual HRESULT DGLE2_API ForceNoMessage() = 0;
		/** Forces engine to ignore current error and tries to continue. 
			\warning Use with care.
		 */
		virtual HRESULT DGLE2_API ForceIgnoreError() = 0;
	};

	// {8D718E48-581D-4cbb-9C40-C04998106F8D}
	static const GUID IID_IEvWinMessage = 
	{ 0x8d718e48, 0x581d, 0x4cbb, { 0x9c, 0x40, 0xc0, 0x49, 0x98, 0x10, 0x6f, 0x8d } };

	/** Event occures every time when window receives message. 
		Use this event to hook window messages.
		\see ET_ON_WIN_MESSAGE, IBaseEvent
	 */
	class IEvWinMessage : public IBaseEvent
	{
	public:
		/** Retrieves window message.
			\param[out] stWinMsg Structure with current message information.
		 */
		virtual HRESULT DGLE2_API GetWinMessage(TWinMessage &stWinMsg) = 0;
	};

	// {2B6D2547-716E-490c-B1F1-422CB428738F}
	static const GUID IID_IEvGetSubSystem = 
	{ 0x2b6d2547, 0x716e, 0x490c, { 0xb1, 0xf1, 0x42, 0x2c, 0xb4, 0x28, 0x73, 0x8f } };

	/** Event occures when someone calls IEngineCore::GetSubSystem method.
		You can substitute any subsystem by your own realisation on this event.
		\see ET_ON_GET_SSYSTEM, IBaseEvent
	 */
	class IEvGetSubSystem : public IBaseEvent
	{
	public:
		/** Returns subsystem type which user is trying to retrieve. 
			\param[out] eSubSystem Type of retrieving subsystem.
		 */
		virtual HRESULT DGLE2_API GetSubSystemType(E_ENGINE_SUB_SYSTEM eSubSystem) = 0;
		/** Substitutes engine subsystem by custom one. 
			\param[int] pSubSystem Pointer to subsystem interface with which retrieving subsystem will be substituted.
		 */		
		virtual HRESULT DGLE2_API OverrideSubSystem(IEngineSubSystem *pSubSystem) = 0;
	};

//Main Engine System//

	// {371B1338-BB25-4B8C-BD6A-BCDF241CC52C}
	static const GUID IID_IUserCallback = 
	{ 0x371b1338, 0xbb25, 0x4b8c, { 0xbd, 0x6a, 0xbc, 0xdf, 0x24, 0x1c, 0xc5, 0x2c } };

	class IUserCallback : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API Initialize() = 0;
		virtual HRESULT DGLE2_API Free() = 0;
		virtual HRESULT DGLE2_API Update(uint64 ui64DeltaTime) = 0;
		virtual HRESULT DGLE2_API Render() = 0;
		virtual HRESULT DGLE2_API OnEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent) = 0;
	};

	/** Type of engine callbacks. 
		IEngineCore can register calbacks of these types.
		\see IEngineCore::AddProcedure, IEngineCore::RemoveProcedure
	 */
	enum E_ENGINE_PROCEDURE_TYPE
	{
		EPT_UPDATE = 0,		/**< Procedure is called periodically(like on timer event). Interval of calling is set on engine initialization. In this procedure you should do any application computes. \see IEngineCore::InitializeEngine */
		EPT_RENDER,			/**< Procedure is called when engine decides to draw new frame. In this procedure you can call any rendering routines. */
		EPT_INIT,			/**< Procedure is called before engine will start its main loop. In this procedure you should load all resources needed by your application. */
		EPT_FREE			/**< Procedure is called before engine quits. In this procedure you should release all resources and free memmory. */
	};

	/** Type of engine log message.
		\see IEngineCore::AddToLogEx
	 */
	enum E_LOG_TYPE
	{
		LT_INFO = 0,	/**< Information message do not need user attention. */
		LT_WARNING,		/**< Information message do not need user attention but marks in engine log as warning and increments warnings counter. */
		LT_ERROR,		/**< Error message do not need user attention but marks in engine log as error and increments errors counter. Error message also includes information about source file and line. */
		LT_FATAL		/**< Fatal message displays user error message and then terminates engine with exit code 2. Also ET_ON_ENGINE_FATAL_MESSAGE event is generated. */
	};

	/** Engine initialization flags. 
		\see IEngineCore::InitializeEngine
	 */
	enum E_ENGINE_INIT_FLAGS
	{
		EIF_DEFAULT				= 0x00000000,	/**< Use default settings. */
		EIF_CATCH_UNHANDLED		= 0x00000001,	/**< All user callbacks will be executed in safe mode and engine will catch any unhandled errors. Engine will convert cached errors to engine fatal errors. Also ET_ON_ENGINE_FATAL_MESSAGE event will be generated. */
		EIF_FORCE_NO_SOUND		= 0x00000002,	/**< Sound subsystem will not be initialized. */
		EIF_LOAD_ALL_PLUGINS	= 0x00000004,	/**< Engine will try to connect any found *.dplug files near it and in all inner folders. \note DGLE2_EXT.dplug is connected automatically without this flag as well. */
		EIF_FORCE_LIMIT_FPS		= 0x00000010,	/**< Engine will limit its FPS(frames per second) not to overload CPU. FPS is limited to engine process interval(uiProcessInterval). \note Recommended for casual games and desktop applications. */
		EIF_FORCE_16_BIT_COLOR	= 0x00000020,	/**< Forces engine to use 16 bit color depth instead of 32 bit by default. \note Not recommended. */
		EIF_DISABLE_SMART_TIMING= 0x00000040,	/**< In some cases engine may call EPT_PROCESS several times at once to reduce lags. This flag will disable this feature. */
		EIF_NO_SPLASH			= 0x10000000	/**< This flag will disable engine splash screen. Splash screen is displayed to the user while engine prepare itself and while user initialization procedure is being processed. \note Turning off splash screen is not recommended because the user could be confused while being waiting application execution. */
	};

	// {111BB884-2BA6-4e84-95A5-5E4700309CBA}
	static const GUID IID_IEngineCore = 
	{ 0x111bb884, 0x2ba6, 0x4e84, { 0x95, 0xa5, 0x5e, 0x47, 0x0, 0x30, 0x9c, 0xba } };

	/** Main engine interface.
		Pointer to this interface is retrieved directly from DGLE2 library.
		\see ENG_DYNAMIC_FUNC
	 */
	class IEngineCore : public IDGLE2_Base
	{
	public:
		/** Set engine splash window picture.
		 \param[in] pcBmpFileName File name of the BMP file with picture to be set.
		 \note You can use this method only before calling InitializeEngine.
		 */
		virtual HRESULT DGLE2_API LoadSplashPicture(const char *pcBmpFileName) = 0;	

		/** Adds plugin to engine initialization list. This means that plugin will be loaded on engine initialization. This is the only correct way to setup Render, Sound, Input or other system plugins.
		 \param[in] pcFileName File name of the plugin.
		 \note Standart DGLE2_EXT plugin will be connected automatically (if found), so you don't need to add it to initialization list.
		 */
		virtual HRESULT DGLE2_API AddPluginToInitList(const char *pcFileName) = 0;	

		/** Initialize engine and all of its subroutines. Also creates main engine window.
		 \param[in] tHandle Handle of some already created window control to render in or NULL in case to let engine create it's own window.
		 \param[in] pcApplicationName Caption of main engine window.
		 \param[in] stWindowParam Structure with some window properties.
		 \param[in] uiProcessInterval Interval in milliseconds between calling of user process routine. \see EPT_PROCESS
		 \param[in] eInitFlags Special engine configuration flags.
		 */
		virtual HRESULT DGLE2_API InitializeEngine(TWinHandle tHandle, const char* pcApplicationName, const TEngWindow &stWindowParam = TEngWindow(), uint uiProcessInterval = 33, E_ENGINE_INIT_FLAGS eInitFlags = EIF_DEFAULT ) = 0;
		
		/** Change interval of calling user process routine after engine has been started. \see EPT_PROCESS
		 \param[in] uiProcessInterval Interval in milliseconds.
		 \see InitializeEngine
		 */
		virtual HRESULT DGLE2_API SetProcessInterval(uint uiProcessInterval) = 0;	
		virtual HRESULT DGLE2_API StartEngine() = 0;
		virtual HRESULT DGLE2_API QuitEngine() = 0;

		virtual HRESULT DGLE2_API ConnectPlugin(const char* pcFileName, IPlugin *&prPlugin) = 0;	
		virtual HRESULT DGLE2_API DisconnectPlugin(IPlugin *pPlugin) = 0;	
		virtual HRESULT DGLE2_API GetPlugin(const char* pcPluginName, IPlugin *&prPlugin) = 0;	

		virtual HRESULT DGLE2_API AddUserCallback(IUserCallback *pUserCallback) = 0;
		virtual HRESULT DGLE2_API RemoveUserCallback(IUserCallback *pUserCallback) = 0;

		virtual HRESULT DGLE2_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE2_API *pProc)(void *pParametr), void *pParametr = NULL) = 0;
		virtual HRESULT DGLE2_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE2_API *pProc)(void *pParametr), void *pParametr = NULL) = 0;

		virtual HRESULT DGLE2_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent) = 0;
		virtual HRESULT DGLE2_API AddEventListner(E_EVENT_TYPE eEventType, void (DGLE2_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr = NULL) = 0;
		virtual HRESULT DGLE2_API RemoveEventListner(E_EVENT_TYPE eEventType, void (DGLE2_API *pListnerProc)(void *pParametr, IBaseEvent *pEvent), void *pParametr = NULL) = 0;

		virtual HRESULT DGLE2_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem) = 0;

		virtual HRESULT DGLE2_API RenderProfilerTxt(const char* pcTxt, const TColor4 &stColor = TColor4()) = 0;
		virtual HRESULT DGLE2_API GetInstanceIdx(uint &uiIdx) = 0;
		virtual HRESULT DGLE2_API GetTimer(uint64 &uiTick) = 0;
		virtual HRESULT DGLE2_API GetSystemInfo(TSystemInfo &stSysInfo) = 0;
		virtual HRESULT DGLE2_API GetCurrentWin(TEngWindow &stWin) = 0;
		virtual HRESULT DGLE2_API GetFPS(uint &uiFPS) = 0;
		virtual HRESULT DGLE2_API GetLastUpdateDeltaTime(uint64 &ui64DeltaTime) = 0;
		virtual HRESULT DGLE2_API GetHandle(TWinHandle &tHandle) = 0;

		virtual HRESULT DGLE2_API ChangeWinMode(const TEngWindow &stNewWin) = 0;
		virtual HRESULT DGLE2_API AllowPause(bool bAllow) = 0;

		virtual HRESULT DGLE2_API AddToLog(const char *pcTxt) = 0;
		virtual HRESULT DGLE2_API AddToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber) = 0;

		virtual HRESULT DGLE2_API ConsoleVisible(bool bIsVisible) = 0;
		virtual HRESULT DGLE2_API ConsoleWrite(const char *pcTxt, bool bWriteToPreviousLine = false) = 0;
		virtual HRESULT DGLE2_API ConsoleExec(const char *pcCommandTxt) = 0;
		virtual HRESULT DGLE2_API ConsoleRegComProc(const char *pcCommandName, const char *pcCommandHelp, void (DGLE2_API *pProc)(void *pParametr, const char *pcParam), void *pParametr = NULL) = 0; 
		virtual HRESULT DGLE2_API ConsoleRegComValue(const char *pcCommandName, const char *pcCommandHelp, int *piValue, int iMinValue, int iMaxValue, void (DGLE2_API *pProc)(void *pParametr, const char *pcParam) = NULL, void *pParametr = NULL) = 0;
		virtual HRESULT DGLE2_API ConsoleUnregCom(const char *pcCommandName) = 0;

		virtual HRESULT DGLE2_API GetVersion(char* pcBuffer, uint uiBufferSize) = 0;	
	};

//Resource Manager SubSystem//

	class IFile;
	class ITexture;
	class IMaterial;
	class IMesh;

	enum E_TEXTURE_DATA_FORMAT
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

	enum E_TEXTURE_CREATION_FLAGS
	{
		TCF_DEFAULT					= 0x00000000,
		TCF_PIXEL_ALIGNMENT_1		= 0x00000001,//use only if your texture input data is not 4 byte aligned
		TCF_MIPMAPS_PRESENTED		= 0x00000002 //all mip levels must be presented
	};

	enum E_TEXTURE_LOAD_FLAGS
	{
		TLF_FILTERING_NONE			= 0x00000001,
		TLF_FILTERING_BILINEAR		= 0x00000002,
		TLF_FILTERING_TRILINEAR		= 0x00000004,
		TLF_FILTERING_ANISOTROPIC	= 0x00000008,
	
		TLF_DECREASE_QUALITY_MEDIUM	= 0x00000020,
		TLF_DECREASE_QUALITY_HIGH	= 0x00000040,

		TLF_COMPRESS				= 0x00000100,

		TLF_COORDS_REPEAT			= 0x00001000,
		TLF_COORDS_CLAMP			= 0x00002000,
		TLF_COORDS_MIRROR_REPEAT	= 0x00004000,
		TLF_COORDS_MIRROR_CLAMP		= 0x00008000,

		TLF_GENERATE_MIPMAPS		= 0x00040000,

		TLF_ANISOTROPY_2X			= 0x00100000,
		TLF_ANISOTROPY_4X			= 0x00200000,
		TLF_ANISOTROPY_8X			= 0x00400000,
		TLF_ANISOTROPY_16X			= 0x00800000
	};

	enum E_MESH_CREATION_FLAGS
	{
		MCF_ONLY_DEFAULT_DATA		= 0x00000000,//vertex and index arrays must be presented
		MCF_NORMALS_PRESENTED		= 0x00000001,
		MCF_TEXTURE_COORDS_PRESENTED= 0x00000002,
		MCF_TANGENT_SPACE_PRESENTED	= 0x00000004
	};

	enum E_MESH_LOAD_FLAGS
	{
		MLF_EDITABLE				= 0x00000001,
		MLF_FORCE_NO_VBO			= 0x00000002,
		MLF_CALCULATE_TANGENT_SPACE = 0x00000004,
		MLF_CALCULATE_NORMALS		= 0x00000008
	};

	const uint TEXTURE_LOAD_DEFAULT_2D = (uint)(TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP);
	const uint TEXTURE_LOAD_DEFAULT_3D = (uint)(TLF_FILTERING_TRILINEAR | TLF_GENERATE_MIPMAPS | TLF_COORDS_REPEAT);

	const uint RES_LOAD_DEFAULT = 0x00000000;

	// {139505B6-5EFC-4f02-A5E8-18CD1FBD69E3}
	static const GUID IID_IResourceManager = 
	{ 0x139505b6, 0x5efc, 0x4f02, { 0xa5, 0xe8, 0x18, 0xcd, 0x1f, 0xbd, 0x69, 0xe3 } };

	class IResourceManager : public IEngineSubSystem
	{
	public:
		virtual HRESULT DGLE2_API CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName = "", bool bAddResourse = false) = 0;
		virtual HRESULT DGLE2_API CreateMaterial(IMaterial *&prMaterial, const char *pcName = "", bool bAddResourse = false) = 0;
		virtual HRESULT DGLE2_API CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_LOAD_FLAGS eLoadFlags, const char *pcName = "", bool bAddResourse = false) = 0;
	
		virtual HRESULT DGLE2_API RegisterFileFormat(const char *pcExtension, E_ENG_OBJ_TYPE eObjType, const char *pcDiscription, bool (DGLE2_API *pLoadProc)(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags, void *pParametr), void *pParametr = NULL) = 0;
		virtual HRESULT DGLE2_API UnregisterFileFormat(const char *pcExtension) = 0;
		virtual HRESULT DGLE2_API RegisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj) = 0;
		virtual HRESULT DGLE2_API UnregisterDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *pObj) = 0;
		virtual HRESULT DGLE2_API GetRegisteredExtensions(char *pcTxt, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API GetExtensionDescription(const char *pcExtension, char *pcTxt, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API GetExtensionType(const char *pcExtension, E_ENG_OBJ_TYPE &eType) = 0;

		virtual HRESULT DGLE2_API GetResourceByFileName(const char *pcFileName, IEngBaseObj *&prObj) = 0;
		virtual HRESULT DGLE2_API GetDefaultResource(E_ENG_OBJ_TYPE eObjType, IEngBaseObj *&prObj) = 0;

		virtual HRESULT DGLE2_API Load(const char *pcFileName, IEngBaseObj *&prObj, uint uiLoadFlags = RES_LOAD_DEFAULT) = 0;
		virtual HRESULT DGLE2_API Load2(IFile *pFile, IEngBaseObj *&prObj, uint uiLoadFlags = RES_LOAD_DEFAULT) = 0;
	
		virtual HRESULT DGLE2_API FreeResource(IEngBaseObj *&prObj) = 0;
		virtual HRESULT DGLE2_API AddResource(const char *pcName, IEngBaseObj *pObj) = 0;
		virtual HRESULT DGLE2_API RemoveResource(IEngBaseObj *pObj, bool &bCanDelete) = 0;
	};

//Render SubSystem//

	class IRender2D;
	class IRender3D;
	class ILight;
	class IPostprocess;
	class ICoreGeometryBuffer;
	
	struct TDrawDataDesc;

	enum E_CORE_RENDERER_DRAW_MODE;

	enum E_GET_POINT3_FLAG
	{
		GP3F_FROM_Z_BUFFER	= 0,
		GP3F_FROM_FAR_PLANE,
		GP3F_FROM_NEAR_PLANE
	};

	// {EA03C661-A334-4225-B5DB-4C45452CCC41}
	static const GUID IID_IRender = 
	{ 0xea03c661, 0xa334, 0x4225, { 0xb5, 0xdb, 0x4c, 0x45, 0x45, 0x2c, 0xcc, 0x41 } };

	class IRender : public IEngineSubSystem
	{
	public:
		virtual HRESULT DGLE2_API SetClearColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API Unbind(E_ENG_OBJ_TYPE eType) = 0; //use EOT_UNKNOWN to unbind all
		virtual HRESULT DGLE2_API EnableScissor(const TRectF &stArea) = 0;	
		virtual HRESULT DGLE2_API DisableScissor() = 0;
		virtual HRESULT DGLE2_API SetRenderTarget(ITexture* pTargetTex = NULL) = 0;
		virtual HRESULT DGLE2_API ScreenshotBMP(const char* pFileName) = 0;
		virtual HRESULT DGLE2_API CreatePostProcess(IPostprocess *&pPP) = 0;
		
		virtual HRESULT DGLE2_API GetRender2D(IRender2D *&prRender2D) = 0;
		virtual HRESULT DGLE2_API GetRender3D(IRender3D *&prRender3D) = 0;
	};

	//Render2D interface//

	//Flags for Primitives
	enum E_PRIMITIVE2D_FLAGS
	{
		PF_DEFAULT			= 0x00000000,
		PF_LINE				= 0x00000000,
		PF_FILL				= 0x00000001, 
		PF_VERTICES_COLOR	= 0x00000002
	};

	//Flags for Effects
	enum E_EFFECT2D_FLAGS
	{
		EF_DEFAULT			= 0x00000000,
		EF_NONE				= 0x00000001,
		EF_ALPHA_TEST		= 0x00000002,
		EF_BLEND			= 0x00000004, 
		EF_FLIPX			= 0x00000008, 
		EF_FLIPY			= 0x00000010, 
		EF_COLORMIX			= 0x00000020, 
		EF_SCALE			= 0x00000040,  
		EF_VERTICES_OFFSET	= 0x00000080, 
		EF_VERTICES_COLOR	= 0x00000100, 
		EF_ROTATEPT			= 0x00000200
	};

	//Flags for Effects Blend mode
	enum E_EFFECT2D_BLENDING_FLAGS
	{
		EBF_NORMAL			= 0x00000001, 
		EBF_ADD				= 0x00000002, 
		EBF_MULT			= 0x00000004, 
		EBF_BLACK			= 0x00000008,
		EBF_WHITE			= 0x00000010,
		EBF_MASK			= 0x00000020
	};

	enum E_BATCH_MODE2D
	{
		BM_AUTO	= 0,
		BM_DISABLED,
		BM_ENABLED_UEP,//update batches every process tick
		BM_ENABLED_UER //update batches every render frame
	};

	// {F5F3257A-F8B8-4d91-BA67-451167A8D63F}
	static const GUID IID_IRender2D = 
	{ 0xf5f3257a, 0xf8b8, 0x4d91, { 0xba, 0x67, 0x45, 0x11, 0x67, 0xa8, 0xd6, 0x3f } };

	class IRender2D : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API Begin2D() = 0;
		virtual HRESULT DGLE2_API End2D() = 0;
		//Note: Only one pair Begin2D-End2D per frame is allowed or batching must be disabled.
		virtual HRESULT DGLE2_API BatchRender(E_BATCH_MODE2D eMode) = 0;
		virtual HRESULT DGLE2_API InvalidateBatchData() = 0;
		virtual HRESULT DGLE2_API BeginBatch(bool bUpdateEveryFrame = false) = 0;
		virtual HRESULT DGLE2_API EndBatch() = 0;
		virtual HRESULT DGLE2_API NeedToUpdateBatchData(bool &bNeedUpdate) = 0;
		virtual HRESULT DGLE2_API SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstaintProportions = true) = 0; //Set resx and resy to current screen size to turn off correction
		virtual HRESULT DGLE2_API SetCamera(const TPoint2 &stCenter, float fAngle = 0.f, const TPoint2 &stScale = TPoint2(1.f, 1.f)) = 0;
		virtual HRESULT DGLE2_API CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull) = 0;

		// 2D Primitives
		virtual HRESULT DGLE2_API LineWidth(uint uiWidth) = 0;
		virtual HRESULT DGLE2_API DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor = TColor4(), uint uiSize = 1) = 0;
		virtual HRESULT DGLE2_API DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor = TColor4(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawRect(const TRectF &stRect, const TColor4 &stColor = TColor4(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor = TColor4(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawEllipse(const TPoint2 &stCoords, const TPoint2 &stRadius, uint uiQuality, const TColor4 &stColor = TColor4(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawPolygon(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
	
		// 2D Sprites		
		virtual HRESULT DGLE2_API DrawSpriteS(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawSpriteA(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, uint uiFrameIndex, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawSpriteC(ITexture *pTexture, const TPoint2 &stCoords, const TPoint2 &stDimensions, const TRectF &stRect, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;

		// Extra
		virtual HRESULT DGLE2_API DrawTriangles(ITexture *pTexture, TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual HRESULT DGLE2_API DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis = TVector3(), float fAngle = 0.f, bool bClip = true, float fFovY = 90.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;

		//Advanced
		virtual HRESULT DGLE2_API Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) = 0;
		virtual HRESULT DGLE2_API DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) = 0;
		virtual HRESULT DGLE2_API DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix &stTransform, const TVector3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY) = 0;

		//Effects
		virtual HRESULT DGLE2_API SetRotationPoint(const TPoint2 &stCoords) = 0;//In texture coord system
		virtual HRESULT DGLE2_API SetScale(const TPoint2 &stScale) = 0;
		virtual HRESULT DGLE2_API SetColorMix(const TColor4 &stColor = TColor4()) = 0;
		virtual HRESULT DGLE2_API SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode = EBF_NORMAL) = 0;
		virtual HRESULT DGLE2_API SetVerticesOffset(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4) = 0;
		virtual HRESULT DGLE2_API SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4) = 0;
		
		virtual HRESULT DGLE2_API GetRotationPoint(TPoint2 &stCoords) = 0;
		virtual HRESULT DGLE2_API GetScale(TPoint2 &stScale) = 0;
		virtual HRESULT DGLE2_API GetColorMix(TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API GetBlendMode(E_EFFECT2D_BLENDING_FLAGS &eMode) = 0;
		virtual HRESULT DGLE2_API GetVerticesOffset(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4) = 0;
		virtual HRESULT DGLE2_API GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4) = 0;
	};

	//Render3D interface//

	enum E_PUSH_STATES_FLAGS
	{
		PSF_ALL		= 0x00000000,
		PSF_MATRIX	= 0x00000001,
		PSF_STATES	= 0x00000002
	};

	// {5275F43A-4FF9-48b2-B88E-B2F842461AB3}
	static const GUID IID_IRender3D = 
	{ 0x5275f43a, 0x4ff9, 0x48b2, { 0xb8, 0x8e, 0xb2, 0xf8, 0x42, 0x46, 0x1a, 0xb3 } };

	class IRender3D : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API SetPerspective(float fFovAngle, float fZNear, float fZFar) = 0;
	
		virtual HRESULT DGLE2_API SetColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetBlendMode(E_EFFECT2D_BLENDING_FLAGS eMode = EBF_NORMAL) = 0;
		/*!!!!!!!!!!*/virtual HRESULT DGLE2_API ToggleAlphaTest(bool bEnabled, float fAlphaMinTreshold = 0.25) = 0;
		virtual HRESULT DGLE2_API ToggleDepthTest(bool bEnabled) = 0;

		virtual HRESULT DGLE2_API SetMatrix(const TMatrix &stMatrix, bool bMult) = 0;
		virtual HRESULT DGLE2_API GetMatrix(TMatrix &stMatrix) = 0;

		virtual HRESULT DGLE2_API DrawAxes(float fSize = 1.f, bool bNoDepthTest = false) = 0;

		virtual HRESULT DGLE2_API PushStates(E_PUSH_STATES_FLAGS eStates = PSF_MATRIX) = 0;
		virtual HRESULT DGLE2_API PopStates() = 0;

		virtual HRESULT DGLE2_API GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_FLAG eFlag = GP3F_FROM_Z_BUFFER) = 0;
		virtual HRESULT DGLE2_API GetPoint2(const TPoint3 &stPoint, TPoint2 &stResultPointOnScreen) = 0;

		virtual HRESULT DGLE2_API FrustumSetup() = 0;
		virtual HRESULT DGLE2_API CullPoint(const TPoint3 &stCoords, bool &bCull) = 0;
		virtual HRESULT DGLE2_API CullSphere(const TPoint3 &stCoords, float fRadius, bool &bCull) = 0;
		virtual HRESULT DGLE2_API CullBox(const TPoint3 &stCenterCoords, const TVector3 &stExtents, bool &bCull) = 0;

		virtual HRESULT DGLE2_API CreateLight(ILight *&prLight) throw() = 0;
		virtual HRESULT DGLE2_API EnableLighting(bool enable) throw() = 0;
		virtual HRESULT DGLE2_API SetAmbient(const TColor4 &color) throw() = 0;
		virtual HRESULT DGLE2_API GetAmbient(TColor4 &color) const throw() = 0;
	};

	//Light interface//

	enum E_LIGHT_TYPE
	{
		LT_DIRECTIONAL = 0,
		LT_POINT
	};

	// {EB73AC84-A465-4554-994D-8BED29744C9D}
	static const GUID IID_ILight = 
	{ 0xeb73ac84, 0xa465, 0x4554, { 0x99, 0x4d, 0x8b, 0xed, 0x29, 0x74, 0x4c, 0x9d } };

	class ILight: public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API SetEnabled(bool bEnabled) = 0;
		virtual HRESULT DGLE2_API SetDiffuseColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetSpecularColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetPos(const TPoint3 &stPos) = 0;
		virtual HRESULT DGLE2_API SetDir(const TVector3 &stDir) = 0;
		virtual HRESULT DGLE2_API SetRange(float range) = 0;
		virtual HRESULT DGLE2_API SetConstantAttenuation(float attenuation) = 0;
		virtual HRESULT DGLE2_API SetLinearAttenuation(float attenuation) = 0;
		virtual HRESULT DGLE2_API SetQuadraticAttenuation(float attenuation) = 0;
		virtual HRESULT DGLE2_API SetType(E_LIGHT_TYPE eType) = 0;
		
		virtual HRESULT DGLE2_API GetEnabled(bool &bEnabled) = 0;
		virtual HRESULT DGLE2_API GetDiffuseColor(TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API GetSpecularColor(TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API GetPos(TPoint3 &stPos) = 0;
		virtual HRESULT DGLE2_API GetDir(TVector3 &stDir) = 0;
		virtual HRESULT DGLE2_API GetRange(float &range) = 0;
		virtual HRESULT DGLE2_API GetConstantAttenuation(float &attenuation) = 0;
		virtual HRESULT DGLE2_API GetLinearAttenuation(float &attenuation) = 0;
		virtual HRESULT DGLE2_API GetQuadraticAttenuation(float &attenuation) = 0;
		
		virtual HRESULT DGLE2_API GetType(E_LIGHT_TYPE &eType) = 0;
		virtual HRESULT DGLE2_API Free() = 0;
	};

	//Texture interface//

	class ICoreTexture;

	// {85BDDBC2-F126-4cae-946D-7D6B079E5CCE}
	static const GUID IID_ITexture = 
	{ 0x85bddbc2, 0xf126, 0x4cae, { 0x94, 0x6d, 0x7d, 0x6b, 0x7, 0x9e, 0x5c, 0xce } };

	class ITexture : public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API GetDimensions(uint &uiWidth, uint &uiHeight) = 0;
		virtual HRESULT DGLE2_API SetFrameSize(uint uiFrameWidth, uint uiFrameHeight) = 0;
		virtual HRESULT DGLE2_API GetFrameSize(uint &uiFrameWidth, uint &uiFrameHeight) = 0;

		virtual HRESULT DGLE2_API GetCoreTexture(ICoreTexture *&prCoreTex) = 0;

		virtual HRESULT DGLE2_API Draw2DSimple(int iX, int iY, uint uiFrameIndex = 0) = 0;
		virtual HRESULT DGLE2_API Draw2D(int iX, int iY, uint uiWidth, uint uiHeight, float fAngle = 0.f, uint uiFrameIndex = 0) = 0;
		virtual HRESULT DGLE2_API Draw3D(uint uiFrameIndex = 0) = 0;

		virtual HRESULT DGLE2_API Bind(uint uiMTextureLayer = 0) = 0;
	};

	//Material interface//

	enum E_TEX_MAPPING
	{
		TM_STD = 0,
		TM_OBJ_SPACE = 1
	};

	enum E_NORMAL_TECHNIQUE
	{
		NT_UNPERTURBED = 0,
		NT_NORMAL_MAP_3_COMPONENT = 1,
		NT_HEIGHT_MAP_HW_DIFF = 3
	};

	enum E_PARALLAX_TECHNIQUE
	{
		PT_NONE = 0,
		PT_SPHERE = 1,
		PT_PLANE = 2
	};

	// {B6506749-BB41-423d-B6C0-982081EF63F9}
	static const GUID IID_IMaterial = 
	{ 0xb6506749, 0xbb41, 0x423d, { 0xb6, 0xc0, 0x98, 0x20, 0x81, 0xef, 0x63, 0xf9 } };

	class IMaterial: public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API SetAmbientColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetDiffuseColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetSpecularColor(const TColor4 &stColor) = 0;
		virtual HRESULT DGLE2_API SetHeightScale(float scale) = 0;
		virtual HRESULT DGLE2_API SetEnvAmount(float amount) = 0;
		virtual HRESULT DGLE2_API SetShininess(float shininess) = 0;
		virtual HRESULT DGLE2_API SetTexMappingMode(E_TEX_MAPPING eTexMapping) = 0;
		virtual HRESULT DGLE2_API SetDiffuseTexture(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetSpecularTexture(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetNormalTexture(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetHeightTexture(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetEnvTexture(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetEnvMask(ITexture *pTexture) = 0;
		virtual HRESULT DGLE2_API SetNormalTechnique(E_NORMAL_TECHNIQUE technique) = 0;
		virtual HRESULT DGLE2_API SetParallaxTechnique(E_PARALLAX_TECHNIQUE technique) = 0;
		
		virtual HRESULT DGLE2_API GetAmbientColor(TColor4 &stColor) const = 0;
		virtual HRESULT DGLE2_API GetDiffuseColor(TColor4 &stColor) const = 0;
		virtual HRESULT DGLE2_API GetSpecularColor(TColor4 &stColor) const = 0;
		virtual HRESULT DGLE2_API GetHeightScale(float &scale) const = 0;
		virtual HRESULT DGLE2_API GetEnvAmount(float &amount) const = 0;
		virtual HRESULT DGLE2_API GetShininess(float &shininess) const = 0;
		virtual HRESULT DGLE2_API GetTexMappingMode(E_TEX_MAPPING &eTexMapping) const = 0;
		virtual HRESULT DGLE2_API GetDiffuseTexture(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetSpecularTexture(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetNormalTexture(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetHeightTexture(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetEnvTexture(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetEnvMask(ITexture *&pTexture) const = 0;
		virtual HRESULT DGLE2_API GetNormalTechnique(E_NORMAL_TECHNIQUE &technique) const = 0;
		virtual HRESULT DGLE2_API GetParallaxTechnique(E_PARALLAX_TECHNIQUE &technique) const = 0;
		
		virtual HRESULT DGLE2_API Bind() const = 0;
	};

	//BitmapFont interface//

	// {0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA}
	static const GUID IID_IBitmapFont = 
	{ 0xb03e8d7, 0x23a3, 0x4c79, { 0x9e, 0x82, 0x5b, 0xc6, 0xe5, 0xe, 0x1e, 0xba } };

	class IBitmapFont : public IEngBaseObj
	{
	public: 	
		virtual HRESULT DGLE2_API GetTexture(ITexture *&prTexture) = 0;
		virtual HRESULT DGLE2_API SetScale(const float &fScale) = 0;
		virtual HRESULT DGLE2_API GetScale(float &fScale) = 0;
		virtual HRESULT DGLE2_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight) = 0;
		virtual HRESULT DGLE2_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor = TColor4()) = 0;
		virtual HRESULT DGLE2_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor = TColor4(), float fAngle = 0, bool bVerticesColors = false) = 0;
		virtual HRESULT DGLE2_API Draw3D(const char *pcTxt) = 0;
	};

	//3D Objects interfaces//
	
	//Mesh interface//

	// {85E360A8-07B3-4f22-AA29-07C7FC7C6893}
	static const GUID IID_IMesh = 
	{ 0x85e360a8, 0x7b3, 0x4f22, { 0xaa, 0x29, 0x7, 0xc7, 0xfc, 0x7c, 0x68, 0x93 } };

	class IMesh : public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API Draw() = 0;
		virtual HRESULT DGLE2_API GetCenter(TPoint3 &stCenter) = 0;
		virtual HRESULT DGLE2_API GetExtents(TVector3 &stExtents) = 0;
		virtual HRESULT DGLE2_API GetTrianglesCount(uint &uiTriCnt) = 0;
		virtual HRESULT DGLE2_API GetGeometryInfo(uint &uiVertsCount, uint &uiFacesCount, uint &uiDataSize, E_MESH_CREATION_FLAGS &eCreationFlags) = 0;
		/*Format x,y,z*vertex_count, nx,ny,nz*vertex_count s,q*vertex_count tx,ty,tz,bx,by,bz*vertex_count int16*uiFacesCount*/
		virtual HRESULT DGLE2_API GetGeometry(uint8 *pubtData) = 0;
		virtual HRESULT DGLE2_API SetGeometry(uint8 *pubtData) = 0;
	};

	//Skeleton xform interface//

	// {1D12C62F-9974-4219-8F12-DA92A4CC259F}
	static const GUID IID_ISkeletonXform = 
	{ 0x1d12c62f, 0x9974, 0x4219, { 0x8f, 0x12, 0xda, 0x92, 0xa4, 0xcc, 0x25, 0x9f } };

	class ISkeletonXform: public IEngBaseObj
	{
	public:
	};

	//Model interface//

	// {6107C296-FC07-48d1-B6A7-F88CC2DAE897}
	static const GUID IID_IModel = 
	{ 0x6107c296, 0xfc07, 0x48d1, { 0xb6, 0xa7, 0xf8, 0x8c, 0xc2, 0xda, 0xe8, 0x97 } };

	class IModel : public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API MeshsCount(uint &uiCount) = 0;
		virtual HRESULT DGLE2_API GetMesh(uint uiIdx, IMesh *&prMesh) = 0;
		virtual HRESULT DGLE2_API Draw(float fFrameIdx) = 0;
	};

	//Postprocess interface//

	// {DC6E4812-6B06-4de8-9DEF-2A13740BC45E}
	static const GUID IID_IPostprocess = 
	{ 0xdc6e4812, 0x6b06, 0x4de8, { 0x9d, 0xef, 0x2a, 0x13, 0x74, 0xb, 0xc4, 0x5e } };

	class IPostprocess: public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API Free() = 0;

		virtual HRESULT DGLE2_API SetTargets(ITexture *pSrc, ITexture *pDst) = 0;
		virtual HRESULT DGLE2_API SetBlurAmount(float fBlur) = 0;
		virtual HRESULT DGLE2_API SetBloomAmount(float fBloom) = 0;
		virtual HRESULT DGLE2_API SetBloomThreshold(float fThreshold) = 0;
		virtual HRESULT DGLE2_API ToggleMonochrome(bool bEnable) = 0;
		virtual HRESULT DGLE2_API Perform() = 0;
	};

//Input Subsystem//
	
	enum E_INPUT_CONFIGURATION_FLAGS
	{
		ICF_DEFAULT					= 0x00000000,
		ICF_EXCLUSIVE				= 0x00000001,
		ICF_HIDE_CURSOR				= 0x00000002,
		ICF_CURSOR_BEYOND_SCREEN	= 0x00000004
	};

	// {64DAAF7F-F92C-425f-8B92-3BE40D8C6666}
	static const GUID IID_IInput = 
	{ 0x64daaf7f, 0xf92c, 0x425f, { 0x8b, 0x92, 0x3b, 0xe4, 0xd, 0x8c, 0x66, 0x66 } };

	class IInput : public IEngineSubSystem
	{
	public:
		virtual HRESULT DGLE2_API Configure(E_INPUT_CONFIGURATION_FLAGS eFlags = ICF_DEFAULT) = 0;
	
		virtual HRESULT DGLE2_API GetMouseStates(TMouseStates &stMStates) = 0;
		virtual HRESULT DGLE2_API GetKey(E_KEYBOARD_KEY_CODES eKeyCode, bool &bPressed) = 0;
		virtual HRESULT DGLE2_API GetKeyName(E_KEYBOARD_KEY_CODES eKeyCode, uchar &cASCIICode) = 0;

		virtual HRESULT DGLE2_API BeginTextInput(char* pcBuffer, uint uiBufferSize) = 0;
		virtual HRESULT DGLE2_API EndTextInput() = 0;
		
		virtual HRESULT DGLE2_API GetJoysticksCount(uint &uiCount) = 0;
		virtual HRESULT DGLE2_API GetJoystickName(uint uiJoyId, char *pcName, uint &uiCharsCount) = 0;
		virtual HRESULT DGLE2_API GetJoystickStates(uint uiJoyId, TJoystickStates &stJoyStates) = 0;
	};

//Sound SubSystem interfaces//

	// {054C07EE-2724-42f2-AC2B-E81FCF5B4ADA}
	static const GUID IID_ISound = 
	{ 0x54c07ee, 0x2724, 0x42f2, { 0xac, 0x2b, 0xe8, 0x1f, 0xcf, 0x5b, 0x4a, 0xda } };

	class ISound : public IEngineSubSystem
	{
	public:
		virtual HRESULT DGLE2_API SetMasterVolume(uint uiVolume) = 0;
		virtual HRESULT DGLE2_API PauseAllChannels(bool bPaused) = 0;
		virtual HRESULT DGLE2_API StopAllChannels() = 0;
		virtual HRESULT DGLE2_API SetMaxChannelsCount(uint uiCount) = 0;
		virtual HRESULT DGLE2_API SetListnerPosition(const TPoint3 &stCoords) = 0;
		virtual HRESULT DGLE2_API GetListnerPosition(TPoint3 &stCoords) = 0;
		virtual HRESULT DGLE2_API SetListnerOrientation(const TVector3 &stDir, const TVector3 &stUp) = 0;
		virtual HRESULT DGLE2_API GetListnerOrientation(TVector3 &stDir, TVector3 &stUp) = 0;
	};

	//SoundSample interface//

	enum E_SOUND_CHANNEL_EFFECTS
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

	// {DE6F7CDD-8262-445c-8D20-68E3324D99A6}
	static const GUID IID_ISoundChannel = 
	{ 0xde6f7cdd, 0x8262, 0x445c, { 0x8d, 0x20, 0x68, 0xe3, 0x32, 0x4d, 0x99, 0xa6 } };

	class ISoundChannel : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API PlayOrPause() = 0;
		virtual HRESULT DGLE2_API Stop() = 0;
		virtual HRESULT DGLE2_API IsPlaying(bool &bIsPlaying) = 0;
		virtual HRESULT DGLE2_API SetVolume(uint uiVolume) = 0; //from 0 to 100
		virtual HRESULT DGLE2_API GetVolume(uint &uiVolume) = 0;
		virtual HRESULT DGLE2_API SetPan(int iPan) = 0; //from -100 to 100
		virtual HRESULT DGLE2_API GetPan(int &iPan) = 0;
		virtual HRESULT DGLE2_API SetFrequency(uint32 uiFreq) = 0;//from 10 to 10000
		virtual HRESULT DGLE2_API GetFrequency(uint32 &uiFreq) = 0;
		virtual HRESULT DGLE2_API SetPosition(const TPoint3 &stCoords) = 0;
		virtual HRESULT DGLE2_API GetPosition(TPoint3 &stCoords) = 0;
		virtual HRESULT DGLE2_API SetEffects(E_SOUND_CHANNEL_EFFECTS eFlags) = 0;
		virtual HRESULT DGLE2_API Unaquire() = 0;
	};
	
	enum E_SOUND_SAMPLE_PARAMS
	{
		SSP_NONE			= 0x00000000,
		SSP_LOOP			= 0x00000001,
		SSP_CONTROL_3D		= 0x00000002,
		SSP_ALLOW_EFFECTS	= 0x00000004

	};

	// {30DD8C94-D3FA-40cf-9C49-649211424919}
	static const GUID IID_ISoundSample = 
	{ 0x30dd8c94, 0xd3fa, 0x40cf, { 0x9c, 0x49, 0x64, 0x92, 0x11, 0x42, 0x49, 0x19 } };

	class ISoundSample : public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API Play() = 0;
		virtual HRESULT DGLE2_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags = SSP_NONE) = 0; //pSndChnl must be checked on nul
	};

	//Music interface//

	// {81F1E67B-3FEB-4ab1-9AD2-D27C4E662164}
	static const GUID IID_IMusic = 
	{ 0x81f1e67b, 0x3feb, 0x4ab1, { 0x9a, 0xd2, 0xd2, 0x7c, 0x4e, 0x66, 0x21, 0x64 } };

	class IMusic : public IEngBaseObj
	{
	public:
		virtual HRESULT DGLE2_API Play(bool bLooped = true) = 0;
		virtual HRESULT DGLE2_API Pause(bool bPaused) = 0;
		virtual HRESULT DGLE2_API Stop() = 0;
		virtual HRESULT DGLE2_API IsPlaying(bool &bIsPlaying) = 0;
		virtual HRESULT DGLE2_API SetVolume(uint uiVolume) = 0;
		virtual HRESULT DGLE2_API GetVolume(uint &uiVolume) = 0;
		virtual HRESULT DGLE2_API SetCurrentPosition(uint uiPos) = 0;
		virtual HRESULT DGLE2_API GetCurrentPosition(uint &uiPos) = 0;
		virtual HRESULT DGLE2_API GetLength(uint &uiLength) = 0;
	};

//FileSystem interfaces//

	class IFileSystem;

	// {4850286F-4770-4bcf-A90A-33D7BE41E686}
	static const GUID IID_IMainFileSystem = 
	{ 0x4850286f, 0x4770, 0x4bcf, { 0xa9, 0xa, 0x33, 0xd7, 0xbe, 0x41, 0xe6, 0x86 } };

	class IMainFileSystem : public IEngineSubSystem
	{
	public:
		virtual HRESULT DGLE2_API LoadFile(const char* pcFileName, IFile *&prFile) = 0;// c:\data.zip|img.jpg
		virtual HRESULT DGLE2_API GetVirtualFileSystem(const char *pcVFSExtension/*NULL to get HDD file system*/, IFileSystem *&prVFS) = 0;
		virtual HRESULT DGLE2_API RegisterVirtualFileSystem(const char* pcVFSExtension, const char *pcDiscription, IFileSystem *pVFS, void (DGLE2_API *pDeleteDGLE2_API)(void *pParametr, IFileSystem *pVFS), void *pParametr = NULL) = 0;
		virtual HRESULT DGLE2_API UnregisterVirtualFileSystem(const char* pcVFSExtension) = 0;
		virtual HRESULT DGLE2_API GetRegisteredVirtualFileSystems(char* pcTxt, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API GetVirtualFileSystemDescription(const char* pcVFSExtension, char* pcTxt, uint uiCharsCount) = 0;
	};

	enum E_FIND_FLAGS
	{
		FF_RECURSIVE = 1
	};

	enum E_FILE_SYSTEM_OPEN_FLAGS
	{
		FSOF_READ	= 0x00000001,	
		FSOF_WRITE	= 0x00000002,	
		FSOF_TRUNC	= 0x00000004,	
		FSOF_BINARY	= 0x00000008	
	};

	enum E_FILE_SYSTEM_SEEK_FLAG
	{
		FSSF_BEGIN = 0,
		FSSF_CURRENT,
		FSSF_END
	};

	//File interface//

	// {AE6E8AE7-3E5B-4bc4-A512-42E1CF1DF005}
	static const GUID IID_IFile = 
	{ 0xae6e8ae7, 0x3e5b, 0x4bc4, { 0xa5, 0x12, 0x42, 0xe1, 0xcf, 0x1d, 0xf0, 0x5 } };

	class IFile : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API Read(void *pBuffer, uint uiCount, uint &uiRead) = 0;
		virtual HRESULT DGLE2_API Write(const void *pBuffer, uint uiCount, uint &uiWritten) = 0;
		virtual HRESULT DGLE2_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position) = 0;
		virtual HRESULT DGLE2_API GetSize(uint32 &ui32Size) = 0;
		virtual HRESULT DGLE2_API IsOpen(bool &bOpened) = 0;
		virtual HRESULT DGLE2_API GetName(char *pcName, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API GetPath(char *pcPath, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API Free() = 0;
	};

	//FileIterator interface//

	// {5D73F249-0E74-4cc5-9646-270CB1E22750}
	static const GUID IID_IFileIterator = 
	{ 0x5d73f249, 0xe74, 0x4cc5, { 0x96, 0x46, 0x27, 0xc, 0xb1, 0xe2, 0x27, 0x50 } };

	class IFileIterator : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API FileName(char *pcName, uint uiCharsCount) = 0;
		virtual HRESULT DGLE2_API Next() = 0;
		virtual HRESULT DGLE2_API Free() = 0;
	};

	//FileSystem interface//

	// {2DAE578E-9636-4fae-BABB-7D835EEA7518}
	static const GUID IID_IFileSystem = 
	{ 0x2dae578e, 0x9636, 0x4fae, { 0xba, 0xbb, 0x7d, 0x83, 0x5e, 0xea, 0x75, 0x18 } };

	class IFileSystem : public IDGLE2_Base
	{
	public:
		virtual HRESULT DGLE2_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile) = 0;	//если передан только путь "C:\MyFolder\" то создаст папку
		virtual HRESULT DGLE2_API DeleteFile(const char *pcName) = 0; //≈сли передан только путь то удалит папку
		virtual HRESULT DGLE2_API FileExists(const char *pcName, bool &bExists) = 0;//если передан только путь, то провер€ет существование папки
		virtual HRESULT DGLE2_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator) = 0;
		virtual HRESULT DGLE2_API SendCommand(const char *pcCommand, char *pcResult, uint uiCharsCount) = 0;
	};

}// end of namespace

	enum E_GET_ENGINE_FLAGS
	{
		GEF_DEFAULT				= 0x00000000,	
		GEF_FORCE_SINGLE_THREAD	= 0x00000001,	
		GEF_FORCE_NO_LOG_FILE	= 0x00000002,
		GEF_FORCE_QUIT			= 0x00000004
	};

//Include one of these defines once to main CPP file//

/** \def ENG_DYNAMIC_FUNC Macros you must insert in your main cpp file to use CreateEngine and FreeEngine functions.
	\note This macros is used when engine is linked dynamically from DLL file.
*/
#define ENG_DYNAMIC_FUNC \
extern"C" bool (DGLE2_API *pCreateEngine)(DGLE2::IEngineCore *&pEngineCore, E_GET_ENGINE_FLAGS eFlags, DGLE2::uint8 ubtSDKVer) = NULL;\
extern"C" bool (DGLE2_API *pFreeEngine)(DGLE2::IEngineCore *pEngineCore) = NULL;\
HMODULE hServer = NULL;\
bool CreateEngine(DGLE2::IEngineCore *&pEngineCore, E_GET_ENGINE_FLAGS eFlags = GEF_DEFAULT)\
{\
	if(pCreateEngine == NULL)\
		return false;\
	return (*pCreateEngine)(pEngineCore, eFlags, _DGLE2_SDK_VER_);\
}\
bool FreeEngine(DGLE2::IEngineCore *pEngineCore = NULL, bool bFreeDLL = true)\
{\
	if(pEngineCore)\
		(*pFreeEngine)(pEngineCore);\
	if(bFreeDLL && hServer)\
	{\
		FreeLibrary(hServer);\
		hServer = NULL;\
		pCreateEngine = NULL;\
		pFreeEngine = NULL;\
	}\
	return true;\
}\
bool GetEngine(const char *pcDllFileName, DGLE2::IEngineCore *&pEngineCore, E_GET_ENGINE_FLAGS eFlags = GEF_DEFAULT)\
{\
	if(hServer == NULL)\
	{\
		pEngineCore = NULL;\
		if(hServer == NULL)\
		{\
			hServer = ::LoadLibraryA(pcDllFileName);\
			if(hServer == NULL) return false;\
		}\
		if(pCreateEngine == NULL && pFreeEngine == NULL)\
		{\
			pCreateEngine = reinterpret_cast<bool (DGLE2_API *)(DGLE2::IEngineCore *&, E_GET_ENGINE_FLAGS, DGLE2::uint8)>\
				(::GetProcAddress(hServer,("CreateEngine")));\
			pFreeEngine = reinterpret_cast<bool (DGLE2_API *)(DGLE2::IEngineCore *)>\
				(::GetProcAddress(hServer,("FreeEngine")));\
			if(pCreateEngine == NULL || pFreeEngine == NULL)\
			{\
				FreeLibrary(hServer);\
				hServer = NULL;\
				return false;\
			}\
		}\
	}\
	if(hServer) return CreateEngine(pEngineCore, eFlags);\
	return false;\
}

/** Macros checks HRESULT value and throws exception of HRESULT type if value is failed.
	\param[in] hr HRESULT value to be checked.
	\see FAILED
*/
#define CHECK_HR(hr)\
{HRESULT hr_ = hr; if(FAILED(hr_)) throw hr_;}

/** \def PARANOIC_CHECK_HR(hr) Macros checks HRESULT value and drop assertion on any value except S_OK.
	Useful for debugging. After skipping assertion always returns S_OK. In release macros do nothing. 
	\param[in] hr HRESULT value to be checked.
	\code
	PARANOIC_CHECK_HR(pEngineCore->StartEngine());
	\endcode
*/
#ifdef _DEBUG
#	define PARANOIC_CHECK_HR(hr) assert(hr == S_OK), S_OK
#else
#	define PARANOIC_CHECK_HR(hr) hr
#endif

//Implementation macroses//

/** \def IDGLE2_BASE_IMPLEMENTATION(interface_name) Macros inserts realisation of IDGLE2_Base interface into class body.
	Can be used with interfaces inherited from IDGLE2_Base.
	\param[in] interface_name Name of the interface inheritance from IDGLE2_Base.
	\note It also inserts IUnknown implementation for Windows builds.
	\see IUNKNOWN_IMPLEMENTATION
*/

/** \def IDGLE2_BASE_IMPLEMENTATION1(interface1_name, interface2_name) Same is IDGLE2_BASE_IMPLEMENTATION but for inheritance chain with two interfaces.
	Can be used with interfaces inherited from IDGLE2_Base and some other interface.
	\param[in] interface1_name Name of the last interface in the inheritance chain.
	\param[in] interface2_name Name of the second interface in the inheritance chain.
	\see IDGLE2_BASE_IMPLEMENTATION
*/

/** \def IUNKNOWN_IMPLEMENTATION(interface_name) Macros inserts realisation of IUnknown interface into class body.
	\param[in] interface_name Name of the interface inheritance from IUnknown.
	\note For non Windows builds inserts nothing.
*/

#ifndef DGLE2_USE_COM

#define IDGLE2_BASE_IMPLEMENTATION(interface_name) HRESULT DGLE2_API GetGUID(GUID &guid) {guid = IID_##interface_name;return S_OK;}
#define IDGLE2_BASE_IMPLEMENTATION1(interface1_name, interface2_name) IDGLE2_BASE_IMPLEMENTATION(interface1_name)
#define IUNKNOWN_IMPLEMENTATION(interface_name)

#else//DGLE2_USE_COM

#define IDGLE2_BASE_IMPLEMENTATION(interface_name) \
	HRESULT DGLE2_API GetGUID(GUID &guid)\
	{\
		guid = IID_##interface_name;\
		return S_OK;\
	}\
	HRESULT DGLE2_API QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)\
	{\
		*ppvObject = NULL;\
		if(::memcmp(&riid,&__uuidof(IUnknown),sizeof(GUID)) == 0) \
		*ppvObject = static_cast<IUnknown *>(this);\
		else if(::memcmp(&riid,&IID_##interface_name,sizeof(GUID)) == 0) \
		*ppvObject = static_cast<interface_name *>(this);\
		else return E_NOINTERFACE;\
		return S_OK;\
	}\
	ULONG DGLE2_API AddRef(){return 1;}\
	ULONG DGLE2_API Release(){return 1;}

#define IDGLE2_BASE_IMPLEMENTATION1(interface1_name, interface2_name) \
	HRESULT DGLE2_API GetGUID(GUID &guid)\
	{\
		guid = IID_##interface1_name;\
		return S_OK;\
	}\
	HRESULT DGLE2_API QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)\
	{\
		*ppvObject = NULL;\
		if(::memcmp(&riid,&__uuidof(IUnknown),sizeof(GUID)) == 0) \
		*ppvObject = static_cast<IUnknown *>(this);\
		else if(::memcmp(&riid,&IID_##interface1_name,sizeof(GUID)) == 0) \
		*ppvObject = static_cast<interface1_name *>(this);\
		else if(::memcmp(&riid,&IID_##interface2_name,sizeof(GUID)) == 0) \
		*ppvObject = static_cast<interface2_name *>(this);\
		else return E_NOINTERFACE;\
		return S_OK;\
	}\
	ULONG DGLE2_API AddRef(){return 1;}\
	ULONG DGLE2_API Release(){return 1;}

#define IUNKNOWN_IMPLEMENTATION(interface_name) \
	HRESULT DGLE2_API QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)\
	{\
		*ppvObject = NULL;\
		if(::memcmp(&riid,&__uuidof(IUnknown),sizeof(GUID)) == 0)\
		*ppvObject = static_cast<IUnknown *>(this);\
		else if(::memcmp(&riid,&__uuidof(interface_name),sizeof(GUID)) == 0)\
		{*ppvObject = static_cast<interface_name *>(this); AddRef();}\
		else return E_NOINTERFACE;\
		return S_OK;\
	}\
	ULONG DGLE2_API AddRef() { return 1; }\
	ULONG DGLE2_API Release() { return 1; }

#endif

#endif//DGLE2_HEADER