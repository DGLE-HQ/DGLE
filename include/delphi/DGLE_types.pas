{
\file		DGLE2_types.pas
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		XX.XX.2012 (c)Korotkov Andrey

\brief		Engine types definition header.

This header is a part of DGLE_SDK.

\warning	Don't include this header directly, include "DGLE.pas" instead.
\see		DGLE.pas
}

unit DGLE_types;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
{$ENDIF}

{$ifndef DGLE_TYPES}
{$define DGLE_TYPES}
{$ENDIF}

uses

Windows

{$IF CompilerVersion >= 20}
, Generics.Collections
{$IFEND}
;

const

  Minus1 = $FFFFFFFF;

	//E_ENGINE_WINDOW_FLAGS
		EWF_DEFAULT				= $00000000;//< This flag is suitable in most cases.
		EWF_ALLOW_SIZEING		= $00000001;//< User can resize engine window arbitrarily
		EWF_TOPMOST				= $00000002;//< Engine window will be always on top.
		EWF_DONT_HOOK_MLOOP		= $00000004;//< If flag set and engine doesn't owns window, host applications main loop will not be hooked. User must call window repaint manually.
		EWF_DONT_HOOK_ROOT_WIN	= $00000008;//< If flag set and engine doesn't owns window, main host application window will not be hooked. User must redirect windows messages manually.
		EWF_RESTRICT_FSCREEN_HOTKEY	= $00000010;//< Switching between fullscreen and windowed modes by pressing "Alt-Enter" will be restricted.
		EWF_RESTRICT_CONSOLE_HOTKEY	= $00000020;//< Restricts calling engine console window by pressing "~" key.

	// E_WINDOW_MESSAGE_TYPE

		WMT_UNKNOWN	= 0;
		WMT_REDRAW	= 1;
		WMT_PRESENT	= 2;
		WMT_CLOSE	= 3;
		WMT_CREATE	= 4;
		WMT_DESTROY	= 5;
		WMT_RELEASED	= 6;
		WMT_ACTIVATED	= 7;
		WMT_DEACTIVATED	= 8;
		WMT_MINIMIZED	= 9;
		WMT_RESTORED	= 10;
		WMT_MOVE	= 11;
		WMT_SIZE	= 12;
		WMT_KEY_UP	= 13;
		WMT_KEY_DOWN	= 14;
		WMT_ENTER_CHAR	= 15;
		WMT_MOUSE_MOVE	= 16;
		WMT_MOUSE_DOWN	= 17;
		WMT_MOUSE_UP	= 18;
		WMT_MOUSE_WHEEL	= 19;

	// E_MULTISAMPLING_MODE
	
		MM_NONE	= $00000000;//**< Multisampling is off. */
		MM_2X	= $00000001;//**< 2xMSAA */
		MM_4X	= $00000002;//**< 4xMSAA \note This value is recomended. */
		MM_8X	= $00000004;//**< 8xMSAA */
		MM_16X	= $00000008;//**< 16xMSAA */


  //variant//

	// E_DGLE_VARIANT_TYPE

		DVT_UNKNOWN = 0;
		DVT_INT     = 1;
		DVT_FLOAT   = 2;
		DVT_BOOL    = 3;
		DVT_POINTER = 4;
		DVT_DATA    = 5;

type

	TVariant = record
		_type: {E_DGLE_VARIANT_TYPE} Integer ;
		_i: Integer;
		_f: single;
		_p: Pointer;

    {$IF CompilerVersion < 18}
    end;
    {$IFEND}

		procedure Clear({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}); {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure SetInt({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}iVal: Integer); {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure SetFloat({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}fVal: single); {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure SetBool({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}bVal: Boolean); {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure SetPointer({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pPointer: Pointer); {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure SetData({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pData: Pointer; uiDataSize: Cardinal); {$IF CompilerVersion >= 18}inline;{$IFEND}
		function AsInt({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Integer; {$IF CompilerVersion >= 18}inline;{$IFEND}
		function AsFloat({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Single; {$IF CompilerVersion >= 18}inline;{$IFEND}
		function  AsBool({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Boolean; {$IF CompilerVersion >= 18}inline;{$IFEND}
		function AsPointer({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Pointer; {$IF CompilerVersion >= 18}inline;{$IFEND}
		procedure GetData({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pData: Pointer; out uiDataSize: Cardinal); {$IF CompilerVersion >= 18}inline;{$IFEND}
		function GetType({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): {E_DGLE_VARIANT_TYPE} Integer; {$IF CompilerVersion >= 18}inline;{$IFEND}
    (*
		function operator int() { Result := _i; }

		function operator float() { Result := _f; }

		function operator bool() { Result := _i == 1; }

		function operator void * () { Result := _p; }
    *)
  {$IF CompilerVersion >= 18}
	end;
  {$ELSE}
  type
  {$IFEND}


  DGLE_RESULT = Integer;

	TWinHandle		= HWND;
	TWinDrawHandle 		= HDC ;
	TCRendererInitResult 	= Boolean ;

  Gluint = Cardinal;

	PByte	 = Pointer;

	TEngWindow = packed record
		uiWidth			: Cardinal;
		uiHeight		: Cardinal;
		bFullScreen		: Boolean;
		bVSync			: Boolean;
		eMSampling		: {E_MULTISAMPLING_MODE} Cardinal;
		uiFlags			:{ENG_WINDOW_FLAGS}Cardinal;
	end;

	TSystemInfo = packed record
		cOSName				: array[0..127] of AnsiChar;
		cCPUName			: array[0..127] of AnsiChar;
		uiCPUCount			: Cardinal;
		uiCPUFreq			: Cardinal;
		uiRAMTotal			: Cardinal;
		uiRAMAvailable		: Cardinal;
		cVideocardName		: array[0..127] of AnsiChar;
		uiVideocardCount	: Cardinal;
		uiVideocardRAM		: Cardinal;
	end;	
	
	TPluginInfo = packed record
		btPluginSDKVersion	: Byte;
		cName				: array[0..127] of AnsiChar;
		cVersion			: array[0..64] of AnsiChar;
		cVendor				: array[0..127] of AnsiChar;
		cDiscription		: array[0..256] of AnsiChar;  
  {$IF CompilerVersion >= 18}
    constructor Create(var dummy);
  {$IFEND}
    end;


	TWinMessage = record
		uiMsgType: {E_WINDOW_MESSAGE_TYPE} Cardinal; //**< Window message type identifier. */
		ui32Param1: Cardinal;	//**< Message first parametr. */
		ui32Param2: Cardinal;	//**< Message second parametr. */
		pParam3: Pointer;	//**< Message third parametr. Points to specific message data. */
  {$IF CompilerVersion >= 18}
    constructor Create(var dummy); overload;
    constructor Create(msg: {E_WINDOW_MESSAGE_TYPE} Integer; param1: Cardinal = 0;
  param2: Cardinal = 0; param3: Pointer = nil); overload;
  {$IFEND}
	end;

	TColor4 = packed record
		r, g, b, a : Single;
	end;
	PColor4 = ^TColor4;

	TPoint3 = packed record
		case byte of
			0: (_1D : array[0..2] of Single);
			1: (x, y, z : Single);
	end;
	PPoint3 = ^TPoint3;

  TVec3 = TPoint3;

	TPoint2 = packed record
		case byte of
			0: (_1D : array[0..1] of Single);
			1: (x, y : Single);
	end;
	PPoint2 = ^TPoint2;

  TVec2 = TPoint2;

	TVertex2 = packed record
		case byte of
			0: (_1D : array[0..7] of Single);
			1: (x, y, u, w, r, g, b, a : Single);
	end;
	PVertex2 = ^TVertex2;

	TVertex3 = packed record
		case byte of
			0: (_1D : array[0..8] of Single);
			1: (x, y, z, u, w, r, g, b, a : Single);
	end;
	PVertex3 = ^TVertex3;
  TVector3 = TVertex3;

	TRectf = packed record
		x, y, width, height : Single;
	end;
	PRectf = ^TRectf;

	TMatrix = packed record
		case byte of
			0: (_1D : array[0..15] of Single);
			1: (_2D : array[0..3, 0..3] of Single);
	end;
	PMatrix = ^TMatrix;

  TTransformStack = class
  private
{$IF CompilerVersion >= 20}
    stack: TStack<TMatrix>;
{$ELSE}
    stack: array of TMatrix;
{$IFEND}
    Constructor Create; overload;
  public
    Constructor Create(const base_transform: TMatrix); overload;
    procedure Push;
    procedure Pop;
    function Top: TMatrix;
    procedure Mult1(const transform: TMatrix);
    procedure Mult2(const transform: TMatrix);
  end;

	TMouseStates = packed record
		iX				: Integer;
		iY				: Integer;
		iDeltaX			: Integer;
		iDeltaY			: Integer;
		iDeltaWheel		: Integer;
		bLeftButton		: Boolean;
		bRightButton	: Boolean;
		bMiddleButton 	: Boolean;
	end;

	TKeyboardStates = packed record
		bCapsLock	: Boolean;
		bShiftL		: Boolean;
		bShiftR		: Boolean;
		bCtrlL		: Boolean;
		bCtrlR		: Boolean;
		bAltL		: Boolean;
		bAltR		: Boolean;
	end;

	TJoystickStates = packed record
		uiBtnsCount: Cardinal;	//**< Count of available joystick buttons. */
		bButtons: array[0..31] of Boolean;	//**< Array of joystick buttons states (pressed or not). */
    iXAxes : Integer;		    //**< X-axis position. Value varies -100 to 100. */
    iYAxes : Integer;		    //**< Y-axis position. Value varies -100 to 100. */
    iZAxes : Integer;		    //**< Z-axis position. Value varies -100 to 100. */
    iRAxes : Integer;			//**< Current position of the rudder or fourth joystick axis. Value varies -100 to 100. */
    iUAxes : Integer;			//**< Current fifth axis position. Value varies -100 to 100. */
    iVAxes : Integer;			//**< Current sixth axis position. Value varies -100 to 100. */
    iPOV	 : Integer;		  //**< Point-Of-View direction. */
	end;

TDrawDataDesc = record
  pData: Pointer; //Must be start of the vertex data. 2 or 3 floats

  uiVertexStride: Cardinal;
  bVertexCoord2: Boolean;

  uiNormalOffset: Cardinal; //3 floats
  uiNormalStride: Cardinal;

  uiTexCoordOffset: Cardinal; //2 floats
  uiTexCoordStride: Cardinal;

  uiColorOffset: Cardinal; //4 floats
  uiColorStride: Cardinal;

  pIndexBuffer: Pointer; //May point to separate memory. uint16 or uint32 data pointer.
  bIndexBuffer32: Boolean;

  //ToDo: Add VertexAttribPointers.
{$IF CompilerVersion >= 18}
  constructor Create(Dummy: Byte); overload;
  constructor Create(pData: Pointer; uiTexCoordDataOffset: Cardinal = minus1;
  bTwoCoordPerVertex: Boolean = True); overload;
{$IFEND}
end;


 const	//E_KEYBOARD_KEY_CODES

	KEY_ESCAPE			= $01;	// Escape
	KEY_TAB				= $0F;	// Tab
	KEY_GRAVE			= $29;	// accent grave "~"
	KEY_CAPSLOCK		= $3A;	// Caps Lock
	KEY_BACKSPACE		= $0E;	// Backspace
	KEY_RETURN			= $1C;	// Enter
	KEY_SPACE			= $39;	// Space
	KEY_SLASH			= $35;	// "/"
	KEY_BACKSLASH		= $2B;	// "\"

	KEY_SYSRQ			= $B7;	// PtrScr (SysRq)
	KEY_SCROLL			= $46;	// Scroll Lock
	KEY_PAUSE			= $C5;	// Pause

	KEY_INSERT			= $D2;	// Insert
	KEY_DELETE			= $D3;	// Delete
	KEY_HOME			= $C7;	// Home
	KEY_END				= $CF;	// End
	KEY_PGUP			= $C9;	// PgUp
	KEY_PGDN			= $D1;	// PgDn

	KEY_LSHIFT			= $2A;	// Left Shift
	KEY_RSHIFT			= $36;	// Right Shift
	KEY_LALT			= $38;	// Left Alt
	KEY_RALT			= $B8;	// Right Alt
	KEY_LWIN_OR_CMD			= $DB;	// Left Windows key
	KEY_RWIN_OR_CMD			= $DC;	// Right Windows key
	KEY_LCONTROL		= $1D;	// Left Control
	KEY_RCONTROL		= $9D;	// Right Control

	KEY_UP				= $C8;	// UpArrow
	KEY_RIGHT			= $CD;	// RightArrow
	KEY_LEFT			= $CB;	// LeftArrow
	KEY_DOWN			= $D0;	// DownArrow

	KEY_1				= $02;
	KEY_2				= $03;
	KEY_3				= $04;
	KEY_4				= $05;
	KEY_5				= $06;
	KEY_6				= $07;
	KEY_7				= $08;
	KEY_8				= $09;
	KEY_9				= $0A;
	KEY_0				= $0B;

	KEY_F1				= $3B;
	KEY_F2				= $3C;
	KEY_F3				= $3D;
	KEY_F4				= $3E;
	KEY_F5				= $3F;
	KEY_F6				= $40;
	KEY_F7				= $41;
	KEY_F8				= $42;
	KEY_F9				= $43;
	KEY_F10				= $44;
	KEY_F11				= $57;
	KEY_F12				= $58;

	KEY_Q				= $10;
	KEY_W				= $11;
	KEY_E				= $12;
	KEY_R				= $13;
	KEY_T				= $14;
	KEY_Y				= $15;
	KEY_U				= $16;
	KEY_I				= $17;
	KEY_O				= $18;
	KEY_P				= $19;
	KEY_A				= $1E;
	KEY_S				= $1F;
	KEY_D				= $20;
	KEY_F				= $21;
	KEY_G				= $22;
	KEY_H				= $23;
	KEY_J				= $24;
	KEY_K				= $25;
	KEY_L				= $26;
	KEY_Z				= $2C;
	KEY_X				= $2D;
	KEY_C				= $2E;
	KEY_V				= $2F;
	KEY_B				= $30;
	KEY_N				= $31;
	KEY_M				= $32;
	KEY_MINUS			= $0C;	// "-"
	KEY_PLUS			= $0D;	// "+"
	KEY_LBRACKET		= $1A;	// "["
	KEY_RBRACKET		= $1B;	// "]"

	KEY_SEMICOLON		= $27;	// ";"
	KEY_APOSTROPHE		= $28;	// '"'

	KEY_COMMA			= $33;	// ","
	KEY_PERIOD			= $34;	// "."

	KEY_NUMPAD0			= $52;
	KEY_NUMPAD1			= $4F;
	KEY_NUMPAD2			= $50;
	KEY_NUMPAD3			= $51;
	KEY_NUMPAD4			= $4B;
	KEY_NUMPAD5			= $4C;
	KEY_NUMPAD6			= $4D;
	KEY_NUMPAD7			= $47;
	KEY_NUMPAD8			= $48;
	KEY_NUMPAD9			= $49;
	KEY_NUMPADPERIOD	= $53;	// "." on numpad
	KEY_NUMPADENTER		= $9C;	// Enter on numpad
	KEY_NUMPADSTAR		= $37;	// "*" on numpad
	KEY_NUMPADPLUS		= $4E;	// "+" on numpad
	KEY_NUMPADMINUS		= $4A;	// "-" on numpad
	KEY_NUMPADSLASH		= $B5;	// "/" on numpad
	KEY_NUMLOCK			= $45;	// Num Lock on numpad


function PluginInfo(): TPluginInfo; {$IF CompilerVersion >= 18}inline;{$IFEND}

function WinMessage(): TWinMessage; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function WinMessage(msg: {E_WINDOW_MESSAGE_TYPE} Integer; param1: Cardinal = 0;
  param2: Cardinal = 0; param3: Pointer = nil): TWinMessage; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}


function Color4f(r,g,b,a: Single): TColor4;  {$IF CompilerVersion >= 18}inline;{$IFEND}
function Color4(r,g,b,a: Byte): TColor4; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Color4(color : Cardinal; alpha : Byte = 255): TColor4;  overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Color4(): TColor4; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Point2(): TPoint2; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Point2(x,y : Single): TPoint2; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Point2(const Floats: Array of Single): TPoint2; overload;
function Point3(): TPoint3; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Point3(x,y,z : Single): TPoint3; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Point3(const Floats: array of Single): TPoint3; overload; 
function Vertex2(x,y,u,w,r,g,b,a : Single): TVertex2; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Vertex3(x,y,z,u,w,r,g,b,a : Single): TVertex3; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Rectf(x, y, width, height : Single): TRectf; overload;  {$IF CompilerVersion >= 18}inline;{$IFEND}
function Rectf(): TRectf; overload;  {$IF CompilerVersion >= 18}inline;{$IFEND}
function RectF(const stLeftTop, stRightBottom: TPoint2): TRectf; overload;  {$IF CompilerVersion >= 18}inline;{$IFEND}


function IntersectRect(const stRect1, stRect2 : TRectf):Boolean; {$IF CompilerVersion >= 18}inline;{$IFEND}
function PointInRect(const stPoint : TPoint2; const stRect : TRectf):Boolean; {$IF CompilerVersion >= 18}inline;{$IFEND}
function RectInRect(const stRect1, stRect2 : TRectf): Boolean; {$IF CompilerVersion >= 18}inline;{$IFEND}
function GetIntersectionRect(const stRect1, stRect2 : TRectf): TRectf; {$IF CompilerVersion >= 18}inline;{$IFEND}

function Matrix(): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixMulGL(stMLeft, stMRight : TMatrix): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixInverse(const stMatrix : TMatrix): TMatrix; 
function MatrixTranspose(const stMatrix : TMatrix): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixIdentity(): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixScale(const fVec : TPoint3): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixTranslate(const fVec : TPoint3): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixRotate(angle : Single; const fAxis : TPoint3): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixBillboard(const stMatrix : TMatrix): TMatrix; {$IF CompilerVersion >= 18}inline;{$IFEND}
function EngWindow(): TEngWindow; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function EngWindow(uiWidth, uiHeight : Integer; bFullScreen : Boolean; bVSync : Boolean = false; msampling: {E_MULTISAMPLING_MODE} Cardinal = MM_NONE;
  uiFlags:{ENG_WINDOW_FLAGS}Integer = EWF_DEFAULT): TEngWindow; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixSub(stLeftMatrix, stRightMatrix : TMatrix): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixAdd(stLeftMatrix, stRightMatrix : TMatrix): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixMul(stLeftMatrix, stRightMatrix : TMatrix): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixSub(stLeftMatrix: TMatrix; right: Single): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixAdd(stLeftMatrix: TMatrix; right: Single): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixDiv(stLeftMatrix: TMatrix; right: Single): TMatrix; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function MatrixMul(stLeftMatrix: TMatrix; right: Single): TMatrix; overload;

function Min(a, b : single): Single; {$IF CompilerVersion >= 18}inline;{$IFEND}
function Max(a, b : single): Single; {$IF CompilerVersion >= 18}inline;{$IFEND}

function DrawDataDesc(): TDrawDataDesc; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}
function DrawDataDesc(pData: Pointer; uiTexCoordDataOffset: Cardinal = minus1;
  bTwoCoordPerVertex: Boolean = True): TDrawDataDesc; overload; {$IF CompilerVersion >= 18}inline;{$IFEND}



implementation

uses
  DGLE; // gets constants about SDK


function Color4f(r,g,b,a: Single): TColor4;
begin
	Result.r := r;
	Result.g := g;
	Result.b := b;
	Result.a := a;
end;

function Color4(r,g,b,a: Byte): TColor4; overload;
begin
	Result.r := r/255.;
	Result.g := g/255.;
	Result.b := b/255.;
	Result.a := a/255.;
end;

function Color4(color : Cardinal; alpha : Byte = 255): TColor4; overload;
begin
	Result.r := Byte(color shr 16)/255.;
	Result.g := Byte(color shr 8)/255.;
	Result.b := Byte(color)/255.;
	Result.a := alpha/255.;
end;

function Color4(): TColor4; overload;
begin
	Result.r := 1.;
	Result.g := 1.;
	Result.b := 1.;
	Result.a := 1.;
end;

function Point2(): TPoint2; overload;
begin
	Result.x := 0.;
	Result.y := 0.;
end;

function Point2(x,y : Single): TPoint2; overload;
begin
	Result.x := x;
	Result.y := y;
end;

function Point2(const Floats: array of Single): TPoint2; overload;
begin
  Result.x := Floats[0];
	Result.y := Floats[1];
end;

function Point3(): TPoint3; overload;
begin
	Result.x := 0.;
	Result.y := 0.;
	Result.z := 0.;
end;

function Point3(x,y,z : Single): TPoint3; overload;
begin
	Result.x := x;
	Result.y := y;
	Result.z := z;
end;

function Point3(const Floats: array of Single): TPoint3; overload;
begin
	Result.x := Floats[0];
	Result.y := Floats[1];
	Result.z := Floats[2];
end;

function Vertex2(x,y,u,w,r,g,b,a : Single): TVertex2;
begin
	Result.x := x;
	Result.y := y;
	Result.u := u;
	Result.w := w;
	Result.r := r;
	Result.g := g;
	Result.b := b;
	Result.a := a;
end;

function Vertex3(x,y,z,u,w,r,g,b,a : Single): TVertex3;
begin
	Result.x := x;
	Result.y := y;
	Result.z := z;
	Result.u := u;
	Result.w := w;
	Result.r := r;
	Result.g := g;
	Result.b := b;
	Result.a := a;
end;

function Rectf(x, y, width, height : Single): TRectf; overload;
begin
	Result.x		  := x;
	Result.y		  := y;
	Result.width	:= width;
	Result.height	:= height;
end;

function Rectf(): TRectf; overload;
begin
	Result.x		  := 0;
	Result.y		  := 0;
	Result.width	:= 0;
	Result.height	:= 0;
end;

function RectF(const stLeftTop, stRightBottom: TPoint2): TRectf; overload;
begin
  Result.x		  := stLeftTop.x;
	Result.y		  := stLeftTop.y;
	Result.width	:= stRightBottom.x - stLeftTop.x;
	Result.height	:= stRightBottom.y - stLeftTop.y;
end;

function IntersectRect(const stRect1, stRect2 : TRectf):Boolean;
begin
	Result :=
    ((stRect1.x < stRect2.x + stRect2.width) and
    (stRect1.x + stRect1.width > stRect2.x) and
    (stRect1.y < stRect2.y + stRect2.height) and
    (stRect1.y + stRect1.height > stRect2.y)) or
	  ((stRect2.x + stRect2.width < stRect1.x) and
    (stRect2.x > stRect1.x + stRect1.width) and
    (stRect2.y + stRect2.height < stRect1.y) and
    (stRect2.y > stRect1.y + stRect1.height));
end;

function PointInRect(const stPoint : TPoint2; const stRect : TRectf):Boolean;
begin
	Result := (stPoint.x > stRect.x) and (stPoint.x < stRect.x + stRect.width) 
    and (stPoint.y > stRect.y) and (stPoint.y < stRect.y + stRect.height);
end;

function RectInRect(const stRect1, stRect2 : TRectf): Boolean;
begin
  Result := (stRect1.x < stRect2.x) and (stRect1.y < stRect2.y) and
    (stRect1.x + stRect1.width > stRect2.x + stRect2.width) and
    (stRect1.y + stRect1.height > stRect2.y + stRect2.height);
end;

function GetIntersectionRect(const stRect1, stRect2: TRectf): TRectf; overload;
begin
  Result := stRect1;
  if stRect2.x > stRect1.x then
    Result.x := stRect2.x;
  if stRect2.y > stRect1.y then
    Result.y := stRect2.y;
  Result.width := Min(stRect2.x + stRect2.width, stRect1.x+ stRect1.width) - Result.x;
  Result.height := Min(stRect2.y + stRect2.height, stRect1.y + stRect1.height) - Result.y;
  if (Result.width <= 0) or (Result.height <= 0) then
    Result := Rectf;
end;

function Matrix(): TMatrix;
begin
	ZeroMemory(@Result._1D, 16 * SizeOf(Single));
end;

function MatrixMulGL(stMLeft, stMRight : TMatrix): TMatrix;
begin
	result := MatrixMul(stMRight, stMLeft);
end;

function MatrixInverse(const stMatrix : TMatrix): TMatrix;
type	MatrixRows = array[0..7] of Single;
		PMatrixRows = ^MatrixRows;
var		mat : array[0..3] of MatrixRows;
		rows : array[0..3] of PMatrixRows;
		i, r, row_num, c : Integer;
		major, cur_ABS, factor : Single;
begin
	mat[0, 0] := stMatrix._2D[0][0];
	mat[0, 1] := stMatrix._2D[0][1];
	mat[0, 2] := stMatrix._2D[0][2];
	mat[0, 3] := stMatrix._2D[0][3];
	mat[0, 4] := 1.0;
	mat[0, 5] := 0.0;
	mat[0, 6] := 0.0;
	mat[0, 7] := 0.0;

	mat[1, 0] := stMatrix._2D[1][0];
	mat[1, 1] := stMatrix._2D[1][1];
	mat[1, 2] := stMatrix._2D[1][2];
	mat[1, 3] := stMatrix._2D[1][3];
	mat[1, 4] := 0.0;
	mat[1, 5] := 1.0;
	mat[1, 6] := 0.0;
	mat[1, 7] := 0.0;

	mat[2, 0] := stMatrix._2D[2][0];
	mat[2, 1] := stMatrix._2D[2][1];
	mat[2, 2] := stMatrix._2D[2][2];
	mat[2, 3] := stMatrix._2D[2][3];
	mat[2, 4] := 0.0;
	mat[2, 5] := 0.0;
	mat[2, 6] := 1.0;
	mat[2, 7] := 0.0;

	mat[3, 0] := stMatrix._2D[3][0];
	mat[3, 1] := stMatrix._2D[3][1];
	mat[3, 2] := stMatrix._2D[3][2];
	mat[3, 3] := stMatrix._2D[3][3];
	mat[3, 4] := 0.0;
	mat[3, 5] := 0.0;
	mat[3, 6] := 0.0;
	mat[3, 7] := 1.0;

	rows[0] := PMatrixRows(@mat[0]);
	rows[1] := PMatrixRows(@mat[1]);
	rows[2] := PMatrixRows(@mat[2]);
	rows[3] := PMatrixRows(@mat[3]);


	for i := 0 to 3 do
	begin
		row_num := i;
		major := Abs(rows[i, i]);
		for r := i + 1 to 3 do
		begin
			cur_ABS := Abs(rows[r, i]);
			if cur_ABS > major then
			begin
				major := cur_ABS;
				row_num := r;
			end;
		end;
		if row_num <> i then
		begin
			rows[i]		 := Pointer(Integer(rows[i]) xor Integer(rows[row_num]));
			rows[row_num]   := Pointer(Integer(rows[i]) xor Integer(rows[row_num]));
			rows[i]		 := Pointer(Integer(rows[i]) xor Integer(rows[row_num]));
		end;
		for r := i + 1 to 3 do
		begin
			factor := rows[r, i] / rows[i, i];
			for c := i to 7 do rows[r, c] := rows[r, c] - factor * rows[i, c];
		end;
	end;

	for i := 3 downto 1 do
		for r := 0 to i - 1 do
		begin
			factor := rows[r, i] / rows[i, i];
			for c := 4 to 7 do rows[r, c] := rows[r, c] - factor * rows[i, c];
		end;

	Result._2D[0, 0] := rows[0, 4] / rows[0, 0];
	Result._2D[0, 1] := rows[0, 5] / rows[0, 0];
	Result._2D[0, 2] := rows[0, 6] / rows[0, 0];
	Result._2D[0, 3] := rows[0, 7] / rows[0, 0];

	Result._2D[1, 0] := rows[1, 4] / rows[1, 1];
	Result._2D[1, 1] := rows[1, 5] / rows[1, 1];
	Result._2D[1, 2] := rows[1, 6] / rows[1, 1];
	Result._2D[1, 3] := rows[1, 7] / rows[1, 1];

	Result._2D[2, 0] := rows[2, 4] / rows[2, 2];
	Result._2D[2, 1] := rows[2, 5] / rows[2, 2];
	Result._2D[2, 2] := rows[2, 6] / rows[2, 2];
	Result._2D[2, 3] := rows[2, 7] / rows[2, 2];

	Result._2D[3, 0] := rows[3, 4] / rows[3, 3];
	Result._2D[3, 1] := rows[3, 5] / rows[3, 3];
	Result._2D[3, 2] := rows[3, 6] / rows[3, 3];
	Result._2D[3, 3] := rows[3, 7] / rows[3, 3];
end;

function MatrixTranspose(const stMatrix : TMatrix): TMatrix;
begin
	Result._2D[0, 0] := stMatrix._2D[0, 0];
	Result._2D[0, 1] := stMatrix._2D[1, 0];
	Result._2D[0, 2] := stMatrix._2D[2, 0];
	Result._2D[0, 3] := stMatrix._2D[3, 0];

	Result._2D[1, 0] := stMatrix._2D[0, 1];
	Result._2D[1, 1] := stMatrix._2D[1, 1];
	Result._2D[1, 2] := stMatrix._2D[2, 1];
	Result._2D[1, 3] := stMatrix._2D[3, 1];

	Result._2D[2, 0] := stMatrix._2D[0, 2];
	Result._2D[2, 1] := stMatrix._2D[1, 2];
	Result._2D[2, 2] := stMatrix._2D[2, 2];
	Result._2D[2, 3] := stMatrix._2D[3, 2];

	Result._2D[3, 0] := stMatrix._2D[0, 3];
	Result._2D[3, 1] := stMatrix._2D[1, 3];
	Result._2D[3, 2] := stMatrix._2D[2, 3];
	Result._2D[3, 3] := stMatrix._2D[3, 3];
end;

function MatrixIdentity(): TMatrix;
begin
	Result := MatrixScale(Point3(1.0,1.0,1.0));
end;

function MatrixScale(const fVec : TPoint3): TMatrix; overload;
begin
	Result._2D[0, 0] := fVec.x;
	Result._2D[0, 1] := 0.0;
	Result._2D[0, 2] := 0.0;
	Result._2D[0, 3] := 0.0;

	Result._2D[1, 0] := 0.0;
	Result._2D[1, 1] := fVec.y;
	Result._2D[1, 2] := 0.0;
	Result._2D[1, 3] := 0.0;

	Result._2D[2, 0] := 0.0;
	Result._2D[2, 1] := 0.0;
	Result._2D[2, 2] := fVec.z;
	Result._2D[2, 3] := 0.0;

	Result._2D[3, 0] := 0.0;
	Result._2D[3, 1] := 0.0;
	Result._2D[3, 2] := 0.0;
	Result._2D[3, 3] := 1.0;
end;

function MatrixTranslate(const fVec : TPoint3): TMatrix;
begin
	Result := MatrixIdentity();
	Result._1D[12] := fVec.x;
	Result._1D[13] := fVec.y;
	Result._1D[14] := fVec.z;
end;

function MatrixRotate(angle : Single; const fAxis : TPoint3): TMatrix;
var axis_norm, x, y, z, sin_angle, cos_angle : Single;
begin
	axis_norm := sqrt(fAxis.x * fAxis.x + fAxis.y * fAxis.y + fAxis.z * fAxis.z);
	x := fAxis.x / axis_norm;
	y := fAxis.y / axis_norm;
	z := fAxis.z / axis_norm;
	sin_angle := sin(angle*Pi/180.0);
	cos_angle := cos(angle*Pi/180.0);

	Result._2D[0][0] := (1.0 - x * x) * cos_angle + x * x;
	Result._2D[0][1] := z * sin_angle + x * y * (1.0 - cos_angle);
	Result._2D[0][2] := x * z * (1.0 - cos_angle) - y * sin_angle;
	Result._2D[0][3] := 0.0;

	Result._2D[1][0] := x * y * (1.0 - cos_angle) - z * sin_angle;
	Result._2D[1][1] := (1.0 - y * y) * cos_angle + y * y;
	Result._2D[1][2] := y * z * (1.0 - cos_angle) + x * sin_angle;
	Result._2D[1][3] := 0.0;

	Result._2D[2][0] := x * z * (1.0 - cos_angle) + y * sin_angle;
	Result._2D[2][1] := y * z * (1.0 - cos_angle) - x * sin_angle;
	Result._2D[2][2] := (1.0 - z * z) * cos_angle + z * z;
	Result._2D[2][3] := 0.0;

	Result._2D[3][0] := 0.0;
	Result._2D[3][1] := 0.0;
	Result._2D[3][2] := 0.0;
	Result._2D[3][3] := 1.0;
end;

function MatrixBillboard(const stMatrix : TMatrix): TMatrix;
begin
	Result := MatrixIdentity();
	Result._2D[3, 0] := stMatrix._2D[3, 0];
	Result._2D[3, 1] := stMatrix._2D[3, 1];
	Result._2D[3, 2] := stMatrix._2D[3, 2];
	Result._2D[0, 3] := stMatrix._2D[0, 3];
	Result._2D[1, 3] := stMatrix._2D[1, 3];
	Result._2D[2, 3] := stMatrix._2D[2, 3];
	Result._2D[3, 3] := stMatrix._2D[3, 3];
end;

function MatrixSub(stLeftMatrix, stRightMatrix : TMatrix): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] - stRightMatrix._1D[i];
	end;
end;

function MatrixAdd(stLeftMatrix, stRightMatrix : TMatrix): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] + stRightMatrix._1D[i];
	end;
end;

function MatrixMul(stLeftMatrix, stRightMatrix : TMatrix): TMatrix;
//var
//  i, j, k: Integer;
begin
  Result := Matrix;

{This BLOCK of code, instead of cycle, makes very nice performance improvement}

  Result._2D[0][0] :=
    stLeftMatrix._2D[0][0] * stRightMatrix._2D[0][0] +
    stLeftMatrix._2D[0][1] * stRightMatrix._2D[1][0] +
    stLeftMatrix._2D[0][2] * stRightMatrix._2D[2][0] +
    stLeftMatrix._2D[0][3] * stRightMatrix._2D[3][0];
  Result._2D[1][0] :=
    stLeftMatrix._2D[1][0] * stRightMatrix._2D[0][0] +
    stLeftMatrix._2D[1][1] * stRightMatrix._2D[1][0] +
    stLeftMatrix._2D[1][2] * stRightMatrix._2D[2][0] +
    stLeftMatrix._2D[1][3] * stRightMatrix._2D[3][0];
  Result._2D[2][0] :=
    stLeftMatrix._2D[2][0] * stRightMatrix._2D[0][0] +
    stLeftMatrix._2D[2][1] * stRightMatrix._2D[1][0] +
    stLeftMatrix._2D[2][2] * stRightMatrix._2D[2][0] +
    stLeftMatrix._2D[2][3] * stRightMatrix._2D[3][0];
  Result._2D[3][0] :=
    stLeftMatrix._2D[3][0] * stRightMatrix._2D[0][0] +
    stLeftMatrix._2D[3][1] * stRightMatrix._2D[1][0] +
    stLeftMatrix._2D[3][2] * stRightMatrix._2D[2][0] +
    stLeftMatrix._2D[3][3] * stRightMatrix._2D[3][0];
  Result._2D[0][1] :=
    stLeftMatrix._2D[0][0] * stRightMatrix._2D[0][1] +
    stLeftMatrix._2D[0][1] * stRightMatrix._2D[1][1] +
    stLeftMatrix._2D[0][2] * stRightMatrix._2D[2][1] +
    stLeftMatrix._2D[0][3] * stRightMatrix._2D[3][1];
  Result._2D[1][1] :=
    stLeftMatrix._2D[1][0] * stRightMatrix._2D[0][1] +
    stLeftMatrix._2D[1][1] * stRightMatrix._2D[1][1] +
    stLeftMatrix._2D[1][2] * stRightMatrix._2D[2][1] +
    stLeftMatrix._2D[1][3] * stRightMatrix._2D[3][1];
  Result._2D[2][1] :=
    stLeftMatrix._2D[2][0] * stRightMatrix._2D[0][1] +
    stLeftMatrix._2D[2][1] * stRightMatrix._2D[1][1] +
    stLeftMatrix._2D[2][2] * stRightMatrix._2D[2][1] +
    stLeftMatrix._2D[2][3] * stRightMatrix._2D[3][1];
  Result._2D[3][1] :=
    stLeftMatrix._2D[3][0] * stRightMatrix._2D[0][1] +
    stLeftMatrix._2D[3][1] * stRightMatrix._2D[1][1] +
    stLeftMatrix._2D[3][2] * stRightMatrix._2D[2][1] +
    stLeftMatrix._2D[3][3] * stRightMatrix._2D[3][1];
  Result._2D[0][2] :=
    stLeftMatrix._2D[0][0] * stRightMatrix._2D[0][2] +
    stLeftMatrix._2D[0][1] * stRightMatrix._2D[1][2] +
    stLeftMatrix._2D[0][2] * stRightMatrix._2D[2][2] +
    stLeftMatrix._2D[0][3] * stRightMatrix._2D[3][2];
  Result._2D[1][2] :=
    stLeftMatrix._2D[1][0] * stRightMatrix._2D[0][2] +
    stLeftMatrix._2D[1][1] * stRightMatrix._2D[1][2] +
    stLeftMatrix._2D[1][2] * stRightMatrix._2D[2][2] +
    stLeftMatrix._2D[1][3] * stRightMatrix._2D[3][2];
  Result._2D[2][2] :=
    stLeftMatrix._2D[2][0] * stRightMatrix._2D[0][2] +
    stLeftMatrix._2D[2][1] * stRightMatrix._2D[1][2] +
    stLeftMatrix._2D[2][2] * stRightMatrix._2D[2][2] +
    stLeftMatrix._2D[2][3] * stRightMatrix._2D[3][2];
  Result._2D[3][2] :=
    stLeftMatrix._2D[3][0] * stRightMatrix._2D[0][2] +
    stLeftMatrix._2D[3][1] * stRightMatrix._2D[1][2] +
    stLeftMatrix._2D[3][2] * stRightMatrix._2D[2][2] +
    stLeftMatrix._2D[3][3] * stRightMatrix._2D[3][2];
  Result._2D[0][3] :=
    stLeftMatrix._2D[0][0] * stRightMatrix._2D[0][3] +
    stLeftMatrix._2D[0][1] * stRightMatrix._2D[1][3] +
    stLeftMatrix._2D[0][2] * stRightMatrix._2D[2][3] +
    stLeftMatrix._2D[0][3] * stRightMatrix._2D[3][3];
  Result._2D[1][3] :=
    stLeftMatrix._2D[1][0] * stRightMatrix._2D[0][3] +
    stLeftMatrix._2D[1][1] * stRightMatrix._2D[1][3] +
    stLeftMatrix._2D[1][2] * stRightMatrix._2D[2][3] +
    stLeftMatrix._2D[1][3] * stRightMatrix._2D[3][3];
  Result._2D[2][3] :=
    stLeftMatrix._2D[2][0] * stRightMatrix._2D[0][3] +
    stLeftMatrix._2D[2][1] * stRightMatrix._2D[1][3] +
    stLeftMatrix._2D[2][2] * stRightMatrix._2D[2][3] +
    stLeftMatrix._2D[2][3] * stRightMatrix._2D[3][3];
  Result._2D[3][3] :=
    stLeftMatrix._2D[3][0] * stRightMatrix._2D[0][3] +
    stLeftMatrix._2D[3][1] * stRightMatrix._2D[1][3] +
    stLeftMatrix._2D[3][2] * stRightMatrix._2D[2][3] +
    stLeftMatrix._2D[3][3] * stRightMatrix._2D[3][3];


//	for i := 0 to 3 do
//		for j := 0 to 3 do
//			for k := 0 to 3 do
//                Result._2D[i][j] := Result._2D[i][j] + stLeftMatrix._2D[i][k] * stRightMatrix._2D[k][j];
end;

function MatrixSub(stLeftMatrix: TMatrix; right: Single): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] - right;
	end;
end;

function MatrixAdd(stLeftMatrix: TMatrix; right: Single): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] + right;
	end;
end;

function MatrixDiv(stLeftMatrix: TMatrix; right: Single): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] / right;
	end;
end;

function MatrixMul(stLeftMatrix: TMatrix; right: Single): TMatrix;
var i: Integer;
begin
	Result := MatrixIdentity();
	for i := 0 to 15 do
	begin
		Result._1D[i] := stLeftMatrix._1D[i] * right;
	end;
end;

{$IF CompilerVersion >= 20}

constructor TTransformStack.Create;
begin
	inherited;
	stack := TStack<TMatrix>.Create();
end;

constructor TTransformStack.Create(const base_transform: TMatrix);
begin
	Create;
	stack.Push(base_transform);
end;

procedure TTransformStack.Push;
begin
	stack.Push(stack.Peek());
end;

procedure TTransformStack.Pop;
begin
if stack.Count > 1 then
	stack.Pop();
end;

function TTransformStack.Top: TMatrix;
begin
    Result := stack.Peek();
end;

procedure TTransformStack.Mult1(const transform: TMatrix);
begin
	stack.Push(MatrixMul(stack.Pop, transform));
end;

procedure TTransformStack.Mult2(const transform: TMatrix);
begin
	stack.Push(MatrixMul(transform, stack.Pop));
end;

{$ELSE}

constructor TTransformStack.Create;
begin
	inherited;
    SetLength(stack, 0);
end;

constructor TTransformStack.Create(const base_transform: TMatrix);
begin
	Create;
	SetLength(stack, 1);
	stack[0] := base_transform;
end;

procedure TTransformStack.Mult1(const transform: TMatrix);
begin
	stack[High(stack)] := MatrixMul(stack[High(stack)], transform);
end;

procedure TTransformStack.Mult2(const transform: TMatrix);
begin
	stack[High(stack)] := MatrixMul(transform, stack[High(stack)]);
end;

procedure TTransformStack.Pop;
begin
if(Length(stack) > 1) then
	SetLength(stack, Length(stack) - 1);
end;

procedure TTransformStack.Push;
begin
	SetLength(stack, Length(stack) + 1);
	stack[High(stack)] := stack[High(stack) - 1];
end;

function TTransformStack.Top: TMatrix;
begin
	Result := stack[High(stack)];
end;

{$IFEND}

function EngWindow(): TEngWindow; overload;
begin
	Result.uiWidth			:= 800;
	Result.uiHeight			:= 600;
	Result.bFullScreen		:= false;
	Result.bVSync			:= false;
	Result.eMSampling		:= MM_NONE;
	Result.uiFlags			:= EWF_DEFAULT;
end;

function EngWindow(uiWidth, uiHeight : Integer; bFullScreen : Boolean;
  bVSync : Boolean = false; msampling: {E_MULTISAMPLING_MODE} Cardinal = MM_NONE;
  uiFlags:{ENG_WINDOW_FLAGS}Integer = EWF_DEFAULT): TEngWindow; overload;
begin
	Result.uiWidth			:= uiWidth;
	Result.uiHeight			:= uiHeight;
	Result.bFullScreen		:= bFullScreen;
	Result.bVSync			:= bVSync;
	Result.eMSampling	:= msampling;
	Result.uiFlags			:= uiFlags;
end;

{$IF CompilerVersion >= 18}
constructor TPluginInfo.Create(var dummy);
begin
  Self := PluginInfo();
end;
{$IFEND}

function PluginInfo(): TPluginInfo;
begin
  Result.btPluginSDKVersion := _DGLE_PLUGIN_SDK_VER_;
end;

function Min(a, b : single): Single;
begin
  if a > b then
    Result := b
  else
    Result := a;
end;

function Max(a, b : single): Single;
begin
  if a < b then
    Result := b
  else
    Result := a;
end;

{$IF CompilerVersion >= 18}
constructor TDrawDataDesc.Create(Dummy: Byte);
begin
  Self := DrawDataDesc();
end;

constructor TDrawDataDesc.Create(pData: Pointer; uiTexCoordDataOffset: Cardinal = minus1;
  bTwoCoordPerVertex: Boolean = True);
begin
  Self := DrawDataDesc(pData, uiTexCoordDataOffset, bTwoCoordPerVertex);
end;
{$IFEND}

function DrawDataDesc(): TDrawDataDesc;
begin
  Result.pData := nil;
  Result.uiVertexStride := 0;
  Result.bVertexCoord2 := false;
  Result.uiNormalOffset := minus1;
  Result.uiNormalStride := 0;
  Result.uiTexCoordOffset := minus1;
  Result.uiTexCoordStride := 0;
  Result.uiColorOffset := minus1;
  Result.uiColorStride := 0;
  Result.pIndexBuffer := nil;
  Result.bIndexBuffer32 := false;
end;



function DrawDataDesc(pData: Pointer; uiTexCoordDataOffset: Cardinal = minus1;
  bTwoCoordPerVertex: Boolean = True): TDrawDataDesc;
begin
  Result.pData := pData;
  Result.uiVertexStride := 0;
  Result.bVertexCoord2 := bTwoCoordPerVertex;
  Result.uiNormalOffset := minus1;
  Result.uiNormalStride := 0;
  Result.uiTexCoordOffset := uiTexCoordDataOffset;
  Result.uiTexCoordStride := 0;
  Result.uiColorOffset := minus1;
  Result.uiColorStride := 0;
  Result.pIndexBuffer := nil;
  Result.bIndexBuffer32 := false;
end;

{$IF CompilerVersion >= 18}
constructor TWinMessage.Create(var dummy);
begin
  Self := WinMessage();
end;
constructor TWinMessage.Create(msg: {E_WINDOW_MESSAGE_TYPE} Integer; param1: Cardinal = 0;
  param2: Cardinal = 0; param3: Pointer = nil); 
begin
  Self := WinMessage(Msg, param1, param2, param3);
end;
{$IFEND}

function WinMessage(): TWinMessage; overload;
begin
  Result.uiMsgType := WMT_UNKNOWN;
  Result.ui32Param1 := 0;
  Result.ui32Param2 := 0;
  Result.pParam3 := nil;
end;

function WinMessage(msg: {E_WINDOW_MESSAGE_TYPE} Integer; param1: Cardinal = 0;
  param2: Cardinal = 0; param3: Pointer = nil): TWinMessage; overload;
begin
  Result.uiMsgType := msg;
  Result.ui32Param1 := param1;
  Result.ui32Param2 := param2;
  Result.pParam3 := param3;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}Clear({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND});
begin
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_UNKNOWN;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_i := 0;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_f := 0;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_p := nil;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}SetInt({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}iVal: Integer);
begin
  Clear();
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_INT;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_i := iVal;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}SetFloat({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}fVal: single);
begin
  Clear();
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_FLOAT;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_f := fVal;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}SetBool({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}bVal: Boolean);
begin
  Clear();
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_BOOL;
  if bVal then
    {$IF CompilerVersion < 18}AVar.{$IFEND}_i := 1
  else
    {$IF CompilerVersion < 18}AVar.{$IFEND}_i := 0;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}SetPointer({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pPointer: Pointer);
begin
  Clear();
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_POINTER;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_p := pPointer;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}SetData({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pData: Pointer; uiDataSize: Cardinal);
begin
  Clear();
  {$IF CompilerVersion < 18}AVar.{$IFEND}_type := DVT_DATA;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_p := pData;
  {$IF CompilerVersion < 18}AVar.{$IFEND}_i := uiDataSize;
end;

function {$IF CompilerVersion >= 18}TVariant.{$IFEND}AsInt({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Integer;
begin
  if (_type <> DVT_INT) then
    Result := 0
  else
    Result := {$IF CompilerVersion < 18}AVar.{$IFEND}_i;
end;

function {$IF CompilerVersion >= 18}TVariant.{$IFEND}AsFloat({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Single;
begin
  if (_type <> DVT_FLOAT) then
    Result := 0
  else
    Result := {$IF CompilerVersion < 18}AVar.{$IFEND}_f;
end;

function {$IF CompilerVersion >= 18}TVariant.{$IFEND}AsBool({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Boolean;
begin
  if (_type <> DVT_BOOL) then
    Result := false
  else
    Result := {$IF CompilerVersion < 18}AVar.{$IFEND}_i = 1;
end;

function {$IF CompilerVersion >= 18}TVariant.{$IFEND}AsPointer({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): Pointer;
begin
  if (_type <> DVT_POINTER) then
    Result := nil
  else
    Result := {$IF CompilerVersion < 18}AVar.{$IFEND}_p;
end;

procedure {$IF CompilerVersion >= 18}TVariant.{$IFEND}GetData({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}pData: Pointer; out uiDataSize: Cardinal);
begin
  if (_type <> DVT_DATA) then
  begin
    pData := nil;
    uiDataSize := 0;
  end
  else
  begin
    pData := {$IF CompilerVersion < 18}AVar.{$IFEND}_p;
    uiDataSize := {$IF CompilerVersion < 18}AVar.{$IFEND}_i;
  end;
end;

function {$IF CompilerVersion >= 18}TVariant.{$IFEND}GetType({$IF CompilerVersion < 18}var AVar: TVariant{$IFEND}): {E_DGLE_VARIANT_TYPE} Integer;
begin
  Result := {$IF CompilerVersion < 18}AVar.{$IFEND}_type;
end;


begin
end.
