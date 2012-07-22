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
        GeoData geoData = new GeoData();


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public GeoData GeoData { get { return geoData; } set { geoData = value; } }



        // test
        protected override void printChildTags(System.IO.StreamWriter writer, int depth)
        {
            // position
            string positionTagString = format("position", true,
                                                "x", geoData.position.x,
                                                "y", geoData.position.y,
                                                "z", geoData.position.z);
            writeDepthMark(writer, depth);
            writer.WriteLine(positionTagString);
            writer.Flush();


            // rotation
            string rotationTagString = format("rotation", true,
                                                "x", geoData.rotation.x,
                                                "y", geoData.rotation.y,
                                                "z", geoData.rotation.z);
            writeDepthMark(writer, depth);
            writer.WriteLine(rotationTagString);
            writer.Flush();


            // position
            string scaleTagString = format("scale", true,
                                                "x", geoData.scale.x,
                                                "y", geoData.scale.y,
                                                "z", geoData.scale.z);
            writeDepthMark(writer, depth);
            writer.WriteLine(scaleTagString);
            writer.Flush();
        }

        protected override string getOpenTagString()
        {
            return format(getTagName(), false, "name", name);
        }

        protected override string getTagName()
        {
            return "bone";
        }
    }
}
