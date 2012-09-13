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

    int curFrame;
	int curKeyFrameIndex;


	// 멤버 함수
public:
    AnimationInstance();

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

	// 애니메이션 설정
	void setAnimation(Animation *animation);

	// 키 프레임 개수 얻기
	void getKeyFrameCount();


	// 접근
	GETTER(Animation*, Animation, animation);
};