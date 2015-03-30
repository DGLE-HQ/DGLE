using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CScorePopup : CGameObject
    {
        private uint _uiScore;
        private IBitmapFont _pFnt;

        public CScorePopup(IGameObjectManager pObjMan, IEngineCore pEngineCore, TPoint2 stPos, float fSize, uint uiScore) :
            base(pObjMan, pEngineCore)
        {
            _uiScore = uiScore;
            pObjMan.IncreaseGameScore(uiScore);

            RenderLayer = 5;
            _fSize = Res.Clamp(fSize / 100f, 0.25f, 5f);
            _stPos = stPos;

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.FntMain, out pBaseObj);
            _pFnt = (IBitmapFont)pBaseObj;
        }

        public override void Update()
        {
            base.Update();

            _fSize *= 1.025f;
            if (_uiCounter == 50)
                _pObjMan.RemoveObject(this);
        }

        public override void Draw()
        {
            string res = _uiScore.ToString();

            uint w, h;

            _pFnt.SetScale(_fSize);
            _pFnt.GetTextDimensions(res, out w, out h);
            _pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_NORMAL);
            TColor4 c = TColor4.ColorWhite((byte)(255 - _uiCounter * 5));
            _pFnt.Draw2D(_stPos.x - w / 2f, _stPos.y - h / 2f, res, ref c);
        }
    }
}
