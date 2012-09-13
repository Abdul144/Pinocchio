
#include "stdafx.h"
#include "Animation.h"

#include "KeyFrame.h"


///  생성자
Animation::Animation() : totalFrameCount(0)
{
}

/// 소멸자
Animation::~Animation()
{
	for (uint i=0; i<keyFrames.size(); ++i)
	{
		DELETE(keyFrames[i])
	}
}

/// 키프레임 추가
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