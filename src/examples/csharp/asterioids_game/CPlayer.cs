using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CPlayer : CGameObject
    {
        private const float _shootingDelay = 15;

        private IInput pInput;
        private IMesh pMesh;
        private ITexture pTex, pTexSpark;

        private TColor4 c = TColor4.ColorRed();

        private float _fVelocity;
        private uint uiShotPause;

        private TPoint3 dimPl;
        private TPoint3 axisPl = new TPoint3(0f, 0f, 1f);

        private void FireEngine(int side)
        {
            float multip = Res.Clamp(side, -1, 1);
            TPoint2 s = new TPoint2((float)(_stPos.x - Math.Cos(Res.ToRad(_fAngle - multip * 20f)) * 75f),
                (float)(_stPos.y - Math.Sin(Res.ToRad(_fAngle - multip * 20f)) * 75f));
            _pObjMan.AddObject(new CSpark(_pObjMan, _pEngineCore, s));
        }

        private void MakeShot()
        {
            if (!(uiShotPause > _shootingDelay)) return;
            _pObjMan.AddObject(new CShot(_pObjMan, _pEngineCore, new TPoint2((float)(_stPos.x +
                Math.Cos(Res.ToRad(_fAngle)) * 65f), (float)(_stPos.y + Math.Sin(Res.ToRad(_fAngle)) * 75f)), _fAngle));
            uiShotPause = 0;
        }

        public CPlayer(IGameObjectManager pObjMan, IEngineCore pEngineCore)
            : base(pObjMan, pEngineCore)
        {
            _fVelocity = 0f;
            uiShotPause = 15;

            ObjType = EGameObjectType.GotPlayer;

            _stPos = new TPoint2(Res.GameVpWidth / 2f, Res.GameVpHeight / 2f);
            _fSize = 150f;
            _fColScale = 0.6f;

            dimPl = new TPoint3(_fSize, _fSize, _fSize);

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out pSubSys);
            pInput = (IInput)pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.MeshShip, out pBaseObj);
            pMesh = (IMesh)pBaseObj;
            pResMan.GetResourceByName(Res.TexShip, out pBaseObj);
            pTex = (ITexture)pBaseObj;
            pResMan.GetResourceByName(Res.TexSpark, out pBaseObj);
            pTexSpark = (ITexture)pBaseObj;
        }

        public override void Update()
        {
            base.Update();

            ++uiShotPause;

            bool isPressed;
            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_SPACE, out isPressed);
            if (isPressed)
            {
                MakeShot();
            }

            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_LEFT, out isPressed);
            if (isPressed)
            {
                _fAngle -= 4f;
                FireEngine(-1); // fire left engine
            }

            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_RIGHT, out isPressed);
            if (isPressed)
            {
                _fAngle += 4f;
                FireEngine(1); // fire right engine
            }

            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_UP, out isPressed);
            if (isPressed)
            {
                _fVelocity = Res.Clamp(_fVelocity + 0.2f, 0f, 8f);
                //fire both engines
                FireEngine(-1);
                FireEngine(1);
            }
            else
            {
                _fVelocity = Res.Clamp(_fVelocity - 0.25f, 0f, 8f);
                // fire both engines sometimes
                if (_uiCounter % 4 == 0)
                {
                    FireEngine(-1);
                    FireEngine(1);
                }
            }

            _stPos.x = (float)Res.Clamp(_stPos.x + Math.Cos(Res.ToRad(_fAngle)) * _fVelocity, 0f, Res.GameVpWidth);
            _stPos.y = (float)Res.Clamp(_stPos.y + Math.Sin(Res.ToRad(_fAngle)) * _fVelocity, 0f, Res.GameVpHeight);
        }

        public override void Draw()
        {
            _pRender2D.DrawMesh(pMesh, pTex, ref _stPos, ref dimPl, ref axisPl, _fAngle - 90f,
                E_EFFECT2D_FLAGS.EF_DEFAULT, false);

            _pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);
            _pRender2D.SetColorMix(ref c);

            float sparkScale = Res.Clamp(uiShotPause / _shootingDelay, 0f, 1f);
            TPoint2 pos = new TPoint2((float)(_stPos.x - 32f * sparkScale + Math.Cos(Res.ToRad(_fAngle)) * 65f),
                (float)(_stPos.y - 32f * sparkScale + Math.Sin(Res.ToRad(_fAngle)) * 65f));
            TPoint2 dim = new TPoint2(64f * sparkScale, 64f * sparkScale);
            _pRender2D.DrawTexture(pTexSpark, ref pos, ref dim, _uiCounter / 2f,
                E_EFFECT2D_FLAGS.EF_COLOR_MIX | E_EFFECT2D_FLAGS.EF_BLEND);
            base.Draw();
        }

        public override void OnCollision(CGameObject pObj)
        {
            if (pObj.ObjType == EGameObjectType.GotAsteroid)
            {
                _pObjMan.AddObject(new CExplo(_pObjMan, _pEngineCore, _stPos, _fSize));
                _pObjMan.RemoveObject(this);
            }
        }

        public override bool IsCollidable()
        {
            return true;
        }
    }
}
