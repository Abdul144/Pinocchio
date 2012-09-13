
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
KeyFrame* Animation::addKeyFrame()
{
    KeyFrame *keyFrame = new KeyFrame;
    keyFrames.push_back(keyFrame);

    return keyFrame;
}

KeyFrame* Animation::getKeyFrame(int index)
{
    return keyFrames[index];
}