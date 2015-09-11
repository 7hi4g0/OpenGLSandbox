#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;

struct Position {
	float x;
	float y;
	float z;

	friend std::istream& operator>>(std::istream& is, Position& pos) {
		is >> pos.x >> pos.y >> pos.z;
	}

	friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
		os << pos.x << " " << pos.y << " " << pos.z;
	}
};

void loadObjModel(const char * const filename);
