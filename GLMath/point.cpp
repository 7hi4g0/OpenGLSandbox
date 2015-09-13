#include <iostream>
#include "point.h"

using std::ostream;
using std::cout;
using std::endl;

ostream& operator<<(ostream& os, Point aPoint){
	os << "(";
	os << aPoint.xAxis << ", ";
	os << aPoint.yAxis << ", ";
	os << aPoint.zAxis << ")";
	
	return os;
}

Point operator*(int multiplier, Point& aPoint){
	return aPoint * multiplier;
}

Point::Point(int x, int y, int z){
	xAxis = x;
	yAxis = y;
	zAxis = z;
}

Point& Point::operator+(Point& aPoint){
	this->xAxis += aPoint.xAxis;
	this->yAxis += aPoint.yAxis;
	this->zAxis += aPoint.zAxis;
	
	return *this;
}

Point Point::operator*(int multiplier){
	return Point(this->xAxis * multiplier, this->yAxis * multiplier, this->zAxis * multiplier);
}

int& Point::operator[](int index){
	switch (index){
		case (0):
			return xAxis;
		case (1):
			return yAxis;
		case (2):
			return zAxis;
	}
}

int main(){
	Point aPoint(2, 3);
	Point bPoint = 3 * aPoint * 3;
	
	cout << *(&aPoint[0]) << endl;
	
	return 0;
}
