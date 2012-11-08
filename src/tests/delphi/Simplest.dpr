program Simplest;

uses
  Windows,
  SysUtils,
  Math,
  DGLE ,
  DGLE_types,
  DGLE_CoreRenderer;
  
const
  SCREEN_X = 800;
  SCREEN_Y = 600;
  APP_CAPTION   = 'Sample Simplest';
  DLL_PATH      = '..\..\..\bin\windows\DGLE.dll';
  RESOURCE_PATH = '..\..\..\resources\';
var

  pEngineCore : IEngineCore       = nil;
  pResMan     : IResourceManager  = nil;
  pInput      : IInput            = nil;
  pRender     : IRender           = nil;
  pRender2D   : IRender2D         = nil;
  pTex        : ITexture          = nil;

  counter: Integer = 0;

procedure Init(pParametr : Pointer); stdcall;
begin
  pEngineCore.GetSubsystem(ESS_RESOURCE_MANAGER, IEngineSubSystem(pResMan));
  pEngineCore.GetSubsystem(ESS_INPUT, IEngineSubSystem(pInput));
  pEngineCore.GetSubsystem(ESS_RENDER, IEngineSubSystem(pRender));
  pRender.GetRender2D(pRender2D);

  pResMan.Load(RESOURCE_PATH+'tests\npot_tex.bmp', IEngBaseObj(pTex), TEXTURE_LOAD_DEFAULT_2D);
end;

procedure Free(pParametr : Pointer); stdcall;
begin
  pResMan.FreeResource(IEngBaseObj(pTex));
  pTex := nil;
  pEngineCore := nil;
  pResMan     := nil;
  pInput      := nil;
  pRender     := nil;
  pRender2D   := nil;
end;

procedure Update(pParametr : Pointer); stdcall;
var
  b_prsd: Boolean;
begin
  pInput.GetKey(KEY_ESCAPE, b_prsd);
  if (b_prsd) then
    pEngineCore.QuitEngine();
  Inc(counter);
end;

procedure Render(pParametr : Pointer); stdcall;
var
  w, h: Cardinal;
begin
  pTex.GetDimension(w, h);
  pTex.Draw2DSimple((SCREEN_X - w) div 2, (SCREEN_Y - w) div 2);
end;

procedure MB(aMsg: PAnsiChar);
begin
  MessageBox(0, aMsg, APP_CAPTION, MB_OK or MB_ICONERROR or MB_SETFOREGROUND);
end;

begin
  if(GetEngine(DLL_PATH, pEngineCore)) then
  begin
   	if(SUCCEEDED(pEngineCore.InitializeEngine(0, APP_CAPTION, EngWindow(SCREEN_X, SCREEN_Y, false, false, MM_NONE, EWF_ALLOW_SIZEING), 33, EIF_LOAD_ALL_PLUGINS))) then
		begin
			// You can do some initialization here
			pEngineCore.ConsoleVisible(true);
			pEngineCore.AddProcedure(EPT_INIT,     @Init);
			pEngineCore.AddProcedure(EPT_FREE,     @Free);
			pEngineCore.AddProcedure(EPT_UPDATE,   @Update);
			pEngineCore.AddProcedure(EPT_RENDER,   @Render);
			pEngineCore.StartEngine(); //Entering engine loop
		end
    else
    begin
      Free(nil);
      MB('Couldn''t initialize DGLE!');
    end;
		FreeEngine();

  end
  else
    MB('Couldn''t load '+DLL_PATH+'!');
end.
