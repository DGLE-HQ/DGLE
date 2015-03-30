using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CSpark : CGameObject
    {
        private ITexture pTexSpark;
        private TPoint2 pos;
        private TPoint2 dim;

        public CSpark(IGameObjectManager pObjMan, IEngineCore pEngineCore, TPoint2 stPos)
            : base(pObjMan, pEngineCore)
        {
            RenderLayer = 1;
            _fSize = 96f;
            this._stPos = stPos;
            _fAngle = Rand.Next(360);

            pos = new TPoint2(stPos.x - _fSize / 2f, stPos.y - _fSize / 2f);
            dim = new TPoint2(_fSize, _fSize);

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.TexSpark, out pBaseObj);
            pTexSpark = (ITexture)pBaseObj;
        }

        public override void Update()
        {
            base.Update();

            if (_uiCounter == 20)
                _pObjMan.RemoveObject(this);
        }

        public override void Draw()
        {
            _pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);
            TColor4 c = new TColor4(255, 128, 40, (byte)(255 - _uiCounter / 12));
            _pRender2D.SetColorMix(ref c);
            _pRender2D.DrawTexture(pTexSpark, ref pos, ref dim, _fAngle,
                E_EFFECT2D_FLAGS.EF_COLOR_MIX | E_EFFECT2D_FLAGS.EF_BLEND);
        }
    }
}
