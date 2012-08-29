
#include "stdafx.h"
#include "Camera.h"

#include <GL/glew.h>

#include "../util/Quaternion.h"


Camera::Camera() : invalid(true)
{
	position.set(0, 0, 0);

	baseDirection.set(0, 0, -1);
	baseUp.set(0, 1, 0);
	baseRight.set(1, 0, 0);

	rotation.set(0.f, 0.f, 0.f);

	direction.set(0, 0, -1);
	up.set(0, 1, 0);
	right.set(1, 0, 0);
}

Camera::~Camera()
{
}

void Camera::updateViewMatrix()
{
	if (invalid)
	{	// 갱신해야 함
		// quaternion 구성
		Quaternion q, qx, qy, qz;
		qx.setFromAxisAngle(baseRight, rotation.getX());
		qy.setFromAxisAngle(baseUp, rotation.getY());
		qz.setFromAxisAngle(baseDirection, rotation.getZ());
		q = qy * qx * qz;

		// 변환 행렬 구성
		Matrix transform;
		q.getMatrix(transform);

		// 축의 변환
		transform.multiply(right, baseRight);
		transform.multiply(up, baseUp);
		transform.multiply(direction, baseDirection);

		// 뷰 행렬 구성
		viewMat.setViewMatrix(position, direction, up);

		invalid = false;
	}
}

void Camera::applyViewMatrix()
{
	updateViewMatrix();

	// 적용
	glMultMatrixf(viewMat.getPointer());
}

/// 평행 이동
void Camera::moveForward(float distance)
{
	position += direction * distance;
	invalidate();
}
void Camera::moveUp(float distance)
{
	position += up * distance;
	invalidate();
}
void Camera::moveRight(float distance)
{
	position += right * distance;
	invalidate();
}

/// view matrix 얻기
const Matrix& Camera::getViewMatrix()
{
	updateViewMatrix();

	return viewMat;
}

/// 기본 축 변경
void Camera::setBaseAxises(const Vector3 &direction, const Vector3 &up)
{
	// 구성
	baseDirection = direction;
	baseUp = up;
	baseRight.cross(direction, up);

	// 정규화
	baseDirection.normalize();
	baseUp.normalize();
	baseRight.normalize();

	// 상대 각도 초기화
	rotation.set(0.f, 0.f, 0.f);

	// update 필요
	invalidate();
}

























