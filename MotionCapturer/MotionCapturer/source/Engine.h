#pragma once

#include <vector>
#include <Windows.h>

#include "util/defines.h"
#include "util/Singleton.h"
#include "camera/Camera.h"


#define ENGINE		Engine::getInstance()

class Vector3;
using namespace std;


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// ��� ����
private:
	bool runningState;
	int width, height;
	Camera camera;

	vector<Vector3*> pointCloudQueue;		///< ����Ʈ Ŭ���带 �־���� �뵵.. free
	

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


	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
	GETTER_REF(vector<Vector3*>, PointCloudQueue, pointCloudQueue)
};

