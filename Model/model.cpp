#include "model.h"

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

size_t VertexHash::operator() (const VertexPtr v) const {
	std::hash<float> float_hash;

	return ((51 + float_hash(v->x)) * 51 + float_hash(v->y)) * 51 + float_hash(v->z);
}

bool VertexEquality::operator() (const VertexPtr& u, const VertexPtr& v) const {
	return *u == *v;
}

bool Face::operator<(const Face f) const {
	return numVertices < f.numVertices || (numVertices == f.numVertices && *pos[0] < *f.pos[0]);
}

Vertex Face::facePos() const {
	Vertex facePos;

	facePos = pos[0]->v + pos[1]->v + pos[2]->v;

	if (numVertices == 4) {
		facePos = facePos + pos[3]->v;
	}
	facePos = facePos / numVertices;

	return facePos;
}

bool Face::isRegular() const {
	if (numVertices != 4) {
		return false;
	}

	for (int vert = 0; vert < numVertices; vert++) {
		if (pos[vert]->isExtraordinary()) {
			return false;
		}
	}

	return true;
}

Edge::Edge(PositionPtr p, PositionPtr q) {
	if (*p < *q) {
		v0 = p;
		v1 = q;
	} else {
		v0 = q;
		v1 = p;
	}

	faces[0] = faces[1] = NULL;
	faceCount = 0;
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

PositionPtr Edge::other(PositionPtr pos) const {
	if (v0 == pos) {
		return v1;
	}

	if (v1 == pos) {
		return v0;
	}

	cerr << "Vertex not in edge" << endl;
	exit(1);
}

Vertex Edge::edgePos() const {
	Vertex edgePos;

	edgePos = v0->v + v1->v;

	if (faceCount == 1) {
		edgePos = edgePos / 2;
	} else {
		edgePos = (edgePos + faces[0]->facePos() + faces[1]->facePos()) / 4;
	}

	return edgePos;
}

Vertex Edge::midPos() const {
	Vertex midPos;

	midPos = (v0->v + v1->v) / 2;

	return midPos;
}

size_t EdgeHash::operator() (const EdgePtr e) const {
	VertexHash vert_hash;

	return (51 + vert_hash(&e->v0->v)) * 51 + vert_hash(&e->v1->v);
}

bool EdgeEquality::operator() (const EdgePtr& e, const EdgePtr& f) const {
	return *e == *f;
}

Position& Position::operator=(const Position& right) {
	v = right.v;

	return (*this);
}

Position& Position::operator=(const Vertex& right) {
	v = right;

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

Position Position::operator-(const Position p) const {
	Position ret = *this;

	ret.v = ret.v - p.v;

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

Vertex Position::vertexPos() const {
	Vertex vertexPos;
	int degree;

	degree = faces.size();

	vertexPos = this->v;

	if (edges.size() > degree) {
		Vertex creaseEdgesPos;
		int creaseEdges;
		
		creaseEdges = 0;
		creaseEdgesPos = {0};

		for (auto edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++) {
			if ((*edgeIt)->faceCount == 1) {
				if (creaseEdges++ > 2) {
					break;
				}

				creaseEdgesPos = creaseEdgesPos + (*edgeIt)->midPos();
			}
		}

		vertexPos = (vertexPos + vertexPos + creaseEdgesPos) / 4;
	} else {
		vertexPos = vertexPos * (degree - 2);

		for (auto faceIt = faces.begin(); faceIt != faces.end(); faceIt++) {
			vertexPos = vertexPos + ((*faceIt)->facePos() / degree);
		}

		for (auto edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++) {
			vertexPos = vertexPos + (((*edgeIt)->v0->v + (*edgeIt)->v1->v - this->v) / degree);
		}

		vertexPos = vertexPos / degree;
	}

	return vertexPos;
}

Vertex Position::vertexNormal() const {
	Vertex normal{0};

	for (auto face : this->faces) {
		normal = normal + *face->normals[0];
	}

	normal = normal / this->faces.size();

	return normal;
}

bool Position::isExtraordinary() const {
	int n;
	int faceCount;

	n = edges.size();
	faceCount = faces.size();

	if (faceCount < n) {
		n *= -1;
	} else if (faceCount > n) {
		cerr << "Unsupported: More faces than edges!" << endl;
		exit(1);
	}

	switch (n) {
			// Regular
		case 4:
			// Regular boundary
		case -2:
		case -3:
			return false;
		default:
			return true;
	}
}

size_t PositionHash::operator() (const PositionPtr p) const {
	VertexHash vert_hash;

	return vert_hash(&p->v);
}

bool PositionEquality::operator() (const PositionPtr& p, const PositionPtr& q) const {
	return *p == *q;
}

Model::~Model() {
	for (auto posIt = pos.begin(); posIt != pos.end(); posIt++) {
		delete (*posIt);
	}
	for (auto normalIt = normals.begin(); normalIt != normals.end(); normalIt++) {
		delete (*normalIt);
	}
	for (auto edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++) {
		delete (*edgeIt);
	}
	for (auto faceIt = faces.begin(); faceIt != faces.end(); faceIt++) {
		delete (*faceIt);
	}
}

ModelBuffer *Model::genBuffer(bool smooth) {
	ModelBuffer *buffer = new ModelBuffer;
	unsigned int index = 0;

	for (auto faceIt = this->faces.begin(); faceIt != this->faces.end(); faceIt++) {
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
			if (smooth) {
				buffer->normals.push_back(face->pos[vert]->vertexNormal());
			} else {
				buffer->normals.push_back(*face->normals[vert]);
			}
			indices->push_back(index++);
		}
	}

	return buffer;
}

void readFacePoint(const char* point, int *posIndex, int *texIndex, int *normIndex) {
	if (sscanf(point, "%d//%d", posIndex, normIndex) == 2) {
		return;
	}

	if (sscanf(point, "%d/%d/%d", posIndex, texIndex, normIndex) == 3) {
		return;
	}

	cerr << "Unsupported face point description!" << endl << point << endl;
	exit(1);
}

Model *loadObjModel(const char * const filename) {
	Model *model = new Model;
	std::vector<Vertex> vertex;
	std::vector<Vertex> normals;

	std::ifstream file;
	file.open(filename);

	std::string type;

	while (file >> type) {
		// This seems wrong! Test it!
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
			FacePtr face(new Face);

			while (indices >> index) {
				int posIndex;
				int texIndex;
				int normIndex;
				PositionPtr pos(new Position);
				VertexPtr normal(new Vertex{0});

				// Always requires position and normal. Ignoring texture.
				readFacePoint(index.c_str(), &posIndex, &texIndex, &normIndex);

				pos->v = vertex[posIndex - 1];
				face->pos[face->numVertices] = *model->pos.insert(pos).first;

				*normal = normals[normIndex - 1];
				face->normals[face->numVertices] = *model->normals.insert(normal).first;

				face->numVertices++;
			}

			if (face->numVertices > 4 && face->numVertices < 3) {
				cerr << "Unsupported face type: " << face->numVertices << " vertices" << endl;
				exit(1);
			}

			for (int i = 0; i < face->numVertices; i++) {
				EdgePtr edge;
				std::pair<EdgeSet::iterator, bool> ret;

				edge = new Edge(face->pos[i], face->pos[(i + 1) % face->numVertices]);

				ret = model->edges.insert(edge);
				if (!ret.second) {
					delete edge;
				}
				edge = *ret.first;

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
