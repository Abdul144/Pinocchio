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

    vector<BoneData*> boneDatas;	///< �� ���� Data.. free



	// ��� �Լ�
public:
    /// ������
    KeyFrame();

	/// �Ҹ���
	~KeyFrame();

    /// ������ �߰�
    BoneData* addBoneData();

    BoneData* getBoneData(int index);

    void setBoneData(int index, BoneData *data);

        
    // ����
    GETTER(int, BoneDataCount, boneDatas.size())
    GETTER_SETTER(int, Duration, duration)
    GETTER_SETTER(int, StartFrame, startFrame)

};