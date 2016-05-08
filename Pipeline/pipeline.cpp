#include <pipeline.h>

std::string readFile(const char * const filename) {
	ifstream file;
	stringstream contents;

	file.open(filename);
	contents << file.rdbuf();

	return contents.str();
}

void setShader(GLuint shader, const char * sourceFile) {
	GLint compileStatus;
	std::string source;
	const char * cSource;
	int length;

	source = readFile(sourceFile);
	length = source.length();
	cSource = source.c_str();

	if (debug >= 1) {
		cerr << "Compiling " << sourceFile << endl;
	} else if (debug > 2) {
		cerr << source << endl;
	}

	glShaderSource(shader, 1, &cSource, &length);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		cerr << "Shader Compilation Error" << endl;

		char log[5000];
		glGetShaderInfoLog(shader, 5000, NULL, log);

		cerr << log << endl;
		exit(1);
	}
}

void validatePipeline(GLint pipeline) {
	GLint valid;

	glValidateProgramPipeline(pipeline);
	glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, &valid);	GLERR();
	if (valid == GL_FALSE) {
		cerr << "Program Pipeline Error" << endl;

		char log[5000];
		glGetProgramPipelineInfoLog(pipeline, 5000, NULL, log);	GLERR();

		cerr << "Pipeline valid status: " << valid << endl << log << endl;
		exit(1);
	}
}

void linkProgram(GLuint program) {
	GLint linkStatus;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		cerr << "Program Linking Error" << endl;

		char log[5000];
		glGetProgramInfoLog(program, 5000, NULL, log);

		cerr << log << endl;
		exit(1);
	}
}
