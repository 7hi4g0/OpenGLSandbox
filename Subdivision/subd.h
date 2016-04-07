#include <model.h>

struct SubSurf {
	std::vector<Model*> subLevels;
	int levels;

	SubSurf(const char * const);

	Model* subdivide();
	Model* getLevel(int);
};

Model* Subdivide(Model *model);
Model* CatmullClark(Model *model);
