
#ifndef GJMATRIX_H_
#define GJMATRIX_H_

#include <float.h>


class Vector3;

/**
 * @author 김종헌
 * @date 2012.02.06
 * @brief 4x4 행렬
 *
 * [0][4][8][12]
 * [1][5][9][13]
 * [2][6][10][14]
 * [3][7][11][15]
 */
class Matrix
{
	// 멤버 변수
private:
	float e[16];		///< 원소


	// 멤버 함수
public:
	/// 생성자
	Matrix();

	/// 복사 생성자
	Matrix(const Matrix &mat);

	/// 단위 행렬로 만들기
	void setIdentity();

	/// Scale 행렬로 만들기
	void setScale(const float &scaleX, const float &scaleY, const float &scaleZ);

	/// Rotate 행렬로 만들기
	void setRotateX(const float &degree);
	void setRotateY(const float &degree);
	void setRotateZ(const float &degree);

	/// Rotate 행렬로 만들기
	void setRotateXByRadian(float radian);
	void setRotateYByRadian(float radian);
	void setRotateZByRadian(float radian);

	/// Translate 행렬로 만들기
	void setTranslate(const float &x, const float &y, const float &z);

	/// View 행렬로 만들기
	void setViewMatrix(const Vector3 &position, const Vector3 &direction, const Vector3 &up);

	/// 자신에다가 곱하기.. 자신 = matA x matB
	void multiply(const Matrix &matA, const Matrix &matB);

	/// 자신에다가 곱하기.. 자신 = 자신 x mat
	void multiply(const Matrix &mat);

	/// 벡터와의 곱하기
	void multiply(Vector3 &result, const Vector3 &vec) const;

	/// 벡터와의 곱하기.. 이동 적용하지 않고
	void multiplyWithoutTranslate(Vector3 &result, const Vector3 &vec) const;

	/// 역행렬 얻기
	bool getInverse(Matrix &result);

	/// 원소 접근
	inline const float get(int i, int j) const
	{
		return e[(j<<2) + i];
	}
	inline void set(int i, int j, const float &value)
	{
		e[(j<<2) + i] = value;
	}
	void set(int column, const Vector3 &vector, float w);


	// 접근
	inline const float* getPointer() const {	return e;	}
};


#endif /* GJMATRIX_H_ */































