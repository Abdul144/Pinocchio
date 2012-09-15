
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
KeyFrame* Animation::addKeyFrame(int index)
{
    KeyFrame *keyFrame = new KeyFrame;

	if (index < 0 || index >= keyFrames.size() - 1)
	{	// 맨 뒤에 추가
		keyFrames.push_back(keyFrame);
		return keyFrame;
	}

	// 배열 확장
	keyFrames.push_back(null);

	// 공간 확보
	for (uint i=keyFrames.size()-1; i>index; --i)
	{
		keyFrames[i] = keyFrames[i-1];
	}

	// 추가
	keyFrames[index] = keyFrame;

    return keyFrame;
}

KeyFrame* Animation::getKeyFrame(int index)
{
    return keyFrames[index];
}

/// 갱신.. totalFrameCount나, 각 keyFrame의 startFrame을 갱신
void Animation::refresh()
{
	totalFrameCount = 0;
	for (uint i=0; i<keyFrames.size(); ++i)
	{
		keyFrames[i]->setStartFrame(totalFrameCount);
		totalFrameCount += keyFrames[i]->getDuration();
	}
}