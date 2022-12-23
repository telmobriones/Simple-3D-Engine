// -*-C++-*-
#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "gObject.h"

class GObjectManager {

public:
	static GObjectManager * instance();

	GObject *create(const std::string & name);

	/**
	 * Register a new geometric object, reading a Wavefront file
	 *
	 * If name is new, create new gObject and register it.
	 * If gObject already exists, return it
	 *
	 */
	GObject *createFromWavefront(const std::string & dir,
								 const std::string & name);

	/**
	 * Get a registered gObject
	 *
	 * Return the gObject or 0 if not found
	 */
	GObject *find(const std::string & dir,
				  const std::string & fName) const;
	GObject *find(const std::string & name) const;

	void print() const;

	// iterate over all gObjects
	typedef mgrIter<GObject *> iterator;
	iterator begin();
	iterator end();

private:
	GObjectManager();
	~GObjectManager();
	GObjectManager(const GObjectManager &);
	GObjectManager & operator=(const GObjectManager &);

	std::map<std::string, GObject *> m_hash;

};
