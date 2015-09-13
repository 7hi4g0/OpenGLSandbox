#include "matrix.h"

class Matrix4 : public Matrix{
public:
	Matrix4() : Matrix() {}
	Matrix4(const Matrix4& old) : Matrix(old) {}
	Matrix4& operator= (const Matrix4& right){
		Matrix::operator=(right);
		
		return (*this);
	}
	Matrix4 operator+ (const Matrix4& right) const{
		return Matrix::operator+(right);
	}
	Matrix4 operator* (const Matrix4& right) const{
		return Matrix::operator*(right);
	}
	Matrix4 operator* (const float right) const{
		return Matrix::operator*(right);
	}
	
	void identity();
	void translate(float x, float y, float z);
	void scale(float x, float y, float z);
	void rotate(float angle, float x, float y, float z);
	
	void setOrthographic(float xLeft, float xRight, float yDown, float yUp, float zNear, float zFar);
	void setFrustum(float xLeft, float xRight, float yDown, float yUp, float zNear, float zFar);
	void setPerspective(float angle, float aspect, float zNear, float zFar);
private:
	Matrix4(const Matrix& old) : Matrix(old) {}
	
	void degreeToRadian(float &angle);
};
