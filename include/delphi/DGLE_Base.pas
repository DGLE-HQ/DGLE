{
\file    DGLE_Base.pas
\author    Korotkov Andrey aka DRON
\version  2:0.3.0
\date    xx.xx.2012 (c)Korotkov Andrey

This header is a part of DGLE_SDK.
}
unit DGLE_Base;

interface

{$I include.inc}

{$IFNDEF DGLE_CRENDERER}
  {$DEFINE DGLE_CRENDERER}
{$ENDIF}

uses
  DGLE_types;

type

  E_ENGINE_OBJECT_TYPE  =
  (
    EOT_UNKNOWN            = 0,
    EOT_TEXTURE            = 1,
    EOT_MATERIAL           = 2,
    EOT_LIGHT              = 3,
    EOT_MESH               = 4,
    EOT_MODEL              = 5,
    EOT_BITMAP_FONT        = 6,
    EOT_SOUND_SAMPLE       = 7,
    EOT_MUSIC              = 8,

    EOT_EMPTY              = 9
  );

  {Engine SubSystem interface flags}

  E_ENGINE_SUB_SYSTEM =
  (
    ESS_RENDER             = 0,
    ESS_INPUT              = 1,
    ESS_SOUND              = 2,
    ESS_RESOURCE_MANAGER   = 3,
    ESS_FILESYSTEM         = 4,
    ESS_CORE_RENDERER      = 5
  );

  E_TEXTURE_DATA_FORMAT =
  (
    TDF_RGB8               = 0,
    TDF_RGBA8              = 1,
    TDF_ALPHA8             = 2,
    TDF_BGR8               = 3,
    TDF_BGRA8              = 4,
    TDF_DXT1               = 5,
    TDF_DXT5               = 6,
    TDF_DEPTH_COMPONENT24  = 7,
    TDF_DEPTH_COMPONENT32  = 8
  );

  E_LIGHT_TYPE =
  (
    LT_DIRECTIONAL         = 0,
    LT_POINT               = 1,
    LT_SPOT                = 2
  );

  DGLE_RESULT = HRESULT;

  IDGLE_Base = interface(IUnknown)
  ['{DFB1F52B-D906-4108-AD6F-3144E224688A}']

    function GetGUID(out stGuid: TGUID): DGLE_RESULT; stdcall;
    function ExecuteCommand(uiCmd: Cardinal; out stVar: TVariant): DGLE_RESULT; stdcall;
    function ExecuteTextCommand(pcCommand: pAnsiChar; out stVar: TVariant): DGLE_RESULT; stdcall;
    function ExecuteTextCommandEx(const pcCommand: pAnsiChar; pcResult: PAnsiChar; out uiCharsCount: Cardinal): DGLE_RESULT; stdcall;
  end;

  //Engine SubSystem interface//

  IEngineSubSystem = interface(IDGLE_Base)
  ['{C682F875-E0BD-4af9-B79C-E209850025F8}']

    function GetType(out eSubSystemType :E_ENGINE_SUB_SYSTEM): DGLE_RESULT; stdcall;

  end;

implementation

end.

