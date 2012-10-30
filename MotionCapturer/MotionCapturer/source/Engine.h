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

	inline bool operator == (const NormalizedPoint &np) const
	{
		return (x == np.x && y == np.y && z == np.z);
	}
};

template <>
class hash<NormalizedPoint>
{
public:
	size_t operator() (const NormalizedPoint &value) const
	{
		return uint64(value.x) | (uint64(value.y) << 20) | (uint64(value.z) << 40);
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

	// 멤버 변수
private:
	bool runningState;
	int width, height;
	Camera camera;
	
	float pointCloudUnit;
	vector<CloudElement> pointCloud;						///< 포인트 클라우드

	Actor actor;
	Actor base;
	Animation *animation;		///< free

public:
	int testX, testY;
	bool rotationFlag;
	float angle;
	

	// 멤버 함수
private:
	Engine();
	~Engine();

	/// 동기화
	float syncronize();

public:
	/// 메인 루프
	void run();

	void update(float deltaTime);

	void draw();

	/// 리사이즈
	void resize(int width, int height);

	/// 포인트 클라우드 초기화
	inline void clearPointCloud()
	{
		pointCloud.clear();
		//foregroundSet.clear();
	}

	void addPointCloud(CloudElement *cloud, int size);

	/// 포인트 클라우드 저장
	void savePointCloud();

	inline void normalizePoint(const Vector3 &from, NormalizedPoint &to)
	{
		to.x = floor(from.getX() / pointCloudUnit);
		to.y = floor(from.getY() / pointCloudUnit);
		to.z = floor(from.getZ() / pointCloudUnit);
	}

	inline void normalizeVector(const NormalizedPoint &from, Vector3 &to)
	{
		to.setX(from.x * pointCloudUnit);
		to.setY(from.y * pointCloudUnit);
		to.setZ(from.z * pointCloudUnit);
	}

	// 접근
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
	GETTER_REF(Actor, Actor, actor)
	GETTER_REF(Actor, BaseActor, base)
	GETTER(Animation*, Animation, animation)
	GETTER(float, PointCloudUnit, pointCloudUnit)
};

