#pragma once


#include "util/defines.h"
#include "util/Singleton.h"


#define ENGINE		Engine::getInstance()


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// 멤버 변수
private:
	bool runningState;


	// 멤버 함수
private:
	Engine();

	/// 동기화
	float syncronize();

	void update(float deltaTime);

	void draw();

	/// depth 버퍼 갱신
	void refreshDepthBuffer();

	/// color 버퍼 갱신
	void refreshColorBuffer();

public:
	/// 메인 루프
	void run();


	// 접근
public:
	GETTER_SETTER(bool, RunningState, runningState)
};

