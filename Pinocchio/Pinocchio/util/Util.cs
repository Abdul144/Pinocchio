using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pinocchio.util
{
    struct Vector3
    {
        float x, y, z;

        public Vector3(float x = 0f, float y = 0f, float z = 0f)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    struct GeoData
    {
        Vector3 position;
        Vector3 rotation;
        Vector3 scale;

        public GeoData(int v = 0)
        {
            position = new Vector3();
            rotation = new Vector3();
            scale = new Vector3();
        }

    }

    class Util
    {
    }
}
