#include <algorithm>

#include <glcommon.h>
#include <winsys.h>
#include <matrix4.h>
#include <model.h>
#include <pipeline.h>
#include <gpusubd.h>

using std::cout;
using std::cerr;
using std::endl;

void setPipeline();
void updateModelView();
KEY_PRESS(keyPress);
BUTTON_PRESS(buttonPress);

static GraphicsContext *ctx;
static AdaptiveSubSurf *model;
static GLuint pipeline;
static GLuint regularProgram;
static GLuint irregularProgram;
static GLuint quadProgram;
static GLuint triProgram;
static GLuint lineProgram;
static GLuint flatProgram;
static GLuint smoothProgram;
static GLint maxTessLevel;
static GLfloat tessLevel;
static Matrix4 modelview;
static Matrix4 projection;
static float angle;
static float distance;
static Vertex lightPos[2];
static Vertex levelColor[6];
static int levels;
//static bool smoothNormals;

int main(int argc, char *argv[]) {
	char opt;
	char *filename;

	filename = (char *) "../Models/suzanne.obj";

	debug = 0;
	levels = 3;

	while ((opt = getopt(argc, argv, ":dvf:l:")) != -1) {
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
			case 'l':
				levels = atoi(optarg);
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

	//smoothNormals = false;

	initGLFunctions();

	ctx = createGraphicsContext();

	ctx->major = 4;
	ctx->minor = 3;

	CreateWindow(ctx);

	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	if (verbose) {
		cout << "Max Tess Level: " << maxTessLevel << endl;
	}

	XClearWindow(ctx->dpy, ctx->win);

	setPipeline();	GLERR();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

	model = new AdaptiveSubSurf(filename);

	model->subdivide(levels);
	model->genBuffers();

	GLint tessLevelUniform;
	GLint modelviewUniform;
	GLint projectionUniform;
	GLint lightPosUniform;
	GLint colorUniform;

	tessLevel = pow(2.0f, levels - 1);

	lightPos[0] = (Vertex) {1.5f, 1.5f, 0.0f};
	lightPos[1] = (Vertex) {-1.5f, 1.5f, 0.0f};
	angle = 180.0f;
	distance = 3.0f;
	modelview.identity();
	modelview.pushMatrix();
	updateModelView();
	projection.setPerspective(45, 1, 1, 1000);

	levelColor[0] = (Vertex) {1.0f, 0.0f, 0.0f};
	levelColor[1] = (Vertex) {1.0f, 1.0f, 0.0f};
	levelColor[2] = (Vertex) {0.0f, 1.0f, 0.0f};
	levelColor[3] = (Vertex) {0.0f, 1.0f, 1.0f};
	levelColor[4] = (Vertex) {0.0f, 0.0f, 1.0f};
	levelColor[5] = (Vertex) {1.0f, 0.0f, 1.0f};

	glBindProgramPipeline(pipeline);	GLERR();

	int lastLevel = model->levels - 1;

	GLuint valencePosBuffer;
	GLuint valencePosSize = model->subLevels[lastLevel]->vertexValence;
	glGenBuffers(1, &valencePosBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, valencePosBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, valencePosSize * sizeof(Vertex), NULL, GL_DYNAMIC_READ);

	loop = true;
	while(loop) {
		TreatEvents(ctx);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tessLevelUniform = glGetUniformLocation(quadProgram, "tessLevel");	GLERR();
		modelviewUniform = glGetUniformLocation(quadProgram, "uModelView");	GLERR();
		projectionUniform = glGetUniformLocation(quadProgram, "uProjection");	GLERR();
		lightPosUniform = glGetUniformLocation(quadProgram, "uLightPos");	GLERR();
		colorUniform = glGetUniformLocation(quadProgram, "uColor");	GLERR();

		glProgramUniformMatrix4fv(quadProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
		glProgramUniformMatrix4fv(quadProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
		glProgramUniform3fv(quadProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();

		GLfloat tess = tessLevel;

		for (int level = 0; level < model->levels; level++) {
			AdaptiveLevel *subLevel = model->subLevels[level];

			glProgramUniform1fv(quadProgram, tessLevelUniform, 1, &tess);	GLERR();
			glProgramUniform3fv(quadProgram, colorUniform, 1, (float *) &levelColor[level]);	GLERR();

			glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, regularProgram);	GLERR();
			glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, quadProgram);	GLERR();
			validatePipeline(pipeline);

			glPatchParameteri(GL_PATCH_VERTICES, 16);

			glBindVertexArray(subLevel->vao);

			if (level == lastLevel) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subLevel->levelRegularIndexBuffer);
				glDrawElements(GL_PATCHES, subLevel->levelRegularIndices, GL_UNSIGNED_INT, 0);	GLERR();

				modelviewUniform = glGetUniformLocation(irregularProgram, "uModelView");	GLERR();
				projectionUniform = glGetUniformLocation(irregularProgram, "uProjection");	GLERR();
				lightPosUniform = glGetUniformLocation(irregularProgram, "uLightPos");	GLERR();
				colorUniform = glGetUniformLocation(irregularProgram, "uColor");	GLERR();

				glProgramUniformMatrix4fv(irregularProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
				glProgramUniformMatrix4fv(irregularProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
				glProgramUniform3fv(irregularProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();
				glProgramUniform3fv(irregularProgram, colorUniform, 1, (float *) &levelColor[level]);	GLERR();

				glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, irregularProgram);	GLERR();
				glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, 0);	GLERR();
				validatePipeline(pipeline);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subLevel->levelIrregularIndexBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, subLevel->posBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, subLevel->vertexValenceBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valencePosBuffer);
				glDrawElements(GL_TRIANGLES, subLevel->levelIrregularIndices, GL_UNSIGNED_INT, 0);	GLERR();

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
			} else {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subLevel->fullIndexBuffer);
				glDrawElements(GL_PATCHES, subLevel->fullIndices, GL_UNSIGNED_INT, 0);	GLERR();
			}

			glBindVertexArray(0);

			tess = std::max(tess / 2.0f, 1.0f);
		}

		EndDraw(ctx);	GLERR();

		msleep(33);
	}
	
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	Vertex *valencePosArray = (Vertex *) glMapNamedBufferRange(valencePosBuffer, 0, valencePosSize * sizeof(Vertex), GL_MAP_READ_BIT);	GLERR();

	Vertex *vertices = &model->subLevels[lastLevel]->adaptiveBuffer->verts[0];
	unsigned int *vertexValence = &model->subLevels[lastLevel]->adaptiveBuffer->vertexValence[0];
	unsigned int index = 1;
	while(index < valencePosSize) {
		unsigned int valence = vertexValence[index] * 2;

		cout << vertexValence[index] << endl;
		cout << valencePosArray[index] << endl;
		index++;

		for (unsigned int v = 0; v < valence; v++) {
			cout << "\t" << vertices[vertexValence[index]] << endl;
			cout << "\t" << valencePosArray[index] << endl;
			index++;
		}
	}

	glUnmapNamedBuffer(valencePosBuffer);

	DestroyWindow(ctx);
	return 0;
}

void setPipeline() {
	GLuint regularVShader;
	GLuint irregularVShader;
	GLuint flatFShader;
	GLuint smoothFShader;
	GLuint quadTCShader;
	GLuint quadTEShader;
	GLuint lineGShader;
	GLuint flatGShader;

#define SHADER_DIR	"../shaders/"

	glGenProgramPipelines(1, &pipeline);	GLERR();

	regularVShader = glCreateShader(GL_VERTEX_SHADER);
	irregularVShader = glCreateShader(GL_VERTEX_SHADER);
	flatFShader = glCreateShader(GL_FRAGMENT_SHADER);
	smoothFShader = glCreateShader(GL_FRAGMENT_SHADER);
	quadTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	quadTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	lineGShader = glCreateShader(GL_GEOMETRY_SHADER);
	flatGShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(regularVShader, SHADER_DIR "duoLightAdaptive.vert");	GLERR();
	setShader(irregularVShader, SHADER_DIR "duoLightIrregularAdaptive.vert");	GLERR();
	setShader(flatFShader, SHADER_DIR "flat.frag");	GLERR();
	setShader(smoothFShader, SHADER_DIR "smooth.frag");	GLERR();
	setShader(quadTCShader, SHADER_DIR "adaptiveConstant.tesc");	GLERR();
	setShader(quadTEShader, SHADER_DIR "adaptiveInterpolate2.tese");	GLERR();
	setShader(lineGShader, SHADER_DIR "line.geom");	GLERR();
	setShader(flatGShader, SHADER_DIR "flat.geom");	GLERR();

	regularProgram = glCreateProgram();
	irregularProgram = glCreateProgram();
	quadProgram = glCreateProgram();
	lineProgram = glCreateProgram();
	flatProgram = glCreateProgram();
	smoothProgram = glCreateProgram();

	glProgramParameteri(regularProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(irregularProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(quadProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(lineProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(flatProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(smoothProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();

	//Regular Vertex Shaders
	glAttachShader(regularProgram, regularVShader);	GLERR();

	//Irregular Vertex Shaders
	glAttachShader(irregularProgram, irregularVShader);	GLERR();

	//Quad Tess Shaders
	glAttachShader(quadProgram, quadTCShader);	GLERR();
	glAttachShader(quadProgram, quadTEShader);	GLERR();

	//Line Geom Shader
	glAttachShader(lineProgram, lineGShader);	GLERR();
	glAttachShader(lineProgram, flatFShader);	GLERR();

	//Flat Solid Shaders
	glAttachShader(flatProgram, flatGShader);	GLERR();
	glAttachShader(flatProgram, flatFShader);	GLERR();

	//Smooth Solid Shaders
	glAttachShader(smoothProgram, smoothFShader);	GLERR();

	linkProgram(regularProgram);	GLERR();
	linkProgram(irregularProgram);	GLERR();
	linkProgram(quadProgram);	GLERR();
	linkProgram(lineProgram);	GLERR();
	linkProgram(flatProgram);	GLERR();
	linkProgram(smoothProgram);	GLERR();

	glDeleteShader(regularVShader);
	glDeleteShader(irregularVShader);
	glDeleteShader(flatFShader);
	glDeleteShader(smoothFShader);
	glDeleteShader(quadTCShader);
	glDeleteShader(quadTEShader);
	glDeleteShader(lineGShader);
	glDeleteShader(flatGShader);

	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, regularProgram);	GLERR();
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
	KeySym keysym;
	unsigned int currentTime;

	switch(keysym = XLookupKeysym(xkey, 0)) {
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
			/*
			smoothNormals = true;
			setBuffer(model->getLevel(levels));
			*/
			break;
		case (XK_f):
			glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, flatProgram);	GLERR();
			geometry = 1;
			/*
			smoothNormals = false;
			setBuffer(model->getLevel(levels));
			*/
			break;
		case (XK_KP_Subtract):
			if (levels > 0) {
				levels--;
				//setBuffer(model->getLevel(levels));
			}
			cout << levels << endl;
			break;
		case (XK_KP_Add):
			if (levels < model->levels - 1) {
			/*
				if (verbose) {
					currentTime = getTime();
				}
				model->subdivide();
				if (verbose) {
					cout << (getTime() - currentTime) << " ms - Catmull-Clark" << endl;
				}
			*/
				levels++;
			}
			cout << levels << endl;
			//setBuffer(model->getLevel(levels));
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
			if (verbose) {
				cerr << "Untreated " << XKeysymToString(keysym) << " captured." << endl;
			}
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
