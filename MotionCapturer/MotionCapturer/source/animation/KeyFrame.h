#pragma once

#include <vector>

#include "../util/defines.h"

using namespace std;


class BoneData;

class KeyFrame
{
	// ��� ����
private:
    int startFrame;
    int duration;					///< ����� ����

    vector<BoneData*> boneDatas;		///< �� ���� Data



	// ��� �Լ�
public:
    /// ������
    KeyFrame()
    {
    }


    /// ������ �߰�
    BoneData* addBoneData();

    BoneData* getBoneData(int index);

    void setBoneData(int index, BoneData *data);

        
    // ����
    GETTER(int, Duration, duration)
    GETTER(int, BoneDataCount, boneDatas.size())
    GETTER(int, StartFrame, startFrame)

};