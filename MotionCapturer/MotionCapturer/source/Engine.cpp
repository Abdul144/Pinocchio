
#include "stdafx.h"
#include "Engine.h"

#include <Windows.h>

#include "kinect/Kinect.h"
#include "kinect/KinectManager.h"


Engine::Engine() : runningState(true)
{
}

/// ���� ����
void Engine::run()
{
	float deltaTime;

	// ����ȭ
	deltaTime = syncronize();

	// ������Ʈ
	update(deltaTime);

	// �׸���
	draw();
}



/// ����ȭ
float Engine::syncronize()
{
	static uint oldTime = timeGetTime();
	uint deltaTime;

	while(true)
	{
		deltaTime = timeGetTime() - oldTime;

		if (deltaTime > 100)
		{
			oldTime += deltaTime;
			continue;

		}else if (deltaTime < 16)
		{
			continue;
		}

		break;
	}

	oldTime += deltaTime;

	return (float)deltaTime * 0.001f;
}

void Engine::update(float deltaTime)
{
	// depth ���� ����
	refreshDepthBuffer();

	// color ���� ����
	refreshColorBuffer();
}

void Engine::draw()
{
}

/// depth ���� ����
void Engine::refreshDepthBuffer()
{
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		KINECT_MANAGER.getKinect(i)->refreshDepthBuffer();
	}
}

/// color ���� ����
void Engine::refreshColorBuffer()
{
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		KINECT_MANAGER.getKinect(i)->refreshColorBuffer();
	}
}