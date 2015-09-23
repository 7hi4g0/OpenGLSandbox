#include <subd.h>

void Subdivide(Model *model) {
	Model newModel;
	std::set<Face>::iterator it;
	int posIndex = 0;

	for (it = model->faces.begin(); it != model->faces.end(); it++) {
		Face face = *it;
		Position facePos;

		facePos = *face.pos[0] + *face.pos[1] + *face.pos[2];

		if (face.numVertices == 4) {
			facePos = facePos + *face.pos[3];
		}
		facePos = facePos / face.numVertices;

		if (facePos.x == 0 && facePos.y == 0 && facePos.z == 0) {
			cout << *face.pos[0] << " " << *face.pos[1] << " " << *face.pos[2] << " " << *face.pos[3] << " " << endl;
		}

		for (int vertice = 0; vertice < face.numVertices; vertice++){
			Face subface;
			Edge from;
			Edge to;
			Position fromPos;
			Position toPos;
			Position normal;

			subface.numVertices = 4;
			from = *face.edges[vertice];
			to = *face.edges[(vertice - 1) % face.numVertices];

			//Vertice 1
			newModel.pos.push_back(*face.pos[vertice]);
			subface.pos[0] = &newModel.pos.back();

			//Vertice 2
			fromPos = (*from.v0 + *from.v1) / 2;
			newModel.pos.push_back(fromPos);
			subface.pos[1] = &newModel.pos.back();

			if (fromPos.x == 0 && fromPos.y == 0 && fromPos.z == 0) {
				cout << *from.v0 << " " << *from.v1 << " " << endl;
			}

			//Vertice 3
			newModel.pos.push_back(facePos);
			subface.pos[2] = &newModel.pos.back();

			//Vertice 4
			toPos = (*to.v0 + *to.v1) / 2;
			newModel.pos.push_back(toPos);
			subface.pos[3] = &newModel.pos.back();

			if (toPos.x == 0 && toPos.y == 0 && toPos.z == 0) {
				cout << *to.v0 << " " << *to.v1 << " " << endl;
			}

			normal = *face.normals[vertice];

			for (int i = 0; i < 4; i++) {
				std::pair<std::set<Edge>::iterator, bool> ret;
				std::set<Edge>::iterator it;
				Edge e;

				e.v0 = subface.pos[i];
				e.v1 = subface.pos[(i + 1) % 4];

				if (newModel.edges.count(e) == 1) {
					it = newModel.edges.find(e);
				} else {
					ret = newModel.edges.insert(e);
					it = ret.first;
				}
				newModel.normals.push_back(normal);
				newModel.quadIndices.push_back(posIndex++);

				subface.normals[i] = &newModel.normals.back();
				subface.edges[i] = &*it;
			}

			newModel.faces.insert(subface);
		}
	}

	*model = newModel;
}

void CatmullClark(Model *model) {
}
