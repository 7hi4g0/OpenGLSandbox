#include <glcommon.h>

#ifndef GL_GLEXT_PROTOTYPES
PFNGLATTACHSHADERPROC 				glAttachShader;
PFNGLBINDBUFFERPROC 				glBindBuffer;
PFNGLBINDBUFFERBASEPROC				glBindBufferBase;
PFNGLBINDPROGRAMPIPELINEPROC 		glBindProgramPipeline;
PFNGLBINDVERTEXARRAYPROC 			glBindVertexArray;
PFNGLBUFFERDATAPROC 				glBufferData;
PFNGLCOMPILESHADERPROC				glCompileShader;
PFNGLCREATEPROGRAMPROC 				glCreateProgram;
PFNGLCREATESHADERPROC 				glCreateShader;
PFNGLDELETESHADERPROC 				glDeleteShader;
PFNGLENABLEVERTEXATTRIBARRAYPROC 	glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC 				glGenBuffers;
PFNGLGENPROGRAMPIPELINESPROC 		glGenProgramPipelines;
PFNGLGENVERTEXARRAYSPROC 			glGenVertexArrays;
PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC				glGetProgramiv;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC	glGetProgramPipelineInfoLog;
PFNGLGETPROGRAMPIPELINEIVPROC 		glGetProgramPipelineiv;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
PFNGLGETSHADERIVPROC				glGetShaderiv;
PFNGLGETSTRINGIPROC					glGetStringi;
PFNGLGETUNIFORMLOCATIONPROC 		glGetUniformLocation;
PFNGLLINKPROGRAMPROC				glLinkProgram;
PFNGLMEMORYBARRIERPROC				glMemoryBarrier;
PFNGLPATCHPARAMETERIPROC 			glPatchParameteri;
PFNGLPROGRAMPARAMETERIPROC 			glProgramParameteri;
PFNGLPROGRAMUNIFORM1FVPROC 			glProgramUniform1fv;
PFNGLPROGRAMUNIFORM3FVPROC 			glProgramUniform3fv;
PFNGLPROGRAMUNIFORM4FVPROC 			glProgramUniform4fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC 	glProgramUniformMatrix4fv;
PFNGLSHADERSOURCEPROC				glShaderSource;
PFNGLUSEPROGRAMSTAGESPROC 			glUseProgramStages;
PFNGLVALIDATEPROGRAMPIPELINEPROC	glValidateProgramPipeline;
PFNGLVERTEXATTRIBPOINTERPROC 		glVertexAttribPointer;
#endif

void checkGLErr(const char *file, int line) {
	GLenum glErr;

	glErr = glGetError();

	if (glErr != GL_NO_ERROR) {
		cerr << file << "," << line << " - OpenGL Error: " << glErr << endl;
		exit(1);
	}
}

#ifndef GL_GLEXT_PROTOTYPES
void initGLFunctions() {
	getFunction(PFNGLATTACHSHADERPROC, 				glAttachShader);
	getFunction(PFNGLBINDBUFFERPROC, 				glBindBuffer);
	getFunction(PFNGLBINDBUFFERBASEPROC, 			glBindBufferBase);
	getFunction(PFNGLBINDPROGRAMPIPELINEPROC, 		glBindProgramPipeline);
	getFunction(PFNGLBINDVERTEXARRAYPROC, 			glBindVertexArray);
	getFunction(PFNGLBUFFERDATAPROC, 				glBufferData);
	getFunction(PFNGLCOMPILESHADERPROC,				glCompileShader);
	getFunction(PFNGLCREATEPROGRAMPROC,				glCreateProgram);
	getFunction(PFNGLCREATESHADERPROC, 				glCreateShader);
	getFunction(PFNGLDELETESHADERPROC, 				glDeleteShader);
	getFunction(PFNGLENABLEVERTEXATTRIBARRAYPROC, 	glEnableVertexAttribArray);
	getFunction(PFNGLGENBUFFERSPROC, 				glGenBuffers);
	getFunction(PFNGLGENPROGRAMPIPELINESPROC, 		glGenProgramPipelines);
	getFunction(PFNGLGENVERTEXARRAYSPROC, 			glGenVertexArrays);
	getFunction(PFNGLGETPROGRAMINFOLOGPROC,			glGetProgramInfoLog);
	getFunction(PFNGLGETPROGRAMIVPROC,				glGetProgramiv);
	getFunction(PFNGLGETPROGRAMPIPELINEINFOLOGPROC,	glGetProgramPipelineInfoLog);
	getFunction(PFNGLGETPROGRAMPIPELINEIVPROC, 		glGetProgramPipelineiv);
	getFunction(PFNGLGETSHADERINFOLOGPROC,			glGetShaderInfoLog);
	getFunction(PFNGLGETSHADERIVPROC,				glGetShaderiv);
	getFunction(PFNGLGETSTRINGIPROC,				glGetStringi);
	getFunction(PFNGLGETUNIFORMLOCATIONPROC, 		glGetUniformLocation);
	getFunction(PFNGLLINKPROGRAMPROC,				glLinkProgram);
	getFunction(PFNGLMEMORYBARRIERPROC,				glMemoryBarrier);
	getFunction(PFNGLPATCHPARAMETERIPROC, 			glPatchParameteri);
	getFunction(PFNGLPROGRAMPARAMETERIPROC, 		glProgramParameteri);
	getFunction(PFNGLPROGRAMUNIFORM1FVPROC, 		glProgramUniform1fv);
	getFunction(PFNGLPROGRAMUNIFORM3FVPROC, 		glProgramUniform3fv);
	getFunction(PFNGLPROGRAMUNIFORM4FVPROC, 		glProgramUniform4fv);
	getFunction(PFNGLPROGRAMUNIFORMMATRIX4FVPROC, 	glProgramUniformMatrix4fv);
	getFunction(PFNGLSHADERSOURCEPROC,				glShaderSource);
	getFunction(PFNGLUSEPROGRAMSTAGESPROC, 			glUseProgramStages);
	getFunction(PFNGLVALIDATEPROGRAMPIPELINEPROC,	glValidateProgramPipeline);
	getFunction(PFNGLVERTEXATTRIBPOINTERPROC, 		glVertexAttribPointer);
}
#else
void initGLFunctions() {
}
#endif
