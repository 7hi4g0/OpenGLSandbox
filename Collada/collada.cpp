#include "collada.h"

int main() {
	getColladaModel("suzanne.dae");
	return 0;
}

void getColladaModel(const char * const filename) {
	XMLDocument doc;
	doc.LoadFile(filename);

	XMLElement *geometriesLib = doc.RootElement()->FirstChildElement("library_geometries");
	XMLElement *geometry = geometriesLib->FirstChildElement("geometry");

	cout << "Found model: " << geometry->Attribute("name") << endl;

	XMLElement *source;

	source = geometry->FirstChildElement("mesh")->FirstChildElement("source");

	getColladaSource(source);
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

	return (Source) {count, (void *) data};
}
