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

			while (indices >> index) {
				int posIndex;
				int normIndex;

				sscanf(index.c_str(), "%d//%d", &posIndex, &normIndex);

				model->pos.push_back(vertex[posIndex - 1]);
				model->normals.push_back(normals[normIndex - 1]);
			}
		}
	}

	return model;
}
