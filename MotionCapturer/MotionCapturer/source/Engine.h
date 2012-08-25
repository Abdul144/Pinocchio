#pragma once

#include <vector>
#include <Windows.h>

#include "camera/Camera.h"
#include "marker/MarkerRecognizer.h"
#include "util/defines.h"
#include "util/Singleton.h"


#define ENGINE		Engine::getInstance()

class Vector3;

using namespace std;


class Engine : public Singleton<Engine>
{
	USE_SINGLETON(Engine)

public:
	struct CloudElement
	{
		Vector3 position;
		byte color[3];
	};

	// ��� ����
private:
	bool runningState;
	int width, height;
	Camera camera;

	vector<CloudElement*> pointCloudQueue;		///< ����Ʈ Ŭ���带 �־���� �뵵.. free

public:
	int testX, testY;
	

	// ��� �Լ�
private:
	Engine();
	~Engine();

	/// ����ȭ
	float syncronize();

	void update(float deltaTime);

	void draw();

public:
	/// ���� ����
	void run();

	/// ��������
	void resize(int width, int height);

	/// ����Ʈ Ŭ���� �ʱ�ȭ
	void clearPointCloudQueue();

	void addPointCloud(CloudElement *cloud);

	// ����Ʈ Ŭ���带 ��Ŀ�� ���߾� ����ȯ
	void inverseTransformPointCloud(CloudElement *result, MarkerRecognizer::sMarkerInfo &marker, const Vector3 *point, const byte *colorBuffer, int width, int height);

	// ����
public:
	GETTER_SETTER(bool, RunningState, runningState)
	GETTER_REF(Camera, Camera, camera)
};

