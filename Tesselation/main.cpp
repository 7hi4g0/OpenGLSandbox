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
void setPipeline();

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Position;

typedef struct {
	GLfloat r;
	GLfloat g;
	GLfloat b;
} Color;

static GLuint program;

int main() {
	CreateWindow();

	XClearWindow(dpy, win);

	setPipeline();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Position rect[6] = {
		{-0.9f, 0.9f, 0.0f},
		{-0.9f, -0.9f, 0.0f},
		{0.9f, 0.9f, 0.0f},
		{0.9f, 0.9f, 0.0f},
		{-0.9f, -0.9f, 0.0f},
		{0.9f, -0.9f, 0.0f},
	};
	Color colors[6] = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	GLuint vao;
	GLuint vbo[2];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Position), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Color), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(program);

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		EndDraw();
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

void setPipeline() {
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint tessControlShader;
	GLuint tessEvalShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);

	setShader(vertexShader, "./shaders/tess.vert");
	setShader(fragmentShader, "./shaders/tess.frag");
	setShader(tessControlShader, "./shaders/tess.tesc");
	setShader(tessEvalShader, "./shaders/tess.tese");

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glAttachShader(program, tessControlShader);
	glAttachShader(program, tessEvalShader);

	glBindAttribLocation(program, 0, "vVertex");
	glBindAttribLocation(program, 1, "vColor");

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
}
