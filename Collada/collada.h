#include <iostream>
#include <sstream>

#include <tinyxml2.h>

using std::cout;
using std::endl;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

typedef struct {
	int count;
	void *data;
} Source;

void getColladaModel(const char * const filename);
Source getColladaSource(XMLElement *source);
