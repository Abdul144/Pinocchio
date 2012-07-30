using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Pinocchio.util;
using Microsoft.Xna.Framework;
using System.Windows;

namespace Pinocchio.animation3D
{
    class XMLLoaderException : System.Exception
    {
        public enum Type
        {
            Invalid_Tag,
            Invalid_Attribute,
            Has_No_Child
        }
        Type type;

        public XMLLoaderException(Type type)
        {
            this.type = type;
        }
        public XMLLoaderException(Type type, string message)
            : base(message)
        {
            this.type = type;
        }
        public Type GetExceptionType() { return type; }
    }

    class XmlImporter : Singleton<XmlImporter>
    {
        /// <summary>
        /// 애니메이션 로드
        /// </summary>
        /// <param name="path">xml파일 경로</param>
        /// <returns>읽어들인 animation</returns>
        public Animation3D import(string path)
        {
            Animation3D animation = null;

            XmlDocument doc = new XmlDocument();

            // 파싱
            doc.PreserveWhitespace = false;
            try
            {
                doc.Load(path);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

            // 로드
            animation = importTag_animation3D(doc);

            return animation;
        }

        /// <summary>
        /// animation3D 태그
        /// </summary>
        /// <param name="doc"></param>
        /// <returns></returns>
        private Animation3D importTag_animation3D(XmlDocument doc)
        {
            Animation3D animation = null;

            // 루트 노드 얻기
            XmlElement animation3DTag = doc.DocumentElement;

            // 이름 체크
            if (animation3DTag.Name != "animation3D")
                return null;

            animation = new Animation3D();


            /// attribute 로드
            XmlAttributeCollection attributes = animation3DTag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                // 이름
                if (att.Name == "name")
                    animation.Name = att.Value;
            }


            /// 트랙 읽기
            animation.TotalFrameCount = 0;
            XmlNodeList childList = animation3DTag.ChildNodes;
            for (int i = 0; i < childList.Count; ++i)
            {
                // track 태그 아니면 넘기기
                if (childList[i].Name != "track")
                    continue;

                // track 태그 로드
                Track track = animation.addTrack();
                bool trackTagResult = importTag_track(track, childList[i]);
                if (trackTagResult == false)
                    return null;

                // startFrame을 설정하고, totalCount 얻기
                int totalCount = 0;
                for (int keyFrameIndex = 0; keyFrameIndex < track.KeyFrameCount; ++keyFrameIndex)
                {
                    KeyFrame keyFrame = track.getKeyFrame(keyFrameIndex);
                    keyFrame.StartFrame = totalCount;
                    totalCount += keyFrame.Duration;
                }

                // 가장 큰 totalCount를 totalFrameCount로 설정
                if (animation.TotalFrameCount < totalCount)
                    animation.TotalFrameCount = totalCount;
            }

            return animation;
        }

        /// <summary>
        /// track 태그
        /// </summary>
        /// <param name="animation"></param>
        /// <param name="parentTag"></param>
        /// <returns></returns>
        private bool importTag_track(Track track, XmlNode trackTag)
        {
            /// attribute 로드
            XmlAttributeCollection attributes = trackTag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                // 이름
                if (att.Name == "name")
                    track.Name = att.Value;
            }


            /// 자식 태그 로드
            XmlNodeList childList = trackTag.ChildNodes;
            for (int i = 0; i < childList.Count; ++i)
            {
                // keyFrame 태그 이름 테스트
                if (childList[i].Name != "keyFrame")
                    continue;

                // keyFrame 태그 로드
                bool keyFrameTagResult = importTag_keyFrame(track.addKeyFrame(), childList[i]);
                if (keyFrameTagResult == false)
                    return false;

                // bone data가 비어있는 경우 이전 프레임에서 복사
                fillOmittedBoneData(track, i);
            }

            return true;
        }

        /// <summary>
        /// 본 데이터가 비어있는경우 이전 키 프레임에서 복사해서 채운다
        /// </summary>
        /// <param name="track"></param>
        /// <param name="keyFrameIndex"></param>
        private void fillOmittedBoneData(Track track, int keyFrameIndex)
        {
            // 키 프레임 얻기
            KeyFrame keyFrame = track.getKeyFrame(keyFrameIndex);

            // 본 데이터 순행
            for (int boneIndex=0; boneIndex<keyFrame.BoneDataCount; ++boneIndex)
            {
                BoneData boneData = keyFrame.getBoneData(boneIndex);
                if (boneData == null)
                {
                    if (keyFrameIndex > 0)
                    {   // 이전 본데이터를 넣는다.
                        keyFrame.setBoneData(boneIndex, track.getKeyFrame(keyFrameIndex - 1).getBoneData(boneIndex));
                    }
                    else
                    {   // 첫번째 키 프레임의 본이 null인 경우 기본 값으로 채워넣는다.
                        keyFrame.initializeBoneData(boneIndex);
                    }
                    continue;
                }

                BoneData preBoneData = getPreBoneData(track, keyFrameIndex, boneIndex);

                // 빠진것 복사해서 채우기
                if (float.IsNaN(boneData.Position.X))
                    boneData.Position = preBoneData.Position;
                if (float.IsNaN(boneData.Rotation.X))
                    boneData.Rotation = preBoneData.Rotation;
                if (float.IsNaN(boneData.Scale.X))
                    boneData.Scale = preBoneData.Scale;
            }

        }

        /// <summary>
        /// 이전 키 프레임의 본데이터 얻기
        /// </summary>
        /// <param name="track"></param>
        /// <param name="keyFrameIndex"></param>
        /// <param name="boneIndex"></param>
        /// <returns></returns>
        private BoneData getPreBoneData(Track track, int keyFrameIndex, int boneIndex)
        {
            for (int preKeyIndex = keyFrameIndex - 1; preKeyIndex >= 0; --preKeyIndex)
            {
                BoneData boneData = track.getKeyFrame(preKeyIndex).getBoneData(boneIndex);
                if (boneData != null)
                    return boneData;
            }

            return new BoneData();
        }


        private bool importTag_keyFrame(KeyFrame keyFrame, XmlNode keyFrameTag)
        {
            /// attribute 로드
            XmlAttributeCollection attributes = keyFrameTag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                // 이름
                if (att.Name == "name")
                    keyFrame.Name = att.Value;

                // 길이
                if (att.Name == "duration")
                    keyFrame.Duration = int.Parse(att.Value);
            }


            /// 자식 태그 로드
            XmlNodeList childList = keyFrameTag.ChildNodes;
            for (int i = 0; i < childList.Count; ++i)
            {
                // bone 태그 이름 테스트
                if (childList[i].Name != "bone")
                    continue;


                /// bone 태그 로드
                try
                {
                    BoneData boneData = keyFrame.addBoneData();
                    bool boneTagResult = importTag_bone(boneData, childList[i]);
                    if (boneTagResult == false)
                        return false;

                }
                catch(XMLLoaderException e)
                {
                    switch (e.GetExceptionType())
                    {
                        case XMLLoaderException.Type.Invalid_Tag:
                            // 올바르지 않는 태그
                            Console.WriteLine(e.ToString());
                            return false;

                        case XMLLoaderException.Type.Has_No_Child:
                            // 자식 태그가 없는 경우
                            keyFrame.nullifyBoneData(i);
                            break;
                    }
                }

            }

            return true;
        }

        private bool importTag_bone(BoneData boneData, XmlNode boneTag)
        {
            /// attribute 로드
            XmlAttributeCollection attributes = boneTag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                // 이름
                if (att.Name == "name")
                    boneData.Name = att.Value;
            }


            /// 자식 태그 로드
            XmlNodeList childList = boneTag.ChildNodes;
            int readChildCount = 0;                     // 읽혀진 자식 태그 개수
            boneData.nullify();
            for (int i = 0; i < childList.Count; ++i, ++readChildCount)
            {
                XmlNode node = childList[i];

                // 태그 종류 파악
                if (node.Name == "position")
                {   // position 태그
                    Vector3 vector = new Vector3();
                    importTag_vector3Type(ref vector, node);
                    boneData.Position = vector;
                }
                else if (node.Name == "rotation")
                {   // rotation 태그
                    Quaternion quaternion = new Quaternion();
                    importTag_quaternionType(ref quaternion, node);
                    boneData.Rotation = quaternion;
                }
                else if (node.Name == "scale")
                {   // scale 태그
                    Vector3 vector = new Vector3();
                    importTag_vector3Type(ref vector, node);
                    boneData.Scale = vector;
                }
                else
                {   // 알수 없는 태그
                    throw new XMLLoaderException(XMLLoaderException.Type.Invalid_Tag,
                                                node.Name + @" - 알수없는 태그.. bone 태그 하위에는 position/rotation/scale이 와야 함");
                }

            }

            if (readChildCount == 0)
            {   // 올바른 자식 태그가 없다
                throw new XMLLoaderException(XMLLoaderException.Type.Has_No_Child);
            }

            return true;
        }

        private bool importTag_vector3Type(ref Vector3 vector, XmlNode tag)
        {
            /// attribute 로드
            XmlAttributeCollection attributes = tag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                if (att.Name == "x")
                    vector.X = float.Parse(att.Value);
                else if (att.Name == "y")
                    vector.Y = float.Parse(att.Value);
                else if (att.Name == "z")
                    vector.Z = float.Parse(att.Value);
                else
                {   // 알수 없는 attribute
                    throw new XMLLoaderException(XMLLoaderException.Type.Invalid_Attribute,
                                                att.Name + @" - invalid attribute");
                }
            }

            return true;
        }

        private bool importTag_quaternionType(ref Quaternion quaternion, XmlNode tag)
        {
            float x = 1, y = 0, z = 0, angle = 0;

            /// attribute 로드
            XmlAttributeCollection attributes = tag.Attributes;
            for (int i = 0; i < attributes.Count; ++i)
            {
                XmlAttribute att = attributes[i];

                if (att.Name == "x")
                    x = float.Parse(att.Value);
                else if (att.Name == "y")
                    y = float.Parse(att.Value);
                else if (att.Name == "z")
                    z = float.Parse(att.Value);
                else if (att.Name == "angle")
                    angle = float.Parse(att.Value);
                else
                {   // 알수 없는 attribute
                    throw new XMLLoaderException(XMLLoaderException.Type.Invalid_Attribute,
                                                att.Name + @" - invalid attribute");
                }
            }

            // 축을 정규화
            Vector3 axis = new Vector3(x, y, z);
            axis.Normalize();

            // 각도를 라디안으로 변경
            float theta = angle * (float)Math.PI / 360f;

            // 쿼터니언 구성
            quaternion = Quaternion.CreateFromAxisAngle(axis, theta);

            return true;
        }

    }

}
