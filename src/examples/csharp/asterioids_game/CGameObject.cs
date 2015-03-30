using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CGameObject
    {
        public enum EGameObjectType
        {
            GotUnknown,
            GotPlayer,
            GotAsteroid,
            GotShot
        }

        public Random Rand = Res.rand;

        public EGameObjectType ObjType { get; protected set; }
        public int RenderLayer { get; protected set; }

        protected IGameObjectManager _pObjMan;
        protected IEngineCore _pEngineCore;
        protected IRender2D _pRender2D;

        protected TPoint2 _stPos;

        protected float _fSize, _fColScale, _fAngle;
        protected uint _uiCounter;

        public CGameObject(IGameObjectManager pObjMan, IEngineCore pEngineCore)
        {
            _pObjMan = pObjMan;
            _pEngineCore = pEngineCore;
            ObjType = EGameObjectType.GotUnknown;

            _fSize = 10f;
            _fColScale = 1f;
            _fAngle = 0f;
            _uiCounter = 0;
            RenderLayer = 0;

            IRender pRender;

            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out pSubSys);
            pRender = (IRender)pSubSys;
            pRender.GetRender2D(out _pRender2D);
        }

        public virtual void Update()
        {
            ++_uiCounter;
        }

        public virtual void Draw()
        {
            if (!_pObjMan.IsDebugMode()) return;
            TColor4 c = TColor4.ColorWhite();
            _pRender2D.DrawCircle(ref _stPos, (uint)(_fSize / 2f), 32, ref c);
            c = TColor4.ColorRed();
            _pRender2D.DrawCircle(ref _stPos, (uint)(_fSize * _fColScale / 2f), 32, ref c);
        }

        public virtual bool CollisionCheck(CGameObject pObjWith)
        {
            return _stPos.DistTo(pObjWith._stPos) < _fSize / 2f * _fColScale + pObjWith._fSize / 2f * pObjWith._fColScale;
        }

        public virtual void OnCollision(CGameObject pObj)
        {

        }

        public virtual bool IsCollidable()
        {
            return false;
        }
    }
}
