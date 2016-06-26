#include <iostream> // TODO: Do I need this?

#include <adptlevel.h>

unsigned int controlPointMap[16] = { 5, 1, 0, 4, 9, 8, 12, 13, 10, 14, 15, 11, 6, 7, 3, 2 };

void addFaceControlPoints(FacePtr face, std::vector<unsigned int> &indicesBuffer) {
	unsigned int indices[16];
	int count = 0;

	/*
	for (int vert = 0; vert < face->numVertices; vert++) {
		PositionPtr pos = face->pos[vert];

		for (auto edgeIt = pos->edges.begin(); edgeIt != pos->edges.end(); edgeIt++) {
			EdgePtr edge = *edgeIt;

			if (edge->faceCount == 1) {
				return;
			}
		}
	}

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

PositionPtr DirFace::next() {
	return face->pos[(index + 1) % face->numVertices];
}

PositionPtr DirFace::diag() {
	return face->pos[(index + 2) % face->numVertices];
}

PositionPtr DirFace::prev() {
	return face->pos[(index + 3) % face->numVertices];
}

AdaptiveLevel::AdaptiveLevel() {
	mesh = NULL;
	adaptiveBuffer = NULL;
	topologyBuffer = NULL;

	fullIndices = 0;
	levelRegularIndices = 0;
	levelIrregularIndices = 0;

	vao = 0;
	vbo[0] = 0;
	vbo[1] = 0;
	vbo[2] = 0;
	vbo[3] = 0;
	vbo[4] = 0;
	vbo[5] = 0;
	vbo[6] = 0;
}

AdaptiveLevel::~AdaptiveLevel() {
	if (mesh != NULL) {
		delete mesh;
	}
	if (adaptiveBuffer != NULL) {
		delete adaptiveBuffer;
	}
	if (topologyBuffer != NULL) {
		delete topologyBuffer;
	}
}

void AdaptiveLevel::addFace(FacePtr face) {
	subFaces.insert(face);

	for (int vertex = 0; vertex < face->numVertices; vertex++) {
		subEdges.insert(face->edges[vertex]);
		subVertices.insert(face->pos[vertex]);
	}
}

void AdaptiveLevel::addFaceRing(FacePtr face) {
	face->tagged = true;
	addFace(face);

	for (int vertice = 0; vertice < face->numVertices; vertice++) {
		PositionPtr pos = face->pos[vertice];

		for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
			FacePtr adjFace = *faceIt;

			if (!adjFace->tagged && !adjFace->isRegular()) {
				addFaceRing(adjFace);
			} else {
				addFace(adjFace);
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

void AdaptiveLevel::genBuffers(GLuint verticesBuffer, GLuint valenceIndicesBuffer) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(3, &vbo[1]);
	vbo[0] = verticesBuffer;
	vbo[4] = valenceIndicesBuffer;
}

void AdaptiveLevel::setBuffersData(VerticesBuffer *vertsBuffer) {
	if (adaptiveBuffer != NULL) {
		delete adaptiveBuffer;
	}
	adaptiveBuffer = genAdaptiveBuffer(vertsBuffer);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fullIndices * sizeof(unsigned int), &(adaptiveBuffer->fullIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, levelRegularIndices * sizeof(unsigned int), &(adaptiveBuffer->levelRegularIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, levelIrregularIndices * sizeof(unsigned int), &(adaptiveBuffer->levelIrregularIndices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

AdaptiveBuffer *AdaptiveLevel::genAdaptiveBuffer(VerticesBuffer *vertsBuffer) {
	AdaptiveBuffer *buffer = new AdaptiveBuffer();

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
			//bool render = true;

			for (unsigned int vert = 0; vert < 4; vert++) {
				PositionPtr pos = face->pos[vert];

				if (vertsBuffer->vertexValenceIndices[pos->idx] == 0) {
					vertsBuffer->vertexValenceIndices[pos->idx] = vertsBuffer->vertexValence.size();

					vertsBuffer->vertexValence.push_back(pos->edges.size());

					/*
					for (auto edgeIt = pos->edges.begin(); edgeIt != pos->edges.end(); edgeIt++) {
						if ((*edgeIt)->faceCount == 1) {
							render = false;
						}
					}

					if (!render) {
						break;
					}
					*/

					DirFace dirFace(face, vert);
					DirFace currFace = dirFace;

					//do {
					for (int edge = 0; edge < pos->edges.size(); edge++) {
						vertsBuffer->vertexValence.push_back(currFace.next()->idx);
						vertsBuffer->vertexValence.push_back(currFace.diag()->idx);

						currFace = currFace.prevEdge().adjFace();
					}
					//} while(currFace != dirFace);
				}
			}

			//if (render) {
				buffer->levelIrregularIndices.push_back(face->pos[0]->idx);
				buffer->levelIrregularIndices.push_back(face->pos[1]->idx);
				buffer->levelIrregularIndices.push_back(face->pos[2]->idx);

				buffer->levelIrregularIndices.push_back(face->pos[2]->idx);
				buffer->levelIrregularIndices.push_back(face->pos[3]->idx);
				buffer->levelIrregularIndices.push_back(face->pos[0]->idx);
			//}
		}
	}

	return buffer;
}

void AdaptiveLevel::genCatmullClarkTable() {
	if (topologyBuffer != NULL) {
		delete topologyBuffer;
	}
	topologyBuffer = new TopologyBuffer;

	for (auto faceIt = subFaces.begin(); faceIt != subFaces.end(); faceIt++) {
		FacePtr face = *faceIt;

		topologyBuffer->adjacencyIndices.push_back(topologyBuffer->adjacency.size());
		topologyBuffer->adjacency.push_back(face->newIdx);
		topologyBuffer->adjacency.push_back(face->numVertices);

		for (int vert = 0; vert < face->numVertices; vert++) {
			topologyBuffer->adjacency.push_back(face->pos[vert]->idx);
		}
	}

	for (auto edgeIt = subEdges.begin(); edgeIt != subEdges.end(); edgeIt++) {
		EdgePtr edge = *edgeIt;

		topologyBuffer->adjacencyIndices.push_back(topologyBuffer->adjacency.size());

		topologyBuffer->adjacency.push_back(edge->newIdx);

		if (edge->faceCount == 2) {
			topologyBuffer->adjacency.push_back(4);

			topologyBuffer->adjacency.push_back(edge->faces[0]->newIdx);
			topologyBuffer->adjacency.push_back(edge->faces[1]->newIdx);
		} else {
			topologyBuffer->adjacency.push_back(2);
		}

		topologyBuffer->adjacency.push_back(edge->v0->idx);
		topologyBuffer->adjacency.push_back(edge->v1->idx);
	}

	for (auto vertexIt = subVertices.begin(); vertexIt != subVertices.end(); vertexIt++) {
		PositionPtr pos = *vertexIt;

		topologyBuffer->adjacencyIndices.push_back(topologyBuffer->adjacency.size());

		topologyBuffer->adjacency.push_back(pos->newIdx);

		if (pos->edges.size() > pos->faces.size()) {
			topologyBuffer->adjacency.push_back(2);

			int creaseEdges = 0;

			for (auto edgeIt = pos->edges.begin(); edgeIt != pos->edges.end(); edgeIt++) {
				if ((*edgeIt)->faceCount == 1) {
					creaseEdges++;

					topologyBuffer->adjacency.push_back((*edgeIt)->other(pos)->idx);

					if (creaseEdges > 2) {
						break;
					}
				}
			}
		} else {
			topologyBuffer->adjacency.push_back(pos->edges.size());

			for (auto edgeIt = pos->edges.begin(); edgeIt != pos->edges.end(); edgeIt++) {
				topologyBuffer->adjacency.push_back((*edgeIt)->other(pos)->idx);
			}

			for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
				topologyBuffer->adjacency.push_back((*faceIt)->newIdx);
			}
		}

		topologyBuffer->adjacency.push_back(pos->idx);
	}
}

void AdaptiveLevel::genTopologyBuffers() {
	glGenBuffers(2, &vbo[5]);
}

void AdaptiveLevel::setTopologyBuffers() {
	adjacency = topologyBuffer->adjacency.size();
	adjacencyIndices = topologyBuffer->adjacencyIndices.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, adjacencyBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, adjacency * sizeof(unsigned int), &(topologyBuffer->adjacency[0]), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, adjacencyIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, adjacencyIndices * sizeof(unsigned int), &(topologyBuffer->adjacencyIndices[0]), GL_DYNAMIC_DRAW);
}
