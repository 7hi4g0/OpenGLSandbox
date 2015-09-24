#include <subd.h>

void Subdivide(Model *model) {
	Model newModel;
	std::set<FacePtr>::iterator faceIt;
	int posIndex = 0;

	for (faceIt = model->faces.begin(); faceIt != model->faces.end(); faceIt++) {
		FacePtr face = *faceIt;
		PositionPtr facePos(new Position);

		*facePos = *face->pos[0] + *face->pos[1] + *face->pos[2];

		if (face->numVertices == 4) {
			*facePos = *facePos + *face->pos[3];
		}
		*facePos = *facePos / face->numVertices;

		for (int vertice = 0; vertice < face->numVertices; vertice++){
			FacePtr subface(new Face);
			EdgePtr from;
			EdgePtr to;
			PositionPtr fromPos(new Position);
			PositionPtr toPos(new Position);
			PositionPtr normal(new Position);

			subface->numVertices = 4;
			from = face->edges[vertice];
			to = face->edges[(vertice + face->numVertices - 1) % face->numVertices];

			//Vertice 1
			newModel.pos.push_back(face->pos[vertice]);
			subface->pos[0] = face->pos[vertice];

			//Vertice 2
			*fromPos = (*from->v0 + *from->v1) / 2;
			newModel.pos.push_back(fromPos);
			subface->pos[1] = fromPos;

			//Vertice 3
			newModel.pos.push_back(facePos);
			subface->pos[2] = facePos;

			//Vertice 4
			*toPos = (*to->v0 + *to->v1) / 2;
			newModel.pos.push_back(toPos);
			subface->pos[3] = toPos;

			normal = face->normals[vertice];

			for (int i = 0; i < 4; i++) {
				std::pair<std::set<EdgePtr>::iterator, bool> ret;
				std::set<EdgePtr>::iterator edgeIt;
				EdgePtr e(new Edge);

				e->v0 = subface->pos[i];
				e->v1 = subface->pos[(i + 1) % 4];

				//TODO: Are edges necessary in the model?
				if (newModel.edges.count(e) == 1) {
					edgeIt = newModel.edges.find(e);
				} else {
					ret = newModel.edges.insert(e);
					edgeIt = ret.first;
				}
				newModel.normals.push_back(normal);

				subface->normals[i] = normal;
				subface->edges[i] = *edgeIt;
			}

			newModel.faces.insert(subface);
		}
	}

	*model = newModel;
}

void CatmullClark(Model *model) {
}
