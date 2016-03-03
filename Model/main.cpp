#include <glcommon.h>
#include <winsys.h>
#include <matrix4.h>
#include <model.h>
#include <pipeline.h>

using std::cout;
using std::cerr;
using std::endl;

void setPipeline();
void updateModelView();
KEY_PRESS(keyPress);
BUTTON_PRESS(buttonPress);

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
static float angle;
static float distance;
static Vertex lightPos[2];

int main(int argc, char *argv[]) {
	char opt;
	char *filename;

	filename = (char *) "../Models/suzanne.obj";

	debug = 0;

	while ((opt = getopt(argc, argv, ":dvf:")) != -1) {
		switch (opt) {
			case 'd':
				debug += 1;
				break;
			case 'v':
				verbose += 1;
				break;
			case 'f':
				filename = optarg;
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

	initGLFunctions();

	CreateWindow();

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
	ModelBuffer *buffer;
	GLuint vertices;
	GLuint quadIndices;
	GLuint triIndices;

	model = loadObjModel(filename);
	buffer = model->genBuffer();

	vertices = buffer->pos.size();
	quadIndices = buffer->quadIndices.size();
	triIndices = buffer->triIndices.size();

	GLuint vao;
	GLuint vbo[4];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(buffer->pos[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(buffer->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices * sizeof(unsigned int), &(buffer->quadIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndices * sizeof(unsigned int), &(buffer->triIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLint innerLevelUniform;
	GLint outerLevelUniform;
	GLint modelviewUniform;
	GLint projectionUniform;
	GLint lightPosUniform;

	innerLevel = 2.0f;
	outerLevel = 2.0f;

	lightPos[0] = (Vertex) {1.5f, 1.5f, 0.0f};
	lightPos[1] = (Vertex) {-1.5f, 1.5f, 0.0f};
	angle = 180.0f;
	distance = 3.0f;
	modelview.identity();
	modelview.pushMatrix();
	updateModelView();
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

#define SHADER_DIR	"../shaders/"

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

	setShader(vertexShader, SHADER_DIR "duoLight.vert");	GLERR();
	setShader(flatFShader, SHADER_DIR "flat.frag");	GLERR();
	setShader(smoothFShader, SHADER_DIR "smooth.frag");	GLERR();
	setShader(quadTCShader, SHADER_DIR "quadConstant.tesc");	GLERR();
	setShader(quadTEShader, SHADER_DIR "quadInterpolate.tese");	GLERR();
	setShader(triTCShader, SHADER_DIR "triConstant.tesc");	GLERR();
	setShader(triTEShader, SHADER_DIR "triInterpolate.tese");	GLERR();
	setShader(lineGShader, SHADER_DIR "line.geom");	GLERR();
	setShader(flatGShader, SHADER_DIR "flat.geom");	GLERR();

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

void updateModelView() {
	modelview.popMatrix();
	modelview.pushMatrix();
	modelview.rotate(angle, 0, 1, 0);
	modelview.translate(0, 0, distance);
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
			lightPos[0] = (Vertex) {1.5f, 1.5f, zLight};
			lightPos[1] = (Vertex) {-1.5f, 1.5f, zLight};
			break;
		case (XK_Down):
			zLight -= 1;
			lightPos[0] = (Vertex) {1.5f, 1.5f, zLight};
			lightPos[1] = (Vertex) {-1.5f, 1.5f, zLight};
			break;
		case (XK_Left):
			angle += 5.0f;
			updateModelView();
			break;
		case (XK_Right):
			angle -= 5.0f;
			updateModelView();
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
			distance += 1.0f;
			updateModelView();
			break;
		case (Button5):
			distance -= 1.0f;
			updateModelView();
			break;
		default:
			break;
	};
}
