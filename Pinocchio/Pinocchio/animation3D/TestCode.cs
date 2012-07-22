using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Pinocchio.animation3D
{
    abstract class TestCode
    {
        public void print(string filePath, int depth)
        {
            // 파일을 연다
            StreamWriter writer = new StreamWriter(filePath);

            print(writer, depth);

            writer.Close();
        }

        public void print(StreamWriter writer, int depth)
        {
            // 열린 태그 출력
            string openTagString = getOpenTagString();
            writeDepthMark(writer, depth);
            writer.WriteLine(openTagString);
            writer.Flush();

            // 자식의 태그 출력
            printChildTags(writer, depth+1);

            // 닫힌 태그 출력
            string closedTagString = "</" + getTagName() + ">";
            writeDepthMark(writer, depth);
            writer.WriteLine(closedTagString);
            writer.Flush();
        }

        protected void writeDepthMark(StreamWriter writer, int depth, string mark = "\t")
        {
            for (int i = 0; i < depth; ++i)
            {
                writer.Write(mark);
            }
        }

        protected string format(string tagName, bool hasNoChild, params object[] list)
        {
            string content;

            // 태그를 만든다
            content = "<" + tagName;
            for (int i = 0; i+1 < list.Length; i += 2)
            {
                content += " " + list[i].ToString() + "=\"" + list[i + 1].ToString() + "\"";
            }
            if (hasNoChild == false)
                content += ">";
            else
                content += "/>";

            return content;
        }

        protected abstract void printChildTags(StreamWriter writer, int depth);

        protected abstract string getOpenTagString();

        protected abstract string getTagName();
    }
}
