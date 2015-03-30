using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsteroidsGame
{
    interface IGameObjectManager
    {
        bool IsDebugMode();
        void IncreaseGameScore(uint uiByValue);
        void AddObject(CGameObject pObj);
        void RemoveObject(CGameObject pObj);
    }
}
