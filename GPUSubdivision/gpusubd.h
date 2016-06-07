#ifndef _GPUSUBD_H_
#define _GPUSUBD_H_

#include <pipeline.h>
#include <adptlevel.h>
#include <subd.h>

struct AdaptiveSubSurf {
	std::vector<AdaptiveLevel*> subLevels;
	int levels;

	GLuint vertices;
	GLuint vertexValence;
	GLuint vertexValenceIndices;

	union {
		GLuint vbo[3];
		struct {
			GLuint verticesBuffer;
			GLuint valenceBuffer;
			GLuint valenceIndicesBuffer;
		};
	};

	GLuint facesProgram;
	GLuint edgesProgram;
	GLuint verticesProgram;

	VerticesBuffer *vertsBuffer;

	AdaptiveSubSurf(const char * const);

	void subdivide(int levels);
	void genBuffers();
	void genCatmullClarkTables();
	void gpuCatmullClark();

	void createShaders();
};

AdaptiveLevel *AdaptiveCatmullClark(AdaptiveLevel *, unsigned int);

#endif //_GPUSUBD_H_
