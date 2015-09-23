#include "model.h"

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
				model->pos.push_back(vertex[posIndex - 1]);
				model->normals.push_back(normals[normIndex - 1]);

				vertices++;
			}

			if (vertices == 4) {
				model->quadIndices.push_back(initialIndex);
				model->quadIndices.push_back(initialIndex + 1);
				model->quadIndices.push_back(initialIndex + 2);
				model->quadIndices.push_back(initialIndex + 3);
			} else if (vertices == 3) {
				model->triIndices.push_back(initialIndex);
				model->triIndices.push_back(initialIndex + 1);
				model->triIndices.push_back(initialIndex + 2);
			} else {
				cerr << "Unsupported face type: " << vertices << " vertices" << endl;
				exit(1);
			}

			std::pair<std::set<Edge>::iterator, bool> ret;
			std::set<Edge>::iterator it;
			Face face;

			face.numVertices = vertices;

			for (int i = 0; i < vertices; i++) {
				Edge edge;

				edge.v0 = &model->pos[initialIndex + i];
				edge.v1 = &model->pos[initialIndex + (i + 1) % vertices];
				if (model->edges.count(edge) == 1) {
					it = model->edges.find(edge);
				} else {
					ret = model->edges.insert(edge);
					it = ret.first;
				}
				face.pos[i] = &model->pos[initialIndex + i];
				face.edges[i] = &*it;
			}

			model->faces.insert(face);
		}
	}

	return model;
}
