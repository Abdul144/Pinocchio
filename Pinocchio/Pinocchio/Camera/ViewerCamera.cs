using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace Pinocchio.Camera
{
    class ViewerCamera
    {
        Vector3 center = new Vector3(0);        // 카메라가 보는 중앙 위치
        Vector3 viewerAngle = new Vector3(0);   // 중점으로 부터의 각도
        float distance = 500f;                  // 중앙 위치로부터 떨어진 거리

        Matrix viewMatrix;              // 뷰 행렬
        private bool invalid = true;    // 변경되면 true


        public void invalidate()
        {
            invalid = true;
        }


        // 접근
        public Matrix ViewMatrix
        {
            get
            {
                if (invalid)
                {   // 다시 계산해야 함
                    Vector3 position = new Vector3(0, 0, -1);
                    Vector3 upVector = new Vector3(0, 1, 0);

                    Matrix rotationMatrix = Matrix.CreateFromYawPitchRoll(viewerAngle.Y, viewerAngle.X, viewerAngle.Z);

                    
                    position = Vector3.Transform(position, Matrix.CreateScale(distance) * rotationMatrix * Matrix.CreateTranslation(center));
                    upVector = Vector3.Transform(upVector, rotationMatrix);

                    viewMatrix = Matrix.CreateLookAt(position, center, upVector);
                }

                return viewMatrix;
            }
        }

        public Vector3 Center
        {
            get
            {
                return center;
            }
            set
            {
                center = value;
                invalidate();
            }
        }

        public Vector3 ViewerAngle
        {
            get
            {
                return viewerAngle;
            }
            set
            {
                viewerAngle = value;
                invalidate();
            }
        }

        public float Distance
        {
            get
            {
                return distance;
            }
            set
            {
                distance = value;
                invalidate();
            }
        }
    }
}
