#ifndef _ADPTLEVEL_H_
#define _ADPTLEVEL_H_

#include <glcommon.h>
#include <model.h>

extern int verbose;
extern int debug;

struct DirFace {
	FacePtr face;
	unsigned int index;

	DirFace();
	DirFace(FacePtr, unsigned int);

	DirFace adjFace();
	DirFace nextEdge();
	DirFace prevEdge();
	PositionPtr vert();
	PositionPtr next();
	PositionPtr diag();
	PositionPtr prev();
};

struct VerticesBuffer {
	std::vector<Vertex> verts;
	std::vector<unsigned int> vertexValence;
	std::vector<unsigned int> vertexValenceIndices;
};

struct AdaptiveBuffer {
	std::vector<unsigned int> fullIndices;
	std::vector<unsigned int> levelRegularIndices;
	std::vector<unsigned int> levelIrregularIndices;
};

struct TopologyBuffer {
	std::vector<unsigned int> adjacency;
	std::vector<unsigned int> adjacencyIndices;
};

struct AdaptiveLevel {
	Model *mesh;
	AdaptiveBuffer *adaptiveBuffer;
	TopologyBuffer *topologyBuffer;
	FaceSet subFaces;
	EdgeSet subEdges;
	PositionSet subVertices;

	FaceSet levelFaces;
	FaceSet fullFaces;

	GLuint fullIndices;
	GLuint levelRegularIndices;
	GLuint levelIrregularIndices;
	GLuint adjacency;
	GLuint adjacencyIndices;

	GLuint vao;
	union {
		GLuint vbo[7];
		struct {
			GLuint posBuffer;
			GLuint fullIndexBuffer;
			GLuint levelRegularIndexBuffer;
			GLuint levelIrregularIndexBuffer;
			GLuint vertexValenceIndexBuffer;
			GLuint adjacencyBuffer;
			GLuint adjacencyIndicesBuffer;
		};
	};

	AdaptiveLevel();
	~AdaptiveLevel();

	void addFace(FacePtr);
	void addFaceRing(FacePtr);

	void genFaceSets();
	void genBuffers(GLuint, GLuint);
	void setBuffersData(VerticesBuffer *);
	AdaptiveBuffer *genAdaptiveBuffer(VerticesBuffer *);
	void genCatmullClarkTable();
	void genTopologyBuffers();
	void setTopologyBuffers();
};


#endif //_ADPTLEVEL_H_
