
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
            
    // ���� Ű������ �ε��� ����
    refreshCurKeyFrameIndex(curFrame);

    // �� ����Ʈ ������Ʈ
    updateBones(bones, curFrame);


    if (curFrame >= animation->getTotalFrameCount())
    {   // ����

        /// �ݺ���Ű��
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
/// ���� Ű������ �ε��� ����
/// </summary>
/// <param name="curFrame"></param>
void AnimationInstance::refreshCurKeyFrameIndex(int curFrame)
{
    while (true)
    {
		KeyFrame *keyFrame = animation->getKeyFrame(curKeyFrameIndex);

        if (curFrame >= keyFrame->getStartFrame() + keyFrame->getDuration())
        {   // �Ѿ
            ++curKeyFrameIndex;
            if (curKeyFrameIndex >= keyFrameCount)
            {   // ����
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

// �� ����Ʈ ������Ʈ
void AnimationInstance::updateBones(const vector<Bone*> &boneList, int curFrame)
{
    KeyFrame *curKeyFrame = animation->getKeyFrame(curKeyFrameIndex);
    KeyFrame *nextKeyFrame = null;
    if (curKeyFrameIndex + 1 < keyFrameCount)
    {   // ���� Ű�������� ������
        nextKeyFrame = animation->getKeyFrame(curKeyFrameIndex + 1);
    }

    // ���� �����ϸ�
    for (int i = 0; i < curKeyFrame->getBoneDataCount(); ++i)
    {
        Bone *bone = boneList[i];                                  // ��
        BoneData *curBoneData = curKeyFrame->getBoneData(i);    // ���� Ű������ �� ������
        BoneData *nextBoneData = null;                                   // ���� Ű������ �� ������
        if (nextKeyFrame != null)
        {   // ���� Ű�������� �ִ�
            nextBoneData = nextKeyFrame->getBoneData(i);
        }

        // �� ������Ʈ
        if (curKeyFrame->getDuration() > 1)
            bone->update(curBoneData, nextBoneData, (float)(curFrame - curKeyFrame->getStartFrame()) / (float)(curKeyFrame->getDuration() - 1));
        else
            bone->update(curBoneData, nextBoneData, 1.0f);
    }
}