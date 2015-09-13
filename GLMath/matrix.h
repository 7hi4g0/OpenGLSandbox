#include <vector>
#include <iostream>

using std::vector;
using std::ostream;

class Matrix{
	friend ostream& operator<<(ostream& os, const Matrix& mat);
	friend Matrix operator* (const float left, const Matrix& right);
public:
	Matrix(const Matrix&);
	Matrix(unsigned int row = 4, unsigned int col = 0);
	virtual ~Matrix();
	Matrix& operator= (const Matrix& right);
	float& operator() (unsigned int row, unsigned int col);
	float operator() (unsigned int row, unsigned int col) const;
	float& operator[] (unsigned int index);
	float operator[] (unsigned int index) const;
	Matrix operator+ (const Matrix& right) const;
	Matrix operator* (const Matrix& right) const;
	Matrix operator* (const float right) const;
	
	void virtual pushMatrix(void);
	void virtual popMatrix(void);
protected:
	vector<float *> stack;
	float *values;
	unsigned int row;
	unsigned int col;
	unsigned int size;
	
	void deleteLast();
};
