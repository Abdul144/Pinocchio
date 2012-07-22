using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pinocchio.util
{
    class Vector3
    {
        public float x, y, z;

        public Vector3(float x = 0f, float y = 0f, float z = 0f)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3(Vector3 vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
        }
    }

    class GeoData
    {
        public Vector3 position;
        public Vector3 rotation;
        public Vector3 scale;

        public GeoData()
        {
            position = new Vector3();
            rotation = new Vector3();
            scale = new Vector3();
        }

        public void nullify()
        {
            position = null;
            rotation = null;
            scale = null;
        }
    }

    class Util
    {
    }
}
