#include <cstdio>
#include "tools.h"
#include "lightManager.h"

using std::vector;
using std::string;

LightManager * LightManager::instance() {
	static LightManager mgr;
	return &mgr;
}

LightManager::LightManager() {}

LightManager::~LightManager() {
	for(auto it = m_v.begin(), end = m_v.end();
		it != end; ++it)
		delete *it;
}

Light *LightManager::create(const std::string &name,
							Light::type_t t) {
	Light *l = this->find(name);
	if (l) {
		fprintf(stderr, "[W] duplicate light %s\n", name.c_str());
		return l;
	}
	Light *newlght = new Light(name, t);
	m_v.push_back(newlght);
	return newlght;
}


Light *LightManager::find(const std::string & name) const {
	for (auto it = m_v.begin(), end = m_v.end();
		 it != end; ++it)
		if ((*it)->getNameString() == name) return *it;
	return 0;
}

void LightManager::switchOn() {

	for(auto it = m_v.begin(), end = m_v.end();
		it != end; ++it)
		(*it)->switchLight(true);
}

void LightManager::switchOff() {

	for(auto it = m_v.begin(), end = m_v.end();
		it != end; ++it)
		(*it)->switchLight(false);
}

void LightManager::print() const {
	for(auto it = m_v.begin(), end = m_v.end();
		it != end; ++it)
		(*it)->print();
}

LightManager::iterator LightManager::begin() { return LightManager::iterator(m_v.begin()); }
LightManager::iterator LightManager::end() { return LightManager::iterator(m_v.end()); }
