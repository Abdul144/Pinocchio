#pragma once

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

	vector<CloudElement*> pointCloudQueue;		///< ����Ʈ Ŭ���带 �־���� �뵵.. free

	Actor actor;
	Actor base;
	Animation *animation;		///< free

public:
	int testX, testY;
	

	// ��� �Լ�
private:
	Engine();
	~Engine();

	/// ����ȭ
	float syncronize();

	void update(float deltaTime);

	void draw();

public:
	/// ���� ����
	void run();

	/// ��������
	void resize(int width, int height);

	/// ����Ʈ Ŭ���� �ʱ�ȭ
	void clearPointCloudQueue();

	void addPointCloud(CloudElement *cloud);

	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
	GETTER_REF(Actor, Actor, actor)
	GETTER_REF(Actor, BaseActor, base)
	GETTER(Animation*, Animation, animation);
};

