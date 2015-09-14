#include <winsys.h>
#include <matrix4.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

#include "model.h"

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
BUTTON_PRESS(buttonPress);

static int debug;
static int verbose;
static GLuint pipeline;
static GLuint vertProgram;
static GLuint quadProgram;
static GLuint triProgram;
static GLuint lineProgram;
static GLuint flatProgram;
static GLuint smoothProgram;
static GLint maxTessLevel;
static GLfloat outerLevel;
static GLfloat innerLevel;
static Matrix4 modelview;
static Matrix4 projection;
static Position lightPos[2];

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
PFNGLPROGRAMUNIFORM3FVPROC			glProgramUniform3fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC	glProgramUniformMatrix4fv;

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
	TreatButtonPress = buttonPress;

	CreateWindow();

	getFunctionPointers();

	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	if (verbose) {
		cout << "Max Tess Level: " << maxTessLevel << endl;
	}

	XClearWindow(dpy, win);

	setPipeline();	GLERR();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

	Model *model;
	GLuint vertices;
	GLuint quadIndices;
	GLuint triIndices;

	model = loadObjModel("../Models/suzanne.obj");

	vertices = model->pos.size();
	quadIndices = model->quadIndices.size();
	triIndices = model->triIndices.size();

	GLuint vao;
	GLuint vbo[4];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Position), &(model->pos[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Position), &(model->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices * sizeof(unsigned int), &(model->quadIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndices * sizeof(unsigned int), &(model->triIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLint innerLevelUniform;
	GLint outerLevelUniform;
	GLint modelviewUniform;
	GLint projectionUniform;
	GLint lightPosUniform;

	innerLevel = 2.0f;
	outerLevel = 2.0f;

	lightPos[0] = (Position) {1.5f, 1.5f, 0.0f};
	lightPos[1] = (Position) {-1.5f, 1.5f, 0.0f};
	modelview.identity();
	modelview.rotate(180, 0, 1, 0);
	modelview.translate(0, 0, 3);
	projection.setPerspective(45, 1, 1, 1000);

	glBindProgramPipeline(pipeline);	GLERR();

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, quadProgram);	GLERR();
		validatePipeline(pipeline);

		glPatchParameteri(GL_PATCH_VERTICES, 4);

		innerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelInner");	GLERR();
		outerLevelUniform = glGetUniformLocation(quadProgram, "tessLevelOuter");	GLERR();
		modelviewUniform = glGetUniformLocation(vertProgram, "uModelView");	GLERR();
		projectionUniform = glGetUniformLocation(vertProgram, "uProjection");	GLERR();
		lightPosUniform = glGetUniformLocation(vertProgram, "uLightPos");	GLERR();

		glProgramUniform1fv(quadProgram, innerLevelUniform, 1, &innerLevel);	GLERR();
		glProgramUniform1fv(quadProgram, outerLevelUniform, 1, &outerLevel);	GLERR();
		glProgramUniformMatrix4fv(vertProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
		glProgramUniformMatrix4fv(vertProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
		glProgramUniform3fv(vertProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glDrawElements(GL_PATCHES, quadIndices, GL_UNSIGNED_INT, 0);	GLERR();
		//glDrawArrays(GL_PATCHES, 0, vertices);	GLERR();

		glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, triProgram);	GLERR();
		validatePipeline(pipeline);

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		innerLevelUniform = glGetUniformLocation(triProgram, "tessLevelInner");
		outerLevelUniform = glGetUniformLocation(triProgram, "tessLevelOuter");

		glProgramUniform1fv(triProgram, innerLevelUniform, 1, &innerLevel);
		glProgramUniform1fv(triProgram, outerLevelUniform, 1, &outerLevel);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glDrawElements(GL_PATCHES, triIndices, GL_UNSIGNED_INT, 0);	GLERR();

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
	GLuint flatFShader;
	GLuint smoothFShader;
	GLuint quadTCShader;
	GLuint quadTEShader;
	GLuint triTCShader;
	GLuint triTEShader;
	GLuint lineGShader;
	GLuint flatGShader;

	glGenProgramPipelines(1, &pipeline);	GLERR();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	flatFShader = glCreateShader(GL_FRAGMENT_SHADER);
	smoothFShader = glCreateShader(GL_FRAGMENT_SHADER);
	quadTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	quadTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	triTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	triTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	lineGShader = glCreateShader(GL_GEOMETRY_SHADER);
	flatGShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(vertexShader, "./shaders/tess.vert");	GLERR();
	setShader(flatFShader, "./shaders/flat.frag");	GLERR();
	setShader(smoothFShader, "./shaders/smooth.frag");	GLERR();
	setShader(quadTCShader, "./shaders/quad.tesc");	GLERR();
	setShader(quadTEShader, "./shaders/quad.tese");	GLERR();
	setShader(triTCShader, "./shaders/tri.tesc");	GLERR();
	setShader(triTEShader, "./shaders/tri.tese");	GLERR();
	setShader(lineGShader, "./shaders/line.geom");	GLERR();
	setShader(flatGShader, "./shaders/flat.geom");	GLERR();

	vertProgram = glCreateProgram();
	quadProgram = glCreateProgram();
	triProgram = glCreateProgram();
	lineProgram = glCreateProgram();
	flatProgram = glCreateProgram();
	smoothProgram = glCreateProgram();

	glProgramParameteri(vertProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(quadProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(triProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(lineProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(flatProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(smoothProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();

	//Base Shaders
	glAttachShader(vertProgram, vertexShader);	GLERR();

	//Quad Tess Shaders
	glAttachShader(quadProgram, quadTCShader);	GLERR();
	glAttachShader(quadProgram, quadTEShader);	GLERR();

	//Tri Tess Shaders
	glAttachShader(triProgram, triTCShader);	GLERR();
	glAttachShader(triProgram, triTEShader);	GLERR();

	//Line Geom Shader
	glAttachShader(lineProgram, lineGShader);	GLERR();
	glAttachShader(lineProgram, flatFShader);	GLERR();

	//Flat Solid Shaders
	glAttachShader(flatProgram, flatGShader);	GLERR();
	glAttachShader(flatProgram, flatFShader);	GLERR();

	//Smooth Solid Shaders
	glAttachShader(smoothProgram, smoothFShader);	GLERR();

	linkProgram(vertProgram);	GLERR();
	linkProgram(quadProgram);	GLERR();
	linkProgram(triProgram);	GLERR();
	linkProgram(lineProgram);	GLERR();
	linkProgram(flatProgram);	GLERR();
	linkProgram(smoothProgram);	GLERR();

	glDeleteShader(vertexShader);
	glDeleteShader(flatFShader);
	glDeleteShader(smoothFShader);
	glDeleteShader(quadTCShader);
	glDeleteShader(quadTEShader);
	glDeleteShader(triTCShader);
	glDeleteShader(triTEShader);
	glDeleteShader(lineGShader);
	glDeleteShader(flatGShader);

	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertProgram);	GLERR();
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
}

KEY_PRESS(keyPress) {
	static unsigned int geometry = 0;
	static float zLight = 0;

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
			if ((++geometry) % 2) {
				glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, flatProgram);	GLERR();
			} else {
				glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
			}
			break;
		case (XK_s):
			glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, 0);	GLERR();
			glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, smoothProgram);	GLERR();
			geometry = 1;
			break;
		case (XK_f):
			glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, flatProgram);	GLERR();
			geometry = 1;
			break;
		case (XK_Up):
			zLight += 1;
			lightPos[0] = (Position) {1.5f, 1.5f, zLight};
			lightPos[1] = (Position) {-1.5f, 1.5f, zLight};
			cout << zLight << endl;
			break;
		case (XK_Down):
			zLight -= 1;
			lightPos[0] = (Position) {1.5f, 1.5f, zLight};
			lightPos[1] = (Position) {-1.5f, 1.5f, zLight};
			cout << zLight << endl;
			break;
		case (XK_Escape):
			loop = false;
			break;
		default:
			break;
	};
}

BUTTON_PRESS(buttonPress) {
	switch (xbutton->button) {
		case (Button4):
			modelview.translate(0, 0, 1);
			break;
		case (Button5):
			modelview.translate(0, 0, -1);
			break;
		default:
			break;
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
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) glXGetProcAddress( (const GLubyte *) "glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) glXGetProcAddress( (const GLubyte *) "glBindVertexArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC) glXGetProcAddress( (const GLubyte *) "glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) glXGetProcAddress( (const GLubyte *) "glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) glXGetProcAddress( (const GLubyte *) "glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) glXGetProcAddress( (const GLubyte *) "glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) glXGetProcAddress( (const GLubyte *) "glEnableVertexAttribArray");
	glShaderSource = (PFNGLSHADERSOURCEPROC) glXGetProcAddress( (const GLubyte *) "glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC) glXGetProcAddress( (const GLubyte *) "glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) glXGetProcAddress( (const GLubyte *) "glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glXGetProcAddress( (const GLubyte *) "glGetShaderInfoLog");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) glXGetProcAddress( (const GLubyte *) "glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) glXGetProcAddress( (const GLubyte *) "glGetProgramInfoLog");
	glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC) glXGetProcAddress( (const GLubyte *) "glGetProgramPipelineiv");
	glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) glXGetProcAddress( (const GLubyte *) "glGetProgramPipelineInfoLog");
	glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC) glXGetProcAddress( (const GLubyte *) "glValidateProgramPipeline");
	glCreateShader = (PFNGLCREATESHADERPROC) glXGetProcAddress( (const GLubyte *) "glCreateShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) glXGetProcAddress( (const GLubyte *) "glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC) glXGetProcAddress( (const GLubyte *) "glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) glXGetProcAddress( (const GLubyte *) "glBindAttribLocation");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) glXGetProcAddress( (const GLubyte *) "glLinkProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC) glXGetProcAddress( (const GLubyte *) "glDeleteShader");
	glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC) glXGetProcAddress( (const GLubyte *) "glUseProgramStages");
	glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC) glXGetProcAddress( (const GLubyte *) "glGenProgramPipelines");
	glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC) glXGetProcAddress( (const GLubyte *) "glBindProgramPipeline");
	glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) glXGetProcAddress( (const GLubyte *) "glProgramParameteri");
	glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) glXGetProcAddress( (const GLubyte *) "glPatchParameteri");
	glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC) glXGetProcAddress( (const GLubyte *) "glPatchParameterfv");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glXGetProcAddress( (const GLubyte *) "glGetUniformLocation");
	glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC) glXGetProcAddress( (const GLubyte *) "glProgramUniform1fv");
	glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC) glXGetProcAddress( (const GLubyte *) "glProgramUniform3fv");
	glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC) glXGetProcAddress( (const GLubyte *) "glProgramUniformMatrix4fv");
}
