using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Primitives3D;
using Microsoft.Xna.Framework.Graphics;

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
            transform = new Matrix();
            localTransform = new Matrix();
        }

        public void update()
        {
        }

        public void draw()
        {
            cube.Draw(transform);
        }


        // 접근
        public BoneType Type { get { return type; } }
        public string Name { get { return name; } }
        public Matrix Transform { get { return transform; } set { transform = value; } }
        public Matrix LocalTransform { get { return localTransform; } set { localTransform = value; } }
    }
}
