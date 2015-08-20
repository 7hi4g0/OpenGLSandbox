#include <winsys.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::stringstream;

std::string readFile(const char * const filename);
void setShader(GLuint shader, const char * sourceFile);
void setPipeline();
void getFunctionPointers();

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

static int debug;
static GLuint program;

PFNGLGENVERTEXARRAYSPROC			glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC			glBindVertexArray;
PFNGLGENBUFFERSPROC					glGenBuffers;
PFNGLBINDBUFFERPROC					glBindBuffer;
PFNGLBUFFERDATAPROC					glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray;
PFNGLUSEPROGRAMPROC					glUseProgram;
PFNGLSHADERSOURCEPROC				glShaderSource;
PFNGLCOMPILESHADERPROC				glCompileShader;
PFNGLGETSHADERIVPROC				glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
PFNGLCREATESHADERPROC				glCreateShader;
PFNGLCREATEPROGRAMPROC				glCreateProgram;
PFNGLATTACHSHADERPROC				glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation;
PFNGLLINKPROGRAMPROC				glLinkProgram;
PFNGLDELETESHADERPROC				glDeleteShader;
PFNGLPATCHPARAMETERIPROC			glPatchParameteri;
PFNGLPATCHPARAMETERFVPROC			glPatchParameterfv;

int main(int argc, char *argv[]) {
	char opt;

	debug = 0;

	while ((opt = getopt(argc, argv, ":d")) != -1) {
		switch (opt) {
			case 'd':
				debug += 1;
				break;
			case ':':
				fprintf(stderr, "%c needs an argument\n", optopt);
				exit(-1);
				break;
			default:
				fprintf(stderr, "Unknown option %c\n", optopt);
				exit(-1);
				break;
		}
	}

	CreateWindow();

	getFunctionPointers();

	XClearWindow(dpy, win);

	setPipeline();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Position rect[4] = {
		{-0.9f, 0.9f, 0.0f},	//downleft
		{-0.9f, -0.9f, 0.0f},	//upleft
		{0.9f, 0.9f, 0.0f},		//downright
		{0.9f, -0.9f, 0.0f},	//upright
	};
	Color colors[4] = {
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
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Position), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Color), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(program);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	GLfloat innerLevel[] = {2.0f, 2.0f};
	GLfloat outerLevel[] = {2.0f, 2.0f, 2.0f, 2.0f};

	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevel);
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevel);

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_PATCHES, 0, 4);

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
	std::string source;
	const char * cSource;
	int length;

	source = readFile(sourceFile);
	length = source.length();
	cSource = source.c_str();

	if (debug) {
		cerr << source << endl;
	}

	glShaderSource(shader, 1, &cSource, &length);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		cerr << "Shader Compilation Error" << endl;

		char log[5000];
		glGetShaderInfoLog(shader, 5000, NULL, log);

		cerr << log << endl;
		exit(1);
	}
}

void setPipeline() {
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint tessControlShader;
	GLuint tessEvalShader;
	GLuint geomShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	geomShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(vertexShader, "./shaders/tess.vert");
	setShader(fragmentShader, "./shaders/tess.frag");
	setShader(tessControlShader, "./shaders/tess.tesc");
	setShader(tessEvalShader, "./shaders/tess.tese");
	setShader(geomShader, "./shaders/tess.geom");

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glAttachShader(program, tessControlShader);
	glAttachShader(program, tessEvalShader);
	glAttachShader(program, geomShader);

	glBindAttribLocation(program, 0, "vVertex");
	glBindAttribLocation(program, 1, "vColor");

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(geomShader);
}

void getFunctionPointers() {
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) glXGetProcAddressARB( (const GLubyte *) "glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) glXGetProcAddressARB( (const GLubyte *) "glBindVertexArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC) glXGetProcAddressARB( (const GLubyte *) "glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) glXGetProcAddressARB( (const GLubyte *) "glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) glXGetProcAddressARB( (const GLubyte *) "glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) glXGetProcAddressARB( (const GLubyte *) "glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) glXGetProcAddressARB( (const GLubyte *) "glEnableVertexAttribArray");
	glUseProgram = (PFNGLUSEPROGRAMPROC) glXGetProcAddressARB( (const GLubyte *) "glUseProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC) glXGetProcAddressARB( (const GLubyte *) "glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) glXGetProcAddressARB( (const GLubyte *) "glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glXGetProcAddressARB( (const GLubyte *) "glGetShaderInfoLog");
	glCreateShader = (PFNGLCREATESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glCreateShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) glXGetProcAddressARB( (const GLubyte *) "glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) glXGetProcAddressARB( (const GLubyte *) "glBindAttribLocation");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) glXGetProcAddressARB( (const GLubyte *) "glLinkProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glDeleteShader");
	glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) glXGetProcAddressARB( (const GLubyte *) "glPatchParameteri");
	glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC) glXGetProcAddressARB( (const GLubyte *) "glPatchParameterfv");
}
