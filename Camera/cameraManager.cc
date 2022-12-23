#include <cstdio>
#include <string>
#include "cameraManager.h"

using std::map;
using std::string;

CameraManager * CameraManager::instance() {
	static CameraManager mgr;
	return &mgr;
}

CameraManager::CameraManager() {}

CameraManager::~CameraManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

OrthographicCamera *CameraManager::createOrthographic(const std::string &key) {
	auto it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate camera %s\n", key.c_str());
		return 0;
	}
	OrthographicCamera *newcam = new OrthographicCamera();
	newcam->setName(key);
	it = m_hash.insert(make_pair(key, newcam)).first;
	return newcam;
}

PerspectiveCamera *CameraManager::createPerspective(const std::string &key) {
	auto it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate camera %s\n", key.c_str());
		return 0;
	}
	PerspectiveCamera *newcam = new PerspectiveCamera();
	newcam->setName(key);
	it = m_hash.insert(make_pair(key, newcam)).first;
	return newcam;
}

Camera *CameraManager::find(const std::string & key) {
	auto it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

void CameraManager::print() const {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}
