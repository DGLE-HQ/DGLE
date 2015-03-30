using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AsteroidsGame
{
    public static class Res
    {
        public static Random rand = new Random();

        // logical game resolution
        public const uint GameVpWidth = 800u;
        public const uint GameVpHeight = 600u;

        // names of some game resources
        public const string MeshPlanet = "MeshPlanet";
        public const string TexPlanet = "TexPlanet";
        public const string TexClouds = "TexClouds";
        public const string TexStars = "TexStars";
        public const string MeshShip = "MeshShip";
        public const string TexShip = "TexShip";
        public const string MeshAsteroid = "MeshAsteroid";
        public const string TexAsteroid = "TexAsteroid";
        public const string TexSpark = "TexSpark";
        public const string SndExplo = "SndExplo";
        public const string SndFire = "SndFire";
        public const string FntMain = "FntMain";

        // sprite config
        public const string TexExplo = "TexExplo";
        public const int ExploAnimationFramesCount = 16;
        public const int ExploAnimationFramesSize = 128;

        // some helper functions
        public static float ToRad(float val)
        {
            return (float)(val * Math.PI / 180f);
        }

        public static T Clamp<T>(T val, T min, T max) where T : IComparable<T>
        {
            return (val.CompareTo(min) < 0 ? min : (val.CompareTo(max) > 0 ? max : val));
        }
    }
}
