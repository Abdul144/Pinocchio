
#include "stdafx.h"
#include "Vector3.h"



Vector3::Vector3(float x, float y, float z)
{
	set(x, y, z);
}

void Vector3::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

/// 정규화
void Vector3::normalize()
{
	float d = getDistance();
	x /= d;
	y /= d;
	z /= d;
}

/// 내적
float Vector3::dot(const Vector3 &vec) const
{
	return x*vec.x + y*vec.y + z*vec.z;
}

/// 외적
void Vector3::cross(const Vector3 &a, const Vector3 &b)
{
	x = a.y*b.z - a.z*b.y;
	y = a.z*b.x - a.x*b.z;
	z = a.x*b.y - a.y*b.x;
}

const Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

const Vector3 Vector3::operator + (const Vector3 &v) const
{
	Vector3 result;
	
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;

	return result;
}

const Vector3 Vector3::operator - (const Vector3 &v) const
{
	Vector3 result;
	
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;

	return result;
}

const Vector3 Vector3::operator * (const float &value) const
{
	Vector3 result;
	
	result.x = x * value;
	result.y = y * value;
	result.z = z * value;

	return result;
}

const Vector3 Vector3::operator / (const float &value) const
{
	Vector3 result;
	
	result.x = x / value;
	result.y = y / value;
	result.z = z / value;

	return result;
}

const Vector3 Vector3::operator += (const Vector3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

const Vector3 Vector3::operator -= (const Vector3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

bool Vector3::operator==(const Vector3& v) const
{
    return x == v.x && y == v.y && z == v.z;
}