#include <iostream>
#include <cstring>
#include <cmath>
#include "matrix4.h"

using std::cout;
using std::cin;
using std::endl;

void Matrix4::identity(){
	memset(values, 0, 16 * sizeof(float));
	
	for (int i = 0; i < 4; i++){
		(*this)(i, i) = 1.0f;
	}
}

void Matrix4::translate(float x, float y, float z){
	Matrix4 mat;
	
	mat.identity();
	
	mat(0, 3) = x;
	mat(1, 3) = y;
	mat(2, 3) = z;
	
	(*this) = mat * (*this);
}

void Matrix4::scale(float x, float y, float z){
	Matrix4 mat;
	
	mat.identity();
	
	mat(0, 0) = x;
	mat(1, 1) = y;
	mat(2, 2) = z;
	
	(*this) = mat * (*this);
}

void Matrix4::rotate(float angle, float x, float y, float z){
	degreeToRadian(angle);
	
	float mag = sqrt(x*x + y*y + z*z);
	
	x /= mag;
	y /= mag;
	z /= mag;
	
	float cosseno = cos(angle);
	float seno = sin(angle);
	
	Matrix4 id;
	Matrix4 axis;
	
	id.identity();
	
	axis(0, 1) = -(axis(1, 0) = z);
	axis(2, 0) = -(axis(0, 2) = y);
	axis(1, 2) = -(axis(2, 1) = x);
	
	(*this) = (id + seno * axis + (1 - cosseno) * (axis * axis)) * (*this);
}

void Matrix4::degreeToRadian(float &angle){
	angle = angle * 3.14159265358979f / 180.0f;
}

void Matrix4::setOrthographic(float xLeft, float xRight, float yDown, float yUp, float zNear, float zFar){
	float xHalf = (xRight - xLeft) / 2.0f;
	float yHalf = (yUp - yDown) / 2.0f;
	float zHalf = (zFar - zNear) / 2.0f;
	
	(*this).identity();
	
	(*this)(0, 0) /= xHalf;
	(*this)(1, 1) /= yHalf;
	(*this)(2, 2) /= zHalf;
	(*this)(0, 3) = -xLeft / xHalf - 1.0f;
	(*this)(1, 3) = -yDown / yHalf - 1.0f;
	(*this)(2, 3) = -zNear / zHalf - 1.0f;
}

void Matrix4::setFrustum(float xLeft, float xRight, float yDown, float yUp, float zNear, float zFar){
	float xHalf = (xRight - xLeft) / 2.0f;
	float yHalf = (yUp - yDown) / 2.0f;
	float zHalf = (zFar - zNear) / 2.0f;
	
	(*this).identity();
	
	(*this)(0, 0) = zNear / xHalf;
	(*this)(1, 1) = zNear / yHalf;
	(*this)(0, 2) = -xLeft / xHalf - 1.0f;
	(*this)(1, 2) = -yDown / yHalf - 1.0f;
	(*this)(2, 2) = zNear / zHalf + 1.0f;
	(*this)(2, 3) = -(zNear * zFar / zHalf);
	(*this)(3, 2) = 1.0f;
	(*this)(3, 3) = 0.0f;
}

void Matrix4::setPerspective(float angle, float aspect, float zNear, float zFar){
	degreeToRadian(angle);
	
	float yUp = zNear * tan(angle / 2.0f);
	float yDown = -yUp;
	float xLeft = yDown * aspect;
	float xRight = -xLeft;
	
	setFrustum(xLeft, xRight, yDown, yUp, zNear, zFar);
}
/*
int main(){
	Matrix4 mat;
	int times;
	
	cin >> times;
	
	mat.identity();
	
	for (; times > 0; times--){
		mat.rotate(5, 1, 0, 0);
	
		cout << mat << endl;
	}
	
	return 0;
}*/
