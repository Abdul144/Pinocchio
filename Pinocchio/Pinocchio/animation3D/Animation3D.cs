using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Pinocchio.animation3D
{
    class Animation3D : TestCode
    {
        string name = "";
        ArrayList tracks = new ArrayList();     // 트랙 배열
        ArrayList bones = new ArrayList();      // 본 참조 배열

        /// <summary>
        ///  생성자
        /// </summary>
        public Animation3D()
        {
        }


        /// <summary>
        /// 트랙 추가
        /// </summary>
        /// <returns></returns>
        public Track addTrack()
        {
            Track track = new Track();
            tracks.Add(track);

            return track;
        }


        // 접근
        public string Name { get { return name; } set { name = value; } }



        // test
        protected override void printChildTags(System.IO.StreamWriter writer, int depth)
        {
            // 트랙 출력
            for (int i = 0; i < tracks.Count; ++i)
            {
                Track track = (Track)tracks[i];
                track.print(writer, depth);
            }
        }

        protected override string getOpenTagString()
        {
            return format(getTagName(), false);
        }

        protected override string getTagName()
        {
            return "animation3D";
        }
    }
}
