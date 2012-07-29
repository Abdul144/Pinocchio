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
        ArrayList tracks;

        int curFrame = 0;


        public Animation3DInstance(Animation3D animation)
        {
            if (animation == null)
                return;

            this.animation = animation;

            // 트랙개수 만큼 인스턴스 생성
            tracks = new ArrayList();
            for (int i = 0; i < animation.TrackCount; ++i)
            {
                // 트랙 인스턴스 생성
                tracks.Add(new TrackInstance(animation.getTrack(i)));
            }
        }

        public void update(ArrayList boneList, int deltaFrame)
        {
            if (animation == null)
                return;

            curFrame += deltaFrame;
            
            // 트랙 업데이트
            foreach (TrackInstance track in tracks)
            {
                track.update(boneList, curFrame);
            }

            if (curFrame >= animation.TotalFrameCount)
            {   // 끝남

                /// 반복시키기
                int newFrame = curFrame % animation.TotalFrameCount;

                // reset & update
                reset();
                update(boneList, newFrame);
                return;
            }
        }

        public void reset()
        {
            curFrame = 0;

            // 모든 트랙 리셋
            foreach (TrackInstance track in tracks)
            {
                track.reset();
            }
        }
    }

}
