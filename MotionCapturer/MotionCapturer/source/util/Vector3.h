#pragma once

#include "../util/defines.h"


class Vector3
{
	// ��� ����
private:
	float x, y, z;
	

	// ��� �Լ�
public:
	Vector3(float x = 0.f, float y = 0.f, float z = 0.f);

	void set(float x, float y, float z);

	/// ����ȭ
	void normalize();

	/// ����
	float dot(const Vector3 &vec) const;

	/// ����
	void cross(const Vector3 &a, const Vector3 &b);

	/// ���� ���
	inline float getDistance()
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	const Vector3 operator + (const Vector3 &v) const;
	const Vector3 operator - (const Vector3 &v) const;
	const Vector3 operator * (const float &value) const;
	const Vector3 operator / (const float &value) const;



	// ����
public:
	GETTER_SETTER(float, X, x)
	GETTER_SETTER(float, Y, y)
	GETTER_SETTER(float, Z, z)
};

