
#include "stdafx.h"
#include "Matrix.h"

#include <string.h>
#include <math.h>

#include "defines.h"
#include "Vector3.h"


/// 생성자
Matrix::Matrix()
{
}

/// 복사 생성자
Matrix::Matrix(const Matrix &mat)
{
	// 단순 복사
	memcpy(e, mat.e, 16 * sizeof(float));
}

/// 단위 행렬로 만들기
void Matrix::setIdentity()
{
	e[0] = 1;			e[4] = 0;			e[8] = 0;			e[12] = 0;
	e[1] = 0;			e[5] = 1;			e[9] = 0;			e[13] = 0;
	e[2] = 0;			e[6] = 0;			e[10] = 1;			e[14] = 0;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}

/// Scale 행렬로 만들기
void Matrix::setScale(const float &scaleX, const float &scaleY, const float &scaleZ)
{
	e[0] = scaleX;		e[4] = 0;			e[8] = 0;			e[12] = 0;
	e[1] = 0;			e[5] = scaleY;		e[9] = 0;			e[13] = 0;
	e[2] = 0;			e[6] = 0;			e[10] = scaleZ;		e[14] = 0;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}

/// Rotate 행렬로 만들기
void Matrix::setRotateX(const float &degree)
{
	float r = deg2rad(degree);
	return setRotateXByRadian(r);
}
void Matrix::setRotateY(const float &degree)
{
	float r = deg2rad(degree);
	return setRotateYByRadian(r);
}
void Matrix::setRotateZ(const float &degree)
{
	float r = deg2rad(degree);
	return setRotateZByRadian(r);
}

/// Rotate 행렬로 만들기
void Matrix::setRotateXByRadian(float radian)
{
	float c, s;
	c = cosf(radian);
	s = sinf(radian);
	e[0] = 1;			e[4] = 0;			e[8] = 0;			e[12] = 0;
	e[1] = 0;			e[5] = c;			e[9] = -s;			e[13] = 0;
	e[2] = 0;			e[6] = s;			e[10] = c;			e[14] = 0;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}
void Matrix::setRotateYByRadian(float radian)
{
	float c, s;
	c = cosf(radian);
	s = sinf(radian);
	e[0] = c;			e[4] = 0;			e[8] = s;			e[12] = 0;
	e[1] = 0;			e[5] = 1;			e[9] = 0;			e[13] = 0;
	e[2] = -s;			e[6] = 0;			e[10] = c;			e[14] = 0;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}
void Matrix::setRotateZByRadian(float radian)
{
	float c, s;
	c = cosf(radian);
	s = sinf(radian);
	e[0] = c;			e[4] = -s;			e[8] = 0;			e[12] = 0;
	e[1] = s;			e[5] = c;			e[9] = 0;			e[13] = 0;
	e[2] = 0;			e[6] = 0;			e[10] = 1;			e[14] = 0;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}

/// Translate 행렬로 만들기
void Matrix::setTranslate(const float &x, const float &y, const float &z)
{
	e[0] = 1;			e[4] = 0;			e[8] = 0;			e[12] = x;
	e[1] = 0;			e[5] = 1;			e[9] = 0;			e[13] = y;
	e[2] = 0;			e[6] = 0;			e[10] = 1;			e[14] = z;
	e[3] = 0;			e[7] = 0;			e[11] = 0;			e[15] = 1;
}

/// View 행렬로 만들기
void Matrix::setViewMatrix(const Vector3 &position, const Vector3 &direction, const Vector3 &up)
{
	Vector3 right;
	right.cross(direction, up);

	Vector3 invDirection;
	invDirection.set(-direction.getX(), -direction.getY(), -direction.getZ());

	e[0] = right.getX();			e[4] = right.getY();			e[8] = right.getZ();			e[12] = - position.dot(right);
	e[1] = up.getX();				e[5] = up.getY();				e[9] = up.getZ();				e[13] = - position.dot(up);
	e[2] = invDirection.getX();		e[6] = invDirection.getY();		e[10] = invDirection.getZ();	e[14] = - position.dot(invDirection);
	e[3] = 0;						e[7] = 0;						e[11] = 0;						e[15] = 1;
}

/// 자신에다가 곱하기.. 자신 = matA x matB
void Matrix::multiply(const Matrix &matA, const Matrix &matB)
{
	for (int i=0; i<4; ++i)
	{
		for (int j=0; j<4; ++j)
		{
			float value = 0;

			for (int k=0; k<4; ++k)
			{
				value += matA.e[i + k*4] * matB.e[k + j*4];
			}

			e[i + j*4] = value;
		}
	}
}

/// 자신에다가 곱하기.. 자신 = 자신 x mat
void Matrix::multiply(const Matrix &mat)
{
	Matrix t = *this;
	multiply(t, mat);
}

/// 벡터와의 곱하기
void Matrix::multiply(Vector3 &result, const Vector3 &vec) const
{
	result.setX(e[0] * vec.getX() + e[4] * vec.getY() + e[8] * vec.getZ() + e[12]);
	result.setY(e[1] * vec.getX() + e[5] * vec.getY() + e[9] * vec.getZ() + e[13]);
	result.setZ(e[2] * vec.getX() + e[6] * vec.getY() + e[10] * vec.getZ() + e[14]);
}

/// 벡터와의 곱하기.. 이동 적용하지 않고
void Matrix::multiplyWithoutTranslate(Vector3 &result, const Vector3 &vec) const
{
	result.setX(e[0] * vec.getX() + e[4] * vec.getY() + e[8] * vec.getZ());
	result.setY(e[1] * vec.getX() + e[5] * vec.getY() + e[9] * vec.getZ());
	result.setZ(e[2] * vec.getX() + e[6] * vec.getY() + e[10] * vec.getZ());
}






































