#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <glcommon.h>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::stringstream;

extern int debug;
extern int verbose;

std::string readFile(const char * const filename);
void setShader(GLuint shader, const char * sourceFile);
void validatePipeline(GLint pipeline);
void linkProgram(GLuint program);

#endif //_PIPELINE_H_
