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
    vector<KeyFrame*> keyFrames;		///< 키프레임 배열.. free
    int totalFrameCount;


	// 멤버 변수
public:
    ///  생성자
    Animation();

	/// 소멸자
	~Animation();

    /// 키프레임 추가
    KeyFrame* addKeyFrame(int index = -1);

    KeyFrame* getKeyFrame(int index);

	/// 갱신.. totalFrameCount나, 각 keyFrame의 startFrame을 갱신
	void refresh();


    // 접근
    GETTER_CONST_REF(string, Name, name)
    GETTER(int, KeyFrameCount, keyFrames.size())
    GETTER(int, TotalFrameCount, totalFrameCount)
};