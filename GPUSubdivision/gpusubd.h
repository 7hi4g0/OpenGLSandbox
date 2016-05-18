#ifndef _GPUSUBD_H_
#define _GPUSUBD_H_

#include <adptlevel.h>
#include <subd.h>

struct AdaptiveSubSurf {
	std::vector<AdaptiveLevel*> subLevels;
	int levels;

	AdaptiveSubSurf(const char * const);

	void subdivide(int levels);
	void genBuffers();
};

AdaptiveLevel *AdaptiveCatmullClark(AdaptiveLevel *);

#endif //_GPUSUBD_H_
