#pragma once

#include <vector>

#include "../util/defines.h"

using namespace std;

class Animation;
class Bone;

class AnimationInstance
{
	// 멤버 변수
private:
    Animation *animation;
	int keyFrameCount;

    int curFrame;
	int curKeyFrameIndex;


	// 멤버 함수
public:
    AnimationInstance(Animation *animation);

    void update(const vector<Bone*> &bones, int deltaFrame);

    void reset();

    /// <summary>
    /// 현재 키프레임 인덱스 갱신
    /// </summary>
    /// <param name="curFrame"></param>
    void refreshCurKeyFrameIndex(int curFrame);

    void reset();

    // 본 리스트 업데이트
    void updateBones(const vector<Bone*> &boneList, int curFrame);


	// 접근
	GETTER(int, KeyFrameCount, keyFrameCount)
}