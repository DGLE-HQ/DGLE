using System;
using System.Collections.Generic;
using System.Linq;
using DGLE;
using System.Windows.Forms;

namespace AsteroidsGame
{
    class Game : IGameObjectManager
    {

        const string AppCaption = "Asteroids Game";
        const string DllPath = @"..\..\..\..\windows\DGLE.dll";
        const string ResPath = @"..\..\..\..\..\resources\";

        const uint ScreenWidth = 1024u;
        const uint ScreenHeight = 768u;

        private IEngineCore pEngineCore;
        private IRender2D pRender2D;
        private IInput pInput;
        private IBitmapFont pFnt;

        private int _debug = 0;
        private uint _uiScore;

        private List<CGameObject> _clObjects = new List<CGameObject>();
        private List<CGameObject> _clRemovePendingList = new List<CGameObject>();

        bool IsPlayerExists()
        {
            return _clObjects.Any(t => t.ObjType == CGameObject.EGameObjectType.GotPlayer);
        }

        bool AreAsteriodsRemain()
        {
            return _clObjects.Any(t => t.ObjType == CGameObject.EGameObjectType.GotAsteroid);
        }

        void StartGame()
        {
            _uiScore = 0;
            AddObject(new CBackGround(this, pEngineCore));
            AddObject(new CPlayer(this, pEngineCore));
            AddObject(new CAsteroid(this, pEngineCore, new TPoint2(200f, 100f), 300f));
        }

        void ClearGame()
        {
            _clObjects.Clear();
        }

        public bool IsDebugMode()
        {
            return _debug == 1;
        }

        public void IncreaseGameScore(uint uiByValue)
        {
            _uiScore += uiByValue;
        }

        public void AddObject(CGameObject pObj)
        {
            _clObjects.Add(pObj);
        }

        public void RemoveObject(CGameObject pObj)
        {
            _clRemovePendingList.Add(pObj);
        }

        void Init(IntPtr pParam)
        {
            IRender pRender;

            IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out pSubSys);
            pRender = (IRender)pSubSys;
            pRender.GetRender2D(out pRender2D);

            // All game logic is developed for this screen resolution, so will keep it (on logical level).
            pRender2D.SetResolutionCorrection(Res.GameVpWidth, Res.GameVpHeight);

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out pSubSys);
            pInput = (IInput)pSubSys;
            pInput.Configure(E_INPUT_CONFIGURATION_FLAGS.ICF_HIDE_CURSOR);

            IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            // Load resources and give them names to use in future via resource manager.
            IEngineBaseObject pBaseObj;

            pResMan.Load(ResPath + "meshes\\planets\\planet.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH, Res.MeshPlanet);
            pResMan.Load(ResPath + "meshes\\planets\\clouds.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D, Res.TexClouds);
            pResMan.Load(ResPath + "meshes\\planets\\earth.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D, Res.TexPlanet);
            pResMan.Load(ResPath + "sprites\\space_bg.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D, Res.TexStars);

            pResMan.Load(ResPath + "meshes\\planets\\asteroid.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH, Res.MeshAsteroid);
            pResMan.Load(ResPath + "meshes\\planets\\asteroid.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D, Res.TexAsteroid);

            pResMan.Load(ResPath + "meshes\\space_ship\\ship.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH, Res.MeshShip);
            pResMan.Load(ResPath + "meshes\\space_ship\\ship.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D, Res.TexShip);
            pResMan.Load(ResPath + "sprites\\spark.bmp", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D, Res.TexSpark);

            pResMan.Load(ResPath + "sounds\\explo.wav", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.RES_LOAD_DEFAULT, Res.SndExplo);
            pResMan.Load(ResPath + "sounds\\fire.wav", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.RES_LOAD_DEFAULT, Res.SndFire);
            pResMan.Load(ResPath + "sprites\\explo.jpg", out pBaseObj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D,
               Res.TexExplo);
            ((ITexture)pBaseObj).SetFrameSize(Res.ExploAnimationFramesSize, Res.ExploAnimationFramesSize);

            pResMan.Load(ResPath + "fonts\\Times_New_Roman_18_Bold.dft", out pBaseObj, 0, Res.FntMain);
            pFnt = (IBitmapFont)pBaseObj;

            StartGame();
        }

        void Free(IntPtr pParam)
        {

        }

        void Update(IntPtr pParam)
        {
            bool isPressed;

            // exit by pressing "Esc" key
            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out isPressed);
            if (isPressed)
            {
                pEngineCore.QuitEngine();
                return;
            }

            // if player is dead restart game on pressing "Enter"
            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_RETURN, out isPressed);
            if (isPressed && !IsPlayerExists())
            {
                ClearGame();
                StartGame();
            }

            // update all game objects
            for (int index = 0; index < _clObjects.Count; index++)
            {
                _clObjects[index].Update();
            }

            // do collision check
            var objectCount = _clObjects.Count;
            for (int i = 0; i < objectCount; i++)
            {
                if (!_clObjects[i].IsCollidable()) continue;
                for (int j = 0; j < _clObjects.Count; j++)
                {
                    if (_clObjects[j].IsCollidable() && i != j && _clObjects[i].CollisionCheck(_clObjects[j]))
                        _clObjects[i].OnCollision(_clObjects[j]);
                }
            }

            // remove objects waiting to be removed
            for (int i = 0; i < _clRemovePendingList.Count; i++)
            {
                for (int j = 0; j < _clObjects.Count; j++)
                {
                    if (_clObjects[j] != _clRemovePendingList[i]) continue;
                    _clObjects.Remove(_clObjects[j]);
                    break;
                }
            }
            _clRemovePendingList.Clear();

            _clObjects.Sort((obj1, obj2) => obj1.RenderLayer.CompareTo(obj2.RenderLayer));

            if (!AreAsteriodsRemain())
                AddObject(new CAsteroid(this, pEngineCore, new TPoint2(-290f, Res.GameVpHeight / 2f), 300f));

        }

        void Render(IntPtr pParam)
        {
            TColor4 c;
            uint w, h;
            string acTxt;
            // render game objects
            for (int i = 0; i < _clObjects.Count; i++)
            {
                _clObjects[i].Draw();
            }

            // render in-game user interface
            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_NORMAL);
            pFnt.SetScale(1f);

            // if player is dead draw message
            if (!IsPlayerExists())
            {
                acTxt = "You are dead! Press \"Enter\" to restart.";
                pFnt.GetTextDimensions(acTxt, out w, out h);
                c = TColor4.ColorRed();
                pFnt.Draw2DSimple((int)((Res.GameVpWidth - w) / 2), (int)(Res.GameVpHeight - h) / 2, acTxt, ref c);
            }

            // draw help before game start
            if (_uiScore == 0)
            {
                acTxt = "Use \"Arrows\" to move and \"Space Bar\" to shoot.";
                pFnt.GetTextDimensions(acTxt, out w, out h);
                c = TColor4.ColorWhite();
                pFnt.Draw2DSimple((int)((Res.GameVpWidth - w) / 2), (int)(Res.GameVpHeight - h), acTxt, ref c);
            }

            acTxt = "Score: " + _uiScore.ToString();
            pFnt.GetTextDimensions(acTxt, out w, out h);
            c = TColor4.ColorWhite();
            pFnt.Draw2DSimple((int)((Res.GameVpWidth - w) / 2), 0, acTxt, ref c);
        }

        void Run()
        {
            if (Engine.GetEngine(DllPath, out pEngineCore))
            {
                var win = new TEngineWindow(ScreenWidth, ScreenHeight, false);
                pEngineCore.InitializeEngine(IntPtr.Zero, AppCaption, ref win);

                pEngineCore.ConsoleVisible(false);

                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, new DSubscriber(Init), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, new DSubscriber(Free), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, new DSubscriber(Update), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, new DSubscriber(Render), IntPtr.Zero);

                pEngineCore.ConsoleRegisterVariable("game_debug", "Display game debug information.",
                    ref _debug, 0, 1, null, IntPtr.Zero);

                pEngineCore.StartEngine();
            }
            else
                MessageBox.Show("Couldn't load \"DGLE.dll\"!", AppCaption, MessageBoxButtons.OK, MessageBoxIcon.Error);
            Engine.FreeEngine();
        }

        [STAThread]
        static void Main()
        {
            var prog = new Game();
            prog.Run();
        }
    }
}