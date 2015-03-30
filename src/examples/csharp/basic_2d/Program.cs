using System;
using System.Collections.Generic;
using System.Linq;
using DGLE;
using System.Windows.Forms;

namespace Basic2D
{
    class Program
    {

        const string AppCaption = "Basic 2D";
        const string DllPath = @"..\..\..\..\windows\DGLE.dll";
        const string ResPath = @"..\..\..\..\..\resources\";

        const uint ScreenWidth = 1024u;
        const uint ScreenHeight = 768u;

        IEngineCore pEngineCore;
        IRender pRender;
        IRender2D pRender2D;

        private TColor4 stRandomCol;
        ITexture pTexGrass, pTexGirl, pTexTankBody, pTexTankTurret, pTexLight, pTexPlanet, pTexMask, pTexBg,
            pTexPlanetRenderIn, pTexLightRound, pTexJellyFish;
        private ITexture[] pTexAnimWater = new ITexture[11];
        IBitmapFont pFont, pFontBold, pFontHard;

        List<TVertex2> waterWaves = new List<TVertex2>();

        private uint counter = 0;
        Random rand = new Random();

        IntPtr pQuad, pTriangles;

        private float ToRad(float f)
        {
            return f * (float)(Math.PI / 180f);
        }

        void Init(IntPtr pParam)
        {
            IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out pSubSys);
            pRender = (IRender)pSubSys;
            TColor4 c = TColor4.ColorOfficialBlack();
            pRender.SetClearColor(ref c);
            pRender.GetRender2D(out pRender2D);

            IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj = null;

            // for fonts part
            pResMan.Load(ResPath + "fonts\\Times_New_Roman_12_rus.dft", out pBaseObj, 0);
            pFont = (IBitmapFont)pBaseObj;
            pResMan.Load(ResPath + "fonts\\Times_New_Roman_18_Bold.dft", out pBaseObj, 0);
            pFontBold = (IBitmapFont)pBaseObj;
            pResMan.Load(ResPath + "fonts\\TheHard_18.dft", out pBaseObj, 0);
            pFontHard = (IBitmapFont)pBaseObj;

            // for sprites part
            const uint unfilitered_2d_flag = (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_NONE |
                                                     E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pResMan.Load(ResPath + "textures\\cartoon_grass.tga", out pBaseObj, unfilitered_2d_flag |
                (uint)E_TEXTURE_LOAD_FLAGS.TLF_DECREASE_QUALITY_MEDIUM);
            pTexGrass = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_tank_body.png", out pBaseObj, unfilitered_2d_flag);
            pTexTankBody = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_tank_turret.png", out pBaseObj, unfilitered_2d_flag);
            pTexTankTurret = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_anime_girl.png", out pBaseObj, unfilitered_2d_flag);
            pTexGirl = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_mistery_light.jpg", out pBaseObj, unfilitered_2d_flag |
                (uint)E_TEXTURE_LOAD_FLAGS.TLF_DECREASE_QUALITY_HIGH);
            pTexLight = (ITexture)pBaseObj;

            pTexGirl.SetFrameSize(55, 117);
            pTexLight.SetFrameSize(64, 128);

            // for advanced part
            pResMan.Load(ResPath + "meshes\\planets\\asteroid.jpg", out pBaseObj, (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR |
                E_TEXTURE_LOAD_FLAGS.TLF_COORDS_MIRROR_REPEAT));
            pTexPlanet = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\light.jpg", out pBaseObj, 0);
            pTexLightRound = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\sphere_mask.png", out pBaseObj, 0);
            pTexMask = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_cloudy_night_sky.jpg", out pBaseObj, 0);
            pTexBg = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\jellyfish.png", out pBaseObj, 0);
            pTexJellyFish = (ITexture)pBaseObj;

            for (int i = 0; i < 11; i++)
            {
                pResMan.Load(ResPath + "textures\\water\\water_" + i + ".tga", out pBaseObj, (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR |
                E_TEXTURE_LOAD_FLAGS.TLF_COORDS_MIRROR_REPEAT));
                pTexAnimWater[i] = (ITexture)pBaseObj;
            }

            pResMan.CreateTexture(out pTexPlanetRenderIn, null, 256, 256, E_TEXTURE_DATA_FORMAT.TDF_RGBA8,
                E_TEXTURE_CREATE_FLAGS.TCF_DEFAULT, E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR |
                E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);

            TVertex2[] triangles = 
	        {
	            new TVertex2(25f, 155f, 0f, 0f, 0f, 1f, 0f, 1f),
	            new TVertex2(25f, 50f, 0f, 0f, 1f, 0f, 0f, 1f),
	            new TVertex2(140f, 155f, 0f, 0f, 0f, 0f, 1f, 1f)
	        };

            pTriangles = Unmanaged.New<TVertex2>(triangles.Length);
            pTriangles.Copy(triangles);

            pQuad = Unmanaged.New<TVertex2>(6);
        }

        void Free(IntPtr pParam)
        {
            pTriangles.Free();
            pQuad.Free();
        }

        void Update(IntPtr pParam)
        {
            RenderPlanetInToTexture();

            TRectF section_adv_rect = new TRectF(ScreenWidth / 2f, ScreenHeight / 2f,
                ScreenWidth / 2f, ScreenHeight / 2f);
            float bottom_y = section_adv_rect.y + section_adv_rect.height;
            float right_x = section_adv_rect.x + section_adv_rect.width;
            float texture_coord_koeff = 75f;

            waterWaves.Clear();
            waterWaves.Add(new TVertex2((int)section_adv_rect.x, (int)bottom_y,
                (int)(section_adv_rect.x / texture_coord_koeff), (int)(bottom_y / texture_coord_koeff),
                (int)0.274f, (int)0.863f, (int)0.863f, (int)1f));

            for (int i = 0; i < 33; ++i)
            {
                float x = section_adv_rect.x + i * section_adv_rect.width / 32f,
                y = (float)((section_adv_rect.y + section_adv_rect.height / 2f) + Math.Sin((float)counter / 20f + (float)i * 50f) * 15f);
                waterWaves.Add(new TVertex2(x, y, x / texture_coord_koeff, y / texture_coord_koeff, 0.274f, 0.863f, 0.863f, 1f));
            }

            waterWaves.Add(new TVertex2(right_x, bottom_y, right_x / texture_coord_koeff, bottom_y / texture_coord_koeff, 0.274f, 0.863f, 0.863f, 1f));

            // other stuff
            stRandomCol = new TColor4((byte)(rand.Next(255)), (byte)(rand.Next(255)), (byte)(rand.Next(255)), 255);
            ++counter;
        }

        void RenderPlanetInToTexture()
        {
            TColor4 c = TColor4.ColorWhite(0);
            pRender.SetClearColor(ref c);
            pRender.SetRenderTarget(pTexPlanetRenderIn);

            pRender2D.Begin2D();

            TPoint2 pos = new TPoint2();
            TPoint2 dim = new TPoint2(256f, 256f);
            pRender2D.DrawTexture(pTexMask, ref pos, ref dim, 0f, E_EFFECT2D_FLAGS.EF_BLEND);

            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_MASK);

            float texOffsetX = counter / 200f;
            TVertex2[] quad =
            {
                new TVertex2(0f, 0f, texOffsetX, 0f, 1f, 1f, 1f, 1f), 
                new TVertex2(256f, 0f, 1f + texOffsetX, 0f, 1f, 1f, 1f, 1f), 
                new TVertex2(256f, 256f, 1f + texOffsetX, 1f, 1f, 1f, 1f, 1f),
                new TVertex2(0f, 0f, texOffsetX, 0f, 1f, 1f, 1f, 1f), 
                new TVertex2(256f, 256f, 1f + texOffsetX, 1f, 1f, 1f, 1f, 1f), 
                new TVertex2(0f, 256f, 0f + texOffsetX, 1f, 1f, 1f, 1f, 1f)
            };

            pQuad.Copy(quad);
            pRender2D.DrawTriangles(pTexPlanet, pQuad, (uint)quad.Length, E_PRIMITIVE2D_FLAGS.PF_FILL);

            pRender2D.End2D();

            c = TColor4.ColorOfficialBlack();
            pRender.SetClearColor(ref c);
            pRender.SetRenderTarget(null);
        }

        void DrawPrimitives(TRectF screen)
        {
            TColor4 c;
            TRectF rect;

            rect = new TRectF(5f, 5f, 170f, 170f);
            c = TColor4.ColorBlue();
            pRender2D.DrawRectangle(ref rect, ref c, E_PRIMITIVE2D_FLAGS.PF_LINE);

            rect = new TRectF(10f, 10f, 160f, 160f);
            c = TColor4.ColorGray();
            pRender2D.DrawRectangle(ref rect, ref c, E_PRIMITIVE2D_FLAGS.PF_FILL);

            pRender2D.DrawTriangles(null, pTriangles, 3, E_PRIMITIVE2D_FLAGS.PF_FILL);

            for (int i = 0; i <= 12; i++)
            {
                TPoint2 p = new TPoint2(screen.x, screen.y) + new TPoint2(15f + 12 * i, 15f + 12 * i);
                c = TColor4.ColorTeal();
                pRender2D.DrawPoint(ref p, ref c, (uint)(1 + i));
            }

            for (int i = 0; i < 15; i++)
            {
                pRender2D.SetLineWidth((uint)(1 + i / 2));
                TPoint2 p = new TPoint2(screen.width - i * 20, screen.height);
                TPoint2 p1 = new TPoint2(screen.width, screen.height - i * 20);
                c = TColor4.ColorOfficialOrange((byte)(255 - i * 17));
                pRender2D.DrawLine(ref p, ref p1, ref c);
            }

            pRender2D.SetLineWidth(2);
            TColor4 c1 = TColor4.ColorAqua(), c2 = TColor4.ColorFuchsia(), c3 = new TColor4(), c4 = new TColor4();
            pRender2D.SetVerticesColors(ref c1, ref c2, ref c3, ref c4);
            c = new TColor4();
            TPoint2 p2 = new TPoint2(screen.width, 75f);
            TPoint2 p3 = new TPoint2(200f, screen.height);
            pRender2D.DrawLine(ref p2, ref p3, ref c, E_PRIMITIVE2D_FLAGS.PF_VERTICES_COLORS);

            pRender2D.SetLineWidth(1);

            c1 = TColor4.ColorGray();
            c2 = TColor4.ColorMagenta();
            c3 = TColor4.ColorOrange();
            c4 = TColor4.ColorViolet();
            pRender2D.SetVerticesColors(ref c1, ref c2, ref c3, ref c4);
            rect = new TRectF(250f, 25f, 125f, 125f);
            pRender2D.DrawRectangle(ref rect, ref c, (E_PRIMITIVE2D_FLAGS.PF_LINE | E_PRIMITIVE2D_FLAGS.PF_VERTICES_COLORS));

            c1 = TColor4.ColorBlack(0);
            c2 = TColor4.ColorGreen();
            c3 = TColor4.ColorGreen();
            c4 = TColor4.ColorBlack(0);
            pRender2D.SetVerticesColors(ref c1, ref c2, ref c3, ref c4);
            rect = new TRectF(260f, 35f, 105f, 105f);
            pRender2D.DrawRectangle(ref rect, ref c, (E_PRIMITIVE2D_FLAGS.PF_FILL | E_PRIMITIVE2D_FLAGS.PF_VERTICES_COLORS));

            p2 = new TPoint2(125f, 50f);
            c = TColor4.ColorOrange();
            pRender2D.DrawCircle(ref p2, 40, 32, ref c, E_PRIMITIVE2D_FLAGS.PF_FILL);

            c = TColor4.ColorWhite();
            for (int i = 0; i < 6; i++)
            {
                pRender2D.DrawCircle(ref p2, (uint)(10 + i * 5), (uint)(4 + i * 2), ref c, E_PRIMITIVE2D_FLAGS.PF_LINE);
            }

            p2 = new TPoint2(200f, 250f);
            p3 = new TPoint2(110f, 34f);
            c = TColor4.ColorViolet();
            pRender2D.DrawEllipse(ref p2, ref p3, 64, ref c, E_PRIMITIVE2D_FLAGS.PF_FILL);

            pRender2D.SetLineWidth(3);
            p3 = new TPoint2(100f, 30f);
            c = TColor4.ColorWhite();
            pRender2D.DrawEllipse(ref p2, ref p3, 32, ref c, E_PRIMITIVE2D_FLAGS.PF_LINE);

            p3 = new TPoint2(80f, 24f);
            c = TColor4.ColorFuchsia();
            pRender2D.DrawEllipse(ref p2, ref p3, 8, ref c, E_PRIMITIVE2D_FLAGS.PF_FILL);

            p3 = new TPoint2(30f, 75f);
            c = TColor4.ColorOrange(100);
            pRender2D.DrawEllipse(ref p2, ref p3, 64, ref c, E_PRIMITIVE2D_FLAGS.PF_FILL);

            float star_x = 25f, star_y = 250f;
            TVertex2[] star_poligon = 
	        {
	            new TVertex2(47f + star_x, 20f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
	            new TVertex2(60f + star_x, 48f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
	            new TVertex2(91f + star_x, 48f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
                new TVertex2(67f + star_x, 64f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(80f + star_x, 94f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(48f + star_x, 75f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(21f + star_x, 92f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(30f + star_x, 65f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(1f  + star_x, 49f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f),
		        new TVertex2(37f + star_x, 48f + star_y, 1f, 1f, 0.7f, 0.7f, 0f, 1f)
	        };
            pRender2D.DrawPolygon(null, ref star_poligon[0], 10, E_PRIMITIVE2D_FLAGS.PF_FILL);
        }

        void DrawFont(TRectF screen)
        {
            string txt;
            uint width, height;
            TColor4 c;

            txt = "I'm rotating... +_+";
            pFontBold.GetTextDimensions(txt, out width, out height);
            c = TColor4.ColorWhite();
            pFontBold.Draw2D(screen.x + screen.width - width - 10f, 200, txt, ref c, counter % 360);

            txt = "I'm just right aligned text. -_-";
            pFontBold.GetTextDimensions(txt, out width, out height);
            pFontBold.Draw2D((int)(screen.x + screen.width - width), 10, txt, ref c);

            txt = "I have a shadow! ;-)";
            pFontBold.GetTextDimensions(txt, out width, out height);
            c = TColor4.ColorBlack();
            pFontBold.Draw2D((int)(screen.x + screen.width - width - 7f), 53, txt, ref c);
            c = TColor4.ColorWhite();
            pFontBold.Draw2D((int)(screen.x + screen.width - width - 10f), 50, txt, ref c);

            txt = "Cool colored me! ^_^";
            pFontBold.GetTextDimensions(txt, out width, out height);
            TColor4 c1 = TColor4.ColorRed();
            TColor4 c2 = TColor4.ColorGreen();
            pRender2D.SetVerticesColors(ref c1, ref c2, ref c1, ref c2);
            c = TColor4.ColorWhite();
            pFontBold.Draw2D(screen.x + screen.width - width - 10f, 100f, txt, ref c, 0, true);

            txt = "I'm randomly colored... >_<";
            pFont.Draw2D(screen.x + 5f, 10, txt, ref stRandomCol);

            txt = "I'm per vertex colored! -_0";
            TColor4 c3 = TColor4.ColorAqua();
            TColor4 c4 = TColor4.ColorOrange();
            pRender2D.SetVerticesColors(ref c3, ref c3, ref c4, ref c4);
            c = TColor4.ColorWhite();
            pFontBold.Draw2D(screen.x + 5f, 50f, txt, ref c, 0, true);

            txt = "А я могу говорить по-русски! [:-|";
            c = TColor4.ColorRed();
            pFont.Draw2D(screen.x + 5f, 100, txt, ref c);

            txt = "I'm scaling... o_O";
            pFontBold.SetScale((float)Math.Abs(Math.Sin(counter / 50f)) * 2f);
            c = TColor4.ColorWhite();
            pFontBold.Draw2D(screen.x + 5f, 200, txt, ref c);
            pFontBold.SetScale(1f);

            txt = "I am just very brutal..!";
            pFontHard.GetTextDimensions(txt, out width, out height);
            c = TColor4.ColorOfficialOrange();
            pFontHard.Draw2D((int)(screen.x + (screen.width - width) / 2f), 300f, txt, ref c);
        }

        void DrawSprites(TRectF screen)
        {
            pTexGrass.Draw2D((int)screen.x, (int)screen.y, (uint)screen.width, (uint)screen.height, 0, 0);
            pTexGirl.Draw2DSimple((int)(screen.x + 400), (int)(screen.y + 30), (counter / 5) % 16);

            uint width, height;

            TPoint2 scale = new TPoint2(2f, 2f);
            pRender2D.SetScale(ref scale);

            pTexTankBody.GetDimensions(out width, out height);
            TPoint2 pos = new TPoint2(screen.x + 50f, screen.y + 150f);
            TPoint2 dimension = new TPoint2(width, height);
            pRender2D.DrawTexture(pTexTankBody, ref pos, ref dimension, 0, E_EFFECT2D_FLAGS.EF_ALPHA_TEST |
                E_EFFECT2D_FLAGS.EF_SCALE);

            pTexTankTurret.GetDimensions(out width, out height);
            TPoint2 rot = new TPoint2(48f, 64f);
            pRender2D.SetRotationPoint(ref rot);
            TPoint2 turretCenter = new TPoint2(screen.x + 50f - 10f, screen.y + 150f + 18f);
            float turretAngle = (float)(-30f + Math.Sin(counter / 15f) * 20f);
            dimension = new TPoint2(width, height);
            pRender2D.DrawTexture(pTexTankTurret, ref turretCenter, ref dimension, turretAngle, E_EFFECT2D_FLAGS.EF_ALPHA_TEST |
                E_EFFECT2D_FLAGS.EF_SCALE | E_EFFECT2D_FLAGS.EF_ROTATION_POINT);

            // draw light at the end of tank cannon
            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);
            pTexLight.GetFrameSize(out width, out height);
            dimension = new TPoint2(width, height);
            pos = turretCenter + new TPoint2((float)Math.Cos(ToRad(turretAngle)) * 165f,
                (float)(Math.Sin(ToRad(turretAngle - 15f)) * 165f));
            pRender2D.DrawTextureSprite(pTexLight, ref pos, ref dimension, (counter / 2) % 14, 0, E_EFFECT2D_FLAGS.EF_BLEND);

            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_NORMAL);
        }

        void DrawAdvanced(TRectF screen)
        {
            pTexBg.Draw2D((int)screen.x, (int)screen.y, (uint)screen.width, (uint)screen.height, 0, 0);

            TPoint2 pos = new TPoint2(screen.x + screen.width - 160f, screen.y + 20f);
            TPoint2 dim = new TPoint2(128f, 128f);
            pRender2D.DrawTexture(pTexPlanetRenderIn, ref pos, ref dim, 0);//, E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_FLIP_VERTICALLY);

            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);

            pRender2D.BeginBatch(false);
            pRender2D.DrawPolygon(pTexAnimWater[counter / 5 % 11], ref waterWaves.ToArray()[0], (uint)waterWaves.Count,
                E_PRIMITIVE2D_FLAGS.PF_FILL);
            pRender2D.EndBatch();

            uint w, h;
            pTexJellyFish.GetDimensions(out w, out h);
            float x_offset = (float)(Math.Abs(Math.Cos(counter / 20f)) * 50);
            float y_offset = (float)(Math.Sin(counter / 10f) * 75);
            TPoint2 jellyPos = new TPoint2(screen.x + 175f, screen.y + 200f);

            TPoint2 vert1 = new TPoint2();
            TPoint2 vert2 = new TPoint2();
            TPoint2 vert3 = new TPoint2(x_offset, -y_offset);
            TPoint2 vert4 = new TPoint2(-x_offset, -y_offset);
            pRender2D.SetVerticesOffsets(ref vert1, ref vert2, ref vert3, ref vert4);
            TColor4 c = TColor4.ColorViolet();
            pRender2D.SetColorMix(ref c);
            dim = new TPoint2(w / 2f, h / 2f);
            pRender2D.DrawTexture(pTexJellyFish, ref jellyPos, ref dim, -45f, E_EFFECT2D_FLAGS.EF_BLEND |
                E_EFFECT2D_FLAGS.EF_VERTICES_OFFSETS | E_EFFECT2D_FLAGS.EF_COLOR_MIX);

            pTexLightRound.GetDimensions(out w, out h);
            pos = jellyPos - new TPoint2(90f, 15f);
            dim = new TPoint2(w, h);
            pRender2D.DrawTexture(pTexLightRound, ref pos, ref dim, 0, E_EFFECT2D_FLAGS.EF_BLEND |
                E_EFFECT2D_FLAGS.EF_COLOR_MIX);
        }

        void Render(IntPtr pParam)
        {
            TRectF sectionRect;
            uint txt_w, txt_h;
            TColor4 c = TColor4.ColorWhite(); ;

            pRender2D.Begin2D();

            // First section with primitives. //
            sectionRect = new TRectF(0f, 0f, ScreenWidth / 2f, ScreenHeight / 2f);
            pRender.EnableScissor(ref sectionRect);

            DrawPrimitives(sectionRect);
            pRender2D.DrawRectangle(ref sectionRect, ref c, E_PRIMITIVE2D_FLAGS.PF_DEFAULT);

            const string txtSec1 = "Primitives";
            pFont.GetTextDimensions(txtSec1, out txt_w, out txt_h);
            pFont.Draw2DSimple((int)((ScreenWidth / 2 - txt_w) / 2), (int)(ScreenHeight / 2 - txt_h), txtSec1, ref c);

            // Second section with font rendering. //
            sectionRect = new TRectF(ScreenWidth / 2f, 0f, ScreenWidth / 2f, ScreenHeight / 2f);
            pRender.EnableScissor(ref sectionRect);

            DrawFont(sectionRect);
            pRender2D.DrawRectangle(ref sectionRect, ref c, E_PRIMITIVE2D_FLAGS.PF_DEFAULT);

            const string txtSec2 = "Fonts";
            pFont.GetTextDimensions(txtSec2, out txt_w, out txt_h);
            pFont.Draw2DSimple((int)(ScreenWidth / 2 + (ScreenWidth / 2 - txt_w) / 2),
                (int)(ScreenHeight / 2 - txt_h), txtSec2, ref c);

            // Third section with sprites. //
            sectionRect = new TRectF(0f, ScreenHeight / 2f, ScreenWidth / 2f, ScreenHeight / 2f);
            pRender.EnableScissor(ref sectionRect);

            DrawSprites(sectionRect);
            pRender2D.DrawRectangle(ref sectionRect, ref c, E_PRIMITIVE2D_FLAGS.PF_DEFAULT);

            const string txtSec3 = "Sprites";
            pFont.GetTextDimensions(txtSec3, out txt_w, out txt_h);
            pFont.Draw2DSimple((int)((ScreenWidth / 2 - txt_w) / 2f), (int)(ScreenHeight - txt_h - 25), txtSec3, ref c);

            // Fourth section with advanced techniques. //
            sectionRect = new TRectF(ScreenWidth / 2f, ScreenHeight / 2f, ScreenWidth / 2f, ScreenHeight / 2f);
            pRender.EnableScissor(ref sectionRect);

            DrawAdvanced(sectionRect);
            pRender2D.DrawRectangle(ref sectionRect, ref c, E_PRIMITIVE2D_FLAGS.PF_DEFAULT);

            const string txtSec4 = "Advanced";
            pFont.GetTextDimensions(txtSec4, out txt_w, out txt_h);
            pFont.Draw2DSimple((int)(ScreenWidth / 2 + (ScreenWidth / 2 - txt_w) / 2f), (int)(ScreenHeight - txt_h - 25), txtSec4, ref c);

            pRender.DisableScissor();

            pRender2D.End2D();
        }

        void Run()
        {
            if (Engine.GetEngine(DllPath, out pEngineCore))
            {
                //multisampling will smooth lines and primitive edges
                var win = new TEngineWindow(ScreenWidth, ScreenHeight, false, false, E_MULTISAMPLING_MODE.MM_4X);
                pEngineCore.InitializeEngine(IntPtr.Zero, AppCaption, ref win);

                pEngineCore.ConsoleVisible(false);

                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, new DSubscriber(Init), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, new DSubscriber(Free), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, new DSubscriber(Update), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, new DSubscriber(Render), IntPtr.Zero);

                pEngineCore.StartEngine();
            }
            else
                MessageBox.Show("Couldn't load \"DGLE.dll\"!", AppCaption, MessageBoxButtons.OK, MessageBoxIcon.Error);
            Engine.FreeEngine();
        }

        [STAThread]
        static void Main()
        {
            var prog = new Program();
            prog.Run();
        }
    }
}