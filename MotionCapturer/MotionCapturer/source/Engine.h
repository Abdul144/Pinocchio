#pragma once

#include <Windows.h>

#include "util/defines.h"
#include "util/Singleton.h"
#include "camera/Camera.h"


#define ENGINE		Engine::getInstance()


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// 멤버 변수
private:
	bool runningState;

	int width, height;

	Camera camera;
	

	// 멤버 함수
private:
	Engine();

	/// 동기화
	float syncronize();

	void update(float deltaTime);

	void draw();

public:
	/// 메인 루프
	void run();

	/// 리사이즈
	void resize(int width, int height);


	// 접근
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
};

