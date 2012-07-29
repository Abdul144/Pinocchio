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
            // 현재 키프레임 얻어옴
            KeyFrame keyFrame;
            while (true)
            {
                keyFrame = track.getKeyFrame(curKeyFrameIndex);

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

            // 본 리스트 업데이트
            updateBones(keyFrame, boneList, curFrame);
        }

        public void reset()
        {
            curKeyFrameIndex = 0;
        }

        // 본 리스트 업데이트
        public void updateBones(KeyFrame keyFrame, ArrayList boneList, int curFrame)
        {
            // 본을 순행하며
            for (int i = 0; i < boneList.Count; ++i)
            {
                Bone bone = (Bone)boneList[i];                             // 본
                BoneData curBoneData = (BoneData)keyFrame.getBoneData(i);  // 현재 키프레임 본 데이터
                BoneData nextBoneData = null;                              // 다음 키프레임 본 데이터
                if (i+1 < boneList.Count)
                {   // 다음 키프레임이 있으면
                    nextBoneData = (BoneData)keyFrame.getBoneData(i);
                }

                // 본 업데이트
                if (keyFrame.Duration > 1)
                    bone.update(curBoneData, nextBoneData, (float)(curFrame - keyFrame.StartFrame) / (float)(keyFrame.Duration - 1));
                else
                    bone.update(curBoneData, nextBoneData, 1.0f);
            }
        }
    }
}
