#include <gpusubd.h>

AdaptiveSubSurf::AdaptiveSubSurf(const char * const filename) {
	AdaptiveLevel *baseLevel = new AdaptiveLevel;

	baseLevel->mesh = loadObjModel(filename);

	unsigned int index = 0;

	for (auto vertIt = baseLevel->mesh->pos.begin(); vertIt != baseLevel->mesh->pos.end(); vertIt++) {
		PositionPtr vert = *vertIt;

		vert->idx = index++;
	}

	subLevels.push_back(baseLevel);
	levels = 1;
	vertsBuffer = new VerticesBuffer;

	createShaders();
}

void AdaptiveSubSurf::subdivide(int levels) {
	AdaptiveLevel *sub;

	// TODO: Verify levels parameter
	// Should I allow regeneration? If so, how should I do it?
	if (subLevels.size() > 1) {
		// TODO: Put cout, cerr, endl, exit, etc in a common.h
		cerr << "Already subdivided" << endl;
		exit(1);
	}

	unsigned int vertices = 0;

	vertices = subLevels.back()->mesh->pos.size();

	for (int level = 1; level < levels; level++) {
		sub = AdaptiveCatmullClark(subLevels.back(), vertices);
		subLevels.push_back(sub);
		vertices += sub->mesh->pos.size();
	}

	vertsBuffer->verts.resize(vertices);

	for (int level = 0; level < levels; level++) {
		Model *mesh = subLevels[level]->mesh;

		for (auto vertIt = mesh->pos.begin(); vertIt != mesh->pos.end(); vertIt++) {
			PositionPtr vert = *vertIt;

			vertsBuffer->verts[vert->idx] = vert->v;
			vertsBuffer->vertexValenceIndices.push_back(0);
		}
	}

	this->levels = levels;
}

void AdaptiveSubSurf::genCatmullClarkTables() {
	for (int level = 0; level < levels - 1; level++) {
		AdaptiveLevel *subLevel = subLevels[level];

		subLevel->genTopologyBuffers();
		subLevel->genCatmullClarkTable();
		subLevel->setTopologyBuffers();
	}
}

void AdaptiveSubSurf::gpuCatmullClark() {

	for (int level = 0; level < levels - 1; level++) {
		AdaptiveLevel *subLevel = subLevels[level];

		GLuint faces = subLevel->subFaces.size();
		GLuint edges = subLevel->subEdges.size();
		GLuint vertices = subLevel->subVertices.size();

		glProgramUniform1ui(facesProgram, 0, faces);	GLERR();
		glProgramUniform1ui(edgesProgram, 0, faces);	GLERR();
		glProgramUniform1ui(edgesProgram, 1, edges);	GLERR();
		glProgramUniform1ui(verticesProgram, 0, faces);	GLERR();
		glProgramUniform1ui(verticesProgram, 1, edges);	GLERR();
		glProgramUniform1ui(verticesProgram, 2, vertices);	GLERR();

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, subLevel->posBuffer);	GLERR();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, subLevel->adjacencyBuffer);	GLERR();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, subLevel->adjacencyIndicesBuffer);	GLERR();

		glBindProgramPipeline(0);	GLERR();

		glUseProgram(facesProgram);	GLERR();
		glDispatchCompute(ceil(faces / 8.0), 8, 1);	GLERR();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);	GLERR();

		glUseProgram(edgesProgram);	GLERR();
		glDispatchCompute(ceil(edges / 8.0), 8, 1);	GLERR();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);	GLERR();

		glUseProgram(verticesProgram);	GLERR();
		glDispatchCompute(ceil(vertices / 8.0), 8, 1);	GLERR();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);	GLERR();

		glUseProgram(0);	GLERR();

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);	GLERR();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);	GLERR();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);	GLERR();
	}
}

void AdaptiveSubSurf::genBuffers() {
	glGenBuffers(3, &vbo[0]);

	vertsBuffer->vertexValence.push_back(0); // pad

	int level = 1;
	for (auto levelIt = subLevels.begin(); levelIt != subLevels.end(); levelIt++) {
		(*levelIt)->genBuffers(verticesBuffer, valenceIndicesBuffer);
		(*levelIt)->genFaceSets();
		(*levelIt)->setBuffersData(vertsBuffer);

		cout << "Level " << level++ << endl;
		cout << "Full indices: " << (*levelIt)->fullIndices << endl;
	}

	vertices = vertsBuffer->verts.size();
	vertexValence = vertsBuffer->vertexValence.size();
	vertexValenceIndices = vertsBuffer->vertexValenceIndices.size();

	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vertex), &(vertsBuffer->verts[0]), GL_STATIC_DRAW);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, valenceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vertexValence * sizeof(unsigned int), &(vertsBuffer->vertexValence[0]), GL_STATIC_DRAW);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_ARRAY_BUFFER, valenceIndicesBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexValenceIndices * sizeof(unsigned int), &(vertsBuffer->vertexValenceIndices[0]), GL_STATIC_DRAW);

	cout << vertices << endl;
}

AdaptiveLevel *AdaptiveCatmullClark(AdaptiveLevel *prevLevel, unsigned int vertexOffset) {
	AdaptiveLevel *newLevel = new AdaptiveLevel;

	for (auto faceIt = prevLevel->mesh->faces.begin(); faceIt != prevLevel->mesh->faces.end(); faceIt++) {
		FacePtr face = *faceIt;

		if (face->numVertices != 4) {
			prevLevel->addFaceRing(face);

			// Mark adjacents for subdivision
			for (int vert = 0; vert < face->numVertices; vert++) {
				PositionPtr pos = face->pos[vert];

				for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
					FacePtr face = *faceIt;

					prevLevel->addFaceRing(face);
				}
			}
		}
	}

	for (auto posIt = prevLevel->mesh->pos.begin(); posIt != prevLevel->mesh->pos.end(); posIt++) {
		PositionPtr pos = *posIt;

		if (pos->isExtraordinary()) {
			for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
				FacePtr face = *faceIt;

				prevLevel->addFaceRing(face);
			}
		}
	}

	unsigned int index = vertexOffset;

	for (auto faceIt = prevLevel->subFaces.begin(); faceIt != prevLevel->subFaces.end(); faceIt++) {
		FacePtr face = *faceIt;

		face->newIdx = index++;
	}

	for (auto edgeIt = prevLevel->subEdges.begin(); edgeIt != prevLevel->subEdges.end(); edgeIt++) {
		EdgePtr edge = *edgeIt;

		edge->newIdx = index++;
	}

	for (auto vertexIt = prevLevel->subVertices.begin(); vertexIt != prevLevel->subVertices.end(); vertexIt++) {
		PositionPtr vertex = *vertexIt;

		vertex->newIdx = index++;
	}

	newLevel->mesh = Subdivide(&prevLevel->subFaces);
}

void AdaptiveSubSurf::createShaders() {
	GLuint facesCShader;
	GLuint edgesCShader;
	GLuint verticesCShader;

#define SHADER_DIR	"../shaders/"

	facesCShader = glCreateShader(GL_COMPUTE_SHADER);
	edgesCShader = glCreateShader(GL_COMPUTE_SHADER);
	verticesCShader = glCreateShader(GL_COMPUTE_SHADER);

	setShader(facesCShader, SHADER_DIR "faces.comp");	GLERR();
	setShader(edgesCShader, SHADER_DIR "edges.comp");	GLERR();
	setShader(verticesCShader, SHADER_DIR "vertices.comp");	GLERR();

	facesProgram = glCreateProgram();
	edgesProgram = glCreateProgram();
	verticesProgram = glCreateProgram();

	glAttachShader(facesProgram, facesCShader);	GLERR();
	glAttachShader(edgesProgram, edgesCShader);	GLERR();
	glAttachShader(verticesProgram, verticesCShader);	GLERR();

	linkProgram(facesProgram);	GLERR();
	linkProgram(edgesProgram);	GLERR();
	linkProgram(verticesProgram);	GLERR();

	glDeleteShader(facesCShader);
	glDeleteShader(edgesCShader);
	glDeleteShader(verticesCShader);
}
