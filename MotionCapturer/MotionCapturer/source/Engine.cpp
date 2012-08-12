
#include "stdafx.h"
#include "Engine.h"

#include <Windows.h>

#include "kinect/Kinect.h"
#include "kinect/KinectManager.h"


Engine::Engine() : runningState(true)
{
}

/// 메인 루프
void Engine::run()
{
	float deltaTime;

	// 동기화
	deltaTime = syncronize();

	// 업데이트
	update(deltaTime);

	// 그리기
	draw();
}



/// 동기화
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
	// depth 버퍼 갱신
	refreshDepthBuffer();

	// color 버퍼 갱신
	refreshColorBuffer();
}

void Engine::draw()
{
}

/// depth 버퍼 갱신
void Engine::refreshDepthBuffer()
{
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		KINECT_MANAGER.getKinect(i)->refreshDepthBuffer();
	}
}

/// color 버퍼 갱신
void Engine::refreshColorBuffer()
{
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		KINECT_MANAGER.getKinect(i)->refreshColorBuffer();
	}
}