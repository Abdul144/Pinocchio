using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Pinocchio.animation3D
{
    class Animation3D
    {
        string name = "";
        ArrayList tracks = new ArrayList();     // 트랙 배열
        ArrayList bones = new ArrayList();      // 본 참조 배열
        int totalFrameCount = 0;

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

        public Track getTrack(int index)
        {
            return (Track)tracks[index];
        }


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public int TrackCount { get { return tracks.Count; } }
        public int BoneCount { get { return bones.Count; } }
        public int TotalFrameCount { get { return totalFrameCount; } set { totalFrameCount = value; } }
    }
}
