// -*-C++-*-

#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "uniform.h"
#include "json.h"

class ShaderProgram {

public:

	void activate();
	void deactivate();

	void add_capability(const std::string & cap);
	void add_uniform(const Uniform & u);

	/**
	 * Whether or not the current shader program has certain capability.
	 */
	bool has_capability(const std::string & cap) const;

	/**
	 * Set-up shader uniforms with state information. Call it before drawing
	 * anything.
	 */
	void beforeDraw();

	const char *getName() const;
	const std::string &getNameString() const;
	friend class ShaderManager;
	void print() const;

private:

	static ShaderProgram *from_json(const std::string & src);
	static ShaderProgram *from_json(Json::Value & shaderjs);

	/**
	 * Create a Shader Program
	 *
	 * @param shaderName name of the shader program.
	 * @param vShaderFile filename of the vertex shader.
	 * @param fShaderFile filename of the fragment shader
	 *
	 * @return the newly created shader program
	 */
	ShaderProgram(const std::string & name,
				  const std::string & vShaderFile,
				  const std::string & fShaderFile);

	ShaderProgram(const std::string & name,
				  const std::string & vShaderFile,
				  const std::string & fShaderFile,
				  const std::vector<Uniform> & uniforms,
				  const std::vector<std::string> & capabilities);

	~ShaderProgram();
	ShaderProgram(const ShaderProgram &);
	ShaderProgram & operator=(const ShaderProgram &);

	void loadShaders(const std::string & name,
					 const std::string & vShaderFile,
					 const std::string & fShaderFile);
	void initDefaultUniforms();
	template<class V> void send_uniform(const std::string uname, const V & value);

	std::string m_name;
	std::string m_vShaderName;
	std::string m_fShaderName;
	GLuint m_vShader, m_fShader, m_program;
	GLuint m_oldProgram; // active program when activate was called
	std::map<std::string, Uniform> m_uniforms;
	std::set<std::string> m_capabilities;
};
