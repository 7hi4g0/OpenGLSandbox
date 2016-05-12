#ifndef _SCENE_H_
#define _SCENE_H_

#include <string>

#include <keyframe.h>

extern int verbose;
extern int debug;

struct Scene {
	const char * filename;
	std::string baseFrameFilename;
	std::string keyframePattern;
	float animationSpeed;
	uint8_t currentFrame;
	uint8_t frameCount;

	std::vector<KeyFrame*> frames;

	Scene(const char * const);
	void genKeyFrames();
	KeyFrame *currFrame();
	void animate();
};

#endif //_SCENE_H_
