#ifndef _GLCOMMON_H_
#define _GLCOMMON_H_

#include <iostream>
#include <cstdlib>

//#define GL_GLEXT_PROTOTYPES

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
extern	PFNGLBINDBUFFERBASEPROC				glBindBufferBase;
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
extern	PFNGLGETSTRINGIPROC					glGetStringi;
extern	PFNGLGETUNIFORMLOCATIONPROC 		glGetUniformLocation;
extern	PFNGLLINKPROGRAMPROC				glLinkProgram;
extern	PFNGLMEMORYBARRIERPROC				glMemoryBarrier;
extern	PFNGLPATCHPARAMETERIPROC 			glPatchParameteri;
extern	PFNGLPROGRAMPARAMETERIPROC 			glProgramParameteri;
extern	PFNGLPROGRAMUNIFORM1FVPROC 			glProgramUniform1fv;
extern	PFNGLPROGRAMUNIFORM3FVPROC 			glProgramUniform3fv;
extern	PFNGLPROGRAMUNIFORMMATRIX4FVPROC 	glProgramUniformMatrix4fv;
extern	PFNGLSHADERSOURCEPROC				glShaderSource;
extern	PFNGLUSEPROGRAMSTAGESPROC 			glUseProgramStages;
extern	PFNGLVALIDATEPROGRAMPIPELINEPROC	glValidateProgramPipeline;
extern	PFNGLVERTEXATTRIBPOINTERPROC 		glVertexAttribPointer;

#endif //GL_GLEXT_PROTOTYPES

void initGLFunctions();

void checkGLErr(const char *file, int line);

#endif //_GLCOMMON_H_
