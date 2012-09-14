
#include "stdafx.h"
#include "KeyFrame.h"

#include "BoneData.h"

/// ������
KeyFrame::KeyFrame() : startFrame(0), duration(1)
{
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