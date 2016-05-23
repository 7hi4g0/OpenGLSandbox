#ifndef _ADPTLEVEL_H_
#define _ADPTLEVEL_H_

// TODO: Will I nedd it?
#include <algorithm>
#include <iterator>

#include <glcommon.h>
#include <model.h>

extern int verbose;
extern int debug;

struct AdaptiveLevel {
	Model *level;
	ModelBuffer *modelBuffer;
	FaceSet subFaces;

	FaceSet levelFaces;
	FaceSet fullFaces;

	GLuint vertices;
	GLuint quadIndices;
	GLuint triIndices;

	GLuint vao;
	union {
		GLuint vbo[4];
		struct {
			GLuint posBuffer;
			GLuint normBuffer;
			GLuint quadIndexBuffer;
			GLuint triIndexBuffer;
		};
	};

	AdaptiveLevel();

	void addFaceRing(FacePtr);
	void genBuffers();
	ModelBuffer *genBuffer(bool smooth);
};

#endif //_ADPTLEVEL_H_
