#pragma once

#include "../util/defines.h"


class Vector3
{
	// 멤버 변수
private:
	float x, y, z;
	

	// 멤버 함수
public:
	Vector3(float x = 0.f, float y = 0.f, float z = 0.f);

	void set(float x, float y, float z);

	/// 정규화
	void normalize();

	/// 내적
	float dot(const Vector3 &vec) const;

	/// 외적
	void cross(const Vector3 &a, const Vector3 &b);

	/// 길이 얻기
	inline float getDistance()
	{
		return sqrt(x*x + y*y + z*z);
	}


	// 접근
public:
	GETTER_SETTER(float, X, x)
	GETTER_SETTER(float, Y, y)
	GETTER_SETTER(float, Z, z)
};

