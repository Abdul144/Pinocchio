using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;
using System.Collections;

namespace Pinocchio.animation3D
{
    class KeyFrame : TestCode
    {
        string name = "";   // 이름
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
        public BoneData addBoneGeoData()
        {
            BoneData boneData = new BoneData();
            boneDatas.Add(boneData);

            return boneData;
        }


        public void nullifyBoneData(int index)
        {
            boneDatas[index] = null;
        }

        
        // 접근
        public string Name { get { return name; } set { name = value; } }
        public int Duration { get { return duration; } set { duration = value; } }



        // test
        protected override void printChildTags(System.IO.StreamWriter writer, int depth)
        {
            // 본 데이터 출력
            for (int i = 0; i < boneDatas.Count; ++i)
            {
                BoneData boneData = (BoneData)boneDatas[i];
                if (boneData != null)
                    boneData.print(writer, depth);
            }
        }

        protected override string getOpenTagString()
        {
            return format(getTagName(), "name", name, "duration", duration);
        }

        protected override string getTagName()
        {
            return "keyFrame";
        }
    }
}
