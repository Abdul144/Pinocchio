
#include "stdafx.h"
#include "Camera.h"

#include <GL/glew.h>



Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::applyViewMatrix()
{
	//*
	glRotatef(-rad2deg(rotation.getZ()), 0.f, 0.f, 1.f);
	glRotatef(-rad2deg(rotation.getX()), 1.f, 0.f, 0.f);
	glRotatef(-rad2deg(rotation.getY()), 0.f, 1.f, 0.f);
	glTranslatef(-position.getX(), -position.getY(), -position.getZ());
	//*/
	/*
	glRotatef(-rotation.getZ(), 0.f, 0.f, 1.f);
	glRotatef(-rotation.getX(), 1.f, 0.f, 0.f);
	glRotatef(-rotation.getY(), 0.f, 1.f, 0.f);
	glTranslatef(-position.getX(), -position.getY(), -position.getZ());
	//*/
}