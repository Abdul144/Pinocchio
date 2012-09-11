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

    vector<BoneData*> boneDatas;		///< 각 본의 Data



	// 멤버 함수
public:
    /// 생성자
    KeyFrame()
    {
    }


    /// 본정보 추가
    BoneData* addBoneData();

    BoneData* getBoneData(int index);

    void setBoneData(int index, BoneData *data);

        
    // 접근
    GETTER(int, Duration, duration)
    GETTER(int, BoneDataCount, boneDatas.size())
    GETTER(int, StartFrame, startFrame)

};