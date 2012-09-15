#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "defines.h"


class Matrix;
class Vector3;


class Quaternion
{
	// 멤버 변수
private:
    float x;
    float y;
    float z;
    float w;
    

    // 멤버 함수
public:
    Quaternion(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
    
    inline void set(float x, float y, float z, float w)
    {
    	this->x = x;
    	this->y = y;
    	this->z = z;
    	this->w = w;
    }

    void slerp(float mu, const Quaternion &a, const Quaternion &b);
    void rotated(const Quaternion &a, const Quaternion &b);
    void scaled(float scale, const Quaternion &q);
    float dot(const Quaternion& q) const;
    void getMatrix(Matrix &matrix) const;
    Quaternion operator-(const Quaternion& q) const;
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator*(const Quaternion& q) const;
    bool operator==(const Quaternion& q) const;
    bool operator!=(const Quaternion& q) const;
    
    void normalize();
    
    void setFromVectors(const Vector3 &v0, const Vector3 &v1);
    void setFromAxisAngle(const Vector3 &axis, float radians);


	// 접근
	GETTER_SETTER(float, X, x)
	GETTER_SETTER(float, Y, y)
	GETTER_SETTER(float, Z, z)
	GETTER_SETTER(float, W, w)
};



#endif


























