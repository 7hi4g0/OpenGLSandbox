#include <glcommon.h>

void checkGLErr(const char *file, int line) {
	GLenum glErr;

	glErr = glGetError();

	if (glErr != GL_NO_ERROR) {
		cerr << file << "," << line << " - OpenGL Error: " << glErr << endl;
		exit(1);
	}
}
