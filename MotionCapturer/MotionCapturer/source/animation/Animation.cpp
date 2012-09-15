
#include "stdafx.h"
#include "Animation.h"

#include "KeyFrame.h"


///  ������
Animation::Animation() : totalFrameCount(0)
{
}

/// �Ҹ���
Animation::~Animation()
{
	for (uint i=0; i<keyFrames.size(); ++i)
	{
		DELETE(keyFrames[i])
	}
}

/// Ű������ �߰�
KeyFrame* Animation::addKeyFrame(int index)
{
    KeyFrame *keyFrame = new KeyFrame;

	if (index < 0 || index >= keyFrames.size() - 1)
	{	// �� �ڿ� �߰�
		keyFrames.push_back(keyFrame);
		return keyFrame;
	}

	// �迭 Ȯ��
	keyFrames.push_back(null);

	// ���� Ȯ��
	for (uint i=keyFrames.size()-1; i>index; --i)
	{
		keyFrames[i] = keyFrames[i-1];
	}

	// �߰�
	keyFrames[index] = keyFrame;

    return keyFrame;
}

KeyFrame* Animation::getKeyFrame(int index)
{
    return keyFrames[index];
}

/// ����.. totalFrameCount��, �� keyFrame�� startFrame�� ����
void Animation::refresh()
{
	totalFrameCount = 0;
	for (uint i=0; i<keyFrames.size(); ++i)
	{
		keyFrames[i]->setStartFrame(totalFrameCount);
		totalFrameCount += keyFrames[i]->getDuration();
	}
}