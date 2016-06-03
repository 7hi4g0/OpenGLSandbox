#include <iostream> // TODO: Do I need this?

#include <adptlevel.h>

unsigned int controlPointMap[16] = { 5, 1, 0, 4, 9, 8, 12, 13, 10, 14, 15, 11, 6, 7, 3, 2 };

void addFaceControlPoints(FacePtr face, std::vector<unsigned int> &indicesBuffer) {
	unsigned int indices[16];
	int count = 0;

	/*
	EdgePtr topEdge;
	FacePtr topFace;

	topEdge = face->edges[0];
	topFace = topEdge->otherFace(face);

	EdgePtr topLeftEdge;
	FacePtr topLeftFace;

	topLeftEdge = topFace->prevEdge(topEdge);
	topLeftFace = topLeftEdge->otherFace(topFace);
	*/

	DirFace currFace(face, 0);
	DirFace diagonal;

	for (int diags = 0; diags < 4; diags++) {
		diagonal = currFace.adjFace().prevEdge().adjFace();

		for (int vert = 0; vert < 4; vert++) {
			indices[controlPointMap[count++]] = diagonal.vert()->idx;
			diagonal = diagonal.nextEdge();
		}

		currFace = currFace.nextEdge();
	}

	for (int indice = 0; indice < 16; indice++) {
		indicesBuffer.push_back(indices[indice]);
	}
}

DirFace::DirFace() {
	face = NULL;
	index = 0;
}

DirFace::DirFace(FacePtr f, unsigned int i) {
	face = f;
	index = i;
}

DirFace DirFace::adjFace() {
	DirFace ret;
	EdgePtr edge;

	edge = face->edges[index];

	if (edge->faces[0] == face) {
		ret.face = edge->faces[1];
	} else if (edge->faces[1] == face) {
		ret.face = edge->faces[0];
	} else {
		// ERROR
	}

	for (int vert = 0; vert < ret.face->numVertices; vert++) {
		if (ret.face->edges[vert] == edge) {
			ret.index = vert;

			break;
		}
	}

	return ret;
}

DirFace DirFace::nextEdge() {
	DirFace ret;

	ret.face = face;
	ret.index = (index + 1) % face->numVertices;

	return ret;
}

DirFace DirFace::prevEdge() {
	DirFace ret;

	ret.face = face;
	ret.index = (index + face->numVertices - 1) % face->numVertices;

	return ret;
}

PositionPtr DirFace::vert() {
	return face->pos[index];
}

AdaptiveLevel::AdaptiveLevel() {
	mesh = NULL;
	adaptiveBuffer = NULL;

	vertices = 0;
	fullIndices = 0;
	levelRegularIndices = 0;
	levelIrregularIndices = 0;

	vao = 0;
	vbo[0] = 0;
	vbo[1] = 0;
	vbo[2] = 0;
	vbo[3] = 0;
}

void AdaptiveLevel::addFaceRing(FacePtr face) {
	face->tagged = true;
	subFaces.insert(face);

	for (int vertice = 0; vertice < face->numVertices; vertice++) {
		PositionPtr pos = face->pos[vertice];

		for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
			FacePtr adjFace = *faceIt;

			if (!adjFace->tagged && !adjFace->isRegular()) {
				addFaceRing(adjFace);
			} else {
				subFaces.insert(adjFace);
			}
		}
	}
}

void AdaptiveLevel::genFaceSets() {
	for (auto faceIt = mesh->faces.begin(); faceIt != mesh->faces.end(); faceIt++) {
		FacePtr face = *faceIt;

		if (face->wasTagged) {
			levelFaces.insert(face);

			if (!face->tagged) {
				fullFaces.insert(face);
			}
		}
	}

	if (verbose) {
		cout << "Faces in control mesh: " << mesh->faces.size() << endl;
		cout << "Faces to subdivide: " << subFaces.size() << endl;
		cout << "Faces in level: " << levelFaces.size() << endl;
		cout << "Faces full in level: " << fullFaces.size() << endl;
	}
}

void AdaptiveLevel::genBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, &vbo[0]);
}

void AdaptiveLevel::setBuffersData() {
	if (adaptiveBuffer != NULL) {
		delete adaptiveBuffer;
	}
	adaptiveBuffer = genAdaptiveBuffer();

	vertices = adaptiveBuffer->verts.size();
	fullIndices = adaptiveBuffer->fullIndices.size();
	levelRegularIndices = adaptiveBuffer->levelRegularIndices.size();
	levelIrregularIndices = adaptiveBuffer->levelIrregularIndices.size();

	// Should I keep this? How?
	/*
	if (verbose) {
		cout << model->faces.size() << " faces" <<  endl;
		cout << vertices << " vertices" << endl;
	}
	if (debug) {
		cout << fullIndices << " fullIndices" << endl;
		cout << levelRegularIndices << " levelRegularIndices" << endl;
	}
	*/

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(adaptiveBuffer->verts[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fullIndices * sizeof(unsigned int), &(adaptiveBuffer->fullIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, levelRegularIndices * sizeof(unsigned int), &(adaptiveBuffer->levelRegularIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, levelIrregularIndices * sizeof(unsigned int), &(adaptiveBuffer->levelIrregularIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

AdaptiveBuffer *AdaptiveLevel::genAdaptiveBuffer() {
	AdaptiveBuffer *buffer = new AdaptiveBuffer();

	for (auto vertIt = mesh->pos.begin(); vertIt != mesh->pos.end(); vertIt++) {
		PositionPtr vert = *vertIt;

		vert->idx = buffer->verts.size();
		buffer->verts.push_back(vert->v);
	}

	for (auto faceIt = fullFaces.begin(); faceIt != fullFaces.end(); faceIt++) {
		FacePtr face = *faceIt;

		if (face->isRegular()) {
			addFaceControlPoints(face, buffer->fullIndices);
		}
	}

	for (auto faceIt = levelFaces.begin(); faceIt != levelFaces.end(); faceIt++) {
		FacePtr face = *faceIt;

		if (face->isRegular()) {
			addFaceControlPoints(face, buffer->levelRegularIndices);
		} else if (face->numVertices == 4) {
			buffer->levelIrregularIndices.push_back(face->pos[0]->idx);
			buffer->levelIrregularIndices.push_back(face->pos[1]->idx);
			buffer->levelIrregularIndices.push_back(face->pos[2]->idx);

			buffer->levelIrregularIndices.push_back(face->pos[2]->idx);
			buffer->levelIrregularIndices.push_back(face->pos[3]->idx);
			buffer->levelIrregularIndices.push_back(face->pos[0]->idx);
		}
	}

	return buffer;
}
