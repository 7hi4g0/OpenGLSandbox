#include "model.h"

int main() {
	loadObjModel("../Models/cube.obj");
	return 0;
}

void loadObjModel(const char * const filename) {
	std::vector<Position> *vertex = new std::vector<Position>();
	std::vector<Position> *normals = new std::vector<Position>();

	std::ifstream file;
	file.open(filename);

	std::string type;

	while (file >> type) {
		if (type.compare("#") == 0) {
			file.ignore(65535, '\n');
		} else if (type.compare("v") == 0) {
			Position pos;

			file >> pos;
			vertex->push_back(pos);
		}
	}

	for (int index = 0; index < vertex->size(); index++) {
		cout << (*vertex)[index] << endl;
	}
}
