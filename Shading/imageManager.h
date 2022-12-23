// -*-C++-*-

#pragma once

#include <map>
#include <string>
#include "mgriter.h"
#include "image.h"

class ImageManager {

public:
	static ImageManager *instance();

	Image *create(const std::string &fname, bool flip = true);
	Image *find(const std::string &fname);

	void print() const;

	// iterate over elements
	typedef mgrIter<Image *> iterator;
	iterator begin();
	iterator end();

private:
	ImageManager();
	~ImageManager();
	ImageManager(const ImageManager &);
	ImageManager & operator =(const ImageManager &);

	std::map<std::string, Image *> m_hash;

};
