using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;

namespace Pinocchio.animation3D
{
    class BoneData : TestCode
    {
        string name = "";
        GeoData geoData;


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public GeoData GeoData { get { return geoData; } }



        // test
        protected override void printChildTags(System.IO.StreamWriter writer, int depth)
        {
        }

        protected override string getOpenTagString()
        {
            return format(getTagName(), "name", name);
        }

        protected override string getTagName()
        {
            return "bone";
        }
    }
}
