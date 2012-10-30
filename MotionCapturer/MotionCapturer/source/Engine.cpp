
#include "stdafx.h"
#include "Engine.h"

#include <GL/glew.h>

#include "animation/Animation.h"
#include "kinect/Kinect.h"
#include "kinect/KinectManager.h"
#include "util/Matrix.h"
#include "util/Vector3.h"


Engine::Engine()
	: runningState(true), pointCloudUnit(0.01f), backgroundPointCloud(102400), foregroundSet(102400), testX(0), testY(0), animation(null), rotationFlag(false), angle(0.f)
{
	camera.setPosition(Vector3(0, 0, 2));

	animation = new Animation;
	actor.setCurAnimation(animation);
}

Engine::~Engine()
{
	DELETE(animation);
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
	actor.update((int)(deltaTime * 60.f));
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
	if (rotationFlag)
	{
		glRotatef(angle, 0, 1, 0);
		angle += 2.f;
	}
	
	glPointSize(1.f);
	
	float radius = 1.f;

	glBegin(GL_POINTS);
	{
		for (int i=0; i<pointCloud.size(); ++i)
		{
			CloudElement &element = pointCloud[i];

			glColor4ub(element.color[0], element.color[1], element.color[2], 255);
			glVertex3f(element.position.getX(), element.position.getY(), element.position.getZ());

		}
	}
	glEnd();

	/*
	// skeleton 그리기
	if (KINECT_MANAGER.getKinectCount() > 0)
	{
		KINECT_MANAGER.getKinect(0)->drawSkeleton();
	}

	// actor 그리기
	actor.draw();
	*/

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

/// 스틸 샷 추가.
void Engine::addPointCloud(CloudElement *cloud, int size)
{
	for (int i=0; i<size; ++i)
	{
		CloudElement &element = cloud[i];

		if (element.position.getZ() > 4000.f)
			continue;
		
		NormalizedPoint np;
		normalizePoint(element.position, np);

		// 배경에 걸리면 다음으로
		if (backgroundPointCloud.find(np) != backgroundPointCloud.end())
			continue;

		// 이미 있으면 다음으로
//		if (foregroundSet.find(np) != foregroundSet.end())
//			continue;

		// 추가
//		foregroundSet.insert(np);
		normalizeVector(np, element.position);
		pointCloud.push_back(element);
	}
}

/// 포인트 클라우드 저장
void Engine::savePointCloud()
{
	FILE *fp = fopen("pointCloud.txt", "wb");

	// OFF 출력
	fprintf(fp, "OFF\n");

	// 총 개수 출력
	fprintf(fp, "%d 0 0\n", pointCloud.size());

	// 출력
	for (uint i=0; i<pointCloud.size(); ++i)
	{
		Vector3 &position = pointCloud[i].position;
		fprintf(fp, "%lf %lf %lf\n", position.getX(), position.getY(), position.getZ());
	}

	fclose(fp);
}