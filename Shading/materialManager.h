// -*-C++-*-

#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "material.h"

class MaterialManager {

public:
	static MaterialManager * instance();

	/**
	 * Register a new material
	 *
	 * If material name is new, create new material and register it.
	 * If material already exists, return it
	 *
	 * @param name: The name of the material
	 */

	Material *create(const std::string & name);

	/**
	 * Get a registered material
	 *
	 * @param name: The name of the material
	 * @return the material or 0 if not found
	 */

	Material *find(const std::string & name) const;

	/**
	 * Get the default material
	 *
	 * @return the default material
	 */

	Material *getDefault() const;

	void print() const;

	// iterate over all materials
	typedef mgrIter<Material *> iterator;
	iterator begin();
	iterator end();

private:
	MaterialManager();
	~MaterialManager();
	MaterialManager(const MaterialManager &);
	MaterialManager & operator=(const MaterialManager &);

	Material *m_mdefault;
	std::map<std::string, Material *> m_hash;

};
