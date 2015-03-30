using System;
using System.Collections.Generic;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CExplo : CGameObject
    {
        private ITexture pTexExplo;

        public CExplo(IGameObjectManager pObjMan, IEngineCore pEngineCore, TPoint2 stPos, float fSize) :
            base(pObjMan, pEngineCore)
        {
            RenderLayer = 4;
            this._fSize = fSize;
            this._stPos = stPos;
            _fAngle = Rand.Next(360);

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.TexExplo, out pBaseObj);
            pTexExplo = (ITexture)pBaseObj;

            ISoundSample p_snd;
		    pResMan.GetResourceByName(Res.SndExplo, out pBaseObj);
            p_snd = (ISoundSample)pBaseObj;
		    p_snd.Play();
        }

        public override void Update()
        {
            base.Update();
            if (_uiCounter / 2 == Res.ExploAnimationFramesCount)
                _pObjMan.RemoveObject(this);
        }

        public override void Draw()
        {
            _pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);
            TPoint2 pos = new TPoint2(_stPos.x - _fSize / 2f, _stPos.y - _fSize / 2f);
            TPoint2 dim = new TPoint2(_fSize, _fSize);
            _pRender2D.DrawTextureSprite(pTexExplo, ref pos, ref dim, _uiCounter / 2, _fAngle, E_EFFECT2D_FLAGS.EF_BLEND);
        }
    }
}
