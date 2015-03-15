program LowLevel3D;

uses
  Windows,
  Math,
  DGLE in '..\..\..\..\include\delphi\DGLE.pas',
  DGLE_Types in '..\..\..\..\include\delphi\DGLE_Types.pas',
  DGLE_CoreRenderer in '..\..\..\..\include\delphi\DGLE_CoreRenderer.pas',
  DGLE_Base in '..\..\..\..\include\delphi\DGLE_Base.pas';

//outputpath ..\..\..\..\bin\windows\examples\delphi\

const
  APP_CAPT = 'Low Level 3D';
  DLL_PATH = '..\..\..\..\bin\windows\DGLE.dll';
  RESOURCE_PATH = '..\..\..\..\resources\';

// Screen resolution
  SCREEN_WIDTH = 1024;
  SCREEN_HEIGHT = 768;

var                                                          
  pEngineCore    : IEngineCore       = nil;
  pResMan        : IResourceManager  = nil;
  pRender        : IRender           = nil;
  pCoreRenderer  : ICoreRenderer     = nil;  
  pRender3D      : IRender3D         = nil;
  pInput         : IInput            = nil;
  pTexGrass      : ITexture          = nil;
  pTexTree1      : ITexture          = nil;
  pTexTree2      : ITexture          = nil;
  pTexTree3      : ITexture          = nil;
  pTexOwl        : ITexture          = nil;
  pTexZard       : ITexture          = nil;
  pMeshTree1     : IMesh             = nil;
  pMeshTree2     : IMesh             = nil;
  pMeshTree3     : IMesh             = nil;
  pModelZard     : IModel            = nil;
  pFont          : IBitmapFont       = nil;

  uiCounter: Cardinal = 800;
  owlGoLeft: Boolean = False;
  owlX: Single = -2.;
  OwlSpeed: Single = 0.0075;
  
  c_afPlane: array[0..31] of Single = (
    // vertices
    {v1 = } -0.5, -0.5, 0, {v2 = } 0.5, -0.5, 0,
    {v3 = } -0.5,  0.5, 0, {v4 = } 0.5,  0.5, 0,
    // normals
    {n1 = } 0, 0, 1, {n2 = } 0, 0, 1,
    {n3 = } 0, 0, 1, {n4 = } 0, 0, 1,
    // texture vertices
    {t1 = } 0, 1, {t2 = } 1, 1,
    {t3 = } 0, 0, {t4 = } 1, 0
  );
  desc: TDrawDataDesc; // works only as global var

const 
  load_3d_flags: Cardinal = TLF_FILTERING_ANISOTROPIC or TLF_ANISOTROPY_4X or TLF_GENERATE_MIPMAPS;
  const txt: PAnsiChar = 'The very basics of the 3D graphics.';
  const txt_scale: Single = 0.005;

procedure Init( pParametr : Pointer ); stdcall;
begin
  pEngineCore.GetSubsystem( ESS_RESOURCE_MANAGER, IEngineSubSystem( pResMan ) );
  pEngineCore.GetSubsystem( ESS_RENDER, IEngineSubSystem( pRender ) );
  pEngineCore.GetSubsystem( ESS_CORE_RENDERER, IEngineSubSystem( pCoreRenderer ) );
  pEngineCore.GetSubsystem( ESS_INPUT,  IEngineSubSystem( pInput ) );
  pRender.GetRender3D( pRender3D );
  
// Background clear color setup.
  pRender.SetClearColor(ColorGray());

  pResMan.Load(RESOURCE_PATH + 'fonts\Times_New_Roman_18_Bold.dft', IEngineBaseObject(pFont));
  pResMan.Load(RESOURCE_PATH + 'textures\grass.jpg',    IEngineBaseObject(pTexGrass), load_3d_flags or TLF_COORDS_REPEAT);
  pResMan.Load( RESOURCE_PATH + 'sprites\cartoon_owl.png', IEngineBaseObject(pTexOwl), load_3d_flags or TLF_COORDS_CLAMP);
  pTexOwl.SetFrameSize( 48, 128 );

  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_1.png', IEngineBaseObject(pTexTree1), load_3d_flags or TLF_COORDS_CLAMP);
  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_1.dmd', IEngineBaseObject(pMeshTree1), MMLF_FORCE_MODEL_TO_MESH);
                                                                                  
  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_2.png', IEngineBaseObject(pTexTree2), load_3d_flags or TLF_COORDS_CLAMP);
  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_2.dmd', IEngineBaseObject(pMeshTree2), MMLF_FORCE_MODEL_TO_MESH);
                                                                                  
  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_3.png', IEngineBaseObject(pTexTree3), load_3d_flags or TLF_COORDS_CLAMP);
  pResMan.Load(RESOURCE_PATH + 'meshes\trees\tree_3.dmd', IEngineBaseObject(pMeshTree3), MMLF_FORCE_MODEL_TO_MESH);
                                                                                  
  pResMan.Load(RESOURCE_PATH + 'meshes\zard\zard_diff.dds', IEngineBaseObject(pTexZard), load_3d_flags or TLF_COORDS_CLAMP);
  pResMan.Load(RESOURCE_PATH + 'meshes\zard\zard_walk.dmd', IEngineBaseObject(pModelZard));

// add some fog to the scene
  pRender3D.SetFogColor(ColorGray());
  pRender3D.SetLinearFogBounds(1.5, 4);
  pRender3D.ToggleFog(True); 
  
// Here is the way for instant rendering of any custom geometry.
  desc := DrawDataDesc();
  desc.pData := @c_afPlane;
  desc.uiNormalOffset := 12 * SizeOf(Single);
  desc.uiTextureVertexOffset := 24 * SizeOf(Single);     
end;

procedure Update( pParametr : Pointer ); stdcall;
begin
  if (owlX > 2.5) then
    owlGoLeft := True
  else
    if (owlX < -2.5) then
      owlGoLeft := False;
  owlX := owlX + IfThen(owlGoLeft, -OwlSpeed, OwlSpeed);

  Inc(uiCounter);
end;

procedure Render( pParametr : Pointer ); stdcall;
var
  tw, th, meshes_count: Cardinal;
  cur_matrix, tree_rotate_and_scale, mat: TMatrix4;
  angle, speed: Single;
  extents, circle_center: TVector3;
begin
  // Attention!
  // The main thing you should keep in the mind is that matrix multiplication order should be
  // reversed when passing matrix to pRender3D.SetMatrix or pRender3D.MultMatrix methods.

  // camera setup
  // here and below matrix multiplication order is reversed
  mat := MatrixMul(
    MatrixMul(
      MatrixRotate(Sin(uiCounter / 150) * 65, Point3(0, 1, 0)), // 4. add some periodic rotation left-right to 65 degrees
      MatrixRotate(30, Point3(1, 0, 0))), // 3. look down a little
    MatrixMul(
      MatrixTranslate(Point3(0, 0, -2.5)), // 2. move camera backwards
      MatrixIdentity())); // 1. use identity matrix as zero point for all transformations

  pRender3D.SetMatrix(mat);

  // If you want direct multiplication order you could rewrite code above like this way:
  {
  // direct multiplication order here will make similar transformation as code above
  pRender3D.SetMatrix(MatrixIdentity()); // 1.
  pRender3D.MultMatrix(MatrixTranslate(TVector3(0, 0, -2.5f))); // 2.
  pRender3D.MultMatrix(MatrixRotate(30, TVector3(1, 0, 0))); // 3.
  pRender3D.MultMatrix(MatrixRotate(Sin(uiCounter / 150) * 65, TVector3(0, 1, 0))); // 4.
  }

  // Draw entire scene.

  // draw tiled grass floor

  // Some low-level things will be shown below, mainly for education purpose.
  // Of course, there is a way to do the same thing without using low-level API.

  pRender3D.PushMatrix(); // save current matrix
  
  // multiplicates current matrix with given one
  pRender3D.MultMatrix(MatrixMul(MatrixScale(Point3(8, 8, 8)),
    MatrixRotate(-90, Point3(1, 0, 0))
    ));

  pCoreRenderer.SetMatrix(MatrixScale(Point3(8, 8, 8)), MT_TEXTURE); // a simple way to tile texture

  pTexGrass.Bind(); // current texture setup
  pRender3D.Draw(desc, CRDM_TRIANGLE_STRIP, 4);

  pCoreRenderer.SetMatrix(MatrixIdentity(), MT_TEXTURE); // return texture matrix to its normal state

  pRender3D.PopMatrix(); // return previous matrix

  // Ok, that's all with low-level things in this example.

  // turn off backface culling because of trees leaves (they will look better) and sprites rendering (we want txt and owl to be visible from both sides)
  pRender3D.ToggleBackfaceCulling(False);

  // draw some trees

  // turn on alpha test for correct rendering of trees leaves
  pRender3D.ToggleAlphaTest(True);
  
  // precalculate similar for all trees transformation part to make it faster
  tree_rotate_and_scale := MatrixMul(MatrixScale(Point3(2, 2, 2)), MatrixRotate(-90, Point3(1, 0, 0)));

  // some copy-pasted code for each tree to draw all of them

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(2.3, 0, 1.2))));
  pTexTree1.Bind(); // way to set current texture
  pMeshTree1.Draw();
  pRender3D.PopMatrix();

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(-2.15, 0, -1.75))));
  pTexTree1.Bind();
  pMeshTree1.Draw();
  pRender3D.PopMatrix();

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(-0.5, 0, -1))));
  pTexTree2.Bind();
  pMeshTree2.Draw();
  pRender3D.PopMatrix();

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(0.75, 0, 0.1))));
  pTexTree2.Bind();
  pMeshTree2.Draw();
  pRender3D.PopMatrix();

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(0.5, 0, -1.5))));
  pTexTree3.Bind();
  pMeshTree3.Draw();
  pRender3D.PopMatrix();

  pRender3D.PushMatrix();
  pRender3D.MultMatrix(MatrixMul(tree_rotate_and_scale, MatrixTranslate(Point3(-0.75, 0, 0.25))));
  pTexTree3.Bind();
  pMeshTree3.Draw();
  pRender3D.PopMatrix();

  // we use blending for further font and sprite rendering
  pRender3D.ToggleBlending(True);

  // draw text
  
  pRender3D.PushMatrix();

  pFont.GetTextDimensions(txt, tw, th);

  pRender3D.MultMatrix(MatrixMul(
    MatrixScale(Point3(txt_scale, txt_scale, txt_scale)), // tex is drawing in rather huge coordinates, so we will downscale it
    MatrixTranslate(Point3(0, th / 2. * txt_scale, 1.25)) // move text up on half of it's height and little forward
    ));
  
  pRender3D.SetColor(ColorOfficialOrange()); // set current color

  pFont.Draw3D(txt);
  
  pRender3D.SetColor(ColorWhite()); // return color back to white

  pRender3D.PopMatrix();

  // draw owl animated sprite as billboard
  pRender3D.PushMatrix();

  // here is a way to make a billboard sprite in 3D

  pRender3D.GetMatrix(cur_matrix); // first we must get current matrix

  // now we set new current matrix
  mat := MatrixMul(
    MatrixMul(MatrixScale(Point3(0.35, 0.35, 0.35)),
      MatrixTranslate(Point3(owlX, 1, -0.35))),
    cur_matrix);

  pRender3D.SetMatrix(MatrixMul(
    MatrixScale(Point3(IfThen(owlGoLeft, -1, 1), 1, 1)), // the way to mirror sprite picture
    MatrixBillboard(mat))); // this function will remove any rotation from given matrix

  pTexOwl.Draw3D((uiCounter div 2) mod 15); // fast way to render texture as square plane with size 1.0

  pRender3D.PopMatrix();

  pRender3D.ToggleAlphaTest(False); // we don't need alphatest anymore
  pRender3D.ToggleBlending(False);
  pRender3D.ToggleBackfaceCulling(True); // turn backface culling back on here

  // draw monster

  pRender3D.PushMatrix();
  

  pModelZard.GetExtents(extents); // retrieves dimensions of the model (ex. extents.x is a half of the models width)
  
  // here we will let monster go circles
  angle := uiCounter / 75;
  speed := 1.2;
  circle_center := Point3(1, 0, 0.25);

  pRender3D.MultMatrix(MatrixMul(MatrixMul(MatrixRotate(-90, Point3(1, 0, 0)),
    MatrixRotate(angle * 180. / Pi, Point3(0, -1, 0))),
    MatrixTranslate(Add(Point3(Cos(angle) * speed, extents.z, Sin(angle) * speed), circle_center))));
  
  pTexZard.Bind();

  // this model uses frame based animation, we just need to switch between meshes
  pModelZard.MeshesCount(meshes_count);
  pModelZard.DrawMesh(uiCounter mod meshes_count);
  
  pRender3D.PopMatrix();
end;

procedure Free( pParametr : Pointer ); stdcall;
begin
  pEngineCore    := nil;
  pResMan        := nil;
  pRender        := nil;
  pCoreRenderer  := nil;
  pRender3D      := nil;
  pInput         := nil;
  pTexGrass      := nil;
  pTexTree1      := nil;
  pTexTree2      := nil;
  pTexTree3      := nil;
  pTexOwl        := nil;
  pTexZard       := nil;
  pMeshTree1     := nil;
  pMeshTree2     := nil;
  pMeshTree3     := nil;
  pModelZard     := nil;
  pFont          := nil;
end;

begin
  if( GetEngine( DLL_PATH, pEngineCore ) ) then
  begin
    if( SUCCEEDED( pEngineCore.InitializeEngine( 0, APP_CAPT, EngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, False, False, MM_4X) ) ) ) then
    begin
      pEngineCore.ConsoleVisible(True);
      pEngineCore.AddProcedure( EPT_INIT,     @Init );
      pEngineCore.AddProcedure( EPT_UPDATE,   @Update );
      pEngineCore.AddProcedure( EPT_RENDER,   @Render );
      pEngineCore.AddProcedure( EPT_FREE,     @Free );
      pEngineCore.StartEngine();
    end;
    FreeEngine();
  end
  else
    MessageBox( 0, 'Couldnt load '+DLL_PATH+'!', APP_CAPT, MB_OK or MB_ICONERROR or MB_SETFOREGROUND );
end.
