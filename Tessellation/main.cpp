#include <winsys.h>
#include <pipeline.h>

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

void setPipeline();
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
