using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;
using Microsoft.Xna.Framework;

namespace Pinocchio.animation3D
{
    class BoneData
    {
        string name;
        Vector3 position = new Vector3();
        Quaternion rotation = new Quaternion();
        Vector3 scale = new Vector3(1, 1, 1);

        public void nullify()
        {
            position = new Vector3(float.NaN);
            rotation = new Quaternion(float.NaN, float.NaN, float.NaN, float.NaN);
            scale = new Vector3(float.NaN);
        }


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public Vector3 Position { get { return position; } set { position = value; } }
        public Quaternion Rotation { get { return rotation; } set { rotation = value; } }
        public Vector3 Scale { get { return scale; } set { scale = value; } }
    }
}
