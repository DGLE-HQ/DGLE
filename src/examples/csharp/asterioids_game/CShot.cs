using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CShot : CGameObject
    {
        private const float _speed = 20f;
        private ITexture pTexSpark;

        public CShot(IGameObjectManager pObjMan, IEngineCore pEngineCore, TPoint2 stPos, float fAngle)
            : base(pObjMan, pEngineCore)
        {
            ObjType = EGameObjectType.GotShot;

            RenderLayer = 3;
            _fSize = 64f;
            _fColScale = 0.5f;
            this._stPos = stPos;
            this._fAngle = fAngle;

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.TexSpark, out pBaseObj);
            pTexSpark = (ITexture)pBaseObj;

            ISoundSample p_snd;
            pResMan.GetResourceByName(Res.SndFire, out pBaseObj);
            p_snd = (ISoundSample)pBaseObj;
            p_snd.Play();
        }

        public override void Update()
        {
            base.Update();

            // move shot spark
            _stPos.x += (float)Math.Cos(Res.ToRad(_fAngle)) * _speed;
            _stPos.y += (float)Math.Sin(Res.ToRad(_fAngle)) * _speed;

            // die if out of screen bounds
            if (_stPos.x < 0 || _stPos.x > Res.GameVpWidth || _stPos.y < 0 || _stPos.y > Res.GameVpHeight)
                _pObjMan.RemoveObject(this);
        }

        public override void Draw()
        {
            TColor4 c = TColor4.ColorRed();
            _pRender2D.SetColorMix(ref c);

            TPoint2 pos = new TPoint2(_stPos.x - _fSize / 2f, _stPos.y - _fSize / 2f);
            TPoint2 dim = new TPoint2(_fSize, _fSize);
            _pRender2D.DrawTexture(pTexSpark, ref pos, ref dim, _uiCounter * 2f, E_EFFECT2D_FLAGS.EF_COLOR_MIX |
                E_EFFECT2D_FLAGS.EF_BLEND);

            base.Draw();
        }

        public override bool IsCollidable()
        {
            return true;
        }
    }
}
