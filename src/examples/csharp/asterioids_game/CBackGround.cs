using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DGLE;

namespace AsteroidsGame
{
    class CBackGround : CGameObject
    {
        private IMesh _pMeshPlanet;
        private ITexture _pTexSpace, _pTexEarth, _pTexClouds;

        //for draw planet
        private TPoint2 coordPl = new TPoint2(Res.GameVpWidth / 2f, Res.GameVpHeight / 2f);
        private TPoint3 dimPl = new TPoint3(350, 350, 350);
        private TPoint3 axisPl = new TPoint3(0f, 1f, 0f);

        //for draw atmosphere
        private TPoint2 coordAtm = new TPoint2(Res.GameVpWidth / 2f, Res.GameVpHeight / 2f);
        private TPoint3 dimAtm = new TPoint3(375, 375, 375);
        private TPoint3 axisAtm = new TPoint3(0.35f, -0.4f, 0.25f);
        TColor4 c = new TColor4(255, 255, 255, 150);

        public CBackGround(IGameObjectManager pObjMan, IEngineCore pEngineCore)
            : base(pObjMan, pEngineCore)
        {
            RenderLayer = -1;

            IResourceManager pResMan;
            IEngineSubSystem pSubSys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;
            pResMan.GetResourceByName(Res.MeshPlanet, out pBaseObj);
            _pMeshPlanet = (IMesh)pBaseObj;
            pResMan.GetResourceByName(Res.TexStars, out pBaseObj);
            _pTexSpace = (ITexture)pBaseObj;
            pResMan.GetResourceByName(Res.TexPlanet, out pBaseObj);
            _pTexEarth = (ITexture)pBaseObj;
            pResMan.GetResourceByName(Res.TexClouds, out pBaseObj);
            _pTexClouds = (ITexture)pBaseObj;
        }

        public override void Draw()
        {
            _pTexSpace.Draw2D(0, 0, Res.GameVpWidth, Res.GameVpHeight, 0f, 0);

            _pRender2D.DrawMesh(_pMeshPlanet, _pTexEarth, ref coordPl, ref dimPl, ref axisPl, _uiCounter / 4f,
                E_EFFECT2D_FLAGS.EF_DEFAULT, false);

            _pRender2D.SetColorMix(ref c);
            _pRender2D.DrawMesh(_pMeshPlanet, _pTexClouds, ref coordAtm, ref dimAtm, ref axisAtm, _uiCounter / 2f,
                E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLOR_MIX, false, 90f, true);
        }
    }
}
