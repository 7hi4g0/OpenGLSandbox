#include <subd.h>

void Subdivide(Model *model) {
	Model newModel;
	std::set<Face>::iterator it;

	for (it = model->faces.begin(); it != model->faces.end(); it++) {
		Face face = *it;
		Position facePos;

		facePos = *face.pos[0] + *face.pos[1] + *face.pos[2];

		if (face.numVertices == 4) {
			facePos = facePos + *face.pos[3];
		}
		facePos = facePos / face.numVertices;

		for (int vertice = 0; vertice < face.numVertices; vertice++){
			Face subface;
			Edge from;
			Edge to;
			Position fromPos;
			Position toPos;

			subface.numVertices = 4;
			from = *face.edges[vertice];
			to = *face.edges[(vertice - 1) % face.numVertices];

			//Vertice 1
			subface.pos[0] = face.pos[vertice];

			//Vertice 2
			fromPos = (*from.v0 + *from.v1) / 2;
			model->pos.push_back(fromPos);
			subface.pos[1] = &model->pos.back();

			//Vertice 3
			model->pos.push_back(facePos);
			subface.pos[2] = &model->pos.back();

			//Vertice 4
			toPos = (*to.v0 + *to.v1) / 2;
			model->pos.push_back(toPos);
			subface.pos[3] = &model->pos.back();

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
				subface.edges[i] = &*it;
			}

			newModel.faces.insert(subface);
		}
	}

	*model = newModel;
}

void CatmullClark(Model *model) {
}
