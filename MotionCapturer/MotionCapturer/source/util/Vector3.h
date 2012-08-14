#pragma once

#include "../util/defines.h"


class Vector3
{
	// ��� ����
private:
	float x, y, z;
	

	// ��� �Լ�
public:
	Vector3(float x = 0.f, float y = 0.f, float z = 0.f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}


	// ����
public:
	GETTER_SETTER(float, X, x)
	GETTER_SETTER(float, Y, y)
	GETTER_SETTER(float, Z, z)
};

