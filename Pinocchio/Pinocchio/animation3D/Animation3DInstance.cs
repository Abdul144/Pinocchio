using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Pinocchio.animation3D
{
    class Animation3DInstance
    {
        Animation3D animation = null;
        ArrayList tracks = new ArrayList();

        int curFrame = 0;


        public Animation3DInstance(Animation3D animation)
        {
            this.animation = animation;

            // 트랙개수 만큼 인스턴스 생성
            for (int i = 0; i < animation.TrackCount; ++i)
            {
                // 트랙 인스턴스 생성
                tracks.Add(new TrackInstance(animation.getTrack(i)));
            }
        }

        public void update(int deltaFrame)
        {
            curFrame = (curFrame + deltaFrame) % animation.TotalFrameCount;
        }
    }

    class TrackInstance
    {
        Track track = null;

        int curKeyFrameIndex = 0;


        public TrackInstance(Track track)
        {
            this.track = track;
        }
    }
}
