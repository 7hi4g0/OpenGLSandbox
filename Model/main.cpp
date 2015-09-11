#include "model.h"

int main() {
	Model *model = loadObjModel("../Models/cube.obj");

	int count = model->pos.size();
	

	for (int i = 0; i < count; i++) {
		cout << model->pos[i] << endl;
	}

	return 0;
}
