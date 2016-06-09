#include <algorithm>

#include <glcommon.h>
#include <winsys.h>
#include <matrix4.h>
#include <model.h>
#include <pipeline.h>
#include <gpusubd.h>
#include <scene.h>

using std::cout;
using std::cerr;
using std::endl;

void setPipeline();
void updateModelView();
KEY_PRESS(keyPress);
BUTTON_PRESS(buttonPress);

static GraphicsContext *ctx;
static AdaptiveSubSurf *model;
static GLuint pipeline[2];
static GLuint cageProgram;
static GLuint regularProgram;
static GLuint irregularProgram;
static GLuint quadProgram;
static GLuint adaptiveProgram;
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
static int cageLevel;
static std::vector<KeyFrame *> cages;
static bool showCage;

int main(int argc, char *argv[]) {
	char opt;
	char *filename;
	unsigned int measure;

	filename = (char *) "../Models/sphere.obj";

	debug = 0;
	levels = 3;
	cageLevel = 0;
	showCage = false;
	measure = 0;

	while ((opt = getopt(argc, argv, ":dvf:l:m:")) != -1) {
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
			case 'm':
				measure = atoi(optarg);
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

	model->genCatmullClarkTables();
	model->gpuCatmullClark();

	for (int level = 0; level < levels; level++) {
		KeyFrame *cage;

		cage = new KeyFrame(model->subLevels[level]->mesh);
		cage->genBuffers();

		cages.push_back(cage);
	}

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
	levelColor[1] = (Vertex) {0.0f, 1.0f, 0.0f};
	levelColor[2] = (Vertex) {0.0f, 0.0f, 1.0f};
	levelColor[3] = (Vertex) {1.0f, 1.0f, 0.0f};
	levelColor[4] = (Vertex) {0.0f, 1.0f, 1.0f};
	levelColor[5] = (Vertex) {1.0f, 0.0f, 1.0f};

	int lastLevel = model->levels - 1;

	FrameCounter *frameCounter = initFrameCounter();

	loop = true;
	while(loop) {
		TreatEvents(ctx);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tessLevelUniform = glGetUniformLocation(adaptiveProgram, "tessLevel");	GLERR();
		modelviewUniform = glGetUniformLocation(adaptiveProgram, "uModelView");	GLERR();
		projectionUniform = glGetUniformLocation(adaptiveProgram, "uProjection");	GLERR();
		lightPosUniform = glGetUniformLocation(adaptiveProgram, "uLightPos");	GLERR();

		glProgramUniformMatrix4fv(adaptiveProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
		glProgramUniformMatrix4fv(adaptiveProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
		glProgramUniform4fv(adaptiveProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();

		modelviewUniform = glGetUniformLocation(cageProgram, "uModelView");	GLERR();
		projectionUniform = glGetUniformLocation(cageProgram, "uProjection");	GLERR();
		lightPosUniform = glGetUniformLocation(cageProgram, "uLightPos");	GLERR();

		glProgramUniform1f(quadProgram, 0, 1.0);	GLERR();
		glProgramUniform1f(quadProgram, 1, 1.0);	GLERR();
		glProgramUniformMatrix4fv(cageProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
		glProgramUniformMatrix4fv(cageProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
		glProgramUniform4fv(cageProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();

		GLfloat tess = tessLevel;

		for (int level = 0; level < model->levels; level++) {
			AdaptiveLevel *subLevel = model->subLevels[level];
			KeyFrame *cage = cages[level];

			colorUniform = glGetUniformLocation(adaptiveProgram, "uColor");	GLERR();

			glProgramUniform1fv(adaptiveProgram, tessLevelUniform, 1, &tess);	GLERR();
			glProgramUniform3fv(adaptiveProgram, colorUniform, 1, (float *) &levelColor[level]);	GLERR();

			colorUniform = glGetUniformLocation(cageProgram, "uColor");	GLERR();

			glProgramUniform3fv(cageProgram, colorUniform, 1, (float *) &levelColor[level]);	GLERR();

			glBindProgramPipeline(pipeline[0]);	GLERR();

			glUseProgramStages(pipeline[0], GL_VERTEX_SHADER_BIT, regularProgram);	GLERR();
			glUseProgramStages(pipeline[0], GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, adaptiveProgram);	GLERR();
			validatePipeline(pipeline[0]);

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
				glProgramUniform4fv(irregularProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();
				glProgramUniform3fv(irregularProgram, colorUniform, 1, (float *) &levelColor[level]);	GLERR();

				glUseProgramStages(pipeline[0], GL_VERTEX_SHADER_BIT, irregularProgram);	GLERR();
				glUseProgramStages(pipeline[0], GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, 0);	GLERR();
				validatePipeline(pipeline[0]);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subLevel->levelIrregularIndexBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, model->verticesBuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model->valenceBuffer);
				glDrawElements(GL_TRIANGLES, subLevel->levelIrregularIndices, GL_UNSIGNED_INT, 0);	GLERR();

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
			} else {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subLevel->fullIndexBuffer);
				glDrawElements(GL_PATCHES, subLevel->fullIndices, GL_UNSIGNED_INT, 0);	GLERR();
			}

			if (showCage && level == cageLevel) {
				glBindProgramPipeline(pipeline[1]);	GLERR();

				glPatchParameteri(GL_PATCH_VERTICES, 4);

				glBindVertexArray(cage->vao);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cage->quadIndexBuffer);
				glDrawElements(GL_PATCHES, cage->quadIndices, GL_UNSIGNED_INT, 0);	GLERR();
			}

			glBindVertexArray(0);

			tess = std::max(tess / 2.0f, 1.0f);
		}

		EndDraw(ctx);	GLERR();

		//msleep(33);

		updateFrameCounter(frameCounter);

		if (measure > 0) {
			if (frameCounter->elapsedTime >= 1000) {
				float fps = (float) (frameCounter->frameCount * 1000) / (float) frameCounter->elapsedTime;

				cout << frameCounter->frameCount << ", ";
				cout << frameCounter->elapsedTime << ", ";
				cout << fps << endl;

				frameCounter->frameCount = 0;
				frameCounter->elapsedTime = 0;
			}

			if (frameCounter->elapsedTimeTotal >= measure) {
				loop = false;
			}
		}
	}

	if (measure > 0) {
		float fps = (float) (frameCounter->frameCountTotal * 1000) / (float) frameCounter->elapsedTimeTotal;
		cout << frameCounter->frameCountTotal << ", ";
		cout << frameCounter->elapsedTimeTotal << ", ";
		cout << fps << endl;
	}

	DestroyWindow(ctx);
	return 0;
}

void setPipeline() {
	GLuint cageVShader;
	GLuint regularVShader;
	GLuint irregularVShader;
	GLuint flatFShader;
	GLuint smoothFShader;
	GLuint quadTCShader;
	GLuint quadTEShader;
	GLuint adaptiveTCShader;
	GLuint adaptiveTEShader;
	GLuint lineGShader;
	GLuint flatGShader;

#define SHADER_DIR	"../shaders/"

	glGenProgramPipelines(2, &pipeline[0]);	GLERR();

	cageVShader = glCreateShader(GL_VERTEX_SHADER);
	regularVShader = glCreateShader(GL_VERTEX_SHADER);
	irregularVShader = glCreateShader(GL_VERTEX_SHADER);
	flatFShader = glCreateShader(GL_FRAGMENT_SHADER);
	smoothFShader = glCreateShader(GL_FRAGMENT_SHADER);
	quadTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	quadTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	adaptiveTCShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	adaptiveTEShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	lineGShader = glCreateShader(GL_GEOMETRY_SHADER);
	flatGShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(cageVShader, SHADER_DIR "duoLightCage.vert");	GLERR();
	setShader(regularVShader, SHADER_DIR "duoLightAdaptive.vert");	GLERR();
	setShader(irregularVShader, SHADER_DIR "duoLightIrregularAdaptive.vert");	GLERR();
	setShader(flatFShader, SHADER_DIR "flat.frag");	GLERR();
	setShader(smoothFShader, SHADER_DIR "smooth.frag");	GLERR();
	setShader(quadTCShader, SHADER_DIR "quadConstant.tesc");	GLERR();
	setShader(quadTEShader, SHADER_DIR "quadInterpolate.tese");	GLERR();
	setShader(adaptiveTCShader, SHADER_DIR "adaptiveConstant.tesc");	GLERR();
	setShader(adaptiveTEShader, SHADER_DIR "adaptiveInterpolate2.tese");	GLERR();
	setShader(lineGShader, SHADER_DIR "line.geom");	GLERR();
	setShader(flatGShader, SHADER_DIR "flat.geom");	GLERR();

	cageProgram = glCreateProgram();
	regularProgram = glCreateProgram();
	irregularProgram = glCreateProgram();
	quadProgram = glCreateProgram();
	adaptiveProgram = glCreateProgram();
	lineProgram = glCreateProgram();
	flatProgram = glCreateProgram();
	smoothProgram = glCreateProgram();

	glProgramParameteri(cageProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(regularProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(irregularProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(quadProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(adaptiveProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(lineProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(flatProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();
	glProgramParameteri(smoothProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();

	//Cage Vertex Shaders
	glAttachShader(cageProgram, cageVShader);	GLERR();

	//Regular Vertex Shaders
	glAttachShader(regularProgram, regularVShader);	GLERR();

	//Irregular Vertex Shaders
	glAttachShader(irregularProgram, irregularVShader);	GLERR();

	//Quad Tess Shaders
	glAttachShader(quadProgram, quadTCShader);	GLERR();
	glAttachShader(quadProgram, quadTEShader);	GLERR();

	//Adaptive Tess Shaders
	glAttachShader(adaptiveProgram, adaptiveTCShader);	GLERR();
	glAttachShader(adaptiveProgram, adaptiveTEShader);	GLERR();

	//Line Geom Shader
	glAttachShader(lineProgram, lineGShader);	GLERR();
	glAttachShader(lineProgram, flatFShader);	GLERR();

	//Flat Solid Shaders
	glAttachShader(flatProgram, flatGShader);	GLERR();
	glAttachShader(flatProgram, flatFShader);	GLERR();

	//Smooth Solid Shaders
	glAttachShader(smoothProgram, smoothFShader);	GLERR();

	linkProgram(cageProgram);	GLERR();
	linkProgram(regularProgram);	GLERR();
	linkProgram(irregularProgram);	GLERR();
	linkProgram(quadProgram);	GLERR();
	linkProgram(adaptiveProgram);	GLERR();
	linkProgram(lineProgram);	GLERR();
	linkProgram(flatProgram);	GLERR();
	linkProgram(smoothProgram);	GLERR();

	glDeleteShader(cageVShader);
	glDeleteShader(regularVShader);
	glDeleteShader(irregularVShader);
	glDeleteShader(flatFShader);
	glDeleteShader(smoothFShader);
	glDeleteShader(quadTCShader);
	glDeleteShader(quadTEShader);
	glDeleteShader(adaptiveTCShader);
	glDeleteShader(adaptiveTEShader);
	glDeleteShader(lineGShader);
	glDeleteShader(flatGShader);

	glUseProgramStages(pipeline[0], GL_VERTEX_SHADER_BIT, regularProgram);	GLERR();
	glUseProgramStages(pipeline[0], GL_FRAGMENT_SHADER_BIT, smoothProgram);	GLERR();

	glUseProgramStages(pipeline[1], GL_VERTEX_SHADER_BIT, cageProgram);	GLERR();
	glUseProgramStages(pipeline[1], GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT, quadProgram);	GLERR();
	glUseProgramStages(pipeline[1], GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
	validatePipeline(pipeline[1]);
}

void updateModelView() {
	modelview.popMatrix();
	modelview.pushMatrix();
	modelview.rotate(angle, 0, 1, 0);
	modelview.translate(0, 0, distance);
}

KEY_PRESS(keyPress) {
	static unsigned int geometry = 1;
	static float zLight = 0;
	KeySym keysym;
	unsigned int currentTime;

	switch(keysym = XLookupKeysym(xkey, 0)) {
		case (XK_c):
			showCage = !showCage;
			break;
		case (XK_g):
			if ((++geometry) % 2) {
				glUseProgramStages(pipeline[0], GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, flatProgram);	GLERR();
			} else {
				glUseProgramStages(pipeline[0], GL_FRAGMENT_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, lineProgram);	GLERR();
			}
			break;
		case (XK_s):
			glUseProgramStages(pipeline[0], GL_GEOMETRY_SHADER_BIT, 0);	GLERR();
			glUseProgramStages(pipeline[0], GL_FRAGMENT_SHADER_BIT, smoothProgram);	GLERR();
			geometry = 1;
			break;
		case (XK_f):
			glUseProgramStages(pipeline[0], GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, flatProgram);	GLERR();
			geometry = 1;
			break;
		case (XK_KP_Subtract):
			if (cageLevel > 0) {
				cageLevel--;
			}
			cout << levels << endl;
			break;
		case (XK_KP_Add):
			if (cageLevel < model->levels - 1) {
				cageLevel++;
			}
			cout << levels << endl;
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
