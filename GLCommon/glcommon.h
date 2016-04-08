#ifndef _GLCOMMON_H_
#define _GLCOMMON_H_

#include <iostream>
#include <cstdlib>

#include <GL/glcorearb.h>
#include <GL/glx.h>

using std::cout;
using std::cerr;
using std::endl;

#define	GLERR()	checkGLErr(__FILE__, __LINE__)
#define getFunction(fnType, fn)										\
	fn = (fnType) glXGetProcAddressARB((const GLubyte *) #fn); GLERR();	\
	if (!fn) {															\
		cout << "Error getting fucntion address to " #fn << endl;		\
		exit(-1);														\
	}

#ifndef GL_GLEXT_PROTOTYPES

extern	PFNGLATTACHSHADERPROC 				glAttachShader;
extern	PFNGLBINDBUFFERPROC 				glBindBuffer;
extern	PFNGLBINDPROGRAMPIPELINEPROC 		glBindProgramPipeline;
extern	PFNGLBINDVERTEXARRAYPROC 			glBindVertexArray;
extern	PFNGLBUFFERDATAPROC 				glBufferData;
extern	PFNGLCOMPILESHADERPROC				glCompileShader;
extern	PFNGLCREATEPROGRAMPROC 				glCreateProgram;
extern	PFNGLCREATESHADERPROC 				glCreateShader;
extern	PFNGLDELETESHADERPROC 				glDeleteShader;
extern	PFNGLENABLEVERTEXATTRIBARRAYPROC 	glEnableVertexAttribArray;
extern	PFNGLGENBUFFERSPROC 				glGenBuffers;
extern	PFNGLGENPROGRAMPIPELINESPROC 		glGenProgramPipelines;
extern	PFNGLGENVERTEXARRAYSPROC 			glGenVertexArrays;
extern	PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
extern	PFNGLGETPROGRAMIVPROC				glGetProgramiv;
extern	PFNGLGETPROGRAMPIPELINEINFOLOGPROC	glGetProgramPipelineInfoLog;
extern	PFNGLGETPROGRAMPIPELINEIVPROC 		glGetProgramPipelineiv;
extern	PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
extern	PFNGLGETSHADERIVPROC				glGetShaderiv;
extern	PFNGLGETUNIFORMLOCATIONPROC 		glGetUniformLocation;
extern	PFNGLLINKPROGRAMPROC				glLinkProgram;
extern	PFNGLPATCHPARAMETERIPROC 			glPatchParameteri;
extern	PFNGLPROGRAMPARAMETERIPROC 			glProgramParameteri;
extern	PFNGLPROGRAMUNIFORM1FVPROC 			glProgramUniform1fv;
extern	PFNGLPROGRAMUNIFORM3FVPROC 			glProgramUniform3fv;
extern	PFNGLPROGRAMUNIFORMMATRIX4FVPROC 	glProgramUniformMatrix4fv;
extern	PFNGLSHADERSOURCEPROC				glShaderSource;
extern	PFNGLUSEPROGRAMPROC 				glUseProgram;
extern	PFNGLUSEPROGRAMSTAGESPROC 			glUseProgramStages;
extern	PFNGLVALIDATEPROGRAMPIPELINEPROC	glValidateProgramPipeline;
extern	PFNGLVERTEXATTRIBPOINTERPROC 		glVertexAttribPointer;

void initGLFunctions();

#else

void initGLFunctions() {}

#endif //GL_GLEXT_PROTOTYPES

void checkGLErr(const char *file, int line);

#endif //_GLCOMMON_H_
