{
\file		DGLE_Base.pas
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		xx.xx.2012 (c)Korotkov Andrey

This header is a part of DGLE_SDK.
}
unit DGLE_Base;

interface

{$IFDEF FPC}
  {$MODE DELPHI}
  {$MACRO ON}
  {$DEFINE COMPILERVERSION := 12}
{$ENDIF}

{$IFNDEF DGLE_CRENDERER}
{$DEFINE DGLE_CRENDERER}
{$ENDIF}

uses
  DGLE_types;

const

  {Engine SubSystem interface flags}

  //E_ENGINE_SUB_SYSTEM
		ESS_RENDER						= 0;
		ESS_INPUT						  = 1;
		ESS_SOUND						  = 2;
		ESS_RESOURCE_MANAGER	= 3;
		ESS_FILESYSTEM				= 4;
		ESS_CORE_RENDERER			= 5;
type

  DGLE_RESULT = HRESULT;

	IDGLE_Base = interface(IUnknown)
	['{DFB1F52B-D906-4108-AD6F-3144E224688A}']

		function GetGUID(out stGuid: TGUID): DGLE_RESULT; stdcall;
  	function ExecCmd(uiCmd: Cardinal; out stVar: TVariant): DGLE_RESULT; stdcall;
		function ExecCmdStr(const pcCommand: pAnsiChar; pcResult: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
		function ExecCmdVar(pcCommand: pAnsiChar; out stVar: TVariant): DGLE_RESULT; stdcall;
	end;

  //Engine SubSystem interface//

	IEngineSubSystem = interface(IDGLE_Base)
	['{C682F875-E0BD-4af9-B79C-E209850025F8}']

		function GetType(out eSubSystemType :{E_ENGINE_SUB_SYSTEM}Integer): DGLE_RESULT; stdcall;

	end;

implementation

end.

