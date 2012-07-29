using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using Pinocchio.Model;

namespace Pinocchio.animation3D
{
    class TrackInstance
    {
        Track track = null;

        int curKeyFrameIndex = 0;


        public TrackInstance(Track track)
        {
            this.track = track;
        }

        public void update(ArrayList boneList, int curFrame)
        {
            // 현재 키프레임 인덱스 갱신
            refreshCurKeyFrameIndex(curFrame);

            // 본 리스트 업데이트
            updateBones(boneList, curFrame);
        }


        /// <summary>
        /// 현재 키프레임 인덱스 갱신
        /// </summary>
        /// <param name="curFrame"></param>
        public void refreshCurKeyFrameIndex(int curFrame)
        {
            while (true)
            {
                KeyFrame keyFrame = track.getKeyFrame(curKeyFrameIndex);

                if (curFrame >= keyFrame.StartFrame + keyFrame.Duration)
                {   // 넘어감
                    ++curKeyFrameIndex;
                    if (curKeyFrameIndex >= track.KeyFrameCount)
                    {   // 끝남
                        curKeyFrameIndex = track.KeyFrameCount - 1;
                        break;
                    }
                }

                break;
            }
        }

        public void reset()
        {
            curKeyFrameIndex = 0;
        }

        // 본 리스트 업데이트
        public void updateBones(ArrayList boneList, int curFrame)
        {
            KeyFrame curKeyFrame = track.getKeyFrame(curKeyFrameIndex);
            KeyFrame nextKeyFrame = null;
            if (curKeyFrameIndex + 1 < track.KeyFrameCount)
            {   // 다음 키프레임이 있으면
                nextKeyFrame = track.getKeyFrame(curKeyFrameIndex + 1);
            }

            // 본을 순행하며
            for (int i = 0; i < curKeyFrame.BoneDataCount; ++i)
            {
                Bone bone = (Bone)boneList[i];                                  // 본
                BoneData curBoneData = (BoneData)curKeyFrame.getBoneData(i);    // 현재 키프레임 본 데이터
                BoneData nextBoneData = null;                                   // 다음 키프레임 본 데이터
                if (nextKeyFrame != null)
                {   // 다음 키프레임이 있다
                    nextBoneData = nextKeyFrame.getBoneData(i);
                }

                // 본 업데이트
                if (curKeyFrame.Duration > 1)
                    bone.update(curBoneData, nextBoneData, (float)(curFrame - curKeyFrame.StartFrame) / (float)(curKeyFrame.Duration - 1));
                else
                    bone.update(curBoneData, nextBoneData, 1.0f);
            }
        }
    }
}
