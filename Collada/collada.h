#include <iostream>
#include <sstream>

#include <tinyxml2.h>

using std::cout;
using std::endl;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

typedef struct {
	int count;
	const char *id;
	void *data;
} Source;

typedef struct {
	Source src[2];
} Model;

Model *getColladaModel(const char * const filename);
Source getColladaSource(XMLElement *source);
