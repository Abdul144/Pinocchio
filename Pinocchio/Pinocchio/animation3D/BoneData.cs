using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;

namespace Pinocchio.animation3D
{
    class BoneData
    {
        string name = "";
        GeoData geoData = new GeoData();


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public GeoData GeoData { get { return geoData; } set { geoData = value; } }
    }
}
