program low_level_2d;

uses
  Windows,      
  DGLE in '..\..\..\..\include\delphi\DGLE.pas',
  DGLE_Types in '..\..\..\..\include\delphi\DGLE_Types.pas',
  DGLE_Base in '..\..\..\..\include\delphi\DGLE_Base.pas';

//outputpath ..\..\..\..\..\bin\examples\delphi\

const
  APP_CAPT = 'Low Level 2D';
  DLL_PATH = '..\..\..\..\bin\windows\DGLE.dll';
  RESOURCE_PATH = '..\..\..\..\resources\';

// This example is made for 800X600 screen resolution.
  GAME_VP_WIDTH	= 800;
  GAME_VP_HEIGHT =	600;
// Screen resolution
  SCREEN_WIDTH =	1024;
  SCREEN_HEIGHT =	768;
  HELP_TEXT : PAnsiChar = 'You can resize this window to see it''s behavior or go fullscreen by pressing "Alt + Enter" keys.';

var                                                          
  pEngineCore    : IEngineCore       = nil;
  pResMan        : IResourceManager  = nil;
  pRender        : IRender           = nil;
  pRender2D      : IRender2D         = nil;
  pInput         : IInput            = nil;
  pBg            : ITexture          = nil;
  pSky           : ITexture          = nil;
  pFog           : ITexture          = nil;
  pTexGirl       : ITexture          = nil;
  pLight         : ITexture          = nil;
  pLightRound    : ITexture          = nil;
  pOwl           : ITexture          = nil;
  pVox           : ITexture          = nil;
  pFont          : IBitmapFont       = nil;
  pSndOwl        : ISoundSample      = nil;
  pForestAmbient : ISoundSample      = nil;
  pChannelAmbientLoop: ISoundChannel = nil;

  Lights: array[0..4] of TPoint2;
  Counter: Integer = 800;
  prevWindowWidth, prevWindowHeight: Cardinal;
  CameraPosition: TPoint2;
  CameraAngle, CameraScale: Single;
  stMousePos: TPoint2 ;
  owlGoLeft: Boolean = False;
  owlX: Single  = -200;
  OwlSpeed: Single = 4;

procedure OnFullScreenEvent(pParametr: Pointer; const pEvent: IBaseEvent); stdcall;
var
  res_width, res_height: Cardinal;
  p_event: IEvGoFullScreen;
  go_fscreen: Boolean;
begin
  p_event := IEvGoFullScreen(pEvent);
  p_event.GetResolution(res_width, res_height, go_fscreen);

  if (go_fscreen) then
  begin
    prevWindowWidth := res_width;
    prevWindowHeight := res_height;
    pEngineCore.GetDesktopResolution( res_width, res_height);
    p_event.SetResolution(res_width, res_height);
  end
  else
    p_event.SetResolution(prevWindowWidth, prevWindowHeight);
end;

procedure Init( pParametr : Pointer ); stdcall;
begin
  pEngineCore.GetSubsystem( ESS_RESOURCE_MANAGER, IEngineSubSystem( pResMan ) );
  pEngineCore.GetSubsystem( ESS_RENDER, IEngineSubSystem( pRender ) );
  pEngineCore.GetSubsystem( ESS_INPUT,  IEngineSubSystem( pInput ) );
  pRender.GetRender2D( pRender2D );
  // Background clear color setup.
	pRender.SetClearColor(Color4(38, 38, 55, 255));
  // This example adapted only for 800X600 resolution, if even resolution will be higher it won't effect it.
	pRender2D.SetResolutionCorrection(GAME_VP_WIDTH, GAME_VP_HEIGHT);

  
  pInput.Configure( ICF_HIDE_CURSOR );

  //The way we can get default system font
  pResMan.GetDefaultResource( EOT_BITMAP_FONT, IEngineBaseObject( pFont ) );

  pResMan.Load(RESOURCE_PATH + 'sounds\owl.wav', IEngineBaseObject(pSndOwl));
  pResMan.Load(RESOURCE_PATH + 'sounds\forest_ambient.wav', IEngineBaseObject(pForestAmbient));

  pForestAmbient.PlayEx(pChannelAmbientLoop, SSP_LOOPED);


  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_forest_background.png',    IEngineBaseObject( pBg ),     TEXTURE_LOAD_DEFAULT_2D );
  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_cloudy_night_sky.jpg',   IEngineBaseObject( pSky ),    TEXTURE_LOAD_DEFAULT_2D );
  pResMan.Load( RESOURCE_PATH + 'textures\smoke.png',   IEngineBaseObject( pFog ),    TEXTURE_LOAD_DEFAULT_2D );
  pResMan.Load( RESOURCE_PATH + 'textures\light.jpg', IEngineBaseObject( pLightRound ), TEXTURE_LOAD_DEFAULT_2D );

  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_anime_girl.png', IEngineBaseObject( pTexGirl ), TEXTURE_LOAD_DEFAULT_2D );
  pTexGirl.SetFrameSize( 55, 117 );

  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_mistery_light.jpg', IEngineBaseObject( pLight ), TEXTURE_LOAD_DEFAULT_2D );
  pLight.SetFrameSize( 64, 128 );

  pResMan.Load( RESOURCE_PATH + 'sprites\vox.jpg', IEngineBaseObject( pVox ), TEXTURE_LOAD_DEFAULT_2D );
  pVox.SetFrameSize( 149, 149);

  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_owl.png', IEngineBaseObject( pOwl ), TEXTURE_LOAD_DEFAULT_2D );
  pOwl.SetFrameSize( 48, 128 );

  Lights[0] := Point2();
  Lights[1] := Point2(  750.0, 315.0 );
  Lights[2] := Point2( 1000.0, 450.0 );
  Lights[3] := Point2( 1010.0, 650.0 );
  Lights[4] := Point2(  150.0, 550.0 );

end;

procedure Update( pParametr : Pointer ); stdcall;
var
  prsd: Boolean;
  states: TMouseStates ;
  chan: ISoundChannel;
begin
  pInput.GetKey( KEY_ESCAPE, prsd );
  if prsd then
    pEngineCore.QuitEngine();

  pInput.GetMouseStates(states);
  stMousePos := Point2(states.iX, states.iY);

  if (counter mod 250 = 0) then
  begin
    pSndOwl.PlayEx(chan);
    chan.SetPan(Trunc(owlX - CameraPosition.x));
    chan.Unaquire();
  end;

  CameraPosition.x := GAME_VP_WIDTH / 2.0 + 250.0 + cos(counter / 200.0) * 225.0;
  CameraPosition.y := GAME_VP_HEIGHT / 2.0 + 300.0;

  CameraAngle := Sin( Counter / 200.0 ) * 5.0;
  CameraScale := 1.0 + Abs( Sin( Counter / 200.0 ) * 0.25 );

  Lights[0].X := 650.0 - Cos( Counter / 100.0 ) * 100.0;
  Lights[0].Y := 700.0 - Sin( Counter / 150.0 ) * 50.0;

  if (owlX > 1500) then
    owlGoLeft := true
  else
    if (owlX < 100) then
      owlGoLeft := false;
  if owlGoLeft then
    owlX :=  owlX - OwlSpeed
  else
    owlX := owlX + OwlSpeed;

  Inc( Counter );
end;

procedure Render( pParametr : Pointer ); stdcall;
var
  i, j: Integer;
  tw, th: Cardinal;
  OwlEffFlag: Cardinal;
  pos: TPoint2;
begin
  pRender2D.Begin2D();

  //Draw static sky

  pRender2D.DrawTexture( pSky, Point2(), Point2(GAME_VP_WIDTH, GAME_VP_HEIGHT) );

  //Draw background

  pRender2D.SetCamera( Point2( CameraPosition.X / 1.5, CameraPosition.Y / 1.5 ), CameraAngle, Point2( CameraScale, CameraScale ) );

  pRender2D.SetBlendMode( BE_NORMAL );

  pRender2D.DrawTextureCropped( pBg, Point2( -200.0, 150.0 ), Point2( 1399.0, 517.0 ), RectF( 0, 905, 1399, 517 ), 0, EF_BLEND );

  //Draw moving fog on background

  pRender2D.SetCamera( Point2( CameraPosition.X / 1.2, CameraPosition.Y / 1.2 ), CameraAngle, Point2( CameraScale, CameraScale ) );

  pRender2D.SetBlendMode( BE_ADD );

  // Sometimes it's better to batch a lot of similar objects.
	pRender2D.BeginBatch();

  pRender2D.SetColorMix( Color4( 255, 255, 255, 64 ) );

    for i := 0 to 21 do
      for j := 0 to 1 do
        pRender2D.DrawTexture( pFog,
              Point2( 500.0 + Sin( i ) * 600.0 - Cos((Counter / 50.0 ) ) * 10.0 * ( i mod 4 ),
                  475.0 + Sin( j * 2.5 ) * 150.0 - Sin( Counter / 50.0 ) * 20.0 * ( i mod 3 ) ),
                Point2( 250.0, 150.0 ), 0, ( EF_BLEND or EF_COLOR_MIX ) );

  pRender2D.EndBatch();

  //Draw foreground scene

  // Setup camera and blending

  pRender2D.SetCamera( CameraPosition, CameraAngle, Point2( CameraScale, CameraScale ) );

  pRender2D.SetBlendMode( BE_NORMAL );

  // Owl

  pRender2D.SetColorMix(Color4(150, 150, 150, 255)); // make sprite little darker
  if owlGoLeft then
    OwlEffFlag := EF_FLIP_HORIZONTALLY
  else
    OwlEffFlag := EF_DEFAULT;
	pRender2D.DrawTextureSprite(pOwl, Point2(owlX, 425.0), Point2(48.0, 128.0), (counter div 3) mod 15, 0.0, EF_BLEND or EF_COLOR_MIX or OwlEffFlag);

  // Draw tree

  pRender2D.DrawTextureCropped( pBg, Point2(), Point2( 1399.0, 900.0 ), RectF( 0, 0, 1399, 900 ), 0, EF_BLEND );

  // Glowing disc under moving light

  pRender2D.SetBlendMode(BE_ADD);

  pRender2D.SetVerticesOffsets( Point2( -100.0, 0.0 ), Point2( -100.0, 0.0 ), Point2(), Point2() );

  pRender2D.SetColorMix( Color4( 65, 59, 193, 255 ) );

  pRender2D.DrawTexture( pLightRound, Point2( Lights[0].X - 32.0, Lights[0].Y + 64.0 ), Point2( 256.0, 256.0 ), 0, (EF_VERTICES_OFFSETS or EF_BLEND or EF_COLOR_MIX ) );

  //Girl Shadow

  pRender2D.SetBlendMode( BE_NORMAL );

  pRender2D.SetVerticesOffsets( Point2( -150.0 + Cos( Counter / 100.0 ) * 100.0, -55.0 ), Point2( -50.0 + Cos( Counter / 100.0 ) * 100.0, -55.0 ), Point2( 15.0, 5.0), Point2( 15.0, 5.0 ) );

  pRender2D.SetColorMix( Color4( 0, 0, 0, 128 ) );

  pRender2D.DrawTextureSprite( pTexGirl, Point2( 550.0, 725.0 ), Point2( 60.0, 120.0 ), ( Counter div 5 ) mod 16, 0, (EF_VERTICES_OFFSETS or EF_BLEND or EF_COLOR_MIX ) );

  //Girl Sprite
  pRender2D.DrawTextureSprite( pTexGirl, Point2( 550.0, 725.0), Point2( 60.0, 120.0 ), ( Counter div 5 ) mod 16, 0, EF_BLEND );

  //Lights
  pRender2D.SetBlendMode( BE_ADD );
  for i := 0 to 4 do
    pRender2D.DrawTextureSprite( pLight, Point2( Lights[i].X, lights[i].Y ), Point2( 64.0, 128.0 ), ( Counter div 2 ) mod 14, 0, EF_BLEND );

  pRender2D.ResetCamera();
  // We must calculate correct coordinates in game space because game resolution and screen resolution can be different.
  pRender2D.AbsoluteToResolutionCorrect(stMousePos, pos);
  pRender2D.DrawTextureSprite(pVox, Point2(pos.x - 37, pos.y - 37), Point2(75.0, 75.0), (counter div 2) mod 16, 0.0, EF_BLEND);

  // Write text
  pFont.GetTextDimensions(HELP_TEXT, tw, th);
  pFont.Draw2D(((GAME_VP_WIDTH - tw) / 2), (GAME_VP_HEIGHT - th), HELP_TEXT, Color4());

  pRender2D.End2D();
end;

procedure Free( pParametr : Pointer ); stdcall;
begin
  // It's a good idea to stop and unaquire looped sound channels before quit.
  pChannelAmbientLoop.Stop();
  pChannelAmbientLoop.Unaquire();
  pLight      := nil;
  pTexGirl    := nil;
  pFog        := nil;
  pSky        := nil;
  pBg         := nil;
  pLightRound := nil;
  pOwl        := nil;
  pVox        := nil;
  pFont       := nil;
  pSndOwl     := nil;
  pForestAmbient := nil;
  pChannelAmbientLoop := nil;
  pRender2D   := nil;
  pInput      := nil;
  pRender     := nil;
  pResMan     := nil;
  pEngineCore := nil;
end;

begin
  if( GetEngine( DLL_PATH, pEngineCore ) ) then
  begin
    if( SUCCEEDED( pEngineCore.InitializeEngine( 0, APP_CAPT, EngWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_NONE, EWF_ALLOW_SIZEING) ) ) ) then
    begin
      pEngineCore.ConsoleVisible(True);
      pEngineCore.AddProcedure( EPT_INIT,     @Init );
      pEngineCore.AddProcedure( EPT_UPDATE,   @Update );
      pEngineCore.AddProcedure( EPT_RENDER,   @Render );
      pEngineCore.AddProcedure( EPT_FREE,     @Free );
      pEngineCore.AddEventListener(ET_ON_FULLSCREEN, @OnFullScreenEvent, nil);
      pEngineCore.StartEngine();
    end;
    FreeEngine();
  end
  else
    MessageBox( 0, 'Couldnt load '+DLL_PATH+'!', APP_CAPT, MB_OK or MB_ICONERROR or MB_SETFOREGROUND );
end.