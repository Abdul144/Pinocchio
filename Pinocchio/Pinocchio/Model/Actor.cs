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
            addBone(graphicsDevice, Bone.BoneType.None, "Hip_Center", Bone.BoneType.Hip_Center, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Center, "Spine", Bone.BoneType.Spine, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Spine, "Shoulder_Center", Bone.BoneType.Shoulder_Center, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, "Head", Bone.BoneType.Head, new Vector3(baseSize), Vector3.Zero);
            
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, "Shoulder_Right", Bone.BoneType.Shoulder_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Right, "Elbow_Right", Bone.BoneType.Elbow_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Elbow_Right, "Wrist_Right", Bone.BoneType.Wrist_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Wrist_Right, "Hand_Right", Bone.BoneType.Hand_Right, new Vector3(baseSize), Vector3.Zero);
            
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Center, "Shoulder_Left", Bone.BoneType.Shoulder_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Shoulder_Left, "Elbow_Left", Bone.BoneType.Elbow_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Elbow_Left, "Wrist_Left", Bone.BoneType.Wrist_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Wrist_Left, "Hand_Left", Bone.BoneType.Hand_Left, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Hip_Center, "Hip_Right", Bone.BoneType.Hip_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Right, "Knee_Right", Bone.BoneType.Knee_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Knee_Right, "Ankle_Right", Bone.BoneType.Ankle_Right, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Ankle_Right, "Foot_Right", Bone.BoneType.Foot_Right, new Vector3(baseSize), Vector3.Zero);

            addBone(graphicsDevice, Bone.BoneType.Hip_Center, "Hip_Left", Bone.BoneType.Hip_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Hip_Left, "Knee_Left", Bone.BoneType.Knee_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Knee_Left, "Ankle_Left", Bone.BoneType.Ankle_Left, new Vector3(baseSize), Vector3.Zero);
            addBone(graphicsDevice, Bone.BoneType.Ankle_Left, "Foot_Left", Bone.BoneType.Foot_Left, new Vector3(baseSize), Vector3.Zero);

        }

        private void addBone(GraphicsDevice graphicsDevice, Bone.BoneType parentType, string name, Bone.BoneType type, Vector3 size, Vector3 center)
        {
            // 본을 생성
            Bone bone = new Bone(graphicsDevice, boneMap[parentType], name, type, size, center);

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
