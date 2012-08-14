#pragma once

#include "../util/defines.h"


class Vector3
{
	// ¸â¹ö º¯¼ö
private:
	float x, y, z;
	

	// ¸â¹ö ÇÔ¼ö
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


	// Á¢±Ù
public:
	GETTER_SETTER(float, X, x)
	GETTER_SETTER(float, Y, y)
	GETTER_SETTER(float, Z, z)
};

