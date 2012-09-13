
#include "stdafx.h"
#include "AnimationInstance.h"

#include "../model/Bone.h"
#include "Animation.h"
#include "BoneData.h"
#include "KeyFrame.h"


AnimationInstance::AnimationInstance() : animation(null)
{
}

void AnimationInstance::update(const vector<Bone*> &bones, int deltaFrame)
{
    if (animation == null)
        return;

    curFrame += deltaFrame;
            
    // 현재 키프레임 인덱스 갱신
    refreshCurKeyFrameIndex(curFrame);

    // 본 리스트 업데이트
    updateBones(bones, curFrame);


    if (curFrame >= animation->getTotalFrameCount())
    {   // 끝남

        /// 반복시키기
        int newFrame = curFrame % animation->getTotalFrameCount();

        // reset & update
        reset();
        update(bones, newFrame);
        return;
    }
}

void AnimationInstance::reset()
{
    curFrame = 0;
	curKeyFrameIndex = 0;
}

/// <summary>
/// 현재 키프레임 인덱스 갱신
/// </summary>
/// <param name="curFrame"></param>
void AnimationInstance::refreshCurKeyFrameIndex(int curFrame)
{
    while (true)
    {
		int keyFrameCount = animation->getKeyFrameCount();
		KeyFrame *keyFrame = animation->getKeyFrame(curKeyFrameIndex);

        if (curFrame >= keyFrame->getStartFrame() + keyFrame->getDuration())
        {   // 넘어감
            ++curKeyFrameIndex;
            if (curKeyFrameIndex >= keyFrameCount)
            {   // 끝남
                curKeyFrameIndex = keyFrameCount - 1;
                break;
            }
        }

        break;
    }
}

void AnimationInstance::reset()
{
	curFrame = 0;
    curKeyFrameIndex = 0;
}

// 본 리스트 업데이트
void AnimationInstance::updateBones(const vector<Bone*> &boneList, int curFrame)
{
	int keyFrameCount = animation->getKeyFrameCount();
    KeyFrame *curKeyFrame = animation->getKeyFrame(curKeyFrameIndex);
    KeyFrame *nextKeyFrame = null;
    if (curKeyFrameIndex + 1 < keyFrameCount)
    {   // 다음 키프레임이 있으면
        nextKeyFrame = animation->getKeyFrame(curKeyFrameIndex + 1);
    }

    // 본을 순행하며
    for (int i = 0; i < curKeyFrame->getBoneDataCount(); ++i)
    {
        Bone *bone = boneList[i];                                  // 본
        BoneData *curBoneData = curKeyFrame->getBoneData(i);    // 현재 키프레임 본 데이터
        BoneData *nextBoneData = null;                                   // 다음 키프레임 본 데이터
        if (nextKeyFrame != null)
        {   // 다음 키프레임이 있다
            nextBoneData = nextKeyFrame->getBoneData(i);
        }

        // 본 업데이트
        if (curKeyFrame->getDuration() > 1)
            bone->update(curBoneData, nextBoneData, (float)(curFrame - curKeyFrame->getStartFrame()) / (float)(curKeyFrame->getDuration() - 1));
        else
            bone->update(curBoneData, nextBoneData, 1.0f);
    }
}

// 애니메이션 설정
void AnimationInstance::setAnimation(Animation *animation)
{
	this->animation = animation;

	curFrame = 0;
	curKeyFrameIndex = 0;
}

// 키 프레임 개수 얻기
void AnimationInstance::getKeyFrameCount()
{
	animation->getKeyFrameCount();
}