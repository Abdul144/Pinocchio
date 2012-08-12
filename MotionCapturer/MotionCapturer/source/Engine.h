#pragma once


#include "util/defines.h"
#include "util/Singleton.h"


#define ENGINE		Engine::getInstance()


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

	// ��� ����
private:
	bool runningState;


	// ��� �Լ�
private:
	Engine();

	/// ����ȭ
	float syncronize();

	void update(float deltaTime);

	void draw();

	/// depth ���� ����
	void refreshDepthBuffer();

	/// color ���� ����
	void refreshColorBuffer();

public:
	/// ���� ����
	void run();


	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
};

