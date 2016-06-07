#ifndef _MODEL_H_
#define _MODEL_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <memory>
#include <cstdlib>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;

struct Position;
struct Vertex;
struct Edge;
struct Face;
struct Model;
struct ModelBuffer;
typedef Position* PositionPtr;
typedef Vertex* VertexPtr;
typedef Edge* EdgePtr;
typedef Face* FacePtr;

struct Vertex {
	float x;
	float y;
	float z;
	float pad;

	friend std::istream& operator>>(std::istream&, Vertex&);
	friend std::ostream& operator<<(std::ostream&, const Vertex&);

	bool operator==(const Vertex);
	bool operator<(const Vertex) const;
	Vertex operator+(const Vertex) const;
	Vertex operator-(const Vertex) const;
	Vertex operator/(const float) const;
	Vertex operator*(const float) const;

	Vertex cross(const Vertex) const;
	Vertex normalize() const;
	static Vertex normal(const Vertex&, const Vertex&, const Vertex&);
};

struct VertexHash {
	size_t operator() (const VertexPtr) const;
};
struct VertexEquality {
	bool operator() (const VertexPtr&, const VertexPtr&) const;
};
typedef std::unordered_set<VertexPtr, VertexHash, VertexEquality> VertexSet;

struct Face {
	unsigned int numVertices = 0;
	PositionPtr pos[4];
	VertexPtr normals[4];
	EdgePtr edges[4];

	unsigned int newIdx;

	bool tagged = false;
	bool wasTagged = true;

	bool operator<(const Face) const;

	Vertex facePos() const;

	bool isRegular() const;
};
typedef std::unordered_set<FacePtr> FaceSet;

struct Edge {
	PositionPtr v0;
	PositionPtr v1;

	unsigned int newIdx;

	FacePtr faces[2];
	int faceCount;

	Edge(PositionPtr, PositionPtr);

	bool operator==(const Edge) const;
	bool operator!=(const Edge) const;
	bool operator<(const Edge) const;

	PositionPtr other(PositionPtr) const;

	Vertex edgePos() const;
	Vertex midPos() const;
};

struct EdgeHash {
	size_t operator() (const EdgePtr) const;
};
struct EdgeEquality {
	bool operator() (const EdgePtr&, const EdgePtr&) const;
};
typedef std::unordered_set<EdgePtr, EdgeHash, EdgeEquality> EdgeSet;

struct Position {
	Vertex v;

	unsigned int idx;
	unsigned int newIdx;

	EdgeSet edges;
	FaceSet faces;

	Position& operator=(const Position&);
	Position& operator=(const Vertex&);
	bool operator==(const Position);
	bool operator<(const Position) const;
	Position operator+(const Position) const;
	Position operator-(const Position) const;
	Position operator/(const float) const;
	Position operator*(const float) const;

	Vertex vertexPos() const;
	Vertex vertexNormal() const;

	bool isExtraordinary() const;
};

struct PositionHash {
	size_t operator() (const PositionPtr) const;
};
struct PositionEquality {
	bool operator() (const PositionPtr&, const PositionPtr&) const;
};
typedef std::unordered_set<PositionPtr, PositionHash, PositionEquality> PositionSet;

/* Model
 *
 * pos		-	Holds coordinates and topology information.
 * normals	-	Holds normals coordinates.
 * 				Set ensures that is not used more memory than necessary.
 * edges	-	Holds edge information.
 * 				Used to ensure no duplicate equivalents.
 * faces	-	Holds face information.
 * 				Used to produce ModelBuffer, the render information.
 *
 */ 
struct Model {
	//STUDY: Do I need position, normals and edges? Can I use a simpler structure?
	PositionSet pos;
	VertexSet normals;
	EdgeSet edges;
	FaceSet faces;

	ModelBuffer *genBuffer(bool = false);
};

struct ModelBuffer {
	std::vector<Vertex> pos;
	std::vector<Vertex> normals;
	std::vector<unsigned int> quadIndices;
	std::vector<unsigned int> triIndices;
};

Model *loadObjModel(const char * const filename);

#endif //_MODEL_H_
