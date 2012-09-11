
#include "stdafx.h"
#include "KeyFrame.h"

#include "BoneData.h"


/// 본정보 추가
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