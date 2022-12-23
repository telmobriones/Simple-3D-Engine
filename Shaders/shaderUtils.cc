#include <cstdio>
#include <cstring>
#include <cassert>

#include "shaderUtils.h"

// Print error and delete shader

// Compile the shader in shaderText. Dies if compilation error.
static GLuint CompileShader(const char *shaderFilename, GLenum shaderType,
							const char *shaderText);

static void die_compile_error( GLuint shader, const char *shaderFilename ) {
	GLint infoLogLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

	GLchar *strInfoLog = new GLchar[ infoLogLength + 1 ];
	glGetShaderInfoLog(shader, infoLogLength, &infoLogLength, strInfoLog);
	strInfoLog[infoLogLength] = 0;
	fprintf(stdout, "\n[E] %s compilation error:\n%s\n", shaderFilename, strInfoLog);
	delete [] strInfoLog;
	glDeleteShader(shader);
	exit(1);
}

// Print error and delete program

static void die_link_error( GLuint program, const char *progamName ) {

	GLint infoLogLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	GLchar *strInfoLog = new GLchar[ infoLogLength + 1 ];
	glGetProgramInfoLog(program, infoLogLength, &infoLogLength, strInfoLog);
	strInfoLog[infoLogLength] = 0;
	fprintf(stdout, "\n[E] %s linking error:\n%s\n", progamName, strInfoLog);
	delete [] strInfoLog;
	glDeleteProgram(program);
	exit(1);
}

static int test_shader_compile(GLuint shader, const char *shaderFilename) {
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_FALSE) return 0;
	die_compile_error(shader, shaderFilename);
	return 1;
}

static int test_shader_link(GLuint program, const char *programName) {
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status != GL_FALSE) return 0;
	die_link_error(program, programName);
	return 1;
}


GLuint LoadShader(GLenum eShaderType, const char *shaderFilename) {

	FILE *fh;
	long int fileSize;
	long int bytesRead;
	char *body;
	GLuint shader_obj = 0;

	fh=fopen(shaderFilename,"r");

	if (fh ==NULL) {
		fprintf(stderr, "\n[E] LoadShader: can't open %s\n",shaderFilename);
		exit(1);
	}
	fseek(fh, 0, SEEK_END);
	fileSize = ftell(fh);
	rewind(fh);
	if (fileSize == 0) {
		fprintf(stderr, "\n[E] LoadShader: %s is empty\n",shaderFilename);
		exit(1);
	}
	body = new char[fileSize + 1];
	bytesRead = fread(body, 1, fileSize, fh);
	fclose(fh);
	assert(bytesRead <= fileSize);
	body[bytesRead] = 0;
	shader_obj = CompileShader(shaderFilename, eShaderType, body);
	delete [] body;
	return shader_obj;
}


static GLuint CompileShader(const char *shaderFilename, GLenum shaderType,
							const char *shaderText) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar **) &shaderText, NULL);
	glCompileShader(shader);
	test_shader_compile(shader, shaderFilename);
	return shader;
}

GLuint CreateProgramFromObjects(const char *programName, GLuint shaderOne, GLuint shaderTwo ) {
	GLuint program = glCreateProgram();

	glAttachShader(program, shaderOne);
	glAttachShader(program, shaderTwo);

	// Set attributes

	// 0-11  (vertex-position, 3x4)
	// 12-23 (normal, 3x4)
	// 24-31 (texture coord, 2x4)
	// 32-43 (TBN tangent, 3x4)
	// 44-55 (TBN bitangent, 3x4)
	// 56-67 (TBN normal, 3x4)
	SetProgramAttribute(program, 0, "v_position");
	SetProgramAttribute(program, 1, "v_normal");
	SetProgramAttribute(program, 2, "v_texCoord");
	SetProgramAttribute(program, 3, "v_TBN_t");
	SetProgramAttribute(program, 4, "v_TBN_b");

	glLinkProgram(program);
	test_shader_link(program, programName);
	glDetachShader(program, shaderOne);
	glDetachShader(program, shaderTwo);
	return program;
}


/////////////////////////////////////////////////////////////////7
// attribute staff

// Set attribute and check for errors

void SetProgramAttribute(GLuint program, int position, const char *attribName) {
	GLenum errorCode;
	glBindAttribLocation(program, position, attribName);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "\n[E] setting attribute \"%s\", position %d, program %d: %s\n", attribName, position, program, gluErrorString(errorCode));
		exit(1);
	}
}


/////////////////////////////////////////////////////////////////7
// uniform staff

GLint GetProgramUniform(const char *shaderName, GLuint program, const char *unifName, bool error_report) {

	GLint res;
	GLenum errorCode;

	res = glGetUniformLocation(program, unifName);
	if (res == -1) {
		if (error_report)
			fprintf(stderr, "[W] %s: uniform %s not found\n", shaderName, unifName);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[W] openGL error:: %s\n", gluErrorString(errorCode));
		}
	}
	return res;
}

void shader_set_uniform(GLint unif, const Vector3 &P) {
	int errorCode;
	if (unif != -1) {
		glUniform3f(unif, P[0], P[1], P[2]);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_1f(GLint unif, GLfloat f0) {
	int errorCode;

	if (unif != -1) {
		glUniform1f(unif, f0);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_1f: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_2f(GLint unif, GLfloat f0, GLfloat f1) {
	int errorCode;

	if (unif != -1) {
		glUniform2f(unif, f0, f1);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_2f: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_3f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2) {
	int errorCode;

	if (unif != -1) {
		glUniform3f(unif, f0, f1, f2);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_3f: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_4f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3) {
	int errorCode;

	if (unif != -1) {
		glUniform4f(unif, f0, f1, f2, f3);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_4f: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_1i(GLint unif, int f0) {
	int errorCode;

	if (unif != -1) {
		glUniform1i(unif, f0);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_1i: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_2i(GLint unif, int f0, int f1) {
	int errorCode;

	if (unif != -1) {
		glUniform2i(unif, f0, f1);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_2i: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_3i(GLint unif, int f0, int f1, int f2) {
	int errorCode;

	if (unif != -1) {
		glUniform3i(unif, f0, f1, f2);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_3i: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_4i(GLint unif, int f0, int f1, int f2, int f3) {
	int errorCode;

	if (unif != -1) {
		glUniform4i(unif, f0, f1, f2, f3);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_4i: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_3fv(GLint unif, const GLfloat *ptr) {
	int errorCode;

	if (!ptr) {
		fprintf (stderr, "[E] shader_set_uniform_3fv: NULL pointer\n");
		exit(1);
	}
	if (unif != -1) {
		glUniform3fv(unif, 1, ptr);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_3fv: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_4fv(GLint unif, const GLfloat *ptr) {
	int errorCode;

	if (!ptr) {
		fprintf (stderr, "[E] shader_set_uniform_3fv: NULL pointer\n");
		exit(1);
	}

	if (unif != -1) {
		glUniform4fv(unif, 1, ptr);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_4fv: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

void shader_set_uniform_matrix4(GLint unif, const GLfloat *ptr) {
	int errorCode;

	if (!ptr) {
		fprintf (stderr, "[E] shader_set_uniform_3fv: NULL pointer\n");
		exit(1);
	}

	if (unif != -1) {
		glUniformMatrix4fv(unif, 1, GL_FALSE, ptr);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[E] shader_set_uniform_matrix4: %s\n", gluErrorString(errorCode));
			exit(1);
		}
	}
}

// unused functions

/* GLuint CompileShaderFromLines(GLenum shaderType, char **shaderLines, size_t numLines, */
/*							  int *errorCode) { */
/*   GLuint shader = glCreateShader(shaderType); */
/*   glShaderSource(shader, numLines, (const GLchar **) shaderLines, NULL); */
/*   glCompileShader(shader); */
/*   ShaderCompileFailed(shader); */
/*   return shader; */
/* } */

/* GLuint CreateProgramFromStrings(char *programName, char *vertexShader, char *fragmentShader ) { */
/*   GLuint vertShader; */
/*   GLuint fragShader; */
/*   GLuint program; */

/*   vertShader = CompileShader(GL_VERTEX_SHADER, vertexShader, errorCode); */
/*   if (*errorCode) return (GLuint) 0; */
/*   fragShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader, errorCode); */
/*   if (*errorCode) { */
/*	  glDeleteShader(vertShader); */
/*	  return (GLuint) 0; */
/*   } */
/*   program = CreateProgramFromObjects(programName, vertShader, fragShader); */
/*   glDeleteShader(vertShader); */
/*   glDeleteShader(fragShader); */
/*   return program; */
/* } */
