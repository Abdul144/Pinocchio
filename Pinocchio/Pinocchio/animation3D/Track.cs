using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Pinocchio.animation3D
{
    class Track
    {
        string name = "";                       // 트랙 이름
        ArrayList keyFrames = new ArrayList();  // 키 프레임 배열


        /// <summary>
        /// 생성자
        /// </summary>
        public Track()
        {
        }


        /// <summary>
        /// 키프레임 추가
        /// </summary>
        /// <returns></returns>
        public KeyFrame addKeyFrame()
        {
            KeyFrame keyFrame = new KeyFrame();
            keyFrames.Add(keyFrame);

            return keyFrame;
        }


        public KeyFrame getKeyFrame(int index)
        {
            return (KeyFrame)keyFrames[index];
        }


        // 접근
        public string Name { get { return name; } set { name = value; } }
        public int KeyFrameCount { get { return keyFrames.Count; } }

    }
}
