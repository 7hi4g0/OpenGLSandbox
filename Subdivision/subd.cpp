#include <subd.h>

SubSurf::SubSurf(const char * const filename) {
	subLevels.push_back(loadObjModel(filename));
	levels = 0;
}

Model* SubSurf::subdivide() {
	Model *sub;

	sub = CatmullClark(subLevels.back());
	subLevels.push_back(sub);
	levels++;
	return sub;
}

Model* SubSurf::getLevel(int level) {
	return subLevels[level];
}

Model* Subdivide(Model *model) {
	Model *newModel = new Model;
	int posIndex = 0;

	for (auto faceIt = model->faces.begin(); faceIt != model->faces.end(); faceIt++) {
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
			VertexPtr normal{new Vertex};

			subface->numVertices = 4;
			from = face->edges[vertice];
			to = face->edges[(vertice + face->numVertices - 1) % face->numVertices];

			//Vertice 1
			subface->pos[0] = *newModel->pos.insert(face->pos[vertice]).first;

			//Vertice 2
			*fromPos = (*from->v0 + *from->v1) / 2;
			subface->pos[1] = *newModel->pos.insert(fromPos).first;

			//Vertice 3
			subface->pos[2] = *newModel->pos.insert(facePos).first;

			//Vertice 4
			*toPos = (*to->v0 + *to->v1) / 2;
			subface->pos[3] = *newModel->pos.insert(toPos).first;

			*normal = Vertex::normal(facePos->v, toPos->v, fromPos->v);
			normal = *newModel->normals.insert(normal).first;

			for (int i = 0; i < 4; i++) {
				EdgePtr edge;

				edge = new Edge(subface->pos[i], subface->pos[(i + 1) % 4]);

				edge = *newModel->edges.insert(edge).first;

				edge->v0->edges.insert(edge);
				edge->v1->edges.insert(edge);

				subface->normals[i] = normal;
				subface->pos[i]->faces.insert(subface);
				edge->faces[edge->faceCount] = subface;
				edge->faceCount++;
			}

			newModel->faces.insert(subface);
		}
	}

	return newModel;
}

Model* CatmullClark(Model *model) {
	Model *newModel = new Model;

	for (auto faceIt = model->faces.begin(); faceIt != model->faces.end(); faceIt++) {
		FacePtr face = *faceIt;
		PositionPtr facePos(new Position);

		*facePos = face->facePos();

		for (int vertice = 0; vertice < face->numVertices; vertice++) {
			FacePtr subface(new Face);
			EdgePtr from;
			EdgePtr to;
			PositionPtr vertexPos(new Position);
			PositionPtr fromPos(new Position);
			PositionPtr toPos(new Position);
			VertexPtr normal{new Vertex};

			subface->numVertices = 4;
			from = face->edges[vertice];
			to = face->edges[(vertice + face->numVertices - 1) % face->numVertices];

			//Vertice 1
			*vertexPos = face->pos[vertice]->vertexPos();
			subface->pos[0] = *newModel->pos.insert(vertexPos).first;

			//Vertice 2
			*fromPos = from->edgePos();
			subface->pos[1] = *newModel->pos.insert(fromPos).first;

			//Vertice 3
			subface->pos[2] = *newModel->pos.insert(facePos).first;

			//Vertice 4
			*toPos = to->edgePos();
			subface->pos[3] = *newModel->pos.insert(toPos).first;

			*normal = Vertex::normal(facePos->v, toPos->v, fromPos->v);
			normal = *newModel->normals.insert(normal).first;

			for (int i = 0; i < 4; i++) {
				EdgePtr edge;

				edge = new Edge(subface->pos[i], subface->pos[(i + 1) % 4]);

				edge = *newModel->edges.insert(edge).first;

				edge->v0->edges.insert(edge);
				edge->v1->edges.insert(edge);

				subface->edges[i] = edge;
				subface->normals[i] = normal;
				subface->pos[i]->faces.insert(subface);
				edge->faces[edge->faceCount++] = subface;
			}

			newModel->faces.insert(subface);
		}
	}

	return newModel;
}
