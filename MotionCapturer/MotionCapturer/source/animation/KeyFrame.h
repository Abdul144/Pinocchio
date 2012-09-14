#pragma once

#include <vector>

#include "../util/defines.h"

using namespace std;


class BoneData;

class KeyFrame
{
	// 멤버 변수
private:
    int startFrame;
    int duration;					///< 재생할 길이

    vector<BoneData*> boneDatas;	///< 각 본의 Data.. free



	// 멤버 함수
public:
    /// 생성자
    KeyFrame();

	/// 소멸자
	~KeyFrame();

    /// 본정보 추가
    BoneData* addBoneData();

    BoneData* getBoneData(int index);

    void setBoneData(int index, BoneData *data);

        
    // 접근
    GETTER(int, BoneDataCount, boneDatas.size())
    GETTER_SETTER(int, Duration, duration)
    GETTER_SETTER(int, StartFrame, startFrame)

};