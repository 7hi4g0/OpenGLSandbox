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
			unsigned int vertices;
			unsigned int initialIndex;

			vertices = 0;
			initialIndex = model->pos.size();

			while (indices >> index) {
				int posIndex;
				int normIndex;

				sscanf(index.c_str(), "%d//%d", &posIndex, &normIndex);

				//TODO: Search for equal vertices and only include if its different
				model->pos.push_back(std::make_shared<Position>(vertex[posIndex - 1]));
				model->normals.push_back(std::make_shared<Position>(normals[normIndex - 1]));

				vertices++;
			}

			if (vertices > 4 && vertices < 3) {
				cerr << "Unsupported face type: " << vertices << " vertices" << endl;
				exit(1);
			}

			std::pair<std::set<EdgePtr>::iterator, bool> ret;
			std::set<EdgePtr>::iterator it;
			FacePtr face(new Face);

			face->numVertices = vertices;

			for (int i = 0; i < vertices; i++) {
				std::shared_ptr<Edge> edge(new Edge);

				edge->v0 = model->pos[initialIndex + i];
				edge->v1 = model->pos[initialIndex + ((i + 1) % vertices)];
				if (model->edges.count(edge) == 1) {
					it = model->edges.find(edge);
				} else {
					ret = model->edges.insert(edge);
					it = ret.first;
				}
				face->pos[i] = model->pos[initialIndex + i];
				face->normals[i] = model->normals[initialIndex + i];
				face->edges[i] = *it;
			}

			model->faces.insert(face);
		}
	}

	return model;
}
