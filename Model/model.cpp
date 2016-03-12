#include "model.h"

bool Face::operator<(const Face f) const {
	return numVertices < f.numVertices || (numVertices == f.numVertices && pos[0].get() < f.pos[0].get());
}

Position Face::facePos() const {
	Position facePos;

	facePos = *pos[0] + *pos[1] + *pos[2];

	if (numVertices == 4) {
		facePos = facePos + *pos[3];
	}
	facePos = facePos / numVertices;

	return facePos;
}

bool Edge::operator==(const Edge e) const {
	return (e.v0 == v0 && e.v1 == v1) || (e.v0 == v1 && e.v1 == v0);
}

bool Edge::operator!=(const Edge e) const {
	return (e.v0 != v0 || e.v1 != v1) && (e.v0 != v1 || e.v1 != v0);
}

bool Edge::operator<(const Edge e) const {
	//return (*v0 < *e.v0 || (*v0 == *e.v0 && *v1 < *e.v1)) && (*v0 < *e.v1 || (*v0 == *e.v1 && *v1 < *e.v0));
	if (*v0 < *v1) {
		if (*e.v0 < *e.v1) {
			return (*v0 < *e.v0) || (*v0 == *e.v0 && *v1 < *e.v1);
		} else {
			return (*v0 < *e.v1) || (*v0 == *e.v1 && *v1 < *e.v0);
		}
	} else {
		if (*e.v0 < *e.v1) {
			return (*v1 < *e.v0) || (*v1 == *e.v0 && *v0 < *e.v1);
		} else {
			return (*v1 < *e.v1) || (*v1 == *e.v1 && *v0 < *e.v0);
		}
	}
}

Position Edge::edgePos() const {
	Position edgePos;

	edgePos = *v0 + *v1;

	if (faceCount == 1) {
		edgePos = edgePos / 2;
	} else {
		edgePos = (edgePos + faces[0]->facePos() + faces[1]->facePos()) / 4;
	}

	return edgePos;
}

Position Edge::midPos() const {
	Position midPos;

	midPos = (*v0 + *v1) / 2;

	return midPos;
}

bool EdgeCompare::operator() (const EdgePtr& e, const EdgePtr& f) {
	return *e < *f;
}

std::istream& operator>>(std::istream& is, Vertex& v) {
	is >> v.x >> v.y >> v.z;
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
	os << v.x << " " << v.y << " " << v.z;
}

bool Vertex::operator==(const Vertex v) {
	return x == v.x && y == v.y && z == v.z;
}

bool Vertex::operator<(const Vertex v) const {
	return (x < v.x || (x == v.x && y < v.y) || (x == v.x && y == v.y && z < v.z));
}

Vertex Vertex::operator+(const Vertex v) const {
	Vertex ret = *this;

	ret.x += v.x;
	ret.y += v.y;
	ret.z += v.z;

	return ret;
}

Vertex Vertex::operator-(const Vertex v) const {
	Vertex ret = *this;

	ret.x -= v.x;
	ret.y -= v.y;
	ret.z -= v.z;

	return ret;
}

Vertex Vertex::operator/(const float n) const {
	Vertex ret = *this;

	ret.x /= n;
	ret.y /= n;
	ret.z /= n;

	return ret;
}

Vertex Vertex::operator*(const float n) const {
	Vertex ret = *this;

	ret.x *= n;
	ret.y *= n;
	ret.z *= n;

	return ret;
}

Vertex Vertex::cross(const Vertex v) const {
	Vertex ret;
	Vertex u = *this;

	ret.x = (u.y * v.z) - (u.z * v.y);
	ret.y = (u.z * v.x) - (u.x * v.z);
	ret.z = (u.x * v.y) - (u.y * v.x);

	return ret;
}

Vertex Vertex::normalize() const {
	Vertex ret = *this;
	float length;

	length = sqrt((ret.x * ret.x) + (ret.y * ret.y) + (ret.z * ret.z));

	ret.x /= length;
	ret.y /= length;
	ret.z /= length;

	return ret;
}

Vertex Vertex::normal(const Vertex& point, const Vertex& next, const Vertex& previous) {
	Vertex edge1;
	Vertex edge2;

	edge1 = next - point;
	edge2 = previous - point;

	return edge1.cross(edge2).normalize();
}

bool VertexCompare::operator() (const VertexPtr& u, const VertexPtr& v) {
	return *u < *v;
}

Position& Position::operator=(const Position& right) {
	v = right.v;

	return (*this);
}

bool Position::operator==(const Position p) {
	return v == p.v;
}

bool Position::operator<(const Position p) const {
	return v < p.v;
}

Position Position::operator+(const Position p) const {
	Position ret = *this;

	ret.v = ret.v + p.v;

	return ret;
}

Position Position::operator/(const float n) const {
	Position ret = *this;

	ret.v = ret.v / n;

	return ret;
}

Position Position::operator*(const float n) const {
	Position ret = *this;

	ret.v = ret.v * n;

	return ret;
}

Position Position::vertexPos() const {
	Position vertexPos;
	EdgeSet::iterator edgeIt;
	FaceSet::iterator faceIt;
	int degree;

	degree = faces.size();

	vertexPos = *this;

	if (edges.size() > degree) {
		degree = 1;

		for (edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++) {
			if ((*edgeIt)->faceCount == 1) {
				vertexPos = vertexPos + (*edgeIt)->midPos();
				degree++;
			}
		}

		vertexPos = vertexPos / degree;
	} else {
		vertexPos = vertexPos * (degree - 3);

		for (faceIt = faces.begin(); faceIt != faces.end(); faceIt++) {
			vertexPos = vertexPos + ((*faceIt)->facePos() / degree);
		}

		for (edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++) {
			vertexPos = vertexPos + ((*edgeIt)->midPos() * 2.0f / degree);
		}

		vertexPos = vertexPos / degree;
	}

	return vertexPos;
}

bool PositionCompare::operator() (const PositionPtr& p, const PositionPtr& q) {
	return *p < *q;
}

ModelBuffer *Model::genBuffer() {
	ModelBuffer *buffer = new ModelBuffer;
	FaceSet::iterator faceIt;
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
			buffer->pos.push_back(face->pos[vert]->v);
			buffer->normals.push_back(*face->normals[vert]);
			indices->push_back(index++);
		}
	}

	return buffer;
}

Model *loadObjModel(const char * const filename) {
	Model *model = new Model;
	std::vector<Vertex> vertex;
	std::vector<Vertex> normals;

	std::ifstream file;
	file.open(filename);

	std::string type;

	while (file >> type) {
		if (type.compare("#") == 0) {
			file.ignore(65535, '\n');
		} else if (type.compare("v") == 0) {
			Vertex vert;

			file >> vert;
			vertex.push_back(vert);
		} else if (type.compare("vn") == 0) {
			Vertex vert;

			file >> vert;
			normals.push_back(vert);
		} else if (type.compare("f") == 0) {
			std::string line;

			std::getline(file, line);

			std::istringstream indices(line);
			std::string index;
			FacePtr face(new Face{0});

			while (indices >> index) {
				int posIndex;
				int normIndex;
				Position pos;

				sscanf(index.c_str(), "%d//%d", &posIndex, &normIndex);

				pos.v = vertex[posIndex - 1];
				face->pos[face->numVertices] = *model->pos.insert(std::make_shared<Position>(pos)).first;

				face->normals[face->numVertices] = *model->normals.insert(std::make_shared<Vertex>(normals[normIndex - 1])).first;

				face->numVertices++;
			}

			if (face->numVertices > 4 && face->numVertices < 3) {
				cerr << "Unsupported face type: " << face->numVertices << " vertices" << endl;
				exit(1);
			}

			for (int i = 0; i < face->numVertices; i++) {
				EdgePtr edge(new Edge{0});

				edge->v0 = face->pos[i];
				edge->v1 = face->pos[(i + 1) % face->numVertices];

				edge = *model->edges.insert(edge).first;

				edge->v0->edges.insert(edge);
				edge->v1->edges.insert(edge);

				face->edges[i] = edge;
				face->pos[i]->faces.insert(face);

				if (edge->faceCount >= 2) {
					cerr << "Unsupported edge type!" << endl << "Edge can only belong to a maximum of two faces." << endl;
					exit(1);
				}

				edge->faces[edge->faceCount++] = face;
			}

			model->faces.insert(face);
		}
	}

	return model;
}
