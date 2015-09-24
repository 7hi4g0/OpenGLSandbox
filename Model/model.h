#ifndef _MODEL_H_
#define _MODEL_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;

struct Position;
struct Edge;
struct Face;
struct Model;
struct ModelBuffer;

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

	bool operator==(const Position p) {
		return x == p.x && y == p.y && z == p.z;
	}

	Position operator+(const Position p) const {
		Position ret = *this;

		ret.x += p.x;
		ret.y += p.y;
		ret.z += p.z;

		return ret;
	}

	Position operator/(const float n) const {
		Position ret = *this;

		ret.x /= n;
		ret.y /= n;
		ret.z /= n;

		return ret;
	}
};
typedef std::shared_ptr<Position> PositionPtr;

struct Edge {
	PositionPtr v0;
	PositionPtr v1;

	bool operator==(Edge e) {
		return (e.v0 == v0 && e.v1 == v1) || (e.v0 == v1 && e.v1 == v0);
	}

	bool operator!=(Edge e) {
		return (e.v0 != v0 || e.v1 != v1) && (e.v0 != v1 || e.v1 != v0);
	}
	
	bool operator<(const Edge e) const {
		return v0 < e.v0;
	}
};
typedef std::shared_ptr<Edge> EdgePtr;

struct Face {
	unsigned int numVertices;
	EdgePtr edges[4];
	PositionPtr pos[4];
	PositionPtr normals[4];

	bool operator<(const Face f) const {
		return numVertices < f.numVertices || (numVertices == f.numVertices && pos[0].get() < f.pos[0].get());
	}
};
typedef std::shared_ptr<Face> FacePtr;

struct Model {
	//TODO: Do I need position, normals and edges?
	std::vector<PositionPtr> pos;
	std::vector<PositionPtr> normals;
	std::set<EdgePtr> edges;
	std::set<FacePtr> faces;

	ModelBuffer *genBuffer();
};

struct ModelBuffer {
	std::vector<Position> pos;
	std::vector<Position> normals;
	std::vector<unsigned int> quadIndices;
	std::vector<unsigned int> triIndices;
};

Model *loadObjModel(const char * const filename);

#endif //_MODEL_H_
