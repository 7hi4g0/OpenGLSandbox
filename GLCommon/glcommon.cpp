#include <glcommon.h>

PFNGLATTACHSHADERPROC 				glAttachShader;
PFNGLBINDBUFFERPROC 				glBindBuffer;
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
PFNGLGETUNIFORMLOCATIONPROC 		glGetUniformLocation;
PFNGLLINKPROGRAMPROC				glLinkProgram;
PFNGLPATCHPARAMETERIPROC 			glPatchParameteri;
PFNGLPROGRAMPARAMETERIPROC 			glProgramParameteri;
PFNGLPROGRAMUNIFORM1FVPROC 			glProgramUniform1fv;
PFNGLPROGRAMUNIFORM3FVPROC 			glProgramUniform3fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC 	glProgramUniformMatrix4fv;
PFNGLSHADERSOURCEPROC				glShaderSource;
PFNGLUSEPROGRAMPROC 				glUseProgram;
PFNGLUSEPROGRAMSTAGESPROC 			glUseProgramStages;
PFNGLVALIDATEPROGRAMPIPELINEPROC	glValidateProgramPipeline;
PFNGLVERTEXATTRIBPOINTERPROC 		glVertexAttribPointer;

void checkGLErr(const char *file, int line) {
	GLenum glErr;

	glErr = glGetError();

	if (glErr != GL_NO_ERROR) {
		cerr << file << "," << line << " - OpenGL Error: " << glErr << endl;
		exit(1);
	}
}

void initGLFunctions() {
	getFunction(PFNGLATTACHSHADERPROC, 				glAttachShader);
	getFunction(PFNGLBINDBUFFERPROC, 				glBindBuffer);
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
	getFunction(PFNGLGETUNIFORMLOCATIONPROC, 		glGetUniformLocation);
	getFunction(PFNGLLINKPROGRAMPROC,				glLinkProgram);
	getFunction(PFNGLPATCHPARAMETERIPROC, 			glPatchParameteri);
	getFunction(PFNGLPROGRAMPARAMETERIPROC, 		glProgramParameteri);
	getFunction(PFNGLPROGRAMUNIFORM1FVPROC, 		glProgramUniform1fv);
	getFunction(PFNGLPROGRAMUNIFORM3FVPROC, 		glProgramUniform3fv);
	getFunction(PFNGLPROGRAMUNIFORMMATRIX4FVPROC, 	glProgramUniformMatrix4fv);
	getFunction(PFNGLSHADERSOURCEPROC,				glShaderSource);
	getFunction(PFNGLUSEPROGRAMPROC,	 			glUseProgram);
	getFunction(PFNGLUSEPROGRAMSTAGESPROC, 			glUseProgramStages);
	getFunction(PFNGLVALIDATEPROGRAMPIPELINEPROC,	glValidateProgramPipeline);
	getFunction(PFNGLVERTEXATTRIBPOINTERPROC, 		glVertexAttribPointer);
}
