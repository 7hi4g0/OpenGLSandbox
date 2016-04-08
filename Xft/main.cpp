#include <glcommon.h>
#include <winsys.h>
#include <pipeline.h>
#include <matrix4.h>
#include <model.h>

void setBuffer(Model*);
void setPipeline();
void updateModelView();

static Model *model;
static ModelBuffer *modelBuffer;
static GLuint quadIndices;
static GLuint triIndices;
static GLuint indices;
static GLuint vao;
static GLuint vbo[4];
static GLuint flatProgram;
static Matrix4 modelview;
static Matrix4 projection;
static float angle;
static float distance;
static Vertex lightPos[2];
static int stepByStep;
static bool smoothNormals;

int main(int argc, char *argv[]) {
	char* filename;
   
	filename = (char *) "../Models/suzanne.obj";

	GraphicsContext GContext;
	GContext.major = 3;
	GContext.minor = 3;

	initGLFunctions();

	CreateWindow(&GContext);

	XClearWindow(dpy, win);

	setPipeline();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

	model = loadObjModel(filename);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, &vbo[0]);

	setBuffer(model);

	GLint modelviewUniform;
	GLint projectionUniform;
	GLint lightPosUniform;

	lightPos[0] = (Vertex) {1.5f, 1.5f, 0.0f};
	lightPos[1] = (Vertex) {-1.5f, 1.5f, 0.0f};
	angle = 180.0f;
	distance = 3.0f;
	modelview.identity();
	modelview.pushMatrix();
	updateModelView();
	projection.setPerspective(45, 1, 1, 1000);

	loop = true;
	while(loop) {
		TreatEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(flatProgram);

		glPatchParameteri(GL_PATCH_VERTICES, 4);

		modelviewUniform = glGetUniformLocation(flatProgram, "uModelView");	GLERR();
		projectionUniform = glGetUniformLocation(flatProgram, "uProjection");	GLERR();
		lightPosUniform = glGetUniformLocation(flatProgram, "uLightPos");	GLERR();

		glProgramUniformMatrix4fv(flatProgram, modelviewUniform, 1, GL_FALSE, &modelview[0]);	GLERR();
		glProgramUniformMatrix4fv(flatProgram, projectionUniform, 1, GL_FALSE, &projection[0]);	GLERR();
		glProgramUniform3fv(flatProgram, lightPosUniform, 2, (float *) &lightPos[0]);	GLERR();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glDrawElements(GL_PATCHES, indices, GL_UNSIGNED_INT, 0);	GLERR();

		EndDraw();	GLERR();

		msleep(33);
	}

	DestroyWindow();

	return 0;
}

void setBuffer(Model *model) {
	GLuint vertices;

	delete modelBuffer;
	modelBuffer = model->genBuffer(smoothNormals);

	vertices = modelBuffer->pos.size();
	quadIndices = modelBuffer->quadIndices.size();
	triIndices = modelBuffer->triIndices.size();

	if (verbose) {
		cout << model->faces.size() << " faces" <<  endl;
		cout << vertices << " vertices" << endl;
	}
	if (debug) {
		cout << quadIndices << " quadIndices" << endl;
		cout << triIndices << " triIndices" << endl;
	}

	indices = quadIndices;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(modelBuffer->pos[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(modelBuffer->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices * sizeof(unsigned int), &(modelBuffer->quadIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndices * sizeof(unsigned int), &(modelBuffer->triIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void setPipeline() {
	GLuint vertexShader;
	GLuint flatFShader;
	GLuint flatGShader;

#define SHADER_DIR	"../shaders/"

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	flatFShader = glCreateShader(GL_FRAGMENT_SHADER);
	flatGShader = glCreateShader(GL_GEOMETRY_SHADER);

	setShader(vertexShader, SHADER_DIR "duoLight.vert");	GLERR();
	setShader(flatFShader, SHADER_DIR "flat.frag");	GLERR();
	setShader(flatGShader, SHADER_DIR "flat.geom");	GLERR();

	flatProgram = glCreateProgram();

	glProgramParameteri(flatProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);	GLERR();

	glAttachShader(flatProgram, vertexShader);	GLERR();
	glAttachShader(flatProgram, flatGShader);	GLERR();
	glAttachShader(flatProgram, flatFShader);	GLERR();

	linkProgram(flatProgram);	GLERR();

	glDeleteShader(vertexShader);
	glDeleteShader(flatFShader);
	glDeleteShader(flatGShader);
}

void updateModelView() {
	modelview.popMatrix();
	modelview.pushMatrix();
	modelview.rotate(angle, 0, 1, 0);
	modelview.translate(0, 0, distance);
}
