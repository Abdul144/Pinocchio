
#include "stdafx.h"
#include "Animation.h"

#include "KeyFrame.h"


///  ������
Animation::Animation() : totalFrameCount(0)
{
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