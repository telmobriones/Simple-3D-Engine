// -*-C++-*-

#pragma once

#include <cstdio>
#include <vector>
#include "vector3.h"
#include "trfm3D.h"

/**
 * @file   shaderUtils.h
 *
 * @brief   Misc. utilities for compiling/linking and accessing shaders.
 *
 * @warning Because vertex attributes has to be set after compilation but before
 * linking, CreateProgramFromObjects sets the following attributes:
 *
 * v_position -> 0
 * v_normal   -> 1
 * v_texCoord -> 2
 * v_TBN_t    -> 3
 * v_TBN_b    -> 4
 * v_TBN_n    -> 5
 */

#include <GL/glew.h>
#include <GL/freeglut.h>

/**
 * Load and compile a vertex or fragment shader
 *
 * @param eShaderType shader type. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 * @param shaderFilename filename of the shader.
 *
 * @return the openGL shader object (dies if error).
 */
GLuint LoadShader(GLenum eShaderType, const char *shaderFilename);

/**
 * Create (link) a shader program given to openGL objects for vertex and fragment shaders, respectively.
 *
 * @param programName  the program name.
 * @param shaderTwo openGL fragment shader object.
 * @param errorCode points to error code. 0 if success.
 *
 * @return The openGL shader object (dies if error).
 */
GLuint CreateProgramFromObjects(const char *programName, GLuint shaderOne, GLuint shaderTwo );

/**
 * Set the attribute position of a given attribute.
 *
 * @param program the openGL object of the shader program.
 * @param position the attribute position.
 * @param attribName the name of the attribute.
 */
void SetProgramAttribute(GLuint program, int position, const char *attribName);

/**
 * Get the number of a given uniform
 *
 * @param shaderName the name of the shader program (for logging).
 * @param program the openGL object of the shader program.
 * @param unifName the name of the uniform.
 *
 * @return the number of the uniform or -1 if error.
 */
GLint GetProgramUniform(const char *shaderName, GLuint program, const char *unifName, bool error_report = true);

/**
 * Set a Vector3 uniform.
 *
 * @param unif the uniform.
 * @param f1 float value.
 * @param f2 float value.
 * @param f3 float value.
 */
void shader_set_uniform(GLint unif, const Vector3 &P);

/**
 * Set a 1 float uniform.
 *
 * @param unif the uniform.
 * @param f0 the float value.
 */
void shader_set_uniform_1f(GLint unif, GLfloat f0);

/**
 * Set a 2 float uniform.
 *
 * @param unif the uniform.
 * @param f1 float value.
 * @param f2 float value.
 */
void shader_set_uniform_2f(GLint unif, GLfloat f0, GLfloat f1);

/**
 * Set a 3 float uniform.
 *
 * @param unif the uniform.
 * @param f1 float value.
 * @param f2 float value.
 * @param f3 float value.
 */
void shader_set_uniform_3f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2);

/**
 * Set a 4 float uniform.
 *
 * @param unif the uniform.
 * @param f1 float value.
 * @param f2 float value.
 * @param f3 float value.
 * @param f4 float value.
 */
void shader_set_uniform_4f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);

/**
 * Set a 1 integer uniform.
 *
 * @param unif the uniform.
 * @param f0 integer value.
 */
void shader_set_uniform_1i(GLint unif, int f0);

/**
 * Set a 2 integer uniform.
 *
 * @param unif the uniform.
 * @param f0 integer value.
 * @param f1 integer value.
 */
void shader_set_uniform_2i(GLint unif, int f0, int f1);

/**
 * Set a 3 integer uniform.
 *
 * @param unif the uniform.
 * @param f0 integer value.
 * @param f1 integer value.
 * @param f2 integer value.
 */
void shader_set_uniform_3i(GLint unif, int f0, int f1, int f2);

/**
 * Set a 4 integer uniform.
 *
 * @param unif the uniform.
 * @param f0 integer value.
 * @param f1 integer value.
 * @param f2 integer value.
 * @param f3 integer value.
 */
void shader_set_uniform_4i(GLint unif, int f0, int f1, int f2, int f3);

/**
 * Set a 3-valued float vector uniform
 *
 * @param unif the uniform
 * @param ptr a pointer to 3 floats
 */
void shader_set_uniform_3fv(GLint unif, const GLfloat *ptr);

/**
 * Set a 4-valued float vector uniform
 *
 * @param unif the uniform
 * @param ptr a pointer to 4 floats
 */
void shader_set_uniform_4fv(GLint unif, const GLfloat *ptr);
/**
 * Set a 4x4 matrix uniform
 *
 * @param unif the uniform.
 * @param ptr a pointer to 16 floats (following openGL convention for 4x4 matrices).
 */
void shader_set_uniform_matrix4(GLint unif, const GLfloat *ptr);
