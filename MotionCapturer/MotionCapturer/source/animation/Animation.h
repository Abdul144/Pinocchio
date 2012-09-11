#pragma once


#include <string>
#include <vector>

#include "../util/defines.h"

using namespace std;


class KeyFrame;

class Animation
{
	// 멤버 변수
private:
    string name;
    vector<KeyFrame*> keyFrames;		///< 키프레임 배열
    int totalFrameCount;


	// 멤버 변수
public:
    ///  생성자
    Animation();


    /// 키프레임 추가
    KeyFrame* addKeyFrame();

    KeyFrame* getKeyFrame(int index);


    // 접근
    GETTER_CONST_REF(string, Name, name)
    GETTER(int, KeyFrameCount, keyFrames.size())
    GETTER(int, TotalFrameCount, totalFrameCount)
};