#include <winsys.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

#define	GLERR()	checkGLErr(__FILE__, __LINE__)

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::stringstream;

std::string readFile(const char * const filename);
void setShader(GLuint shader, const char * sourceFile);
void validatePipeline(GLint pipeline);
void setPipeline();
void getFunctionPointers();
void checkGLErr(const char *file, int line);
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
static GLuint pipeline;
static GLuint vertProgram;
static GLuint fragProgram;
static GLuint quadProgram;
static GLuint triProgram;
static GLuint lineProgram;
static GLuint solidProgram;
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
PFNGLSHADERSOURCEPROC				glShaderSource;
PFNGLCOMPILESHADERPROC				glCompileShader;
PFNGLGETSHADERIVPROC				glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC				glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
PFNGLGETPROGRAMPIPELINEIVPROC		glGetProgramPipelineiv;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC	glGetProgramPipelineInfoLog;
PFNGLVALIDATEPROGRAMPIPELINEPROC	glValidateProgramPipeline;
PFNGLCREATESHADERPROC				glCreateShader;
PFNGLCREATEPROGRAMPROC				glCreateProgram;
PFNGLATTACHSHADERPROC				glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation;
PFNGLLINKPROGRAMPROC				glLinkProgram;
PFNGLDELETESHADERPROC				glDeleteShader;
PFNGLUSEPROGRAMSTAGESPROC			glUseProgramStages;
PFNGLGENPROGRAMPIPELINESPROC		glGenProgramPipelines;
PFNGLBINDPROGRAMPIPELINEPROC		glBindProgramPipeline;
PFNGLPROGRAMPARAMETERIPROC			glProgramParameteri;
PFNGLPATCHPARAMETERIPROC			glPatchParameteri;
PFNGLPATCHPARAMETERFVPROC			glPatchParameterfv;
PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
PFNGLPROGRAMUNIFORM1FVPROC			glProgramUniform1fv;

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

	setPipeline();	GLERR();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Position rect[7] = {
		{-0.9f, 0.4f, 0.0f},	//upleft
		{-0.9f, -0.4f, 0.0f},	//downleft
		{-0.1f, -0.4f, 0.0f},	//downright
		{-0.1f, 0.4f, 0.0f},	//upright
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

	glBindProgramPipeline(pipeline);	GLERR();

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, quadProgram);	GLERR();
		validatePipeline(pipeline);

		glPatchParameteri(GL_PATCH_VERTICES, 4);

		innerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelInner");
		outerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelOuter");

		glProgramUniform1fv(quadProgram, innerLevelUniform, 1, &innerLevel);
		glProgramUniform1fv(quadProgram, outerLevelUniform, 1, &outerLevel);

		glDrawArrays(GL_PATCHES, 0, 4);	GLERR();

		glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, triProgram);	GLERR();
		validatePipeline(pipeline);

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		innerLevelUniform = glGetUniformLocation(triProgram, "tessLevelInner");
		outerLevelUniform = glGetUniformLocation(triProgram, "tessLevelOuter");

		glProgramUniform1fv(triProgram, innerLevelUniform, 1, &innerLevel);
		glProgramUniform1fv(triProgram, outerLevelUniform, 1, &outerLevel);

		glDrawArrays(GL_PATCHES, 4, 3);	GLERR();

		EndDraw();	GLERR();

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

void validatePipeline(GLint pipeline) {
	GLint valid;

	glValidateProgramPipeline(pipeline);
	glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, &valid);	GLERR();
	if (valid == GL_FALSE) {
		cerr << "Program Pipeline Error" << endl;

		char log[5000];
		glGetProgramPipelineInfoLog(pipeline, 5000, NULL, log);	GLERR();

		cerr << "Pipeline valid status: " << valid << endl << log << endl;
		exit(1);
	}
}

void linkProgram(GLuint program) {
	GLint linkStatus;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		cerr << "Program Linking Error" << endl;

		char log[5000];
		glGetProgramInfoLog(program, 5000, NULL, log);

		cerr << log << endl;
		exit(1);
	}
}

void setPipeline() {
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint quadTCShader;
	GLuint quadTEShader;
	GLuint triTCShader;
	GLuint triTEShader;
	GLuint lineGShader;
	GLuint solidGShader;

#define SHADER_DIR "../shaders/"

	glGenProgramPipelines(1, &pipeline);	GLERR();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	quadTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	quadTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	triTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	triTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	lineGShader = glCreateShader(GL_GEOMETRY_SHADER);
	solidGShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(vertexShader, SHADER_DIR "tess.vert");	GLERR();
	setShader(fragmentShader, SHADER_DIR "flat.frag");	GLERR();
	setShader(quadTCShader, SHADER_DIR "quadConstant.tesc");	GLERR();
	setShader(quadTEShader, SHADER_DIR "quadInterpolate.tese");	GLERR();
	setShader(triTCShader, SHADER_DIR "triConstant.tesc");	GLERR();
	setShader(triTEShader, SHADER_DIR "triInterpolate.tese");	GLERR();
	setShader(lineGShader, SHADER_DIR "line.geom");	GLERR();
	setShader(solidGShader, SHADER_DIR "flat.geom");	GLERR();

	vertProgram = glCreateProgram();
	fragProgram = glCreateProgram();
	quadProgram = glCreateProgram();
	triProgram = glCreateProgram();
	lineProgram = glCreateProgram();
	solidProgram = glCreateProgram();

	glProgramParameteri(vertProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(fragProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(quadProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(triProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(lineProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(solidProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();

	//Base Shaders
	glAttachShader(vertProgram, vertexShader);	GLERR();
	glAttachShader(fragProgram, fragmentShader);	GLERR();

	//Quad Tess Shaders
	glAttachShader(quadProgram, quadTCShader);	GLERR();
	glAttachShader(quadProgram, quadTEShader);	GLERR();

	//Tri Tess Shaders
	glAttachShader(triProgram, triTCShader);	GLERR();
	glAttachShader(triProgram, triTEShader);	GLERR();

	//Line Geom Shader
	glAttachShader(lineProgram, lineGShader);	GLERR();

	//Solid Geom Shader
	glAttachShader(solidProgram, solidGShader);	GLERR();

	linkProgram(vertProgram);	GLERR();
	linkProgram(fragProgram);	GLERR();
	linkProgram(quadProgram);	GLERR();
	linkProgram(triProgram);	GLERR();
	linkProgram(lineProgram);	GLERR();
	linkProgram(solidProgram);	GLERR();

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(quadTCShader);
	glDeleteShader(quadTEShader);
	glDeleteShader(triTCShader);
	glDeleteShader(triTEShader);
	glDeleteShader(lineGShader);
	glDeleteShader(solidGShader);

	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertProgram);	GLERR();
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragProgram);	GLERR();
	glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
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
		case (XK_g):
			static unsigned int geometry = 0;

			if ((++geometry) % 2) {
				glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, solidProgram);	GLERR();
			} else {
				glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
			}
	};
}

void checkGLErr(const char *file, int line) {
	GLenum glErr;

	glErr = glGetError();

	if (glErr != GL_NO_ERROR) {
		cerr << file << "," << line << " - OpenGL Error: " << glErr << endl;
		exit(1);
	}
}

void getFunctionPointers() {
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) glXGetProcAddressARB( (const GLubyte *) "glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) glXGetProcAddressARB( (const GLubyte *) "glBindVertexArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC) glXGetProcAddressARB( (const GLubyte *) "glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) glXGetProcAddressARB( (const GLubyte *) "glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) glXGetProcAddressARB( (const GLubyte *) "glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) glXGetProcAddressARB( (const GLubyte *) "glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) glXGetProcAddressARB( (const GLubyte *) "glEnableVertexAttribArray");
	glShaderSource = (PFNGLSHADERSOURCEPROC) glXGetProcAddressARB( (const GLubyte *) "glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) glXGetProcAddressARB( (const GLubyte *) "glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glXGetProcAddressARB( (const GLubyte *) "glGetShaderInfoLog");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) glXGetProcAddressARB( (const GLubyte *) "glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) glXGetProcAddressARB( (const GLubyte *) "glGetProgramInfoLog");
	glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC) glXGetProcAddressARB( (const GLubyte *) "glGetProgramPipelineiv");
	glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) glXGetProcAddressARB( (const GLubyte *) "glGetProgramPipelineInfoLog");
	glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC) glXGetProcAddressARB( (const GLubyte *) "glValidateProgramPipeline");
	glCreateShader = (PFNGLCREATESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glCreateShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) glXGetProcAddressARB( (const GLubyte *) "glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) glXGetProcAddressARB( (const GLubyte *) "glBindAttribLocation");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) glXGetProcAddressARB( (const GLubyte *) "glLinkProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC) glXGetProcAddressARB( (const GLubyte *) "glDeleteShader");
	glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC) glXGetProcAddressARB( (const GLubyte *) "glUseProgramStages");
	glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC) glXGetProcAddressARB( (const GLubyte *) "glGenProgramPipelines");
	glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC) glXGetProcAddressARB( (const GLubyte *) "glBindProgramPipeline");
	glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) glXGetProcAddressARB( (const GLubyte *) "glProgramParameteri");
	glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) glXGetProcAddressARB( (const GLubyte *) "glPatchParameteri");
	glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC) glXGetProcAddressARB( (const GLubyte *) "glPatchParameterfv");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glXGetProcAddressARB( (const GLubyte *) "glGetUniformLocation");
	glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC) glXGetProcAddressARB( (const GLubyte *) "glProgramUniform1fv");
}
