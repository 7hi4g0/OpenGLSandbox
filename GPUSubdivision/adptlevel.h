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
};

struct AdaptiveBuffer {
	std::vector<Vertex> verts;
	std::vector<unsigned int> fullIndices;
	std::vector<unsigned int> levelRegularIndices;
	std::vector<unsigned int> levelIrregularIndices;
};

struct AdaptiveLevel {
	Model *mesh;
	AdaptiveBuffer *adaptiveBuffer;
	FaceSet subFaces;

	FaceSet levelFaces;
	FaceSet fullFaces;

	GLuint vertices;
	GLuint fullIndices;
	GLuint levelRegularIndices;
	GLuint levelIrregularIndices;

	GLuint vao;
	union {
		GLuint vbo[4];
		struct {
			GLuint posBuffer;
			GLuint fullIndexBuffer;
			GLuint levelRegularIndexBuffer;
			GLuint levelIrregularIndexBuffer;
		};
	};

	AdaptiveLevel();

	void addFaceRing(FacePtr);

	void genFaceSets();
	void genBuffers();
	void setBuffersData();
	AdaptiveBuffer *genAdaptiveBuffer();
};


#endif //_ADPTLEVEL_H_
