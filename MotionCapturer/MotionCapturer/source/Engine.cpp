
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
	/*
	// Ű��Ʈ���� ������ �޾ƿͼ� �����ϱ�
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		bool depthIsRefreshed = KINECT_MANAGER.getKinect(i)->refreshDepthBuffer() >= 0;
		bool colorIsRefreshed = KINECT_MANAGER.getKinect(i)->refreshColorBuffer() >= 0;
		if (depthIsRefreshed && colorIsRefreshed)
			KINECT_MANAGER.getKinect(i)->mapColorToDepth();
	}
	//*/
}

void Engine::draw()
{
	/// �׸���
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	
    static const float xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);

	glPushMatrix();

	camera.applyViewMatrix();
	
	//glTranslatef(0, 0, -2 - 1.f);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	/*
	static float r = 0.f;
	glRotatef(r, 0, 1, 0);
	r += 2.f;
	//*/
	
	//glTranslatef(0, 0, 2);

	glPointSize(1.f);
	
	float radius = 1.f;
	Kinect *kinect = KINECT_MANAGER.getKinect(0);

	//glColor4ub(255, 255, 255, 255);
	//*
	glBegin(GL_POINTS);
	{
		for (int i=0; i<pointCloudQueue.size(); ++i)
		{
			CloudElement *cloud = pointCloudQueue[i];

			for (int y=0; y<480; ++y)
			{
				for (int x=0; x<640; ++x)
				{
					//byte *p = &kinect->getMappedColorBuffer()[(x + y*kinect->getColorWidth())*4];
					
					CloudElement &element = cloud[x + y*640];

					glColor4ub(element.color[0], element.color[1], element.color[2], 255);
					glVertex3f(element.position.getX(), element.position.getY(), element.position.getZ());
				}
			}
		}
	}
	glEnd();
	//*/


	// z�� �׸���
	glPointSize(2.f);
	glColor4ub(255, 0, 0, 255);
	/*
	glBegin(GL_POINTS);
	{
		for (int i=-1000; i<=1000; ++i)
			glVertex3f(0, 0, i*0.01f);
	}
	glEnd();
	//*/
	/*
	if (MARKER_RECOGNIZER.getMarkerCount() > 0)
	{
		glBegin(GL_POINTS);
		{
			MarkerRecognizer::sMarkerInfo &marker = MARKER_RECOGNIZER.getMarker(0);
			for (int i=0; i<4; ++i)
			{
				Vector3 &v = kinect->getPointCloud()[(int)marker.corner[i].x + (int)marker.corner[i].y * 640];
				glVertex3f(v.getX(), v.getY(), v.getZ());
			}
		}
		glEnd();
	}
	//*/

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

/// ��������
void Engine::resize(int width, int height)
{
	this->width = width;
	this->height = height;

	/*
	/// ���� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, width, height, 0, -1000.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	/// ���� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluPerspective(45, (float)width / (float)height, 1000, -1000);
	//glFrustum(-width / 2, width / 2, -height / 2, height / 2, 0, 100);
	//glFrustum(-1, 1, -1, 1, 0, 10);
	myGLPerspectivef(58.5f, (float)width / (float)height, 1, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    glViewport(0, 0, width, height);

	/// open gl ����
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
