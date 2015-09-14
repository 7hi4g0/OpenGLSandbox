#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

using std::cout;
using std::cerr;
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

struct Model {
	std::vector<Position> pos;
	std::vector<Position> normals;
	std::vector<unsigned int> quadIndices;
	std::vector<unsigned int> triIndices;
};

Model *loadObjModel(const char * const filename);
