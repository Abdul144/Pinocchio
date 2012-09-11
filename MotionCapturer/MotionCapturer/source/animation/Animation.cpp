
#include "stdafx.h"
#include "Animation.h"

#include "KeyFrame.h"


///  생성자
Animation::Animation() : totalFrameCount(0)
{
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