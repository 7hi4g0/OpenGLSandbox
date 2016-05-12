#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

#include <glcommon.h>
#include <model.h>

struct KeyFrame {
	Model *model;
	ModelBuffer *modelBuffer;

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

	KeyFrame(const char * const);

	void genBuffers();
};

#endif //_KEYFRAME_H_
