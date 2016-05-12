#include <fstream>
#include <cctype>

#include <scene.h>

#define	MODELS_DIR	"../Models/"

using std::cout;
using std::endl;

void trim(std::string &line) {
	if (line.length() == 0) {
		return;
	}
	while (::isspace(*(--line.end()))) {
		line.erase(--line.end());
	}
	while (::isspace(*(line.begin()))) {
		line.erase(line.begin());
	}
}

Scene::Scene(const char * const filename) {
	animationSpeed = 10;
	currentFrame = 0;

	this->filename = filename;

	std::ifstream file;
	file.open(filename);

	while (!file.eof()) {
		std::string line;

		std::getline(file, line);
		trim(line);

		if (line.length() == 0) {
			continue;
		}

		if (line[0] == '#') {
			continue;
		}

		if (line == "BeginObject") {
			while (!file.eof()) {
				std::getline(file, line);
				trim(line);

				if (line.length() == 0) {
					continue;
				}

				if (line[0] == '#') {
					continue;
				}

				if (line == "EndObject") {
					break;
				}

				std::string attr = line.substr(0, line.find('='));
				std::string value = line.substr(line.find('\"') + 1, line.rfind('\"') - line.find('\"') - 1);

				trim(attr);
				trim(value);

				if (attr == "filename") {
					baseFrameFilename = MODELS_DIR + value;
				} else if (attr == "keyframeBaseFilename") {
					keyframePattern = MODELS_DIR + value;
				} else if (attr == "keyframes") {
					frameCount = std::stoi(value);
				} else {
					// Ignore other attributes for now
				}
			}
			break; // Only read one object
		}
	}

	file.close();
}

void Scene::genKeyFrames() {
	for (uint8_t currFrame = 0; currFrame < frameCount; currFrame++) {
		KeyFrame *keyframe;
		char keyframeFilename[512];

		sprintf(keyframeFilename, keyframePattern.c_str(), currFrame);

		if (verbose) {
			cout << "Processing frame " << (int) currFrame << ", file: " << keyframeFilename << endl;
		}

		keyframe = new KeyFrame(keyframeFilename);
		keyframe->genBuffers();
		frames.push_back(keyframe);
	}
}

KeyFrame *Scene::currFrame() {
	return frames[currentFrame];
}

void Scene::animate() {
	static uint8_t dir = 1;

	currentFrame += dir;

	if (currentFrame == 0 || currentFrame == frameCount - 1) {
		dir *= -1;
	}
}
