#ifndef _SCENE_H_
#define _SCENE_H_

#include <keyframe.h>

struct Scene {
	float animationSpeed;
	uint8_t currentFrame;
	uint8_t frameCount;

	std::vector<KeyFrame*> frames;

	//Scene(const char * const);
};

#endif //_SCENE_H_
