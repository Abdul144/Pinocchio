#pragma once

#include <vector>

#include "../util/Singleton.h"


#define KINECT_MANAGER			KinectManager::getInstance()

using namespace std;

class Kinect;


class KinectManager : public Singleton<KinectManager>
{
	USE_SINGLETON(KinectManager)

	// ��� ����
private:
	vector<Kinect*> kinects;		///< free


	// ��� �Լ�
private:
	KinectManager();
	~KinectManager();

public:
	/// Ű��Ʈ ���� Ȯ��.. �߰� ����� ���� ����
	int connectKinects(int count);

	/// Ű��Ʈ ���� ��� ����
	void deconnectKinects();

	uint getKinectCount()
	{
		return kinects.size();
	}

	Kinect* getKinect(int index)
	{
		return kinects[index];
	}


	// ����
public:
};

