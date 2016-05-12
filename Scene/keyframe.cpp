#include <keyframe.h>

KeyFrame::KeyFrame(const char * const filename) {
	model = loadObjModel(filename);
	modelBuffer = NULL;

	vertices = 0;
	quadIndices = 0;
	triIndices = 0;

	vao = 0;
	vbo[0] = 0;
	vbo[1] = 0;
	vbo[2] = 0;
	vbo[3] = 0;
}

void KeyFrame::genBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, &vbo[0]);

	if (modelBuffer != NULL) {
		delete modelBuffer;
	}
	modelBuffer = model->genBuffer(true);

	vertices = modelBuffer->pos.size();
	quadIndices = modelBuffer->quadIndices.size();
	triIndices = modelBuffer->triIndices.size();

	// Should I keep this? How?
	/*
	if (verbose) {
		cout << model->faces.size() << " faces" <<  endl;
		cout << vertices << " vertices" << endl;
	}
	if (debug) {
		cout << quadIndices << " quadIndices" << endl;
		cout << triIndices << " triIndices" << endl;
	}
	*/

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

	glBindVertexArray(0);
}
