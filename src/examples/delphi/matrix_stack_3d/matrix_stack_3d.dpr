program matrix_stack_3d;

uses
  Windows,
  SysUtils,
  Math,
  DGLE in '..\..\..\..\Include\Delphi3\DGLE.pas',
  DGLE_Base in '..\..\..\..\Include\Delphi3\DGLE_Base.pas',
  DGLE_CoreRenderer in '..\..\..\..\Include\Delphi3\DGLE_CoreRenderer.pas',
  DGLE_Types in '..\..\..\..\Include\Delphi3\DGLE_types.pas';

const
  APP_CAPTION   = 'matrix_stack_3d';
  DLL_PATH      = '..\..\..\DGLE.dll';
	RESOURCE_PATH = '..\..\..\..\resources\';
  SCREEN_WIDTH	= 1280;
  SCREEN_HEIGHT	= 800;
var

  transform    : TTransformStack;
  pEngineCore  : IEngineCore       = nil;
  pRender      : IRender           = nil;
  p_res_man    : IResourceManager  = nil;
  pRender3D    : IRender3D         = nil;
  pLightDirect : ILight            = nil;
  pLightSpot   : ILight            = nil;
  pTexFloor    : ITexture          = nil;
  pTexLight    : ITexture          = nil;
  pMdlDesk     : IModel            = nil;
  pMdlLamp     : IModel            = nil;
  pMdlChair    : IModel            = nil;
  pMdlMusicBox : IModel            = nil;
  pSnowGlobe   : IModel            = nil;
  pModelChurch : IModel            = nil;
  uiCounter    : Integer           = 1500;

procedure Init(pParametr : Pointer); stdcall;
var
  p_tex: ITexture;
  p_mat: IMaterial;
begin
  Randomize();
	pEngineCore.GetSubsystem(ESS_RESOURCE_MANAGER,IEngineSubSystem(p_res_man));
	pEngineCore.GetSubsystem(ESS_RENDER,IEngineSubSystem(pRender));
	pRender.GetRender3D(pRender3D);

  p_res_man.Load(RESOURCE_PATH+'textures\floor.dds', IEngineBaseObject(pTexFloor), TEXTURE_LOAD_DEFAULT_3D);
  p_res_man.Load(RESOURCE_PATH+'sprites\light.jpg',IEngineBaseObject(pTexLight), TEXTURE_LOAD_DEFAULT_2D);
	// some global lighting

	pRender3D.ToggleLighting(true);
	pRender3D.SetGlobalAmbientLighting(ColorBlack()); // turn off ambient lighting

  // setup lights

	// use single directional light to simulate ambient lighting
	p_res_man.CreateLight(pLightDirect);
	pLightDirect.SetType(LT_DIRECTIONAL);
	pLightDirect.SetColor(ColorGray()); // dim light
	pLightDirect.SetDirection(Point3(-0.5, 0.5, 0.75));
	pLightDirect.SetEnabled(true);
//
//	// Position is ignored for direction lights but is used by engine for debug drawing.
//	// Use "rnd3d_draw_lights 1" console command to debug lights.
	pLightDirect.SetPosition(Point3(0, 7.5, 0));
////
////	// create light for the table-lamp
	p_res_man.CreateLight(pLightSpot);
	pLightSpot.SetType(LT_SPOT);
	pLightSpot.SetColor(ColorYellow());
	pLightSpot.SetSpotAngle(100);
	pLightSpot.SetDirection(Point3(0.15, 0, -1));
	pLightSpot.SetEnabled(true);

  // create and setup materials and load models

	

	// desk
	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\desk\desk_diff.jpg', IEngineBaseObject(p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\desk\desk.dmd', IEngineBaseObject( pMdlDesk));
	pMdlDesk.SetModelMaterial(p_mat);

	// table-lamp
	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\table_lamp\lamp_diff.jpg', IEngineBaseObject( p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\table_lamp\lamp.dmd', IEngineBaseObject( pMdlLamp));
	pMdlLamp.SetModelMaterial(p_mat);

	// chair
	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\chair\chair_diff.jpg', IEngineBaseObject( p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\chair\chair.dmd', IEngineBaseObject( pMdlChair));
	pMdlChair.SetModelMaterial(p_mat);

	// music box
	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\music_box\mbox_d.dds', IEngineBaseObject( p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	p_res_man.Load(RESOURCE_PATH + 'meshes\furniture\music_box\music_box.dmd', IEngineBaseObject( pMdlMusicBox));
	pMdlMusicBox.SetModelMaterial(p_mat);

	// church
	p_res_man.Load(RESOURCE_PATH + 'meshes\church\church.dmd', IEngineBaseObject( pModelChurch));
	pModelChurch.SetModelMaterial(p_mat);

	p_res_man.CreateMaterial(p_mat);
	p_mat.SetDiffuseColor(ColorSilver());
	p_mat.SetSpecularColor(ColorWhite());
	p_mat.SetShininess(25);
	pModelChurch.SetMeshMaterial(0, p_mat);

	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\church\church_roof.jpg', IEngineBaseObject( p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	pModelChurch.SetMeshMaterial(1, p_mat);

	p_res_man.CreateMaterial(p_mat);
	p_res_man.Load(RESOURCE_PATH + 'meshes\church\church_main.jpg', IEngineBaseObject( p_tex), TEXTURE_LOAD_DEFAULT_3D);
	p_mat.SetDiffuseTexture(p_tex);
	pModelChurch.SetMeshMaterial(2, p_mat);

	// snow globe
	p_res_man.Load(RESOURCE_PATH + 'meshes\snow_globe.dmd', IEngineBaseObject( pSnowGlobe));

	p_res_man.CreateMaterial(p_mat);
	p_mat.SetDiffuseColor(ColorWhite());
	pSnowGlobe.SetMeshMaterial(0, p_mat);

	p_res_man.CreateMaterial(p_mat);
	p_mat.SetDiffuseColor(ColorBrown());
	p_mat.SetSpecularColor(ColorWhite());
	p_mat.SetShininess(25);
	pSnowGlobe.SetMeshMaterial(2, p_mat);

	p_res_man.CreateMaterial(p_mat);
	p_mat.SetDiffuseColor(ColorWhite(100));
	p_mat.SetSpecularColor(ColorWhite());
	p_mat.SetShininess(50);
	p_mat.SetBlending(true, BE_NORMAL);
	
	// When material with blending is set model will sort mesh order for meshes with blending to be the last.
	pSnowGlobe.SetMeshMaterial(1, p_mat);

	// We will use black fog to simulate darkness.
	pRender3D.SetFogColor(ColorBlack());
	pRender3D.SetLinearFogBounds(12.5, 20);
	pRender3D.ToggleFog(true);
  transform := TTransformStack.Create(MatrixIdentity());
end;

procedure Free(pParametr : Pointer); stdcall;
begin
//  p_res_man.FreeResource(IEngineBaseObject(pTexFloor));
//  p_res_man.FreeResource(IEngineBaseObject(pTexLight));

  pLightDirect := nil;
  pLightSpot   := nil;
  pTexFloor    := nil;
  pTexLight    := nil;
  pMdlDesk     := nil;
  pMdlLamp     := nil;
  pMdlChair    := nil;
  pMdlMusicBox := nil;
  pSnowGlobe   := nil;
  pModelChurch := nil;
  pEngineCore  := nil;
  pRender      := nil;
  pRender3D    := nil;
  p_res_man    := nil;
end;

procedure Update(pParametr : Pointer); stdcall;
begin
  if (uiCounter mod 25 = 0) then
		pLightSpot.SetEnabled(Random(100) < 75);
	Inc(uiCounter);
end;

procedure Render(pParametr : Pointer); stdcall;
var
  i, j: Integer;
  desk_extents, extents: TPoint3;
  desk_pos, light_pos: TPoint3;
	is_light_active: Boolean;
begin
  // camera setup
	transform.Clear(MatrixIdentity());
	transform.MultLocal(MatrixTranslate(Point3(0., -4.5, -4. -abs(sin(uiCounter / 250)) * 5)));
	transform.MultLocal(MatrixRotate(15. + cos(uiCounter / 200) * 10., Point3(1., 0., 0)));
	transform.MultLocal(MatrixRotate(uiCounter / 5., Point3(0., 1., 0)));

	pRender3D.SetMatrix(transform.Top);

	// set light in the world coordinate system
	pLightDirect.Update();

	// draw floor

	pRender.Unbind(EOT_MATERIAL); // unbind last material from previous frame

	for i := -5 to 4 do
		for j := -5 to 4 do
		begin
			transform.Push();

			transform.MultLocal(MatrixTranslate(Point3(i * 5., 0., j * 5)));
			transform.MultLocal(MatrixScale(Point3(5., 5., 5)));
			transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));

			pRender3D.SetMatrix(transform.Top);
			pTexFloor.Draw3D();

			transform.Pop();
		end;

	// draw desk

	
	pMdlDesk.GetExtents(desk_extents);
	desk_pos := Point3(0., desk_extents.z * 10., 0);

	transform.Push();
	transform.MultLocal(MatrixTranslate(desk_pos));
	transform.MultLocal(MatrixScale(Point3(10., 10., 10)));
	transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));
	transform.MultLocal(MatrixRotate(180., Point3(0., 0., 1)));
	pRender3D.SetMatrix(transform.Top);
	pMdlDesk.Draw();
	transform.Pop();

	// draw chair
	pMdlChair.GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(Point3(-1.5, extents.z * 5., 4.5)));
	transform.MultLocal(MatrixScale(Point3(5., 5., 5)));
	transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));
	transform.MultLocal(MatrixRotate(65., Point3(0., 0., 1)));
	pRender3D.SetMatrix(transform.Top);
	pMdlChair.Draw();
	transform.Pop();

	// draw music box
	pMdlMusicBox.GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(Add(desk_pos, Point3(-2.75, extents.y * 6., -1))));
	transform.MultLocal(MatrixScale(Point3(3., 3., 3)));
	transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));
	transform.MultLocal(MatrixRotate(25., Point3(0., 0., 1)));
	pRender3D.SetMatrix(transform.Top);
	pMdlMusicBox.Draw();
	transform.Pop();

	// draw table-lamp
	pMdlLamp.GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(Add(desk_pos, Point3(3.75, extents.z * 8. - 0.2, -1))));
	transform.MultLocal(MatrixScale(Point3(4., 4., 4)));
	transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));
	transform.MultLocal(MatrixRotate(-150., Point3(0., 0., 1)));
	pRender3D.SetMatrix(transform.Top);
	pMdlLamp.Draw();
	
	// here we set light position in the coordinate space of the lamp (object space)
	light_pos := Point3(0.35, 0., extents.z - 0.15);
	pLightSpot.SetPosition(light_pos);
	pLightSpot.Update();

	// draw light halo
	pLightSpot.GetEnabled(is_light_active);

	if (is_light_active) then
	begin
		pRender3D.ToggleLighting(false);
		pRender3D.SetBlendMode(BE_ADD);
		pRender3D.ToggleBlending(true);
		pRender3D.SetColor(ColorYellow(200));
		transform.MultLocal(MatrixTranslate(Add(light_pos, Point3(0., 0., -0.1))));
		transform.MultLocal(MatrixScale(Point3(0.25, 0.25, 0.25)));
		pRender3D.SetMatrix(MatrixBillboard(transform.Top));
		pTexLight.Draw3D();
		pRender3D.ToggleLighting(true);
	end;

	transform.Pop();

	// draw snow globe and church
	pSnowGlobe.GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(Add(desk_pos, Point3(1., extents.z * 4. + 0.75, 0.5))));
	transform.MultLocal(MatrixScale(Point3(2., 2., 2)));
	transform.MultLocal(MatrixRotate(-90., Point3(1., 0., 0)));

	transform.Push();
	transform.MultLocal(MatrixTranslate(Point3(0., 0., 0.125)));
	transform.MultLocal(MatrixScale(Point3(0.7, 0.7, 0.7)));
	pRender3D.SetMatrix(transform.Top);
	pModelChurch.Draw();
	transform.Pop();

	pRender3D.SetMatrix(transform.Top);
	pSnowGlobe.Draw();

	transform.Pop();

end;

begin
  if(GetEngine(DLL_PATH,pEngineCore)) then
  begin
   	if(SUCCEEDED(pEngineCore.InitializeEngine(0,APP_CAPTION,EngWindow()))) then
		begin
//      pEngineCore.ConsoleExecute('rnd3d_draw_lights 1');
//      pEngineCore.ConsoleExecute('rnd3d_draw_axes 1');
      pEngineCore.AddProcedure(EPT_INIT,     @Init);
			pEngineCore.AddProcedure(EPT_FREE,     @Free);
			pEngineCore.AddProcedure(EPT_Update,   @Update);
			pEngineCore.AddProcedure(EPT_RENDER,   @Render);
			pEngineCore.StartEngine(); //Entering engine loop
		end;
		FreeEngine();
  end
  else
    MessageBox(0,'Couldn''t load '+DLL_PATH+'!', APP_CAPTION, MB_OK or MB_ICONERROR or MB_SETFOREGROUND);
end.
