#pragma once

#include "../util/defines.h"
#include "../util/Vector3.h"


class Camera
{
	// ¸â¹ö º¯¼ö
private:
	Vector3 position;
	Vector3 rotation;
	

	// ¸â¹ö ÇÔ¼ö
public:
	Camera();
	~Camera();

	void applyViewMatrix();


	// Á¢±Ù
public:
	GETTER_REF(Vector3, Position, position)
	GETTER_REF(Vector3, Rotation, rotation)
};

