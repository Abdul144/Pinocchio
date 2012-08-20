
#include "stdafx.h"
#include "Camera.h"

#include <GL/glew.h>

#include "../util/Matrix.h"


Camera::Camera()
{
	position.set(0, 0, 0);
	direction.set(0, 0, -1);
	up.set(0, 1, 0);
}

Camera::~Camera()
{
}

void Camera::applyViewMatrix()
{
	/*
	glRotatef(-rad2deg(rotation.getZ()), 0.f, 0.f, 1.f);
	glRotatef(-rad2deg(rotation.getX()), 1.f, 0.f, 0.f);
	glRotatef(-rad2deg(rotation.getY()), 0.f, 1.f, 0.f);
	glTranslatef(-position.getX(), -position.getY(), -position.getZ());
	//*/

	direction.normalize();
	up.normalize();

	Matrix viewMat;
	viewMat.setViewMatrix(position, direction, up);
	glMultMatrixf(viewMat.getPointer());
}