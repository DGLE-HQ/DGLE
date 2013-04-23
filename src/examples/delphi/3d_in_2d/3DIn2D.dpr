program d3DIn2D;
uses
  Windows, Math,
  DGLE in 'DGLE.pas',
  DGLE_Types in 'DGLE_Types.pas',
  DGLE_Base in 'DGLE_Base.pas',
  DGLE_CoreRenderer in 'DGLE_CoreRenderer.pas';

  {$I include.inc}

type
  TSceneObjects = (soHouse, soTree1, soTree2, soTree3, soCopter, soRotor, soZombie, soGrass, soStone);

const
  APP_CAPTION = 'Sample 3DIn2D';
  DLL_PATH = '..\..\..\..\bin\windows\DGLE.dll';
RESPATH = '..\..\..\..\resources\';
  SCREEN_WIDTH	= 1024;
  SCREEN_HEIGHT	= 768;
  meshfilenames: array[soHouse..soCopter] of AnsiString =
  (
    'meshes\house\house.dmd',
    'meshes\trees\tree_1.dmd',
    'meshes\trees\tree_2.dmd',
    'meshes\trees\tree_3.dmd',
    'meshes\copter\copter.dmd'
  );
  meshtexfilenames: array[soHouse..soCopter] of AnsiString =
  (
    'meshes\house\tex_diffuse.jpg',
    'meshes\trees\tree_1.png',
    'meshes\trees\tree_2.png',
    'meshes\trees\tree_3.png',
    'meshes\copter\copter.png'
  );

var
  pEngineCore: IEngineCore = nil;
  pResMan: IResourceManager = nil;
  pRender: IRender = nil;
  pRender2D: IRender2D = nil;
  pSound: ISoundSample = nil;
  pHelicopterSndChan: ISoundChannel = nil;
  pInp: IInput = nil;
  pMeshes: array[soHouse..soCopter] of IMesh;
  pTextures: array[TSceneObjects] of ITexture;
  pShadows: array[soHouse..soRotor] of ITexture;

  fCopterAngle: Single = 0.5;
  stCopterPos: TPoint2;
  stZombiePos: TPoint2;
  fZombieAngle: Single = 0;
  fZombieAnimFrame: Single = 0;
  bFrameForward: Boolean = True;
  Counter: Integer = 1;
  stMouseOnScreen, stMouseInCamera: TPoint2;

// This function creates texture and renders models shadow to it.
procedure RenderMeshToTexture(var pTex: ITexture; pMesh: IMesh; pMeshTex: ITexture);
begin
	pResMan.CreateTexture(pTex, nil, 256, 256, TDF_RGBA8, TCF_DEFAULT, TLF_FILTERING_BILINEAR);
	pRender.SetRenderTarget(pTex);
	pRender2D.DrawMesh(pMesh, pMeshTex, Point2(128, 128), Point3(256, 256, 1), Point3());
	pRender.SetRenderTarget(nil); // switch back to on-screen drawing
end;

procedure Init(pParametr: Pointer); stdcall;
var
  i: TSceneObjects;
begin
  pEngineCore.GetSubsystem(ESS_RESOURCE_MANAGER, IEngineSubSystem(pResMan));
  pEngineCore.GetSubsystem(ESS_RENDER, IEngineSubSystem(pRender));
  pEngineCore.GetSubsystem(ESS_INPUT, IEngineSubSystem(pInp));

  pRender.GetRender2D(pRender2D);
  pResMan.Load(RESPATH + 'sounds\helicopter.wav', IEngineBaseObject(pSound));
  pSound.PlayEx(pHelicopterSndChan, SSP_LOOPED);
// Meshes
  for i := soHouse to soCopter do
  begin
    pResMan.Load(PAnsiChar(RESPATH + meshtexfilenames[i]), IEngineBaseObject(pTextures[i]), TEXTURE_LOAD_DEFAULT_2D);
    pResMan.Load(PAnsiChar(RESPATH + meshfilenames[i]), IEngineBaseObject(pMeshes[i]), MMLF_FORCE_MODEL_TO_MESH);
    // render shadows
    RenderMeshToTexture(pshadows[i], pMeshes[i], pTextures[i]);
  end;

  pResMan.Load(RESPATH + 'grass.jpg', IEngineBaseObject(pTextures[soGrass]), TLF_FILTERING_BILINEAR or TLF_COORDS_REPEAT);
  pResMan.Load(RESPATH + 'textures\stone.tga', IEngineBaseObject(pTextures[soStone]));
  pResMan.Load(RESPATH + 'sprites\zombie.png', IEngineBaseObject(pTextures[soZombie]), TEXTURE_LOAD_DEFAULT_2D);
  pTextures[soZombie].SetFrameSize(256, 256);
  pResMan.Load(RESPATH + 'meshs\copter\propeller.png', IEngineBaseObject(pTextures[soRotor]), TEXTURE_LOAD_DEFAULT_2D);

  // render rotor shadow
	pResMan.CreateTexture(pShadows[soRotor], nil, 256, 256, TDF_RGBA8, TCF_DEFAULT, TLF_FILTERING_BILINEAR);
	pRender.SetRenderTarget(pShadows[soRotor]);
	pRender2D.DrawCircle(Point2(128, 128), 100, 64, ColorWhite(), PF_FILL);
	pRender.SetRenderTarget(nil);

  stCopterPos:= Point2(400, 200);
  stZombiePos:= Point2(10, 400);
end;

procedure Free(pParametr: Pointer); stdcall;
var
  I: TSceneObjects;
begin
  for I := Low(TSceneObjects) to High(TSceneObjects) do
  begin
    pResMan.FreeResource(IEngineBaseObject(pTextures[i]));
    pTextures[i] := nil;
  end;
  for I := Low(TSceneObjects) to soCopter do
  begin
    pResMan.FreeResource(IEngineBaseObject(pMeshes[i]));
    pMeshes[i] := nil;
  end;
  for I := Low(TSceneObjects) to sorotor do
  begin
    pResMan.FreeResource(IEngineBaseObject(pShadows[i]));
    pShadows[i] := nil;
  end;
  pEngineCore := nil;
  prender := nil;
  prender2D := nil;
  pHelicopterSndChan.Unaquire();
  pHelicopterSndChan := nil;
  pSound := nil;
  pResMan := nil;
  pInp := nil;
end;

procedure Update(pParametr: Pointer); stdcall;
var
  ms: TMouseStates;
  prsd: Boolean;
begin
	pInp.GetKey(KEY_ESCAPE, prsd);
	if prsd then
  begin
		pEngineCore.QuitEngine();
		Exit;
	End;

  fCopterAngle := fCopterAngle + 0.5;
  stCopterPos.x := stCopterPos.x + Cos(DegToRad(fCopterAngle)) * 4;
  stCopterPos.y := stCopterPos.y + Sin(DegToRad(fCopterAngle)) * 6;
  pHelicopterSndChan.SetPan(Trunc((stCopterPos.x + 60 - 460) * 0.2));

  if (bFrameForward) then
		fZombieAnimFrame := fZombieAnimFrame + 0.3
	else
		fZombieAnimFrame := fZombieAnimFrame - 0.3;

	if bFrameForward and (Round(fZombieAnimFrame) = 15) then
		bFrameForward := false
	else
		if not bFrameForward and (Round(fZombieAnimFrame) = 0) then
			bFrameForward := true;

  pInp.GetMouseStates(ms);
  stMouseOnScreen := Point2(ms.iX, ms.iY);

	fZombieAngle := RadToDeg(arctan2(stMouseInCamera.y - (stZombiePos.y + 64),
    stMouseInCamera.x - (stZombiePos.x + 64)));

	stZombiePos := Add(stZombiePos, Point2(cos(degTORAD(fZombieAngle)) * 0.75, Sin(degTORAD(fZombieAngle)) * 0.75));

  inc(counter);
end;

procedure Render(pParametr: Pointer); stdcall;
var
  I, J: Integer;
  X, Y: Cardinal;
  PtAxis: TPoint3;
  RotorPos, StoneDim: TPoint2;
  Col: TColor4;
begin
  pRender2D.Begin2D();

  pRender2D.SetCamera(Point2(350 + cos(Counter / 200) * 200, 350 + sin(Counter / 200) * 80), sin(Counter / 50) * 15, Point2(1, 1));

	// conver our on-screen mouse coordinates to camera space
	pRender2D.ProjectScreenToCamera(stMouseOnScreen, stMouseInCamera);

  pTextures[soStone].GetDimension(X, Y);
  StoneDim := Point2(X, Y);
  pTextures[soGrass].GetDimension(X, Y);

//  for I := -3 to 4 do
//    for J := -3 to 4 do
//      pTextures[soGrass].Draw2DSimple(i * X, j * Y);
	pRender2D.DrawTexture(pTextures[soGrass], Point2(-750, -750), Point2(2000, 2000), 0, EF_TILE_TEXTURE);

  // draw stone layer with per vertex blending
  Col := ColorWhite(0);
	pRender2D.SetVerticesColors(Col, ColorWhite(), ColorWhite(), Col);
	pRender2D.DrawTexture(pTextures[soStone], Point2(0, 225), StoneDim, 0, (EF_VERTICES_COLORS or EF_BLEND));

	pRender2D.SetVerticesColors(ColorWhite(), Col, Col, ColorWhite());
	pRender2D.DrawTexture(pTextures[soStone], Point2(0 + StoneDim.X, 225), StoneDim, 0, (EF_VERTICES_COLORS or EF_BLEND));

	pRender2D.SetVerticesColors(ColorWhite(), Col, Col, Col);
	pRender2D.DrawTexture(pTextures[soStone], Point2(0 + StoneDim.X, 225 + StoneDim.Y), StoneDim, 0, (EF_VERTICES_COLORS or EF_BLEND));

	pRender2D.SetVerticesColors(Col, ColorWhite(), Col, Col);
	pRender2D.DrawTexture(pTextures[soStone], Point2(0, 225 + StoneDim.Y), StoneDim, 0, (EF_VERTICES_COLORS or EF_BLEND));

	// set color mix to semi-transparent black for shadows
	pRender2D.SetColorMix(ColorBlack(128));

	// draw zombie, first draw shadow then zombie itself
	pRender2D.DrawTextureSprite(pTextures[soZombie], Add(stZombiePos , Point2(10, 10)), Point2(128, 128), Trunc(fZombieAnimFrame), fZombieAngle + 90, (EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTextureSprite(pTextures[soZombie], stZombiePos, Point2(128, 128), Trunc(fZombieAnimFrame), fZombieAngle + 90);

	// render objects shadows
	pRender2D.DrawTexture(pShadows[soHouse], Point2( 180 - 200, 150 - 200), Point2(450, 500), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soHouse], Point2( 600 - 200, 550 - 200), Point2(450, 500), 90, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));

	pRender2D.DrawTexture(pShadows[soTree1], Point2( 900 - 225, 500 - 225), Point2(450, 450), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soTree1], Point2(-250 - 225, 300 - 225), Point2(450, 450), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soTree1], Point2( 800 - 225, 200 - 225), Point2(450, 450), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soTree2], Point2(   0 - 175, 450 - 175), Point2(350, 350), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soTree2], Point2(  50 - 175, 750 - 175), Point2(350, 350), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));
	pRender2D.DrawTexture(pShadows[soTree3], Point2( 500 - 225, 150 - 225), Point2(450, 450), 0, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));

  // draw 3D models in 2D space
  PtAxis := Point3(0, 0, 1);
  pRender2D.DrawMesh(pMeshes[soHouse], pTextures[soHouse], Point2( 180, 150), Point3(400, 400, 600), PtAxis);
  pRender2D.DrawMesh(pMeshes[soHouse], pTextures[soHouse], Point2( 600, 550), Point3(400, 400, 600), PtAxis, 90);
  pRender2D.DrawMesh(pMeshes[soTree1], pTextures[soTree1], Point2( 900, 500), Point3(400, 400, 500), PtAxis);
  pRender2D.DrawMesh(pMeshes[soTree1], pTextures[soTree1], Point2(-250, 300), Point3(400, 400, 500), PtAxis);
  pRender2D.DrawMesh(pMeshes[soTree1], pTextures[soTree1], Point2( 800, 200), Point3(400, 400, 400), PtAxis);
  pRender2D.DrawMesh(pMeshes[soTree2], pTextures[soTree2], Point2(   0, 450), Point3(300, 300, 400), PtAxis);
  pRender2D.DrawMesh(pMeshes[soTree2], pTextures[soTree2], Point2(  50, 750), Point3(300, 300, 300), PtAxis);
  pRender2D.DrawMesh(pMeshes[soTree3], pTextures[soTree3], Point2( 500, 150), Point3(400, 400, 500), PtAxis);

  	// draw copter model and rotor sprite
	RotorPos := Point2(stCopterPos.x - 200. + cos(degTORAD(fCopterAngle)) * 80, stCopterPos.y - 200 + sin(degTORAD(fCopterAngle)) * 80);
	pRender2D.DrawTexture(pShadows[soCopter], Subtract(stCopterPos, point2(200, 200)), point2(600, 600), fCopterAngle, (EF_FLIP_VERTICALLY or EF_COLOR_MIX or EF_BLEND));

	// make blinky rotor shadow
	pRender2D.SetColorMix(ColorBlack(IfThen(Counter mod 3 = 2, 64, 16)));
	pRender2D.DrawTexture(pShadows[soRotor], Add(RotorPos, point2(100, 100)), point2(400, 400), 0, (EF_COLOR_MIX or EF_BLEND));

	pRender2D.DrawMesh(pMeshes[socopter], pTextures[socopter], stCopterPos, Point3(600, 600, 600), PtAxis, fCopterAngle, EF_DEFAULT, false, 90, true);
	pRender2D.DrawTexture(pTextures[soRotor], RotorPos, point2(400, 400), Counter * 25);

  pRender2D.End2D();
end;

begin
  if (GetEngine(DLL_PATH, pEngineCore)) then
  begin
    if (SUCCEEDED(pEngineCore.InitializeEngine(0, APP_CAPTION,
      EngWindow(1024, 768, False, False, 0, EWF_ALLOW_SIZEING)))) then
    begin
      pEngineCore.ConsoleVisible(true);
      pEngineCore.AddProcedure(EPT_INIT, @Init);
      pEngineCore.AddProcedure(EPT_FREE, @Free);
      pEngineCore.AddProcedure(EPT_UPDATE, @Update);
      pEngineCore.AddProcedure(EPT_RENDER, @Render);
      pEngineCore.StartEngine();
    end;
    FreeEngine();
  end
  else
    MessageBox(0, 'Couldnt load ' + DLL_PATH + '!', APP_CAPTION, MB_OK or
      MB_ICONERROR or MB_SETFOREGROUND);
end.

