using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Primitives3D;
using Microsoft.Xna.Framework.Graphics;
using Pinocchio.animation3D;

namespace Pinocchio.Model
{
    class Bone
    {
        public enum BoneType
        {
            None,
            Hip_Center,
            Spine,
            Shoulder_Center,
            
            Head,
            
            Shoulder_Right,
            Elbow_Right,
            Wrist_Right,
            Hand_Right,


            Shoulder_Left,
            Elbow_Left,
            Wrist_Left,
            Hand_Left,

            Hip_Right,
            Knee_Right,
            Ankle_Right,
            Foot_Right,

            Hip_Left,
            Knee_Left,
            Ankle_Left,
            Foot_Left
        }

        Bone parent;
        string name;
        BoneType type;
        CubePrimitive cube;
        Matrix transform;
        Matrix localTransform;  // 부모로 부터의 변환행렬

        public Bone(GraphicsDevice graphicsDevice, Bone parent, string name, BoneType type, Vector3 size, Vector3 center)
        {
            this.parent = parent;
            this.name = name;
            this.type = type;
            cube = new CubePrimitive(graphicsDevice, size, center);
            transform = Matrix.CreateScale(new Vector3(1f)) * Matrix.CreateFromQuaternion(Quaternion.Identity) * Matrix.CreateTranslation(Vector3.Zero);
            localTransform = transform;
        }


        /// <summary>
        /// 두 키프레임의 본 데이터를 alpha[0,1]의 비율로 블렌딩하여 변환행렬 업데이트
        /// </summary>
        /// <param name="curData"></param>
        /// <param name="nextData"></param>
        /// <param name="alpha"></param>
        public void update(BoneData curData, BoneData nextData, float alpha)
        {
            Matrix objectTransform;
            if (nextData != null)
            {   // 다음 데이터가 있다.. 두 데이터를 보간
                // 보간
                Vector3 position = curData.Position * (1f - alpha) + nextData.Position * alpha;
                Quaternion rotation = Quaternion.Slerp(curData.Rotation, nextData.Rotation, alpha);
                Vector3 scale = curData.Scale * (1f - alpha) + nextData.Scale * alpha;

                // 행렬 설정
                objectTransform = Matrix.CreateScale(scale) * Matrix.CreateFromQuaternion(rotation) * Matrix.CreateTranslation(position);
            }
            else
            {   // 다음 데이터가 없다.. 현재 데이터 적용
                // 행렬 설정
                objectTransform = Matrix.CreateScale(curData.Scale) * Matrix.CreateFromQuaternion(curData.Rotation) * Matrix.CreateTranslation(curData.Position);
            }

            // 변환행렬 설정
            if (parent != null)
                transform = objectTransform * localTransform * parent.Transform;
            else
                transform = objectTransform * localTransform;
        }

        public void draw(Matrix view, Matrix projection, Color color)
        {
            cube.Draw(transform, view, projection, color);
        }


        // 접근
        public BoneType Type { get { return type; } }
        public string Name { get { return name; } }
        public Matrix Transform { get { return transform; } set { transform = value; } }
        public Matrix LocalTransform { get { return localTransform; } set { localTransform = value; } }
    }
}
