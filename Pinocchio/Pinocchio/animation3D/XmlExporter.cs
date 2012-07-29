using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Pinocchio.util;
using System.Xml;
using Microsoft.Xna.Framework;

namespace Pinocchio.animation3D
{
    class XmlExporter : Singleton<XmlExporter>
    {
        /// <summary>
        /// xml 파일로 export
        /// </summary>
        /// <param name="path"></param>
        /// <param name="animation"></param>
        public void export(string path, Animation3D animation)
        {
            XmlTextWriter writer = new XmlTextWriter(path, Encoding.UTF8);

            // 들여쓰기 설정
            writer.Formatting = Formatting.Indented;
            writer.IndentChar = '\t';
            writer.Indentation = 1;

            // 쓰기 시작
            writer.WriteStartDocument();

            // 애니메이션 태그 쓰기
            exportTag_animation3D(writer, animation);

            // 닫기
            writer.Close();
        }

        public void exportTag_animation3D(XmlTextWriter writer, Animation3D animation)
        {
            // 열기
            writer.WriteStartElement("animation3D");

            // 자식 태그 쓰기
            for (int i = 0; i < animation.TrackCount; ++i)
            {
                exportTag_track(writer, animation.getTrack(i));
            }

            // 닫기
            writer.WriteEndElement();
        }

        public void exportTag_track(XmlTextWriter writer, Track track)
        {
            // 열기
            writer.WriteStartElement("track");

            // 애트리뷰트 쓰기
            writer.WriteAttributeString("name", track.Name);

            // 자식 태그 쓰기
            for (int i = 0; i < track.KeyFrameCount; ++i)
            {
                exportTag_keyFrame(writer, track.getKeyFrame(i));
            }

            // 닫기
            writer.WriteEndElement();
        }

        public void exportTag_keyFrame(XmlTextWriter writer, KeyFrame keyFrame)
        {
            // 열기
            writer.WriteStartElement("keyFrame");

            // 애트리뷰트 쓰기
            writer.WriteAttributeString("name", keyFrame.Name);
            writer.WriteAttributeString("duration", keyFrame.Duration.ToString());

            // 자식 태그 쓰기
            for (int i = 0; i < keyFrame.BoneDataCount; ++i)
            {
                BoneData boneData = keyFrame.getBoneData(i);
                if (boneData != null)
                    exportTag_bone(writer, boneData);
            }

            // 닫기
            writer.WriteEndElement();
        }

        public void exportTag_bone(XmlTextWriter writer, BoneData boneData)
        {
            // 열기
            writer.WriteStartElement("bone");

            // 애트리뷰트 쓰기
            writer.WriteAttributeString("name", boneData.Name);

            // 자식 태그 쓰기
            exportTag_vector3Type(writer, "position", boneData.Position);
            exportTag_quarternionType(writer, "rotation", boneData.Rotation);
            exportTag_vector3Type(writer, "scale", boneData.Scale);

            // 닫기
            writer.WriteEndElement();
        }

        public void exportTag_vector3Type(XmlTextWriter writer, string tagName, Vector3 vector)
        {
            // 열기
            writer.WriteStartElement(tagName);

            // 애트리뷰트 쓰기
            writer.WriteAttributeString("x", vector.X.ToString());
            writer.WriteAttributeString("y", vector.Y.ToString());
            writer.WriteAttributeString("z", vector.Z.ToString());

            // 닫기
            writer.WriteEndElement();
        }

        public void exportTag_quarternionType(XmlTextWriter writer, string tagName, Quaternion quaternion)
        {
            // 열기
            writer.WriteStartElement(tagName);

            // 애트리뷰트 쓰기
            writer.WriteAttributeString("x", quaternion.X.ToString());
            writer.WriteAttributeString("y", quaternion.Y.ToString());
            writer.WriteAttributeString("z", quaternion.Z.ToString());
            writer.WriteAttributeString("w", quaternion.W.ToString());

            // 닫기
            writer.WriteEndElement();
        }
    }
}
