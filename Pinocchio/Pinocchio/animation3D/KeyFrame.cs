using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;
using System.Collections;

namespace Pinocchio.animation3D
{
    class KeyFrame
    {
        string name = "";   // 이름
        int startFrame = 0;
        int duration;       // 재생할 길이

        ArrayList boneDatas = new ArrayList(); // 각 본의 Data


        /// <summary>
        /// 생성자
        /// </summary>
        public KeyFrame()
        {
        }


        /// <summary>
        /// 본정보 추가
        /// </summary>
        /// <returns></returns>
        public BoneData addBoneData()
        {
            BoneData boneData = new BoneData();
            boneDatas.Add(boneData);

            return boneData;
        }


        public void nullifyBoneData(int index)
        {
            boneDatas[index] = null;
        }

        public BoneData getBoneData(int index)
        {
            return (BoneData)boneDatas[index];
        }

        
        // 접근
        public string Name { get { return name; } set { name = value; } }
        public int Duration { get { return duration; } set { duration = value; } }
        public int BoneDataCount { get { return boneDatas.Count; } }
        public int StartFrame { get { return startFrame; } set { startFrame = value; } }

    }
}
