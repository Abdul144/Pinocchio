
#ifndef GJMATRIX_H_
#define GJMATRIX_H_

#include <float.h>


class Vector3;

/**
 * @author ������
 * @date 2012.02.06
 * @brief 4x4 ���
 *
 * [0][4][8][12]
 * [1][5][9][13]
 * [2][6][10][14]
 * [3][7][11][15]
 */
class Matrix
{
	// ��� ����
private:
	float e[16];		///< ����


	// ��� �Լ�
public:
	/// ������
	Matrix();

	/// ���� ������
	Matrix(const Matrix &mat);

	/// ���� ��ķ� �����
	void setIdentity();

	/// Scale ��ķ� �����
	void setScale(const float &scaleX, const float &scaleY, const float &scaleZ);

	/// Rotate ��ķ� �����
	void setRotateX(const float &degree);
	void setRotateY(const float &degree);
	void setRotateZ(const float &degree);

	/// Rotate ��ķ� �����
	void setRotateXByRadian(float radian);
	void setRotateYByRadian(float radian);
	void setRotateZByRadian(float radian);

	/// Translate ��ķ� �����
	void setTranslate(const float &x, const float &y, const float &z);

	/// View ��ķ� �����
	void setViewMatrix(const Vector3 &position, const Vector3 &direction, const Vector3 &up);

	/// �ڽſ��ٰ� ���ϱ�.. �ڽ� = matA x matB
	void multiply(const Matrix &matA, const Matrix &matB);

	/// �ڽſ��ٰ� ���ϱ�.. �ڽ� = �ڽ� x mat
	void multiply(const Matrix &mat);

	/// ���Ϳ��� ���ϱ�
	void multiply(Vector3 &result, const Vector3 &vec) const;

	/// ���Ϳ��� ���ϱ�.. �̵� �������� �ʰ�
	void multiplyWithoutTranslate(Vector3 &result, const Vector3 &vec) const;

	/// ����� ���
	bool getInverse(Matrix &result);

	/// ���� ����
	inline const float get(int i, int j) const
	{
		return e[(j<<2) + i];
	}
	inline void set(int i, int j, const float &value)
	{
		e[(j<<2) + i] = value;
	}
	void set(int column, const Vector3 &vector, float w);


	// ����
	inline const float* getPointer() const {	return e;	}
};


#endif /* GJMATRIX_H_ */































