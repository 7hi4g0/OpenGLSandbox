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
void setPipeline1();
void setPipeline2();
void getFunctionPointers();
KEY_PRESS(keyPress);

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
static int verbose;
static GLuint quadProgram;
static GLuint triProgram;
static GLint maxTessLevel;
static GLfloat outerLevel;
static GLfloat innerLevel;

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
PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
PFNGLUNIFORM1FVPROC					glUniform1fv;

int main(int argc, char *argv[]) {
	char opt;

	debug = 0;

	while ((opt = getopt(argc, argv, ":dv")) != -1) {
		switch (opt) {
			case 'd':
				debug += 1;
				break;
			case 'v':
				verbose += 1;
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

	TreatKeyPress = keyPress;

	CreateWindow();

	getFunctionPointers();

	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	if (verbose) {
		cout << "Max Tess Level: " << maxTessLevel << endl;
	}

	XClearWindow(dpy, win);

	setPipeline1();
	setPipeline2();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Position rect[7] = {
		{-0.9f, 0.4f, 0.0f},	//downleft
		{-0.9f, -0.4f, 0.0f},	//upleft
		{-0.1f, 0.4f, 0.0f},		//downright
		{-0.1f, -0.4f, 0.0f},	//upright
		{0.5f, 0.4f, 0.0f},
		{0.1f, -0.4f, 0.0f},
		{0.9f, -0.4f, 0.0f},
	};
	Color colors[7] = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
	};

	GLuint vao;
	GLuint vbo[2];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 7 * sizeof(Position), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 7 * sizeof(Color), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLint innerLevelUniform;
	GLint outerLevelUniform;

	innerLevel = 2.0f;
	outerLevel = 2.0f;

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(quadProgram);

		glPatchParameteri(GL_PATCH_VERTICES, 4);

		innerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelInner");
		outerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelOuter");

		glUniform1fv(innerLevelUniform, 1, &innerLevel);
		glUniform1fv(outerLevelUniform, 1, &outerLevel);

		glDrawArrays(GL_PATCHES, 0, 4);

		glUseProgram(triProgram);

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		innerLevelUniform = glGetUniformLocation(triProgram, "tessLevelInner");
		outerLevelUniform = glGetUniformLocation(triProgram, "tessLevelOuter");

		glUniform1fv(innerLevelUniform, 1, &innerLevel);
		glUniform1fv(outerLevelUniform, 1, &outerLevel);

		glDrawArrays(GL_PATCHES, 4, 3);

		EndDraw();

		msleep(33);
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

void setPipeline1() {
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
	setShader(tessControlShader, "./shaders/quad.tesc");
	setShader(tessEvalShader, "./shaders/quad.tese");
	setShader(geomShader, "./shaders/tess.geom");

	quadProgram = glCreateProgram();
	glAttachShader(quadProgram, vertexShader);
	glAttachShader(quadProgram, fragmentShader);
	glAttachShader(quadProgram, tessControlShader);
	glAttachShader(quadProgram, tessEvalShader);
	glAttachShader(quadProgram, geomShader);

	glLinkProgram(quadProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(geomShader);
}

void setPipeline2() {
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
	setShader(tessControlShader, "./shaders/tri.tesc");
	setShader(tessEvalShader, "./shaders/tri.tese");
	setShader(geomShader, "./shaders/tess.geom");

	triProgram = glCreateProgram();
	glAttachShader(triProgram, vertexShader);
	glAttachShader(triProgram, fragmentShader);
	glAttachShader(triProgram, tessControlShader);
	glAttachShader(triProgram, tessEvalShader);
	glAttachShader(triProgram, geomShader);

	glLinkProgram(triProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(geomShader);
}

KEY_PRESS(keyPress) {
	switch(XLookupKeysym(xkey, 0)) {
		case (XK_o):
			if (outerLevel < maxTessLevel) {
				outerLevel += 1;
			}
			if (verbose) {
				cout << "Current Outer Tess Level: " << outerLevel << endl;
			}
			break;
		case (XK_l):
			if (outerLevel > 1) {
				outerLevel -= 1;
			}
			if (verbose) {
				cout << "Current Outer Tess Level: " << outerLevel << endl;
			}
			break;
		case (XK_i):
			if (innerLevel < maxTessLevel) {
				innerLevel += 1;
			}
			if (verbose) {
				cout << "Current Inner Tess Level: " << innerLevel << endl;
			}
			break;
		case (XK_k):
			if (innerLevel > 1) {
				innerLevel -= 1;
			}
			if (verbose) {
				cout << "Current Inner Tess Level: " << innerLevel << endl;
			}
			break;
	};
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
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glXGetProcAddressARB( (const GLubyte *) "glGetUniformLocation");
	glUniform1fv = (PFNGLUNIFORM1FVPROC) glXGetProcAddressARB( (const GLubyte *) "glUniform1fv");
}
