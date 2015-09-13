#include <iostream>
#include <cstring>
#include "matrix.h"

using std::cout;
using std::endl;
using std::min;

ostream& operator<<(ostream& os, const Matrix& mat){
	for (unsigned int x = 0; x < mat.row; x++){
		for (unsigned int y = 0; y < mat.col; y++){
			os << mat(x, y) << ", ";
		}
		os << "\b\b " << endl;
	}
	
	return os;
}

Matrix operator* (const float left, const Matrix& right){
	return right * left;
}

Matrix::Matrix(const Matrix& old):
row(old.row),
col(old.col),
size(old.size)
{
	values = new float[size];
	
	stack.push_back(values);
	memcpy(values, old.values, size * sizeof(float));
}

Matrix::Matrix(unsigned int row, unsigned int col){
	if (col == 0)
		col = row;
	
	size = row * col;
	
	values = new float[size];
	memset(values, 0, size * sizeof(float));
	
	stack.push_back(values);
	this->row = row;
	this->col = col;
}

Matrix::~Matrix(){
	while (stack.size() > 0)
		deleteLast();
}

Matrix& Matrix::operator= (const Matrix& right){	
	memcpy(values, right.values, size * sizeof(float));
	
	return (*this);
}

float& Matrix::operator() (unsigned int row, unsigned int col){
	return values[col*this->row + row];
}

float Matrix::operator() (unsigned int row, unsigned int col) const{
	return values[col*this->row + row];
}

float& Matrix::operator[] (unsigned int index){
	return values[index];
}

float Matrix::operator[] (unsigned int index) const{
	return values[index];
}

Matrix Matrix::operator+ (const Matrix& right) const{
	Matrix result(row, col);
	
	for (unsigned int i = 0; i < size; i++){
		result[i] = (*this)[i] + right[i];
	}
	
	return result;
}

Matrix Matrix::operator* (const Matrix& right) const{
	Matrix result(row, right.col);
	
	for (int rc = 0; rc < right.col; rc++){
		for (int x = 0; x < row; x++){
			for (int y = 0; y < col; y++){
				result(x, rc) += (*this)(x, y) * right(y, rc);
			}
		}
	}
	
	return result;
}

Matrix Matrix::operator* (const float right) const{
	Matrix result(row, col);
	
	for (int i = 0; i < size; i++)
		result[i] = (*this)[i] * right;
	
	return result;
}

void Matrix::pushMatrix(){
	float *newValues = new float[size];
	
	memcpy(newValues, values, size * sizeof(float));
	
	stack.push_back(newValues);
	
	values = newValues;
}

void Matrix::popMatrix(){
	if (stack.size() > 1){
		deleteLast();
	}
}

void Matrix::deleteLast(){
	delete[] values;
	
	stack.pop_back();
	
	if (stack.size() > 0)
		values = stack.back();
	else
		values = NULL;
}
