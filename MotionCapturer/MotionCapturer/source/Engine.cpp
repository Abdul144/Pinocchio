
#include "stdafx.h"
#include "Engine.h"

#include <GL/glew.h>

#include "kinect/Kinect.h"
#include "kinect/KinectManager.h"
#include "marker/MarkerRecognizer.h"
#include "util/Vector3.h"


Engine::Engine() : runningState(true), testX(0), testY(0)
{
	camera.getPosition().set(0, 0, 2);
}

Engine::~Engine()
{
}

void Engine::clearPointCloudQueue()
{
	for (int i=0; i<pointCloudQueue.size(); ++i)
	{
		DELETE_ARRAY(pointCloudQueue[i]);
	}
	pointCloudQueue.clear();
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
}

void Engine::draw()
{
	/// 그리기
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	
    static const float xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);

	glPushMatrix();

	camera.applyViewMatrix();
	
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	/*
	static float r = 0.f;
	glRotatef(r, 0, 1, 0);
	r += 2.f;
	//*/
	
	glPointSize(1.f);
	
	float radius = 1.f;
	Kinect *kinect = KINECT_MANAGER.getKinect(0);

	glBegin(GL_POINTS);
	{
		for (int i=0; i<pointCloudQueue.size(); ++i)
		{
			CloudElement *cloud = pointCloudQueue[i];

			for (int y=0; y<480; ++y)
			{
				for (int x=0; x<640; ++x)
				{
					CloudElement &element = cloud[x + y*640];

					glColor4ub(element.color[0], element.color[1], element.color[2], 255);
					glVertex3f(element.position.getX(), element.position.getY(), element.position.getZ());
				}
			}
		}
	}
	glEnd();

	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}

void myGLPerspectivef(float fovy, float aspect, float nearValue, float farValue)
{
	float h = 2 * nearValue * tanf(deg2rad(fovy/2));
	float top = h;
	float bottom = -h;
	float left = aspect * bottom;
	float right = aspect * top;
	glFrustum(left, right, bottom, top, nearValue, farValue);
}

/// 리사이즈
void Engine::resize(int width, int height)
{
	this->width = width;
	this->height = height;

	/*
	/// 직교 투영
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, width, height, 0, -1000.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	/// 원근 투영
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	myGLPerspectivef(58.5f, (float)width / (float)height, 1, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    glViewport(0, 0, width, height);

	/// open gl 설정
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}
