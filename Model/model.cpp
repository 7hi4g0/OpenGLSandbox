#include "model.h"

ModelBuffer *Model::genBuffer() {
	ModelBuffer *buffer = new ModelBuffer;
	std::set<FacePtr>::iterator faceIt;
	unsigned int index = 0;

	for (faceIt = this->faces.begin(); faceIt != this->faces.end(); faceIt++) {
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
			buffer->pos.push_back(*face->pos[vert]);
			buffer->normals.push_back(*face->normals[vert]);
			indices->push_back(index++);
		}
	}

	return buffer;
}

Model *loadObjModel(const char * const filename) {
	Model *model = new Model;
	std::vector<Position> vertex;
	std::vector<Position> normals;

	std::ifstream file;
	file.open(filename);

	std::string type;

	while (file >> type) {
		if (type.compare("#") == 0) {
			file.ignore(65535, '\n');
		} else if (type.compare("v") == 0) {
			Position pos;

			file >> pos;
			vertex.push_back(pos);
		} else if (type.compare("vn") == 0) {
			Position pos;

			file >> pos;
			normals.push_back(pos);
		} else if (type.compare("f") == 0) {
			std::string line;

			std::getline(file, line);

			std::istringstream indices(line);
			std::string index;
			std::pair<PositionSet::iterator, bool> posRet;
			PositionSet::iterator posIt;
			FacePtr face(new Face);

			face->numVertices = 0;

			while (indices >> index) {
				int posIndex;
				int normIndex;

				sscanf(index.c_str(), "%d//%d", &posIndex, &normIndex);

				posRet = model->pos.insert(std::make_shared<Position>(vertex[posIndex - 1]));
				posIt = posRet.first;
				face->pos[face->numVertices] = *posIt;
				posRet = model->normals.insert(std::make_shared<Position>(normals[normIndex - 1]));
				posIt = posRet.first;
				face->normals[face->numVertices] = *posIt;

				face->numVertices++;
			}

			if (face->numVertices > 4 && face->numVertices < 3) {
				cerr << "Unsupported face type: " << face->numVertices << " vertices" << endl;
				exit(1);
			}

			std::pair<std::set<EdgePtr>::iterator, bool> ret;
			std::set<EdgePtr>::iterator it;

			for (int i = 0; i < face->numVertices; i++) {
				std::shared_ptr<Edge> edge(new Edge);

				edge->v0 = face->pos[i];
				edge->v1 = face->pos[(i + 1) % face->numVertices];

				ret = model->edges.insert(edge);
				it = ret.first;

				face->edges[i] = *it;
			}

			model->faces.insert(face);
		}
	}

	return model;
}
