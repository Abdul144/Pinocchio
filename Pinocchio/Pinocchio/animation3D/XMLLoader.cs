using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Pinocchio.util;

namespace Pinocchio.animation3D
{
    class XMLLoaderException : System.Exception
    {
        public enum Type
        {
            Invalid_Tag,
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
        public Type GetType() { return type; }
    }

    class XMLLoader : Singleton<XMLLoader>
    {
        /// <summary>
        /// 애니메이션 로드
        /// </summary>
        /// <param name="path">xml파일 경로</param>
        /// <returns>읽어들인 animation</returns>
        public Animation3D load(string path)
        {
            Animation3D animation = null;

            XmlDocument doc = new XmlDocument();

            // 파싱
            doc.PreserveWhitespace = false;
            doc.Load(path);

            // 로드
            animation = loadTag_animation3D(doc);

            return animation;
        }

        /// <summary>
        /// animation3D 태그
        /// </summary>
        /// <param name="doc"></param>
        /// <returns></returns>
        private Animation3D loadTag_animation3D(XmlDocument doc)
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
            XmlNodeList childList = animation3DTag.ChildNodes;
            for (int i = 0; i < childList.Count; ++i)
            {
                // track 태그 아니면 넘기기
                if (childList[i].Name != "track")
                    continue;

                // track 태그 로드
                bool trackTagResult = loadTag_track(animation.addTrack(), childList[i]);
                if (trackTagResult == false)
                    return null;
            }

            return animation;
        }

        /// <summary>
        /// track 태그
        /// </summary>
        /// <param name="animation"></param>
        /// <param name="parentTag"></param>
        /// <returns></returns>
        private bool loadTag_track(Track track, XmlNode trackTag)
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
                bool keyFrameTagResult = loadTag_keyFrame(track.addKeyFrame(), childList[i]);
                if (keyFrameTagResult == false)
                    return false;
            }

            return true;
        }


        private bool loadTag_keyFrame(KeyFrame keyFrame, XmlNode keyFrameTag)
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
                    bool boneTagResult = loadTag_bone(keyFrame.addBoneGeoData(), childList[i]);
                    if (boneTagResult == false)
                        return false;
                }
                catch(XMLLoaderException e)
                {
                    switch (e.GetType())
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

        private bool loadTag_bone(BoneData boneData, XmlNode boneTag)
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
            for (int i = 0; i < childList.Count; ++i, ++readChildCount)
            {
                XmlNode node = childList[i];

                // 태그 종류 파악
                if (node.Name == "position")
                {   // position 태그
                }
                else if (node.Name == "rotation")
                {   // rotation 태그
                }
                else if (node.Name == "scale")
                {   // scale 태그
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

    }

}
