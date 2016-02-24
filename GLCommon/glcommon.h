#ifndef _GLCOMMON_H_
#define _GLCOMMON_H_

#include <iostream>
#include <cstdlib>

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#define	GLERR()	checkGLErr(__FILE__, __LINE__)

using std::cout;
using std::cerr;
using std::endl;

void checkGLErr(const char *file, int line);

#endif //_GLCOMMON_H_
