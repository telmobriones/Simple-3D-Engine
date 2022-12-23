#include <cstdio>
#include "imageManager.h"

using std::map;
using std::string;

ImageManager *ImageManager::instance() {
	static ImageManager mgr;
	return &mgr;
}

ImageManager::ImageManager() {}

ImageManager::~ImageManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Image *ImageManager::create(const std::string &fName, bool flip) {
	auto it = m_hash.find(fName);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate image %s\n", fName.c_str());
		return it->second;
	}
	Image * newtex = new Image(fName, flip);
	it = m_hash.insert(make_pair(fName, newtex)).first;
	return it->second;
}

Image *ImageManager::find(const std::string &fName) {
	auto it = m_hash.find(fName);
	if (it == m_hash.end()) return 0;
	return it->second;
}


void ImageManager::print() const {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}
