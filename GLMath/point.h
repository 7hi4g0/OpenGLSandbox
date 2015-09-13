#include <iostream>

using std::ostream;

class Point{
	friend ostream& operator<<(ostream& os, Point aPoint);
	friend Point operator*(int multiplier, Point& aPoint);
	
public:
	Point(int x = 0, int y = 0, int z = 0);
	Point& operator+(Point& aPoint);
	Point operator*(int multiplier);
	int& operator[](int index);
	
private:
	int xAxis;
	int yAxis;
	int zAxis;
};
