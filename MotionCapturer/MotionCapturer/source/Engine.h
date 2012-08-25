#pragma once

#include <vector>
#include <Windows.h>

#include "camera/Camera.h"
#include "marker/MarkerRecognizer.h"
#include "util/defines.h"
#include "util/Singleton.h"


#define ENGINE		Engine::getInstance()

class Vector3;

using namespace std;


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

public:
	struct CloudElement
	{
		Vector3 position;
		byte color[3];
	};

	// 멤버 변수
private:
	bool runningState;
	int width, height;
	Camera camera;

	vector<CloudElement*> pointCloudQueue;		///< 포인트 클라우드를 넣어놓는 용도.. free

public:
	int testX, testY;
	

	// 멤버 함수
private:
	Engine();
	~Engine();

	/// 동기화
	float syncronize();

	void update(float deltaTime);

	void draw();

public:
	/// 메인 루프
	void run();

	/// 리사이즈
	void resize(int width, int height);

	/// 포인트 클라우드 초기화
	void clearPointCloudQueue();

	void addPointCloud(CloudElement *cloud);

	// 포인트 클라우드를 마커에 맞추어 역변환
	void inverseTransformPointCloud(CloudElement *result, MarkerRecognizer::sMarkerInfo &marker, const Vector3 *point, const byte *colorBuffer, int width, int height);

	// 접근
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
};

