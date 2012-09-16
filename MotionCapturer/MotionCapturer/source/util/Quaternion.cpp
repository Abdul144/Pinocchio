
#include "stdafx.h"
#include "Quaternion.h"

#include <math.h>

#include "Matrix.h"
#include "Vector3.h"


Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
}

void Quaternion::slerp(float t, const Quaternion& q0, const Quaternion& q1)
{
	float fcos = q0.dot(q1);	// = cos(еш) 
	float fAngle = acos(fcos); 

	if ( fabs(fAngle) < 0.0001 ) 
	{
		*this = q0;
		return;
	}

	float fsin = sin(fAngle); 
	float fInvsin = 1.0f/fsin; 
	float fCoeff0 = sin( (1.0f-t)*fAngle ) * fInvsin; 
	float fCoeff1 = sin( t*fAngle ) * fInvsin; 

	*this = q0*fCoeff0 + q1*fCoeff1;
}


void Quaternion::rotated(const Quaternion &a, const Quaternion &b)
{
    w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    y = a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z;
    z = a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x;
    normalize();
}


void Quaternion::scaled(float s, const Quaternion &q)
{
    set(q.x * s, q.y * s, q.z * s, q.w * s);
}


float Quaternion::dot(const Quaternion& q) const
{
    return x * q.x + y * q.y + z * q.z + w * q.w;
}


void Quaternion::getMatrix(Matrix &matrix) const
{
    const float s = 2;
    float xs, ys, zs;
    float wx, wy, wz;
    float xx, xy, xz;
    float yy, yz, zz;
    xs = x * s;  ys = y * s;  zs = z * s;
    wx = w * xs; wy = w * ys; wz = w * zs;
    xx = x * xs; xy = x * ys; xz = x * zs;
    yy = y * ys; yz = y * zs; zz = z * zs;

    float *m = (float*)matrix.getPointer();
    m[0] = 1 - (yy + zz);	m[4] = xy - wz;			m[8] = xz + wy;			m[12] = 0.f;
    m[1] = xy + wz;			m[5] = 1 - (xx + zz);	m[9] = yz - wx;			m[13] = 0.f;
    m[2] = xz - wy;			m[6] = yz + wx;			m[10] = 1 - (xx + yy);	m[14] = 0.f;
    m[3] = 0.f;				m[7] = 0.f;				m[11] = 0.f;			m[15] = 1.f;
}


Quaternion Quaternion::operator-(const Quaternion& q) const
{
    return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
}


Quaternion Quaternion::operator+(const Quaternion& q) const
{
    return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}


bool Quaternion::operator==(const Quaternion& q) const
{
    return x == q.x && y == q.y && z == q.z && w == q.w;
}


bool Quaternion::operator!=(const Quaternion& q) const
{
    return !(*this == q);
}

Quaternion Quaternion ::operator*(const float& value) const
{
	Quaternion q;

	q.set(x*value, y*value, z*value, w*value);
	return q;
}

// Compute the quaternion that rotates from a to b, avoiding numerical instability.
// Taken from "The Shortest Arc Quaternion" by Stan Melax in "Game Programming Gems".

void Quaternion::setFromVectors(const Vector3 &v0, const Vector3 &v1)
{
    if (v0 == -v1)
    {
    	Vector3 v;
		v.cross(v0, v1);
    	return setFromAxisAngle(v, 3.141592f);
    }

    Vector3 c;
    c.cross(v0, v1);
    float d = v0.dot(v1);
    float s = sqrt((1 + d) * 2);

    x = c.getX() / s;
    y = c.getY() / s;
    z = c.getZ() / s;
    w = s / 2.0f;
}


void Quaternion::setFromAxisAngle(const Vector3 &axis, float radians)
{
    w = cos(radians / 2);
    x = y = z = sin(radians / 2);
    x *= axis.getX();
    y *= axis.getY();
    z *= axis.getZ();
}


void Quaternion::normalize()
{
    scaled(1 / sqrt(dot(*this)), *this);
}

Quaternion Quaternion::operator*(const Quaternion& q2) const
{
    Quaternion q;
    const Quaternion& q1 = *this;

    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
    q.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;

    q.normalize();

    return q;
}




