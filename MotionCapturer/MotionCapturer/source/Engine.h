#pragma once

#include <unordered_set>
#include <vector>
#include <Windows.h>

#include "camera/Camera.h"
#include "marker/MarkerRecognizer.h"
#include "model/Actor.h"
#include "util/defines.h"
#include "util/Singleton.h"


#define ENGINE		Engine::getInstance()

class Animation;
class Vector3;

using namespace std;

struct NormalizedPoint
{
	int x, y, z;
}

class hash<NormalizedPoint>
{
public:
	size_t operator() (const NormalizedPoint &value)
	{
		return uint64(value.z) | (uint64(value.y) << 20) | (uint64(value.x) << 40);
	}
};

struct CloudElement
{
	Vector3 position;
	byte color[3];
};

class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// ��� ����
private:
	bool runningState;
	int width, height;
	Camera camera;
	
	float pointCloudUnit;
	unordered_set<NormalizedPoint> backgroundPointCloud;	///< ���
	vector<CloudElement*> pointCloudQueue;					///< ����Ʈ Ŭ���带 �־���� �뵵.. free

	Actor actor;
	Actor base;
	Animation *animation;		///< free

public:
	int testX, testY;
	bool rotationFlag;
	float angle;
	

	// ��� �Լ�
private:
	Engine();
	~Engine();

	/// ����ȭ
	float syncronize();

public:
	/// ���� ����
	void run();

	void update(float deltaTime);

	void draw();

	/// ��������
	void resize(int width, int height);

	/// ����Ʈ Ŭ���� �ʱ�ȭ
	void clearPointCloudQueue();

	void addPointCloud(CloudElement *cloud);

	/// ����Ʈ Ŭ���� ����
	void savePointCloud();

	inline void addBackground(NormalizedPoint &np)
	{
		backgroundPointCloud.insert(np);
	}

	inline void clearBackground()
	{
		backgroundPointCloud.clear();
	}

	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
	GETTER_REF(Actor, Actor, actor)
	GETTER_REF(Actor, BaseActor, base)
	GETTER(Animation*, Animation, animation)
	GETTER(float, PointCloudUnit, pointCloudUnit)
};

