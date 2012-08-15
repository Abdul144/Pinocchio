#pragma once

#include <Windows.h>

#include "util/defines.h"
#include "util/Singleton.h"
#include "camera/Camera.h"


#define ENGINE		Engine::getInstance()


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// ��� ����
private:
	bool runningState;

	int width, height;

	Camera camera;
	

	// ��� �Լ�
private:
	Engine();

	/// ����ȭ
	float syncronize();

	void update(float deltaTime);

	void draw();

public:
	/// ���� ����
	void run();

	/// ��������
	void resize(int width, int height);


	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
};

