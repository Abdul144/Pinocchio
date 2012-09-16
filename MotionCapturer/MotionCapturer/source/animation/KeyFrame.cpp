
#include "stdafx.h"
#include "KeyFrame.h"

#include <NuiApi.h>

#include "../kinect/Kinect.h"
#include "../model/Bone.h"
#include "BoneData.h"

/// ������
KeyFrame::KeyFrame() : startFrame(0), duration(1)
{
	for (int i=0; i<20; ++i)
	{
		addBoneData();
	}
}

/// �Ҹ���
KeyFrame::~KeyFrame()
{
	for (uint i=0; i<boneDatas.size(); ++i)
	{
		DELETE(boneDatas[i])
	}
}

/// ������ �߰�
BoneData* KeyFrame::addBoneData()
{
    BoneData *boneData = new BoneData;
    boneDatas.push_back(boneData);

    return boneData;
}

BoneData* KeyFrame::getBoneData(int index)
{
    return boneDatas[index];
}

void KeyFrame::setBoneData(int index, BoneData *data)
{
    boneDatas[index] = data;
}

void KeyFrame::setBoneDataFromKinect(Kinect *kinect)
{
	for (uint i=0; i<boneDatas.size(); ++i)
	{
		boneDatas[i]->rotation = kinect->getSkeletonRotationInfo()[kinect->getKinectBoneIndex(i)];
	}

}