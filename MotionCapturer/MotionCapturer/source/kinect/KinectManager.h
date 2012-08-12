#pragma once

#include <vector>

#include "../util/Singleton.h"


#define KINECT_MANAGER			KinectManager::getInstance()

using namespace std;

class Kinect;


class KinectManager : public Singleton<KinectManager>
{
	USE_SINGLETON(KinectManager)

	// 멤버 변수
private:
	vector<Kinect*> kinects;		///< free


	// 멤버 함수
private:
	KinectManager();
	~KinectManager();

public:
	/// 키넥트 연결 확보.. 추가 연결된 개수 리턴
	int connectKinects(int count);

	/// 키넥트 연결 모두 끊기
	void deconnectKinects();

	uint getKinectCount()
	{
		return kinects.size();
	}

	Kinect* getKinect(int index)
	{
		return kinects[index];
	}


	// 접근
public:
};

