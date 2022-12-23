#include <cstdio>
#include "shaderManager.h"

using std::string;
using std::map;

//////////////////////////////////////////////////7
// node manager

ShaderManager * ShaderManager::instance() {
	static ShaderManager mgr;
	return &mgr;
}

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

ShaderProgram *ShaderManager::create(const std::string &key,
									 const std::string &vShaderFile,
									 const std::string &fShaderFile) {
	auto it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate shader %s\n", key.c_str());
		return it->second;
	}
	ShaderProgram * newProgram = new ShaderProgram(key, vShaderFile, fShaderFile);
	if (!newProgram) {
		fprintf(stderr, "[E] Can't register shader \'%s\'. See line above.\n", key.c_str());
		exit(1);
	}
	it = m_hash.insert(make_pair(key, newProgram)).first;
	return it->second;
}

ShaderProgram *ShaderManager::create_from_json(Json::Value &json) {
	ShaderProgram *newProgram = ShaderProgram::from_json(json);
	auto it = m_hash.find(newProgram->m_name);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate shader %s\n", newProgram->m_name.c_str());
		return it->second;
	}
	it = m_hash.insert(make_pair(newProgram->m_name, newProgram)).first;
	return it->second;
}

ShaderProgram *ShaderManager::find(const std::string & key) const {
	auto it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

ShaderManager::iterator ShaderManager::begin() { return ShaderManager::iterator(m_hash.begin()); }
ShaderManager::iterator ShaderManager::end() { return ShaderManager::iterator(m_hash.end()); }

void ShaderManager::print() const {
	for (auto it = m_hash.begin(); it != m_hash.end(); ++it)
		it->second->print();
}
