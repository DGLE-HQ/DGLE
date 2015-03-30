using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CAsteroid : CGameObject
    {
        private IMesh _pMesh;
        private ITexture _pTex;

        public CAsteroid(IGameObjectManager pObjMan, IEngineCore pEngineCore, TPoint2 stPos, float fSize)
            : base(pObjMan, pEngineCore)
        {
            ObjType = EGameObjectType.GotAsteroid;

            _uiCounter = (uint)Rand.Next(500);
            RenderLayer = 2;
            _fSize = fSize;
            _fColScale = 0.8f;
            _stPos = stPos;
            _fAngle = Rand.Next(360);

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.MeshAsteroid, out pBaseObj);
            _pMesh = (IMesh)pBaseObj;
            pResMan.GetResourceByName(Res.TexAsteroid, out pBaseObj);
            _pTex = (ITexture)pBaseObj;
        }

        public override void Update()
        {
            base.Update();

            // move asteroid
            _stPos.x += (float)Math.Cos(Res.ToRad(_fAngle)) * Res.Clamp(100f / _fSize, 0.5f, 50f);
            _stPos.y += (float)Math.Sin(Res.ToRad(_fAngle)) * Res.Clamp(100f / _fSize, 0.5f, 50f);

            // asteroids fly beyond screen
            if (_stPos.x + _fSize / 2f < 0)
                _stPos.x = Res.GameVpWidth + _fSize / 2;
            if (_stPos.x - _fSize / 2f > Res.GameVpWidth)
                _stPos.x = -_fSize / 2f;
            if (_stPos.y + _fSize / 2f < 0)
                _stPos.y = Res.GameVpHeight + _fSize / 2;
            if (_stPos.y - _fSize / 2f > Res.GameVpHeight)
                _stPos.y = -_fSize / 2f;
        }

        public override void Draw()
        {
            TPoint3 dimASt = new TPoint3(_fSize, _fSize, _fSize);
            TPoint3 axisAst = new TPoint3(0.6f, 0.2f, 0.4f);

            _pRender2D.DrawMesh(_pMesh, _pTex, ref _stPos, ref dimASt, ref axisAst, _uiCounter * (250f / _fSize),
                E_EFFECT2D_FLAGS.EF_DEFAULT, false);
            base.Draw();
        }

        public override bool IsCollidable()
        {
            return true;
        }

        // collision with shot is fatal
        public override void OnCollision(CGameObject pObj)
        {
            if (pObj.ObjType != EGameObjectType.GotShot && pObj.ObjType != EGameObjectType.GotPlayer) return;
            // spawn little children asteroids
            if (_fSize > 25f)
            {
                uint count = (uint)(2 + Rand.Next(3));
                for (int i = 0; i < count; i++)
                {
                    _pObjMan.AddObject(new CAsteroid(_pObjMan, _pEngineCore,
                        new TPoint2(_stPos.x - _fSize / 4f + Rand.Next((int)(_fSize / 2f)),
                            _stPos.y - _fSize / 4f + Rand.Next((int)(_fSize / 2f))), _fSize * 0.5f));
                }
            }

            _pObjMan.AddObject(new CExplo(_pObjMan, _pEngineCore, _stPos, _fSize));
            _pObjMan.AddObject(new CScorePopup(_pObjMan, _pEngineCore, _stPos, _fSize, (uint)(_fSize * 5)));
            _pObjMan.RemoveObject(pObj);
            _pObjMan.RemoveObject(this);
        }
    }
}
