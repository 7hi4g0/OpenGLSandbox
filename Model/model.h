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
struct Vertex;
struct Edge;
struct Face;
struct Model;
struct ModelBuffer;
typedef std::shared_ptr<Position> PositionPtr;
typedef std::shared_ptr<Vertex> VertexPtr;
typedef std::shared_ptr<Edge> EdgePtr;
typedef std::shared_ptr<Face> FacePtr;

struct Face {
	unsigned int numVertices;
	PositionPtr pos[4];
	VertexPtr normals[4];
	EdgePtr edges[4];

	bool operator<(const Face f) const;

	Position facePos() const;
};
typedef std::set<FacePtr> FaceSet;

struct Edge {
	PositionPtr v0;
	PositionPtr v1;

	FacePtr faces[2];
	int faceCount;

	bool operator==(const Edge e) const;

	bool operator!=(const Edge e) const;
	
	bool operator<(const Edge e) const;

	Position edgePos() const;

	Position midPos() const;
};

struct EdgeCompare {
	bool operator() (const EdgePtr& e, const EdgePtr& f);
};

typedef std::set<EdgePtr, EdgeCompare> EdgeSet;

struct Vertex {
	float x;
	float y;
	float z;

	friend std::istream& operator>>(std::istream& is, Vertex& pos);

	friend std::ostream& operator<<(std::ostream& os, const Vertex& pos);

	bool operator==(const Vertex p);

	bool operator<(const Vertex p) const;

	Vertex operator+(const Vertex p) const;

	Vertex operator/(const float n) const;

	Vertex operator*(const float n) const;
};

typedef std::set<VertexPtr> VertexSet;

struct Position {
	Vertex v;

	EdgeSet edges;
	FaceSet faces;

	Position& operator=(const Position& right);

	bool operator==(const Position p);

	bool operator<(const Position p) const;

	Position operator+(const Position p) const;

	Position operator/(const float n) const;

	Position operator*(const float n) const;

	Position vertexPos() const;
};

struct PositionCompare {
	bool operator() (const PositionPtr& p, const PositionPtr& q);
};

typedef std::set<PositionPtr, PositionCompare> PositionSet;

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

	ModelBuffer *genBuffer();
};

struct ModelBuffer {
	std::vector<Vertex> pos;
	std::vector<Vertex> normals;
	std::vector<unsigned int> quadIndices;
	std::vector<unsigned int> triIndices;
};

Model *loadObjModel(const char * const filename);

#endif //_MODEL_H_
