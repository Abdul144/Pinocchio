
#include "stdafx.h"
#include "Engine.h"

#include <GL/glew.h>

#include "kinect/Kinect.h"
#include "kinect/KinectManager.h"
#include "util/Matrix.h"
#include "util/Vector3.h"


Engine::Engine() : runningState(true), testX(0), testY(0)
{
	camera.setPosition(Vector3(0, 0, 2));
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

/// 스틸 샷 추가.
void Engine::addPointCloud(CloudElement *cloud)
{
	pointCloudQueue.push_back(cloud);
}

// 포인트 클라우드를 마커에 맞추어 역변환
void Engine::inverseTransformPointCloud(CloudElement *result, MarkerRecognizer::sMarkerInfo &marker, const Vector3 *point, const byte *colorBuffer, int width, int height)
{
	// 코너를 얻어옴
	const Vector3 &v0 = point[(int)marker.corner[0].x + (int)marker.corner[0].y * width];
	const Vector3 &v1 = point[(int)marker.corner[1].x + (int)marker.corner[1].y * width];
	const Vector3 &v2 = point[(int)marker.corner[2].x + (int)marker.corner[2].y * width];
	const Vector3 &v3 = point[(int)marker.corner[3].x + (int)marker.corner[3].y * width];

	// 변환행렬 구성위해 up, direction 벡터를 얻는다.
	Vector3 right, up, direction;
	up = v0 - v1;
	right = v2 - v1;
	direction.cross(up, right);
	up.normalize();
	direction.normalize();
	Vector3 vc;
	vc = (v0 + v1 + v2 + v3) / 4.f;
	
	// 변환행렬 구성
	Matrix transform;
	transform.setViewMatrix(vc, direction, up);
										
	// 마커 위치를 기준으로 pointCloud를 변환
	for (int i=0; i<width*height; ++i)
	{
		Engine::CloudElement &element = result[i];
		transform.multiply(element.position, point[i]);
						
		element.color[0] = colorBuffer[i*4 + 2];
		element.color[1] = colorBuffer[i*4 + 1];
		element.color[2] = colorBuffer[i*4 + 0];
	}
}