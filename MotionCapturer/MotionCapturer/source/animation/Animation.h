#pragma once


#include <string>
#include <vector>

#include "../util/defines.h"

using namespace std;


class KeyFrame;

class Animation
{
	// ��� ����
private:
    string name;
    vector<KeyFrame*> keyFrames;		///< Ű������ �迭.. free
    int totalFrameCount;


	// ��� ����
public:
    ///  ������
    Animation();

	/// �Ҹ���
	~Animation();

    /// Ű������ �߰�
    KeyFrame* addKeyFrame();

    KeyFrame* getKeyFrame(int index);


    // ����
    GETTER_CONST_REF(string, Name, name)
    GETTER(int, KeyFrameCount, keyFrames.size())
    GETTER(int, TotalFrameCount, totalFrameCount)
};