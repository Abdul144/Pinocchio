#pragma once

#include "../util/defines.h"
#include "../util/Vector3.h"


class Camera
{
	// ��� ����
private:
	Vector3 position;
	Vector3 rotation;
	

	// ��� �Լ�
public:
	Camera();
	~Camera();

	void applyViewMatrix();


	// ����
public:
	GETTER_REF(Vector3, Position, position)
	GETTER_REF(Vector3, Rotation, rotation)
};

