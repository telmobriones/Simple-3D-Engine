// -*-C++-*-

#pragma once

#include <map>
#include <string>
#include "mgriter.h"
#include "shader.h"

class ShaderManager {

public:
	static ShaderManager *instance();

	/**
	 * Register a new shader
	 *
	 * If name is new, create new shader and register it.
	 * If shader already exists, return it
	 *
	 */
	ShaderProgram *create(const std::string & name,
						  const std::string & vShaderFile,
						  const std::string & fShaderFile);

	ShaderProgram *create_from_json(Json::Value & json);
	/**
	 * Get a registered shader
	 *
	 * Return the shader or 0 if not found
	 */
	ShaderProgram *find(const std::string & name) const;

	void print() const;

	// iterate over elements
	typedef mgrIter<ShaderProgram *> iterator;
	iterator begin();
	iterator end();

private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager &);
	ShaderManager & operator=(const ShaderManager &);

	std::map<std::string, ShaderProgram *> m_hash;

};
