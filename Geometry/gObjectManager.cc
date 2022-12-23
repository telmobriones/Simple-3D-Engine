#include "gObjectManager.h"
#include "tools.h"

using std::map;
using std::string;

GObjectManager * GObjectManager::instance() {
	static GObjectManager mgr;
	return &mgr;
}

GObjectManager::GObjectManager() {}

GObjectManager::~GObjectManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

GObject *GObjectManager::create(const std::string & name) {
	auto it = m_hash.find(name);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate GObject %s\n", name.c_str());
		return it->second;
	}
	GObject * newgobj = new GObject(name);
	it = m_hash.insert(make_pair(name, newgobj)).first;
	return it->second;
}

GObject *GObjectManager::createFromWavefront(const std::string &dir,
											 const std::string &fName) {
	string key = getFilename(dir, fName);
	auto it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate GObject %s\n", key.c_str());
		return it->second;
	}
	GObject *newgobj = GObject::createFromWavefront(dir, fName);
	it = m_hash.insert(make_pair(key, newgobj)).first;
	return it->second;
}


GObject *GObjectManager::find(const std::string & key) const {
	auto it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

GObject *GObjectManager::find(const std::string & dir,
							  const std::string & fName) const {
	string key = getFilename(dir, fName);
	return find(key);
}

void GObjectManager::print() const {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		printf ("Gobj: %s\n", it->first.c_str());
}

GObjectManager::iterator GObjectManager::begin() {
	return GObjectManager::iterator(m_hash.begin());
}

GObjectManager::iterator GObjectManager::end() {
	return GObjectManager::iterator(m_hash.end());
}
