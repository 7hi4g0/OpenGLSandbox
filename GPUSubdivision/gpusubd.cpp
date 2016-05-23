#include <gpusubd.h>

AdaptiveSubSurf::AdaptiveSubSurf(const char * const filename) {
	AdaptiveLevel *baseLevel = new AdaptiveLevel;

	baseLevel->level = loadObjModel(filename);

	subLevels.push_back(baseLevel);
	levels = 1;
}

void AdaptiveSubSurf::subdivide(int levels) {
	AdaptiveLevel *sub;

	// TODO: Verify levels parameter
	// Should I allow regeneration? If so, how should I do it?
	if (subLevels.size() > 1) {
		// TODO: Put cout, cerr, endl, exit, etc in a common.h
		cerr << "Already subdivided" << endl;
		exit(1);
	}

	for (int level = 1; level < levels; level++) {
		sub = AdaptiveCatmullClark(subLevels.back());
		subLevels.push_back(sub);
	}

	this->levels = levels;
}

void AdaptiveSubSurf::genBuffers() {
	for (auto levelIt = subLevels.begin(); levelIt != subLevels.end(); levelIt++) {
		(*levelIt)->genBuffers();
	}
}

AdaptiveLevel *AdaptiveCatmullClark(AdaptiveLevel *prevLevel) {
	AdaptiveLevel *newLevel = new AdaptiveLevel;

	for (auto faceIt = prevLevel->level->faces.begin(); faceIt != prevLevel->level->faces.end(); faceIt++) {
		FacePtr face = *faceIt;

		if (face->numVertices != 4) {
			prevLevel->addFaceRing(face);

			// Mark adjacents for subdivision
			for (int vert = 0; vert < face->numVertices; vert++) {
				PositionPtr pos = face->pos[vert];

				for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
					FacePtr face = *faceIt;

					prevLevel->addFaceRing(face);
				}
			}
		}
	}

	for (auto posIt = prevLevel->level->pos.begin(); posIt != prevLevel->level->pos.end(); posIt++) {
		PositionPtr pos = *posIt;

		if (pos->isExtraordinary()) {
			for (auto faceIt = pos->faces.begin(); faceIt != pos->faces.end(); faceIt++) {
				FacePtr face = *faceIt;

				prevLevel->addFaceRing(face);
			}
		}
	}

	newLevel->level = CatmullClark(&prevLevel->subFaces);
}
