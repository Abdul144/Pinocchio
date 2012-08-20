#pragma once

#include "../util/defines.h"
#include "../util/Vector3.h"


class Camera
{
	// ��� ����
private:
	Vector3 position;
	Vector3 direction;
	Vector3 up;
	

	// ��� �Լ�
public:
	Camera();
	~Camera();

	void applyViewMatrix();


	// ����
public:
	GETTER_REF(Vector3, Position, position)
	GETTER_REF(Vector3, Direction, direction)
	GETTER_REF(Vector3, Up, up)
};

