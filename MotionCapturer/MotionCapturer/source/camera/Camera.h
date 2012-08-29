#pragma once

#include "../util/defines.h"
#include "../util/Matrix.h"
#include "../util/Vector3.h"


class Camera
{
	// 멤버 변수
private:
	Vector3 position;		///< 위치

	Vector3 baseRight;		///< 기본 우 벡터
	Vector3 baseUp;			///< 기본 위 벡터
	Vector3 baseDirection;	///< 기본 방향 벡터

	Vector3 rotation;		///< 기본 축에서의 회전값(radian).. pitch, yaw, roll.. 오일러 순서로 적용.. x,y,z 축이 아닌 right,up,direction

	Vector3 right;			///< 기본으로부터 rotation으로 회전변환된 우 벡터
	Vector3 up;				///< 기본으로부터 rotation으로 회전변환된 위 벡터
	Vector3 direction;		///< 기본으로부터 rotation으로 회전변환된 방향 벡터

	Matrix viewMat;
	bool invalid;
	

	// 멤버 함수
private:
	void updateViewMatrix();

public:
	Camera();
	~Camera();

	void applyViewMatrix();

	/// 회전
	inline void pitch(float radian)
	{
		rotation.setX(rotation.getX() + radian);
		invalidate();
	}

	inline void yaw(float radian)
	{
		rotation.setY(rotation.getY() + radian);
		invalidate();
	}

	inline void roll(float radian)
	{
		rotation.setZ(rotation.getZ() + radian);
		invalidate();
	}

	/// 평행 이동
	void moveForward(float distance);
	void moveUp(float distance);
	void moveRight(float distance);

	/// 기본 축 변경
	void setBaseAxises(const Vector3 &direction, const Vector3 &up);

	/// view matrix 얻기
	const Matrix& getViewMatrix();

	/// 갱신하도록 상태 수정
	inline void invalidate()
	{
		invalid = true;
	}

	/// 위치 수정
	inline void setPosition(const Vector3 &position)
	{
		this->position = position;
		invalidate();
	}

	/// 각도 수정
	inline void setRotation(const Vector3 &rotation)
	{
		this->rotation = rotation;
		invalidate();
	}

	// 접근
public:
	GETTER_CONST_REF(Vector3, Position, position)
	GETTER_CONST_REF(Vector3, BaseRight, baseRight)
	GETTER_CONST_REF(Vector3, BaseUp, baseUp)
	GETTER_CONST_REF(Vector3, BaseDirection, baseDirection)
	GETTER_CONST_REF(Vector3, Rotation, rotation)
	GETTER_CONST_REF(Vector3, Right, right)
	GETTER_CONST_REF(Vector3, Up, up)
	GETTER_CONST_REF(Vector3, Direction, direction)
	GETTER_CONST_REF(Matrix, ViewMatrix, viewMat)
};

