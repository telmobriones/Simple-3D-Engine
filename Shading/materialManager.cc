#include <cstdio>
#include <cstdlib>
#include "tools.h"
#include "materialManager.h"

using std::map;
using std::string;

MaterialManager * MaterialManager::instance() {
	static MaterialManager mgr;
	return &mgr;
}

MaterialManager::MaterialManager() {
	m_mdefault = create(string("_MG_Default_"));
}

MaterialManager::~MaterialManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Material *MaterialManager::create(const std::string & name) {
	auto it = m_hash.find(name);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate material %s\n", name.c_str());
		return it->second;
	}
	Material *newmat = new Material(name);
	it = m_hash.insert(make_pair(name, newmat)).first;
	return it->second;
}

Material *MaterialManager::find(const std::string & name) const {
	auto it = m_hash.find(name);
	if (it == m_hash.end()) return 0;
	return it->second;
}

Material *MaterialManager::getDefault() const { return m_mdefault; }

// Debug

void MaterialManager::print() const {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}
