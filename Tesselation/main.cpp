#include <winsys.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;

std::string readFile(const char * const filename);
void setShader(GLuint shader, const char * sourceFile);

int main() {
	CreateWindow();

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint tessControlShader;
	GLuint tessEvalShader;
	GLuint program;
	GLint testVal;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);

	setShader(vertexShader, "./shaders/tess.vert");
	setShader(fragmentShader, "./shaders/tess.frag");
	setShader(tessControlShader, "./shaders/tess.tesc");
	setShader(tessEvalShader, "./shaders/tess.tese");

	XClearWindow(dpy, win);

	loop = true;
	while(loop) {
		TreatEvents();

	}

	DestroyWindow();
	return 0;
}

std::string readFile(const char * const filename) {
	ifstream file;
	stringstream contents;

	file.open(filename);
	contents << file.rdbuf();

	return contents.str();
}

void setShader(GLuint shader, const char * sourceFile) {
	GLint compileStatus;
	const char * source;

	source = readFile(sourceFile).c_str();

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		cout << "Shader Compilation Error" << endl;

		char log[5000];
		glGetShaderInfoLog(shader, 5000, NULL, log);

		cout << log << endl;
		exit(1);
	}
}
