#pragma once

#include <vector>

#include "../util/defines.h"

using namespace std;

class Animation;
class Bone;

class AnimationInstance
{
	// ��� ����
private:
    Animation *animation;

    int curFrame;
	int curKeyFrameIndex;


	// ��� �Լ�
public:
    AnimationInstance();

    void update(const vector<Bone*> &bones, int deltaFrame);

    void reset();

    /// <summary>
    /// ���� Ű������ �ε��� ����
    /// </summary>
    /// <param name="curFrame"></param>
    void refreshCurKeyFrameIndex(int curFrame);

    void reset();

    // �� ����Ʈ ������Ʈ
    void updateBones(const vector<Bone*> &boneList, int curFrame);

	// �ִϸ��̼� ����
	void setAnimation(Animation *animation);

	// Ű ������ ���� ���
	void getKeyFrameCount();


	// ����
	GETTER(Animation*, Animation, animation);
};