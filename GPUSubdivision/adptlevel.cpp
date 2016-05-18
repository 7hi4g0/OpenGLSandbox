#include <iostream>

#include <adptlevel.h>

AdaptiveLevel::AdaptiveLevel() {
	level = NULL;
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

void AdaptiveLevel::addFaceRing(FacePtr face) {
	for (int vertice = 0; vertice < face->numVertices; vertice++) {
		PositionPtr pos = face->pos[vertice];

		for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
			FacePtr face = *faceIt;

			subFaces.insert(face);
		}
	}
}

ModelBuffer *AdaptiveLevel::genBuffer(bool smooth) {
	ModelBuffer *buffer = new ModelBuffer;
	unsigned int index = 0;
	FaceSet levelFaces;

	// Why it is not working?
	/*
	std::set_difference(level->faces.begin(), level->faces.end(),
						subFaces.begin(), subFaces.end(),
						std::inserter(levelFaces, levelFaces.end()));
						*/
	for (auto faceIt = level->faces.begin(); faceIt != level->faces.end(); faceIt++) {
		if (subFaces.count(*faceIt) == 0) {
			levelFaces.insert(*faceIt);
		}
	}

	if (verbose) {
		cout << "Faces in level: " << level->faces.size() << endl;
		cout << "Faces to subdivide: " << subFaces.size() << endl;
		cout << "Faces to render in level: " << levelFaces.size() << endl;
	}

	for (auto faceIt = levelFaces.begin(); faceIt != levelFaces.end(); faceIt++) {
		FacePtr face = *faceIt;
		std::vector<unsigned int> *indices;

		if (face->numVertices == 4) {
			indices = &buffer->quadIndices;
		} else if (face->numVertices == 3) {
			indices = &buffer->triIndices;
		} else {
			cerr << "Unsupported face type" << endl;
			exit(1);
		}

		for (int vert = 0; vert < face->numVertices; vert++) {
			buffer->pos.push_back(face->pos[vert]->v);
			if (smooth) {
				buffer->normals.push_back(face->pos[vert]->vertexNormal());
			} else {
				buffer->normals.push_back(*face->normals[vert]);
			}
			indices->push_back(index++);
		}
	}

	return buffer;
}

void AdaptiveLevel::genBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, &vbo[0]);

	if (modelBuffer != NULL) {
		delete modelBuffer;
	}
	modelBuffer = genBuffer(true);

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
