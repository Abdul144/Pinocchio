using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework;

namespace Pinocchio.Model
{
    class Actor
    {
        ArrayList boneList;
        Dictionary<Bone.BoneType, Bone> boneMap;
        Vector3 position = new Vector3();
        Vector3 rotation = new Vector3();
        Vector3 scale = new Vector3(1.0f);

        public Actor(GraphicsDevice graphicsDevice)
        {
            // 본리스트 초기화
            initializeBones(graphicsDevice);
            
        }

        private void initializeBones(GraphicsDevice graphicsDevice)
        {
            // 초기화
            const float baseSize = 30.0f;
            boneList = new ArrayList();
            boneMap = new Dictionary<Bone.BoneType,Bone>();

            /// 본 추가
            addBone(graphicsDevice, Bone.BoneType.None, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Hip_Center", Bone.BoneType.Hip_Center, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Spine", Bone.BoneType.Spine, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Spine, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Shoulder_Center", Bone.BoneType.Shoulder_Center, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Head", Bone.BoneType.Head, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Shoulder_Right", Bone.BoneType.Shoulder_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Elbow_Right", Bone.BoneType.Elbow_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Elbow_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Wrist_Right", Bone.BoneType.Wrist_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Wrist_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Hand_Right", Bone.BoneType.Hand_Right, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Shoulder_Left", Bone.BoneType.Shoulder_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Elbow_Left", Bone.BoneType.Elbow_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Elbow_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Wrist_Left", Bone.BoneType.Wrist_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Wrist_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Hand_Left", Bone.BoneType.Hand_Left, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Hip_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Hip_Right", Bone.BoneType.Hip_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Knee_Right", Bone.BoneType.Knee_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Knee_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Ankle_Right", Bone.BoneType.Ankle_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Ankle_Right, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Foot_Right", Bone.BoneType.Foot_Right, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Hip_Center, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Hip_Left", Bone.BoneType.Hip_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Knee_Left", Bone.BoneType.Knee_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Knee_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Ankle_Left", Bone.BoneType.Ankle_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Ankle_Left, new Vector3(), new Quaternion(), new Vector3(1.0f),
                "Foot_Left", Bone.BoneType.Foot_Left, new Vector3(baseSize), Vector3.Zero);

        }

        private void addBone(GraphicsDevice graphicsDevice, Bone.BoneType parentType, Vector3 localPosition, Quaternion localRotation, Vector3 localScale,
            string name, Bone.BoneType type, Vector3 size, Vector3 center)
        {
            // 본을 생성
            Bone bone = new Bone(graphicsDevice, boneMap[parentType], name, type, size, center);

            // 본의 부모상대 변환행렬 설정
            Matrix localTransform = Matrix.CreateScale(localScale) * Matrix.CreateFromQuaternion(localRotation) * Matrix.CreateTranslation(localPosition);
            bone.LocalTransform = localTransform;

            // 리스트와 맵에 추가
            boneList.Add(bone);
            boneMap[bone.Type] = bone;
        }

        public void update(float deltaTime)
        {
            // 각 본들을 업데이트
            foreach (Bone bone in boneList)
            {
            }
        }

        public void draw()
        {
            // 각 본들을 그린다.
            foreach (Bone bone in boneList)
                bone.draw();
        }


        // 접근
        public Vector3 Position { get { return position; } set { position = value; } }
        public Vector3 Rotation { get { return rotation; } set { rotation = value; } }
        public Vector3 Scale { get { return scale; } set { scale = value; } }
    }

}
