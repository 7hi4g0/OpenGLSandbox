#include "collada.h"

int main() {
	Model *model;

	model = getColladaModel("suzanne.dae");

	for (int i = 0; i < 2; i++) {
		cout << "Found data: " << model->src[i].id << " with size " << model->src[i].count << endl;
	}
	return 0;
}

Model *getColladaModel(const char * const filename) {
	XMLDocument doc;
	doc.LoadFile(filename);

	XMLElement *geometriesLib = doc.RootElement()->FirstChildElement("library_geometries");
	XMLElement *geometry = geometriesLib->FirstChildElement("geometry");

	cout << "Found model: " << geometry->Attribute("name") << endl;

	Model *model = new Model;
	int srcCount = 0;
	XMLElement *meshElem;
	XMLElement *elem;

	meshElem = geometry->FirstChildElement("mesh")->FirstChildElement();

	for (; meshElem != NULL; meshElem = meshElem->NextSiblingElement()) {
		const char *name;

		name = meshElem->Name();

		if (strcmp(name, "source") == 0) {
			model->src[srcCount++] = getColladaSource(elem);
		}
	}

	return model;
}

Source getColladaSource(XMLElement *source) {
	int count;
	float *data;
	XMLElement *elem;

	elem = source->FirstChildElement("float_array");

	count = elem->IntAttribute("count");

	data = new float[count];

	std::stringstream floats(elem->GetText());

	for (int index = 0; index < count; index++) {
		floats >> data[index];
	}

	return (Source) {count, source->Attribute("id"), (void *) data};
}
